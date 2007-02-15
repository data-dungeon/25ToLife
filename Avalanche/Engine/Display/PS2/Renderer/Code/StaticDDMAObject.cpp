#include "VUDataHeaders.h"
#include "StaticDDMAObject.h"

		
void * StaticDDMAObject::createDMAModel( const DMAModelHdr & Hdr)
{
	if( Hdr.mFlags & OBJ_FLAG_DYNAMIC_TEXTURE)
	{
		void *			pTableData = (void *) Hdr.getDMAData( );
		unsigned int 	numEntries = *((unsigned int *) pTableData)++;
		DTInfo *			pTable = (DTInfo *) pTableData;
		
		for( unsigned int i = 0; i < numEntries; i++)
			pTable[i].uiBlockOffset += (unsigned int) &Hdr;
			
	}
		
	return Hdr.getDMAData( );
}
