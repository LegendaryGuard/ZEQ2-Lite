#include "cg_music.h"
void CG_Music_Start(void){CG_Music_ParsePlaylist();}
void CG_Music_Update(void){
	playerState_t* state = &cg.predictedPlayerState;
	int difference = cg.time - Music.endTime;
	int flags = state->bitFlags;
	TrackTypes type = flags & isStruggling ? Struggling : Fighting;
	if(flags & isTransforming){
		char var[8];
		type = Transforming;
		trap_Cvar_VariableStringBuffer("cg_playTransformTrackToEnd",var,sizeof(var));
		Music.playToEnd = atoi(var) ? qtrue : qfalse;
	}
	else if(!(flags & isUnsafe)){
		float dangerThreshold = state->powerLevel[plMaximum] * 0.4f;
		type = flags & underWater ? Underwater : Idling;
		type = state->powerLevel[plHealth] < dangerThreshold ? IdleDanger : type;
		if(flags & isTargeted || state->lockedTarget > 0){
			type = state->powerLevel[plHealth] < dangerThreshold ? StandoffDanger : Standoff;
		}
	}
	CG_Music_CheckType(type);
	if(cg.time <= Music.endTime){return;}
	if(difference < Music.fadeAmount){
		float percent = 1.0f - (float)difference / (float)Music.fadeAmount;
		trap_Cvar_Set("s_musicvolume",va("%f",percent * cg_music.value));
		return;
	}
	Music.isFading = qfalse;
	Music.playToEnd = qfalse;
	CG_Music_NextTrack();
}
void CG_Music_CheckType(int type){
	if(Music.currentType == type){return;}
	Music.currentType = type;
	if(type == Idling || type == IdleDanger){CG_Music_FadeNext();}
	else if(type == Transforming){
		playerState_t* state = &cg.predictedPlayerState;
		clientInfo_t* info = &cgs.clientinfo[state->clientNum];
		tierConfig_cg* tier = &info->tierConfig[info->tierCurrent];
		if(!tier->transformMusic[0]){CG_Music_NextTrack();}
		else{CG_Music_Play(tier->transformMusic,tier->transformMusicLength);}
	}
	else{CG_Music_NextTrack();}
}
void CG_Music_Play(char* track,int duration){
	char* path = va("music/%s",track);
	if(duration > MUSIC_MAXDURATION){duration = MUSIC_MAXDURATION;}
	Music.endTime = cg.time + duration - Music.fadeAmount;
	trap_S_StartBackgroundTrack(path,path);
	trap_Cvar_Set("s_musicvolume",va("%f",cg_music.value));
}
void CG_Music_NextTrack(){
	int type = Music.currentType;
	int typeSize = Music.typeSize[type];
	int trackIndex = 0;
	if(Music.isFading || Music.playToEnd){return;}
	trackIndex = Music.isRandom ? random() * typeSize : Music.lastTrack[type]+1;
	trackIndex %= typeSize;
	if(trackIndex == Music.lastTrack[type]){trackIndex = (Music.lastTrack[type]+1) % typeSize;}
	Music.lastTrack[type] = trackIndex;
	CG_Music_Play(Music.playlist[type][trackIndex],Music.trackLength[type][trackIndex]);
}
void CG_Music_FadeNext(void){
	if(Music.playToEnd){return;}
	Music.isFading = qtrue;
	Music.endTime = cg.time;
}
int CG_Music_GetMilliseconds(char* time){
	int msPerSecond = 1000;
	int msPerMinute = 60000;
	char* minuteMark = strchr(time,':');
	*minuteMark = '\0';
	return atoi(time) * msPerMinute + atoi(minuteMark+1) * msPerSecond;
}
void CG_Music_ParsePlaylist(void){
	fileHandle_t playlist;
	char* token;
	char* parse;
	char first;
	char last;
	int trackIndex = 0;
	int typeIndex = 0;
	int fileLength = trap_FS_FOpenFile("music/playlist.cfg",&playlist,FS_READ);
	char fileContents[32000];
	Music.fadeAmount = 0;
	Music.isRandom = qfalse;
	if(!fileLength){return;}
	typeIndex = 0;
	trap_FS_Read(fileContents,fileLength,playlist);
	fileContents[fileLength] = 0;
	trap_FS_FCloseFile(playlist);
	parse = fileContents;
	while(1){
		token = COM_Parse(&parse);
		if(!token[0]){break;}
		first = token[0];
		last = token[strlen(token)-1];
		if(!Q_stricmp(token,"type")){
			token = COM_Parse(&parse);
			if(!token[0]){break;}
			if(!Q_stricmp(token,"random")){Music.isRandom = qtrue;}
		}
		else if(!Q_stricmp(token,"fade")){
			token = COM_Parse(&parse);
			if(!token[0]){break;}
			Music.fadeAmount = CG_Music_GetMilliseconds(token);
		}
		else if(first == '}'){
			Music.typeSize[typeIndex] = trackIndex;
			typeIndex += 1;
			trackIndex = 0;
		}
		else if(last != '{'){
			char* track = Music.playlist[typeIndex][trackIndex];
			Q_strncpyz(track,token,strlen(token)+1);
			token = COM_Parse(&parse);
			if(!token[0]){break;}
			Music.trackLength[typeIndex][trackIndex] = CG_Music_GetMilliseconds(token);
			trackIndex += 1;
		}
	}
}
