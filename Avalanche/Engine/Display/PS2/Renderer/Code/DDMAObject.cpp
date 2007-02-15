#include "renderassert.h"
#include <libvu0.h>
#include <eestruct.h>

#include "DDMAObject.h"
#include "PS2Renderer.h"
#include "VUDataHeaders.h"

#ifndef ASSERT
#define ASSERT RENDER_ASSERT
#endif

TextureBindCallback * DDMAObject::p_TextureBindCallback = 0;

DDMAObject::DDMAObject(
	DDMAObject_DCID		_ClassID,
	unsigned int		_Flags,
	const PRIMReg		_PRIMReg,
	void *				_DMAPtr,
	unsigned int		_DMASize,
	void *				_DTTableData,
	unsigned int		_VertCount,
	void *				_Handler,
	DDMAObject *		_Next
) :
	p_Next( _Next),
	p_Handler( _Handler),
	d_DMASize( _DMASize),
	d_VertCount( _VertCount),
	d_ClassID( _ClassID),
	p_DMAPtr( _DMAPtr),
	p_DTTableData( _DTTableData),
	d_Flags( _Flags),
	d_PRIMReg( _PRIMReg)
{

}

DDMAObject::DDMAObject( 
	const DMAModelHdr &		Hdr, 
	unsigned int 		_ExtraFlags, 
	void * 				_DMAPtr,
	unsigned int 		_DMASize,
	void *				_DTTableData,
	void *				_Handler,
	DDMAObject *	 	_Next
) :							  
	p_Next( _Next),
	p_Handler( _Handler),
	d_DMASize( _DMASize),
	d_VertCount( Hdr.mVertCount),
	d_ClassID( (DDMAObject_DCID) Hdr.mDataClassID),
	p_DMAPtr( _DMAPtr),
	p_DTTableData( _DTTableData),
	d_Flags( Hdr.mFlags | _ExtraFlags),
	d_PRIMReg( Hdr.mPRIM)
{

}
	
DDMAObject::~DDMAObject( )
{
	if( p_DMAPtr  &&  (d_Flags & OBJ_FLAG_OWNS_DMA_DATA))
		PS2RendererFree( p_DMAPtr);
}

// called by the patch function
void DDMAObject::standardFixup( DMAModelHdr * pHdr)
{
	// just deal with the standard stuff that VSTtoDblBuffer writes out... nothing else
	RENDER_ASSERT( pHdr);

	// this is already patched...
	if( pHdr->mFlags & OBJ_FLAG_TEXTURE_PATCHED )
		return;

	// is this model using the new dynamic texture format
	// we need to lookup all the ids and find the appropriate binds
	ASSERT(pHdr->mFlags & OBJ_FLAG_DYNAMIC_TEXTURE);

	// get table data pointer...
	void* pTableData = pHdr->getDTTableData();
	
	unsigned int 	numEntries = *((unsigned int *) pTableData)++;
	DTInfo *			pTable = (DTInfo *) pTableData;
	
	for( unsigned int i = 0; i < numEntries; i++)
	{
		u_int BindNumber;
		u_int LayerIndex;
		bool NeedsAlphaReg = false;
		
		ASSERT( p_TextureBindCallback);
		
		if((pTable[i].uiTexID & 0x00FFFFFF) == 0x00FFFFFF)
		{
			pTable[i].uiTexID = (*p_TextureBindCallback)(0xFFFFFFFF, 0);
		}
		else
		{
			BindNumber = (pTable[i].uiTexID & 0x00FF0000) << 8;
			if(BindNumber == 0x80000000)
			{
				BindNumber = 0;
				NeedsAlphaReg = true;
			}
			else if(BindNumber == 0x40000000)
			{
				BindNumber = 0xFF000000;
			}
			BindNumber |= pTable[i].uiTexID & 0x0000FFFF;
			LayerIndex = pTable[i].uiTexID >> 24;
			
			pTable[i].uiTexID = (*p_TextureBindCallback)(BindNumber, LayerIndex);
		}
		
		if(NeedsAlphaReg)
			pTable[i].uiTexID |= 0x80000000;
	}

	// make future calls to this do nothing
	pHdr->mFlags |= OBJ_FLAG_TEXTURE_PATCHED;
}
