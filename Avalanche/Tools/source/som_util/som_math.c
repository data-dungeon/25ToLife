/* --------------------------------------------------
 * File    : SOM_MATH.C
 * Created : Tue Oct 20 16:30:50 1998
 * Descript:
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mathlib.h>
#include <t_som.h>

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
void					somBuildFaceNormals( ts_SOMUP *pModel, ts_f3DCoord *pNormals);

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

void somFaceNormal( ts_SOMFaceUP *pFace, ts_f3DCoord *pVtxList, ts_f3DCoord *pNorm)
{
ts_f3DCoord			*p0, *p1, *p2;

l_Vct3f				A, B;

	p0 = pVtxList + pFace->usVtxIdxs[0];
	p1 = pVtxList + pFace->usVtxIdxs[1];
	p2 = pVtxList + pFace->usVtxIdxs[2];

	vctSub3f( (l_Vct3f *) p2, (l_Vct3f *) p1, &A);
	vctSub3f( (l_Vct3f *) p0, (l_Vct3f *) p1, &B);

	vctCross3f( &A, &B, (l_Vct3f *) pNorm);
	vctNormalize3f( (l_Vct3f *) pNorm);

}

void somCalcFaceNormals( ts_SOMUP *pModel)
{
unsigned long		ul;

ts_f3DCoord			*pVtxList;

ts_SOMFaceUP		*pFace;

	ul = pModel->ulFaceCount;
	pFace = pModel->pFaceBuffer;
	pVtxList = pModel->pVtxBuffer;

	while( ul--)
	{
		somFaceNormal( pFace, pVtxList, &(pFace->Normal));
		pFace->Flags |= SOMF_NORMAL;
		pFace++;
	}

}


void somAddFaceNormals( ts_SOMUP *pModel)
{

	if( pModel->ModelFlags & SOMM_VTX_NORMALS)
	{
		if( pModel->pNormals)
		{
			free( pModel->pNormals);
			pModel->pNormals = NULL;
		}

		pModel->ModelFlags &= ~SOMM_VTX_NORMALS;
	}

	if( !(pModel->ModelFlags & SOMM_FACE_NORMALS)  ||  pModel->pNormals == NULL)
		pModel->pNormals = (ts_f3DCoord *) malloc( pModel->ulFaceCount * sizeof( ts_f3DCoord));

	somBuildFaceNormals( pModel, pModel->pNormals);

	pModel->ModelFlags |= SOMM_FACE_NORMALS;

}


void somAddVertexNormals( ts_SOMUP *pModel)
{
unsigned long		l, m, ulVC, ulThisV;
unsigned long		ulVtxs;

unsigned long		*pFaceCounts;

float					fInvCount;

ts_SOMFaceUP		*pFace;

ts_f3DCoord			*pFaceNormals;

	if( pModel->ModelFlags & SOMM_FACE_NORMALS)
	{
		if( pModel->pNormals)
		{
			free( pModel->pNormals);
			pModel->pNormals = NULL;
		}

		pModel->ModelFlags &= ~SOMM_FACE_NORMALS;
	}

	if( !(pModel->ModelFlags & SOMM_VTX_NORMALS)  ||  pModel->pNormals == NULL)
		pModel->pNormals = (ts_f3DCoord *) malloc( pModel->ulVtxCount * sizeof( ts_f3DCoord));

	memset( pModel->pNormals, 0, pModel->ulVtxCount * sizeof( ts_f3DCoord));

	// Build Face Normals
	pFaceNormals = (ts_f3DCoord *) malloc( pModel->ulFaceCount * sizeof( ts_f3DCoord));
	somBuildFaceNormals( pModel, pFaceNormals);

	// Alloc Vtx Normals
	ulVtxs = pModel->ulVtxCount;

	pFaceCounts = (unsigned long *) malloc( ulVtxs * sizeof( ts_f3DCoord));

	memset( pFaceCounts, 0, ulVtxs * sizeof( unsigned long));

	pFace = pModel->pFaceBuffer;

	for( l = 0; l < pModel->ulFaceCount; l++)
	{

		ulVC = (pFace->Flags & SOMF_TRIANGLE) ? 3 : 4;

		for( m = 0; m < ulVC; m++)
		{
			ulThisV = pFace->usVtxIdxs[m];

			pModel->pNormals[ulThisV].X += pFaceNormals[l].X;
			pModel->pNormals[ulThisV].Y += pFaceNormals[l].Y;
			pModel->pNormals[ulThisV].Z += pFaceNormals[l].Z;

			pFaceCounts[ulThisV]++;
		}

		pFace++;
	}

	for( l = 0; l < ulVtxs; l++)
	{
		if( pFaceCounts[l])
		{
			fInvCount = 1.0f / (float) pFaceCounts[l];

			pModel->pNormals[l].X *= fInvCount;
			pModel->pNormals[l].Y *= fInvCount;
			pModel->pNormals[l].Z *= fInvCount;

			vctNormalize3f( (l_Vct3f *) (pModel->pNormals + l));
		}
	}

	pModel->ModelFlags |= SOMM_VTX_NORMALS;

	free( pFaceNormals);
	free( pFaceCounts);
}

void somMove( ts_SOMUP *pModel, float fDeltaX, float fDeltaY, float fDeltaZ)
{
unsigned long		l;
ts_f3DCoord			*pVtxs;

	pVtxs = pModel->pVtxBuffer;

	for( l = 0; l < pModel->ulVtxCount; l++)
	{
		pVtxs->X += fDeltaX;
		pVtxs->Y += fDeltaY;
		pVtxs->Z += fDeltaZ;

		pVtxs++;
	}
}

void somScale( ts_SOMUP *pModel, float fScaleX, float fScaleY, float fScaleZ)
{
unsigned long		l;
ts_f3DCoord			*pVtxs;

	pVtxs = pModel->pVtxBuffer;

	for( l = 0; l < pModel->ulVtxCount; l++)
	{
		pVtxs->X *= fScaleX;
		pVtxs->Y *= fScaleY;
		pVtxs->Z *= fScaleZ;

		pVtxs++;
	}
}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
void somBuildFaceNormals( ts_SOMUP *pModel, ts_f3DCoord *pNormals)
{
unsigned long		ul;

ts_f3DCoord			*pVtxList;

ts_SOMFaceUP		*pFace;


	ul = pModel->ulFaceCount;
	pFace = pModel->pFaceBuffer;
	pVtxList = pModel->pVtxBuffer;

	while( ul--)
	{
		somFaceNormal( pFace, pVtxList, pNormals);
		pFace++;
		pNormals++;
	}
}



