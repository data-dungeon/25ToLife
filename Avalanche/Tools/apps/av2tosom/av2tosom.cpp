/* --------------------------------------------------
 * File    : AV2TOSOM.CPP
 * Created : Wed Oct  7 18:07:35 1998
 * Descript:
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <dbwriter.h>
#include <av2_dll.h>
#include <k2a_dll.h>
#include <fn_wrap.h>
#include <som_writ.h>
#include <som_util.h>
#include <t_macros.h>
#include <t_som.h>
#include <t_struct.h>
#include <t_hierar.h>
#include <textdb.h>
#include <readbase.h>
#include <ireadlib.h>

#include "\lib\argparse\argparse.h"

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/
typedef bool (*ReadFunc)( const char *, ts_SOMUP **, ts_CSList *, ts_Hierarchy **ppBones, stat_func);	
typedef void (*FreeFunc)( ts_SOMUP *, ts_CSList *, ts_Hierarchy *pBones);
typedef void (*CleanFunc) (void);

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
void				Usage( void);
void				Status( char *pPri, char *pSec, int iProgress);
void				ShowSOMUP( ts_SOMUP *pModel);
void				ShowSOM( ts_SOM *pModel);
void				FindDrvr( char *pFName, ReadFunc *pRead, FreeFunc *pFree);
void				FreeNames( char **ppNames, int iNames);

void				WriteTextureListFile( char *pFileName, char **ppFileNames, char **ppTextureNames, int iNames);
char **			BuildPathedTextureNames( char **ppTextureNames, int iNames, char *pAltPath);
char **			FindTextureFiles( char **ppTextureNames, int iNames);
ts_RBImage *	ReadTextureFiles( char **ppTextureNames, int iNames);

extern void		WriteBones( mpostream &stream, ts_Hierarchy *pBones);

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/
bool					bSort = true;
bool					bWritePSX = false;
bool					bWriteN64 = false;
bool					bCombine = false;
bool					bOneTex = false;
bool					bDispUP = false;
bool					bDispP = false;
bool					bAddVertexNormals = false;
bool					bCalcFaceNormals = false;
bool					bNoTex = false;
bool					bBones = false;
bool					bMakeQuad = false;
bool					bNoModel = false;

char					MapPath[200] = { 0 } ;
char					OutPath[200] = { 0 } ;
char					TexFile[200] = { 0 } ;
char					ObjFile[200] = { 0 } ;
char					LstFile[200] = { 0 } ;

char					*pTexName = NULL;
char					*pObjName = NULL;

float					fScaleL = 1.0f;
float					fScaleX = 1.0f;
float					fScaleY = 1.0f;
float					fScaleZ = 1.0f;


ts_ArgParseList	Args[] =
{
	{ "MPATH",		NULL, &MapPath,	ARG_STRINGCPY	},
	{ "OPATH",		NULL, &OutPath,	ARG_STRINGCPY	},
	{ "LFILE",		NULL, &LstFile,	ARG_STRINGCPY	},
	{ "TNAME",		NULL, &pTexName,	ARG_STRINGGET	},
	{ "ONAME",		NULL, &pObjName,	ARG_STRINGGET	},
	{ "TFILE",		NULL, &TexFile,	ARG_STRINGCPY	},
	{ "OFILE",		NULL, &ObjFile,	ARG_STRINGCPY	},
	{ "C",			NULL,	&bCombine,	ARG_BOOLT		},
	{ "PSX",			NULL, &bWritePSX,	ARG_BOOLT		},
	{ "N64",			NULL, &bWriteN64,	ARG_BOOLT		},
	{ "QUAD",		NULL, &bMakeQuad, ARG_BOOLT		},
	{ "ONETEX",		NULL, &bOneTex,	ARG_BOOLT		},
	{ "NOTEX",		NULL, &bNoTex,		ARG_BOOLT		},
	{ "DISPUP",		NULL, &bDispUP,	ARG_BOOLT		},
	{ "DISPP",		NULL, &bDispP,		ARG_BOOLT		},
	{ "NOSORT",		NULL, &bSort,		ARG_BOOLF		},
	{ "SCALEL",		NULL, &fScaleL,	ARG_FLOAT		},
	{ "SCALEX",		NULL, &fScaleX,	ARG_FLOAT		},
	{ "SCALEY",		NULL, &fScaleY,	ARG_FLOAT		},
	{ "SCALEZ",		NULL, &fScaleZ,	ARG_FLOAT		},
	{ "FNORMS",		NULL, &bCalcFaceNormals,	ARG_BOOLT		},
	{ "VNORMS",		NULL, &bAddVertexNormals,	ARG_BOOLT		},
	{ "BONES",		NULL, &bBones, ARG_BOOLT			},
	{ "NOMODEL",	NULL, &bNoModel, ARG_BOOLT			},
	NULL
} ;

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
int main( int iArgC, char **ppcArgL)
{

char					sBuffer[256];
char					*pMapPath;
char					**ppInNames = NULL;

unsigned long		l;
unsigned long		ulWriteFlags;

ts_SOMUP				*pDLLModel;
ts_SOMUP				*pModel;
ts_SOM				*pPacked;

ts_Hierarchy		*pBones;
ts_CSList			Textures;
ts_CSList			PathedTextureNames;

ts_RBImage			*pImages;

FNWrapper			ObjWrapper;
FNWrapper			TexWrapper;

DBWriter				*pObjWriter = NULL;
DBWriter				*pTexWriter = NULL;

TextureDB			*pTexDBWriter;

ReadFunc				pRead = NULL;
FreeFunc				pFree = NULL;

	ParseArgs( iArgC, &ppcArgL[1], Args, &ppInNames);

	if( ppInNames[0] == NULL)
	{
		Usage( );
		printf( "Error: No Input FileName specified.\n");
		return -1;
	}

	if( ObjFile[0] == 0  &&  TexFile[0] == 0  &&  LstFile[0] == 0)
	{
		Usage( );
		printf( "Error: No Output Specified.\n");
		return -2;
	}

	if( MapPath[0] == 0)
		pMapPath = NULL;
	else
	{
		AddSlash( MapPath);
		pMapPath = MapPath;
	}
	
	if( OutPath[0] != 0)
		AddSlash( OutPath);

	if( ObjFile[0])
	{
		strlwr( ObjFile);
		ObjWrapper = (char *) ObjFile;
		ObjWrapper.ChangeExt( ".dbl");

		if( OutPath[0] != 0)
			ObjWrapper.ChangePath( OutPath);

		if( pObjName == NULL)
		{
			ObjWrapper.Break( NULL, &pObjName, NULL, FNW_GET_NAME);
			strupr( pObjName);
		}

	}
	else if( bCombine)
	{
		printf( "Warning: Combine flag not valid without Obj FileName. Ignoring.\n");
		bCombine = false;
	}

	ulWriteFlags = 0;

	if( bWritePSX)
	{
		if( !bNoTex)
			bOneTex = true;

		ulWriteFlags |= SOM_PAK_CLR_BYTE;
		ulWriteFlags |= SOM_PAK_UV_BYTE;
	}
	else if( bWriteN64)
	{
		if( !bNoTex)
			bOneTex = true;

		ulWriteFlags |= SOM_PAK_CLR_BYTE;
		ulWriteFlags |= SOM_PAK_UV_FLOAT;
	}
	else
	{
		ulWriteFlags |= SOM_PAK_CLR_FLOAT;
		ulWriteFlags |= SOM_PAK_UV_FLOAT;
	}

	if( bNoTex)
	{
		ulWriteFlags |= SOM_PAK_NO_TEXTURES;
		if( bOneTex)
		{
			printf( "Warning: NOTEX option overrides ONETEX.\n");
			bOneTex = false;
		}
	}
	else	if( bOneTex)
		ulWriteFlags |= SOM_PAK_SNGL_TEXTURE;

	if( TexFile[0])
	{
		strlwr( TexFile);
		TexWrapper = (char *) TexFile;
		TexWrapper.ChangeExt( ".dbl");

		if( OutPath[0])
			TexWrapper.ChangePath( OutPath);

		if( bCombine)
		{
			printf( "Warning: Combine flag overridden by Texture FileName.\n");
			bCombine = false;
		}

		if( pTexName == NULL)
		{
			TexWrapper.Break( NULL, &pTexName, NULL, FNW_GET_NAME);
			strupr( pTexName);
		}
	}


	FindDrvr( ppInNames[0], &pRead, &pFree);

	if( ppInNames[0]  && pRead )
	{
		Status( "Reading", ppInNames[0], 0);
		pBones = NULL;

		if( pRead( ppInNames[0], &pDLLModel, &Textures, &pBones, Status))
		{
			pModel = somCopySOMUP( pDLLModel);

			somSetup( pModel);
			if( bCalcFaceNormals)
			{
				Status( "Calculating Face Normals", ppInNames[0], 0);
				somCalcFaceNormals( pModel);
			}
		
			if( bAddVertexNormals)
			{
				Status( "Adding Vertex Normals", ppInNames[0], 0);
				somAddVertexNormals( pModel);
			}

			if( bMakeQuad)
			{
				somConvertQuad( &pModel);
			}

			if( bSort)
			{
				Status( "Sorting", ppInNames[0], 0);
				if( bOneTex)
					somSort( pModel, SOM_SORT_SNGL_TEXTURE);
				else
					somSort( pModel, 0);
			}

			if( fScaleL != 1.0f || fScaleX != 1.0f || fScaleY != 1.0f || fScaleZ != 1.0f)
			{
				sprintf( sBuffer, "Scaling by X:%8.4f Y:%8.4f Z:%8.4f", fScaleL * fScaleX, fScaleL * fScaleY, fScaleL * fScaleZ);
				Status( sBuffer, ppInNames[0], 0);

				somScale( pModel, fScaleL * fScaleX,  fScaleL * fScaleY,  fScaleL * fScaleZ);
				somBoundingSphere( pModel, &(pModel->BoundingSphere));

			}

			pPacked = somPack( pModel, ulWriteFlags, 0);

			// Find Pathed Textures
			PathedTextureNames.ulCount = Textures.ulCount;
			PathedTextureNames.ulSize = Textures.ulSize;
			if( Textures.ulCount)
				PathedTextureNames.pList = BuildPathedTextureNames( (char **) Textures.pList, Textures.ulCount, pMapPath);
			else
				PathedTextureNames.pList = NULL;

			// ReadTextures
			if( TexFile[0])
				pImages = ReadTextureFiles( (char **) PathedTextureNames.pList, (int) PathedTextureNames.ulCount);

			if( ObjFile[0])
			{
				if( bWritePSX == true)
					pObjWriter = new DBWriter( ObjWrapper, DBWriter::PSXSt);
				else if( bWriteN64 == true)
					pObjWriter = new DBWriter( ObjWrapper, DBWriter::N64St);
				else
					pObjWriter = new DBWriter( ObjWrapper, DBWriter::PCSt);

				pObjWriter->DBStart( 0);
			}
			else
				pObjWriter = NULL;

			if( TexFile[0]  &&  pImages)
			{
				pTexWriter = new DBWriter( TexWrapper, DBWriter::PCSt);
				pTexWriter->DBStart( 0);
			}
			else if( bCombine)
			{
				pTexWriter = pObjWriter;
			}
			else
			{
				pTexWriter = NULL;
			}

			if( pTexWriter)
			{
				ts_RBImage			*pImage = pImages;

				if( TexFile[0])
					Status( "Writing Textures", strupr( (char *) TexWrapper), 0);
				else
					Status( "Writing Textures", strupr( (char *) ObjWrapper), 0);

				pTexDBWriter = new TextureDB( pTexWriter);

				for( l = 0; l < PathedTextureNames.ulCount; l++)
				{
					pTexDBWriter->AddTexture( pImage->IFlags, pImage->usWidth, pImage->usHeight, pImage->pImage);
					pImage++;
				}

				pTexDBWriter->Write( pTexName);

				delete pTexDBWriter;

				for( l = 0; l < PathedTextureNames.ulCount; l++)
					SFREE( pImages[l].pImage);

				free( pImages);
			}

			if( pObjWriter)
			{
				if( bWritePSX)
					Status( "Writing PSX Model", strupr( (char *) ObjWrapper), 0);
				else if( bWriteN64)
					Status( "Writing N64 Model", strupr( (char *) ObjWrapper), 0);

				else
					Status( "Writing PC Model", strupr( (char *) ObjWrapper), 0);
	
				if( bBones  &&  pBones)
				{
					if( bNoModel)
						pObjWriter->ChunkStart( 0x23, 0, pObjName);
					else
						pObjWriter->ChunkStart( 0x21, 0, pObjName);
				}
				else
					pObjWriter->ChunkStart( 0x20, 0, pObjName);

				if( !bNoModel)
				{
					if( bWritePSX == true)
						WritePSXModel1( pObjWriter->stream( ), *pPacked, 0);
					else if( bWriteN64 == true)
						WriteN64Model1( pObjWriter->stream( ), *pPacked, 0);
					else
						pObjWriter->stream( ) << *pPacked;
				}

				if( bBones == true  &&  pBones)
					WriteBones( pObjWriter->stream( ), pBones);

				pObjWriter->ChunkEnd( );
				pObjWriter->DBEnd( );
			}

			if( LstFile[0])
			{
				char **ppFileNames = FindTextureFiles( (char **) PathedTextureNames.pList, PathedTextureNames.ulCount);
				
				if( ppFileNames)
					WriteTextureListFile( LstFile, ppFileNames, (char **) Textures.pList, (int) PathedTextureNames.ulCount);

				if( ppFileNames  &&  PathedTextureNames.ulCount)
				{
					FreeNames( (char **) ppFileNames, PathedTextureNames.ulCount);
					free( ppFileNames);
				}
			}

			if( bDispUP)
				ShowSOMUP( pModel);

			if( bDispP)
				ShowSOM( pPacked);

			somFreeSOM( pPacked);
			somFreeSOMUP( pModel);

			if( PathedTextureNames.pList  &&  PathedTextureNames.ulCount)
			{
				FreeNames( (char **) PathedTextureNames.pList, PathedTextureNames.ulCount);
				free( PathedTextureNames.pList);
			}


			if( pFree)
				pFree( pDLLModel, &Textures, pBones);

			if( pTexWriter)
			{
				if( pTexWriter != pObjWriter)
				{
					pTexWriter->DBEnd( );
					delete pTexWriter;
				}
			}

			if( pObjWriter)
				delete pObjWriter;
		}

	}
	SFREE( pObjName);
	SFREE( pTexName);

	SFREE( ppInNames);

	return 0;
}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
void Usage( void)
{
	printf( "AV2toSOM <options> <av2file>\n");
	printf( " options:\n");
	printf( "   -MPATH <path>      Path to Textures\n");
	printf( "   -OPATH <path>      Output file Path\n");
	printf( "   -TNAME <string>    Chunk Name for Textures\n");
	printf( "   -ONAME <string>    Chunk Name for Model\n");
	printf( "   -TFILE <filename>  FileName for Textures\n");
	printf( "   -OFILE <filename>  FileName for Model Data\n");
	printf( "   -LFILE <filename>  FileName for Texture List\n");
	printf( "   -C                 Combine Textures and Objects\n");
	printf( "   -PSX               Write PSX format data\n");
	printf( "   -SCALE[LXYZ] <var> Scale\n");
	printf( "                      X = X Axis\n");
	printf( "                      Y = Y Axis\n");
	printf( "                      Z = Z Axis\n");
	printf( "                      L = All Axis\n");
	printf( "   -ONETEX            Only write 1 texture\n");
	printf( "   -NOTEX             No Texture Information\n");
	printf( "   -DISPUP            Print out debug info for UnPacked Model\n");
	printf( "   -DISPP             Print out debug info for Packed Model\n");
	printf( "   -NOSORT            Don't sort the Model\n");

}

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

void Status( char *pPri, char *pAlt, int iStatus)
{
	printf( "[%03d%]", iStatus);
	if( pPri)
	{
		printf( " - %s\n", pPri);
		if( pAlt)
			printf( "      - %s\n", pAlt);
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

