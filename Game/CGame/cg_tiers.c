#include "cg_local.h"
qboolean CG_RegisterClientModelnameWithTiers(clientInfo_t *ci, const char *modelName, const char *skinName){
	int	i,index,partIndex,damageIndex,lastSkinIndex,lastModelIndex;
	char filename[MAX_QPATH * 2];
	char tierPath[MAX_QPATH];
	char tempPath[MAX_QPATH];
	char legsPath[MAX_QPATH];
	char headPath[MAX_QPATH];
	char cameraPath[MAX_QPATH];
	qhandle_t tempShader;
	Com_sprintf(legsPath,sizeof(legsPath),"%s",modelName);
	Com_sprintf(headPath,sizeof(headPath),"%s",modelName);
	Com_sprintf(cameraPath,sizeof(cameraPath),"%s",modelName);
	Com_sprintf(tempPath,sizeof(tempPath),"players/%s/animCam.cfg",modelName);
	if(ci->cameraModelName && trap_FS_FOpenFile(tempPath,0,FS_READ)>0){
		Com_sprintf(cameraPath,sizeof(modelName),"%s",ci->cameraModelName);
	}
	Com_sprintf(tempPath,sizeof(tempPath),"players/%s/animation.cfg",ci->legsModelName);
	if(ci->legsModelName && trap_FS_FOpenFile(tempPath,0,FS_READ)>0){
		Com_sprintf(legsPath,sizeof(legsPath),"%s",ci->legsModelName);
	}
	Com_sprintf(tempPath,sizeof(tempPath),"players/%s/animation.cfg",ci->headModelName);
	if(ci->headModelName && trap_FS_FOpenFile(tempPath,0,FS_READ)>0){
		Com_sprintf(headPath,sizeof(headPath),"%s",ci->headModelName);
	}
	Com_sprintf(filename,sizeof(filename),"players/%s/animation.cfg",modelName);
	if(!CG_ParseAnimationFile(filename,ci,qtrue)){
		Com_sprintf(filename,sizeof(filename),"players/characters/%s/animation.cfg",modelName);
		if(!CG_ParseAnimationFile(filename,ci,qtrue)){return qfalse;}
	}
	Com_sprintf(filename,sizeof(filename),"players/%s/animCam.cfg",modelName);
	if(!CG_ParseAnimationFile(filename,ci,qfalse)){
		Com_sprintf(filename,sizeof(filename),"players/animCam.cfg");
		if(!CG_ParseAnimationFile(filename,ci,qfalse)){return qfalse;}
	}
	for(i=0;i<MAX_TIERS;++i){
		// ===================================
		// Config
		// ===================================
		Com_sprintf(tierPath,sizeof(tierPath),"players/%s/Tier%i/icon.png",modelName,i+1);
		if(trap_FS_FOpenFile(tierPath,0,FS_READ) <= 0){continue;}
		memset(&ci->tierConfig[i],0,sizeof(tierConfig_cg));
		tempShader = trap_R_RegisterShaderNoMip(tierPath);
		for(index=0;index<10;++index){
			ci->tierConfig[i].icon2D[index] = tempShader; 
			ci->tierConfig[i].screenEffect[index] = cgs.media.clearShader;
		}
		Com_sprintf(tierPath,sizeof(tierPath),"players/%s/Tier%i/transformScript.cfg",modelName,i+1);
		if(trap_FS_FOpenFile(tierPath,0,FS_READ) > 0){
			ci->tierConfig[i].transformScriptExists = qtrue;
		}
		Com_sprintf(filename,sizeof(filename),"players/TierDefault.config",modelName,i+1);
		parseTier(filename,&ci->tierConfig[i]);
		Com_sprintf(filename,sizeof(filename),"players/%s/Tier%i/%s/Tier.config",modelName,i+1,skinName);
		parseTier(filename,&ci->tierConfig[i]);
		// ===================================
		// Models
		// ===================================
		if(!ci->tierConfig[i].headModel){
			if(i != 0){
				ci->tierConfig[i].headModel = ci->tierConfig[i-1].headModel;
			}
			else{return qfalse;}
		}
		if(!ci->tierConfig[i].torsoModel){
			if(i != 0){
				ci->tierConfig[i].torsoModel = ci->tierConfig[i-1].torsoModel;
			}
			else{return qfalse;}
		}
		if(!ci->tierConfig[i].legsModel){
			if(i != 0){
				ci->tierConfig[i].legsModel = ci->tierConfig[i-1].legsModel;
			}
			else{return qfalse;}
		}
		// ===================================
		// Skins
		// ===================================
		if(!ci->tierConfig[i].headSkin){
			if(i != 0){
				ci->tierConfig[i].headSkin = ci->tierConfig[i-1].headSkin;
			}
			else{return qfalse;}
		}
		if(!ci->tierConfig[i].torsoSkin){
			if(i != 0){
				ci->tierConfig[i].torsoSkin = ci->tierConfig[i-1].torsoSkin;
			}
			else{return qfalse;}
		}
		if(!ci->tierConfig[i].legsSkin){
			if(i != 0){
				ci->tierConfig[i].legsSkin = ci->tierConfig[i-1].legsSkin;
			}
			else{return qfalse;}
		}
		// ===================================
		// Damage States
		// ===================================
		for(partIndex=0;partIndex<3;++partIndex){
			lastSkinIndex = -1;
			lastModelIndex = -1;
			for(damageIndex=MAX_DAMAGED_STATES-1;damageIndex>=0;--damageIndex){
				if(partIndex == 0){
					if(ci->tierConfig[i].headModelDamaged[damageIndex]){
						lastModelIndex = damageIndex;
					}
					else{
						ci->tierConfig[i].headModelDamaged[damageIndex] = lastModelIndex != -1 ? ci->tierConfig[i].headModelDamaged[lastModelIndex] : ci->tierConfig[i].headModel;
					}
					if(ci->tierConfig[i].headSkinDamaged[damageIndex]){
						lastSkinIndex = damageIndex;
					}
					else{
						ci->tierConfig[i].headSkinDamaged[damageIndex] = lastSkinIndex != -1 ? ci->tierConfig[i].headSkinDamaged[lastSkinIndex] : ci->tierConfig[i].headSkin;
					}
				}
				if(partIndex == 1){
					if(ci->tierConfig[i].torsoModelDamaged[damageIndex]){
						lastModelIndex = damageIndex;
					}
					else{
						ci->tierConfig[i].torsoModelDamaged[damageIndex] = lastModelIndex != -1 ? ci->tierConfig[i].torsoModelDamaged[lastModelIndex] : ci->tierConfig[i].torsoModel;
					}
					if(ci->tierConfig[i].torsoSkinDamaged[damageIndex]){
						lastSkinIndex = damageIndex;
					}
					else{
						ci->tierConfig[i].torsoSkinDamaged[damageIndex] = lastSkinIndex != -1 ? ci->tierConfig[i].torsoSkinDamaged[lastSkinIndex] : ci->tierConfig[i].torsoSkin;
					}
				}
				else{
					if(ci->tierConfig[i].legsModelDamaged[damageIndex]){
						lastModelIndex = damageIndex;
					}
					else{
						ci->tierConfig[i].legsModelDamaged[damageIndex] = lastModelIndex != -1 ? ci->tierConfig[i].legsModelDamaged[lastModelIndex] : ci->tierConfig[i].legsModel;
					}
					if(ci->tierConfig[i].legsSkinDamaged[damageIndex]){
						lastSkinIndex = damageIndex;
					}
					else{
						ci->tierConfig[i].legsSkinDamaged[damageIndex] = lastSkinIndex != -1 ? ci->tierConfig[i].legsSkinDamaged[lastSkinIndex] : ci->tierConfig[i].legsSkin;
					}
				}
			}
		}
	}
	return qtrue;
}
void parseTier(char *path,tierConfig_cg *tier){
	fileHandle_t tierCFG;
	int i;
	char *token,*parse;
	int fileLength;
	int tokenInt;
	float tokenFloat;
	char fileContents[32000];
	if(trap_FS_FOpenFile(path,0,FS_READ)>0){
		fileLength = trap_FS_FOpenFile(path,&tierCFG,FS_READ);
		trap_FS_Read(fileContents,fileLength,tierCFG);
		fileContents[fileLength] = 0;
		trap_FS_FCloseFile(tierCFG);
		parse = fileContents;
		while(1){
			token = COM_Parse(&parse);
			if(!token[0]){break;}
			else if(!Q_stricmp(token,"tierName")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				Q_strncpyz(tier->name, token, sizeof(tier->name));
			}
			else if(!Q_stricmp(token,"powerLevelHudMultiplier")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->hudMultiplier = atof(token);
			}
			else if(!Q_stricmp(token,"sustainCurrent")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->sustainCurrent = atoi(token);
			}
			else if(!Q_stricmp(token,"sustainCurrentPercent")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->sustainCurrentPercent = atoi(token);
			}
			else if(!Q_stricmp(token,"sustainMaximum")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->sustainMaximum = atoi(token);
			}
			else if(!Q_stricmp(token,"sustainFatigue")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->sustainFatigue = atoi(token);
			}
			else if(!Q_stricmp(token,"sustainHealth")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->sustainHealth = atoi(token);
			}
			else if(!Q_stricmp(token,"requirementCurrent")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->requirementCurrent = atoi(token);
			}
			else if(!Q_stricmp(token,"requirementCurrentPercent")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->requirementCurrentPercent = atoi(token);
			}
			else if(!Q_stricmp(token,"requirementFatigue")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->requirementFatigue = atoi(token);
			}
			else if(!Q_stricmp(token,"requirementMaximum")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->requirementMaximum = atoi(token);
			}
			else if(!Q_stricmp(token,"requirementHealth")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->requirementHealth = atoi(token);
			}
			else if(!Q_stricmp(token,"requirementHealthMaximum")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->requirementHealthMaximum = atoi(token);
			}
			else if(!Q_stricmp(token,"transformSoundFirst")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->soundTransformFirst = trap_S_RegisterSound(token,qfalse);
			}
			else if(!Q_stricmp(token,"transformSoundUp")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->soundTransformUp = trap_S_RegisterSound(token,qfalse);
			}
			else if(!Q_stricmp(token,"transformSoundDown")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->soundTransformDown = trap_S_RegisterSound(token,qfalse);
			}
			else if(!Q_stricmp(token,"transformMusic")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				if(trap_FS_FOpenFile(va("music/%s.ogg", token),0,FS_READ)>0){
					Q_strncpyz(tier->transformMusic, token, sizeof(tier->transformMusic));
					token = COM_Parse(&parse);
					if(!token[0]){break;}
					tier->transformMusicLength = CG_GetMilliseconds(token);
				}
			}
			else if(!Q_stricmp(token,"poweringUpSound")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->soundPoweringUp = trap_S_RegisterSound(token,qfalse);
			}
			else if(!Q_stricmp(token,"headModel")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->headModel = trap_R_RegisterModel(token);
			}
			else if(!Q_stricmp(token,"torsoModel")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->torsoModel = trap_R_RegisterModel(token);
			}
			else if(!Q_stricmp(token,"legsModel")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->legsModel = trap_R_RegisterModel(token);
			}
			else if(!Q_stricmp(token,"cameraModel")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->cameraModel = trap_R_RegisterModel(token);
			}
			else if(!Q_stricmp(token,"headSkin")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->headSkin = trap_R_RegisterSkin(token);
			}
			else if(!Q_stricmp(token,"torsoSkin")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->torsoSkin = trap_R_RegisterSkin(token);
			}
			else if(!Q_stricmp(token,"legsSkin")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->legsSkin = trap_R_RegisterSkin(token);
			}
			else if(!Q_stricmp(token,"headModelDamaged")){
				int percent = -1;
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				percent = atoi(token) / 10 - 1;
				if(percent < 0){percent = 0;}
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->headModelDamaged[percent] = trap_R_RegisterModel(token);
			}
			else if(!Q_stricmp(token,"torsoModelDamaged")){
				int percent = -1;
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				percent = atoi(token) / 10 - 1;
				if(percent < 0){percent = 0;}
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->torsoModelDamaged[percent] = trap_R_RegisterModel(token);
			}
			else if(!Q_stricmp(token,"legsModelDamaged")){
				int percent = -1;
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				percent = atoi(token) / 10 - 1;
				if(percent < 0){percent = 0;}
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->legsModelDamaged[percent] = trap_R_RegisterModel(token);
			}
			else if(!Q_stricmp(token,"headSkinDamaged")){
				int percent = -1;
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				percent = atoi(token) / 10 - 1;
				if(percent < 0){percent = 0;}
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->headSkinDamaged[percent] = trap_R_RegisterSkin(token);
			}
			else if(!Q_stricmp(token,"torsoSkinDamaged")){
				int percent = -1;
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				percent = atoi(token) / 10 - 1;
				if(percent < 0){percent = 0;}
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->torsoSkinDamaged[percent] = trap_R_RegisterSkin(token);
			}
			else if(!Q_stricmp(token,"legsSkinDamaged")){
				int percent = -1;
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				percent = atoi(token) / 10 - 1;
				if(percent < 0){percent = 0;}
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->legsSkinDamaged[percent] = trap_R_RegisterSkin(token);
			}
			else if(!Q_stricmp(token,"damageFeatures")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->damageFeatures = strlen(token) == 4 ? qtrue : qfalse;
			}
			else if(!Q_stricmp(token,"damageModelsRevertHealed")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->damageModelsRevertHealed = strlen(token) == 4 ? qtrue : qfalse;
			}
			else if(!Q_stricmp(token,"damageTexturesRevertHealed")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->damageTexturesRevertHealed = strlen(token) == 4 ? qtrue : qfalse;
			}
			else if(!Q_stricmp(token,"icon2DShader")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tokenInt = atoi(token);
				token = COM_Parse(&parse);
				if(Q_stricmp(token,"default")){
					int countdown = tokenInt/10-1;
					while(countdown > 0){
						tier->icon2D[countdown] = trap_R_RegisterShaderNoMip(token);
						countdown -= 1;
					}
				}
			}
			else if(!Q_stricmp(token,"icon2DPoweringShader")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				if(Q_stricmp(token,"default")){
					tier->icon2DPowering = trap_R_RegisterShaderNoMip(token);
				}
			}
			else if(!Q_stricmp(token,"icon2DTransformingShader")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				if(Q_stricmp(token,"default")){
					tier->icon2DTransforming = trap_R_RegisterShaderNoMip(token);
				}
			}
			else if(!Q_stricmp(token,"icon3DOffset")){
				tier->icon3DOffset[0] = atoi(COM_Parse(&parse));
				tier->icon3DOffset[1] = atoi(COM_Parse(&parse));
			}
			else if(!Q_stricmp(token,"icon3DRotation")){
				tier->icon3DRotation[0] = atoi(COM_Parse(&parse));
				tier->icon3DRotation[1] = atoi(COM_Parse(&parse));
				tier->icon3DRotation[2] = atoi(COM_Parse(&parse));
			}
			else if(!Q_stricmp(token,"icon3DSize")){
				tier->icon3DSize[0] = atoi(COM_Parse(&parse));
				tier->icon3DSize[1] = atoi(COM_Parse(&parse));
			}
			else if(!Q_stricmp(token,"icon3DZoom")){
				tier->icon3DZoom = atof(COM_Parse(&parse));
			}
			else if(!Q_stricmp(token,"screenShader")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tokenInt = atoi(token);
				token = COM_Parse(&parse);
				if(Q_stricmp(token,"default")){
					int countdown = tokenInt/10-1;
					while(countdown > 0){
						tier->screenEffect[countdown] = trap_R_RegisterShaderNoMip(token);
						countdown -= 1;
					}
				}
			}
			else if(!Q_stricmp(token,"screenPoweringShader")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				if(Q_stricmp(token,"default")){
					tier->screenEffectPowering = trap_R_RegisterShaderNoMip(token);
				}
			}
			else if(!Q_stricmp(token,"screenTransformingShader")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				if(Q_stricmp(token,"default")){
					tier->screenEffectTransforming = trap_R_RegisterShaderNoMip(token);
				}
			}
			else if(!Q_stricmp(token,"crosshairShader")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				if(Q_stricmp(token,"default")){
					tier->crosshair = trap_R_RegisterShaderNoMip(token);
				}
			}
			else if(!Q_stricmp(token,"crosshairPoweringShader")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				if(Q_stricmp(token,"default")){
					tier->crosshairPowering = trap_R_RegisterShaderNoMip(token);
				}
			}
			else if(!Q_stricmp(token,"meshScale")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->meshScale = atof(token);
			}
			else if(!Q_stricmp(token,"meshOffset")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->meshOffset = atoi(token);
			}
			else if(!Q_stricmp(token,"cameraOffsetSlide")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->cameraOffset[0] = atoi(token);
			}
			else if(!Q_stricmp(token,"cameraOffsetHeight")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->cameraOffset[1] = atoi(token);
			}
			else if(!Q_stricmp(token,"cameraOffsetRange")){
				token = COM_Parse(&parse);
				if(!token[0]){break;}
				tier->cameraOffset[2] = atoi(token);
			}
		}
	}
}


/*
===============
CG_NextTier_f
===============
*/
void CG_NextTier_f( void ) {
	if ( !cg.snap ) {
		return;
	}
	if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) {
		return;
	}

	if(cg.tierSelect == -1)
	{
		cg.tierSelectionMode = 2;
		cg.tierSelect = 0;
	}
}

/*
===============
CG_PrevTier_f
===============
*/
void CG_PrevTier_f( void ) {
	if ( !cg.snap ) {
		return;
	}
	if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) {
		return;
	}

	if(cg.tierSelect == -1)
		{
			cg.tierSelectionMode = 1;
			cg.tierSelect = 0;
		}
}


/*
===============
Tier
===============
*/
void CG_Tier_f( void ) {
	int		num;

	if ( !cg.snap ) {
		return;
	}
	if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) {
		return;
	}
	num = atoi( CG_Argv( 1 ) );

	if ( num < 0 || num > 10 ) {
		return;
	}

	if(cg.tierSelect == -1)
	{
		cg.tierSelect = num;
		cg.tierSelectionMode = 3;
	}
}
