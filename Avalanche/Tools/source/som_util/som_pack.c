/* --------------------------------------------------
 * File    : SOM_PACK.C
 * Created : Tue Oct  6 12:01:32 1998
 * Descript:
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <t_macros.h>
#include <t_struct.h>
#include <t_som.h>

#include <som_util.h>
/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/

#define SOM_PAK_COLOR		(SOM_PAK_CLR_BYTE | SOM_PAK_CLR_FLOAT)

#define MAX_TEXS				5
#define MAX_POLY_SIZE		(104 + (MAX_TEXS * 36))

typedef struct TempFace
{
	unsigned long	ulSize;
	unsigned char	Data[MAX_POLY_SIZE];
} ts_TempFace;

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
void					_somCheckFlags( void);
unsigned long 		somPackedFaceSize( ts_SOMFaceUP *pFace);
void 				   _somPackFace( ts_SOMFaceUP *pPacked, ts_SOMFace **ppPacked);
unsigned char		_ByteUV( float fVal);
unsigned char *	_PackTextures( unsigned char *pFaceData, unsigned long ulVtxs, ts_SOMMultTex *pTex);

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/
t_lflagbits			gblPackFlags;
t_lflagbits			gblSetFlags;

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

ts_SOM *somPack( ts_SOMUP *pModel, t_lflagbits PackFlags, t_lflagbits SetFlags)
{
unsigned long		l;
unsigned long		ulFaceBufSize;
unsigned long		ulSetFaceSize;
unsigned long		ulNormalCount;

unsigned char		*pNextFace;
ts_SOM				*pNew;

ts_TempFace			*pPackedFaces;

	gblPackFlags = PackFlags;
	gblSetFlags = SetFlags;

	_somCheckFlags( );

	pNew = (ts_SOM *) malloc( sizeof( ts_SOM));
	memset( pNew, 0, sizeof( ts_SOM));

	// Misc
	pNew->ModelFlags = pModel->ModelFlags;

	pNew->BoundingSphere = pModel->BoundingSphere;

	// Copy Vertexs
	pNew->ulVtxCount = pModel->ulVtxCount;

	pNew->pVtxBuffer = (ts_f3DCoord *) malloc( pModel->ulVtxCount * sizeof( ts_f3DCoord));
	memcpy( pNew->pVtxBuffer, pModel->pVtxBuffer, pModel->ulVtxCount * sizeof( ts_f3DCoord));

	// Copy Vertex Normals
	if( pModel->pNormals  &&  (pModel->ModelFlags & (SOMM_FACE_NORMALS | SOMM_VTX_NORMALS)))
	{
		ulNormalCount = (pModel->ModelFlags & SOMM_FACE_NORMALS) ? pModel->ulFaceCount : pModel->ulVtxCount;

		pNew->pNormals = (ts_f3DCoord *) malloc( ulNormalCount * sizeof( ts_f3DCoord));
		memcpy( pNew->pNormals, pModel->pNormals, ulNormalCount * sizeof( ts_f3DCoord));
	}

	// Pack Faces
	pNew->ulFaceCount = pModel->ulFaceCount;

	pPackedFaces = (ts_TempFace *) malloc( pModel->ulFaceCount * sizeof( ts_TempFace));

	for( l = 0; l < pModel->ulFaceCount; l++)
	{
		pNextFace = pPackedFaces[l].Data;
		_somPackFace( pModel->pFaceBuffer + l, (ts_SOMFace **) &pNextFace);

		pPackedFaces[l].ulSize = ((unsigned long) pNextFace) - ((unsigned long) pPackedFaces[l].Data);
	}

	ulSetFaceSize = pPackedFaces[0].ulSize;

	if( gblSetFlags)
	{
		ulFaceBufSize = ulSetFaceSize * pModel->ulFaceCount;
	}
	else
	{
		ulFaceBufSize = 0;
		for( l = 0; l < pModel->ulFaceCount; l++)
			ulFaceBufSize += pPackedFaces[l].ulSize;
	}

	pNew->pFaceBuffer = (ts_SOMFace *) malloc( ulFaceBufSize);

	pNextFace = (unsigned char *) pNew->pFaceBuffer;

	for( l = 0; l < pModel->ulFaceCount; l++)
	{
		memcpy( pNextFace, pPackedFaces[l].Data, pPackedFaces[l].ulSize);
		pNextFace += pPackedFaces[l].ulSize;
	}

	if( gblSetFlags)
	{
		pNew->ModelFlags |= gblSetFlags;
		pNew->ulSetFaceSize = ulSetFaceSize;
	}
	else
		pNew->ulFaceBufferSize = ulFaceBufSize;

	SFREE( pPackedFaces);

	return pNew;
}

unsigned long somPackedFaceSize( ts_SOMFaceUP *pFace)
{
unsigned char		ucTempBuffer[MAX_POLY_SIZE];

ts_SOMFace			*pRet;

	pRet = (ts_SOMFace *) ucTempBuffer;

	_somPackFace( pFace, &pRet);

	return ((unsigned long) pRet) - ((unsigned long) ucTempBuffer);
}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
void _somCheckFlags( void)
{
	if( gblPackFlags & SOM_PAK_NO_TEXTURES)
		gblSetFlags &= ~SOM_SET_TEXTURE;

	if( gblSetFlags & SOM_SET_TEXTURE)
		gblPackFlags |= SOM_PAK_SNGL_TEXTURE;

}

void _somPackFace( ts_SOMFaceUP *pUnPacked, ts_SOMFace **ppPacked)
{
bool					bWriteColor;
unsigned char		*pFaceData;

unsigned long		l, m;
unsigned long		ulTexs;
unsigned long		ulVtxs;

ts_SOMFace			*pPacked;
ts_fRGBA				Color = { 1.0f, 1.0f, 1.0f, 1.0f };
ts_fRGBA				*pColor;

	pPacked = *ppPacked;

	ulVtxs = (pUnPacked->Flags & SOMF_TRIANGLE) ? 3 : 4;

	pFaceData = (unsigned char *) (pPacked + 1);

	pPacked->Flags = 0;

	// Write Vertex Indexs
	if( ulVtxs == 3)
		pPacked->Flags |= SOMF_TRIANGLE;

	if( gblSetFlags)
		ulVtxs = 4;

	for( l = 0; l < ulVtxs; l++)
		((unsigned short *) pFaceData)[l] = pUnPacked->usVtxIdxs[l];

	pFaceData += ulVtxs * sizeof( unsigned short);

	if( pUnPacked->ulTexCount  &&  !(gblPackFlags & SOM_PAK_NO_TEXTURES))
	{
		if( gblPackFlags & SOM_PAK_SNGL_TEXTURE || pUnPacked->ulTexCount == 1)
		{
			ulTexs = 1;
			pPacked->TexID = pUnPacked->pTextures[0].TexID;
		}
		else
		{
			ulTexs = pUnPacked->ulTexCount;
			pPacked->ulTexCount = ulTexs;
		}

		pPacked->Flags |= (ulTexs == 1) ? SOMF_SNGL_TEXTURE : SOMF_MULT_TEXTURE;
	}
	else
	{
		ulTexs = 0;

		if( gblSetFlags & SOM_SET_TEXTURE)
			pPacked->Flags |= SOMF_SNGL_TEXTURE;
	}

	bWriteColor = false;

	if( gblSetFlags & SOM_SET_CLR_SINGLE)
		bWriteColor = true;

	if( ulTexs != 0  &&  gblPackFlags & SOM_PAK_CLR_WITH_TEX)
		bWriteColor = true;

	if( (pUnPacked->Flags & SOMF_CLR_SINGLE)  &&  (gblSetFlags & SOM_SET_TEXTURE))
		bWriteColor = true;


/* jlb mod 2/18/99
	if( gblPackFlags & SOM_PAK_CLR_FLOAT)
		bWriteColor = true;
 */

// Write Vertex Color Data
	if( pUnPacked->Flags & SOMF_CLR_VERTEX  ||
		 gblSetFlags & SOM_SET_CLR_VERTEX )
	{
		bWriteColor = false;

		pPacked->Flags |= SOMF_CLR_VERTEX;

		if( gblPackFlags & SOM_PAK_CLR_FLOAT)
		{
			pPacked->Flags |= SOMF_CLR_FLOAT;
			if( pUnPacked->Flags & SOMF_CLR_VERTEX)
			{
				for( l = 0; l < ulVtxs; l++)
				{
					((ts_fRGBA *) pFaceData)[l].R = pUnPacked->VtxColor[l].R;
					((ts_fRGBA *) pFaceData)[l].G = pUnPacked->VtxColor[l].G;
					((ts_fRGBA *) pFaceData)[l].B = pUnPacked->VtxColor[l].B;
					((ts_fRGBA *) pFaceData)[l].A = pUnPacked->VtxColor[l].A;
				}
			}
			else
			{
				if( pUnPacked->Flags & SOMF_CLR_SINGLE)
					pColor = &pUnPacked->Color;
				else
					pColor = &Color;

				for( l = 0; l < ulVtxs; l++)
				{
					((ts_fRGBA *) pFaceData)[l].R = pColor->R;
					((ts_fRGBA *) pFaceData)[l].G = pColor->G;
					((ts_fRGBA *) pFaceData)[l].B = pColor->B;
					((ts_fRGBA *) pFaceData)[l].A = pColor->A;
				}
			}

			pFaceData += ulVtxs * sizeof( ts_fRGBA);
		}
		else
		{
			if( pUnPacked->Flags & SOMF_CLR_VERTEX)
			{
				for( l = 0; l < ulVtxs; l++)
				{
					((ts_bRGBA *) pFaceData)[l].R = RGBF_TO_B( pUnPacked->VtxColor[l].R);
					((ts_bRGBA *) pFaceData)[l].G = RGBF_TO_B( pUnPacked->VtxColor[l].G);
					((ts_bRGBA *) pFaceData)[l].B = RGBF_TO_B( pUnPacked->VtxColor[l].B);
					((ts_bRGBA *) pFaceData)[l].A = RGBF_TO_B( pUnPacked->VtxColor[l].A);
				}
			}
			else
			{
				if( pUnPacked->Flags & SOMF_CLR_SINGLE)
					pColor = &pUnPacked->Color;
				else
					pColor = &Color;

				for( l = 0; l < ulVtxs; l++)
				{
					((ts_bRGBA *) pFaceData)[l].R = RGBF_TO_B( pColor->R);
					((ts_bRGBA *) pFaceData)[l].G = RGBF_TO_B( pColor->G);
					((ts_bRGBA *) pFaceData)[l].B = RGBF_TO_B( pColor->B);
					((ts_bRGBA *) pFaceData)[l].A = RGBF_TO_B( pColor->A);
				}
			}

			pFaceData += ulVtxs * sizeof( ts_bRGBA);
		}
	}

// Write Face Color
	if( bWriteColor == true)
	{
		pPacked->Flags |= SOMF_CLR_SINGLE;

		if( pUnPacked->Flags & SOMF_CLR_VERTEX)
			pColor = &pUnPacked->VtxColor[0];
		else if( pUnPacked->Flags & SOMF_CLR_SINGLE)
			pColor = &pUnPacked->Color;
		else
			pColor = &Color;

		if( gblPackFlags & SOM_PAK_CLR_FLOAT)
		{
			pPacked->Flags |= SOMF_CLR_FLOAT;
			memcpy( pFaceData, pColor, sizeof( ts_fRGBA));

			pFaceData += sizeof( ts_fRGBA);
		}
		else
		{
			((ts_bRGBA *) pFaceData)->R = RGBF_TO_B( pColor->R);
			((ts_bRGBA *) pFaceData)->G = RGBF_TO_B( pColor->G);
			((ts_bRGBA *) pFaceData)->B = RGBF_TO_B( pColor->B);
			((ts_bRGBA *) pFaceData)->A = RGBF_TO_B( pColor->A);

			pFaceData += sizeof( ts_bRGBA);
		}
	}
	else if( pUnPacked->Flags & SOMF_CLR_SINGLE)
	{
		pPacked->PackedColor.R = RGBF_TO_B( pUnPacked->Color.R);
		pPacked->PackedColor.G = RGBF_TO_B( pUnPacked->Color.G);
		pPacked->PackedColor.B = RGBF_TO_B( pUnPacked->Color.B);
		pPacked->PackedColor.A = RGBF_TO_B( pUnPacked->Color.A);

		pPacked->Flags |= SOMF_CLR_PACKED;
	}

// Write Face Normal
	if( ((pUnPacked->Flags & SOMF_NORMAL)  &&  (gblPackFlags & SOM_PAK_NORMALS))  ||
		 gblSetFlags & SOM_SET_NORMAL)
	{
		pPacked->Flags |= SOMF_NORMAL;

		*((ts_f3DCoord *) pFaceData) = pUnPacked->Normal;
		pFaceData += sizeof( ts_f3DCoord);
	}

// Write Texture Info
	if( ulTexs == 0  &&  gblSetFlags & SOM_SET_TEXTURE)
	{
		pFaceData += ulVtxs * sizeof( ts_f2DCoord);
	}
	else  if( ulTexs == 1)
	{
		if( gblPackFlags & SOM_PAK_UV_BYTE)
			pPacked->Flags |= SOMF_UV_BYTE;

		pFaceData = _PackTextures( pFaceData, ulVtxs, pUnPacked->pTextures);
	}
	else if( ulTexs > 1)
	{
		if( gblPackFlags & SOM_PAK_UV_BYTE)
			pPacked->Flags |= SOMF_UV_BYTE;

		for( m = 0; m < ulTexs; m++)
		{
			*((unsigned short *) pFaceData) = (unsigned short) pUnPacked->pTextures[m].TexID;
			pFaceData += sizeof( unsigned short);

			pFaceData = _PackTextures( pFaceData, ulVtxs, pUnPacked->pTextures + m);

		}
	}

	*ppPacked = (ts_SOMFace *) pFaceData;
}


unsigned char _ByteUV( float fVal)
{

	fVal -= (float) ((int) fVal);

	return (unsigned char) (fVal * 255.0f);
}

unsigned char *_PackTextures( unsigned char *pFaceData, unsigned long ulVtxs, ts_SOMMultTex *pTex)
{
unsigned long 		l;

	if( gblPackFlags & SOM_PAK_UV_BYTE)
	{
		for( l = 0; l < ulVtxs; l++)
		{
			*pFaceData++ = _ByteUV(pTex->TexUV[l].X);
			*pFaceData++ = _ByteUV(pTex->TexUV[l].Y);
		}
	}
	else
	{
		for( l = 0; l < ulVtxs; l++)
	 		((ts_f2DCoord *) pFaceData)[l] = pTex->TexUV[l];

		pFaceData += ulVtxs * sizeof( ts_f2DCoord);
	}

	return pFaceData;
}
