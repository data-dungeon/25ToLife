/* --------------------------------------------------
 * File    : MOT_LOAD.C
 * Created : Thu Mar  4 15:44:11 1999
 * Descript:
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/

/******************* includes ****************************/

#include "Game/GamePCH.h"


/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/

/* data load fix-up scheme is this: on load, pointer field contains offset from pointer field to data. if offset */
/* is 0, leave pointer as NULL */

#define FIXUP_PTR(p)		if (*(int*)&(p) != 0) *(int*)&(p) = (((int)(p) + (int)&(p)) + 1)

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/

u32 _CountOneBits( unsigned short usBitArray );

/* --------------------------------------------------
* Exported Functions
* --------------------------------------------------*/

bool LoadMOT( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize)
{
#ifdef GCN
	BOOL	enabled;

	enabled = OSDisableInterrupts( );
	OSDisableScheduler( );
	OSRestoreInterrupts( enabled);

	t_Handle		hSave = memSetActiveHandle( Layers__GetVirtualHeap( ));

	void		*pVirtualData = memAlloc( pChunkHdr->u32Size);

	memSetActiveHandle( hSave);

	enabled = OSDisableInterrupts( );
	OSEnableScheduler( );
	OSRestoreInterrupts( enabled);

	if( pVirtualData != NULL)
	{
		memcpy( pVirtualData, pData, pChunkHdr->u32Size);

		pData = pVirtualData;
	}
#endif

	/* start at motion file header */

	ts_MotionFileHeader *pMotionFileHeader = (ts_MotionFileHeader *) pData;
#ifdef GCN
	pMotionFileHeader->EndianSwap();	// motion data is platform-independent, so may have to endian swap
#endif //GCN
	FIXUP_PTR(pMotionFileHeader->d_ppMotionHeaderTable);
	FIXUP_PTR(pMotionFileHeader->d_pBoneEntries);

	/* loop through bone entries */

	int i;
#ifdef GCN
	for (i = 0; i < pMotionFileHeader->d_u16NumBoneEntries; i++)
	{
		ts_BoneEntry *pBoneEntry = &pMotionFileHeader->d_pBoneEntries[i];
		pBoneEntry->EndianSwap();	// motion data is platform-independent, so may have to endian swap
	}
#endif //GCN

	/* loop through motions */
	
	for (i = 0; i < pMotionFileHeader->d_u16NumMotions; i++)
	{
		/* d_ppMotionHeaderTable is an array of pointers, each has to endian swapped if necesary before fix-up */

#ifdef GCN
		ByteSwap(pMotionFileHeader->d_ppMotionHeaderTable[i]);
#endif //GCN
		FIXUP_PTR(pMotionFileHeader->d_ppMotionHeaderTable[i]);

		/* now we can get to the motion */

		ts_MotionHeader *pMotionHeader = pMotionFileHeader->d_ppMotionHeaderTable[i];
#ifdef GCN
		pMotionHeader->EndianSwap();
#endif //GCN
		FIXUP_PTR(pMotionHeader->d_pBoneChannel);

		/* go through channels */

		for (int j = 0; j < pMotionHeader->d_u16NumBoneChannels; j++)
		{
			ts_BoneChannel *pBoneChannel = &pMotionHeader->d_pBoneChannel[j];
#ifdef GCN
			pBoneChannel->EndianSwap();
#endif //GCN
			FIXUP_PTR(pBoneChannel->d_pBoneEntry);
			FIXUP_PTR(pBoneChannel->d_pBoneChannelItem);

			/* each channel contains some subset of the available channel components (x/y/z translation, */
			/* x/y/z scale, x/y/z/w rotation) */

			int nNumChannelItems = _CountOneBits(pBoneChannel->d_u16ChannelItemBitArray);
			for (int k = 0; k < nNumChannelItems; k++)
			{
				ts_BoneChannelItem *pBoneChannelItem = &pBoneChannel->d_pBoneChannelItem[k];
#ifdef GCN
				pBoneChannelItem->EndianSwap();
#endif //GCN
				FIXUP_PTR(pBoneChannelItem->d_pKeyFrames);

				/* go through key frames */

				for (int l = 0; l < pBoneChannelItem->d_u16NumKeyFrames; l++)
				{
					ts_MotionKeyFrame *pKeyFrame = &pBoneChannelItem->d_pKeyFrames[l];
#ifdef GCN
					pKeyFrame->EndianSwap();
#endif //GCN
				}
			}
		}
	}

#ifdef GCN
	if( pVirtualData != NULL)
	{
		pChunkHdr->u16Flags |= DBL_FLUSHABLE;
	}
	else
	{
		pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
	}
#else
	pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
#endif
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	return TRUE;
}

/* load the motion chunk for an actor if not already loaded */

t_Handle motLoadMotionChunk(const char *pFilename, CActorInstance *inst /*= NULL*/, const int nIndex /*= 0*/)
{
	/* load file-- if already loaded, will just return handle */

	t_Handle Handle = dblLoad(pFilename);
	if (Handle == INVALID_HANDLE)
	{
		ASSERTF(false, ("Couldn't load motion file %s", pFilename));
		return(INVALID_HANDLE);
	}

	/* let load-by-handle finish it up */

	return(motLoadMotionChunk(Handle, inst, nIndex));
}
t_Handle motLoadMotionChunk(t_Handle Handle, CActorInstance *inst /*= NULL*/, const int nIndex /*= 0*/)
{
	/* get chunk data */

	void *pData = dblGetChunkData(DB_MOTION, DEFAULT_CHUNK_KEY_STRING, Handle);
	if ((Handle == INVALID_HANDLE) || (pData == NULL))
	{
		ASSERTF(false, ("Couldn't load motion file 0x%08x", Handle));
		return(INVALID_HANDLE);
	}

	/* get the name table */
	MotionNameHash* nameTable = NULL;
	nameTable = (MotionNameHash*) dblGetChunkData( DB_MOTION_NAMES, DEFAULT_CHUNK_KEY_STRING, Handle );
	if ( Handle == INVALID_HANDLE )
		nameTable = NULL;

	/* found the chunk */

	if (inst)
	{
		inst->SetMotionFile( nIndex, (ts_MotionFileHeader*) pData, nameTable );
	}

	return(Handle);
}

/* get header for a particular motion within a motion set file */

ts_MotionHeader *GetMotionHeader(ts_MotionFileHeader *pMotionFileHeader, u32 motnDefine)
{
	ASSERT_PTR( pMotionFileHeader );
	ASSERT(pMotionFileHeader->d_u16NumMotions > (motnDefine & 0x0000FFFF));
	return(pMotionFileHeader->d_ppMotionHeaderTable[motnDefine & 0x0000FFFF]);
}

//

// Motion Name Structs
struct MotionNameHeader
{
	unsigned short m_numMotions;
	char*				m_names[ 1 ];		// actually variable, based on m_numMotions
};


/* --------------------------------------------------
 * Load the motion names and build a lookup hash for them
 * --------------------------------------------------*/

bool LoadMOTNames( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize)
{
	MotionNameHeader* nameHeader = (MotionNameHeader*) pData;
	ASSERT( nameHeader );

	// step 1 - fixup the file chunk so we can use it ... this chunk contains each
	//          of the motion names.

	#ifdef GCN
	ByteSwap( nameHeader->m_numMotions );
	#endif //GCN

	int i;
	for ( i = 0 ; i < nameHeader->m_numMotions ; i++ )
	{
		#ifdef GCN
		ByteSwap( nameHeader->m_names[ i ] );
		#endif //GCN

		FIXUP_PTR( nameHeader->m_names[ i ] );
	}

	// step 2 - build a hash table

	MotionNameHash* hashTable = new MotionNameHash( ( nameHeader->m_numMotions + 4 ) * 13 / 10 );
	for ( i = 0 ; i < nameHeader->m_numMotions ; i++ )
		hashTable->Add( nameHeader->m_names[ i ], i );

	// tell dbl manager to free the loaded chunk and consider the
	// hash table to be the real data.
	pChunkHdr->u16Flags |= DBL_FLUSHABLE;
	*ppRetData    = hashTable;
	*pu32DataSize = hashTable->Size();

	return true;
}

/* --------------------------------------------------
 * Local Functions
  * --------------------------------------------------*/

u32 _CountOneBits( unsigned short usBitArray )
{
	u32 i, u32OneBits=0;

	for( i=0; i < sizeof(usBitArray)*8; i++)
	{
		if( usBitArray & 1 )
			u32OneBits++;
		usBitArray >>= 1;
	}

	return u32OneBits;
	
}
