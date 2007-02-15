/* --------------------------------------------------
 * File    : DLLSTUFF.CPP
 * Created : Wed Feb 10 17:05:31 1999
 * Descript:
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/

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
void FindDrvr( char *pFName, ReadFunc *pRead, FreeFunc *pFree)
{
char					*pExt;

	*pRead = NULL;
	*pFree = NULL;

	pExt = strchr( pFName, '.');

	if( pExt)
	{
		pExt++;
		if( pExt)
		{
			if( stricmp( pExt, "av2") == 0)
			{
				*pRead = _AV2Read;
				*pFree = _AV2Free;
			}
			else if( stricmp( pExt, "k2a") == 0)
			{
				*pRead = _K2ARead;
				*pFree = _K2AFree;
			}
		}
	}
}

void FreeNames( char **ppNames, int iNames)
{
int					i;

	assert( ppNames);
	assert( iNames);

	for( i = 0; i < iNames; i++)
	{
		if( ppNames[i])
			free( ppNames[i]);
	}

}

void WriteTextureListFile( char *pFileName, char **ppFileNames, char **ppTextureNames, int iNames)
{
int					i;
ofstream				LstStream;

	assert( iNames);
	assert( ppTextureNames);
	assert( ppFileNames);

	LstStream.open( LstFile);

	if( LstStream)
	{
		for( i = 0; i < iNames; i++)
		{
		if( ppFileNames[i])
		{
			LstStream << ppFileNames[i];
			if( ppTextureNames[i])
				LstStream << " ; " << ppTextureNames[i];
		}
		else if( ppTextureNames[i])
		{
			LstStream << ppTextureNames[i];
		}

		LstStream << "\n";
		}

		LstStream.close( );
	}

}

char **BuildPathedTextureNames( char **ppTextureNames, int iNames, char *pAltPath)
{
char					**ppPathedNames;
FNWrapper			FName;

	assert( iNames);

	ppPathedNames = (char **) malloc( iNames * sizeof( char *));
	memset( ppPathedNames, 0, iNames * sizeof( char *));

	for( int i = 0; i < iNames; i++)
	{
		if( ppTextureNames[i])
		{
			FName = ppTextureNames[i];

			if( pAltPath)
				FName.ChangePath( pAltPath);

		ppPathedNames[i] = strdup( (char *) FName);
		}
	}

	return ppPathedNames;
}

char **FindTextureFiles( char **ppTextureNames, int iNames)
{
char					cTemp[256];

char					**ppNames;

	assert( ppTextureNames != NULL);
	assert( iNames);

	ppNames = (char **) malloc( iNames * sizeof( char *));
	memset( ppNames, 0, iNames * sizeof( char *));

	for( int i = 0; i < iNames; i++)
	{
		if( ppTextureNames[i])
		{
			if( FindImage( ppTextureNames[i], cTemp) == 0)
				ppNames[i] = strdup( cTemp);
			else
				ppNames[i] = NULL;
		}
		else
			ppNames[i] = NULL;
	}

	return ppNames;
}

ts_RBImage *ReadTextureFiles( char **ppTextureNames, int iNames)
{
ts_RBImage			*pImageList;
ts_RBImage			*pImageTemp;

	assert( ppTextureNames != NULL);
	assert( iNames);

	pImageList = (ts_RBImage *) malloc( iNames * sizeof( ts_RBImage));
	memset( pImageList, 0, iNames * sizeof( ts_RBImage));

	for( int i = 0; i < iNames; i++)
	{
		if( ppTextureNames[i])
		{
			Status( "Reading Texture", ppTextureNames[i], i);
			if( GetImage( ppTextureNames[i], &pImageTemp, RB_GET_RGB_M) == 0)
			{
				memcpy( pImageList + i, pImageTemp, sizeof( ts_RBImage));
				free( pImageTemp);
			}
			else
				pImageList[i].pImage = NULL;
		}
		else
			pImageList[i].pImage = NULL;
	}

	return pImageList;
}

