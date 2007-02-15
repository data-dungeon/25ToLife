#include "ExportPch.h"
#include "MeshToWriteInfo.h"

unsigned int 			MeshToWriteInfo::d_RGBASourceBits = 8;
unsigned int 			MeshToWriteInfo::d_AlphaSourceBits = 8;

RGBADataSize			MeshToWriteInfo::d_DefRGBASize = RGBA_8888;
RGBAStorageMethod		MeshToWriteInfo::d_DefRGBAStorage = RGBA_STORE_COMPRESSED;

//NormalDataSize			MeshToWriteInfo::d_DefNormalSize = NORMAL_FLOAT;
NormalDataSize			MeshToWriteInfo::d_DefNormalSize = NORMAL_INT16;
NormalStorageMethod	MeshToWriteInfo::d_DefNormalStorage = NORMAL_STORE_ROW_STANDARD;

void MeshToWriteInfo::sortSetup( unsigned int sizeElement)
{
	d_SortElementSize = sizeElement;

	if( p_SortList)
		sortCleanup( );	

	p_SortBuffer = new unsigned char[sizeElement];


}

void MeshToWriteInfo::sortAddPointer( void *pElement)
{
	SortElement * pSortElement = new SortElement;

	pSortElement->pElement;
	pSortElement->pNext = p_SortList;

	p_SortList = pSortElement;
}


void MeshToWriteInfo::sortCleanup( void )
{
	while( p_SortList)
	{
		SortElement *pDelete = p_SortList;
		p_SortList = p_SortList->pNext;

		delete pDelete;
	}

	if( p_SortBuffer)
	{
		delete [] p_SortBuffer;
		p_SortBuffer = 0;
	}

}

void MeshToWriteInfo::SortElements( void )
{

	if( !p_SortList  ||  !p_SortList->pNext)
		return;

	for( SortElement *pE1 = p_SortList; pE1->pNext; pE1 = pE1->pNext)
	{
		SortElement *pSwap = pE1;
		
		for( SortElement *pE2 = pE1->pNext; pE2; pE2 = pE2->pNext)
			if( sortCompareFunction( pE1, pE2) > 0)
				pSwap = pE2;

		if( pSwap->pElement != pE1->pElement)
		{
			void * pTemp = pE1->pElement;
			pE1->pElement = pSwap->pElement;
			pSwap->pElement = pTemp;
		}
	}
}

void MeshToWriteInfo::SortElementList( unsigned int numElements, void *pElementList)
{
	assert( d_SortElementSize);

	if( numElements < 2)
		return;

	for( unsigned int i = 0; i < (numElements - 1); i++)
	{
		unsigned int 		j;
		unsigned int 		swap = i;

		unsigned char * 	pSortElements = (unsigned char *) pElementList;

		for( j = i + 1; j < numElements; j++)
			if( sortCompareFunction( pSortElements + (j * d_SortElementSize), pSortElements + (swap * d_SortElementSize) ) > 0)
				swap = j;

		if( swap != i)
		{
			memcpy( p_SortBuffer, pSortElements + (i * d_SortElementSize), d_SortElementSize);
			memcpy( pSortElements + (i * d_SortElementSize), pSortElements + (swap * d_SortElementSize), d_SortElementSize);
			memcpy( pSortElements + (swap * d_SortElementSize), p_SortBuffer, d_SortElementSize);
		}
	}
}
			
		


