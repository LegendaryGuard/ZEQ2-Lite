// Copyright (C) 2003-2005 RiO
// cg_weapGfxScanner.c -- lexical scanner for ZEQ2's weapon graphics script language.
#include "cg_weapGfxParser.h"
cg_weapGfxField_t cg_weapGfxChargeFields[] = {
	{"model",cg_weapGfxBuffer.chargeModel,CG_weapGfx_ParseString},
	{"skin",cg_weapGfxBuffer.chargeSkin,CG_weapGfx_ParseString},
	{"shader",cg_weapGfxBuffer.chargeShader,CG_weapGfx_ParseString},
	{"animationRange",cg_weapGfxBuffer.chargePercentRange,CG_weapGfx_ParseRange},
	{"size",cg_weapGfxBuffer.chargeSizeRange,CG_weapGfx_ParseRange},
	{"lightColor",cg_weapGfxBuffer.chargeDlightColor,CG_weapGfx_ParseVector},
	{"lightSize",cg_weapGfxBuffer.chargeDlightRadiusRange,CG_weapGfx_ParseRange},
	{"spin",cg_weapGfxBuffer.chargeSpin,CG_weapGfx_ParseVector},
	{"tagTo",cg_weapGfxBuffer.chargeTag,CG_weapGfx_ParseString},
	{"loopFx",cg_weapGfxBuffer.chargeLoopSound,CG_weapGfx_ParseString},
	{"timedFx",cg_weapGfxBuffer.chargeVoice,CG_weapGfx_ParseTimed,MAX_CHARGE_VOICES},
	{"loopParticles",cg_weapGfxBuffer.chargeParticleSystem,CG_weapGfx_ParseString},
	{"",NULL,CG_weapGfx_ParseDummy}
};
cg_weapGfxField_t cg_weapGfxFlashFields[] = {
	{"model",cg_weapGfxBuffer.flashModel,CG_weapGfx_ParseString},
	{"skin",cg_weapGfxBuffer.flashSkin,CG_weapGfx_ParseString},
	{"shader",cg_weapGfxBuffer.flashShader,CG_weapGfx_ParseString},
	{"size",&cg_weapGfxBuffer.flashSize,CG_weapGfx_ParseFloat},
	{"lightColor",cg_weapGfxBuffer.flashDlightColor,CG_weapGfx_ParseVector},
	{"lightSize",&cg_weapGfxBuffer.flashDlightRadius,CG_weapGfx_ParseFloat},
	{"soundFx",cg_weapGfxBuffer.flashSound,CG_weapGfx_ParseList,MAX_FLASH_SOUNDS},
	{"voiceFx",cg_weapGfxBuffer.voiceSound,CG_weapGfx_ParseList,MAX_FLASH_VOICES},
	{"loopFx",cg_weapGfxBuffer.firingSound,CG_weapGfx_ParseString},
	{"onceFx",cg_weapGfxBuffer.flashOnceSound,CG_weapGfx_ParseString},
	{"loopParticles",cg_weapGfxBuffer.firingParticleSystem,CG_weapGfx_ParseString},
	{"particles",cg_weapGfxBuffer.flashParticleSystem,CG_weapGfx_ParseString},
	{"",NULL,CG_weapGfx_ParseDummy}
};
cg_weapGfxField_t cg_weapGfxTrailFields[] = {
	{"shader",cg_weapGfxBuffer.missileTrailShader,CG_weapGfx_ParseString},
	{"size",&cg_weapGfxBuffer.missileTrailRadius,CG_weapGfx_ParseFloat},
	{"spiralShader",cg_weapGfxBuffer.missileTrailSpiralShader,CG_weapGfx_ParseString},
	{"spiralSize",&cg_weapGfxBuffer.missileTrailSpiralRadius,CG_weapGfx_ParseFloat},
	{"spiralOffset",&cg_weapGfxBuffer.missileTrailSpiralOffset,CG_weapGfx_ParseFloat},
	{"",NULL,CG_weapGfx_ParseDummy}
};
cg_weapGfxField_t cg_weapGfxMissileFields[] = {
	{"model",cg_weapGfxBuffer.missileModel,CG_weapGfx_ParseString},
	{"skin",cg_weapGfxBuffer.missileSkin,CG_weapGfx_ParseString},
	{"shader",cg_weapGfxBuffer.missileShader,CG_weapGfx_ParseString},
	{"size",&cg_weapGfxBuffer.missileSize,CG_weapGfx_ParseFloat},
	{"lightColor",cg_weapGfxBuffer.missileDlightColor,CG_weapGfx_ParseVector},
	{"lightSize",&cg_weapGfxBuffer.missileDlightRadius,CG_weapGfx_ParseFloat},
	{"spin",cg_weapGfxBuffer.missileSpin,CG_weapGfx_ParseVector},
	{"loopFx",cg_weapGfxBuffer.missileSound,CG_weapGfx_ParseString},
	{"particles",cg_weapGfxBuffer.missileParticleSystem,CG_weapGfx_ParseString},
	{"",NULL,CG_weapGfx_ParseDummy}
};
cg_weapGfxField_t cg_weapGfxStruggleFields[] = {
	{"model",cg_weapGfxBuffer.missileStruggleModel,CG_weapGfx_ParseString},
	{"skin",cg_weapGfxBuffer.missileStruggleSkin,CG_weapGfx_ParseString},
	{"shader",cg_weapGfxBuffer.missileStruggleShader,CG_weapGfx_ParseString},
	{"",NULL,CG_weapGfx_ParseDummy}
};
cg_weapGfxField_t cg_weapGfxExplosionFields[] = {
	{"model",cg_weapGfxBuffer.explosionModel,CG_weapGfx_ParseString},
	{"skin",cg_weapGfxBuffer.explosionSkin,CG_weapGfx_ParseString},
	{"shader",cg_weapGfxBuffer.explosionShader,CG_weapGfx_ParseString},
	{"size",&cg_weapGfxBuffer.explosionSize,CG_weapGfx_ParseFloat},
	{"lightColor",cg_weapGfxBuffer.explosionDlightColor,CG_weapGfx_ParseVector},
	{"lightSize",&cg_weapGfxBuffer.explosionDlightRadius,CG_weapGfx_ParseFloat},
	{"soundFx",cg_weapGfxBuffer.explosionSound,CG_weapGfx_ParseList,MAX_EXPLOSION_SOUNDS},
	{"duration",&cg_weapGfxBuffer.explosionTime,CG_weapGfx_ParseInt},
	{"shockwaveModel",cg_weapGfxBuffer.shockwaveModel,CG_weapGfx_ParseString},
	{"shockwaveSkin",cg_weapGfxBuffer.shockwaveSkin,CG_weapGfx_ParseString},
	{"markShader",cg_weapGfxBuffer.markShader,CG_weapGfx_ParseString},
	{"markSize",&cg_weapGfxBuffer.markSize,CG_weapGfx_ParseFloat},
	{"noRockDebris",&cg_weapGfxBuffer.noRockDebris,CG_weapGfx_ParseFloat},
	{"particles",cg_weapGfxBuffer.explosionParticleSystem,CG_weapGfx_ParseString},
	{"smokeParticles",cg_weapGfxBuffer.smokeParticleSystem,CG_weapGfx_ParseString},
	{"",NULL,CG_weapGfx_ParseDummy}
};
cg_weapGfxField_t cg_weapGfxHudFields[] = {
	{"icon",cg_weapGfxBuffer.weaponIcon,CG_weapGfx_ParseString},
	{"displayName",cg_weapGfxBuffer.weaponName,CG_weapGfx_ParseString},
	{"",NULL,CG_weapGfx_ParseDummy}
};
cg_weapGfxCategory_t cg_weapGfxCategories[] = {
	{"Charge",cg_weapGfxChargeFields},
	{"Flash",cg_weapGfxFlashFields},
	{"Trail",cg_weapGfxTrailFields},
	{"Missile",cg_weapGfxMissileFields},
	{"Struggle",cg_weapGfxStruggleFields},
	{"Explosion",cg_weapGfxExplosionFields},
	{"Hud",cg_weapGfxHudFields},
	{"",NULL}
};
cg_weapGfxSyntax_t cg_weapGfxSyntax[] = {
	{"=",TOKEN_EQUALS},
	{"+",TOKEN_PLUS},
	{"|",TOKEN_COLON},
	{"{",TOKEN_OPENBLOCK},
	{"}",TOKEN_CLOSEBLOCK},
	{"(",TOKEN_OPENVECTOR},
	{")",TOKEN_CLOSEVECTOR},
	{"[",TOKEN_OPENRANGE},
	{"]",TOKEN_CLOSERANGE},
	{"",-1}
};
cg_weapGfxSyntax_t cg_weapGfxSyntaxKeywords[] = {
	{"import",TOKEN_IMPORT},
	{"private",TOKEN_PRIVATE},
	{"protected",TOKEN_PROTECTED},
	{"public",TOKEN_PUBLIC},
	{"weapon",TOKEN_WEAPON},
	{"true",TOKEN_TRUE},
	{"false",TOKEN_FALSE},
	{"null",TOKEN_NULL},
	{"",-1}
};
qboolean CG_weapGfx_Error(cg_weapGfxError_t errorNr,cg_weapGfxScanner_t* scanner,char* string1,char* string2){
	char* file = scanner->filename;
	int line = scanner->line + 1;
	if(errorNr == ERROR_FILE_NOTFOUND){CG_Printf("^1%s: File not found.\n",file);}
	else if(errorNr == ERROR_FILE_TOOBIG){CG_Printf("^1%s: File exceeds maximum script length.\n",file);}
	else if(errorNr == ERROR_PREMATURE_EOF){CG_Printf("^1%s(%i): Premature end of file.\n",file,line);}
	else if(errorNr == ERROR_STRING_TOOBIG){CG_Printf("^1%s(%i): String exceeds limit of %i characters.\n",file,line,MAX_TOKENSTRING_LENGTH);}
	else if(errorNr == ERROR_TOKEN_TOOBIG){CG_Printf("^1%s(%i): Symbol exceeds limit of %i characters.\n",file,line,MAX_TOKENSTRING_LENGTH);}
	else if(errorNr == ERROR_UNKNOWN_SYMBOL){CG_Printf("^1%s(%i): Unknown symbol '%s' encountered.\n",file,line,string1);}
	else if(errorNr == ERROR_UNEXPECTED_SYMBOL){
		if(!string2){CG_Printf("^1%s(%i): Unexpected symbol '%s' found.\n",file,line,string1);}
		else{CG_Printf("^1%s(%i): Unexpected symbol '%s' found, expected '%s'.\n",file,line,string1,string2);}
	}
	else if(errorNr == ERROR_STRING_EXPECTED){CG_Printf("^1%s(%i): String expected. '%s' is not a string or is missing quotes.\n",file,line,string1);}
	else if(errorNr == ERROR_INTEGER_EXPECTED){CG_Printf("^1%s(%i): Integer expected, but '%s' found.\n",file,line,string1);}
	else if(errorNr == ERROR_FLOAT_EXPECTED){CG_Printf("^1%s(%i): Float or integer expected, but '%s' found.\n",file,line,string1);}
	else if(errorNr == ERROR_IMPORTS_EXCEEDED){CG_Printf("^1%s(%i): Trying to exceed maximum number of %i imports.\n",file,line,MAX_IMPORTS);}
	else if(errorNr == ERROR_IMPORT_REDEFINED){CG_Printf("^1%s(%i): Trying to redefine previously defined import definition '%s'.\n",file,line,string1);}
	else if(errorNr == ERROR_IMPORT_DOUBLED){CG_Printf("^1%s(%i): Trying to duplicate a previously imported definition under the new name '%s'.\n",file,line,string1);}
	else if(errorNr == ERROR_IMPORT_UNDEFINED){CG_Printf("^1%s(%i): Undefined import '%s' being referenced.\n",file,line,string1);}
	else if(errorNr == ERROR_DEFINITIONS_EXCEEDED){CG_Printf("^1%s(%i): Trying to exceed maximum number of %i weapon definitions.\n",file,line,MAX_DEFINES);}
	else if(errorNr == ERROR_DEFINITION_REDEFINED){CG_Printf("^1%s(%i): Trying to redefine previously defined weapon definition '%s'.\n",file,line,string1);}
	else if(errorNr == ERROR_DEFINITION_UNDEFINED){CG_Printf("^1%s(%i): Undefined weapon definition '%s' being referenced.\n",file,line,string1);}
	else if(errorNr == ERROR_REDEFINE_IMPORT_AS_DEFINITION){CG_Printf("^1%s(%i): Trying to redefine previously defined import definition '%s' as a local weapon definition.\n",file,line,string1);}
	else if(errorNr == ERROR_LINK_BOUNDS){CG_Printf("^1%s(%i): Weapon link out of bounds. Must be in range [1..6].\n",file,line);}
	else if(errorNr == ERROR_LINK_REDEFINED){CG_Printf("^1%s(%i): Trying to redefine a previously defined weapon link number.\n",file,line);}
	else if(errorNr == ERROR_FIELD_NOT_IN_CATEGORY){CG_Printf("^1%s(%i): Field '%s' is not supported by category '%s'.\n",file,line,string1,string2);}
	else if(errorNr == ERROR_INVERTED_RANGE){CG_Printf("^1%s(%i): This range doesn't allow end value '%s' to be larger than start value '%s'.\n",file,line,string1,string2);}
	else if(errorNr == ERROR_OVER_MAXBOUND){CG_Printf("^1%s(%i): Value '%s' is larger than maximum bound of %s.\n",file,line,string1,string2);}
	else if(errorNr == ERROR_UNDER_MINBOUND){CG_Printf("^1%s(%i): Value '%s' is smaller than minimum bound of %s.\n",file,line,string1,string2);}
	else if(errorNr == ERROR_OVER_MAXVECTORELEMS){CG_Printf("^1%s(%i): Element '%s' exceeds maximum storage capacity of %s elements in vector.\n",file,line,string1,string2);}
	else if(errorNr == ERROR_MAX_RECURSION){CG_Printf("^1%s: Compiler is trying to read beyond maximum recursion depth %i for overriding.\n",file,MAX_RECURSION_DEPTH);}
	else if(errorNr == ERROR_INHERITING_PRIVATE){CG_Printf("^1%s: Private definition '%s' can not be inherited.\n",file,string1);}
	else if(errorNr == ERROR_IMPORTING_NON_PUBLIC){CG_Printf("^1%s: Non-public definition '%s' can not be imported for local use.\n",file,string1);}
	else if(errorNr == ERROR_OVERRIDING_WITH_HIGHER_ACCESS){CG_Printf("^1%s: Definition '%s' may not be declared with higher access than its superclass.\n",file,string1);}
	else{CG_Printf("^1WEAPONSCRIPT ERROR: Unknown error occured.\n");}
	return qfalse;
}
qboolean CG_weapGfx_NextSym(cg_weapGfxScanner_t* scanner,cg_weapGfxToken_t* token){
	int index = 0;
	int length = 0;
	int categoryIndex = 0;
	char* start = NULL;
	//Skippables
	while(1){
		while(scanner->pos[0] <= ' '){
			if(!scanner->pos[0]){break;}
			if(scanner->pos[0] == '\n'){scanner->line += 1;}
			scanner->pos += 1;
		}
		if(scanner->pos[0] == '/' && scanner->pos[1] == '/'){
			char* newLine = strchr(scanner->pos,'\n');
			scanner->pos = newLine ? newLine : scanner->pos + 2;
		}
		else if(scanner->pos[0] == '/' && scanner->pos[1] == '*'){
			char* endComment = strstr(scanner->pos,"*/");
			scanner->pos = endComment ? endComment + 2 : scanner->pos + 2;
		}
		else{break;}
	}
	//Strings
	if(scanner->pos[0] == '\"'){
		char* endString = strchr(++scanner->pos,'\"');
		length = endString - scanner->pos;
		if(!endString){return token->tokenSym = TOKEN_EOF;}
		if(length >= MAX_TOKENSTRING_LENGTH - 1){
			return CG_weapGfx_Error(ERROR_STRING_TOOBIG,scanner,NULL,NULL);
		}
		Q_strncpyz(token->stringval,scanner->pos,length + 1);
		scanner->pos = endString + 1;
		return token->tokenSym = TOKEN_STRING;
	}
	//Numbers
	if(scanner->pos[0] >= '0' && scanner->pos[0] <= '9'){
		qboolean dot = qfalse;
		start = scanner->pos;
		length = 0;
		while(scanner->pos[0] >= '0' && scanner->pos[0] <= '9'){
			if(length >= MAX_TOKENSTRING_LENGTH-1){
				return CG_weapGfx_Error(ERROR_TOKEN_TOOBIG,scanner,NULL,NULL);
			}
			length = ++scanner->pos - start;
			if(scanner->pos[0] != '.' || dot){continue;}
			if(length >= MAX_TOKENSTRING_LENGTH-1){
				return CG_weapGfx_Error(ERROR_TOKEN_TOOBIG,scanner,NULL,NULL);
			}
			dot = qtrue;
			length = ++scanner->pos - start;
		}
		Q_strncpyz(token->stringval,start,length + 1);
		token->floatval = atof(token->stringval);
		token->intval = ceil(token->floatval);
		return token->tokenSym = dot ? TOKEN_FLOAT : TOKEN_INTEGER;
	}
	//Syntax symbols
	for(;strcmp(cg_weapGfxSyntax[index].symbol,"");++index){
		cg_weapGfxSyntax_t* syntax = &cg_weapGfxSyntax[index];
		if(scanner->pos[0] != syntax->symbol[0]){continue;}
		scanner->pos += 1;
		strcpy(token->stringval,syntax->symbol);
		return token->tokenSym = syntax->tokenType;
	}
	//Keywords
	start = scanner->pos;
	length = 0;
	while((scanner->pos[0] >= 'a' && scanner->pos[0] <= 'z') || (scanner->pos[0] >= 'A' && scanner->pos[0] <= 'Z')){
		if(length > MAX_TOKENSTRING_LENGTH-1){
			return CG_weapGfx_Error(ERROR_TOKEN_TOOBIG,scanner,NULL,NULL);
		}
		length = ++scanner->pos - start;
	}
	Q_strncpyz(token->stringval,start,length + 1);
	for(index=0;strcmp(cg_weapGfxSyntaxKeywords[index].symbol,"");++index){
		cg_weapGfxSyntax_t* syntax = &cg_weapGfxSyntaxKeywords[index];
		if(Q_stricmp(token->stringval,syntax->symbol)){continue;}
		token->tokenSym = syntax->tokenType;
		return qtrue;
	}
	for(;strcmp(cg_weapGfxCategories[categoryIndex].name,"");++categoryIndex){
		cg_weapGfxCategory_t* category = &cg_weapGfxCategories[categoryIndex];
		if(Q_stricmp(token->stringval,category->name)){continue;}
		scanner->category = token->identifierIndex = categoryIndex;
		return token->tokenSym = TOKEN_CATEGORY;
	}
	if(scanner->category >= CAT_CHARGE && scanner->category <= CAT_HUD){
		cg_weapGfxCategory_t* category = &cg_weapGfxCategories[scanner->category];
		int fieldIndex = 0;
		for(;strcmp(category->fields[fieldIndex].name,"");++fieldIndex){
			cg_weapGfxField_t* field = &category->fields[fieldIndex];
			if(Q_stricmp(token->stringval,field->name)){continue;}
			token->identifierIndex = fieldIndex;
			return token->tokenSym = TOKEN_FIELD;
		}
	}
	if(scanner->pos[0] == '\0'){
		return token->tokenSym = TOKEN_EOF;
	}
	return CG_weapGfx_Error(ERROR_UNKNOWN_SYMBOL,scanner,token->stringval,NULL);
}
qboolean CG_weapGfx_Scan(cg_weapGfxScanner_t* scanner,cg_weapGfxToken_t* token){
	if(CG_weapGfx_NextSym(scanner,token)){return qtrue;}
	return token->tokenSym == TOKEN_EOF ? CG_weapGfx_Error(ERROR_PREMATURE_EOF,scanner,NULL,NULL) : qfalse;
}
qboolean CG_weapGfx_LoadFile(cg_weapGfxScanner_t* scanner,char* filename){
	int len;
	qhandle_t file;
	Q_strncpyz(scanner->filename,filename,sizeof(scanner->filename));
	len = trap_FS_FOpenFile(filename,&file,FS_READ);
	if(!file){
		return CG_weapGfx_Error(ERROR_FILE_NOTFOUND,scanner,filename,NULL);
	}
	if(len >= sizeof(char) * MAX_SCRIPT_LENGTH - 1){
		trap_FS_FCloseFile(file);
		return CG_weapGfx_Error(ERROR_FILE_TOOBIG,scanner,NULL,NULL);
	}
	trap_FS_Read(scanner->script,len,file);
	trap_FS_FCloseFile(file);
	scanner->script[len] = '\0';
	scanner->line = 0;
	scanner->pos = scanner->script;
	if(cg_verboseParse.integer){
		CG_Printf("Scriptfile '%s' has been read into memory.\n",filename);
	}
	return qtrue;
}
