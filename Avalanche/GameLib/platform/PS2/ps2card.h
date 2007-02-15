/*************************************************************************/
// PS2 Memory card framework.
/*************************************************************************/
#ifndef PS2CARD_H
#define PS2CARD_H

#include "platform/memcard.h"
#include "platform/BaseType.h"
#include "platform/CompilerControl.h"
#include <libscf.h>
#include <libcdvd.h>
#include <libmc.h>

#define MEMCARD_MAX_PORTS 2
#define MEMCARD_BLOCK_SIZE		( 1 * 1024)
#define MEMCARD_CLUSTER_SIZE	( 1 * 1024)

#define MEMCARD_PS2_DIRNAME_SIZE 21+1	//+1 for the forward slash
#define MEMCARD_PS2_FILENAME_SIZE 31

#define MEMCARD_PS2_MAX_FILES_DIR	20

class PS2MemCard : public MemCard
{
public:
	PS2MemCard( void);
	
	// Startup
	MEMCARD_RESULT Initialize( const char *pIconFileName, const char *pDataFileName, u32 versionID);

	MEMCARD_RESULT DefaultPort( u32 Port);

	MEMCARD_RESULT Detect( u32 Port);

	MEMCARD_RESULT CheckSpace( u32 Port, size_t SaveDataSize, u32 &BlocksNeeded);
	MEMCARD_RESULT CheckSpace( size_t SaveDataSize, u32 &BlocksNeeded);

	u8		CardID( u32 Port )
	{
		return m_cardID[Port];
	}

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

	MEMCARD_RESULT UpdateFileComment( const char *pComment) { };
	MEMCARD_RESULT UpdateFileComment( u32 Port, const char *pComment) {};

	MEMCARD_RESULT GetDirectory( u32 Port, char* Filename );
	int	SetNumFiles();
	int	GetNumFilesFound() { return (m_NumFiles); };
	MEMCARD_RESULT Exists(u32 Port);

	int	PS2MemCard::Sync();

	virtual MEMCARD_RESULT GetCardFreeSpace( u32 &freeFiles, u32 &freeBlocks) {};
	virtual MEMCARD_RESULT GetCardFreeSpace( u32 Port, u32 &freeFiles, u32 &freeBlocks){};
	virtual MEMCARD_RESULT GetCardSize( u32 &cardBlocks){};
	virtual MEMCARD_RESULT GetCardSize( u32 Port, u32 &cardBlocks){};

	virtual sceMcTblGetDir*		GetDirTable() { return (&m_Directory[0]); };

	char * GetPortText( u32 Port);

	char * GetCardText( void);

	char * GetOtherCardText( void);
	
	//get the actual system clock of the platform
	ts_SystemClock GetClockTime( void );

	// for debugging
	char * ResultAsText( MEMCARD_RESULT Result );

	// for debugging
	MEMCARD_RESULT Result( void )
	{
		return m_aResult[m_DefaultPort];
	}

	// --- additional file support
	MEMCARD_RESULT SetFileName( const char *pDataFileName );
	
	//creates a aligned and padded Load/Save file buffer
	char *CreateLoadSaveBuffer( size_t size );
	MEMCARD_RESULT DestroyLoadSaveBuffer( char *pLoadSaveBuffer );

	s32 ReturnIconFileSize( void );

	inline u32 GetBlockSize(  ) { return( MEMCARD_BLOCK_SIZE ); }
	inline u32 GetBlocksForFileSize( u32 size )
	{
		return(	BytesToBlocks( size ) +										//for game data
					BytesToBlocks( ReturnIconFileSize( ) ) +				//for icon file
					1 +																//for icon.sys
					2 +																//for root directory
					2 + 																//for number of files 
					1
				);
	}

	inline u32 GetBlocksForActualFileSize( u32 fileNo, u32 size )
	{
		return(	BytesToBlocks( size ) +										//for game data
					1
				);
	}
	
protected:

	struct ts_ps2MemCardHdr
	{
		u32		checkSum;
		u32		versionID;
		u8			pad[ 8 ];				// pad out to 16 bytes
	} ALIGNED( 16 );

	sceMcTblGetDir		m_Directory[MEMCARD_PS2_MAX_FILES_DIR] __attribute__ ((aligned(64)));
	u32					m_NumFiles;

	u32					m_DefaultPort;
   
	s32					m_IconFileSize;

	char					*m_pIconSysName;

	char					*m_pIconFileName;

	MEMCARD_RESULT		m_aResult[MEMCARD_MAX_PORTS];
	u8						m_cardID[MEMCARD_MAX_PORTS];
	u8						m_lastValidCardID[MEMCARD_MAX_PORTS];

	char					m_DirName[MEMCARD_PS2_DIRNAME_SIZE + 1],
							m_DataFileName[MEMCARD_PS2_FILENAME_SIZE + 1],
                     m_IconFileName[MEMCARD_PS2_FILENAME_SIZE + 1],
                     m_IconSysName[MEMCARD_PS2_FILENAME_SIZE + 1];


	MEMCARD_RESULT		Create( u32 Port, size_t SaveDataSize);
   
	MEMCARD_RESULT		ReturnResult( u32 Port, MEMCARD_RESULT Result);

	MEMCARD_RESULT		FileCreate( u32 Port, char *pFileName, int *fd);

	MEMCARD_RESULT		FileWrite( u32 Port, int fd, void *pData, int nBytes);

	MEMCARD_RESULT		FileClose( u32 Port, int fd);

	MEMCARD_RESULT		WriteFile( u32 Port, char *pFileName, void *pData, int nBytes);

	char *BasePtr( char *pData )
	{
		return(  pData - sizeof( ts_ps2MemCardHdr ) );
	}
	void	ToggleNoCard(){};
	void	ToggleNoSpace(){};

};

#endif
