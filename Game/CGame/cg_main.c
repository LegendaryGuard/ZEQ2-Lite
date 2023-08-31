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
// cg_main.c -- initialization and primary entry point for cgame
#include "cg_local.h"
int forceModelModificationCount = -1;

void CG_Init( int serverMessageNum, int serverCommandSequence, int clientNum );
void CG_Shutdown( void );


/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
Q_EXPORT intptr_t vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11  ) {

	switch ( command ) {
	case CG_INIT:
		CG_Init( arg0, arg1, arg2 );
		return 0;
	case CG_SHUTDOWN:
		CG_Shutdown();
		return 0;
	case CG_CONSOLE_COMMAND:
		return CG_ConsoleCommand();
	case CG_DRAW_ACTIVE_FRAME:
		CG_DrawActiveFrame( arg0, arg1, arg2 );
		return 0;
	case CG_CROSSHAIR_PLAYER:
		return CG_CrosshairPlayer();
	case CG_LAST_ATTACKER:
		return CG_LastAttacker();
	case CG_KEY_EVENT:
		CG_KeyEvent(arg0, arg1);
		return 0;
	case CG_MOUSE_EVENT:
		CG_MouseEvent(arg0, arg1);
		return 0;
	case CG_EVENT_HANDLING:
		CG_EventHandling(arg0);
		return 0;
	default:
		CG_Error( "vmMain: unknown command %i", command );
		break;
	}
	return -1;
}


cg_t				cg;
cgs_t				cgs;
centity_t			cg_entities[MAX_GENTITIES];
weaponInfo_t		cg_weapons[MAX_WEAPONS];

#define DECLARE_CG_CVAR
	#include "cg_cvar.h"
#undef DECLARE_CG_CVAR

typedef struct {
	vmCvar_t	*vmCvar;
	char		*cvarName;
	char		*defaultString;
	int			cvarFlags;
} cvarTable_t;

static cvarTable_t cvarTable[] = {

#define CG_CVAR_LIST
	#include "cg_cvar.h"
#undef CG_CVAR_LIST

};

static int  cvarTableSize = ARRAY_LEN( cvarTable );

/*
=================
CG_RegisterCvars
=================
*/
void CG_RegisterCvars( void ) {
	int			i;
	cvarTable_t	*cv;
	char		var[MAX_TOKEN_CHARS];

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		trap_Cvar_Register( cv->vmCvar, cv->cvarName,
			cv->defaultString, cv->cvarFlags );
	}

	// see if we are also running the server on this machine
	trap_Cvar_VariableStringBuffer( "sv_running", var, sizeof( var ) );
	cgs.localServer = atoi( var );

	forceModelModificationCount = cg_forceModel.modificationCount;

	trap_Cvar_Register(NULL, "model", DEFAULT_MODEL, CVAR_USERINFO | CVAR_ARCHIVE );
	trap_Cvar_Register(NULL, "legsmodel", DEFAULT_MODEL, CVAR_USERINFO | CVAR_ARCHIVE );
	trap_Cvar_Register(NULL, "headmodel", DEFAULT_MODEL, CVAR_USERINFO | CVAR_ARCHIVE );
	trap_Cvar_Register(NULL, "team_model", DEFAULT_TEAM_MODEL, CVAR_USERINFO | CVAR_ARCHIVE );
	trap_Cvar_Register(NULL, "team_headmodel", DEFAULT_TEAM_HEAD, CVAR_USERINFO | CVAR_ARCHIVE );
}

/*
=================
CG_UpdateCvars
=================
*/
void CG_UpdateCvars( void ) {
	int			i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		trap_Cvar_Update( cv->vmCvar );
	}

	// check for modications here

	// If team overlay is on, ask for updates from the server.  If it's off,
	// let the server know so we don't receive it
	if ( drawTeamOverlayModificationCount != cg_drawTeamOverlay.modificationCount ) {
		drawTeamOverlayModificationCount = cg_drawTeamOverlay.modificationCount;

		if ( cg_drawTeamOverlay.integer > 0 ) {
			trap_Cvar_Set( "teamoverlay", "1" );
		} else {
			trap_Cvar_Set( "teamoverlay", "0" );
		}
	}
}

int CG_CrosshairPlayer( void ) {
	if ( cg.time > ( cg.crosshairClientTime + 1000 ) ) {
		return -1;
	}
	return cg.crosshairClientNum;
}

int CG_LastAttacker( void ) {
	if ( !cg.attackerTime ) {
		return -1;
	}
	return cg.snap->ps.persistant[PERS_ATTACKER];
}

void QDECL CG_Printf( const char *msg, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	Q_vsnprintf (text, sizeof(text), msg, argptr);
	va_end (argptr);

	trap_Print( text );
}

void QDECL CG_Error( const char *msg, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	Q_vsnprintf (text, sizeof(text), msg, argptr);
	va_end (argptr);

	trap_Error( text );
}

void QDECL Com_Error( int level, const char *error, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, error);
	Q_vsnprintf (text, sizeof(text), error, argptr);
	va_end (argptr);

	CG_Error( "%s", text);
}

void QDECL Com_Printf( const char *msg, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	Q_vsnprintf (text, sizeof(text), msg, argptr);
	va_end (argptr);

	CG_Printf ("%s", text);
}

/*
================
CG_Argv
================
*/
const char *CG_Argv( int arg ) {
	static char	buffer[MAX_STRING_CHARS];

	trap_Argv( arg, buffer, sizeof( buffer ) );

	return buffer;
}


/*
=================
CG_RegisterSounds

called during a precache command
=================
*/
static void CG_RegisterSounds( void ) {
	int		i;
	const char	*soundName;

	for ( i = 1 ; i < MAX_SOUNDS ; i++ ) {
		soundName = CG_ConfigString( CS_SOUNDS+i );
		if ( !soundName[0] ) {
			break;
		}
		if ( soundName[0] == '*' ) {
			continue;	// custom sound
		}
		cgs.gameSounds[i] = trap_S_RegisterSound( soundName, qfalse );
	}
	// ADDING FOR ZEQ2

	cgs.media.radarwarningSound = trap_S_RegisterSound( "interface/sense/warning.ogg", qfalse );
	cgs.media.lightspeedSound1 = trap_S_RegisterSound( "effects/zanzoken/zanzoken1.ogg", qfalse );
	cgs.media.lightspeedSound2 = trap_S_RegisterSound( "effects/zanzoken/zanzoken2.ogg", qfalse );
	cgs.media.lightspeedSound3 = trap_S_RegisterSound( "effects/zanzoken/zanzoken3.ogg", qfalse );
	cgs.media.lightspeedSound4 = trap_S_RegisterSound( "effects/zanzoken/zanzoken4.ogg", qfalse );
	cgs.media.lightspeedSound5 = trap_S_RegisterSound( "effects/zanzoken/zanzoken5.ogg", qfalse );
	cgs.media.bigLightning[0] = trap_S_RegisterSound("effects/melee/lightning1.ogg",qfalse);
	cgs.media.bigLightning[1] = trap_S_RegisterSound("effects/melee/lightning2.ogg",qfalse);
	cgs.media.bigLightning[2] = trap_S_RegisterSound("effects/melee/lightning3.ogg",qfalse);
	cgs.media.bigLightning[3] = trap_S_RegisterSound("effects/melee/lightning4.ogg",qfalse);
	cgs.media.bigLightning[4] = trap_S_RegisterSound("effects/melee/lightning5.ogg",qfalse);
	cgs.media.bigLightning[5] = trap_S_RegisterSound("effects/melee/lightning6.ogg",qfalse);
	cgs.media.bigLightning[6] = trap_S_RegisterSound("effects/melee/lightning7.ogg",qfalse);
	cgs.media.blockSound = trap_S_RegisterSound( "effects/melee/block.ogg", qfalse );
	cgs.media.knockbackSound = trap_S_RegisterSound( "effects/melee/knockback.ogg", qfalse );
	cgs.media.knockbackLoopSound = trap_S_RegisterSound( "effects/melee/knockbackLoop.ogg", qfalse );
	cgs.media.speedMeleeSound = trap_S_RegisterSound( "effects/melee/speedHit1.ogg", qfalse );
	cgs.media.speedMissSound = trap_S_RegisterSound( "effects/melee/speedMiss1.ogg", qfalse );
	cgs.media.speedBlockSound = trap_S_RegisterSound( "effects/melee/speedBlock1.ogg", qfalse );
	cgs.media.stunSound = trap_S_RegisterSound( "effects/melee/stun1.ogg", qfalse );
	cgs.media.powerStunSound1 = trap_S_RegisterSound( "effects/melee/powerStun1.ogg", qfalse );
	cgs.media.powerStunSound2 = trap_S_RegisterSound( "effects/melee/powerStun2.ogg", qfalse );
	cgs.media.powerMeleeSound = trap_S_RegisterSound( "effects/melee/powerHit1.ogg", qfalse );
	cgs.media.powerMissSound = trap_S_RegisterSound( "effects/melee/powerMiss1.ogg", qfalse );
	cgs.media.lockonStart = trap_S_RegisterSound( "effects/powerSense.ogg", qfalse );
	cgs.media.nullSound = trap_S_RegisterSound( "effects/null.ogg", qfalse );
	cgs.media.airBrake1 = trap_S_RegisterSound( "effects/airBrake1.ogg", qfalse );
	cgs.media.airBrake2 = trap_S_RegisterSound( "effects/airBrake2.ogg", qfalse );
	cgs.media.hover = trap_S_RegisterSound( "effects/hover.ogg", qfalse );
	cgs.media.hoverFast = trap_S_RegisterSound( "effects/hoverFast.ogg", qfalse );
	cgs.media.hoverLong = trap_S_RegisterSound( "effects/hoverLong.ogg", qfalse );
	cgs.media.smallSplash[0] = trap_S_RegisterSound("effects/water/SplashSmall.ogg",qfalse);
	cgs.media.smallSplash[1] = trap_S_RegisterSound("effects/water/SplashSmall2.ogg",qfalse);
	cgs.media.smallSplash[2] = trap_S_RegisterSound("effects/water/SplashSmall3.ogg",qfalse);
	cgs.media.mediumSplash[0] = trap_S_RegisterSound("effects/water/SplashMedium.ogg",qfalse);
	cgs.media.mediumSplash[1] = trap_S_RegisterSound("effects/water/SplashMedium2.ogg",qfalse);
	cgs.media.mediumSplash[2] = trap_S_RegisterSound("effects/water/SplashMedium3.ogg",qfalse);
	cgs.media.mediumSplash[3] = trap_S_RegisterSound("effects/water/SplashMedium4.ogg",qfalse);
	cgs.media.largeSplash[0] = trap_S_RegisterSound("effects/water/SplashLarge.ogg",qfalse);
	cgs.media.extraLargeSplash[0] = trap_S_RegisterSound("effects/water/SplashExtraLarge.ogg",qfalse);
	cgs.media.extraLargeSplash[1] = trap_S_RegisterSound("effects/water/SplashExtraLarge2.ogg",qfalse);
	// END ADDING

}


//===================================================================================

/*
=================
CG_RegisterGraphics

This function may execute for a couple of minutes with a slow disk.
=================
*/
static void CG_RegisterGraphics( void ) {
	int			i;
	static char		*sb_nums[11] = {
		"interface/fonts/numbers/0",
		"interface/fonts/numbers/1",
		"interface/fonts/numbers/2",
		"interface/fonts/numbers/3",
		"interface/fonts/numbers/4",
		"interface/fonts/numbers/5",
		"interface/fonts/numbers/6",
		"interface/fonts/numbers/7",
		"interface/fonts/numbers/8",
		"interface/fonts/numbers/9",
		"interface/fonts/numbers/-",
	};

	// clear any references to old media
	memset( &cg.refdef, 0, sizeof( cg.refdef ) );
	trap_R_ClearScene();

	CG_LoadingString( cgs.mapname );

	trap_R_LoadWorldMap( cgs.mapname );
	CG_LoadingString( "game media" );

	for ( i=0 ; i<11 ; i++) {
		cgs.media.numberShaders[i] = trap_R_RegisterShader( sb_nums[i] );
	}

	cgs.media.waterBubbleLargeShader = trap_R_RegisterShader( "waterBubbleLarge" );
	cgs.media.waterBubbleMediumShader = trap_R_RegisterShader( "waterBubbleMedium" );
	cgs.media.waterBubbleSmallShader = trap_R_RegisterShader( "waterBubbleSmall" );
	cgs.media.waterBubbleTinyShader = trap_R_RegisterShader( "waterBubbleTiny" );
	cgs.media.selectShader = trap_R_RegisterShader( "interface/hud/select.png" );
	cgs.media.chatBubble = trap_R_RegisterShader( "chatBubble");

	for ( i = 0 ; i < NUM_CROSSHAIRS ; i++ ) {
		cgs.media.crosshairShader[i] = trap_R_RegisterShader( va("crosshair%c", 'a'+i) );
	}
	cgs.media.speedLineShader = trap_R_RegisterShaderNoMip("speedLines");
	cgs.media.speedLineSpinShader = trap_R_RegisterShaderNoMip("speedLinesSpin");
	cgs.media.globalCelLighting = trap_R_RegisterShader("GlobalCelLighting");
	cgs.media.waterSplashSkin = trap_R_RegisterSkin( "effects/water/waterSplash.skin" );
	cgs.media.waterSplashModel = trap_R_RegisterModel( "effects/water/waterSplash.md3" );
	cgs.media.waterRippleSkin = trap_R_RegisterSkin( "effects/water/waterRipple.skin" );
	cgs.media.waterRippleModel = trap_R_RegisterModel( "effects/water/waterRipple.md3" );
	cgs.media.waterRippleSingleSkin = trap_R_RegisterSkin( "effects/water/waterRippleSingle.skin" );
	cgs.media.waterRippleSingleModel = trap_R_RegisterModel( "effects/water/waterRippleSingle.md3" );
	cgs.media.meleeSpeedEffectShader = trap_R_RegisterShader( "skills/energyBlast" );
	cgs.media.meleePowerEffectShader = trap_R_RegisterShader( "shockwave" );
	cgs.media.boltEffectShader = trap_R_RegisterShader( "boltEffect" );
	cgs.media.auraLightningSparks1 = trap_R_RegisterShader( "AuraLightningSparks1" );
	cgs.media.auraLightningSparks2 = trap_R_RegisterShader( "AuraLightningSparks2" );
	cgs.media.powerStruggleRaysEffectShader = trap_R_RegisterShader( "PowerStruggleRays" );
	cgs.media.powerStruggleShockwaveEffectShader = trap_R_RegisterShader( "PowerStruggleShockwave" );
	cgs.media.bfgLFGlare = trap_R_RegisterShader("bfgLFGlare");
	cgs.media.bfgLFDisc = trap_R_RegisterShader("bfgLFDisc");
	cgs.media.bfgLFRing = trap_R_RegisterShader("bfgLFRing");
	cgs.media.bfgLFStar = trap_R_RegisterShader("bfgLFStar");
	cgs.media.bfgLFLine = trap_R_RegisterShader("bfgLFLine");

	memset( cg_weapons, 0, sizeof( cg_weapons ) );

	cgs.media.wakeMarkShader = trap_R_RegisterShader( "wake" );
	cgs.media.dirtPushShader = trap_R_RegisterShader( "DirtPush" );
	cgs.media.dirtPushSkin = trap_R_RegisterSkin( "effects/shockwave/dirtPush.skin" );
	cgs.media.dirtPushModel = trap_R_RegisterModel( "effects/shockwave/dirtPush.md3" );

	cgs.media.hudShader = trap_R_RegisterShaderNoMip( "interface/hud/main.png" );
	cgs.media.chatBackgroundShader = trap_R_RegisterShaderNoMip("chatBox");
	cgs.media.markerAscendShader = trap_R_RegisterShaderNoMip( "interface/hud/markerAscend.png" );
	cgs.media.markerDescendShader = trap_R_RegisterShaderNoMip( "interface/hud/markerDescend.png" );
	cgs.media.breakLimitShader = trap_R_RegisterShaderNoMip( "breakLimit" );
	cgs.media.RadarBlipShader = trap_R_RegisterShaderNoMip( "interface/sense/blip.png" );
	cgs.media.RadarBlipTeamShader = trap_R_RegisterShaderNoMip( "interface/sense/blipteam.png" );
	cgs.media.RadarBurstShader = trap_R_RegisterShaderNoMip( "interface/sense/burst.png" );
	cgs.media.RadarWarningShader = trap_R_RegisterShaderNoMip( "interface/sense/warning.png" );
	cgs.media.RadarMidpointShader = trap_R_RegisterShaderNoMip( "interface/sense/midpoint.png" );
	// END ADDING

	// register the inline models
	cgs.numInlineModels = trap_CM_NumInlineModels();
	for ( i = 1 ; i < cgs.numInlineModels ; i++ ) {
		char	name[10];
		vec3_t			mins, maxs;
		int				j;

		Com_sprintf( name, sizeof(name), "*%i", i );
		cgs.inlineDrawModel[i] = trap_R_RegisterModel( name );
		trap_R_ModelBounds( cgs.inlineDrawModel[i], mins, maxs, 0 );
		for ( j = 0 ; j < 3 ; j++ ) {
			cgs.inlineModelMidpoints[i][j] = mins[j] + 0.5 * ( maxs[j] - mins[j] );
		}
	}

	// register all the server specified models
	for (i=1 ; i<MAX_MODELS ; i++) {
		const char		*modelName;

		modelName = CG_ConfigString( CS_MODELS+i );
		if ( !modelName[0] ) {
			break;
		}
		cgs.gameModels[i] = trap_R_RegisterModel( modelName );
	}
}



/*																																			
=======================
CG_BuildSpectatorString

=======================
*/
void CG_BuildSpectatorString(void) {
	int i;
	cg.spectatorList[0] = 0;
	for (i = 0; i < MAX_CLIENTS; i++) {
		if (cgs.clientinfo[i].infoValid && cgs.clientinfo[i].team == TEAM_SPECTATOR ) {
			Q_strcat(cg.spectatorList, sizeof(cg.spectatorList), va("%s     ", cgs.clientinfo[i].name));
		}
	}
	i = strlen(cg.spectatorList);
	if (i != cg.spectatorLen) {
		cg.spectatorLen = i;
		cg.spectatorWidth = -1;
	}
}


/*																																			
===================
CG_RegisterClients
===================
*/
static void CG_RegisterClients( void ) {
	int		i;

	CG_LoadingClient(cg.clientNum);
	CG_NewClientInfo(cg.clientNum);

	for (i=0 ; i<MAX_CLIENTS ; i++) {
		const char		*clientInfo;

		if (cg.clientNum == i) {
			continue;
		}

		clientInfo = CG_ConfigString( CS_PLAYERS+i );
		if ( !clientInfo[0]) {
			continue;
		}
		CG_LoadingClient( i );
		CG_NewClientInfo( i );
	}
	CG_BuildSpectatorString();
}

//===========================================================================

/*
=================
CG_ConfigString
=================
*/
const char *CG_ConfigString( int index ) {
	if ( index < 0 || index >= MAX_CONFIGSTRINGS ) {
		CG_Error( "CG_ConfigString: bad index: %i", index );
	}
	return cgs.gameState.stringData + cgs.gameState.stringOffsets[ index ];
}



/*
=================
CG_Init

Called after every level change or subsystem restart
Will perform callbacks to make the loading info screen update.
=================
*/
void CG_Init( int serverMessageNum, int serverCommandSequence, int clientNum ) {
	const char	*s;

	// clear everything
	memset( &cgs, 0, sizeof( cgs ) );
	memset( &cg, 0, sizeof( cg ) );
	memset( cg_entities, 0, sizeof(cg_entities) );
	memset( cg_weapons, 0, sizeof(cg_weapons) );

	cg.clientNum = clientNum;

	cgs.processedSnapshotNum = serverMessageNum;
	cgs.serverCommandSequence = serverCommandSequence;

	// load a few needed things before we do any screen updates
	cgs.media.charsetShader		= trap_R_RegisterShader( "interface/fonts/font0.png" );
	cgs.media.whiteShader		= trap_R_RegisterShader( "white" );
	cgs.media.clearShader		= trap_R_RegisterShader( "clear" );
	cgs.media.charsetProp		= trap_R_RegisterShaderNoMip( "interface/fonts/font1.png" );
	cgs.media.charsetPropGlow	= trap_R_RegisterShaderNoMip( "interface/fonts/font1Glow.png" );
	cgs.media.charsetPropB		= trap_R_RegisterShaderNoMip( "interface/fonts/font2.png" );

	CG_RegisterCvars();

	CG_InitConsoleCommands();

	cg.weaponSelect = 1;

	cgs.redflag = cgs.blueflag = -1; // For compatibily, default to unset for
	cgs.flagStatus = -1;			 // old servers

	// get the rendering configuration from the client system
	trap_GetGlconfig( &cgs.glconfig );
	cgs.screenXScale = cgs.glconfig.vidWidth / 640.0;
	cgs.screenYScale = cgs.glconfig.vidHeight / 480.0;

	// get the gamestate from the client system
	trap_GetGameState( &cgs.gameState );

	// check version
	s = CG_ConfigString( CS_GAME_VERSION );
	if ( strcmp( s, GAME_VERSION ) ) {
		CG_Error( "Client/Server game mismatch: %s/%s", GAME_VERSION, s );
	}

	s = CG_ConfigString( CS_LEVEL_START_TIME );
	cgs.levelStartTime = atoi( s );

	CG_ParseServerinfo();

	// load the new map
	CG_LoadingString( "collision map" );

	trap_CM_LoadMap( cgs.mapname );
	cg.loading = qtrue;		// force players to load instead of defer

	CG_LoadingString( "sounds" );

	CG_RegisterSounds();

	CG_LoadingString( "graphics" );

	CG_RegisterGraphics();

	CG_LoadingString( "clients" );

	CG_RegisterClients();		// if low on memory, some clients will be deferred


	cg.loading = qfalse;	// future players will be deferred
	CG_InitLocalEntities();

	// ADDING FOR ZEQ2
	CG_FrameHist_Init();
	CG_InitTrails();
	CG_InitParticleSystems();
	CG_InitBeamTables();
	CG_InitRadarBlips();
	CG_Music_Start();
	// END ADDING

	CG_InitMarkPolys();

	// remove the last loading update
	cg.infoScreenText[0] = 0;

	// Make sure we have update values (scores)
	CG_SetConfigValues();

	CG_LoadingString( "" );
	CG_ShaderStateChanged();

	trap_S_ClearLoopingSounds( qtrue );
}

/*
=================
CG_Shutdown

Called before every level change or subsystem restart
=================
*/
void CG_Shutdown( void ) {
	// some mods may need to do cleanup work here,
	// like closing files or archiving session data
}


/*
==================
CG_EventHandling
==================
 type 0 - no event handling
      1 - team menu
      2 - hud editor

*/
void CG_EventHandling(int type) {
}



void CG_KeyEvent(int key, qboolean down) {
}

void CG_MouseEvent(int x, int y) {
}
