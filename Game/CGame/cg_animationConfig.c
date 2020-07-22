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
//
#include "cg_local.h"
qboolean CG_ParseAnimationFile(const char* fileName,clientInfo_t* ci,qboolean isCamera){
	char* text_p;
	char* prev;
	char* token;
	int i;
	float fps;
	char text[32000];
	fileHandle_t file;
	animation_t* animations = isCamera ? ci->camAnimations : ci->animations;
	int fileLength = trap_FS_FOpenFile(fileName,&file,FS_READ);
	if(fileLength < 1){
		return qfalse;
	}
	if(fileLength >= sizeof(text) - 1){
		CG_Printf("File %s too long\n",fileName);
		trap_FS_FCloseFile(file);
		return qfalse;
	}
	trap_FS_Read(text,fileLength,file);
	text[fileLength] = 0;
	trap_FS_FCloseFile(file);
	//=============================
	// Parsing
	//=============================
	text_p = text;
	VectorClear(ci->headOffset);
	ci->fixedlegs = qfalse;
	ci->fixedtorso = qfalse;
	ci->overrideHead = qfalse;
	// read optional parameters
	while ( 1 ) {
		prev = text_p;	// so we can unget
		token = COM_Parse( &text_p );
		if(!*token){break;}
		if(!Q_stricmp(token,"headoffset")){
			for(i=0;i<3;i++){
				token = COM_Parse(&text_p);
				if(!*token){break;}
				ci->headOffset[i] = atof(token);
			}
			continue;
		}
		else if(!Q_stricmp(token,"fixedlegs")){
			ci->fixedlegs = qtrue;
			continue;
		}
		else if(!Q_stricmp(token,"fixedtorso")){
			ci->fixedtorso = qtrue;
			continue;
		}
		else if(!Q_stricmp(token,"overrideHead")){
			ci->overrideHead = qtrue;
			continue;
		}
		// if it is a number, start parsing animations
		if(token[0] >= '0' && token[0] <= '9'){
			// unget the token
			text_p = prev;
			break;
		}
		Com_Printf("Unknown token '%s' in %s\n", token, fileName );
	}
	// read information for each frame
	for(i=0;i<MAX_ANIMATIONS;i++){
		token = COM_Parse(&text_p);
		if(!*token){

		// ADDING FOR ZEQ2
		// until we include optional extra taunts, this block will remain turned off.
/*			if(i >= ANIM_GETFLAG && i <= ANIM_NEGATIVE) {
				animations[i].firstFrame = animations[ANIM_GESTURE].firstFrame;
				animations[i].frameLerp = animations[ANIM_GESTURE].frameLerp;
				animations[i].initialLerp = animations[ANIM_GESTURE].initialLerp;
				animations[i].loopFrames = animations[ANIM_GESTURE].loopFrames;
				animations[i].numFrames = animations[ANIM_GESTURE].numFrames;
				animations[i].reversed = qfalse;
				animations[i].flipflop = qfalse;
				continue;
			}
*/			
// END ADDING
			break;
		}
		animations[i].firstFrame = atoi(token);
		token = COM_Parse(&text_p);
		if(!*token){break;}
		animations[i].numFrames = atoi(token);
		animations[i].reversed = qfalse;
		animations[i].flipflop = qfalse;
		if (animations[i].numFrames < 0){
			animations[i].reversed = qtrue;
			animations[i].numFrames = -animations[i].numFrames;
		}
		token = COM_Parse(&text_p);
		if(!*token){break;}
		animations[i].loopFrames = atoi(token);
		token = COM_Parse(&text_p);
		if(!*token){break;}
		fps = atof(token);
		if(!fps){fps = 1;}
		animations[i].frameLerp = 1000 / fps;
		animations[i].initialLerp = 1000 / fps;
		if(!isCamera){
			token = COM_Parse(&text_p);
			if(!*token){break;}
			animations[i].continuous = atoi(token);
		}
		else{
			// Read the continuous flag for ki attack animations
			if(i >= ANIM_KI_ATTACK1_FIRE && i < MAX_ANIMATIONS && (i - ANIM_KI_ATTACK1_FIRE) % 2 == 0){
				token = COM_Parse(&text_p);
				if(!*token){break;}
				animations[i].continuous = atoi(token);
			}
		}
	}
	if(i != MAX_ANIMATIONS){
		CG_Printf("Error parsing animation file: %s\n",fileName);
		return qfalse;
	}
	memcpy(&animations[ANIM_BACKWALK],&animations[ANIM_WALK],sizeof(animation_t));
	memcpy(&animations[ANIM_BACKRUN],&animations[ANIM_RUN],sizeof(animation_t));
	animations[ANIM_BACKWALK].reversed = qtrue;
	animations[ANIM_BACKRUN].reversed = qtrue;
	return qtrue;
}
