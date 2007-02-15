/* --------------------------------------------------
 * File    : SOM_TESS.C
 * Created : Mon Mar 22 11:46:44 1999
 * Descript:
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <mathlib.h>
#include <t_som.h>
/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/
typedef struct VtxLink
{
	struct VtxLink		*pNext;
	l_Vct3f 				Vtx;
} ts_VtxLink;


typedef struct FaceLink
{
	struct FaceLink	*pNext;
	ts_SOMFaceUP		Face;
} ts_FaceLink;

typedef struct CustList
{
	unsigned long		ulCount;
	void					*pBase;
	void 					**ppInsert;
} ts_CustList;

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
ts_CustList				MasterVtxList;

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

int somSubDivide( ts_SOMUP *pModel, float fMaxLen, ts_SOMUP **ppNewModel)
{
int					l;
int					NeedsDivision;

	sdivInit( );

	NeedsSubDivision = 0;

	for( l = 0; l < pModel->ulFaceCount; l++)
	{
		if( sdivCanSubDivide( pModel->pFaceBuffer + l, pModel->pVtxBuffer, fMaxLen))
		{
			NeedsSubDivision = 1;
			break;
		}
	}

	if( !NeedsSubDivision)
		return 0;

	for( l = 0; l < pModel->ulVtxCount; l++)
		sdivAddVertex( (l_Vtx3f *) (pModel->pVtxBuffer + l);

	for( l = 0; l < pModel->



}
/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
void sdivInit( void)
{
	MasterVtxList.ulCount = 0;
	MasterVtxList.pBase = NULL;
	MasterVtxList.ppInsert = &( MasterVtxList.pBase);

	MasterFaceList.ulCount = 0;
	MasterFaceList.pBase = NULL;
	MasterFaceList.ppInsert = &( MasterFaceList.pBase);
}


unsigned long sdivAddVertex( l_Vct3f *pVtx)
{
ts_VtxLink			*pNew;

	pNew = (ts_VtxLink *) malloc( sizeof( ts_VtxLink));

	pNew->pNext = *(MasterVtxList.ppInsert);

	MasterVtxList.ppInsert = &( (void *) pNew->pNext);

	pNew->Vtx = *pVtx;

	return MasterVtxList.ulCount++;
}

unsigned long sdivAddFace( ts_SOMFaceUP *pFace)
{
ts_FaceLink			*pNew;

	pNew = (ts_FaceLink *) malloc( sizeof( ts_FaceLink));

	pNew->pNext = *(MasterFaceList.ppInsert);

	MasterFaceList.ppInsert = &(pNew->pNext);

	pNew->Face = *pFace;

	return MasterFaceList.ulCount++;
}

int sdivCanSubDivide( ts_SOMFaceUP *pFace, ts_f3DCoord *pVtxList, float fMaxLen)
{
int					i, v1, v2;
int					iVtxs;

float					fLen;
l_Vct3f				Diff;
l_Vct3f				*pVtxs[4];


	iVtxs = (pFace->Flags & SOMF_TRIANGE) ? 3 : 4;

	for( i = 0; i < iVtxs; i++)
	{
		v1 = i;

		if( i == (iVtxs - 1))
				v2 = 0;
		else
				v2 = i + 1;

		vctSub3f( 	(l_Vct3f *) (pVtxs + pFace->usVtxIdxs[v2]),
						(l_Vct3f *) (pVtxs + pFace->usVtxIdxs[v1]), &Diff);

		fLen = VctLength3f( &Diff);

		if( fLen > fMaxLen)
				return 1;
	}

	return 0;

}