/* stack.cpp */

/******************* includes ****************************/

#include "Layers/LayersPCH.h"





/* -----------------8/3/98 3:27PM--------------------
 * Local Defines
 * --------------------------------------------------*/

/* -----------------8/3/98 3:27PM--------------------
 * Local Data
 * --------------------------------------------------*/

/* -----------------8/3/98 3:27PM--------------------
 * Local Prototypes
 * --------------------------------------------------*/

/* -----------------8/3/98 3:28PM--------------------
 * Local Data
 * --------------------------------------------------*/

/* -----------------8/3/98 3:28PM--------------------
 * Exported Functions
 * --------------------------------------------------*/

/* -----------------8/3/98 3:28PM--------------------
 * Local Functions
 * --------------------------------------------------*/


ts_Stack *stkNew( unsigned short usItems, unsigned short usItemSize)
{
	ts_Stack				*pNew;

	MEM_SET_ALLOC_NAME("Stack");
	pNew = (ts_Stack *) memAlloc( sizeof( ts_Stack) + (usItems * usItemSize));

	pNew->usItems = usItems;
	pNew->usCurrent = 0;
	pNew->usItemSize = usItemSize;
	pNew->pData = (unsigned char *) (pNew + 1);

	return pNew;
}

bool stkPush( ts_Stack *pStack, void *pData)
{
	if( pStack->usCurrent <= pStack->usItems)
	{
		pStack->usCurrent++;

		memcpy( pStack->pData, pData, pStack->usItemSize);

		pStack->pData += pStack->usItemSize;

		return TRUE;
	}


	return FALSE;
}

bool stkPop( ts_Stack *pStack, void *pData)
{

	if( pStack->usCurrent)
	{
		pStack->usCurrent--;
		pStack->pData -= pStack->usItemSize;

		memcpy( pData, pStack->pData, pStack->usItemSize);

		return TRUE;
	}

	return FALSE;

}

bool stkPeek( ts_Stack *pStack, void *pData)
{

	if( pStack->usCurrent)
	{
		memcpy( pData, pStack->pData - pStack->usItemSize, pStack->usItemSize);

		return TRUE;
	}

	return FALSE;

}

void stkFree( ts_Stack *pStack)
{
	memFree( pStack);
}
