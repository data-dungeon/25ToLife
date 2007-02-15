/* --------------------------------------------------
 * File    : SOM_CONV.C
 * Created : Fri Oct  2 12:47:20 1998
 * Descript:
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <t_som.h>
#include <mathlib.h>

#include <som_util.h>
#include "som_supt.h"

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/
typedef struct FaceLink
{
	struct FaceLink	*pNext;
	ts_SOMFaceUP		*pFace;
} ts_FaceLink;

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
void				_somBuildTri( ts_SOMFaceUP *pIn, ts_SOMFaceUP **ppOut);
void				_somCopyFaceDataDown( ts_SOMFaceUP *pFace, unsigned long ulA, unsigned long ulB);
ts_FaceLink	*	_somBuildFaceLinkList( ts_SOMUP *pModel);
int				somCollapseQuad( ts_SOMFaceUP *pF1, ts_SOMFaceUP *pF2, ts_f3DCoord *pVtx);
void				CreateQuad( short *pF1Points, short F2Point, ts_SOMFaceUP *pF1, ts_SOMFaceUP *pF2);

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

void somConvertQuad( ts_SOMUP **ppModel)
{
unsigned long		ulCvtCount;

unsigned long		l;
unsigned long		ulQuadCount;

ts_SOMUP				*pModel;
ts_SOMFaceUP		*pFace;
ts_SOMFaceUP		*pNewFaceBuffer;

ts_FaceLink			*pFaceList;
ts_FaceLink			*pBase;
ts_FaceLink			*pRemove;

ts_FaceLink			**ppComp;

	pModel = *ppModel;

	pFace = pModel->pFaceBuffer;
	
	ulQuadCount = 0;

	for( l = 0; l < pModel->ulFaceCount; l++)
	{
		if( !(pFace[l].Flags & SOMF_TRIANGLE))
			ulQuadCount++;
	}

	if( ulQuadCount == pModel->ulFaceCount)
		return;

	ulCvtCount = 0;

	pFaceList = _somBuildFaceLinkList( *ppModel);

	pBase = pFaceList;
	
	while( pBase  &&  pBase->pNext)
	{
		ppComp = &(pBase->pNext);

		while( *ppComp)
		{
			if( somCollapseQuad( pBase->pFace, (*ppComp)->pFace, pModel->pVtxBuffer))
			{
				ulCvtCount++;

				pRemove = *ppComp;
				*ppComp =	(*ppComp)->pNext;

				if( pRemove->pFace->Flags & SOMF_SOME_TEXTURE)
					free( pRemove->pFace->pTextures);

				free( pRemove);

				break;
			}
			else
			{
				ppComp = &((*ppComp)->pNext);
			}
		}
		
		pBase = pBase->pNext;
	}

	pModel->ulFaceCount -= ulCvtCount;

	pNewFaceBuffer = (ts_SOMFaceUP *) malloc( pModel->ulFaceCount * sizeof( ts_SOMFaceUP));

	l = 0;
	pBase = pFaceList;

	while( pBase)
	{
		memcpy( pNewFaceBuffer + l, pBase->pFace, sizeof( ts_SOMFaceUP));

		pRemove = pBase;

		pBase = pBase->pNext;
		l++;

		free( pRemove);
	}

	free( pModel->pFaceBuffer);

	pModel->pFaceBuffer = pNewFaceBuffer;

}

float UVMaxError( ts_f2DCoord *pUV1, ts_f2DCoord *pUV2)
{
float					fMaxError = 0.0f;

	if( fabs( pUV1->X - pUV2->X) > fMaxError)
		fMaxError = (float) fabs( pUV1->X - pUV2->X);

	if( fabs( pUV1->Y - pUV2->Y) > fMaxError)
		fMaxError = (float) fabs( pUV1->Y - pUV2->Y);

	return fMaxError;
}

float ColorMaxError( ts_fRGBA *pC1, ts_fRGBA *pC2)
{
float					fMaxError = 0.0f;
float					fError;

	fError = (float) fabs( pC1->R - pC2->R);

	if( fError > fMaxError)
		fMaxError = fError;

	fError = (float) fabs( pC1->G - pC2->G);

	if( fError > fMaxError)
		fMaxError = fError;

	fError = (float) fabs( pC1->B - pC2->B);

	if( fError > fMaxError)
		fMaxError = fError;

	fError = (float) fabs( pC1->A - pC2->A);

	if( fError > fMaxError)
		fMaxError = fError;

	return fMaxError;
}

int somCollapseQuad( ts_SOMFaceUP *pF1, ts_SOMFaceUP *pF2, ts_f3DCoord *pVtx)
{
int					i, j, t;
int					iCanQuad;
short					T1I1, T1I2, T1I3, T2I1, T2I2, T2I3;
unsigned short		T1P1, T1P2, T2P1, T2P2;
short					sT1Idx[] = { 0, 1, 2, 0 };
short					sT2Idx[] = { 2, 1, 0, 2 };
short					sT1QuadOrder[3][3] = { { 1, 2, 0}, { 2, 0, 1}, { 0, 1, 2 } };
short					sT1OtherPoint[] = { 2, 0, 1 };
short					sT2OtherPoint[] = { 0, 2, 1 };

float					fError = .001f;
float					fUVError = .001f;
float					fColorError = .001f;


l_Vct3f				V1, V2, V1xV2;
l_Vct3f				FNorm;

	if( pF1->Flags != pF2->Flags)
		return 0;

	if( pF1->Flags & SOMF_SOME_TEXTURE)
	{
		if( (pF1->Flags & SOMF_MULT_TEXTURE)  &&  (pF1->ulTexCount != pF1->ulTexCount))
			return 0;

		for( i = 0; i < (int) pF1->ulTexCount; i++)
		{
			if( pF1->pTextures[i].TexID != pF2->pTextures[i].TexID)
				return 0;

		}
	}

	if( pF1->Flags & SOMF_CLR_SINGLE)
	{
		if( ColorMaxError( &(pF1->Color), &(pF2->Color)) > fColorError)
			return 0;
	}

	somFaceNormal( pF1, pVtx, (ts_f3DCoord *) &FNorm);
	somFaceNormal( pF2, pVtx, (ts_f3DCoord *) &V1);

	for( i = 0; i < 3; i++)
		if( fabs( FNorm[i] - V1[i]) > fError)
			return 0;

	for( i = 0; i < 3; i++)
	{
		T1I1 = sT1Idx[i];
		T1I2 = sT1Idx[i+ 1];

		T1P1 = pF1->usVtxIdxs[T1I1];
		T1P2 = pF1->usVtxIdxs[T1I2];

		for( j = 0; j < 3; j++)
		{
			T2I1 = sT2Idx[j];
			T2I2 = sT2Idx[j + 1];

			T2P1 = pF2->usVtxIdxs[T2I1];
			T2P2 = pF2->usVtxIdxs[T2I2];

			if( T1P1 == T2P1  &&  T1P2 == T2P2)
			{
				iCanQuad = 1;

				// Check UV's
				if( pF1->Flags & SOMF_SOME_TEXTURE)
				{
					for( t = 0; t < (int) pF1->ulTexCount; t++)
					{
						if( UVMaxError( & (pF1->pTextures[t].TexUV[T1I1]), & (pF2->pTextures[t].TexUV[T2I1])) > fUVError)
						{	
							iCanQuad = 0;
						}
						else if( UVMaxError( & (pF1->pTextures[t].TexUV[T1I2]), & (pF2->pTextures[t].TexUV[T2I2])) > fUVError)
						{
							iCanQuad = 0;
						}
					}
				}

				// Check Vertex Colors
				if( iCanQuad == 1  &&  pF1->Flags & SOMF_CLR_VERTEX)
				{
					if( ColorMaxError( & (pF1->VtxColor[T1I1]), &(pF2->VtxColor[T2I1])) > fColorError)
					{
						iCanQuad = 0;
					}
					else if( ColorMaxError( & (pF1->VtxColor[T1I2]), &(pF2->VtxColor[T2I2])) > fColorError)
					{
						iCanQuad = 0;
					}
				}

				if( iCanQuad == 1)
				{
					T1I3 = sT1OtherPoint[i];
					T2I3 = sT2OtherPoint[j];

					// Check for Convex Hull
					vctSub3f( (l_Vct3f *) (pVtx + pF2->usVtxIdxs[T2I3]), (l_Vct3f *) (pVtx + T1P1), &V1);
					vctSub3f( (l_Vct3f *) (pVtx + pF1->usVtxIdxs[T1I3]), (l_Vct3f *) (pVtx + T1P1), &V2);

					vctCross3f( &V1, &V2, &V1xV2);		
					vctNormalize3f( &V1xV2);

					if( vctDot3f( &V1xV2, &FNorm) < 0)
					{
						iCanQuad = 0;
					}
					else
					{
						vctSub3f( (l_Vct3f *) (pVtx + pF1->usVtxIdxs[T1I3]), (l_Vct3f *) (pVtx + T1P2), &V1);
						vctSub3f( (l_Vct3f *) (pVtx + pF2->usVtxIdxs[T2I3]), (l_Vct3f *) (pVtx + T1P2), &V2);

						vctCross3f( &V1, &V2, &V1xV2);
						vctNormalize3f( &V1xV2);

						if( vctDot3f( &V1xV2, &FNorm) < 0)
							iCanQuad = 0;
					}
				}

				if( iCanQuad == 1)
				{
					CreateQuad( sT1QuadOrder[i], sT2OtherPoint[j], pF1, pF2);
					return 1;
				}
			}
		}
	}

	return 0;
}


void CreateQuad( short *pF1Points, short F2Point, ts_SOMFaceUP *pF1, ts_SOMFaceUP *pF2)
{
int					i, j;

ts_SOMFaceUP		TempFace;

	memcpy( &TempFace, pF1, sizeof( ts_SOMFaceUP));

	if( pF1->Flags & SOMF_SOME_TEXTURE)
	{
		TempFace.pTextures = (ts_SOMMultTex *) malloc( pF1->ulTexCount * sizeof( ts_SOMMultTex));
		memcpy( TempFace.pTextures, pF1->pTextures, pF1->ulTexCount * sizeof( ts_SOMMultTex));
	}

	pF1->usVtxIdxs[0] = pF2->usVtxIdxs[F2Point];
	pF1->VtxColor[0] = pF2->VtxColor[F2Point];
	pF1->VtxNormal[0] = pF2->VtxNormal[F2Point];

	if( pF1->Flags & SOMF_SOME_TEXTURE)
		for( j = 0; j < (int) pF1->ulTexCount; j++)
		{
			pF1->pTextures[j].TexUV[0].X = pF2->pTextures[j].TexUV[F2Point].X;
			pF1->pTextures[j].TexUV[0].Y = pF2->pTextures[j].TexUV[F2Point].Y;
		}

	for( i = 0; i < 3; i++)
	{
		pF1->usVtxIdxs[i + 1] = TempFace.usVtxIdxs[pF1Points[i]];
		pF1->VtxColor[i + 1] = TempFace.VtxColor[pF1Points[i]];
		pF1->VtxNormal[i + 1] = TempFace.VtxNormal[pF1Points[i]];

		if( pF1->Flags & SOMF_SOME_TEXTURE)
			for( j = 0; j < (int) pF1->ulTexCount; j++)
			{
				pF1->pTextures[j].TexUV[i + 1].X = TempFace.pTextures[j].TexUV[pF1Points[i]].X;
				pF1->pTextures[j].TexUV[i + 1].Y = TempFace.pTextures[j].TexUV[pF1Points[i]].Y;
			}
	}

	pF1->Flags &= ~SOMF_TRIANGLE;

	if( pF1->Flags & SOMF_SOME_TEXTURE)
		free( TempFace.pTextures);

}



void somConvertTri( ts_SOMUP **ppModel)
{
unsigned long		l;
unsigned long		ulTriCount;

ts_SOMUP				*pModel;
ts_SOMFaceUP		*pOldFaces;
ts_SOMFaceUP		*pNewFaces;
ts_SOMFaceUP		*pCurFace;

	pModel = *ppModel;

	pOldFaces = pModel->pFaceBuffer;
	ulTriCount = pModel->ulFaceCount;

	for( l = 0; l < pModel->ulFaceCount; l++)
	{
		if( !somTriFace( pOldFaces + l))
			ulTriCount++;
	}

	if( ulTriCount == pModel->ulFaceCount)
		return;

	pNewFaces = (ts_SOMFaceUP *) malloc( ulTriCount * sizeof( ts_SOMFaceUP));
	pCurFace = pNewFaces;

	for( l = 0; l < pModel->ulFaceCount; l++)
		_somBuildTri( pOldFaces + l, &pCurFace);

	free( pModel->pFaceBuffer);

	pModel->pFaceBuffer = pNewFaces;
	pModel->ulFaceCount = ulTriCount;

}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
ts_FaceLink	*_somBuildFaceLinkList( ts_SOMUP *pModel)
{
unsigned long		ul;
ts_FaceLink			*pReturn;
ts_FaceLink			*pNew;

ts_FaceLink			**ppInsert;

	pReturn = NULL;
	ppInsert = &pReturn;

	for( ul = 0; ul < pModel->ulFaceCount; ul++)
	{
		pNew = (ts_FaceLink *) malloc( sizeof( ts_FaceLink));

		pNew->pFace = pModel->pFaceBuffer + ul;
		pNew->pNext = NULL;

		*ppInsert = pNew;

		ppInsert = &(pNew->pNext);
	}


	return pReturn;
}

void _somBuildTri( ts_SOMFaceUP *pIn, ts_SOMFaceUP **ppOut)
{
unsigned long		l, m;
ts_SOMFaceUP		*pOut;

	pOut = *ppOut;

	if( !somTriFace( pIn))
	{
		memcpy( pOut, pIn, sizeof( ts_SOMFaceUP));
		memcpy( pOut + 1, pIn, sizeof( ts_SOMFaceUP));

		somDupeFaceData( pIn, pOut + 1);

		pOut[0].Flags |= SOMF_TRIANGLE;
		pOut[1].Flags |= SOMF_TRIANGLE;

		for( l = 1; l <= 2; l++)
			_somCopyFaceDataDown( &pOut[1], l, l + 1);

		pOut[0].usVtxIdxs[3] = (unsigned short) -1;
		pOut[1].usVtxIdxs[3] = (unsigned short) -1;

		(*ppOut) += 2;
	}
	else
	{
		memcpy( pOut, pIn, sizeof( ts_SOMFaceUP));

		pOut[0].Flags |= SOMF_TRIANGLE;

		for( l = 1; l < 3; l++)
		{
			if( pOut->usVtxIdxs[l] == pOut->usVtxIdxs[l - 1])
			{
				for( m = l; m < 3; m++)
					_somCopyFaceDataDown( pOut, m, m + 1);
				break;
			}
		}
		pOut[0].usVtxIdxs[3] = (unsigned short) -1;

		(*ppOut)++;
	}
}

void _somCopyFaceDataDown( ts_SOMFaceUP *pFace, unsigned long ulA, unsigned long ulB)
{
unsigned long		l;

	pFace->usVtxIdxs[ulA] = pFace->usVtxIdxs[ulB];

	pFace->VtxColor[ulA] = pFace->VtxColor[ulB];

	if( pFace->Flags & (SOMF_SNGL_TEXTURE | SOMF_MULT_TEXTURE))
		for( l = 0; l < pFace->ulTexCount; l++)
			pFace->pTextures[l].TexUV[ulA] = pFace->pTextures[l].TexUV[ulB];

}
