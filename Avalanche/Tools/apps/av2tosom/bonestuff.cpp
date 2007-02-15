/* --------------------------------------------------
 * File    : BONESTUFF.CPP
 * Created : Wed Feb 24 14:47:23 1999
 * Descript: 
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include <mpostr.h>
#include <t_hierar.h>

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
typedef struct OutTree
{
	struct OutTree *pSTree;
	struct OutTree *pCTree;

	long				lSiblingNo;
	long				lChildNo;
	long				lBoneNo;

	ts_Hierarchy	*pBoneData;
} ts_OutTree;

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/

ts_OutTree *BuildOutTree( ts_Hierarchy *pBones, int *pBoneCount)
{
ts_OutTree			*pNew;



	pNew = (ts_OutTree *) malloc( sizeof( ts_OutTree));

	pNew->pBoneData = pBones;
	pNew->lBoneNo = *pBoneCount;

	(*pBoneCount)++;

	if( pBones->pChildren)
	{
		pNew->lChildNo = *pBoneCount;
		pNew->pCTree = BuildOutTree( pBones->pChildren, pBoneCount);
	}
	else
	{
		pNew->lChildNo = 0;
		pNew->pCTree = NULL;
	}

	if( pBones->pSiblings)
	{
		pNew->lSiblingNo = *pBoneCount;
		pNew->pSTree = BuildOutTree( pBones->pSiblings, pBoneCount);
	}
	else
	{
		pNew->lSiblingNo = 0;
		pNew->pSTree = NULL;
	}

	return pNew;
}

void WriteOutTree( mpostream &stream, ts_OutTree *pTree, int *pVtxIdxOffset, int iBoneCount)
{
	stream << pTree->lChildNo;
	stream << pTree->lSiblingNo;
	stream << pTree->pBoneData->nVtxIdxs;
	stream << (long) *pVtxIdxOffset;

	if( pTree->lBoneNo == 0)
		stream << iBoneCount;
	else
		stream << pTree->lBoneNo;

	printf( "%s\n", pTree->pBoneData->pName);

	*pVtxIdxOffset += pTree->pBoneData->nVtxIdxs * sizeof( short);

	if( pTree->pCTree)
		WriteOutTree( stream, pTree->pCTree, pVtxIdxOffset, iBoneCount);

	if( pTree->pSTree)
		WriteOutTree( stream, pTree->pSTree, pVtxIdxOffset, iBoneCount);
	
}

void FreeOutTree( ts_OutTree *pTree)
{

	if( pTree->pCTree)
		FreeOutTree( pTree->pCTree);

	if( pTree->pSTree)
		FreeOutTree( pTree->pSTree);

	free( pTree);
}

void WriteVtxIdxs( mpostream &stream, ts_Hierarchy *pBones)
{
int					i;

	for( i = 0; i < pBones->nVtxIdxs; i++)
		stream << pBones->pVtxIdxs[i];

	if( pBones->pChildren)
		WriteVtxIdxs( stream, pBones->pChildren);

	if( pBones->pSiblings)
		WriteVtxIdxs( stream, pBones->pSiblings);

}

void WriteBones( mpostream &stream, ts_Hierarchy *pBones)
{
ts_OutTree		*pTree;
int				iBoneCount = 0;
int				iVtxOffset;

	pTree = BuildOutTree( pBones, &iBoneCount);

	iVtxOffset = iBoneCount * 20;

	WriteOutTree( stream, pTree, &iVtxOffset, iBoneCount);

	WriteVtxIdxs( stream, pBones);

	FreeOutTree( pTree);	

}
