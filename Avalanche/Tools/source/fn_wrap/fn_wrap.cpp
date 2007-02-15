/* --------------------------------------------------
 * File    : FN_WRAP.CPP
 * Created : Mon Sep 21 11:37:25 1998
 * Descript:
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fn_wrap.h>

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/

FNWrapper::FNWrapper( )
{
	SetFromString( NULL);

}

FNWrapper::FNWrapper( const char *pcStr)
{
	SetFromString( (char *) pcStr);
}

void FNWrapper::SetFromString( char *pcStr)
{

	if( pcStr)
	{
		pFName = strdup( pcStr);
		iAllocBytes = strlen( pcStr) + 1;
	}
	else
	{
		pFName = NULL;
		iAllocBytes = 0;
	}

	SetEnds( );
	Setup( );
}

void FNWrapper::Release( void)
{
	FreeBuffer( );

	SetEnds( );
	Setup( );
}

void FNWrapper::FreeBuffer( void)
{
	if( pFName)
		free( pFName);

	iAllocBytes = 0;
}

void FNWrapper::SetEnds( void)
{
	if( pFName == NULL)
	{
		pFileEnd = NULL;
		pBufferEnd = NULL;
	}
	else
	{
		pFileEnd = pFName + strlen( pFName);
		pBufferEnd = pFName + iAllocBytes - 1;
	}
}

void FNWrapper::Setup( void)
{
char					*pNameTemp;

	Flags = 0;

	if( pFName == NULL)
	{
		pExtPtr = NULL;
		pNamePtr = NULL;
		return;
	}

	ConvSlash( pFName);

	pNameTemp = pNamePtr = pFName;

	while( pNameTemp)
	{
		pNameTemp = strchr( pNameTemp + 1, '\\');

		if( pNameTemp)
			pNamePtr = pNameTemp;
	} ;

	if( pNamePtr != pFName)
	{
		Flags |= FNW_HAS_PATH;
		pNamePtr++;
	}

	pExtPtr = NULL;

	if( *pNamePtr == '.')
	{
		pExtPtr = pNamePtr;
		pNamePtr = NULL;
	}
	else if( pNamePtr)
	{
		Flags |= FNW_HAS_NAME;
		pExtPtr = strchr( pNamePtr, '.');
	}

	if( pExtPtr)
		Flags |= FNW_HAS_EXT;
}

void FNWrapper::Break( char **ppPath, char **ppName, char **ppExt, t_lflagbits GetFlags)
{
int					iTempLen;

	if( GetFlags & FNW_GET_PATH)
	{
		if( *ppPath)
			free( *ppPath);

		if( Flags & FNW_HAS_PATH)
		{
			iTempLen = PathLen( );
			*ppPath = (char *) malloc( iTempLen + 1);
			memcpy( *ppPath, pFName, iTempLen);
			(*ppPath)[iTempLen] = 0;
		}
		else
		{
			*ppPath = NULL;
		}
	}

	if( GetFlags & FNW_GET_NAME)
	{
		if( *ppName)
			free( *ppName);

		if( Flags & FNW_HAS_NAME)
		{
			iTempLen = NameLen( );
			*ppName = (char *) malloc( iTempLen + 1);
			memcpy( *ppName, pNamePtr, iTempLen);
			(*ppName)[iTempLen] = 0;
		}
		else
		{
			*ppName = NULL;
		}
	}

	if( GetFlags & FNW_GET_EXT)
	{
		if( *ppExt)
			free( *ppExt);

		if( Flags * FNW_HAS_EXT)
		{
			*ppExt = strdup( pExtPtr);
		}
		else
		{
			*ppExt = NULL;
		}
	}

}

void FNWrapper::ChangeExt( const char *pcExt)
{
char					*pNewBuffer;
char					*pNewEnd;
char					*pLocalExt;

	if( *pcExt == '.')
		pLocalExt = strdup( pcExt);
	else
	{
		pLocalExt = (char *) malloc( strlen( pcExt) + 2);
		*pLocalExt = '.';
		strcpy( pLocalExt + 1, pcExt);
	}


	if( Flags & FNW_HAS_EXT)
		pNewEnd = pExtPtr + strlen( pLocalExt);
	else
		pNewEnd = pFileEnd + strlen( pLocalExt);


	if( pNewEnd > pBufferEnd)
	{
		iAllocBytes = pNewEnd - pFName + 1;
		pNewBuffer = (char *) malloc( iAllocBytes);

		CopyToBuffer( pNewBuffer, (char *) pLocalExt, FNW_EXT);

		NewBuffer( pNewBuffer);
	}
	else
	{
		if( Flags & FNW_HAS_EXT)
			strcpy( pExtPtr, pLocalExt);
		else
			strcat( pFName, pLocalExt);

		Flags |= FNW_HAS_EXT;
	}

	free( pLocalExt);

}

void FNWrapper::ChangeName( const char *pcName)
{
char					*pNewBuffer;

	iAllocBytes = PathLen( ) + strlen( pcName) + ExtLen( ) + 1;

	pNewBuffer = (char *) malloc( iAllocBytes);

	CopyToBuffer( pNewBuffer, (char *) pcName, FNW_NAME);
	NewBuffer( pNewBuffer);

}

void FNWrapper::ChangePath( const char *pcPath)
{
int					iPathLen;
char					*pNewBuffer;

	iPathLen = strlen( pcPath);

   if( pcPath[iPathLen - 1] != '\\')
   	iPathLen++;

	iAllocBytes = iPathLen + NameLen( ) + ExtLen( ) + 1;

	pNewBuffer = (char *) malloc( iAllocBytes);

	CopyToBuffer( pNewBuffer, (char *) pcPath, FNW_PATH);

	NewBuffer( pNewBuffer);
}

void FNWrapper::NewBuffer( char *pNewBuffer)
{
	if( pFName)
		free( pFName);

	pFName = pNewBuffer;

	SetEnds( );
	Setup( );
}

void FNWrapper::CopyToBuffer( char *pNewBuffer, char *pNewStr, int iReplace)
{

	if( iReplace == FNW_NONE)
	{
		strcpy( pNewBuffer, pFName);
		return;
	}

	*pNewBuffer = 0;

	if( iReplace == FNW_PATH)
   {
		strcpy( pNewBuffer, pNewStr);
      if( pNewStr[strlen( pNewStr) - 1] != '\\')
      	strcat( pNewBuffer, "\\");
   }
	else if( Flags & FNW_HAS_PATH)
		strncpy( pNewBuffer, pFName, PathLen( ));

	if( iReplace == FNW_NAME)
		strcat( pNewBuffer, pNewStr);
	else if( Flags & FNW_HAS_NAME)
		strncat( pNewBuffer, pNamePtr, NameLen( ));

	if( iReplace == FNW_EXT)
		strcat( pNewBuffer, pNewStr);
	else if( Flags & FNW_HAS_EXT)
		strncat( pNewBuffer, pExtPtr, ExtLen( ));
}

int FNWrapper::ExtLen( void)
{
	return (Flags & FNW_HAS_EXT) ? strlen( pExtPtr) : 0;
}

int FNWrapper::NameLen( void)
{
	if( Flags & FNW_HAS_NAME)
	{
		if( Flags & FNW_HAS_EXT)
			return pExtPtr - pNamePtr;
		else
			return strlen( pNamePtr);
	}

	return 0;
}

int FNWrapper::PathLen( void)
{

	if( Flags & FNW_HAS_PATH)
	{
		if( Flags & FNW_HAS_NAME)
			return pNamePtr - pFName;
		else if( Flags & FNW_HAS_EXT)
			return pExtPtr - pFName;

		return strlen( pFName);
	}

	return 0;
}

void FNWrapper::Debug( void)
{
	printf( "Flags: ");

	if( Flags & FNW_HAS_PATH)
		printf( "HAS_PATH ");
	if( Flags & FNW_HAS_NAME)
		printf( "HAS_NAME ");
	if( Flags & FNW_HAS_EXT)
		printf( "HAS_EXT ");

	printf( "\n");

	printf( "pFName: %p %s\n", pFName, pFName ? pFName : "NULL");
	printf( "pFileEnd  : %p %lu\n", pFileEnd, pFileEnd - pFName);
	printf( "pBufferEnd: %p %lu\n", pBufferEnd, pBufferEnd - pFName);
	printf( "pName : %p %s\n", pNamePtr, pNamePtr ? pNamePtr : "NULL");
	printf( "pExt  : %p %s\n", pExtPtr, pExtPtr ? pExtPtr : "NULL");
	printf( "\n");
}

void AddSlash( char *pStr)
{
	pStr += strlen( pStr);

	if( pStr[-1] != '\\')
	{
		pStr[0] = '\\';
		pStr[1] = 0;
	}
}

void ConvSlash( char *pStr)
{
char					*pSlash;

	pSlash = strchr( pStr, '/');

	while( pSlash)
	{
		*pSlash = '\\';
		pSlash = strchr( pSlash, '/');
	}
}
