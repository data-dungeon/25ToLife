/*************************************************************************/
// GCN Memory card framework.
/*************************************************************************/
#ifndef GCNCARD_H
#define GCNCARD_H

#include "platform/memcard.h"
#include "platform/BaseType.h"
#include <dolphin/card.h>

#define MEMCARD_MAX_PORTS					2
#define MEMCARD_BLOCK_SIZE					( 8 * 1024)

#define MEMCARD_IOBUFFER_SIZE_BLOCKS	8
#define MEMCARD_IOBUFFER_SIZE_BYTES		( MEMCARD_IOBUFFER_SIZE_BLOCKS * MEMCARD_BLOCK_SIZE)

class GCNMemCard : public MemCard
{
public:
	GCNMemCard( void);
	
	// Startup
	MEMCARD_RESULT Initialize( const char *pIconFileName, const char *pDataFileName, u32 versionID);

	MEMCARD_RESULT DefaultPort( u32 Port);

	MEMCARD_RESULT Detect( u32 Port);

	MEMCARD_RESULT CheckSpace( u32 Port, size_t SaveDataSize, u32 &BlocksNeeded);
	MEMCARD_RESULT CheckSpace( size_t SaveDataSize, u32 &BlocksNeeded);

	u32	BytesToBlocks( u32 Bytes)
	{
		return( ( Bytes + (MEMCARD_BLOCK_SIZE - 1)) / MEMCARD_BLOCK_SIZE);
	}

	MEMCARD_RESULT CheckStatus( u32 Port);
	MEMCARD_RESULT CheckStatus( void);

	MEMCARD_RESULT Format( u32 Port);
	MEMCARD_RESULT Format( void);

	MEMCARD_RESULT Load( u32 Port, void *pSaveData, size_t SaveDataSize, int HeaderReadOnly);
	MEMCARD_RESULT Load( void *pSaveData, size_t SaveDataSize, int HeaderReadOnly);

	MEMCARD_RESULT Save( u32 Port, void *pSaveData, size_t SaveDataSize);
	MEMCARD_RESULT Save( void *pSaveData, size_t SaveDataSize);

	MEMCARD_RESULT Delete( u32 Port);
	MEMCARD_RESULT Delete( void);
	MEMCARD_RESULT DeleteAll( u32 Port, char *TitleId);

	char * GetPortText( u32 Port);

	char * GetCardText( void);

	char * GetOtherCardText( void);

	//get the actual system clock of the platform
	ts_SystemClock GetClockTime( void );

	MEMCARD_RESULT SetFileName( const char *pDataFileName );

	//creates a aligned and padded Load/Save file buffer
	char *CreateLoadSaveBuffer( size_t size );
	MEMCARD_RESULT DestroyLoadSaveBuffer( char *pLoadSaveBuffer );
	
	s32 ReturnIconFileSize( void ) { return ( 0 ); }

	MEMCARD_RESULT UpdateFileComment( const char *pComment);
	MEMCARD_RESULT UpdateFileComment( u32 Port, const char *pComment);

	// must call Detect first
	virtual MEMCARD_RESULT GetCardFreeSpace( u32 &freeFiles, u32 &freeBlocks)
	{
		return( GetCardFreeSpace( m_DefaultPort, freeFiles, freeBlocks));
	}
	virtual MEMCARD_RESULT GetCardFreeSpace( u32 Port, u32 &freeFiles, u32 &freeBlocks);
	
	// must call Detect first
	virtual MEMCARD_RESULT GetCardSize( u32 &cardBlocks)
	{
		return( GetCardSize( m_DefaultPort, cardBlocks));
	}

	virtual MEMCARD_RESULT GetCardSize( u32 Port, u32 &cardBlocks);
	
	inline u32 GetBlockSize( void)
	{
		return( MEMCARD_BLOCK_SIZE );
	}

	inline u32 GetBlocksForFileSize( u32 size)
	{
		return( GetBufferSize( size) + 1);
	}

	inline u32 GetBlocksForActualFileSize( u32 fileNo, u32 size )
	{
		return (BytesToBlocks( size ) );
	}

protected:

	char			m_cDataFileName[CARD_FILENAME_MAX + 1];
	char			m_cIconFileName[CARD_FILENAME_MAX + 1];
	
	struct ts_gcnMemCardHdr
	{
		OSTime	saveTime;
		u32		checkSum;
		u32		versionID;
		u32		checkSumDataSize;
		u32		pad[3];
	};

	char					*m_apWorkArea[MEMCARD_MAX_PORTS];

	u32					m_DefaultPort;

	char					*m_pIconFileName,
							*m_pDataFileName;

	u8						*m_pIOBuffer;

	MEMCARD_RESULT		m_aResult[MEMCARD_MAX_PORTS];

	CARDFileInfo		m_CardFileInfo;

	CARDStat				m_CardStat;

	char					m_acCardComment[32];
	bool					m_bCardCommentSet;

	MEMCARD_RESULT		Create( u32 Port, size_t SaveDataSize);

	MEMCARD_RESULT		MapGCNResult( s32 Result);

	MEMCARD_RESULT		ReturnResult( u32 Port, MEMCARD_RESULT Result)
	{
		m_aResult[Port] = Result;
		return( Result);
	};

	char *BasePtr( char *pData)
	{
		return( pData - sizeof(ts_gcnMemCardHdr));
	}

	u32 GetBufferSize( u32 size)
	{
		return( BytesToBlocks( ( ( size + 7) & ~7) + ( 2 * sizeof(ts_gcnMemCardHdr))));
	}

};

#endif
