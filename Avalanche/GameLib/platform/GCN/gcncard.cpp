/*************************************************************************/
// GCN Memory card framework.
/*************************************************************************/

/******************* includes ****************************/

#include "platform/PlatformPCH.h"



#include "EngineHelper/filestream.h"

#define GCN_MAX_ICONS		8

#define GCN_ICONBLOCK_ID	0xAbbeAbbe

#define GCN_DATA_CHECKSUM	0xabadfade

struct ts_GCNIconHeader
{
	unsigned char		bannerFormat,
							iconFormat,
							animType,
							iconSpeed[GCN_MAX_ICONS],
							filler[32 - 11];
};

GCNMemCard::GCNMemCard( void)	: MemCard( )
{
	m_pIconFileName = m_cIconFileName;
	m_pDataFileName = m_cDataFileName;

	memSetOneShotNewAlign( 32);
	m_apWorkArea[0] = new char [CARD_WORKAREA_SIZE * 2];
	m_apWorkArea[1] = m_apWorkArea[0] + CARD_WORKAREA_SIZE;
	
	memSetOneShotNewAlign( 32);
	m_pIOBuffer = new u8[MEMCARD_IOBUFFER_SIZE_BYTES];

	memset( m_acCardComment, ' ', sizeof m_acCardComment);
	m_bCardCommentSet = false;

	ASSERT( ( sizeof(ts_gcnMemCardHdr) & 0x1f) == 0);
}

char * GCNMemCard::GetPortText( u32 Port)
{
	if( Port == 0)
	{
		return( "Memory Card Slot A");
	}
	else
	if( Port == 1)
	{
		return( "Memory Card Slot B");
	}
	return( "Memory Card Slot ?");
}

char * GCNMemCard::GetCardText( void)
{
	return( "Nintendo GameCube Memory Card");
}

char * GCNMemCard::GetOtherCardText( void)
{
	return( "Memory Card");
}

//  Get the actual system clock from the PS2
ts_SystemClock GCNMemCard::GetClockTime( void)
{

	ts_SystemClock		clockInformation;
	
	OSCalendarTime		GCNClock;
	
	OSTicksToCalendarTime(OSGetTime(), &GCNClock);

	//  handle the year
	clockInformation.year = GCNClock.year;

	// handle the month
	// gcn month is zero-based, but we're expecting january to be 1, not 0
	clockInformation.month = GCNClock.mon + 1; 
	
	// handle the day
	clockInformation.day = GCNClock.mday;

	// handle the hour  TGS - it looks like the game cube likes miliary time, so keep it
	clockInformation.hourclock12 = false;
	clockInformation.hour = GCNClock.hour;

	// handle the minutes
	clockInformation.minute = GCNClock.min;

	// handle the seconds
	clockInformation.second = GCNClock.sec;

	// handle the format of the date  TGS - GCN seems to like this format
	clockInformation.dateformat = MEMCARD_DATE_MMDDYYYY;

	return ( clockInformation);
}

MEMCARD_RESULT GCNMemCard::Initialize( const char *pIconFileName, const char *pDataFileName, u32 versionID)
{
	if( strlen( pDataFileName) >= CARD_FILENAME_MAX)
	{
		return( MEMCARD_RESULT_BADPARAM);
	}

//	m_pIconFileName = new char[strlen( pIconFileName) + 1];
	ASSERT( m_pIconFileName != NULL);
	strcpy( m_pIconFileName, pIconFileName);

	// ---DAS, changed to allow multiple file names to be set
//	m_pDataFileName = new char[strlen( pDataFileName) + 1];
//	m_pDataFileName = new char[ CARD_FILENAME_MAX + 1];
	ASSERT( m_pDataFileName != NULL);
	strcpy( m_pDataFileName, pDataFileName);

	for( u32 port = 0; port < MEMCARD_MAX_PORTS; port++)
	{
		m_aResult[port] = MEMCARD_RESULT_NEEDDETECT;
	}
	
	m_VersionID = versionID;

	m_bInitialized = true;

	return( MEMCARD_RESULT_OK);
}

MEMCARD_RESULT GCNMemCard::SetFileName( const char *pDataFileName)
{
	if( strlen( pDataFileName) >= CARD_FILENAME_MAX)
	{
		ASSERT( 0);
		return( MEMCARD_RESULT_BADPARAM);
	}

	strcpy( m_pDataFileName, pDataFileName);

	return( MEMCARD_RESULT_OK);
}

char *GCNMemCard::CreateLoadSaveBuffer( size_t size)
{

	int			nBlocks,
					nBytes;

	char			*pData;

	BOOL			enabled; 
	
	enabled = OSDisableInterrupts( );
	OSDisableScheduler( );
	OSRestoreInterrupts( enabled);

	t_Handle		hSave = memSetActiveHandle( Layers__GetVirtualHeap( ));
	
	nBlocks = GetBufferSize( size);
	nBytes = nBlocks * MEMCARD_BLOCK_SIZE;

	MEM_SET_ALLOC_NAME("GCNCARD LSBUFF");
	pData = ( char *)memAlloc( nBytes);
	memSetActiveHandle( hSave);

	if( pData == NULL)
	{
		pData = ( char *)memAlloc( nBytes);
	}
	
	enabled = OSDisableInterrupts( );
	OSEnableScheduler( );
	OSRestoreInterrupts( enabled);

	ASSERT( pData != NULL);
	
	memset( pData, 0, nBytes);
	return( pData + sizeof(ts_gcnMemCardHdr));
}

MEMCARD_RESULT GCNMemCard::DestroyLoadSaveBuffer( char *pLoadSaveBuffer)
{
	memFree( BasePtr( pLoadSaveBuffer));

	return( MEMCARD_RESULT_OK );
}

MEMCARD_RESULT GCNMemCard::DefaultPort( u32 Port)
{

	MEMCARD_RESULT		result;

	switch( Port)
	{
		case 0:
		case 1:
			m_DefaultPort = Port;
			result = MEMCARD_RESULT_OK;
			break;

		default:
			result = MEMCARD_RESULT_BADPARAM;
			break;
	}

	return( result);
}

MEMCARD_RESULT GCNMemCard::Detect( u32 Port)
{

	s32					gcnResult,
							memSize,
							sectorSize;

	MEMCARD_RESULT		result;

	if( !m_bInitialized)
	{
		return( MEMCARD_RESULT_NOTINIT);
	}

	do
	{
		gcnResult = CARDProbeEx( Port, &memSize, &sectorSize);
	} while( gcnResult == CARD_RESULT_BUSY);

	result = MapGCNResult( gcnResult);

	if( result != MEMCARD_RESULT_OK)
	{
		return( ReturnResult( Port, result));
	}

	if( sectorSize != MEMCARD_BLOCK_SIZE)
	{
		return( ReturnResult( Port, MEMCARD_RESULT_WRONGTYPE));
	}

	gcnResult = CARDMount( Port, m_apWorkArea[Port], NULL);

	if(	gcnResult != CARD_RESULT_READY &&
			gcnResult != CARD_RESULT_BROKEN)
	{
		return( ReturnResult( Port, MapGCNResult( gcnResult)));
	}

	result = MapGCNResult( CARDCheck( Port));

	return( ReturnResult( Port, result));
}
	
MEMCARD_RESULT GCNMemCard::CheckSpace( u32 Port, size_t SaveDataSize, u32 &BlocksNeeded)
{

	s32					gcnResult,
							freeBytes,
							freeBlocks,
							freeFiles;

	MEMCARD_RESULT		result;

	BlocksNeeded = BytesToBlocks( SaveDataSize);		//SaveDataSize has been adjusted by GetBlocksForFileSize

	gcnResult = CARDFreeBlocks( Port, &freeBytes, &freeFiles);
	if( gcnResult != CARD_RESULT_READY)
	{
		return( ReturnResult( Port, MapGCNResult( gcnResult)));
	}

	freeBlocks = BytesToBlocks( freeBytes);

	gcnResult = CARDOpen( Port, m_pDataFileName, &m_CardFileInfo);
	if( gcnResult == CARD_RESULT_READY)
	{
		CARDClose( &m_CardFileInfo);
		return( ReturnResult( Port, MEMCARD_RESULT_OK));
	}

	if( freeBlocks < BlocksNeeded)
	{
		return( ReturnResult( Port, MEMCARD_RESULT_NOSPACE));
	}

	if( freeFiles == 0)
	{
		return( ReturnResult( Port, MEMCARD_RESULT_NOFILES));
	}

	return( ReturnResult( Port, MEMCARD_RESULT_OK));
}

MEMCARD_RESULT GCNMemCard::CheckSpace( size_t SaveDataSize, u32 &BlocksNeeded)
{
	return( CheckSpace( m_DefaultPort, SaveDataSize, BlocksNeeded));
}

MEMCARD_RESULT GCNMemCard::CheckStatus( u32 Port)
{
	if( CARDProbe( Port))
	{
		return( MEMCARD_RESULT_OK);
	}
	else
	{
		return( MEMCARD_RESULT_NOCARD);
	}
}

MEMCARD_RESULT GCNMemCard::CheckStatus( void)
{
	return( CheckStatus( m_DefaultPort));
}

MEMCARD_RESULT GCNMemCard::Format( u32 Port)
{
	if( m_aResult[Port] == MEMCARD_RESULT_NEEDDETECT)
	{
		return( MEMCARD_RESULT_NEEDDETECT);
	}

	return( ReturnResult( Port, MapGCNResult( CARDFormat( Port))));
}

MEMCARD_RESULT GCNMemCard::Format( void)
{
	return( Format( m_DefaultPort));
}

MEMCARD_RESULT GCNMemCard::Load( u32 Port, void *pSaveData, size_t SaveDataSize, int HeaderReadOnly)
{

	char					*pData;

	s32					gcnResult,
							nBlocksTotal,
							nBlocks,
							nCheckSum,
							offset;

	u32					*pCheckSumData,
							checkSum;

	ts_gcnMemCardHdr	*pHdr;

	MEMCARD_RESULT		result;

	gcnResult = CARDOpen( Port, m_pDataFileName, &m_CardFileInfo);
	if( gcnResult != CARD_RESULT_READY)
	{
		return( ReturnResult( Port, MapGCNResult( gcnResult)));
	}

	SaveDataSize += sizeof(ts_gcnMemCardHdr);

	nBlocksTotal = BytesToBlocks( SaveDataSize);

	//lets verify the data in the icon blocks
	gcnResult = CARDRead( &m_CardFileInfo, m_pIOBuffer, MEMCARD_BLOCK_SIZE, 0);
	pHdr = (ts_gcnMemCardHdr *)m_pIOBuffer;
	if( pHdr->versionID != GCN_ICONBLOCK_ID)
	{
		return( MEMCARD_RESULT_BADDATA);
	}

	if( pHdr->checkSumDataSize > ( MEMCARD_BLOCK_SIZE - sizeof(ts_gcnMemCardHdr)) / sizeof(u32))
	{
		return( MEMCARD_RESULT_BADDATA);
	}

	pCheckSumData = (u32 *)( pHdr + 1);
	checkSum = 0;
	for( int i = 0; i < pHdr->checkSumDataSize; i++, pCheckSumData++)
	{
		checkSum ^= *pCheckSumData;
	}

	if( pHdr->checkSum != checkSum)
	{
		return( MEMCARD_RESULT_BADDATA);
	}

	//lets verify the file data
	CARDStat		cardStat;

	gcnResult = CARDGetStatus( Port, CARDGetFileNo( &m_CardFileInfo), &cardStat);
	if( gcnResult != CARD_RESULT_READY)
	{
		CARDClose( &m_CardFileInfo);
		CARDUnmount( Port);
		return( ReturnResult( Port, MapGCNResult( gcnResult)));
	}

	u32	numFileBlocks = BytesToBlocks( cardStat.length);

	ts_gcnMemCardHdr	frontHdr,
							rearHdr;

	//read first block for front header
	gcnResult = CARDRead( &m_CardFileInfo, m_pIOBuffer, MEMCARD_BLOCK_SIZE, MEMCARD_BLOCK_SIZE);
	if( gcnResult != CARD_RESULT_READY)
	{
		CARDClose( &m_CardFileInfo);
		CARDUnmount( Port);
		return( ReturnResult( Port, MapGCNResult( gcnResult)));
	}
	memcpy( &frontHdr, m_pIOBuffer, sizeof(ts_gcnMemCardHdr));

	//read last block for front header
	gcnResult = CARDRead( &m_CardFileInfo, m_pIOBuffer, MEMCARD_BLOCK_SIZE, ( numFileBlocks - 1) * MEMCARD_BLOCK_SIZE);
	if( gcnResult != CARD_RESULT_READY)
	{
		CARDClose( &m_CardFileInfo);
		CARDUnmount( Port);
		return( ReturnResult( Port, MapGCNResult( gcnResult)));
	}
	memcpy( &rearHdr, m_pIOBuffer + ( MEMCARD_BLOCK_SIZE - sizeof(ts_gcnMemCardHdr)), sizeof(ts_gcnMemCardHdr));

	if( frontHdr.versionID != m_VersionID)
	{
		CARDClose( &m_CardFileInfo);
		CARDUnmount( Port);
		return( MEMCARD_RESULT_BADDATA);
	}
	if( rearHdr.versionID != m_VersionID)
	{
		CARDClose( &m_CardFileInfo);
		CARDUnmount( Port);
		return( MEMCARD_RESULT_BADDATA);
	}
	
	if( frontHdr.checkSum != GCN_DATA_CHECKSUM)
	{
		CARDClose( &m_CardFileInfo);
		CARDUnmount( Port);
		return( MEMCARD_RESULT_BADDATA);
	}
	if( rearHdr.checkSum != GCN_DATA_CHECKSUM)
	{
		CARDClose( &m_CardFileInfo);
		CARDUnmount( Port);
		return( MEMCARD_RESULT_BADDATA);
	}

	if( frontHdr.saveTime != rearHdr.saveTime)
	{
		CARDClose( &m_CardFileInfo);
		CARDUnmount( Port);
		return( MEMCARD_RESULT_BADDATA);
	}
	
	//lets read the data
	pData = BasePtr( (char *)pSaveData);
	ASSERT( pData != NULL);
	pHdr = (ts_gcnMemCardHdr *)pData;

	offset = MEMCARD_BLOCK_SIZE;

	while( nBlocksTotal)
	{
		nBlocks = Math::Min( (long)nBlocksTotal, (long)MEMCARD_IOBUFFER_SIZE_BLOCKS);
		s32 nBytesRead = nBlocks * MEMCARD_BLOCK_SIZE;
		
		gcnResult = CARDRead( &m_CardFileInfo, m_pIOBuffer, nBytesRead, offset);
		if( gcnResult != CARD_RESULT_READY)
		{
			CARDClose( &m_CardFileInfo);
			CARDUnmount( Port);
			return( ReturnResult( Port, MapGCNResult( gcnResult)));
		}

		s32	nBytesCopied = Math::Min( (int)nBytesRead, (int)SaveDataSize);

		memcpy( pData, m_pIOBuffer, nBytesCopied);

		SaveDataSize -= nBytesCopied;
		pData += nBytesCopied;
		offset += nBytesRead;
		nBlocksTotal -= nBlocks;
	}

	CARDClose( &m_CardFileInfo);
	CARDUnmount( Port);

	if( gcnResult != CARD_RESULT_READY)
	{
		return( ReturnResult( Port, MapGCNResult( gcnResult)));
	}

	m_aResult[Port] = MEMCARD_RESULT_NEEDDETECT;
	return( MEMCARD_RESULT_OK);
}

MEMCARD_RESULT GCNMemCard::Load( void *pBuffer, size_t SaveDataSize)
{
	return( Load( m_DefaultPort, pBuffer, SaveDataSize));
}

MEMCARD_RESULT GCNMemCard::Save( u32 Port, void *pSaveData, size_t SaveDataSize)
{

	char					*pData;

	s32					gcnResult,
							nBlocksTotal,
							nBlocks,
							nBytes,
							nCheckSum,
							offset;

	u32					*pCheckSumData,
							checkSum;

	ts_gcnMemCardHdr	*pHdr;

	MEMCARD_RESULT		result;

	gcnResult = CARDOpen( Port, m_pDataFileName, &m_CardFileInfo);
	if( gcnResult == CARD_RESULT_NOFILE)
	{
		result = Create( Port, SaveDataSize);
		if( result != MEMCARD_RESULT_OK)
		{
			return( ReturnResult( Port, result));
		}
	}
	else
	if( gcnResult != CARD_RESULT_READY)
	{
		return( ReturnResult( Port, MapGCNResult( gcnResult)));
	}

	nBlocksTotal = GetBufferSize( SaveDataSize);

	pData = BasePtr( (char *)pSaveData);
	
	pHdr = (ts_gcnMemCardHdr *)pData;

	pHdr->versionID = m_VersionID;
	pHdr->checkSum = GCN_DATA_CHECKSUM;
	pHdr->saveTime = OSGetTime( );

	offset = MEMCARD_BLOCK_SIZE;
	while( nBlocksTotal)
	{
		nBlocks = Math::Min( (long)nBlocksTotal, (long)MEMCARD_IOBUFFER_SIZE_BLOCKS);
		nBytes = nBlocks * MEMCARD_BLOCK_SIZE;

		memcpy( m_pIOBuffer, pData, nBytes);

		if( nBlocks == nBlocksTotal)
		{
			ts_gcnMemCardHdr	*pRearHdr;

			pRearHdr = (ts_gcnMemCardHdr *)( m_pIOBuffer + ( nBytes - sizeof(ts_gcnMemCardHdr)));
			*pRearHdr = *pHdr;
		}
		
		gcnResult = CARDWrite( &m_CardFileInfo, m_pIOBuffer, nBytes, offset);
		if( gcnResult != CARD_RESULT_READY)
		{
			CARDClose( &m_CardFileInfo);
			CARDUnmount( Port);
			return( ReturnResult( Port, MapGCNResult( gcnResult)));
		}

		pData += nBytes;
		offset += nBytes;
		nBlocksTotal -= nBlocks;
	}

	CARDClose( &m_CardFileInfo);
	CARDUnmount( Port);

	if( gcnResult != CARD_RESULT_READY)
	{
		return( ReturnResult( Port, MapGCNResult( gcnResult)));
	}

	m_aResult[Port] = MEMCARD_RESULT_NEEDDETECT;
	return( MEMCARD_RESULT_OK);
}

MEMCARD_RESULT GCNMemCard::Save( void *pBuffer, size_t SaveDataSize, int HeaderReadOnly)
{
	return( Save( m_DefaultPort, pBuffer, SaveDataSize, HeaderReadOnly));
}

MEMCARD_RESULT GCNMemCard::Delete( u32 Port)
{

	s32					gcnResult;

	gcnResult = CARDDelete( Port, m_pDataFileName);
	
	return( ReturnResult( Port, MapGCNResult( gcnResult)));
}

MEMCARD_RESULT GCNMemCard::Delete( void)
{
	return( Delete( m_DefaultPort));
}

MEMCARD_RESULT GCNMemCard::DeleteAll( u32 Port, char* TitleID)
{
	return (MEMCARD_RESULT_OK);
}

MEMCARD_RESULT GCNMemCard::Create( u32 Port, size_t SaveDataSize)
{

	char					*pIconData;

	s32					gcnResult,
							nIconBytes,
							nIcons,
							iIcon,
							nBlocks,
							nBytes;

	t_Error				lError;

	ts_gcnMemCardHdr	*pHdr;

	ts_GCNIconHeader	*pIconHdr;

	CARDStat				CardStat;

	MEMCARD_RESULT		result;

	u32					*pCheckSumData,
							checkSum;
	
	nBlocks = GetBufferSize( SaveDataSize) + 1;
	nBytes = nBlocks * MEMCARD_BLOCK_SIZE;

	gcnResult = CARDCreate( Port, m_pDataFileName, nBytes, &m_CardFileInfo);
	if( gcnResult != CARD_RESULT_READY)
	{
		CARDUnmount( Port);
		return( ReturnResult( Port, MapGCNResult( gcnResult)));
	}

	gcnResult = CARDGetStatus( Port, CARDGetFileNo( &m_CardFileInfo), &CardStat);
	if( gcnResult != CARD_RESULT_READY)
	{
		return( ReturnResult( Port, MapGCNResult( gcnResult)));
	}

	FileStream *pStream = FileStream::NewAndOpen( m_pIconFileName, FileStream::READ);

	nIconBytes = pStream->Size( );

	pIconData = (char *)memAlloc( nIconBytes, 32);

	pStream->Read( pIconData, nIconBytes);

	pStream->Close( );

	delete pStream;

	pIconHdr = (ts_GCNIconHeader *)pIconData;

	memset( m_pIOBuffer, 0, MEMCARD_BLOCK_SIZE);

	pHdr = (ts_gcnMemCardHdr *)m_pIOBuffer;
	pHdr->versionID = GCN_ICONBLOCK_ID;
	pHdr->checkSumDataSize = ( nIconBytes + ( sizeof(u32) - 1)) / sizeof(u32);

	//copy icon data at end of checksum/version header
	memcpy( ( pHdr + 1), ( pIconHdr + 1), nIconBytes - sizeof(ts_GCNIconHeader));

	if( m_bCardCommentSet)
	{

		char *pCardComment = (char *)( pHdr + 1);

		//leave first line of comments alone
		pCardComment += 32;
		memcpy( pCardComment, m_acCardComment, 32);
	}

	pCheckSumData = (u32 *)( pHdr + 1);
	checkSum = 0;
	for( int i = 0; i < pHdr->checkSumDataSize; i++, pCheckSumData++)
	{
		checkSum ^= *pCheckSumData;
	}
	pHdr->checkSum = checkSum;

	//set header for user data
	pHdr = (ts_gcnMemCardHdr *)( m_pIOBuffer + MEMCARD_BLOCK_SIZE);								//skip icon block
	pHdr->checkSum = (u32)-1;			//set to bogus checksum
	pHdr->versionID = 0xbadbad;

	gcnResult = CARDWrite( &m_CardFileInfo, m_pIOBuffer, 2 * MEMCARD_BLOCK_SIZE, 0);
	if( gcnResult != CARD_RESULT_READY)
	{
		memFree( pIconData);
		return( ReturnResult( Port, MapGCNResult( gcnResult)));
	}

	CARDSetCommentAddress( &CardStat, 0 + sizeof(ts_gcnMemCardHdr));
	CARDSetIconAddress( &CardStat, 64 + sizeof(ts_gcnMemCardHdr));
	CARDSetBannerFormat( &CardStat, pIconHdr->bannerFormat);
	CARDSetIconAnim( &CardStat, pIconHdr->animType);

	nIcons = 0;
	iIcon = 0;
	while( iIcon < GCN_MAX_ICONS && pIconHdr->iconSpeed[iIcon] != CARD_STAT_SPEED_END)
	{
		nIcons++;
		iIcon++;
	}

	for( iIcon = 0; iIcon < GCN_MAX_ICONS; iIcon++)
	{
		if( iIcon < nIcons)
		{
			CARDSetIconFormat( &CardStat, iIcon, CARD_STAT_ICON_C8);
		}
		else
		{
			CARDSetIconFormat( &CardStat, iIcon, CARD_STAT_ICON_NONE);
		}
		CARDSetIconSpeed( &CardStat, iIcon, pIconHdr->iconSpeed[iIcon]);
	}

	gcnResult = CARDSetStatus( Port, CARDGetFileNo( &m_CardFileInfo), &CardStat);

	memFree( pIconData);
	return( ReturnResult( Port, MapGCNResult( gcnResult)));
}

MEMCARD_RESULT GCNMemCard::UpdateFileComment( const char *pComment)
{
	return( UpdateFileComment( m_DefaultPort, pComment));
}

MEMCARD_RESULT GCNMemCard::UpdateFileComment( u32 Port, const char *pComment)
{

	char					*pIconData;

	s32					gcnResult,
							nIconBytes,
							nIcons,
							iIcon,
							nBlocks,
							nBytes;

	t_Error				lError;

	ts_gcnMemCardHdr	*pHdr;

	ts_GCNIconHeader	*pIconHdr;

	CARDStat				CardStat;

	MEMCARD_RESULT		result;

	u32					*pCheckSumData,
							checkSum;

	m_bCardCommentSet = true;
	for( int i = 0; i < 32; i++)
	{
		if( i < strlen( pComment))
		{
			m_acCardComment[i] = pComment[i];
		}
		else
		{
			m_acCardComment[i] = ' ';
		}
	}
	
	gcnResult = CARDOpen( Port, m_pDataFileName, &m_CardFileInfo);
	if( gcnResult != CARD_RESULT_READY)
	{
		if( gcnResult == CARD_RESULT_NOFILE)
		{
			return( ReturnResult( Port, MEMCARD_RESULT_OK));
		}

		return( ReturnResult( Port, MapGCNResult( gcnResult)));
	}
	
	FileStream *pStream = FileStream::NewAndOpen( m_pIconFileName, FileStream::READ);

	nIconBytes = pStream->Size( );

	pIconData = (char *)memAlloc( nIconBytes, 32);

	pStream->Read( pIconData, nIconBytes);

	pStream->Close( );

	delete pStream;

	pIconHdr = (ts_GCNIconHeader *)pIconData;

	memset( m_pIOBuffer, 0, MEMCARD_BLOCK_SIZE);

	pHdr = (ts_gcnMemCardHdr *)m_pIOBuffer;
	pHdr->versionID = GCN_ICONBLOCK_ID;
	pHdr->checkSumDataSize = ( nIconBytes + ( sizeof(u32) - 1)) / sizeof(u32);

	//copy icon data at end of checksum/version header
	memcpy( ( pHdr + 1), ( pIconHdr + 1), nIconBytes - sizeof(ts_GCNIconHeader));

	//comment is first 64 bytes of icon data
	char *pCardComment = (char *)( pHdr + 1);
	//leave first line of comments alone
	pCardComment += 32;
	memcpy( pCardComment, m_acCardComment, 32);

	pCheckSumData = (u32 *)( pHdr + 1);
	checkSum = 0;
	for( int i = 0; i < pHdr->checkSumDataSize; i++, pCheckSumData++)
	{
		checkSum ^= *pCheckSumData;
	}
	pHdr->checkSum = checkSum;

	gcnResult = CARDWrite( &m_CardFileInfo, m_pIOBuffer, MEMCARD_BLOCK_SIZE, 0);
	if( gcnResult != CARD_RESULT_READY)
	{
		CARDClose( &m_CardFileInfo);
		CARDUnmount( Port);
	}

	memFree( pIconData);
	return( ReturnResult( Port, MapGCNResult( gcnResult)));
}

MEMCARD_RESULT GCNMemCard::GetCardFreeSpace( u32 Port, u32 &freeFiles, u32 &freeBlocks)
{

	s32		gcnResult,
				s32FreeFiles,
				s32FreeBytes;

	gcnResult = CARDFreeBlocks( Port, &s32FreeBytes, &s32FreeFiles);
	freeFiles = s32FreeFiles;
	freeBlocks = BytesToBlocks( s32FreeBytes);

	return( ReturnResult( Port, MapGCNResult( gcnResult)));
}

MEMCARD_RESULT GCNMemCard::GetCardSize( u32 Port, u32 &cardBlocks)
{
	
	s32		gcnResult,
				s32Bytes;

	u16		u16MBits;
	
	gcnResult = CARDGetMemSize( Port, &u16MBits);
	s32Bytes = u16MBits * 1024 * 1024 / 8;
	cardBlocks = BytesToBlocks( s32Bytes);
	
	return( ReturnResult( Port, MapGCNResult( gcnResult)));
}

MEMCARD_RESULT GCNMemCard::MapGCNResult( s32 Result)
{
	switch( Result)
	{
		case CARD_RESULT_READY:
			return( MEMCARD_RESULT_OK);

		case CARD_RESULT_NOCARD:
			return( MEMCARD_RESULT_NOCARD);

		case CARD_RESULT_WRONGDEVICE:
			return( MEMCARD_RESULT_OTHERDEVICE);

		case CARD_RESULT_FATAL_ERROR:
		case CARD_RESULT_IOERROR:
			return( MEMCARD_RESULT_BADCARD);

		case CARD_RESULT_BROKEN:
			return( MEMCARD_RESULT_UNFORMATTED);

		case CARD_RESULT_ENCODING:
			return( MEMCARD_RESULT_OTHERMARKET);

		case CARD_RESULT_NOFILE:
			return( MEMCARD_RESULT_NODATA);

		default:
			return( MEMCARD_RESULT_NOCARD);
	}
}

