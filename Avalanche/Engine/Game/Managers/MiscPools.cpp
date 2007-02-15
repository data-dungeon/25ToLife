/*************************************/
/*                                   */
/*   miscpools.c                     */
/*   big juju prototype  06/01/99    */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   miscellaneous pool systems      */
/*                                   */
/*************************************/

/* this file handles simple free/active pool systems for things that need to be allocated on the fly, like
matrices */

/******************* includes ****************************/

#include "Game/GamePCH.h"



/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/* matrix pool */

struct ts_MatrixPool
{
	/* record passed initialization values */

	int					nMaxMatrices;

	/* malloc'd buffer, linked lists */

	ts_PoolMatrix		*pMatrixAlloc;
	ts_SLinkList		FreeList;
	ts_SLinkList		ActiveList;
};

/* frustum pool */

struct ts_FrustumPool
{
	/* record passed initialization values */

	int					nMaxFrustums;

	/* malloc'd buffer, linked lists */

	ts_PoolFrustum		*pFrustumAlloc;
	ts_SLinkList		FreeList;
	ts_SLinkList		ActiveList;
};

/* instance pool */

struct CInstancePool
{
	/* record passed initialization values */

	int					nMaxInstances;

	/* malloc'd buffer, linked lists */

	ts_PoolInstance	*pInstanceAlloc;
	ts_SLinkList		FreeList;
	ts_SLinkList		ActiveList;
};

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/* matrix pool */

static ts_MatrixPool MatrixPool;

/* frustum pool */

static ts_FrustumPool FrustumPool;

/* instance pool */

static CInstancePool InstancePool;

/******************* local prototypes ********************/

/******************* functions ***************************/

/* initialize matrix pool */

int MatrixPool__Init(int nMaxMatrices)
{
	int i;
	ts_PoolMatrix *pMatrix;

	/* clear it */

	memset(&MatrixPool, 0, sizeof(ts_MatrixPool));
	if (nMaxMatrices == 0)
		return(true);	// nothing more to do

	/* allocate records, initialize linked lists */

	MEM_SET_ALLOC_NAME("Matrix pool list");
	MatrixPool.nMaxMatrices = nMaxMatrices;
	MatrixPool.pMatrixAlloc = (ts_PoolMatrix *) memAlloc(MatrixPool.nMaxMatrices * sizeof(ts_PoolMatrix));
	SLinkList__Init(&MatrixPool.FreeList, offsetof(ts_PoolMatrix, pNext));
	SLinkList__LinkBlock(&MatrixPool.FreeList, MatrixPool.pMatrixAlloc, MatrixPool.nMaxMatrices, sizeof(ts_PoolMatrix));
	SLinkList__Init(&MatrixPool.ActiveList, offsetof(ts_PoolMatrix, pNext));
	
	/* initialize records */

	pMatrix = MatrixPool.pMatrixAlloc;
	for (i = 0; i < MatrixPool.nMaxMatrices; i++, pMatrix++)
	{
		pMatrix->dh.u8DataType = DATA_MATRIX;
		pMatrix->dh.u8Priority = 0;
#ifdef PS2
		ASSERT(IS_VU_ALIGNED(&pMatrix->mMatrix));	// matrices must be aligned for fast vu0 processing on ps2
#endif
	}
	
	/* success */

	return(TRUE);
}

/* reset */

void MatrixPool__Reset(void)
{
	/* free all matrices */

	while(MatrixPool.ActiveList.pHead)
		MatrixPool__Free((ts_PoolMatrix *) MatrixPool.ActiveList.pHead);
}

/* shutdown */

void MatrixPool__Shutdown(void)
{
	if (MatrixPool.nMaxMatrices == 0)
		return;	// nothing to do
	ASSERT(MatrixPool.pMatrixAlloc);	// if this hits, Init was never called
	if (MatrixPool.pMatrixAlloc)
		memFree(MatrixPool.pMatrixAlloc);
	memset(&MatrixPool, 0, sizeof(ts_MatrixPool));
}

/* alloc */

ts_PoolMatrix *MatrixPool__Alloc(void *pOwner)
{
	ts_PoolMatrix *pMatrix;

	/* check owner */

//yuch-- we are not using the data type scheme. this should all be removed...
//	ASSERT(VALID_DATA_TYPE(pOwner));

	/* grab a matrix record from the free list */

	pMatrix = (ts_PoolMatrix *) SLinkList__GetAndRemoveHead(&MatrixPool.FreeList);
	if (pMatrix == NULL)
		return(NULL);
	ASSERT(pMatrix->dh.u8DataType == DATA_MATRIX);
//yuch-- check priority, steal oldest? clean up using owner type and pointer?

	/* add to head of active list */

	SLinkList__AddLinkToHead(&MatrixPool.ActiveList, pMatrix);
	pMatrix->pOwner = pOwner;								// record owner
	return(pMatrix);
}

void MatrixPool__Free(ts_PoolMatrix *pMatrix)
{
	/* memory check, list check */

	ASSERT(pMatrix >= MatrixPool.pMatrixAlloc);
	ASSERT(SLinkList__InList(&MatrixPool.ActiveList, pMatrix));

	/* move from active to free list */

	SLinkList__RemoveLink(&MatrixPool.ActiveList, pMatrix);
	SLinkList__AddLinkToHead(&MatrixPool.FreeList, pMatrix);
}

/********************** frustum pool **********************/

/* initialize frustum pool */

int FrustumPool__Init(int nMaxFrustums)
{
	/* clear it */

	memset(&FrustumPool, 0, sizeof(ts_FrustumPool));
	if (nMaxFrustums == 0)
		return(true);	// nothing more to do

	/* allocate records, initialize linked lists */

	MEM_SET_ALLOC_NAME("Frustum pool list");
	FrustumPool.nMaxFrustums = nMaxFrustums;
	FrustumPool.pFrustumAlloc = (ts_PoolFrustum *) memAlloc(FrustumPool.nMaxFrustums * sizeof(ts_PoolFrustum));
	SLinkList__Init(&FrustumPool.FreeList, offsetof(ts_PoolFrustum, pNext));
	SLinkList__LinkBlock(&FrustumPool.FreeList, FrustumPool.pFrustumAlloc, FrustumPool.nMaxFrustums, sizeof(ts_PoolFrustum));
	SLinkList__Init(&FrustumPool.ActiveList, offsetof(ts_PoolFrustum, pNext));
	
	/* success */

	return(TRUE);
}

/* reset */

void FrustumPool__Reset(void)
{
	/* free all frustums */

	while(FrustumPool.ActiveList.pHead)
		FrustumPool__Free((ts_PoolFrustum *) FrustumPool.ActiveList.pHead);
}

/* shutdown */

void FrustumPool__Shutdown(void)
{
	if (FrustumPool.nMaxFrustums == 0)
		return;	// nothing to do
	ASSERT(FrustumPool.pFrustumAlloc);	// if this hits, Init was never called
	if (FrustumPool.pFrustumAlloc)
		memFree(FrustumPool.pFrustumAlloc);
	memset(&FrustumPool, 0, sizeof(ts_FrustumPool));
}

/* alloc */

ts_PoolFrustum *FrustumPool__Alloc()
{
	ts_PoolFrustum *pFrustum;

	/* grab a frustum record from the free list */

	pFrustum = (ts_PoolFrustum *) SLinkList__GetAndRemoveHead(&FrustumPool.FreeList);
	if (pFrustum == NULL)
		return(NULL);
//yuch-- check priority, steal oldest? clean up using owner type and pointer?

	/* add to head of active list */

	SLinkList__AddLinkToHead(&FrustumPool.ActiveList, pFrustum);
	return(pFrustum);
}

void FrustumPool__Free(ts_PoolFrustum *pFrustum)
{
	/* memory check, list check */

	ASSERT(pFrustum >= FrustumPool.pFrustumAlloc);
	ASSERT(SLinkList__InList(&FrustumPool.ActiveList, pFrustum));

	/* move from active to free list */

	SLinkList__RemoveLink(&FrustumPool.ActiveList, pFrustum);
	SLinkList__AddLinkToHead(&FrustumPool.FreeList, pFrustum);
}

/********************** instance pool **********************/

/* initialize instance pool */

int InstancePool__Init(int nMaxInstances)
{
	/* clear it */

	memset(&InstancePool, 0, sizeof(CInstancePool));
	if (nMaxInstances == 0)
		return(true);	// nothing more to do

	/* allocate records, initialize linked lists */

	MEM_SET_ALLOC_NAME("Instance pool list");
	InstancePool.nMaxInstances = nMaxInstances;
	InstancePool.pInstanceAlloc = (ts_PoolInstance *) memAlloc(InstancePool.nMaxInstances * sizeof(ts_PoolInstance));
	SLinkList__Init(&InstancePool.FreeList, offsetof(ts_PoolInstance, pNext));
	SLinkList__LinkBlock(&InstancePool.FreeList, InstancePool.pInstanceAlloc, InstancePool.nMaxInstances, sizeof(ts_PoolInstance));
	SLinkList__Init(&InstancePool.ActiveList, offsetof(ts_PoolInstance, pNext));
	
	/* initialize records */

	ts_PoolInstance *pPoolInstance = InstancePool.pInstanceAlloc;
	for (int i = 0; i < InstancePool.nMaxInstances; i++, pPoolInstance++)
	{
		pPoolInstance->Instance.Reset();
		pPoolInstance->Instance.SetName( "InstancePool" );
		pPoolInstance->Instance.Matrix().SetYRotation(Math::Pi);
	}
	
	/* success */

	return(TRUE);
}

/* reset */

void InstancePool__Reset(void)
{
	/* free all matrices */

	while(InstancePool.ActiveList.pHead)
		InstancePool__Free((ts_PoolInstance *) InstancePool.ActiveList.pHead);
}

/* shutdown */

void InstancePool__Shutdown(void)
{
	if (InstancePool.nMaxInstances == 0)
		return;	// nothing to do
	ASSERT(InstancePool.pInstanceAlloc);	// if this hits, Init was never called
	if (InstancePool.pInstanceAlloc)
		memFree(InstancePool.pInstanceAlloc);
	memset(&InstancePool, 0, sizeof(CInstancePool));
}

/* alloc */

ts_PoolInstance *InstancePool__Alloc(void *pOwner)
{
	/* grab a instance record from the free list */

	ts_PoolInstance *pInstance = (ts_PoolInstance *) SLinkList__GetAndRemoveHead(&InstancePool.FreeList);
	if (pInstance == NULL)
		return(NULL);
//yuch-- check priority, steal oldest? clean up using owner type and pointer?

	/* add to head of active list */

	SLinkList__AddLinkToHead(&InstancePool.ActiveList, pInstance);
	pInstance->pOwner = pOwner;								// record owner
	return(pInstance);
}

void InstancePool__Free(ts_PoolInstance *pInstance)
{
	/* memory check, list check */

	ASSERT(pInstance >= InstancePool.pInstanceAlloc);
	ASSERT(SLinkList__InList(&InstancePool.ActiveList, pInstance));

	/* move from active to free list */

	SLinkList__RemoveLink(&InstancePool.ActiveList, pInstance);
	SLinkList__AddLinkToHead(&InstancePool.FreeList, pInstance);
}

