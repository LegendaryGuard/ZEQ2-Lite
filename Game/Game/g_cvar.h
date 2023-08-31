#ifdef EXTERN_G_CVAR
	#define G_CVAR( vmCvar, cvarName, defaultString, cvarFlags, modificationCount, trackChange ) extern vmCvar_t vmCvar;
#endif

#ifdef DECLARE_G_CVAR
	#define G_CVAR( vmCvar, cvarName, defaultString, cvarFlags, modificationCount, trackChange ) vmCvar_t vmCvar;
#endif

#ifdef G_CVAR_LIST
	#define G_CVAR( vmCvar, cvarName, defaultString, cvarFlags, modificationCount, trackChange ) { & vmCvar, cvarName, defaultString, cvarFlags, modificationCount, trackChange },
#endif

// don't override the cheat state set by the system
G_CVAR( g_cheats, "sv_cheats", "", 0, 0, qfalse )

// noset vars
G_CVAR( g_restarted, "g_restarted", "0", CVAR_ROM, 0, qfalse  )
// latched vars
G_CVAR( g_gametype, "g_gametype", "0", CVAR_SERVERINFO | CVAR_USERINFO | CVAR_LATCH, 0, qfalse  )

G_CVAR( g_maxclients, "sv_maxclients", "8", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  ) // allow this many total, including spectators
G_CVAR( g_maxGameClients, "g_maxGameClients", "0", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  ) // allow this many active

// change anytime vars
G_CVAR( g_dmflags, "dmflags", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qtrue  )
G_CVAR( g_fraglimit, "fraglimit", "20", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue )
G_CVAR( g_timelimit, "timelimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue )
G_CVAR( g_capturelimit, "capturelimit", "8", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue )

G_CVAR( g_synchronousClients, "g_synchronousClients", "0", CVAR_SYSTEMINFO, 0, qfalse  )

G_CVAR( g_friendlyFire, "g_friendlyFire", "0", CVAR_ARCHIVE, 0, qtrue  )

G_CVAR( g_teamAutoJoin, "g_teamAutoJoin", "0", CVAR_ARCHIVE, 0, qfalse  )
G_CVAR( g_teamForceBalance, "g_teamForceBalance", "0", CVAR_ARCHIVE, 0, qfalse  )

G_CVAR( g_warmup, "g_warmup", "20", CVAR_ARCHIVE, 0, qtrue  )
G_CVAR( g_doWarmup, "g_doWarmup", "0", CVAR_ARCHIVE, 0, qtrue  )
G_CVAR( g_logfile, "g_log", "games.log", CVAR_ARCHIVE, 0, qfalse  )
G_CVAR( g_logfileSync, "g_logsync", "0", CVAR_ARCHIVE, 0, qfalse  )

G_CVAR( g_password, "g_password", "", CVAR_USERINFO, 0, qfalse  )
G_CVAR( g_banIPs, "g_banIPs", "", CVAR_ARCHIVE, 0, qfalse  )
G_CVAR( g_filterBan, "g_filterBan", "1", CVAR_ARCHIVE, 0, qfalse  )
G_CVAR( g_needpass, "g_needpass", "0", CVAR_SERVERINFO | CVAR_ROM, 0, qfalse )
G_CVAR( g_dedicated, "dedicated", "0", 0, 0, qfalse  )
G_CVAR( g_knockback, "g_knockback", "1000", 0, 0, qtrue  )
G_CVAR( g_quadfactor, "g_quadfactor", "3", 0, 0, qtrue  )
G_CVAR( g_weaponRespawn, "g_weaponrespawn", "5", 0, 0, qtrue  )
G_CVAR( g_weaponTeamRespawn, "g_weaponTeamRespawn", "30", 0, 0, qtrue )
G_CVAR( g_forcerespawn, "g_forcerespawn", "20", 0, 0, qtrue )
G_CVAR( g_inactivity, "g_inactivity", "0", 0, 0, qtrue )
G_CVAR( g_debugMove, "g_debugMove", "0", 0, 0, qfalse )
G_CVAR( g_debugDamage, "g_debugDamage", "0", 0, 0, qfalse )
G_CVAR( g_debugAlloc, "g_debugAlloc", "0", 0, 0, qfalse )
G_CVAR( g_motd, "g_motd", "", 0, 0, qfalse )
G_CVAR( g_blood, "com_blood", "1", 0, 0, qfalse )

G_CVAR( g_podiumDist, "g_podiumDist", "80", 0, 0, qfalse )
G_CVAR( g_podiumDrop, "g_podiumDrop", "70", 0, 0, qfalse )

G_CVAR( g_allowVote, "g_allowVote", "1", CVAR_ARCHIVE, 0, qfalse )
G_CVAR( g_listEntity, "g_listEntity", "0", 0, 0, qfalse )
G_CVAR( g_smoothClients, "g_smoothClients", "1", 0, 0, qfalse )
G_CVAR( pmove_fixed, "pmove_fixed", "0", CVAR_SYSTEMINFO, 0, qfalse )
G_CVAR( pmove_msec, "pmove_msec", "8", CVAR_SYSTEMINFO, 0, qfalse )

G_CVAR( g_rankings, "g_rankings", "0", 0, 0, qfalse )
// ADDING FOR ZEQ2
G_CVAR( g_verboseParse, "g_verboseParse", "0", CVAR_ARCHIVE, 0, qfalse )
G_CVAR( g_powerlevel, "g_powerlevel", "1000", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qtrue  )
G_CVAR( g_powerlevelMaximum, "g_powerlevelMaximum", "32767", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qtrue  )
G_CVAR( g_rolling, "g_rolling", "1", CVAR_ARCHIVE, 0, qtrue )
G_CVAR( g_running, "g_running", "0", CVAR_ARCHIVE, 0, qtrue )
G_CVAR( g_pointGravity, "g_pointGravity", "0", CVAR_ARCHIVE, 0, qtrue )
G_CVAR( g_allowTiers, "g_allowTiers", "1", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
G_CVAR( g_allowScoreboard, "g_allowScoreboard", "0", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
G_CVAR( g_allowSoar, "g_allowSoar", "1", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
G_CVAR( g_allowBoost, "g_allowBoost", "1", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
G_CVAR( g_allowFly, "g_allowFly", "1", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
G_CVAR( g_allowZanzoken, "g_allowZanzoken", "1", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
G_CVAR( g_allowJump, "g_allowJump", "1", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
G_CVAR( g_allowBallFlip, "g_allowBallFlip", "1", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
G_CVAR( g_allowOverheal, "g_allowOverheal", "1", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
G_CVAR( g_allowBreakLimit, "g_allowBreakLimit", "1", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
G_CVAR( g_allowMelee, "g_allowMelee", "1", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
G_CVAR( g_allowLockon, "g_allowLockon", "1", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
G_CVAR( g_allowBlock, "g_allowBlock", "1", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
G_CVAR( g_allowAdvancedMelee, "g_allowAllowAdvancedMelee", "1", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
G_CVAR( g_breakLimitRate, "g_breakLimitRate", "1.0", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qtrue  )
G_CVAR( g_quickTransformCost, "g_quickTransformCost", "0.32", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
G_CVAR( g_quickTransformCostPerTier , "g_quickTransformCostPerTier", "0.08", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
G_CVAR( g_quickZanzokenCost , "g_quickZanzokenCost", "-1.0", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
G_CVAR( g_quickZanzokenDistance , "g_quickZanzokenDistance", "-1.0", CVAR_ARCHIVE | CVAR_SERVERINFO,0,qtrue )
// END ADDING

#undef G_CVAR
