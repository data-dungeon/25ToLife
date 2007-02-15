#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argparse.h"

int					iArgsLeft;

char					*pFileArg;

char					**ppcNextArg;

char					**ppcNextRet;
int					nReturnCount;

char *				(*PopArg)( void);


char	*PopArgList( void)
{
	
	if( iArgsLeft)
	{
		iArgsLeft--;

		return *ppcNextArg++;
	}
	else
		return NULL;
}

char *PopArgFile( void)
{
char					*pReturn;

	if( *pFileArg == '\"')
	{
		pFileArg++;

		pReturn = pFileArg;

		while( *pFileArg  &&  *pFileArg != '\"')
			pFileArg++;

		*pFileArg++ = 0;
	}
	else
		pReturn = pFileArg;

	while( *pFileArg  &&  *pFileArg != ' ')
		pFileArg++;

	*pFileArg++ = 0;

	return pReturn;
}

void PushReturn( char *pArg)
{
	*ppcNextRet++ = pArg;
	nReturnCount++;	// track how many arguments get returned
}

void GetString( char **ppString)
{
	char *pString = *ppString;

	char *pArgString = (char *) PopArg( );

	if( pString == NULL)
		*ppString = strdup( pArgString);
	else
		strcpy( pString, pArgString);
}

void RemoveQuotes( char *pString)
{
	if( *pString == '\"')
	{
		int			iStrLen = strlen( pString) - 1;

		strcpy( pString, pString + 1);

	if( pString[iStrLen - 1] == '\"')
		pString[iStrLen - 1] = 0;
	}
}

bool ProcessToken( ts_ArgParseList *pArgs, char *pToken)
{
char					*pValue;

int					iArgLen;

	while( pArgs->pArg)
	{
		iArgLen = strlen( pArgs->pArg);

		if( strncmp( pArgs->pArg, pToken, iArgLen) == 0)
		{
			pValue = pToken + iArgLen;

			switch( pArgs->usFlags)
			{
			case ARG_DOUBLE:
				*((double *) pArgs->pData) = atof( PopArg( ));
				break;
			case ARG_FLOAT:
				*((float *) pArgs->pData) = (float) atof( PopArg( ));
				break;
			case ARG_SHORT:
				*((short *) pArgs->pData) = (short) atoi( PopArg( ));
				break;
			case ARG_USHORT:
				*((unsigned short *) pArgs->pData) = (unsigned short) atoi( PopArg( ));
				break;
			case ARG_LONG:
				*((long *) pArgs->pData) = atol( PopArg( ));
				break;
			case ARG_ULONG:
				*((unsigned long *) pArgs->pData) = (unsigned long) atol( PopArg( ));
				break;
			case ARG_INT:
				*((int *) pArgs->pData) = atoi( PopArg( ));
				break;
			case ARG_UINT:
				*((unsigned int *) pArgs->pData) = (unsigned int) atoi( PopArg( ));
				break;
			case ARG_CHAR:
			case ARG_UCHAR:
				*((unsigned char *) pArgs->pData) = *((unsigned char *) PopArg( ));
				break;
			case ARG_STRINGGET:
				GetString( (char **) pArgs->pData);
				RemoveQuotes( *((char **) pArgs->pData));
				break;
			case ARG_STRINGCPY:
				strcpy( (char *) pArgs->pData, (char *) PopArg( ));
				RemoveQuotes( (char *) pArgs->pData);
				break;
			case ARG_BOOLT:
				*((bool *) pArgs->pData) = true;
				break;
			case ARG_BOOLF:
				*((bool *) pArgs->pData) = false;
				break;
			}
			break;
		}

		pArgs++;
	}

	return true;
}

int ParseArgs( int iArgC, char **ppcArgList, ts_ArgParseList *pArgs, char ***pppcRetList)
{
char					*pCurrentArg;

	iArgsLeft = iArgC;
	PopArg = PopArgList;

	ppcNextArg = ppcArgList;

	if( *pppcRetList == NULL)
	{
		*pppcRetList = (char **) malloc( iArgC * sizeof( char *));
		memset( *pppcRetList, 0, iArgC * sizeof( char *));
	}

	ppcNextRet = *pppcRetList;
	nReturnCount = 0;

	pCurrentArg = PopArg( );

	while( pCurrentArg)
	{
		if( pCurrentArg[0] == '-'  ||  pCurrentArg[0] == '/')
		{
			if( ProcessToken( pArgs, pCurrentArg + 1) == false)
				PushReturn( pCurrentArg);

		}
		else if( pCurrentArg[0] == '@')
		{
			ParseFile( pCurrentArg + 1, pArgs);
			PopArg = PopArgList;
		}
		else
			PushReturn( pCurrentArg);

		pCurrentArg = PopArg( );
	}

	/* return number of arguments returned via PushReturn */

	return(nReturnCount);
}

void ParseFile( char *pFile, ts_ArgParseList *pArgs)
{
char					*pLine;
char					*pToken;
char					FileLine[1024];

FILE					*pFP;

	PopArg = PopArgFile;

	pFP = fopen( pFile, "rt");

	if( !pFP)
		return;

	while( !feof( pFP))
	{
		pLine = fgets( FileLine, 1023, pFP);

		if( pLine)
		{
			strtok( pLine, "\n");
			strtok( pLine, ";");

			pToken = pLine;

			while( *pLine != ' '  &&  *pLine)
				pLine++;

			*pLine++ = 0;

			while( *pLine == ' '  &&  *pLine)
				pLine++;

			pFileArg = pLine;

			if( pToken)
				ProcessToken( pArgs, pToken);
		}

	}

	fclose( pFP);
}