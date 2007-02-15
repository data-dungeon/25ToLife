/* --------------------------------------------------
 * File    : SOM_DEBG.C
 * Created : Tue Dec  1 11:40:49 1998
 * Descript: 
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include <t_macros.h>
#include <t_struct.h>
#include <t_som.h>
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

void ShowSOMFaceUP( ts_SOMFaceUP *pFace);

void ShowSOMUP( ts_SOMUP *pModel)
{
unsigned long		l;

	printf( "Flags : %08x\n", pModel->ModelFlags);
	printf( "Vtxs  : %d\n", pModel->ulVtxCount);
	printf( "Faces : %d\n", pModel->ulFaceCount);

	for( l = 0; l < pModel->ulFaceCount; l++)
		ShowSOMFaceUP( pModel->pFaceBuffer + l);

	printf( "\n");
}

void ShowSOMFaceUP( ts_SOMFaceUP *pFace)
{
unsigned long		i, j;

	printf( "Flags   : %08x Color: [%f, %f, %f, %f]\n", pFace->Flags, pFace->Color.R, pFace->Color.G, pFace->Color.B, pFace->Color.A );
	printf( "VtxIdxs : [%d,%d,%d,%d]\n", pFace->usVtxIdxs[0], pFace->usVtxIdxs[1], pFace->usVtxIdxs[2], pFace->usVtxIdxs[3]);
	printf( "Tex Cnt : %d\n", pFace->ulTexCount);
	for( i = 0; i < pFace->ulTexCount; i++)
	{
		printf( "  %02d:  ID: %d\n", i, pFace->pTextures[i].TexID);
		printf( "     UVs: ");
		for( j = 0; j < 4; j++)
			printf( "[%f,%f] ", pFace->pTextures[i].TexUV[j].X, pFace->pTextures[i].TexUV[j].Y);
		printf( "\n");
	}
}

long ShowSOMFace( ts_SOMFace *pFace);

void ShowSOM( ts_SOM *pModel)
{
unsigned long		ulThisFaceSize;
unsigned long		ulFaceData;
unsigned char		*pCurFace;


	printf( "Flags : %08x\n", pModel->ModelFlags);
	printf( "Vtxs  : %d\n", pModel->ulVtxCount);
	if( pModel->ModelFlags & SOMM_SET_FACE)
		printf( "Set Face Size : %d\n", pModel->ulSetFaceSize);
	else
		printf( "Face Buf Size : %d\n", pModel->ulFaceBufferSize);

	ulFaceData = 0;
	pCurFace = (unsigned char *) pModel->pFaceBuffer;
	
	while( ulFaceData < pModel->ulFaceBufferSize)
	{
		ulThisFaceSize = ShowSOMFace( (ts_SOMFace *) pCurFace);
		pCurFace += ulThisFaceSize;
		ulFaceData += ulThisFaceSize;
	}
		
}

long ShowSOMFace( ts_SOMFace *pFace)
{
unsigned long		l, m;
unsigned char		*pFaceData;
unsigned short		*pShort;
unsigned long		ulVtxs;

ts_f2DCoord			*pSTex, *pMTex;

	ulVtxs = (pFace->Flags & SOMF_TRIANGLE) ? 3 : 4;

	if( (pFace->Flags & 0x03) == SOMF_CLR_PACKED)
		printf( "Flags   : %08x Color: [%f, %f, %f, %f]\n", pFace->Flags, 
					RGBB_TO_F( pFace->PackedColor.R), RGBB_TO_F( pFace->PackedColor.G), RGBB_TO_F( pFace->PackedColor.B), RGBB_TO_F( pFace->PackedColor.A));
	else
		printf( "Flags : %08x \n", pFace->Flags);

	pFaceData = (unsigned char *) (pFace + 1);

	pShort = (unsigned short *) pFaceData;

	if( ulVtxs == 3)
		printf( "VtxIdxs : [%d,%d,%d]\n", pShort[0], pShort[1], pShort[2], pShort[3]); 
	else
		printf( "VtxIdxs : [%d,%d,%d,%d]\n", pShort[0], pShort[1], pShort[2], pShort[3]); 

	pFaceData += ulVtxs * sizeof( unsigned short);

	if( pFace->Flags & SOMF_CLR_SINGLE)
	{
		if( pFace->Flags & SOMF_CLR_FLOAT)
		{
			ts_fRGBA *pBClr = (ts_fRGBA *) pFaceData;
			printf( "CLRF : %f %f %f %f\n", pBClr->R, pBClr->G, pBClr->B, pBClr->A);
			pFaceData += sizeof( ts_fRGBA);
		}
		else
		{
			ts_bRGBA *pBClr = (ts_bRGBA *) pFaceData;
			printf( "CLRB : %d %d %d %d\n", pBClr->R, pBClr->G, pBClr->B, pBClr->A);
			pFaceData += sizeof( ts_bRGBA);
		}
	}
	if( pFace->Flags & SOMF_CLR_VERTEX)
	{
		if( pFace->Flags & SOMF_CLR_FLOAT)
		{
			ts_fRGBA *pBClr = (ts_fRGBA *) pFaceData;
			for( l = 0; l < ulVtxs; l++)
			{
				printf( "CLRF : %f %f %f %f\n", pBClr->R, pBClr->G, pBClr->B, pBClr->A);
				pBClr++;
			}
			pFaceData += sizeof( ts_fRGBA) * ulVtxs;
		}
		else
		{
			ts_bRGBA *pBClr = (ts_bRGBA *) pFaceData;
			for( l = 0; l < ulVtxs; l++)
			{
				printf( "CLRB : %d %d %d %d\n", pBClr->R, pBClr->G, pBClr->B, pBClr->A);
				pBClr++;
			}
			pFaceData += sizeof( ts_bRGBA) * ulVtxs;
		}
	}
	if( pFace->Flags & SOMF_NORMAL)
	{
		ts_f3DCoord *pNormal = (ts_f3DCoord *) pFaceData;
		printf( "NRML : [%f,%f,%f]\n", pNormal->X, pNormal->Y, pNormal->Z);
		pFaceData += sizeof( ts_f3DCoord);
	}

	if( (pFace->Flags & 0x03) == SOMF_SNGL_TEXTURE)
	{
		printf( "Tex Cnt : 1\n");
		printf( "  00:  ID: %d\n   UV : ", pFace->TexID);
		pSTex = (ts_f2DCoord *) pFaceData;
		for( l = 0; l < ulVtxs; l++)
			printf( "[%f,%f] ", pSTex[l].X, pSTex[l].Y);

		printf( "\n");
		pFaceData += ulVtxs * sizeof( ts_f2DCoord);

	}
	else if( (pFace->Flags & 0x03) == SOMF_MULT_TEXTURE)
	{
		printf( "Tex Cnt : %d\n", pFace->ulTexCount);
		for( m = 0; m < pFace->ulTexCount; m++)
		{
			printf( "  %02d: %d\n   UV : ", m, *((unsigned short *) pFaceData));

			pFaceData += sizeof( unsigned short);

			pMTex = (ts_f2DCoord *) pFaceData;
			for( l = 0; l < ulVtxs; l++)
				printf( "[%f,%f] ", pMTex[l].X, pMTex[l].Y);

			printf( "\n");
			pFaceData += ulVtxs * sizeof( ts_f2DCoord);
		}
	}
	else
		printf( "Tex Cnt : 0\n");

	return ((unsigned long) pFaceData) - ((unsigned long) pFace);
}

