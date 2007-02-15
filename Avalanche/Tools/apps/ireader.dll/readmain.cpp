/* --------------------------------------------------
 * File    : READMAIN.CPP
 * Created : Fri Sep 18 17:29:34 1998
 * Descript: 
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <fstream.h>
#include <string.h>

#include <read_tga.h>

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/
enum
{
	RI_ERR_NONE = 0,
	RI_ERR_TYPEUNKNOWN,
	RI_ERR_FILEOPEN,
	RI_ERR_READERROR,
	RI_ERR_UNSUPPORTED,

	RI_ERR_MAX
} ;

enum
{
	EXT_TGA = 0,
	
	EXT_MAX
} ;

const char			*pcExtList[] = 
{
	"TGA",
	NULL
} ;

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
ReadBase	*			GetReader( const char *pcExt);
ifstream	*			GetStream( const char *pcFName);

const char *		PointExt( const char *pcFName);

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/
t_error				LastError;

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

t_error ReadImageFile( const char *pcFName, ts_RBImage **ppImage, t_lflagbits ReadFlags)
{
char					*pcExt;

ifstream				*pStream;

ReadBase				*pReader;

	LastError = 0;
	pStream = NULL;
	pReader = NULL;

	*ppImage == NULL;

	pcExt = (char *) PointExt( pcFName);

	if( pcExt == NULL)
		return RI_ERR_TYPEUNKNOWN;


	pStream = GetStream( pcFName);

	if( pStream)
	{
		pReader = GetReader( pcExt);

		if( pReader)
		{
			pReader->Attach( *pStream);
			if( pReader->Read( ) == false)
				LastError = RI_ERR_READERROR;
		}
		else
		{
			LastError = RI_ERR_UNSUPPORTED;
		}
	}
	else
	{
		LastError = RI_ERR_FILEOPEN;
	}

	if( LastError == 0)
		*ppImage = pReader->GetImage( ReadFlags);

	return LastError;
}

t_error ReadImageFileAs( const char *pcFName, ts_RBImage **ppImage, t_lflagbits ReadFlags)
{

	LastError = RI_ERR_UNSUPPORTED;

	return LastError;
}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
ReadBase *GetReader( const char *pcExt)
{
int					i;

	i = 0;

	while( pcExtList[i] != NULL)
	{
		if( strcmp( pcExtList[i], pcExt) == 0)
			break;
		i++;
	}

	switch( i)
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

	pStream = new ifstream( pcFName, ios::in | ios::nocreate | ios::binary);

	if( !pStream->is_open( ))
	{
		delete pStream;
		return false;
	}


	return pStream;
}


const char *PointExt( const char *pcFName)
{
char					*pPos;

	pPos = strstr( pcFName, ".");

	return (const char *) ( pPos == NULL) ? NULL : pPos + 1;
}
