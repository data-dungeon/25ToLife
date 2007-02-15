/* --------------------------------------------------
 * File    : CVTMOTN.C
 * Created : Thu Mar  4 10:28:58 1999
 * Descript:
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream.h>

#include <dbwriter.h>

#include <mathlib.h>

#include "motion.h"

#include "\lib\argparse\argparse.h"
/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
void					Usage( void);
char **				ReadBoneFile( char *pcFName, int *pNames);
ts_K2AngAxisSet * LoadMotionFile( char *pcFName);
void					WritePSXMotion( mpostream &str, ts_K2AngAxisSet *pMotion, char **ppBoneNames, int iBones);

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

int main( int iArgC, char **ppcArgL)
{
bool					bWritePSX;
int					nBoneNames;

char					**ppBoneNames;
char					**ppInNames = NULL;

ts_K2AngAxisSet	*pMotion;

DBWriter				*pMotWriter;

ts_ArgParseList	Args[] =
{
	{ "PSX",			NULL, &bWritePSX,	ARG_BOOLT		},
} ;
	
	ParseArgs( iArgC, &ppcArgL[1], Args, &ppInNames);

	if( ppInNames[0] == NULL)
	{
		Usage( );
		return -1;
	}

	if( ppInNames[1] == NULL)
	{
		Usage( );
		return -1;
	}


	ppBoneNames = ReadBoneFile( ppInNames[0], &nBoneNames);
	
	pMotion = LoadMotionFile( ppInNames[1]);

	pMotWriter = new DBWriter( "temp.bin", DBWriter::PSXSt);

	pMotWriter->DBStart( 0);
	pMotWriter->ChunkStart( 0x100, 0, NULL);

	WritePSXMotion( pMotWriter->stream( ), pMotion, ppBoneNames, nBoneNames);

	pMotWriter->ChunkEnd( );
	pMotWriter->DBEnd( );

	delete pMotWriter;

	return 0;
}
/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
void Usage( void)
{
	puts( "Usage: cvtmotn <options> <bonefile> <motnfile>");
	
}

char **ReadBoneFile( char *pcFName, int *pBoneCount)
{
int					NameLen;
int					nBones;

char					cInBuf[256];
char					**ppNames;

ifstream				BFile;

	*pBoneCount = 0;

	BFile.open( pcFName, ios::in | ios::nocreate);

	if( !BFile)
		return NULL;

	nBones = 0;

	while( BFile.peek( ) != EOF)
	{
		cInBuf[0] = 0;

		BFile.getline( cInBuf, 255);

		NameLen = strlen( cInBuf);

		if( NameLen)
			nBones++;

	}

	BFile.seekg( 0, ios::beg);

	ppNames = (char **) malloc( nBones * sizeof( char *));
	memset( ppNames, 0, nBones * sizeof( char *));

	*pBoneCount = nBones;
	nBones = 0;

	while( BFile.peek( ) != EOF)
	{
		cInBuf[0] = 0;

		BFile.getline( cInBuf, 255);

		NameLen = strlen( cInBuf);
		if( NameLen)
			ppNames[nBones++] = strdup( cInBuf);
			
	}

	return ppNames;	

}


ts_K2AngAxisSet * LoadMotionFile( char *pcFName)
{
char					cNameTemp[256];

int					i, j;

ifstream				MFile;

ts_K2MFHdr        FHdr;
ts_K2AngAxisSet	*pSet;


	MFile.open( pcFName, ios::binary);

   if( !MFile)
   	return NULL;

   MFile.read( (char *) &FHdr, sizeof( ts_K2MFHdr));

	pSet = (ts_K2AngAxisSet *) malloc( sizeof( ts_K2AngAxisSet));

   pSet->usFrames = FHdr.usFrames;
   pSet->usBones = FHdr.usBones;

   pSet->pMotionSet = (ts_K2AngAxisHdr *) malloc( FHdr.usBones * sizeof( ts_K2AngAxisHdr));

   for( i = 0; i < FHdr.usBones; i++)
   {
   	j = 0;
		do
   	{
	   MFile.read( &cNameTemp[j], 1);
      } while( cNameTemp[j++]);

		pSet->pMotionSet[i].pBoneName = strdup( cNameTemp);
      pSet->pMotionSet[i].pMotion = (ts_K2AngAxis *) malloc( FHdr.usFrames * sizeof( ts_K2AngAxis));
   }

   for( i = 0; i < FHdr.usFrames; i++)
   	for( j = 0; j < FHdr.usBones; j++)
      {
      	MFile.read( (char *) (pSet->pMotionSet[j].pMotion + i), sizeof( ts_K2AngAxis));
         vctNormalize3f( &(pSet->pMotionSet[j].pMotion[i].Axis));
      }

	return pSet;
}

int BoneIdx( char **ppBoneNames, int iBones, char *pName)
{
int					i;

	for( i = 0; i < iBones; i++)
		if( strcmp( ppBoneNames[i], pName) == 0)
			return i;

	return -1;
}
void					WritePSXFrame( mpostream &str, ts_K2AngAxis *pFrame);

mpostream & operator << ( mpostream &str, ts_K2MFHdr &Hdr);
mpostream & operator << ( mpostream &str, ts_Quatvw &Quat);
mpostream & operator << ( mpostream &str, l_Vct3f &Vct);

void WritePSXMotion( mpostream &str, ts_K2AngAxisSet *pMotion, char **ppBoneNames, int iBones)
{
int					i, j;
int					iWriteBones;
int					*pBoneIdxs;

ts_K2AngAxis		*pMotionData;

ts_K2MFHdr			Hdr = { "K2M", 0x200, 0 ,0 };

	iWriteBones = 0;

	pBoneIdxs = (int *) malloc( pMotion->usBones * sizeof( int));

	for( i = 0; i < (int) pMotion->usBones; i++)
	{
		pBoneIdxs[i] = BoneIdx( ppBoneNames, iBones, pMotion->pMotionSet[i].pBoneName);
		if( pBoneIdxs[i] >= 0)
			iWriteBones++;
	}

	Hdr.usFrames = pMotion->usFrames;
	Hdr.usBones = (unsigned short) iWriteBones;

	str << Hdr;

	for( i = 0; i < (int) pMotion->usBones; i++)
	{
		if( pBoneIdxs[i] >= 0)
			str << (short) pBoneIdxs[i];
	}

	// Keep it aligned
	if( iWriteBones & 0x01)
		str << (short) 0;

	for( i = 0; i < (int) pMotion->usBones; i++)
	{
		if( pBoneIdxs[i] >= 0)
		{
			printf( "Part %d:\n", pBoneIdxs[i]);
			pMotionData = pMotion->pMotionSet[i].pMotion;

			for( j = 0; j < (int) pMotion->usFrames; j++)
				WritePSXFrame( str, pMotionData + j);
		}
	}


}

void WritePSXFrame( mpostream &str, ts_K2AngAxis *pFrame)
{
ts_Quatvw			Quat;

	qatFromAxisAngle( &(pFrame->Axis), pFrame->fRot, &Quat);
	qatNormalize( &Quat);
	
	printf( "  %f %f %f %f ", Quat.V[0], Quat.V[1], Quat.V[2], Quat.W);
	printf( " - %d %d %d\n", (long) (240.0f * pFrame->Trans[X]), 
		(long) (240.0f * pFrame->Trans[Y]), (long) (240.0f * pFrame->Trans[Z]));

	str << Quat;
	str << (long) (240.0f * pFrame->Trans[0]);
	str << (long) (240.0f * pFrame->Trans[1]);
	str << (long) (240.0f * pFrame->Trans[2]);

}

mpostream & operator << ( mpostream &str, ts_K2MFHdr &Hdr)
{
	str.Write( (unsigned char *) Hdr.ID, 4);
	str << Hdr.usVer;
	str << Hdr.usFrames;
	str << Hdr.usBones;

	return str;
}

mpostream & operator << ( mpostream &str, ts_Quatvw &Quat)
{
	str << Quat.V;
	str << Quat.W;

	return str;
}

mpostream & operator << ( mpostream &str, l_Vct3f &Vct)
{
	str << Vct[0];
	str << Vct[1];
	str << Vct[2];

	return str;
}

