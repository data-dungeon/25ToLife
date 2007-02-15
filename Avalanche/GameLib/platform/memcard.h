/*************************************************************************/
// Memory card framework.
/*************************************************************************/
#ifndef MEMCARD_H
#define MEMCARD_H

#include "platform/BaseType.h"

#if defined (PS2)
#include <libmc.h>
#endif

// for Xbox mainly
#define HEADER_READ_ONLY		0
#define READ_ALL					1

#define WIN32_PLATFORM_ENV		"TTLPATH"
typedef enum
{
	MEMCARD_RESULT_OK,					//operation completed successfully
	MEMCARD_RESULT_NOCARD,				//no card is inserted
	MEMCARD_RESULT_NODATA,				//no save data is stored on the card
	MEMCARD_RESULT_UNFORMATTED,		//memory card not formatted
	MEMCARD_RESULT_NOSPACE,				//not enough free space
	MEMCARD_RESULT_NOFILES,				//no remaining files
	MEMCARD_RESULT_BADCARD,				//memory card damaged beyond repair
	MEMCARD_RESULT_BADDATA,				//data in file is corrupted
	MEMCARD_RESULT_WRONGTYPE,			//memory card type not supported			
	MEMCARD_RESULT_OTHERMARKET,		//memory card is formatted for a different market
	MEMCARD_RESULT_OTHERDEVICE,		//device other than memcard in slot
	MEMCARD_RESULT_BADPARAM,			//bad parameter to memcard function
	MEMCARD_RESULT_NOTINIT,				//memcard subsystem not initialized
	MEMCARD_RESULT_NEEDDETECT,			//must call detect on slot
	MEMCARD_RESULT_BADSAVE,				//card was yanked during a save operation
	MEMCARD_RESULT_FAILED,				//operation failed..
	MEMCARD_RESULT_NETWORK_CONFIGURATION_FAILED,	// could not read network configuration
	MEMCARD_RESULT_NONE,					//nothing flag
	MEMCARD_RESULT_CANCEL_OPERATION,

} MEMCARD_RESULT;

typedef enum {
	MEMCARD_DATE_YYYYMMDD,
	MEMCARD_DATE_MMDDYYYY,
	MEMCARD_DATE_DDMMYYYY,
} MEMCARD_DATESTRUCTURE;

typedef	void	(CMCard_VoidFunc)(void);

typedef struct
{
	int		              year;
	int		              month;
	int		              day;
	int		              hour;
	int		              minute;
	int		              second;
	bool                  hourclock12;
	MEMCARD_DATESTRUCTURE dateformat;
} ts_SystemClock;

class MemCard
{
public:
	MemCard( void);

	// Startup
	virtual MEMCARD_RESULT Initialize( const char *pIconFileName, const char *pDataFileName, u32 versionID) = 0;

	//called first to detect if a valid memcard is inserted in port
	//must be first function called on memcard during any function
	virtual MEMCARD_RESULT Detect( u32 Port) = 0;

	virtual MEMCARD_RESULT DefaultPort( u32 Port) = 0;

	//checks whether their is enough space on the card
	virtual MEMCARD_RESULT CheckSpace( u32 Port, size_t SaveDataSize, u32 &BlocksNeeded) = 0;
	virtual MEMCARD_RESULT CheckSpace( size_t SaveDataSize, u32 &BlocksNeeded) = 0;
	virtual u32 BytesToBlocks( u32 Bytes ) = 0;

	//checks to see if card had been pulled or is still inserted
	virtual MEMCARD_RESULT CheckStatus( u32 Port) = 0;
	virtual MEMCARD_RESULT CheckStatus( void) = 0;

	//formats the memcard
	virtual MEMCARD_RESULT Format( u32 Port) = 0;
	virtual MEMCARD_RESULT Format( void) = 0;

	//load data from memcard
	virtual MEMCARD_RESULT Load( u32 Port, void *pSaveData, size_t SaveDataSize, int HeaderReadOnly) = 0;
	virtual MEMCARD_RESULT Load( void *pSaveData, size_t SaveDataSize, int HeaderReadOnly) = 0;

	//save data to memcard
	virtual MEMCARD_RESULT Save( u32 Port, void *pSaveData, size_t SaveDataSize) = 0;
	virtual MEMCARD_RESULT Save( void *pSaveData, size_t SaveDataSize) = 0;

	//delete data file from memcard
	virtual MEMCARD_RESULT Delete( u32 Port) = 0;
	virtual MEMCARD_RESULT Delete( void) = 0;
	virtual MEMCARD_RESULT DeleteAll (u32 Port, char* TitleID) = 0;

	virtual MEMCARD_RESULT Exists(u32 Port) = 0;

	#if defined (_XBOX)
	virtual int	FileSize(HANDLE Handle) = 0;
	virtual MEMCARD_RESULT DeleteContainer( u32 Port) = 0;
	#endif

	#if defined (PS2)
	virtual MEMCARD_RESULT GetDirectory( u32 Port, char* Filename ) = 0;
	virtual int	GetNumFilesFound() = 0;
	virtual sceMcTblGetDir*		GetDirTable() = 0;
	#endif

	// updater function for async
	void	RegisterUpdate(void* Function) { m_Update = (CMCard_VoidFunc*)Function; };

	static char * GetPortText( u32 Port );
	static char * GetCardText( void );
	static char * GetOtherCardText( void);

	// read header only? - turns off sig checks
	void	SetHeaderOnly(bool Flag) { m_ReadHeader = Flag; };

	//get the actual system clock of the platform
	virtual ts_SystemClock GetClockTime( void ) = 0;

	//sets the name of the save file
	virtual MEMCARD_RESULT SetFileName( const char *pDataFileName ) = 0;
	
	//creates a save file buffer
	virtual char *CreateLoadSaveBuffer( size_t size ) = 0;
	virtual MEMCARD_RESULT DestroyLoadSaveBuffer( char *pLoadSaveBuffer ) = 0;

	virtual s32 ReturnIconFileSize( void ) = 0;

	virtual MEMCARD_RESULT UpdateFileComment( const char *pComment)
	{
		return( MEMCARD_RESULT_OK);
	}
	virtual MEMCARD_RESULT UpdateFileComment( u32 Port, const char *pComment)
	{
		return( MEMCARD_RESULT_OK);
	}

	// must call Detect first
	virtual MEMCARD_RESULT GetCardFreeSpace( u32 &freeFiles, u32 &freeBlocks)
	{
		freeFiles = 0;
		freeBlocks = 0;
		return( MEMCARD_RESULT_OK);
	}

	virtual MEMCARD_RESULT GetCardFreeSpace( u32 Port, u32 &freeFiles, u32 &freeBlocks)
	{
		freeFiles = 0;
		freeBlocks = 0;
		return( MEMCARD_RESULT_OK);
	}

	// must call Detect first
	virtual MEMCARD_RESULT GetCardSize( u32 &cardBlocks)
	{
		cardBlocks = 0;
		return( MEMCARD_RESULT_OK);
	}
	
	virtual MEMCARD_RESULT GetCardSize( u32 Port, u32 &cardBlocks)
	{
		cardBlocks = 0;
		return( MEMCARD_RESULT_OK);
	}
	
	virtual u32 GetBlockSize(  ) = 0;
	virtual u32 GetBlocksForFileSize(  u32 size ) = 0;
	virtual u32 GetBlocksForActualFileSize( u32 fileNo, u32 size ) = 0;

	virtual void	ToggleNoCard() = 0;
	virtual void	ToggleNoSpace() = 0;


protected:
	bool					m_bInitialized;
	CMCard_VoidFunc*	m_Update;
	u32					m_VersionID;
	bool					m_ReadHeader;
};

// Our singleton
extern MemCard *g_MemCardPtr;
inline MemCard &SafePtr(MemCard *ptr)
{
	ASSERT(ptr);
	return *ptr;
}
#define g_MemCardMgr	SafePtr(g_MemCardPtr)

#endif
