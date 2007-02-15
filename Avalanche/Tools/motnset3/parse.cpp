#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "parse.h"

char     *numString 		= "+-0123456789";
char		*spaceString 	= "; ,	|\n\r";     // semi-colon, comma, space, tab.
char 		*GetFileName(char *file);


void ParseFileTxt(char *fileName, ts_parse *typePtr, DWORD typeLen, DWORD flags, DWORD startIndex, DWORD eofIndex, DWORD unrecognizedIndex)
{
	FILE		*fp;
	char 		curLine[MAX_LINE_LENGTH];
	DWORD		rCode;
	DWORD		typeIndex;

	fp = fopen(fileName, "rt");
	if(fp == NULL)
	{
		char temp[256];
		sprintf(temp, "Can't open file %s", fileName);
		printParseError(temp);
		return;
	}

	while(fgets(curLine, MAX_LINE_LENGTH, fp) != NULL)
	{
		if(ParseFindType(curLine, typePtr, typeLen, &typeIndex, startIndex) == TRUE)
		{
			if(typePtr[typeIndex].parseProc != NULL)
			{
				rCode = typePtr[typeIndex].parseProc(fp, GetFileName(fileName), typePtr, typeLen, typeIndex, curLine,0);
				if(rCode != TRUE)
					PARSER_ERROR(rCode);
			}
		}
		else
		{
			//unrecognized command - try parsing it with the unreocgnized command if one is defined
			char *line = eatWhite(curLine);//ensure there is data on the line to try to parse as a filename
			if( line && strlen(line) && (strlen(curLine) > 2) && (unrecognizedIndex >= 0) )
			{
				rCode = typePtr[unrecognizedIndex].parseProc(fp, GetFileName(fileName), typePtr, typeLen, unrecognizedIndex, curLine,0);
				if(rCode != TRUE)
					PARSER_ERROR(rCode);
			}
	
			
		}
	}

	//call the clean-up routine if one is defined
	if(eofIndex >= 0)
	{
		rCode = typePtr[eofIndex].parseProc(fp, GetFileName(fileName), typePtr, typeLen, eofIndex, curLine,0);
		if(rCode != TRUE)
			PARSER_ERROR(rCode);
	}
	
	fclose(fp);
	return;
}

DWORD ParseFindType(char *curLine, ts_parse *typePtr, DWORD typeLen, DWORD *typeIndex, DWORD startIndex)
{
	DWORD	i;

	strupr(curLine);
	for(i = startIndex; i < typeLen; i++)
	{
		if(strstr(curLine, typePtr[i].name) != NULL)
		{
			*typeIndex = i;
			return(TRUE);
		}
	}
	*typeIndex = 0;
	return(FALSE);
}

void ParseFileBin(char *fileName, ts_parse *typePtr, DWORD typeLen, DWORD flags)
{
	FILE		*fp;
	DWORD		rCode;

	if(flags & PARSE_BIN_FOPEN)
   {
		fp = fopen(fileName, "rb");
		if(fp == NULL)
			return;
	}

	rCode = typePtr[0].parseProc(fp, GetFileName(fileName), typePtr, typeLen, 0, NULL,0);
	if(rCode != TRUE)
		PARSER_ERROR(rCode);

	if(flags & PARSE_BIN_FOPEN)
		fclose(fp);

	return;
}


/*
 * Get a pointer to just the file name.
 */
char *GetFileName(char *file)
{
	char *prevPtr, *curPtr;

	prevPtr = NULL;
   curPtr = file;
   while((curPtr = strchr(curPtr, '\\')) != NULL)
   {
		curPtr++;
		prevPtr = curPtr;
   }

	if(prevPtr == NULL)
   	return(file);
	else
	   return(prevPtr);
}

/* -----------------2/11/99 12:42PM------------------
 * Returns a pointer skipping all space characters
 * returning a pointer to the first non white character.
 * --------------------------------------------------*/
char *eatWhite(char *pString)
{
	char curChar;

	while((char )*pString != 0)
	{
		curChar = *pString;

		/* -----------------2/11/99 12:47PM------------------
		 * If the curChar is not a space character then return
		 * a pointer to this character.
		 * --------------------------------------------------*/
		if(strchr(spaceString, curChar) == NULL)
			return(pString);

		pString++;
	}

	return(NULL);
}





void ParserError(char *fileName, DWORD lineNum, HRESULT rCode)
{
	char textBuf[100];

	sprintf(textBuf, "File: %s, Line #: %d, rCode: %d\n\r", fileName, lineNum, rCode);
	MessageBox(NULL, textBuf, "ERROR:  PARSER", MB_OK | MB_ICONSTOP);

	return;
}