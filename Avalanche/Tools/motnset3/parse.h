#ifndef __parse_h
#define __parse_h

#define 	MAX_LINE_LENGTH	256
#define  PARSER_ERROR(rCode)  ParserError(__FILE__,__LINE__, rCode)

typedef struct parseStruct
	{
		char *name;
		DWORD (*parseProc)(FILE *fp, char *fileName, struct parseStruct *typePtr, DWORD typeLen,
								 DWORD typeIndex, char *curLine, DWORD flags);
	}ts_parse;

typedef struct parseListInf
		{
			ts_parse *listPtr;
			DWORD		 listSize;
		}ts_parseListInf;


#define PARSE_BIN_FOPEN 		0x00000001
#define PARSE_BIN_NO_FOPEN    0x00000002

extern char *numString;
extern char *spaceString;

extern void  ParserError(char *fileName, DWORD lineNum, HRESULT rCode);
extern DWORD ParseFindType(char *curLine, ts_parse *typePtr, DWORD typeLen, DWORD *typeIndex, DWORD startIndex = 0);
extern void  ParseFileTxt(char *fileName, ts_parse *typePtr, DWORD typeLen, DWORD flags, DWORD startIndex = 0, DWORD eofIndex = -1, DWORD unrecognizedIndex = -1);
extern void  ParseFileBin(char *fileName, ts_parse *typePtr, DWORD typeLen, DWORD flags);
extern char  *eatWhite(char *pString);
extern void  printParseError(char *errorString);


#endif