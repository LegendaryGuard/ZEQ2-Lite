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
// cg_effects.c -- these functions generate localentities, usually as a result
// of event processing
#include "cg_local.h"
void CG_BubbleTrail(vec3_t start,vec3_t end,float spacing){
	vec3_t move;
	vec3_t vec;
	float len;
	// advance a random amount first
	int i = rand() % (int)spacing;
	VectorCopy(start,move);
	VectorSubtract(end,start,vec);
	len = VectorNormalize(vec);
	VectorMA(move,i,vec,move);
	VectorScale(vec,spacing,vec);
	for(;i<len;i+=spacing){
		localEntity_t* le = CG_AllocLocalEntity();
		refEntity_t* re = &le->refEntity;
		le->leFlags = LEF_PUFF_DONT_SCALE;
		le->leType = LE_MOVE_SCALE_FADE;
		le->startTime = cg.time;
		le->endTime = cg.time + 1000 + random() * 250;
		le->lifeRate = 1.0f / (le->endTime - le->startTime);
		re->shaderTime = cg.time / 1000.0f;
		re->reType = RT_SPRITE;
		re->rotation = 0;
		re->radius = 3;
		re->customShader = cgs.media.waterBubbleMediumShader;
		VectorSet(re->shaderRGBA,255,255,255);
		re->shaderRGBA[3] = 255;
		le->color[3] = 1.0f;
		le->pos.trType = TR_LINEAR;
		le->pos.trTime = cg.time;
		VectorCopy(move,le->pos.trBase);
		le->pos.trDelta[0] = crandom()*5;
		le->pos.trDelta[1] = crandom()*5;
		le->pos.trDelta[2] = crandom()*5 + 6;
		VectorAdd(move,vec,move);
	}
}
void CG_WaterBubble(const vec3_t p,float radius,float alpha,float duration,int fadeInTime,qhandle_t hShader){
	static int seed = 0x92;
	localEntity_t* le;
	refEntity_t* re;
	int contents;
	if(cgs.clientPaused){return;}
	le = CG_AllocLocalEntity();
	le->leFlags = LEF_PUFF_DONT_SCALE;
	le->radius = radius;
	re = &le->refEntity;
	re->rotation = Q_random(&seed) * 360;
	re->radius = radius;
	re->shaderTime = cg.time / 1000.0f;
	le->leType = LE_MOVE_SCALE_FADE;
	le->startTime = cg.time;
	le->fadeInTime = fadeInTime;
	le->endTime = cg.time + duration;
	le->lifeRate = fadeInTime > cg.time ? 1.0f / (le->endTime - le->fadeInTime) : 1.0f / (le->endTime - le->startTime);
	VectorSet(le->color,1.0f,1.0f,1.0f);
	le->color[3] = alpha;
	le->pos.trType = TR_LINEAR;
	le->pos.trTime = cg.time;
	VectorSet(le->pos.trDelta,0,0,8);
	VectorCopy(p,le->pos.trBase);
	le->pos.trDelta[0] = crandom()*5;
	le->pos.trDelta[1] = crandom()*5;
	le->pos.trDelta[2] = crandom()*5 + 6;
	VectorCopy(p,re->origin);
	re->customShader = hShader;
	// rage pro can't alpha fade, so use a different shader
	re->shaderRGBA[0] = le->color[0] * 255;
	re->shaderRGBA[1] = le->color[1] * 255;
	re->shaderRGBA[2] = le->color[2] * 255;
	re->shaderRGBA[3] = 255;
	re->reType = RT_SPRITE;
	re->radius = le->radius;
	contents = trap_CM_PointContents(re->origin,0);
	if(!(contents & CONTENTS_WATER)){le->fadeInTime = 0;}
}
void CG_DirtPush(vec3_t org,vec3_t dir,int size){
	localEntity_t* le;
	refEntity_t* re;
	if(cgs.clientPaused){return;}
	le = CG_AllocLocalEntity();
	le->leFlags = 0;
	le->leType = LE_ZEQSPLASH;
	le->startTime = cg.time;
	le->endTime = cg.time + 500;
	le->lifeRate = 1.0 / (le->endTime - le->startTime);
	VectorSet(le->color,1.0f,1.0f,1.0f);
	le->color[3] = 1.0f;
	re = &le->refEntity;
	re->reType = RT_MODEL;
	re->shaderRGBA[0] = le->color[0] * 255;
	re->shaderRGBA[1] = le->color[1] * 255;
	re->shaderRGBA[2] = le->color[2] * 255;
	re->shaderRGBA[3] = 255;
	re->customSkin = cgs.media.dirtPushSkin;
	re->hModel = cgs.media.dirtPushModel;
	// bias the time so all shader effects start correctly
	re->shaderTime = le->startTime / 1000.0f;
	AxisClear(re->axis);
	re->nonNormalizedAxes = qtrue;
	VectorNormalize(re->axis[0]);
	VectorNormalize(re->axis[1]);
	VectorNormalize(re->axis[2]);
	VectorScale(re->axis[0],size,re->axis[0]);
	VectorScale(re->axis[1],size,re->axis[1]);
	VectorScale(re->axis[2],size,re->axis[2]);
	VectorCopy(org,re->origin);
}
void CG_WaterRipple(vec3_t org,int size,qboolean single){
	sfxHandle_t* sounds = cgs.media.smallSplash;
	int soundIndex = random() * MAX_MEDIA_SOUNDS;
	localEntity_t* le;
	refEntity_t* re;
	if(cgs.clientPaused){return;}
	le = CG_AllocLocalEntity();
	le->leFlags = 0;
	le->leType = LE_ZEQSPLASH;
	le->startTime = cg.time;
	le->endTime = cg.time + 200 * size;
	le->lifeRate = 1.0 / (le->endTime - le->startTime);
	VectorSet(le->color,1.0f,1.0f,1.0f);
	le->color[3] = 1.0f;
	re = &le->refEntity;
	re->reType = RT_MODEL;
	re->shaderRGBA[0] = le->color[0] * 255;
	re->shaderRGBA[1] = le->color[1] * 255;
	re->shaderRGBA[2] = le->color[2] * 255;
	re->shaderRGBA[3] = 255;
	re->customSkin = single ? cgs.media.waterRippleSingleSkin : cgs.media.waterRippleSkin;
	re->hModel = single ? cgs.media.waterRippleSingleModel : cgs.media.waterRippleModel;
	if(!single){
		re->customSkin = cgs.media.waterRippleSkin;
		re->hModel = cgs.media.waterRippleModel;
		if(size > 10){sounds = cgs.media.mediumSplash;}
		else if(size > 25){sounds = cgs.media.largeSplash;}
		else if(size > 50){sounds = cgs.media.extraLargeSplash;}
	}
	while(!sounds[soundIndex]){
		soundIndex = random() * MAX_MEDIA_SOUNDS;
	}
	trap_S_StartSound(org,ENTITYNUM_NONE,CHAN_AUTO,sounds[soundIndex]);
	// bias the time so all shader effects start correctly
	re->shaderTime = le->startTime / 1000.0f;
	AxisClear(re->axis);
	re->nonNormalizedAxes = qtrue;
	VectorNormalize(re->axis[0]);
	VectorNormalize(re->axis[1]);
	VectorNormalize(re->axis[2]);
	VectorScale(re->axis[0],size,re->axis[0]);
	VectorScale(re->axis[1],size,re->axis[1]);
	VectorScale(re->axis[2],size,re->axis[2]);
	VectorCopy(org,re->origin);
}
void CG_WaterSplash(vec3_t org,int size){
	localEntity_t* le;
	refEntity_t* re;
	float ang;
	vec3_t oldAxis[3];
	if(cgs.clientPaused){return;}
	le = CG_AllocLocalEntity();
	le->leFlags = 0;
	le->leType = LE_ZEQSPLASH;
	le->startTime = cg.time;
	le->endTime = cg.time + 100 * size;
	le->lifeRate = 1.0 / (le->endTime - le->startTime);
	VectorSet(le->color,1.0f,1.0f,1.0f);
	le->color[3] = 1.0f;
	re = &le->refEntity;
	re->reType = RT_MODEL;
	re->shaderRGBA[0] = le->color[0] * 255;
	re->shaderRGBA[1] = le->color[1] * 255;
	re->shaderRGBA[2] = le->color[2] * 255;
	re->shaderRGBA[3] = 255;
	re->customSkin = cgs.media.waterSplashSkin;
	re->hModel = cgs.media.waterSplashModel;
	// bias the time so all shader effects start correctly
	re->shaderTime = le->startTime / 1000.0f;
	AxisClear(re->axis);
	// set axis with random rotate
	ang = rand() % 360;
	// avoid horizontal angles
	if((ang >= 70 && ang <= 110) || (ang >= 250 && ang <= 290)){
		ang = rand() % 69;
	}
	VectorCopy(re->axis[0],oldAxis[0]);
	VectorCopy(re->axis[1],oldAxis[1]);
	RotateAroundDirection(re->axis,ang);
	VectorCopy(oldAxis[0],re->axis[0]);
	VectorCopy(oldAxis[1],re->axis[1]);
	re->nonNormalizedAxes = qtrue;
	VectorNormalize(re->axis[0]);
	VectorNormalize(re->axis[1]);
	VectorNormalize(re->axis[2]);
	VectorScale(re->axis[0],size,re->axis[0]);
	VectorScale(re->axis[1],size,re->axis[1]);
	VectorScale(re->axis[2],size,re->axis[2]);
	VectorCopy(org,re->origin);
}
void CG_LightningEffect(vec3_t org,clientInfo_t* ci,int tier){
	localEntity_t* le;
	refEntity_t* re;
	int randoms[] = {32,32,48,32,32,40};
	int soundIndex = 0;
	int index = 0;
	if(cgs.clientPaused){return;}
	if(random() * 60 < 59){return;}
	for(;index<sizeof(randoms)/sizeof(randoms[0]);++index){
		randoms[index] *= random();
	}
	le = CG_AllocLocalEntity();
	le->leFlags = 0;
	le->leType = LE_FADE_ALPHA;
	le->startTime = cg.time;
	le->endTime = cg.time + 250;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );
	le->radius = 16;
	VectorSet(le->color,1.0f,1.0f,1.0f);
	le->color[3] = 1.0f;
	re = &le->refEntity;
	re->reType = RT_SPRITE;
	re->radius = le->radius;
	VectorSet(re->shaderRGBA,255,255,255);
	re->shaderRGBA[3] = 255;
	re->customShader = ci->auraConfig[tier]->lightningShader;
	AxisClear(re->axis);
	VectorCopy(org,re->origin);
	VectorAdd(re->origin,randoms,re->origin);
	VectorSubtract(re->origin,&randoms[3],re->origin);
	while(!cgs.media.bigLightning[soundIndex]){
		soundIndex = random() * MAX_MEDIA_SOUNDS;
	}
	trap_S_StartSound(org,ENTITYNUM_NONE,CHAN_AUTO,cgs.media.bigLightning[soundIndex]);
}
void CG_BigLightningEffect(vec3_t org){
	localEntity_t* le;
	refEntity_t* re;
	int number;
	if(cgs.clientPaused){return;}
	number = random() * 200;
	if(number >= 1 && number <= 198){return;}
	le = CG_AllocLocalEntity();
	le->leFlags = 0;
	le->startTime = cg.time;
	le->leType = LE_FADE_NO;
	le->endTime = cg.time + 200;
	le->lifeRate = 1.0 / (le->endTime - le->startTime);
	VectorSet(le->color,1.0f,1.0f,1.0f);
	le->color[3] = 1.0f;
	le->radius = 128;
	le->light = 200;
	VectorSet(le->lightColor,1.0f,1.0f,1.0f);
	re = &le->refEntity;
	re->reType = RT_SPRITE;
	re->radius = le->radius;
	re->shaderRGBA[0] = le->color[0] * 255;
	re->shaderRGBA[1] = le->color[1] * 255;
	re->shaderRGBA[2] = le->color[2] * 255;
	re->shaderRGBA[3] = 255;
	AxisClear(re->axis);
	VectorCopy(org,re->origin);
	re->origin[2] += 16;
	re->customShader = number < 1 ? cgs.media.auraLightningSparks1 : cgs.media.auraLightningSparks2;
	while(!cgs.media.bigLightning[number]){
		number = random() * MAX_MEDIA_SOUNDS;
	}
	trap_S_StartSound(org,ENTITYNUM_NONE,CHAN_AUTO,cgs.media.bigLightning[number]);
}
void CG_SpeedMeleeEffect(vec3_t org,int tier){
	localEntity_t* le;
	refEntity_t* re;
	int randoms[] = {32,32,48,32,32,40};
	int index = 0;
	if(cgs.clientPaused){return;}
	if(random() * 60 < 51){return;}
	for(;index<sizeof(randoms)/sizeof(randoms[0]);++index){
		randoms[index] *= random();
	}
	le = CG_AllocLocalEntity();
	le->leFlags = 0;
	le->startTime = cg.time;
	le->leType = LE_SCALE_FADE_RGB;
	le->endTime = cg.time + 100;
	le->radius = 16 << tier;
	le->lifeRate = 1.0 / (le->endTime - le->startTime);
	VectorSet(le->color,1.0f,1.0f,1.0f);
	le->color[3] = 1.0f;
	re = &le->refEntity;
	re->reType = RT_SPRITE;
	re->radius = le->radius;
	re->shaderRGBA[0] = le->color[0] * 255;
	re->shaderRGBA[1] = le->color[1] * 255;
	re->shaderRGBA[2] = le->color[2] * 255;
	re->shaderRGBA[3] = 255;
	re->customShader = cgs.media.meleeSpeedEffectShader;
	AxisClear(re->axis);
	VectorCopy(org,re->origin);
	VectorAdd(re->origin,randoms,re->origin);
	VectorSubtract(re->origin,&randoms[3],re->origin);
}
void CG_PowerMeleeEffect(vec3_t org,int tier){
	localEntity_t* le;
	refEntity_t* re;
	int randoms[] = {32,32,48,32,32,40};
	int index = 0;
	if(cgs.clientPaused){return;}
	for(;index<sizeof(randoms)/sizeof(randoms[0]);++index){
		randoms[index] *= random();
	}
	le = CG_AllocLocalEntity();
	le->leFlags = 0;
	le->startTime = cg.time;
	le->leType = LE_SCALE_FADE;
	le->endTime = cg.time + 250;
	le->radius = 16 << tier;
	le->lifeRate = 1.0 / (le->endTime - le->startTime);
	VectorSet(le->color,1.0f,1.0f,1.0f);
	le->color[3] = 1.0f;
	re = &le->refEntity;
	re->reType = RT_SPRITE;
	re->radius = le->radius;
	re->shaderRGBA[0] = le->color[0] * 255;
	re->shaderRGBA[1] = le->color[1] * 255;
	re->shaderRGBA[2] = le->color[2] * 255;
	re->shaderRGBA[3] = 255;
	re->customShader = cgs.media.meleePowerEffectShader;
	AxisClear(re->axis);
	VectorCopy(org,re->origin);
	VectorAdd(re->origin,randoms,re->origin);
	VectorSubtract(re->origin,&randoms[3],re->origin);
}
void CG_PowerStruggleEffect(vec3_t org,int size){
	localEntity_t* le;
	localEntity_t* le2;
	refEntity_t* re;
	refEntity_t* re2;
	if(cgs.clientPaused){return;}
	le = CG_AllocLocalEntity();
	le->leFlags = 0;
	le->startTime = cg.time;
	le->leType = LE_SCALE_FADE_RGB;
	le->endTime = cg.time + 250;
	le->radius = size;
	le->lifeRate = 1.0 / (le->endTime - le->startTime);
	VectorSet(le->color,1.0f,1.0f,1.0f);
	le->color[3] = 1.0f;
	re = &le->refEntity;
	re->reType = RT_SPRITE;
	re->radius = le->radius;
	re->shaderRGBA[0] = le->color[0] * 255;
	re->shaderRGBA[1] = le->color[1] * 255;
	re->shaderRGBA[2] = le->color[2] * 255;
	re->shaderRGBA[3] = 255;
	re->customShader = cgs.media.powerStruggleRaysEffectShader;
	re->rotation = random() * 360;
	AxisClear(re->axis);
	VectorCopy(org,re->origin);
	le2 = CG_AllocLocalEntity();
	le2->leFlags = 0;
	le2->startTime = cg.time;
	le2->leType = LE_SCALE_FADE_RGB;
	le2->endTime = cg.time + 1000;
	le2->radius = size * 2;
	le2->lifeRate = 1.0 / (le2->endTime - le2->startTime);
	VectorSet(le->color,1.0f,1.0f,1.0f);
	le->color[3] = 1.0f;
	re2 = &le2->refEntity;
	re2->reType = RT_SPRITE;
	re2->radius = le2->radius;
	re->shaderRGBA[0] = le->color[0] * 255;
	re->shaderRGBA[1] = le->color[1] * 255;
	re->shaderRGBA[2] = le->color[2] * 255;
	re->shaderRGBA[3] = 255;
	re2->customShader = cgs.media.powerStruggleShockwaveEffectShader;
	re2->rotation = random() * 360;
	AxisClear(re2->axis);
	VectorCopy(org,re2->origin);
}
/*=================
CG_Bleed
This is the spurt of blood when a character gets hit
=================*/
void CG_Bleed( vec3_t origin, int entityNum ){}
void CG_MakeUserExplosion(vec3_t origin,vec3_t dir,cg_userWeapon_t* weaponGraphics,int powerups,int number){
	float angle;
	localEntity_t* expShell;
	localEntity_t* expShock;
	int offset;
	vec3_t tmpVec;
	vec3_t newOrigin;
	float explosionScale = 1;
	int attackChargeLvl;
	// The attacks's charge level was stored in this field. We hijacked it on the
	// server to be able to transmit the missile's own charge level.
	attackChargeLvl = powerups;
	// Obtain the scale the missile must have.
	if(weaponGraphics->chargeGrowth){
		// below the start, we use the lowest form
		if(weaponGraphics->chargePercentRange[0] >= attackChargeLvl){
			explosionScale = weaponGraphics->chargeSizeRange[0];
		}
		else{
			// above the end, we use the highest form
			if(weaponGraphics->chargePercentRange[1] <= attackChargeLvl){
				explosionScale = weaponGraphics->chargeSizeRange[1];
			}
			else{
				// inbetween, we work out the value
				float PctRange = weaponGraphics->chargePercentRange[1] - weaponGraphics->chargePercentRange[0];
				float PctVal = attackChargeLvl - weaponGraphics->chargePercentRange[0];
				float SizeRange = weaponGraphics->chargeSizeRange[1] - weaponGraphics->chargeSizeRange[0];
				float SizeVal = (PctVal / PctRange) * SizeRange;
				explosionScale = SizeVal + weaponGraphics->chargeSizeRange[0];
			}
		}
	}
	if(explosionScale > 2){explosionScale = 2;}
	explosionScale = explosionScale * weaponGraphics->explosionSize;
	// skew the time a bit so they aren't all in sync
	offset = rand() & 63;	
	if(!weaponGraphics->explosionModel || !weaponGraphics->explosionSkin){
		if(weaponGraphics->explosionShader){
			// allocate the entity as a sprite explosion
			expShell = CG_AllocLocalEntity();
			expShell->leFlags = 0;
			expShell->leType = LE_SPRITE_EXPLOSION;
			// set the type as sprite and link the image
			expShell->refEntity.reType = RT_SPRITE;
			expShell->refEntity.customShader = weaponGraphics->explosionShader;
			expShell->refEntity.radius = 4 * explosionScale;//weaponGraphics->explosionSize;
			// randomly rotate sprite orientation
			expShell->refEntity.rotation = rand() % 360;
			// set origin
			VectorScale(dir,16,tmpVec);
			VectorAdd(tmpVec,origin,newOrigin);
			VectorCopy(newOrigin,expShell->refEntity.origin);
			VectorCopy(newOrigin,expShell->refEntity.oldorigin);
			// set the explosion's duration
			expShell->startTime = cg.time - offset;
			expShell->endTime = expShell->startTime + weaponGraphics->explosionTime;
			expShell->lifeRate = 1.0f / (expShell->endTime - expShell->startTime);
			// screen fade
			cg.screenFlashTime = cg.time - offset;
			cg.screenFlastTimeTotal = weaponGraphics->explosionTime;
			cg.screenFlashFadeTime = weaponGraphics->explosionTime;
			if(explosionScale > 0){
				float distance = Distance(cg.refdef.vieworg,origin);
				cg.screenFlashFadeAmount = 1.0f - distance / (explosionScale * 100);
				cg.screenFlashFadeAmount = Com_Clamp(0.0f,0.5f,cg.screenFlashFadeAmount);
			}
			// create a camera shake
			CG_AddEarthquake(origin, 
				explosionScale/*weaponGraphics->explosionSize*/ / 2 
				,weaponGraphics->explosionTime / 1000
				,(weaponGraphics->explosionTime / 1000 ) / 2
				,(weaponGraphics->explosionTime / 1000 ) * 2
				,explosionScale/*weaponGraphics->explosionSize*/ / 2);
			// bias the time so all shader effects start correctly
			expShell->refEntity.shaderTime = expShell->startTime / 1000.0f;
			// set the Dlight RGB values
			VectorCopy(weaponGraphics->explosionDlightColor,expShell->lightColor);
			expShell->light = weaponGraphics->explosionDlightRadius * 100;
		}
	}
	else{
		// allocate the entity as a ZEQ explosion
		expShell = CG_AllocLocalEntity();
		expShell->leFlags = 0;
		expShell->leType = LE_ZEQEXPLOSION;
		// set the type as model and link the model and skin
		expShell->refEntity.reType = RT_MODEL;
		expShell->refEntity.hModel = weaponGraphics->explosionModel;
		expShell->refEntity.customSkin = weaponGraphics->explosionSkin;
		// set axis with random rotate
		if(!dir){
			AxisClear(expShell->refEntity.axis);
		}
		else{
			angle = rand() % 360;
			VectorCopy(dir,expShell->refEntity.axis[0]);
			RotateAroundDirection( expShell->refEntity.axis,angle);
		}
		// scale axes to explosion's full size
		expShell->refEntity.nonNormalizedAxes = qtrue;
		VectorNormalize(expShell->refEntity.axis[0]);
		VectorNormalize(expShell->refEntity.axis[1]);
		VectorNormalize(expShell->refEntity.axis[2]);
		VectorScale(expShell->refEntity.axis[0],explosionScale/*weaponGraphics->explosionSize*/,expShell->refEntity.axis[0]);
		VectorScale(expShell->refEntity.axis[1],explosionScale/*weaponGraphics->explosionSize*/,expShell->refEntity.axis[1]);
		VectorScale(expShell->refEntity.axis[2],explosionScale/*weaponGraphics->explosionSize*/,expShell->refEntity.axis[2]);
		expShell->refEntity.radius = explosionScale;
		// set origin
		VectorCopy(origin,newOrigin);
		VectorCopy(newOrigin,expShell->refEntity.origin);
		VectorCopy(newOrigin,expShell->refEntity.oldorigin);
		// set the explosion's duration
		expShell->startTime = cg.time - offset;
		expShell->endTime = expShell->startTime + weaponGraphics->explosionTime;
		expShell->lifeRate = 1.0f / (expShell->endTime - expShell->startTime);
		// screen fade
		cg.screenFlashTime = cg.time - offset;
		cg.screenFlastTimeTotal = weaponGraphics->explosionTime;
		cg.screenFlashFadeTime = weaponGraphics->explosionTime;
		if(explosionScale > 0){
			float distance = Distance(cg.refdef.vieworg,origin);
			cg.screenFlashFadeAmount = 1.0f - distance / (explosionScale * 100);
			cg.screenFlashFadeAmount = Com_Clamp(0.0f,1.0f,cg.screenFlashFadeAmount);
		}
		// create a camera shake
		CG_AddEarthquake(origin, 
			explosionScale/*weaponGraphics->explosionSize*/ * 100 
			,weaponGraphics->explosionTime / 1000
			,(weaponGraphics->explosionTime / 1000) / 2
			,(weaponGraphics->explosionTime / 1000) * 2
			,explosionScale/*weaponGraphics->explosionSize*/ * 100);
		// bias the time so all shader effects start correctly
		expShell->refEntity.shaderTime = expShell->startTime / 1000.0f;
		// set the Dlight RGB values
		VectorCopy(weaponGraphics->explosionDlightColor,expShell->lightColor);
		expShell->light = weaponGraphics->explosionDlightRadius * 100;
	}
	if(weaponGraphics->shockwaveModel && weaponGraphics->shockwaveSkin){
		// allocate the entity as a ZEQ explosion
		expShock = CG_AllocLocalEntity();
		expShock->leFlags = 0;
		expShock->leType = LE_ZEQEXPLOSION;
		// set the type as model and link the model and skin
		expShock->refEntity.reType = RT_MODEL;
		expShock->refEntity.hModel = weaponGraphics->shockwaveModel;
		expShock->refEntity.customSkin = weaponGraphics->shockwaveSkin;
		// set axis with random rotate
		if(!dir){
			AxisClear(expShock->refEntity.axis);
		}
		else{
			angle = rand() % 360;
			VectorCopy(dir,expShock->refEntity.axis[0]);
			RotateAroundDirection(expShock->refEntity.axis,angle);
		}
		// scale axes to explosion's full size
		expShock->refEntity.nonNormalizedAxes = qtrue;
		VectorNormalize(expShock->refEntity.axis[0]);
		VectorNormalize(expShock->refEntity.axis[1]);
		VectorNormalize(expShock->refEntity.axis[2]);
		VectorScale(expShock->refEntity.axis[0],2 * explosionScale/*weaponGraphics->explosionSize*/,expShock->refEntity.axis[0]);
		VectorScale(expShock->refEntity.axis[1],2 * explosionScale/*weaponGraphics->explosionSize*/,expShock->refEntity.axis[1]);
		VectorScale(expShock->refEntity.axis[2],2 * explosionScale/*weaponGraphics->explosionSize*/,expShock->refEntity.axis[2]);
		// set origin
		VectorCopy(origin,newOrigin);
		VectorCopy(newOrigin,expShock->refEntity.origin);
		VectorCopy(newOrigin,expShock->refEntity.oldorigin);
		// set the explosion's duration
		expShock->startTime = cg.time - offset - weaponGraphics->explosionTime / 4;
		expShock->endTime = expShock->startTime + weaponGraphics->explosionTime / 1.5;
		expShock->lifeRate = 1.0 / (expShock->endTime - expShock->startTime);
/*
		// NOTE: camera shakes for shockwaves?
		// create a camera shake
		CG_AddEarthquake( origin, 
			weaponGraphics->explosionSize * 100 
			,weaponGraphics->explosionTime / 1000
			,(weaponGraphics->explosionTime / 1000) / 2
			,(weaponGraphics->explosionTime / 1000) * 2
			,weaponGraphics->explosionSize * 100);
*/
		// bias the time so all shader effects start correctly
		expShock->refEntity.shaderTime = expShock->startTime / 1000.0f;
		// disable this Dlight
		expShock->light = 0;
	}
	if(weaponGraphics->explosionParticleSystem[0]){
		vec3_t tempAxis[3];
		VectorCopy(dir,tempAxis[0]);
		MakeNormalVectors(tempAxis[0],tempAxis[1],tempAxis[2]);
		PSys_SpawnCachedSystem(weaponGraphics->explosionParticleSystem,origin,tempAxis,NULL,NULL,qfalse,qfalse);
	}
	if(weaponGraphics->smokeParticleSystem[0]){
		vec3_t tempAxis[3];
		VectorCopy(dir,tempAxis[0]);
		MakeNormalVectors(tempAxis[0],tempAxis[1],tempAxis[2]);
		PSys_SpawnCachedSystem(weaponGraphics->smokeParticleSystem,origin,tempAxis,NULL,NULL,qfalse,qfalse);
	}
}
void CG_CreateStraightBeamFade(vec3_t start,vec3_t end,cg_userWeapon_t* weaponGraphics){
	localEntity_t* beam;
	int duration;
	if(!weaponGraphics->missileTrailShader){return;}
	beam = CG_AllocLocalEntity();
	beam->leFlags = 0;
	beam->leType = LE_STRAIGHTBEAM_FADE;
	beam->radius = weaponGraphics->missileTrailRadius ? weaponGraphics->missileTrailRadius : 10.0f;
	beam->refEntity.customShader = weaponGraphics->missileTrailShader;
	VectorCopy(start,beam->refEntity.origin);
	VectorCopy(end,beam->refEntity.oldorigin);
	duration = Distance(start,end);
	beam->startTime = cg.time;
	beam->endTime = cg.time + duration;
	beam->lifeRate = 1.0 / (beam->endTime - beam->startTime);
	beam->refEntity.shaderTime = beam->startTime / 1000.0f;
}
