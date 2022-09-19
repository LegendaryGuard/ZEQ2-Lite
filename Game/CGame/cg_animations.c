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
#include "cg_local.h"
// May include ANIM_TOGGLEBIT
void CG_SetLerpFrameAnimation(clientInfo_t* ci,lerpFrame_t* lf,int newAnimation,qboolean isCamera){
	animation_t* anim;
	lf->animationNumber = newAnimation;
	newAnimation &= ~ANIM_TOGGLEBIT;
	if(newAnimation < 0 || newAnimation >= MAX_TOTALANIMATIONS){
		CG_Error("Bad animation number: %i",newAnimation);
	}
    anim = isCamera ? &ci->animations[newAnimation] : &ci->camAnimations[newAnimation];
	lf->animation = anim;
	lf->animationTime = lf->frameTime + anim->initialLerp;
	if(cg_debugAnim.integer){
		CG_Printf("Anim: %i\n",newAnimation);
	}
}
/*
===============
CG_RunLerpFrame

Sets cg.snap, cg.oldFrame, and cg.backlerp
cg.time should be between oldFrameTime and frameTime after exit
===============
*/
void CG_RunLerpFrame(clientInfo_t* ci,lerpFrame_t* lf,int newAnimation,float speedScale,qboolean isCamera){
	int f;
    int numFrames;
	animation_t* anim;
	// debugging tool to get no animations
	if(!cg_animSpeed.integer){
		lf->oldFrame = lf->frame = lf->backlerp = 0;
		return;
	}
	// see if the animation sequence is switching
	if(!lf->animation){
		CG_SetLerpFrameAnimation(ci,lf,newAnimation,isCamera);
	}
	if(newAnimation != lf->animationNumber){
		// If the only difference is the togglebit, and the animation is supposed to be
		// continuous.
		if((newAnimation & ~ANIM_TOGGLEBIT) == (lf->animationNumber & ~ANIM_TOGGLEBIT) && lf->animation->continuous){
			// do nothing, animation should continue to loop
		}
		else{
			CG_SetLerpFrameAnimation( ci, lf, newAnimation, isCamera );
		}
	}
	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	if(cg.time >= lf->frameTime){
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;
		// get the next frame based on the animation
		anim = lf->animation;
		// shouldn't happen
		if(!anim->frameLerp){return;}
		if(cg.time < lf->animationTime){
			// initial lerp
			lf->frameTime = lf->animationTime;
		}
		else{
			lf->frameTime = lf->oldFrameTime + anim->frameLerp;
		}
		f = (lf->frameTime - lf->animationTime) / anim->frameLerp;
		// adjust for haste, etc
		f *= speedScale;
		numFrames = anim->numFrames;
		if(anim->flipflop){
			numFrames *= 2;
		}
		if(f >= numFrames){
			f -= numFrames;
			if(anim->loopFrames){
				f %= anim->loopFrames;
				f += anim->numFrames - anim->loopFrames;
			}
			else{
				f = numFrames - 1;
				// the animation is stuck at the end, so it
				// can immediately transition to another sequence
				lf->frameTime = cg.time;
			}
		}
		if(anim->reversed){
			lf->frame = anim->firstFrame + anim->numFrames - 1 - f;
		}
		else if(anim->flipflop && f>=anim->numFrames){
			lf->frame = anim->firstFrame + anim->numFrames - 1 - (f%anim->numFrames);
		}
		else{
			lf->frame = anim->firstFrame + f;
		}
		if(cg.time > lf->frameTime){
			lf->frameTime = cg.time;
			if(cg_debugAnim.integer){
				CG_Printf("Clamp lf->frameTime\n");
			}
		}
	}
	if(lf->frameTime > cg.time + 200){
		lf->frameTime = cg.time;
	}
	if(lf->oldFrameTime > cg.time){
		lf->oldFrameTime = cg.time;
	}
	// calculate current lerp value
	lf->backlerp = lf->frameTime == lf->oldFrameTime ? 0 : 1 - (float)(cg.time - lf->oldFrameTime) / (lf->frameTime - lf->oldFrameTime);
}
void CG_ClearLerpFrame(clientInfo_t* ci,lerpFrame_t* lf,int animationNumber,qboolean isCamera){
	lf->frameTime = lf->oldFrameTime = cg.time;
	CG_SetLerpFrameAnimation(ci,lf,animationNumber,isCamera);
	lf->oldFrame = lf->frame = lf->animation->firstFrame;
}
void CG_PlayerAnimation(centity_t* cent,
								int* legsOld,int* legs,float* legsBackLerp,
								int* torsoOld,int* torso,float* torsoBackLerp,
								int* headOld,int* head,float* headBackLerp,
								int* cameraOld,int* camera,float* cameraBackLerp){
	clientInfo_t* ci;
	int clientNum = cent->currentState.clientNum;
	qboolean isCorpse = cent->currentState.number != clientNum;
	int tier = isCorpse ? 0 : cent->currentState.tier;
	float speedScale = 1.0f;
	if(cg_noPlayerAnims.integer){
		*legsOld = *legs = *torsoOld = *torso = 0;
		return;
	}
	ci = &cgs.clientinfo[clientNum];
	CG_RunLerpFrame(ci,&cent->pe.legs,cent->currentState.legsAnim,speedScale,qfalse);
	*legsOld = cent->pe.legs.oldFrame;
	*legs = cent->pe.legs.frame;
	*legsBackLerp = cent->pe.legs.backlerp;
	CG_RunLerpFrame(ci,&cent->pe.torso,cent->currentState.torsoAnim,speedScale,qfalse);
	*torsoOld = cent->pe.torso.oldFrame;
	*torso = cent->pe.torso.frame;
	*torsoBackLerp = cent->pe.torso.backlerp;
	CG_RunLerpFrame(ci,&cent->pe.camera,cent->currentState.torsoAnim,speedScale,qtrue);
	*cameraOld = cent->pe.camera.oldFrame;
	*camera = cent->pe.camera.frame;
	*cameraBackLerp = cent->pe.camera.backlerp;
	{
		// NOTE: Torso animations take precedence over leg animations when deciding which head animation to play.
		int torsoAnimNum = cent->currentState.torsoAnim & ~ANIM_TOGGLEBIT;
		int headAnimation = torsoAnimNum;
		qboolean isBoosting = cg.predictedPlayerState.bitFlags & usingBoost;
		if(isBoosting){
			headAnimation = ANIM_KI_CHARGE;
		}
		else if(ci->overrideHead){
			qboolean hasAura = cent->currentState.eFlags & EF_AURA;
			qboolean auraAlways = ci->auraConfig[tier]->auraAlways;
			if(hasAura || auraAlways || torsoAnimNum == ANIM_FLY_UP || torsoAnimNum == ANIM_FLY_DOWN){
				headAnimation = ANIM_KI_CHARGE;
			}
		}
		else if(torsoAnimNum == ANIM_KI_CHARGE || torsoAnimNum == ANIM_PL_UP || torsoAnimNum == ANIM_PUSH){
			headAnimation = ANIM_TRANS_UP;
		}
		CG_RunLerpFrame(ci,&cent->pe.head,headAnimation,speedScale,qfalse);
	}
	*headOld = cent->pe.head.oldFrame;
	*head = cent->pe.head.frame;
	*headBackLerp = cent->pe.head.backlerp;
}
