#ifndef __VU_DATA_HEADERS_H
#define __VU_DATA_HEADERS_H

#include "DDMAObjectClassIDs.h"
#include "PRIMReg.h"

typedef struct _DTInfo
{
	unsigned int 	uiTexID;
	unsigned int 	uiBlockOffset;
} DTInfo;

class DMAModelHdr
{
public:
	unsigned short		mFixupClassID;			// DMA
	unsigned short		mDataClassID;			// ClassID for DDMAObject creation

	unsigned int		mVertCount;
	unsigned int		mFlags;
	unsigned int		mOffsetToFirstTexture;
	unsigned int		mOffsetToDMAData;

	unsigned int		mDMADataSize;

	PRIMReg				mPRIM;


	inline void *			getDMAData( void ) const
	{
		return (void *)	((unsigned int) this + mOffsetToDMAData);
	}
	
	inline void *			getDTTableData( void ) const
	{
		return (mFlags & OBJ_FLAG_DYNAMIC_TEXTURE) ? (void *) ((unsigned int) this +  mOffsetToFirstTexture) : 0;
	}

} ;

class AnimDMAModelHdr : public DMAModelHdr
{
public:
	unsigned int		mOffsetToInstructionFixup;
	unsigned int		mDMAChainSize;

	unsigned int		mMatrixDMASize;

	unsigned int		mMatrixCount;
	unsigned int		mMatrixDMACount;

	inline void *			getObjectData( void ) const
	{
		unsigned int * pObjData = (unsigned int *) &this[1];
		
		if( mFlags & OBJ_FLAG_DYNAMIC_TEXTURE)
		{
			unsigned int numDTEntries = *pObjData++;
			pObjData = (unsigned int *) ( (DTInfo *) pObjData + numDTEntries );
		}

		return pObjData;
	}

	inline void *			getFirstInstructionFixup( void ) const
	{
		return (void *) ((unsigned int) this + mOffsetToInstructionFixup);
	}
} ;

struct PatchHdr
{
	unsigned int		mPatchDataSize;
	unsigned int		mPatchFlags;
	unsigned int		mOffsetToPatchData;
	unsigned int		mUnused;
} ;

class PatchDMAModelHdr : public DMAModelHdr
{
public:
	unsigned int		mPatchSetCount;
	unsigned int		mOffsetToDMAChain;
	unsigned int		mDMAChainSize;

	inline void *			getObjectData( void ) const
	{
		void * pObjData = (void *) &this[1];
		
		if( mFlags & OBJ_FLAG_DYNAMIC_TEXTURE)
		{
			unsigned int numDTEntries = *((unsigned int *) pObjData);
			pObjData = ( (unsigned int *) pObjData + 1);
			pObjData = (void *) ( (DTInfo *) pObjData + numDTEntries );
		}

		return pObjData;
	}

	inline PatchHdr *	 	getPatchHdr( unsigned int patchNumber ) const
	{
		return ((PatchHdr *) getObjectData( ) + patchNumber);
	}
	
	inline void *			getFixupData( void ) const
	{
		return (void *) getPatchHdr( mPatchSetCount);
	}

	inline void *			getDMAChain( void ) const
	{
		return (void *) ((unsigned int) this + mOffsetToDMAChain);
	}

	inline void * 			getPatchData( unsigned int patchNumber) const
	{
		return (void *) ((unsigned int) this + getPatchHdr( patchNumber)->mOffsetToPatchData);
	}
} ;


#endif
			