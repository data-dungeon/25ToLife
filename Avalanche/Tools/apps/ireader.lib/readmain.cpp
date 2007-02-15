/* --------------------------------------------------
 * File    : READMAIN.CPP
 * Created : Tue Sep 22 13:51:19 1998
 * Descript: 
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <fstream.h>
#include <string.h>

#include <t_macros.h>

#include <fn_wrap.h>
#include <readbase.h>
#include <read_tga.h>

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/
enum
{
	EXT_TGA = 0,
	
	EXT_MAX
} ;

enum
{
	RI_ERR_NONE = 0,
	RI_ERR_IREAD,
	RI_ERR_NOREADER,
	RI_ERR_UNSUPPORTED,
	RI_ERR_UNKNOWN
} ;

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
int			ExtIdx( const char *pcExt);
ReadBase *	GetReader( const char *pcExt);
ifstream *	GetStream( const char *pcFName);

t_error		FindImage( const char *pcBaseName, char *pName);
t_error		GetImage( const char *pcBaseName, ts_RBImage **ppImage, t_lflagbits Flags);

extern "C" __declspec( dllexport) t_error _cdecl _FindImage( const char *pcBaseName, char *pName);
extern "C" __declspec( dllexport) t_error _cdecl _GetImage( const char *pcBaseName, ts_RBImage **ppImage, t_lflagbits Flags);


/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/
const char			*pcExtList[] = 
{
	"TGA",
	NULL
} ;

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/
t_error				LastError;

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
__declspec( dllexport) t_error _cdecl _FindImage( const char *pcBaseName, char *pName)
{
	return FindImage( pcBaseName, pName);
}

__declspec( dllexport) t_error _cdecl _GetImage( const char *pcBaseName, ts_RBImage **ppImage, t_lflagbits Flags)
{
	return GetImage( pcBaseName, ppImage, Flags);
}

t_error  FindImage( const char *pcBaseName, char *pFName)
{
char					*pExt;

int					i;

ifstream				*pStream;

FNWrapper			FName;
ReadBase				*pReader;


	FName = pcBaseName;

	pExt = FName.Ext( );

	if( pExt == NULL)
	{
		LastError = RI_ERR_UNKNOWN;
		return LastError;
	}

	pStream = NULL;
	if( ExtIdx( pExt + 1) != -1)
		pStream = GetStream( FName);

	i = 0;
	while( pStream == NULL  &&  pcExtList[i])
	{
		FName.ChangeExt( pcExtList[i]);

		pStream = GetStream( FName);

		if( pStream)
			break;
		i++;
	}

	pReader = NULL;
	LastError = 0;

	if( pStream)
		strcpy( pFName, (char *) FName);
	else
		LastError = RI_ERR_UNSUPPORTED;


	SDELETE( pReader);
	SDELETE( pStream);

	return LastError;

}

t_error	GetImage( const char *pcBaseName, ts_RBImage **ppImage, t_lflagbits Flags)
{
char					*pExt;

int					i;

ifstream				*pStream;

FNWrapper			FName;
ReadBase				*pReader;

ts_RBImage			*pNewImage;

	FName = pcBaseName;

	pExt = FName.Ext( );

	if( pExt == NULL)
	{
		LastError = RI_ERR_UNKNOWN;
		return LastError;
	}

	pStream = NULL;
	if( ExtIdx( pExt + 1) != -1)
		pStream = GetStream( FName);

	i = 0;
	while( pStream == NULL  &&  pcExtList[i])
	{
		FName.ChangeExt( pcExtList[i]);

		pStream = GetStream( FName);

		if( pStream)
			break;
		i++;
	}

	pReader = NULL;
	LastError = 0;

	if( pStream)
	{
		pReader = GetReader( FName.Ext( ));

		if( pReader)
		{
			pReader->Attach( *pStream);

			if( !pReader->Read( ))
				LastError = RI_ERR_IREAD;
		}
		else
			LastError = RI_ERR_NOREADER;
	}
	else
		LastError = RI_ERR_UNSUPPORTED;

	if( LastError == 0)
	{
		pNewImage = pReader->GetImage( Flags);
		if( *ppImage == NULL)
			*ppImage = pNewImage;
		else
		{
			memcpy( *ppImage, pNewImage, sizeof( ts_RBImage));
			free( pNewImage);
		}
	}

	SDELETE( pReader);
	SDELETE( pStream);

	return LastError;
}



ReadBase *GetReader( const char *pcExt)
{

	switch( ExtIdx( pcExt))
	{
	case EXT_TGA:
		return new ReadTGA( );
	default:
		break;
	}

	return NULL;

}


ifstream *GetStream( const char *pcFName)
{
ifstream				*pStream;

	pStream = new ifstream( );
	pStream->open( pcFName, ios::in | ios::nocreate | ios::binary);

	pStream->peek( );

	if( *pStream)
	{
		return pStream;
	}
	else
	{
		pStream->close( );
		delete pStream;
		return NULL;
	}
}


int ExtIdx( const char *pcExt)
{
int					i;

	while( *pcExt == '.')
		pcExt++;

	i = 0;

	while( pcExtList[i] != NULL)
	{
		if( strcmp( pcExtList[i], pcExt) == 0)
			return i;
		i++;
	}

	return -1;
}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/

