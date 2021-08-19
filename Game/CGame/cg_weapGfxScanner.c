// Copyright (C) 2003-2005 RiO
// cg_weapGfxScanner.c -- lexical scanner for ZEQ2's weapon graphics script language.
#include "cg_weapGfxParser.h"
char *cg_weapGfxCategories[] = {"Charge","Explosion","Struggle","Missile","Flash","Trail","Hud",""};
cg_weapGfxField_t cg_weapGfxFields[] = {
	{"model",         CG_weapGfx_ParseModel},          // Charge, Explosion, Struggle, Missile, Flash
	{"skin",          CG_weapGfx_ParseSkin},           // Charge, Explosion, Struggle, Missile, Flash
	{"shader",        CG_weapGfx_ParseShader},         // Charge, Explosion, Struggle, Missile, Flash
	{"animationRange",CG_weapGfx_ParseAnimationRange}, // Charge
	{"size",          CG_weapGfx_ParseSize},           // Charge, Explosion, Missile, Flash, Trail
	{"light",         CG_weapGfx_ParseDlight},         // Charge, Explosion, Missile, Flash
	{"spin",          CG_weapGfx_ParseSpin},           // Charge, Missile
	{"tagTo",         CG_weapGfx_ParseTagTo},          // Charge, Flash
	{"soundFx",       CG_weapGfx_ParseSoundFx},        // Explosion, Flash
	{"voiceFx",       CG_weapGfx_ParseVoiceFx},        // Flash
	{"loopFx",        CG_weapGfx_ParseLoopFx},         // Charge, Missile, Flash
	{"timedFx",       CG_weapGfx_ParseTimedFx},        // Charge, Flash
	{"onceFx",        CG_weapGfx_ParseOnceFx},         // Flash
	{"duration",      CG_weapGfx_ParseDuration},       // Explosion
	{"shockwave",     CG_weapGfx_ParseShockwave},      // Explosion
	{"markShader",    CG_weapGfx_ParseMarkShader},     // Explosion
	{"markSize",      CG_weapGfx_ParseMarkSize},       // Explosion
	{"noRockDebris",  CG_weapGfx_ParseRockDebris},     // Explosion
	{"particles",     CG_weapGfx_ParseParticles},      // Charge, Explosion, Missile, Flash
	{"loopParticles", CG_weapGfx_ParseLoopParticles},  // Charge, Flash
	{"smokeParticles",CG_weapGfx_ParseSmokeParticles}, // Explosion
	{"spiralShader",  CG_weapGfx_ParseSpiralShader},   // Trail
	{"spiralSize",    CG_weapGfx_ParseSpiralSize},     // Trail
	{"spiralOffset",  CG_weapGfx_ParseSpiralOffset},   // Trail
	{"icon",          CG_weapGfx_ParseIcon},           // HUD
	{"displayName",   CG_weapGfx_ParseDisplayName},    // HUD
	{"",              CG_weapGfx_ParseDummy}           // Terminator dummy function
};
// Always returns false to allow one-liner halting of the caller.
qboolean CG_weapGfx_ErrorHandle(cg_weapGfxError_t errorNr,cg_weapGfxScanner_t* scanner,char* string1,char* string2){
	char* file = scanner->filename;
	// <-- Internally we start from 0, for the user we start from 1
	int line = scanner->line + 1;
	if(errorNr == ERROR_FILE_NOTFOUND){CG_Printf("%s: File not found.\n",file);}
	else if(errorNr == ERROR_FILE_TOOBIG){CG_Printf("%s: File exceeds maximum script length.\n",file);}
	else if(errorNr == ERROR_PREMATURE_EOF){CG_Printf("%s(%i): Premature end of file.\n",file,line);}
	else if(errorNr == ERROR_STRING_TOOBIG){CG_Printf("%s(%i): String exceeds limit of %i characters.\n",file,line,MAX_TOKENSTRING_LENGTH);}
	else if(errorNr == ERROR_TOKEN_TOOBIG){CG_Printf("%s(%i): Symbol exceeds limit of %i characters.\n",file,line,MAX_TOKENSTRING_LENGTH);}
	else if(errorNr == ERROR_UNKNOWN_SYMBOL){CG_Printf("%s(%i): Unknown symbol '%s' encountered.\n",file,line,string1);}
	else if(errorNr == ERROR_UNEXPECTED_SYMBOL){
		if(!string2){CG_Printf("%s(%i): Unexpected symbol '%s' found.\n",file,line,string1);}
		else{CG_Printf("%s(%i): Unexpected symbol '%s' found, expected '%s'.\n",file,line,string1,string2);}
	}
	else if(errorNr == ERROR_STRING_EXPECTED){CG_Printf("%s(%i): String expected. '%s' is not a string or is missing quotes.\n",file,line,string1);}
	else if(errorNr == ERROR_INTEGER_EXPECTED){CG_Printf("%s(%i): Integer expected, but '%s' found.\n",file,line,string1);}
	else if(errorNr == ERROR_FLOAT_EXPECTED){CG_Printf("%s(%i): Float or integer expected, but '%s' found.\n",file,line,string1);}
	else if(errorNr == ERROR_IMPORTS_EXCEEDED){CG_Printf("%s(%i): Trying to exceed maximum number of %i imports.\n",file,line,MAX_IMPORTS);}
	else if(errorNr == ERROR_IMPORT_REDEFINED){CG_Printf("%s(%i): Trying to redefine previously defined import definition '%s'.\n",file,line,string1);}
	else if(errorNr == ERROR_IMPORT_DOUBLED){CG_Printf("%s(%i): Trying to duplicate a previously imported definition under the new name '%s'.\n",file,line,string1);}
	else if(errorNr == ERROR_IMPORT_UNDEFINED){CG_Printf("%s(%i): Undefined import '%s' being referenced.\n",file,line,string1);}
	else if(errorNr == ERROR_DEFINITIONS_EXCEEDED){CG_Printf("%s(%i): Trying to exceed maximum number of %i weapon definitions.\n",file,line,MAX_DEFINES);}
	else if(errorNr == ERROR_DEFINITION_REDEFINED){CG_Printf("%s(%i): Trying to redefine previously defined weapon definition '%s'.\n",file,line,string1);}
	else if(errorNr == ERROR_DEFINITION_UNDEFINED){CG_Printf("%s(%i): Undefined weapon definition '%s' being referenced.\n",file,line,string1);}
	else if(errorNr == ERROR_REDEFINE_IMPORT_AS_DEFINITION){CG_Printf("%s(%i): Trying to redefine previously defined import definition '%s' as a local weapon definition.\n",file,line,string1);}
	else if(errorNr == ERROR_LINK_BOUNDS){CG_Printf("%s(%i): Weapon link out of bounds. Must be in range [1..6].\n",file,line);}
	else if(errorNr == ERROR_LINK_REDEFINED){CG_Printf("%s(%i): Trying to redefine a previously defined weapon link number.\n",file,line);}
	else if(errorNr == ERROR_FIELD_NOT_IN_CATEGORY){CG_Printf("%s(%i): Field '%s' is not supported by category '%s'.\n",file,line,string1,string2);}
	else if(errorNr == ERROR_INVERTED_RANGE){CG_Printf("%s(%i): This range doesn't allow end value '%s' to be larger than start value '%s'.\n",file,line,string1,string2);}
	else if(errorNr == ERROR_OVER_MAXBOUND){CG_Printf("%s(%i): Value '%s' is larger than maximum bound of %s.\n",file,line,string1,string2);}
	else if(errorNr == ERROR_UNDER_MINBOUND){CG_Printf("%s(%i): Value '%s' is smaller than minimum bound of %s.\n",file,line,string1,string2);}
	else if(errorNr == ERROR_OVER_MAXVECTORELEMS){CG_Printf("%s(%i): Element '%s' exceeds maximum storage capacity of %s elements in vector.\n",file,line,string1,string2);}
	else if(errorNr == ERROR_MAX_RECURSION){CG_Printf("%s: Compiler is trying to read beyond maximum recursion depth %i for overriding.\n",file,MAX_RECURSION_DEPTH);}
	else if(errorNr == ERROR_INHERITING_PRIVATE){CG_Printf("%s: Private definition '%s' can not be inherited.\n",file,string1);}
	else if(errorNr == ERROR_IMPORTING_NON_PUBLIC){CG_Printf("%s: Non-public definition '%s' can not be imported for local use.\n",file,string1);}
	else if(errorNr == ERROR_OVERRIDING_WITH_HIGHER_ACCESS){CG_Printf("%s: Definition '%s' may not be declared with higher access than its superclass.\n",file,string1);}
	else{CG_Printf("WEAPONSCRIPT ERROR: Unknown error occured.\n");}
	return qfalse;
}
qboolean CG_weapGfx_NextSym(cg_weapGfxScanner_t* scanner,cg_weapGfxToken_t* token){
	// Skip non-relevant characters
	while(1){
		// Leading whitespace
		while(*scanner->pos <= ' '){
			if(!*scanner->pos){break;}
			if(*scanner->pos == '\n'){scanner->line++;}
			scanner->pos++;
		}
		if(scanner->pos[0] == '/' && scanner->pos[1] == '/' ) {
			scanner->pos += 2;
			while(*scanner->pos && *scanner->pos != '\n') {
				scanner->pos++;
			}
		}
		else if(scanner->pos[0] == '/' && scanner->pos[1] == '*'){
			scanner->pos += 2;
			while(*scanner->pos && (scanner->pos[0] != '*' || scanner->pos[1] != '/')){
				scanner->pos++;
			}
			if(*scanner->pos){scanner->pos += 2;}
		}
		else{break;}
	}
	if(*scanner->pos == '\"'){
		int len = 0;
		scanner->pos++;
		while(1){
			if(!(*scanner->pos)){
				return CG_weapGfx_ErrorHandle(ERROR_PREMATURE_EOF,scanner,NULL,NULL);
			}
			if(*scanner->pos == '\n'){scanner->line++;}
			if(*scanner->pos == '\"') {
				scanner->pos++;
				break;
			}
			if(len < MAX_TOKENSTRING_LENGTH-1 && *scanner->pos >= ' '){
				token->stringval[len] = *scanner->pos;
				len++;
			}
			else{
				return CG_weapGfx_ErrorHandle(ERROR_STRING_TOOBIG,scanner,NULL,NULL);
			}
			scanner->pos++;
		}
		token->stringval[len] = '\0';
		token->tokenSym = TOKEN_STRING;
		return qtrue;
	}
	if(*scanner->pos >= '0' && *scanner->pos <= '9'){
		int len = 0;
		qboolean dot = qfalse;
		do{
			if(len < (MAX_TOKENSTRING_LENGTH-1)){
				token->stringval[len] = *scanner->pos;
				len++;
			}
			else{
				return CG_weapGfx_ErrorHandle(ERROR_TOKEN_TOOBIG,scanner,NULL,NULL);
			}
			scanner->pos++;
			if(*scanner->pos == '.' && !dot){
				dot = qtrue;
				if(len < MAX_TOKENSTRING_LENGTH-1){
					token->stringval[len] = *scanner->pos;
					len++;
				}
				else{
					return CG_weapGfx_ErrorHandle(ERROR_TOKEN_TOOBIG,scanner,NULL,NULL);
				}
				scanner->pos++;
			}
		}
		while(*scanner->pos >= '0' && *scanner->pos <= '9');
		token->tokenSym = dot ? TOKEN_FLOAT : TOKEN_INTEGER;
		token->stringval[len] = '\0';
		token->floatval = atof(token->stringval);
		token->intval = ceil(token->floatval);
		return qtrue;
	}
	switch(*scanner->pos){
		case '=':
			scanner->pos++;
			token->stringval[0] = '=';
			token->stringval[1] = '\0';
			token->tokenSym = TOKEN_EQUALS;
			return qtrue;
		break;
		case '+':
			scanner->pos++;
			token->stringval[0] = '+';
			token->stringval[1] = '\0';
			token->tokenSym = TOKEN_PLUS;
			return qtrue;
		break;
		case '|':
			scanner->pos++;
			token->stringval[0] = '|';
			token->stringval[1] = '\0';
			token->tokenSym = TOKEN_COLON;
			return qtrue;
		break;
		case '{':
			scanner->pos++;
			token->stringval[0] = '{';
			token->stringval[1] = '\0';
			token->tokenSym = TOKEN_OPENBLOCK;
			return qtrue;
		break;
		case '}':
			scanner->pos++;
			token->stringval[0] = '}';
			token->stringval[1] = '\0';
			token->tokenSym = TOKEN_CLOSEBLOCK;
			return qtrue;
		break;
		case '(':
			scanner->pos++;
			token->stringval[0] = '(';
			token->stringval[1] = '\0';
			token->tokenSym = TOKEN_OPENVECTOR;
			return qtrue;
		break;
		case ')':
			scanner->pos++;
			token->stringval[0] = ')';
			token->stringval[1] = '\0';
			token->tokenSym = TOKEN_CLOSEVECTOR;
			return qtrue;
		break;
		case '[':
			scanner->pos++;
			token->stringval[0] = '[';
			token->stringval[1] = '\0';
			token->tokenSym = TOKEN_OPENRANGE;
			return qtrue;
		break;
		case ']':
			scanner->pos++;
			token->stringval[0] = ']';
			token->stringval[1] = '\0';
			token->tokenSym = TOKEN_CLOSERANGE;
			return qtrue;
		break;
	}
	// Handle keywords
	{
		int len = 0;
		int i;
		while(*scanner->pos >= 'a' && *scanner->pos <= 'z' || *scanner->pos >= 'A' && *scanner->pos <= 'Z'){
			if(len > MAX_TOKENSTRING_LENGTH-1){
				return CG_weapGfx_ErrorHandle(ERROR_TOKEN_TOOBIG,scanner,NULL,NULL);
			}
			token->stringval[len] = *scanner->pos;
			len++;
			scanner->pos++;
		}
		token->stringval[len] = '\0';
		// Check if the keyword is a category.
		for(i=0;strcmp(cg_weapGfxCategories[i],"");i++){
			if(!Q_stricmp(cg_weapGfxCategories[i],token->stringval)){
				token->identifierIndex = i;
				token->tokenSym = TOKEN_CATEGORY;
				return qtrue;
			}
		}
		// Check if the keyword is a field.
		for(i=0;strcmp(cg_weapGfxFields[i].fieldname,"");i++){
			// NOTE:
			// Need to do this comparison case independantly. Q_stricmp is case
			// independant, so use this instead of standard library function strcmp.
			if(!Q_stricmp(cg_weapGfxFields[i].fieldname,token->stringval)){
				token->identifierIndex = i;
				token->tokenSym = TOKEN_FIELD;
				return qtrue;
			}
		}
		// Check if the keyword is 'import', 'private', 'protected', 'public',
		// 'weapon', 'true', 'false', or 'null'.
		if(!Q_stricmp(token->stringval,"import")){
			token->tokenSym = TOKEN_IMPORT;
			return qtrue;
		}
		if(!Q_stricmp(token->stringval,"private")){
			token->tokenSym = TOKEN_PRIVATE;
			return qtrue;
		}
		if(!Q_stricmp(token->stringval,"protected")){
			token->tokenSym = TOKEN_PROTECTED;
			return qtrue;
		}
		if(!Q_stricmp(token->stringval,"public")){
			token->tokenSym = TOKEN_PUBLIC;
			return qtrue;
		}
		if(!Q_stricmp(token->stringval,"weapon")){
			token->tokenSym = TOKEN_WEAPON;
			return qtrue;
		}
		if(!Q_stricmp(token->stringval,"true")){
			token->tokenSym = TOKEN_TRUE;
			return qtrue;
		}
		if(!Q_stricmp(token->stringval,"false")){
			token->tokenSym = TOKEN_FALSE;
			return qtrue;
		}
		if(!Q_stricmp(token->stringval,"null")){
			token->tokenSym = TOKEN_NULL;
			return qtrue;
		}
	}
	if(*scanner->pos == '\0'){
		token->tokenSym = TOKEN_EOF;
		return qfalse;
	}
	return CG_weapGfx_ErrorHandle(ERROR_UNKNOWN_SYMBOL,scanner,token->stringval,NULL);
}
qboolean CG_weapGfx_CheckPrematureEOF(cg_weapGfxScanner_t* scanner,cg_weapGfxToken_t* token){
	if(CG_weapGfx_NextSym(scanner,token)){return qtrue;}
	return token->tokenSym == TOKEN_EOF ? CG_weapGfx_ErrorHandle(ERROR_PREMATURE_EOF,scanner,NULL,NULL) : qfalse;
}
qboolean CG_weapGfx_LoadFile(cg_weapGfxScanner_t* scanner,char* filename){
	int len;
	qhandle_t file;
	Q_strncpyz(scanner->filename,filename,sizeof(scanner->filename));
	len = trap_FS_FOpenFile( filename, &file, FS_READ );
	if(!file){
		return CG_weapGfx_ErrorHandle(ERROR_FILE_NOTFOUND,scanner,filename,NULL);
	}
	if(len >= sizeof(char) * MAX_SCRIPT_LENGTH - 1){
		trap_FS_FCloseFile(file);
		return CG_weapGfx_ErrorHandle(ERROR_FILE_TOOBIG,scanner,NULL,NULL);
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
