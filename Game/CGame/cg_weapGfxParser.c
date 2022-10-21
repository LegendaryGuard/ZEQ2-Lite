// Copyright (C) 2003-2005 RiO
// cg_weapGfxParser.c -- token parser for ZEQ2's weapon graphics script language.
#include "cg_weapGfxParser.h"
static int cg_weapGfxRecursionDepth;
// FIXME: Can this be a local variable instead, or would it give us
//		  > 32k locals errors in the VM-bytecode compiler?
cg_userWeaponParseBuffer_t cg_weapGfxBuffer;
//=========================================
// Copies buffer to a client's weapon
// configuration, converting filestrings
// into qhandle_t in the process.
//=========================================
void CG_weapGfx_StoreBuffer(int clientNum,int weaponNum){
	cg_userWeapon_t* dest = CG_FindUserWeaponGraphics(clientNum,weaponNum + 1);
	cg_userWeaponParseBuffer_t* src = &cg_weapGfxBuffer;
	int i;
	memset(dest,0,sizeof(cg_userWeapon_t));
	// --< Charge >--
	if(*src->chargeModel){dest->chargeModel = trap_R_RegisterModel(src->chargeModel);}
	if(*src->chargeSkin){dest->chargeSkin = trap_R_RegisterSkin(src->chargeSkin);}
	if(*src->chargeShader){dest->chargeShader = trap_R_RegisterShader(src->chargeShader);}
	if(*src->chargeLoopSound){dest->chargeLoopSound = trap_S_RegisterSound(src->chargeLoopSound,qfalse);}
	for(i=0;i<MAX_CHARGE_VOICES;i++){
		if(!*src->chargeVoice[i].voice){continue;}
		dest->chargeVoice[i].voice = trap_S_RegisterSound(src->chargeVoice[i].voice,qfalse);
		dest->chargeVoice[i].startPct = src->chargeVoice[i].startPct;
	}
	VectorCopy(src->chargeDlightColor,dest->chargeDlightColor);
	VectorCopy(src->chargeSpin,dest->chargeSpin);
	Q_strncpyz(dest->chargeTag[0],src->chargeTag[0],sizeof(dest->chargeTag[0]));
	dest->chargeGrowth = src->chargePercentRange[0] != src->chargePercentRange[1];
	dest->chargePercentRange[0] = src->chargePercentRange[0];
	dest->chargePercentRange[1] = src->chargePercentRange[1];
	dest->chargeSizeRange[0] = src->chargeSizeRange[0];
	dest->chargeSizeRange[1] = src->chargeSizeRange[1];
	dest->chargeDlightRadiusRange[0] = src->chargeDlightRadiusRange[0];
	dest->chargeDlightRadiusRange[1] = src->chargeDlightRadiusRange[1];
	Q_strncpyz(dest->chargeParticleSystem,src->chargeParticleSystem,sizeof(dest->chargeParticleSystem));
	// --< Flash >--
	if(*src->flashModel){dest->flashModel = trap_R_RegisterModel(src->flashModel);}
	if(*src->flashSkin){dest->flashSkin = trap_R_RegisterSkin(src->flashSkin);}
	if(*src->flashShader){dest->flashShader = trap_R_RegisterShader(src->flashShader);}
	if(*src->firingSound){dest->firingSound = trap_S_RegisterSound(src->firingSound,qfalse);}
	if(*src->flashOnceSound){dest->flashOnceSound = trap_S_RegisterSound(src->flashOnceSound,qfalse);}
	for(i=0;i<MAX_FLASH_SOUNDS;i++){
		if(*src->flashSound[i]){dest->flashSound[i] = trap_S_RegisterSound(src->flashSound[i],qfalse);}
	}
	for(i=0;i<MAX_FLASH_VOICES;i++){
		if(*src->voiceSound[i]){dest->voiceSound[i] = trap_S_RegisterSound(src->voiceSound[i],qfalse);}
	}
	VectorCopy(src->flashDlightColor,dest->flashDlightColor);
	dest->flashDlightRadius = src->flashDlightRadius;
	dest->flashSize = src->flashSize;
	Q_strncpyz(dest->flashParticleSystem,src->flashParticleSystem,sizeof(dest->flashParticleSystem));
	Q_strncpyz(dest->firingParticleSystem,src->firingParticleSystem,sizeof(dest->firingParticleSystem));	
	// --< Explosion >--
	if(*src->explosionModel){dest->explosionModel = trap_R_RegisterModel(src->explosionModel);}
	if(*src->explosionSkin){dest->explosionSkin = trap_R_RegisterSkin(src->explosionSkin);}
	if(*src->explosionShader){dest->explosionShader = trap_R_RegisterShader(src->explosionShader);}
	if(*src->shockwaveModel){dest->shockwaveModel = trap_R_RegisterModel(src->shockwaveModel);}
	if(*src->shockwaveSkin){dest->shockwaveSkin = trap_R_RegisterSkin(src->shockwaveSkin);}
	if(*src->markShader){dest->markShader = trap_R_RegisterShader(src->markShader);}
	for(i=0;i<MAX_EXPLOSION_SOUNDS;i++){
		if(*src->explosionSound[i]){dest->explosionSound[i] = trap_S_RegisterSound(src->explosionSound[i],qfalse);}
	}
	VectorCopy(src->explosionDlightColor,dest->explosionDlightColor);
	dest->explosionDlightRadius = src->explosionDlightRadius;
	dest->explosionSize = src->explosionSize;
	dest->explosionTime = src->explosionTime;
	dest->markSize = src->markSize;
	dest->noRockDebris = src->noRockDebris;
	Q_strncpyz(dest->smokeParticleSystem,src->smokeParticleSystem,sizeof(dest->smokeParticleSystem));
	Q_strncpyz(dest->explosionParticleSystem,src->explosionParticleSystem,sizeof(dest->explosionParticleSystem));
	// --< Missile >--
	if(*src->missileModel){dest->missileModel = trap_R_RegisterModel(src->missileModel);}
	if(*src->missileSkin){dest->missileSkin = trap_R_RegisterSkin(src->missileSkin);}
	if(*src->missileShader){dest->missileShader = trap_R_RegisterShader(src->missileShader);}
	// --< Missile Struggle >--
	if(*src->missileStruggleModel){dest->missileStruggleModel = trap_R_RegisterModel(src->missileStruggleModel);}
	if(*src->missileStruggleSkin){dest->missileStruggleSkin = trap_R_RegisterSkin(src->missileStruggleSkin);}
	if(*src->missileStruggleShader){dest->missileStruggleShader = trap_R_RegisterShader(src->missileStruggleShader);}
	if(*src->missileSound){dest->missileSound = trap_S_RegisterSound(src->missileSound,qfalse);}
	VectorCopy(src->missileSpin,dest->missileSpin);
	VectorCopy(src->missileDlightColor,dest->missileDlightColor);
	dest->missileDlightRadius = src->missileDlightRadius;
	dest->missileSize = src->missileSize;
	Q_strncpyz(dest->missileParticleSystem,src->missileParticleSystem,sizeof(dest->missileParticleSystem));
	// --< Trail >--
	if(*src->missileTrailShader){dest->missileTrailShader = trap_R_RegisterShader(src->missileTrailShader);}
	if(*src->missileTrailSpiralShader){dest->missileTrailSpiralShader = trap_R_RegisterShader(src->missileTrailSpiralShader);}
	dest->missileTrailRadius = src->missileTrailRadius;
	dest->missileTrailSpiralRadius = src->missileTrailSpiralRadius;
	dest->missileTrailSpiralOffset = src->missileTrailSpiralOffset;
	// --< Hud >--
	if(*src->weaponIcon){dest->weaponIcon = trap_R_RegisterShaderNoMip(src->weaponIcon);}
	Q_strncpyz(dest->weaponName,src->weaponName,sizeof(dest->weaponName));
}
//=========================================
// A special dummy function used in the terminator of
// the cg_weapGfxFields list. Never actually used, and
// if it _would_ be called, it would always hault
// parsing.
//=========================================
qboolean CG_weapGfx_ParseDummy(cg_weapGfxParser_t* parser,void* field,int listLimit){return qfalse;}
qboolean CG_weapGfx_ParseInt(cg_weapGfxParser_t* parser,void* field,int listLimit){
	cg_weapGfxScanner_t* scanner = &parser->scanner;
	cg_weapGfxToken_t* token = &parser->token;
	if(token->tokenSym != TOKEN_INTEGER && token->tokenSym != TOKEN_FLOAT){
		return CG_weapGfx_Error(ERROR_FLOAT_EXPECTED,scanner,token->stringval,NULL);
	}
	*(int*)field = token->tokenSym == TOKEN_INTEGER ? token->intval : token->floatval;
	return qtrue;
}
qboolean CG_weapGfx_ParseFloat(cg_weapGfxParser_t* parser,void* field,int listLimit){
	cg_weapGfxScanner_t* scanner = &parser->scanner;
	cg_weapGfxToken_t* token = &parser->token;
	if(token->tokenSym != TOKEN_INTEGER && token->tokenSym != TOKEN_FLOAT){
		return CG_weapGfx_Error(ERROR_FLOAT_EXPECTED,scanner,token->stringval,NULL);
	}
	*(float*)field = token->tokenSym == TOKEN_INTEGER ? token->intval : token->floatval;
	return qtrue;
}
qboolean CG_weapGfx_ParseString(cg_weapGfxParser_t* parser,void* field,int listLimit){
	cg_weapGfxScanner_t* scanner = &parser->scanner;
	cg_weapGfxToken_t* token = &parser->token;
	if(token->tokenSym == TOKEN_NULL){
		*token->stringval = '\0';
	}
	else if(token->tokenSym != TOKEN_STRING){
		return CG_weapGfx_Error(ERROR_STRING_EXPECTED,scanner,token->stringval,NULL);
	}
	Q_strncpyz(field,token->stringval,MAX_QPATH);
	return qtrue;
}
qboolean CG_weapGfx_ParseRange(cg_weapGfxParser_t* parser,void* field,int listLimit){
	cg_weapGfxScanner_t* scanner = &parser->scanner;
	cg_weapGfxToken_t* token = &parser->token;
	float* start = field;
	float* end = start + 1;
	if(token->tokenSym == TOKEN_INTEGER || token->tokenSym == TOKEN_FLOAT){
		qboolean result = CG_weapGfx_ParseFloat(parser,start,listLimit);
		*end = *start;
		return result;
	}
	if(token->tokenSym != TOKEN_OPENRANGE){
		return CG_weapGfx_Error(ERROR_UNEXPECTED_SYMBOL,scanner,token->stringval,"[");
	}
	if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
	CG_weapGfx_ParseFloat(parser,start,listLimit);
	if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
	CG_weapGfx_ParseFloat(parser,end,listLimit);
	if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
	if(token->tokenSym != TOKEN_CLOSERANGE){
		return CG_weapGfx_Error(ERROR_UNEXPECTED_SYMBOL,scanner,token->stringval,"]");
	}
	return qtrue;
}
qboolean CG_weapGfx_ParseVector(cg_weapGfxParser_t* parser,void* field,int listLimit){
	cg_weapGfxScanner_t* scanner = &parser->scanner;
	cg_weapGfxToken_t* token = &parser->token;
	float* vector = field;
	if(token->tokenSym != TOKEN_OPENVECTOR){
		return CG_weapGfx_Error(ERROR_UNEXPECTED_SYMBOL,scanner,token->stringval,"(");
	}
	if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
	CG_weapGfx_ParseFloat(parser,vector,listLimit);
	if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
	CG_weapGfx_ParseFloat(parser,vector+1,listLimit);
	if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
	CG_weapGfx_ParseFloat(parser,vector+2,listLimit);
	if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
	if(token->tokenSym != TOKEN_CLOSEVECTOR){
		return CG_weapGfx_Error(ERROR_UNEXPECTED_SYMBOL,scanner,token->stringval,")");
	}
	return qtrue;
}
qboolean CG_weapGfx_ParseList(cg_weapGfxParser_t* parser,void* field,int listLimit){
	cg_weapGfxScanner_t* scanner = &parser->scanner;
	cg_weapGfxToken_t* token = &parser->token;
	char* name = field;
	int iteration = 0;
	if(token->tokenSym == TOKEN_STRING){
		return CG_weapGfx_ParseString(parser,name,listLimit);
	}
	if(token->tokenSym != TOKEN_OPENVECTOR){
		return CG_weapGfx_Error(ERROR_UNEXPECTED_SYMBOL,scanner,token->stringval,"(");
	}
	if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
	while(token->tokenSym == TOKEN_STRING){
		if(iteration > listLimit-1){
			return CG_weapGfx_Error(ERROR_OVER_MAXVECTORELEMS,scanner,token->stringval,va("%d",listLimit));
		}
		CG_weapGfx_ParseString(parser,name,listLimit);
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		name += MAX_QPATH;
		iteration += 1;
	}
	if(token->tokenSym != TOKEN_CLOSEVECTOR){
		return CG_weapGfx_Error(ERROR_UNEXPECTED_SYMBOL,scanner,token->stringval,")");
	}
	return qtrue;
}
qboolean CG_weapGfx_ParseTimed(cg_weapGfxParser_t* parser,void* field,int listLimit){
	cg_weapGfxScanner_t* scanner = &parser->scanner;
	cg_weapGfxToken_t* token = &parser->token;
	chargeVoiceParseBuffer_t* voice = field;
	int iteration = 0;
	if(token->tokenSym != TOKEN_OPENVECTOR){
		return CG_weapGfx_Error(ERROR_UNEXPECTED_SYMBOL,scanner,token->stringval,"(");
	}
	if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
	while(token->tokenSym == TOKEN_INTEGER || token->tokenSym == TOKEN_FLOAT){
		if(iteration > listLimit-1){
			return CG_weapGfx_Error(ERROR_OVER_MAXVECTORELEMS,scanner,token->stringval,va("%d",listLimit));
		}
		CG_weapGfx_ParseFloat(parser,&voice->startPct,listLimit);
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		CG_weapGfx_ParseString(parser,&voice->voice,listLimit);
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		voice += 1;
		iteration += 1;
	}
	if(token->tokenSym != TOKEN_CLOSEVECTOR){
		return CG_weapGfx_Error(ERROR_UNEXPECTED_SYMBOL,scanner,token->stringval,")");
	}
	return qtrue;
}
//============================================
// FIXED FUNCTIONS - Do not change the below.
//============================================
// Stores import definitions in a reference list.
// Syntax: 'import' "<refname>" '=' "<filename>" "<defname>"
static qboolean CG_weapGfx_ParseImports(cg_weapGfxParser_t* parser){
	cg_weapGfxScanner_t* scanner = &parser->scanner;
	cg_weapGfxToken_t* token = &parser->token;
	char refname[MAX_TOKENSTRING_LENGTH];
	char filename[MAX_TOKENSTRING_LENGTH];
	while(token->tokenSym == TOKEN_IMPORT){
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		if(token->tokenSym != TOKEN_STRING){
			return CG_weapGfx_Error(ERROR_STRING_EXPECTED,scanner,token->stringval,NULL);
		}
		Q_strncpyz(refname,token->stringval,sizeof(refname));
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		if(token->tokenSym != TOKEN_EQUALS){
			return CG_weapGfx_Error(ERROR_UNEXPECTED_SYMBOL,scanner,token->stringval,"=");
		}
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		if(token->tokenSym != TOKEN_STRING){
			return CG_weapGfx_Error(ERROR_STRING_EXPECTED,scanner,token->stringval,NULL);
		}
		Q_strncpyz(filename,token->stringval,sizeof(filename));
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		if(token->tokenSym != TOKEN_STRING){
			return CG_weapGfx_Error(ERROR_STRING_EXPECTED,scanner,token->stringval,NULL);
		}
		if(!CG_weapGfx_AddImportRef(parser,refname,filename,token->stringval)){
			return qfalse;
		}
		// NOTE: Do it like this to prevent errors if a file happens to only contain
		//       import lines. While it is actually useless to have such a file, it
		//       still is syntacticly correct.
		if(!CG_weapGfx_NextSym(scanner,token) && token->tokenSym != TOKEN_EOF){
			return qfalse;
		}
	}
	return qtrue;
}
static qboolean CG_weapGfx_ParseFields(cg_weapGfxParser_t* parser){
	cg_weapGfxScanner_t* scanner = &parser->scanner;
	cg_weapGfxToken_t* token = &parser->token;
	while(token->tokenSym == TOKEN_FIELD){
		int fieldIndex = token->identifierIndex;
		cg_weapGfxCategory_t* category = &cg_weapGfxCategories[scanner->category];
		cg_weapGfxField_t* field = &category->fields[fieldIndex];
		if(!Q_stricmp(field->name,"")){
			return CG_weapGfx_Error(ERROR_FIELD_NOT_IN_CATEGORY,scanner,token->stringval,category->name);
		}
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		if(token->tokenSym != TOKEN_EQUALS){
			return CG_weapGfx_Error(ERROR_UNEXPECTED_SYMBOL,scanner,token->stringval,"=");
		}
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		if(!field->Parse(parser,field->field,field->listIterations)){return qfalse;}
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
	}
	return qtrue;
}
// Syntax: <categoryname> '{' <HANDLE FIELDS> '}'
static qboolean CG_weapGfx_ParseCategories(cg_weapGfxParser_t* parser){
	cg_weapGfxScanner_t* scanner = &parser->scanner;
	cg_weapGfxToken_t* token = &parser->token;
	while(token->tokenSym == TOKEN_CATEGORY){
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		if(token->tokenSym != TOKEN_OPENBLOCK){
			return CG_weapGfx_Error(ERROR_UNEXPECTED_SYMBOL,scanner,token->stringval,"{");
		}
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		if(!CG_weapGfx_ParseFields(parser)){
			return qfalse;
		}
		scanner->category = -1;
		if(token->tokenSym != TOKEN_CLOSEBLOCK){
			return CG_weapGfx_Error(ERROR_UNEXPECTED_SYMBOL,scanner,token->stringval,"}");
		}
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
	}
	if(token->tokenSym != TOKEN_CLOSEBLOCK){
		return CG_weapGfx_Error(ERROR_UNEXPECTED_SYMBOL,scanner,token->stringval,"}");
	}
	if(cg_verboseParse.integer){
		CG_Printf("Processed categories succesfully.\n");
	}
	return qtrue;
}
// The contents of a definition block are checked for validity
// by the lexical scanner and parser to make sure syntax is correct.
// The parsed values however, are not yet stored, because we don't
// have a link to an actual weapon yet. Entry points into the definitions
// are cached into a table for a quick jump once
// we find out the links in CG_weapGfx_ParseLinks.
// Syntax: ( 'public' | 'protected' | 'private' ) "<refname>" ( e | '=' ( "<importref>" | "<definitionref>" ) ) '{' <HANDLE CATEGORIES> '}'
static qboolean CG_weapGfx_PreParseDefinitions(cg_weapGfxParser_t* parser){
	cg_weapGfxScanner_t* scanner = &parser->scanner;
	cg_weapGfxToken_t* token = &parser->token;
	int defline;
	char* defpos;
	qboolean hasSuper;
	char supername[MAX_TOKENSTRING_LENGTH];
	char refname[MAX_TOKENSTRING_LENGTH];
	int blockCount;
	while(token->tokenSym == TOKEN_PRIVATE || token->tokenSym == TOKEN_PUBLIC || token->tokenSym == TOKEN_PROTECTED){
		cg_weapGfxAccessLvls_t accessLvl = LVL_PRIVATE;
		if(token->tokenSym == TOKEN_PUBLIC){accessLvl = LVL_PUBLIC;}
		if(token->tokenSym == TOKEN_PROTECTED){accessLvl = LVL_PROTECTED;}
		hasSuper = qfalse;
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		if(token->tokenSym != TOKEN_STRING){
			return CG_weapGfx_Error(ERROR_STRING_EXPECTED,scanner,token->stringval,NULL);
		}
		Q_strncpyz(refname,token->stringval,sizeof(refname));
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		// Are we deriving?
		if(token->tokenSym == TOKEN_EQUALS){
			hasSuper = qtrue;
			if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
			if(token->tokenSym != TOKEN_STRING){
				return CG_weapGfx_Error(ERROR_STRING_EXPECTED,scanner,token->stringval,NULL);
			}
			if(!CG_weapGfx_FindDefinitionRef(parser,token->stringval)){
				return qfalse;
			}
			Q_strncpyz(supername,token->stringval,sizeof(supername));
			if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		}
		if(token->tokenSym != TOKEN_OPENBLOCK){
			return CG_weapGfx_Error(ERROR_UNEXPECTED_SYMBOL,scanner,token->stringval,"{");
		}
		blockCount = 1;
		defpos = scanner->pos;
		defline = scanner->line;
		if(!CG_weapGfx_AddDefinitionRef(parser,refname,defpos,defline,accessLvl,hasSuper,supername)){
			return qfalse;
		}
		while(blockCount > 0){
			if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
			if(token->tokenSym == TOKEN_OPENBLOCK){blockCount += 1;}
			if(token->tokenSym == TOKEN_CLOSEBLOCK){
				scanner->category = -1;
				blockCount -= 1;
			}
		}
		// NOTE: This makes sure we don't get an error if a file contains no weapon link
		//       lines, but instead terminates after the definitions. This is what one
		//       would expect of 'library' files.
		if(!CG_weapGfx_NextSym(scanner,token) && token->tokenSym != TOKEN_EOF){
			return qfalse;
		}
	}
	return qtrue;
}
// Parses weapon links to definitions and continues to parse the actual weapon definitions.
// Syntax: 'weapon' <int> '=' "<refname>" ( e | '|' "<refname>" )
static qboolean CG_weapGfx_ParseLinks(cg_weapGfxParser_t* parser){
	cg_weapGfxScanner_t* scanner = &parser->scanner;
	cg_weapGfxToken_t* token = &parser->token;
	int weaponNum;
	char pri_refname[MAX_TOKENSTRING_LENGTH];
	char sec_refname[MAX_TOKENSTRING_LENGTH];
	while(token->tokenSym == TOKEN_WEAPON){
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		if(token->tokenSym != TOKEN_INTEGER){
			return CG_weapGfx_Error(ERROR_INTEGER_EXPECTED,scanner,token->stringval,NULL);
		}
		weaponNum = token->intval;
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		if(token->tokenSym != TOKEN_EQUALS){
			return CG_weapGfx_Error(ERROR_UNEXPECTED_SYMBOL,scanner,token->stringval,"=");
		}
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		if(token->tokenSym != TOKEN_STRING){
			return CG_weapGfx_Error(ERROR_STRING_EXPECTED,scanner,token->stringval,NULL);
		}
		if(!CG_weapGfx_FindDefinitionRef(parser,token->stringval)){
			return CG_weapGfx_Error(ERROR_DEFINITION_UNDEFINED,scanner,token->stringval,NULL);
		}
		Q_strncpyz(pri_refname,token->stringval,sizeof(pri_refname));
		// NOTE: This makes sure we don't get an error if the last link in the file contains
		//       no secondary definition, but instead terminates after the primary one.
		if(!CG_weapGfx_NextSym(scanner,token) && token->tokenSym != TOKEN_EOF){
			return qfalse;
		}
		if(token->tokenSym == TOKEN_COLON){
			if(!CG_weapGfx_NextSym(scanner,token)){
				return qfalse;
			}
			if(token->tokenSym != TOKEN_STRING){
				return CG_weapGfx_Error(ERROR_STRING_EXPECTED,scanner,token->stringval,NULL);
			}
			if(!CG_weapGfx_FindDefinitionRef(parser,token->stringval)){
				return CG_weapGfx_Error(ERROR_DEFINITION_UNDEFINED,scanner,token->stringval,NULL);
			}
			Q_strncpyz(sec_refname,token->stringval,sizeof(sec_refname));
			// NOTE: This makes sure we don't get an error if this is the last link in the file.
			if(!CG_weapGfx_NextSym(scanner,token) && token->tokenSym != TOKEN_EOF){
				return qfalse;
			}
		}
		else{
			Q_strncpyz(sec_refname,"",sizeof(sec_refname));
		}
		if(!CG_weapGfx_AddLinkRef(parser,weaponNum,pri_refname,sec_refname)){
			return qfalse;
		}
	}
	return qtrue;
}
static qboolean CG_weapGfx_IncreaseRecursionDepth(void){
	if(cg_weapGfxRecursionDepth == MAX_RECURSION_DEPTH){
		return qfalse;
	}
	cg_weapGfxRecursionDepth += 1;
	return qtrue;
}
static void CG_weapGfx_DecreaseRecursionDepth(void){cg_weapGfxRecursionDepth--;}
qboolean CG_weapGfx_ParseDefinition(cg_weapGfxParser_t* parser,char* refname,cg_weapGfxAccessLvls_t* accessLvl);
// Instantiates a new parser and scanner to parse a remote definition.
qboolean CG_weapGfx_ParseRemoteDefinition(char* filename,char* refname){
	cg_weapGfxParser_t parser;
	cg_weapGfxScanner_t* scanner;
	cg_weapGfxToken_t* token;
	int i;
	memset(&parser,0,sizeof(parser));
	scanner = &parser.scanner;
	token = &parser.token;	
	CG_weapGfx_LoadFile(scanner,filename);
	// Get the very first token initialized. If
	// it is an end of file token, we will not parse
	// the empty file but will instead exit with true.
	if(!CG_weapGfx_NextSym(scanner,token)){return token->tokenSym == TOKEN_EOF;}
	if(!CG_weapGfx_ParseImports(&parser)){return qfalse;}
	if(!CG_weapGfx_PreParseDefinitions(&parser)){return qfalse;}
	// NOTE: We don't really need to do this, but it does
	//       ensure file structure.
	if(!CG_weapGfx_ParseLinks(&parser)){return qfalse;}
	// Respond with an error if something is trailing the
	// link definitions.
	// NOTE: We don't really need to do this, but it does
	//       ensure file structure.
	if(token->tokenSym != TOKEN_EOF){return CG_weapGfx_Error(ERROR_UNEXPECTED_SYMBOL,scanner,token->stringval,NULL);}
	// If we're dealing with a local definition in this file, then that definition
	// MUST be public, since we're importing it to another file.
	i = CG_weapGfx_FindDefinitionRef(&parser,refname) - 1;
	if(i < MAX_DEFINES && parser.definitionRef[i].accessLvl != LVL_PUBLIC){
		scanner->line = parser.definitionRef[i].scannerLine;
		return CG_weapGfx_Error(ERROR_IMPORTING_NON_PUBLIC,scanner,parser.definitionRef[i].refname,NULL);
	}
	return CG_weapGfx_ParseDefinition(&parser,refname,NULL);
}
// Takes inheritance into account.
qboolean CG_weapGfx_ParseDefinition(cg_weapGfxParser_t* parser,char* refname,cg_weapGfxAccessLvls_t* accessLvl){
	int i = CG_weapGfx_FindDefinitionRef(parser,refname) - 1;
	cg_weapGfxScanner_t* scanner = &parser->scanner;
	cg_weapGfxToken_t* token = &parser->token;
	// <-- Incase there IS no last access level from a super class
	cg_weapGfxAccessLvls_t lastAccessLvl = LVL_PUBLIC;
	if(i < MAX_DEFINES){
		// local declaration
		if(parser->definitionRef[i].hasSuper){
			if(!CG_weapGfx_IncreaseRecursionDepth()){
				return CG_weapGfx_Error(ERROR_MAX_RECURSION,scanner,NULL,NULL);
			}
			if(cg_verboseParse.integer){
				CG_Printf("Inheriting superclass '%s'\n",parser->definitionRef[i].supername);
			}
			if(!CG_weapGfx_ParseDefinition(parser,parser->definitionRef[i].supername,&lastAccessLvl)){
				return qfalse;
			}
			CG_weapGfx_DecreaseRecursionDepth();
		}
		if(lastAccessLvl == LVL_PRIVATE){
			return CG_weapGfx_Error(ERROR_INHERITING_PRIVATE,scanner,parser->definitionRef[i].supername,NULL);
		}
		// Reposition the lexical scanner
		scanner->pos = parser->definitionRef[i].scannerPos;
		scanner->line = parser->definitionRef[i].scannerLine;
		// Check if we're not breaking access level hierarchy
		if(accessLvl){
			*accessLvl = parser->definitionRef[i].accessLvl;
			if(*accessLvl < lastAccessLvl){
				return CG_weapGfx_Error(ERROR_OVERRIDING_WITH_HIGHER_ACCESS,scanner,parser->definitionRef[i].refname,NULL);
			}
		}
		// Skip the '{' opening brace of the definition block, and align to the first real
		// symbol in the block.
		if(!CG_weapGfx_Scan(scanner,token)){return qfalse;}
		if(!CG_weapGfx_ParseCategories(parser)){return qfalse;}
	}
	else{
		// imported declaration
		// First subtract the offset we added to detect difference between
		// an imported and a local definition.
		i -= MAX_DEFINES;
		if(!CG_weapGfx_IncreaseRecursionDepth()){
			return CG_weapGfx_Error(ERROR_MAX_RECURSION,scanner,NULL,NULL);
		}
		if(cg_verboseParse.integer) {
			CG_Printf("Importing '%s'\n",refname);
		}
		if(!CG_weapGfx_ParseRemoteDefinition(parser->importRef[i].filename,parser->importRef[i].defname)){
			return qfalse;
		}
		CG_weapGfx_DecreaseRecursionDepth();
	}
	return qtrue;
}
// System's entry point.
qboolean CG_weapGfx_Parse(char* filename,int clientNum){
	cg_weapGfxParser_t parser;
	cg_weapGfxScanner_t* scanner;
	cg_weapGfxToken_t* token;
	int i;
	memset(&parser,0,sizeof(parser));
	scanner = &parser.scanner;
	token = &parser.token;
	cg_weapGfxRecursionDepth = 0;
	scanner->category = -1;
	CG_weapGfx_LoadFile(scanner,filename);
	// Get the very first token initialized. If
	// it is an end of file token, we will not parse
	// the empty file but will instead exit with true.
	if(!CG_weapGfx_NextSym(scanner,token)){return token->tokenSym == TOKEN_EOF;}
	if(!CG_weapGfx_ParseImports(&parser)){return qfalse;}
	if(!CG_weapGfx_PreParseDefinitions(&parser)){return qfalse;}
	if(!CG_weapGfx_ParseLinks(&parser)){return qfalse;}
	// Respond with an error if something is trailing the
	// link definitions.
	if(token->tokenSym != TOKEN_EOF){return CG_weapGfx_Error(ERROR_UNEXPECTED_SYMBOL,scanner,token->stringval,NULL);}
	for(i=0;i<MAX_LINKS;i++){
		if(!parser.linkRef[i].active){continue;}
		memset(&cg_weapGfxBuffer,0,sizeof(cg_weapGfxBuffer));
		if(cg_verboseParse.integer){
			CG_Printf("Processing weapon nr %i, primary '%s'.\n",i+1,parser.linkRef[i].pri_refname);
		}
		if(!CG_weapGfx_ParseDefinition(&parser,parser.linkRef[i].pri_refname,NULL)){
			return qfalse;
		}
		CG_weapGfx_StoreBuffer(clientNum,i);
		memset(&cg_weapGfxBuffer,0,sizeof(cg_weapGfxBuffer));
		if(strcmp(parser.linkRef[i].sec_refname,"")){
			if(cg_verboseParse.integer){
				CG_Printf("Processing weapon nr %i, secondary '%s'.\n",i+1,parser.linkRef[i].sec_refname);
			}
			if(!CG_weapGfx_ParseDefinition(&parser,parser.linkRef[i].sec_refname,NULL)){
				return qfalse;
			}
		}
		CG_weapGfx_StoreBuffer(clientNum,i+ALTWEAPON_OFFSET);
	}
	if(cg_verboseParse.integer){
		CG_Printf("Parse completed succesfully.\n");
	}
	return qtrue;
}
