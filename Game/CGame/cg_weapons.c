/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
// cg_weapons.c -- events and effects dealing with weapons
#include "cg_local.h"
/*========================================================================================
VIEW WEAPON
========================================================================================*/
static void CG_SetWeaponSprite(refEntity_t* entity,float scale,qhandle_t shader){
	entity->reType = RT_SPRITE;
	entity->radius = 4 * scale;
	entity->rotation = 0;
	entity->customShader = shader;
}
static void CG_SetWeaponModel(refEntity_t* entity,float scale,qhandle_t model,qhandle_t skin){
	vec3_t minimum;
	vec3_t maximum;
	entity->reType = RT_MODEL;
	entity->hModel = model;
	entity->customSkin = skin;
	entity->nonNormalizedAxes = qtrue;
	VectorScale(entity->axis[0],scale,entity->axis[0]);
	VectorScale(entity->axis[1],scale,entity->axis[1]);
	VectorScale(entity->axis[2],scale,entity->axis[2]);
	if(!cg_drawBBox.value){return;}
	trap_R_ModelBounds(entity->hModel,minimum,maximum,entity->frame);
	VectorScale(minimum,scale,minimum);
	VectorScale(maximum,scale,maximum);
	CG_DrawBoundingBox(entity->origin,minimum,maximum);
}
static void CG_AddPlayerWeaponCharge(refEntity_t* parent,cg_userWeapon_t* weaponGraphics,refEntity_t* charge,float progress){
	float scale = weaponGraphics->chargeSizeRange[0];
	float lightScale = weaponGraphics->chargeDlightRadiusRange[0];
	VectorCopy(parent->lightingOrigin,charge->lightingOrigin);
	charge->shadowPlane = parent->shadowPlane;
	charge->renderfx = parent->renderfx;
	if(weaponGraphics->chargeGrowth){
		if(weaponGraphics->chargePercentRange[1] <= progress){
			scale = weaponGraphics->chargeSizeRange[1];
			lightScale = weaponGraphics->chargeDlightRadiusRange[1];
		}
		else{
			float percentRange = weaponGraphics->chargePercentRange[1] - weaponGraphics->chargePercentRange[0];
			float sizeRange = weaponGraphics->chargeSizeRange[1] - weaponGraphics->chargeSizeRange[0];
			float percent = progress - weaponGraphics->chargePercentRange[0];
			float size = percent / percentRange * sizeRange;
			scale = size + weaponGraphics->chargeSizeRange[0];
			sizeRange = weaponGraphics->chargeDlightRadiusRange[1] - weaponGraphics->chargeDlightRadiusRange[0];
			size = percent / percentRange * sizeRange;
			lightScale = size + weaponGraphics->chargeDlightRadiusRange[0];
		}
	}
	if(!weaponGraphics->chargeModel || !weaponGraphics->chargeSkin){
		CG_SetWeaponSprite(charge,scale,weaponGraphics->chargeShader);
	}
	else{
		CG_SetWeaponModel(charge,scale,weaponGraphics->chargeModel,weaponGraphics->chargeSkin);
	}
	trap_R_AddRefEntityToScene(charge);
	if(lightScale){
		trap_R_AddLightToScene(charge->origin,100 * lightScale,weaponGraphics->chargeDlightColor);
	}
}
static void CG_AddPlayerWeaponChargeVoices(centity_t* player,cg_userWeapon_t* weaponGraphics,float currentProgress,float previousProgress){
	int index = MAX_CHARGE_VOICES-1;
	for(;index>=0;--index){
		chargeVoice_t* voice = &weaponGraphics->chargeVoice[index];
		if(!voice->voice){continue;}
		if(previousProgress < voice->startPct && voice->startPct <= currentProgress){
			trap_S_StartSound(NULL,player->currentState.number,CHAN_VOICE,voice->voice);
			break;
		}
	}
}
static void CG_AddPlayerWeaponFlash(refEntity_t* parent,cg_userWeapon_t* weaponGraphics,refEntity_t* flash,int totalPower,int currentPower){
	float scale;
	float radiusScale;
	VectorCopy(parent->lightingOrigin,flash->lightingOrigin);
	flash->shadowPlane = parent->shadowPlane;
	flash->renderfx = parent->renderfx;
	if(currentPower >= totalPower * 2){currentPower = totalPower * 2;}
	radiusScale = Com_Clamp(0,1,(float)currentPower / (float)totalPower);
	scale = weaponGraphics->flashSize * radiusScale;
	if(!weaponGraphics->flashModel || !weaponGraphics->flashSkin){
		CG_SetWeaponSprite(flash,scale,weaponGraphics->flashShader);
	}
	else{
		CG_SetWeaponModel(flash,scale,weaponGraphics->flashModel,weaponGraphics->flashSkin);
	}
	trap_R_AddRefEntityToScene(flash);
	if(weaponGraphics->flashDlightRadius){
		trap_R_AddLightToScene(flash->origin,100 * weaponGraphics->flashDlightRadius,weaponGraphics->flashDlightColor);
	}
}
void CG_AddPlayerWeapon(refEntity_t* parent,centity_t* client){
	entityState_t* ent = &client->currentState;
	weaponstate_t weaponState = ent->weaponstate;
	cg_userWeapon_t* weaponGraphics;
	orientation_t orient;
	refEntity_t refEnt;
	qboolean wasPVS = CG_FrameHist_WasInPVS(ent->number);
	qboolean newNR = CG_FrameHist_IsWeaponNr(ent->number) != CG_FrameHist_WasWeaponNr(ent->number);
	qboolean newState = CG_FrameHist_IsWeaponState(ent->number) != CG_FrameHist_WasWeaponState(ent->number);
	if(weaponState == WEAPON_CHARGING || weaponState == WEAPON_ALTCHARGING){
		int newLerp;
		float backLerp;
		float lerp;
		if(weaponState == WEAPON_CHARGING){
			weaponGraphics = CG_FindUserWeaponGraphics(ent->clientNum,ent->weapon);
			newLerp = ent->charge1.chBase;
			backLerp = client->lerpPrim;
			client->lerpPrim = newLerp < backLerp ? newLerp : (client->lerpPrim + newLerp) / 2.0f;
			lerp = client->lerpPrim;
			client->lerpSec = 0;
		}
		else{
			weaponGraphics = CG_FindUserWeaponGraphics(ent->clientNum, ent->weapon + ALTWEAPON_OFFSET );
			newLerp = ent->charge2.chBase;
			backLerp = client->lerpSec;
			client->lerpSec = newLerp < backLerp ? newLerp : (client->lerpSec + newLerp) / 2.0f;
			lerp = client->lerpSec;
			client->lerpPrim = 0;
		}
		if(lerp > weaponGraphics->chargePercentRange[0]){
			if(CG_GetTagOrientationFromPlayerEntity(client,weaponGraphics->chargeTag[0],&orient)){
				memset(&refEnt,0,sizeof(refEnt));
				if(VectorLength(weaponGraphics->chargeSpin)){
					vec3_t tempAngles;
					vec3_t lerpAxis[3];
					vec3_t tempAxis[3];
					VectorCopy(orient.origin,refEnt.origin);
					VectorSet(tempAngles,cg.time / 4.0f,cg.time / 4.0f,cg.time / 4.0f);
					VectorPieceWiseMultiply(tempAngles,weaponGraphics->chargeSpin,tempAngles);
					AnglesToAxis(tempAngles,lerpAxis);
					AxisClear(refEnt.axis);
					MatrixMultiply(refEnt.axis,lerpAxis,tempAxis);
					MatrixMultiply(tempAxis,orient.axis,refEnt.axis);
				}
				else{
					VectorCopy(orient.origin,refEnt.origin);
					AxisCopy(orient.axis,refEnt.axis);
				}
				CG_AddPlayerWeaponCharge(parent,weaponGraphics,&refEnt,lerp);
			}
		}
		if(weaponGraphics->chargeLoopSound){
			trap_S_AddLoopingSound(ent->number,client->lerpOrigin,vec3_origin,weaponGraphics->chargeLoopSound);
		}
		CG_AddPlayerWeaponChargeVoices(client,weaponGraphics,lerp,backLerp);
		if(weaponGraphics->chargeParticleSystem[0]){
			// If the entity wasn't previously in the PVS, if the weapon nr switched, or if the weaponstate switched
			// we need to start a new system
			if(!wasPVS || newNR || newState){
				PSys_SpawnCachedSystem(weaponGraphics->chargeParticleSystem,client->lerpOrigin,NULL,client,weaponGraphics->chargeTag[0],qfalse,qtrue);
			}
		}
	}
	else if(weaponState == WEAPON_GUIDING || weaponState == WEAPON_ALTGUIDING || weaponState == WEAPON_FIRING || weaponState == WEAPON_ALTFIRING){
		int index = weaponState == WEAPON_GUIDING || weaponState == WEAPON_FIRING ? ent->weapon : ent->weapon + ALTWEAPON_OFFSET;
		weaponGraphics = CG_FindUserWeaponGraphics(ent->clientNum,index);
		if(CG_GetTagOrientationFromPlayerEntity(client,weaponGraphics->chargeTag[0],&orient)){
			memset(&refEnt,0,sizeof(refEnt));
			VectorCopy(orient.origin,refEnt.origin);
			AxisCopy(orient.axis,refEnt.axis);
			CG_AddPlayerWeaponFlash(parent,weaponGraphics,&refEnt,ent->attackPowerTotal,ent->attackPowerCurrent);
		}
		if(weaponGraphics->firingSound){
			trap_S_AddLoopingSound(ent->number,client->lerpOrigin,vec3_origin,weaponGraphics->firingSound);
		}
		if(weaponGraphics->firingParticleSystem[0]){
			// If the entity wasn't previously in the PVS, if the weapon nr switched, or if the weaponstate switched
			// we need to start a new system
			if(!wasPVS || newNR || newState){
				PSys_SpawnCachedSystem(weaponGraphics->firingParticleSystem,client->lerpOrigin,NULL,client,weaponGraphics->chargeTag[0],qfalse,qtrue);
			}
		}
	}
}
/*
==============================================================================
WEAPON SELECTION
==============================================================================
*/
static void CG_DrawWeaponSelectHorCenterBar(void){
	int i;
	int statBits = cg.snap->ps.stats[stSkills];
	float* color = CG_FadeColor(cg.weaponSelectTime,WEAPON_SELECT_TIME,200);
	vec2_t position = {70,418};
	cg_userWeapon_t* weaponInfo;
	cg_userWeapon_t* alt_weaponInfo;
	if(!color){
		cg.drawWeaponBar = 0;
		return;
	}
	trap_R_SetColor(color);
	for(i=1;i<(MAX_PLAYERWEAPONS * 2);++i){
		qboolean exists = statBits & (1 << i);
		qboolean usable = cg.snap->ps.powerups[PW_SKILLS] & (1 << (i-1));
		if(!exists || !usable){continue;}
		weaponInfo = CG_FindUserWeaponGraphics(cg.snap->ps.clientNum,i);
		CG_DrawPic(qfalse,position[0],position[1],24,24,weaponInfo->weaponIcon);
		if(i == cg.weaponSelect){CG_DrawPic(qfalse,position[0] - 4,position[1] - 4, 32,32,cgs.media.selectShader);}
		position[0] += 30;
	}
	weaponInfo = CG_FindUserWeaponGraphics(cg.snap->ps.clientNum,cg.weaponSelect);
	alt_weaponInfo = CG_FindUserWeaponGraphics(cg.snap->ps.clientNum,ALTWEAPON_OFFSET + cg.weaponSelect);
	if(weaponInfo->weaponName[0]){
		// Need a little more leeway
		char name[MAX_QPATH * 2 + 4];
		Com_sprintf(name,sizeof(name),"%s",weaponInfo->weaponName);
		if(alt_weaponInfo->weaponName[0]){
			strcat(name,va(" / %s",alt_weaponInfo->weaponName));
		}
		CG_DrawSmallStringColor(6,position[1] - 30,name,color);
	}
	trap_R_SetColor(NULL);
}
void CG_DrawWeaponSelect(void){
	cg.itemPickupTime = 0;
	if(cg.drawWeaponBar == 1){CG_DrawWeaponSelectHorCenterBar();}
}
void CG_NextWeapon_f(void){
	if(!cg.snap || cg.snap->ps.bitFlags & usingMelee){return;}
	if(cg.snap->ps.pm_flags & PMF_FOLLOW){return;}
	cg.weaponSelectionMode = 2;
	cg.weaponSelectTime = cg.time;
}
void CG_PrevWeapon_f(void){
	if(!cg.snap || cg.snap->ps.bitFlags & usingMelee){return;}
	if(cg.snap->ps.pm_flags & PMF_FOLLOW){return;}
	cg.weaponSelectionMode = 1;
	cg.weaponSelectTime = cg.time;
}
void CG_Weapon_f(void){
	int num = atoi(CG_Argv(1));
	if(!cg.snap){return;}
	if(cg.snap->ps.pm_flags & PMF_FOLLOW){return;}
	if(num < 1 || num > 15){return;}
	cg.weaponSelectTime = cg.time;
	cg.weaponDesired = num;
	cg.weaponSelectionMode = 3;
}
/*
==========================================================
WEAPON EVENTS
==========================================================
*/
/*
================
CG_FireWeapon
Caused by an EV_FIRE_WEAPON event
================
*/
void CG_FireWeapon(centity_t* client,qboolean altFire){
	cg_userWeapon_t* weaponGraphics;
	entityState_t* ent = &client->currentState;
	int maxSounds = 0;
	int maxVoices = 0;
	int weapNr;
	if(ent->weapon == WP_NONE){return;}
	weapNr = altFire ? ent->weapon + ALTWEAPON_OFFSET : ent->weapon;
	weaponGraphics = CG_FindUserWeaponGraphics(ent->clientNum, weapNr );
	// mark the entity as muzzle flashing, so when it is added it will
	// append the flash to the weapon model.
	client->muzzleFlashTime = cg.time;
	if(weaponGraphics->flashParticleSystem[0]){
		PSys_SpawnCachedSystem(weaponGraphics->flashParticleSystem,client->lerpOrigin,NULL,client,weaponGraphics->chargeTag[0],qfalse,qfalse);
	}
	for(;maxSounds<MAX_FLASH_SOUNDS;++maxSounds){
		if(!weaponGraphics->flashSound[maxSounds]){break;}
	}
	if(maxSounds > 0){
		trap_S_StartSound(NULL,ent->number,CHAN_WEAPON,weaponGraphics->flashSound[rand()%maxSounds]);
	}
	for(;maxVoices<MAX_FLASH_VOICES;++maxVoices){
		if(!weaponGraphics->voiceSound[maxVoices]){break;}
	}
	if(maxVoices > 0){
		trap_S_StartSound(NULL,ent->number,CHAN_WEAPON,weaponGraphics->voiceSound[rand()%maxVoices]);
	}
}
/*
======================
CG_UserMissileHitWall
======================
Caused by an EV_MISSILE_MISS event
*/
void CG_UserMissileHitWall(int weapon,int clientNum,int powerups,int number,vec3_t origin,vec3_t dir,qboolean inAir){
	cg_userWeapon_t* weaponGraphics = CG_FindUserWeaponGraphics(clientNum,weapon);
	vec3_t end;
	trace_t tr;
	int maxSounds = 0;
	for(;maxSounds<MAX_EXPLOSION_SOUNDS;++maxSounds){
		if(!weaponGraphics->explosionSound[maxSounds]){break;}
	}
	if(maxSounds>0){
		maxSounds = rand() % maxSounds;
		if(weaponGraphics->explosionSound[maxSounds]){
			trap_S_StartSound(origin,ENTITYNUM_WORLD,CHAN_AUTO,weaponGraphics->explosionSound[maxSounds]);
		}
	}
	CG_MakeUserExplosion(origin,dir,weaponGraphics,powerups,number);
	if(!inAir){
		vec3_t tempAxis[3];
		VectorNormalize2(dir,tempAxis[0]);
		MakeNormalVectors(tempAxis[0],tempAxis[1],tempAxis[2]);
		VectorCopy(origin,end);
		end[2] -= 64;
		CG_Trace(&tr,origin,NULL,NULL,end,-1,MASK_PLAYERSOLID);
		if(!weaponGraphics->noRockDebris){
			char particles[MAX_QPATH] = "ExtraLarge";
			if(weaponGraphics->explosionSize <= 10){Q_strncpyz(particles,"Small",sizeof("Small")+1);}
			else if(weaponGraphics->explosionSize <= 25){Q_strncpyz(particles,"Normal",sizeof("Normal")+1);}
			else if(weaponGraphics->explosionSize <= 50){Q_strncpyz(particles,"Large",sizeof("Large")+1);}
			strcat(particles,"ExplosionDebris");
			if(cg_particlesQuality.value == 1){strcat(particles,"Low");}
			PSys_SpawnCachedSystem(particles,origin,tempAxis,NULL,NULL,qfalse,qfalse);
		}
		if(weaponGraphics->markSize && weaponGraphics->markShader){
			CG_ImpactMark(weaponGraphics->markShader,origin,dir,random() * 360,1,1,1,1,qfalse,60,qfalse);
		}
	}
/* NOTE: Find another way of doing this in the new system...
	if ( weaponGraphics->missileTrailFunc == CG_TrailFunc_StraightBeam ||
		 weaponGraphics->missileTrailFunc == CG_TrailFunc_SpiralBeam ) {
		CG_CreateStraightBeamFade( cgs.clientinfo[ clientNum ].weaponTagPos0, origin, weaponGraphics);
	}
*/
}
void CG_UserMissileHitPlayer(int weapon,int clientNum,int powerups,int number,vec3_t origin,vec3_t dir,int entityNum){
	CG_Bleed(origin,entityNum);
	CG_UserMissileHitWall(weapon,clientNum,powerups,number,origin,dir,qtrue);
}
