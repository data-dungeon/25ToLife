/*************************************************************************/
// WIN Memory card framework.
/*************************************************************************/
#ifndef WINCARD_H
#define WINCARD_H

#include "platform/memcard.h"
#include "platform/BaseType.h"

struct WinCard
{
	MEMCARD_RESULT	m_CardStatus;			// Current status
	int				m_FreeSpace;			// Free space in blocks
};

#define MAX_FILENAME_SIZE 128
#define MEMCARD_BLOCK_SIZE		( 1 * 1024)
#define MEMCARD_CLUSTER_SIZE	( 1 * 1024)

class WinMemCard : public MemCard
{
public:
	WinMemCard( void);
	~WinMemCard( void );

	MEMCARD_RESULT Initialize( const char *pIconFileName, const char *pDataFileName, u32 versionID);
	MEMCARD_RESULT DefaultPort( u32 Port);
	MEMCARD_RESULT Detect( u32 Port);
	MEMCARD_RESULT CheckSpace( u32 Port, size_t SaveDataSize, u32 &BlocksNeeded);
	MEMCARD_RESULT CheckSpace( size_t SaveDataSize, u32 &BlocksNeeded);
	u32	BytesToBlocks( u32 Bytes)
	{
		return( ( Bytes + (MEMCARD_BLOCK_SIZE - 1)) / MEMCARD_BLOCK_SIZE);
	}

	MEMCARD_RESULT CheckStatus( u32 Port );
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

	MEMCARD_RESULT Exists(u32 Port);

	MEMCARD_RESULT UpdateFileComment( const char *pComment);
	MEMCARD_RESULT UpdateFileComment( u32 Port, const char *pComment);

	char * GetPortText( u32 Port);
	char * GetCardText( void);
	char * GetOtherCardText( void);

	//get the actual system clock of the platform
	ts_SystemClock GetClockTime( void );

	void RemoveCard( u32 Port );
	void InsertCard( u32 Port, MEMCARD_RESULT Result = MEMCARD_RESULT_OK );
	char *ResultAsText( MEMCARD_RESULT Result );
	
	MEMCARD_RESULT SetFileName( const char *pDataFileName );

	char *CreateLoadSaveBuffer( size_t size );
	MEMCARD_RESULT DestroyLoadSaveBuffer( char *pSaveBuffer );

	s32 ReturnIconFileSize( void ) { return ( 0 ); }

	inline u32 GetBlockSize( ){ return( MEMCARD_BLOCK_SIZE ); }
	inline u32 GetBlocksForFileSize( u32 size )
	{
		return( BytesToBlocks( size ) );
	}
	inline u32 GetBlocksForActualFileSize( u32 fileNo, u32 size )
	{
		return (BytesToBlocks( size ) );
	}

	void	ToggleNoCard();
	void	ToggleNoSpace();

protected:

	struct ts_winMemCardHdr
	{
		u32		versionID;
	};

	char *BasePtr( char *pData )
	{
		return(  pData - sizeof( ts_winMemCardHdr ) );
	}
	
	char			m_Win32Dir[256];

	char			m_cDataFileName[ MAX_FILENAME_SIZE + 1 ];
	char			*m_pDataFileName;
	static u32	m_DefaultPort;
	WinCard		m_Port[2];
	int	m_FakeNoCard;
	int	m_FakeNoSpace;

};

#endif
