/*************************************************************************/
// PS2 Memory card framework.
/*************************************************************************/

/******************* includes ****************************/

#include "platform/PlatformPCH.h"



#include "EngineHelper/filestream.h"

#define CARD_FILENAME_MAX		20

PS2MemCard::PS2MemCard( void)	: MemCard( )
{
	strcpy( m_DirName, "");
	strcpy( m_DataFileName, "");
	strcpy( m_IconFileName, "");
	strcpy( m_IconSysName, "");
	m_IconFileSize = 0;
}

// wait for memcard function to finish executing
int	PS2MemCard::Sync()
{
	int cmd    = -1;
	int result = -1;
	sceMcSync(0, &cmd, &result);
	return result;
}

char * PS2MemCard::GetPortText( u32 Port)
{
	if( Port == 0)
	{
		return( "MEMORY CARD slot 1");
	}
	else
	if( Port == 1)
	{
		return( "MEMORY CARD slot 2");
	}
	return NULL;
}

char * PS2MemCard::GetCardText( void)
{
	return( "memory card (8MB)(for PlayStation(R) 2)");
}

char * PS2MemCard::GetOtherCardText( void)
{
	return( "memory card");
}

char * PS2MemCard::ResultAsText( MEMCARD_RESULT Result )
{
	char *pResult = 0 ;

	switch( Result )
	{
	case MEMCARD_RESULT_OK:
		pResult = "MEMCARD_RESULT_OK";
		break;
	case MEMCARD_RESULT_NOCARD:
		pResult = "MEMCARD_RESULT_NOCARD";
		break;
	case MEMCARD_RESULT_NODATA:
		pResult = "MEMCARD_RESULT_NODATA";
		break;
	case MEMCARD_RESULT_UNFORMATTED:
		pResult = "MEMCARD_RESULT_UNFORMATTED";
		break;
	case MEMCARD_RESULT_NOSPACE:
		pResult = "MEMCARD_RESULT_NOSPACE";
		break;
	case MEMCARD_RESULT_NOFILES:
		pResult = "MEMCARD_RESULT_NOFILES";
		break;
	case MEMCARD_RESULT_BADCARD:
		pResult = "MEMCARD_RESULT_BADCARD";
		break;
	case MEMCARD_RESULT_BADDATA:
		pResult = "MEMCARD_RESULT_BADDATA";
		break;
	case MEMCARD_RESULT_WRONGTYPE:
		pResult = "MEMCARD_RESULT_WRONGTYPE";
		break;
	case MEMCARD_RESULT_OTHERMARKET:
		pResult = "MEMCARD_RESULT_OTHERMARKET";
		break;
	case MEMCARD_RESULT_OTHERDEVICE:
		pResult = "MEMCARD_RESULT_OTHERDEVICE";
		break;
	case MEMCARD_RESULT_BADPARAM:
		pResult = "MEMCARD_RESULT_BADPARAM";
		break;
	case MEMCARD_RESULT_NOTINIT:
		pResult = "MEMCARD_RESULT_NOTINIT";
		break;
	case MEMCARD_RESULT_NEEDDETECT:
		pResult = "MEMCARD_RESULT_NEEDDETECT";
		break;
	default:
		pResult = "INVALID";
		break;
	}

	return pResult;
}

//  Get the actual system clock from the PS2
ts_SystemClock PS2MemCard::GetClockTime( void ) {
	ts_SystemClock clockInformation;
	int result, clocksettings, timenotation;
	sceCdCLOCK clocktime;

	//  Get the system clock
	result = sceCdReadClock ( &clocktime );
	//  TGS - if clocktime.stat returns a value other than 0, there is a problem
	//  with the clock and it returns a default value, I'm not sure what to do with it
	//  so I'm going to treat it as a real time.

	sceScfGetLocalTimefromRTC( &clocktime );

	clocksettings = sceScfGetDateNotation();
	timenotation = sceScfGetTimeNotation();

	//  handle the year
	if ( clocktime.year == 0x99 ) {
		//  TGS - this is a stupid case, but I want to make sure some dipsh*t doesn't fail us for it.
		//  I'm doing what the test deck does when you go to 99, which reverts to 1999 )
		clockInformation.year = 1999;
	} else {
		clockInformation.year = (int)( 2000 + (int)( (int)clocktime.year / 16 ) * 10 + Math::Mod( (int)clocktime.year, 16 ) );
	}

	// handle the month
	clockInformation.month = (int)((int)( (int)clocktime.month / 16 ) * 10 + Math::Mod( (int)clocktime.month, 16 ));
	
	// handle the day
	clockInformation.day = (int)((int)( (int)clocktime.day / 16 ) * 10 + Math::Mod( (int)clocktime.day, 16 ));

	// handle the hour
	if ( timenotation == SCE_TIME_12HOUR )
		clockInformation.hourclock12 = true;
	else
		clockInformation.hourclock12 = false;
	clockInformation.hour = (int)((int)( (int)clocktime.hour / 16 ) * 10 + Math::Mod( (int)clocktime.hour, 16 ));

	// handle the minutes
	clockInformation.minute = (int)((int)( (int)clocktime.minute / 16 ) * 10 + Math::Mod( (int)clocktime.minute, 16 ));

	// handle the seconds
	clockInformation.second = (int)((int)( (int)clocktime.second / 16 ) * 10 + Math::Mod( (int)clocktime.second, 16 ));

	// handle the format of the date
	switch ( clocksettings ) {
		case SCE_DATE_YYYYMMDD :
			clockInformation.dateformat = MEMCARD_DATE_YYYYMMDD;
			break;
		case SCE_DATE_DDMMYYYY :
			clockInformation.dateformat = MEMCARD_DATE_DDMMYYYY;
			break;
		case SCE_DATE_MMDDYYYY :
		default:
			clockInformation.dateformat = MEMCARD_DATE_MMDDYYYY;
			break;
	}

	return ( clockInformation );
}

MEMCARD_RESULT PS2MemCard::Initialize( const char *pIconFileName, const char *pDataFileName, u32 versionID)
{
	int					ps2Result;

	if( strlen( pDataFileName) >= MEMCARD_PS2_DIRNAME_SIZE)
	{
		ASSERT( 0);
		return( MEMCARD_RESULT_BADPARAM);
	}

	if( strlen( pIconFileName) >= MEMCARD_PS2_FILENAME_SIZE)
	{
		ASSERT( 0);
		return( MEMCARD_RESULT_BADPARAM);
	}

	// to reliably ChDir into directory, make it an absolute path
	strcpy( m_DirName, "/");
	strcat( m_DirName, pDataFileName);

	strcpy( m_DataFileName, pDataFileName);
	strcpy( m_IconFileName, pIconFileName);
	m_pIconFileName = (char *)&m_IconFileName;

	//looks like the icon.sys file MUST BE NAMED icon.sys
	strcpy( m_IconSysName, "icon.sys");
	m_pIconSysName = (char *)&m_IconSysName;
	
	//get the icon file size (for required space computations)
	FileStream *pStream = FileStream::NewAndOpen( m_pIconFileName, FileStream::READ);
	m_IconFileSize = pStream->Size( );
	pStream->Close( );
	delete pStream;


	for( u32 port = 0; port < MEMCARD_MAX_PORTS; port++)
	{
		m_aResult[port] = MEMCARD_RESULT_NEEDDETECT;
		m_cardID[port] = 0;
		m_lastValidCardID[port] = 0;
	}
	
	m_VersionID = versionID;

	ps2Result = sceMcInit();	
	if( ps2Result != 0)
	{
//		if(ps2Result == -101){	printf("mc init failed\n");}
//		if(ps2Result == -120){	printf("mcserv.irx old\n");}
//		if(ps2Result == -121){	printf("mcman.irx old \n");}

		ASSERT( 0);
		return( ReturnResult( 0, MEMCARD_RESULT_NOTINIT));
	}

	m_bInitialized = true;

	return( MEMCARD_RESULT_OK);
}

MEMCARD_RESULT PS2MemCard::SetFileName( const char *pDataFileName )
{
	if( strlen( pDataFileName) >= MEMCARD_PS2_FILENAME_SIZE)
	{
		ASSERT( 0);
		return( MEMCARD_RESULT_BADPARAM);
	}

	strcpy( m_DataFileName, pDataFileName);

	return( MEMCARD_RESULT_OK );
}

char *PS2MemCard::CreateLoadSaveBuffer( size_t size )
{
	int	nBlocks, nBytes;
	char	*pData;

	nBlocks = BytesToBlocks( size + sizeof( ts_ps2MemCardHdr ) );
	nBytes = nBlocks * MEMCARD_BLOCK_SIZE;

	MEM_SET_ALLOC_NAME("PS2MemCardCreateLoadSaveBuf");
	pData = ( char *)memAlloc( nBytes, 64);
	ASSERT( pData != NULL);
	MEM_SET_ALLOC_NAME("");
	
	memset( pData, 0, nBytes );
	return( pData + sizeof( ts_ps2MemCardHdr ) );
}

MEMCARD_RESULT PS2MemCard::DestroyLoadSaveBuffer( char *pLoadSaveBuffer )
{
	memFree( BasePtr( pLoadSaveBuffer) );

	return( MEMCARD_RESULT_OK );
}

MEMCARD_RESULT PS2MemCard::DefaultPort( u32 Port)
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

MEMCARD_RESULT PS2MemCard::Detect( u32 Port)
{

	int					ps2Result,
							cardType,
							formatted,
							cmd;

	if( !m_bInitialized)
	{
		return( MEMCARD_RESULT_NOTINIT);
	}

	ps2Result = sceMcGetInfo( Port, 0, &cardType, NULL, &formatted);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();
	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));

	if( ps2Result == -5 || cardType != 2)
		return( ReturnResult( Port, MEMCARD_RESULT_WRONGTYPE));

	if( ps2Result == -2 || formatted == 0)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));

	return( ReturnResult( Port, MEMCARD_RESULT_OK));
}

s32 PS2MemCard::ReturnIconFileSize( void ) {
	ASSERT( m_IconFileSize > 0 );
	return ( m_IconFileSize );
}

MEMCARD_RESULT 
PS2MemCard::CheckSpace( 
							  u32 Port, 
							  size_t SaveDataSize, 
							  u32 &BlocksNeeded)
{

	int					ps2Result,
							freeClusters,
							cmd,
							spaceNeeded;

	sceMcTblGetDir		*pDirTable;

	char					*pDir;

	ASSERT( m_IconFileSize > 0 );

	spaceNeeded = BlocksNeeded;

	if( !m_bInitialized)
		return( MEMCARD_RESULT_NOTINIT);

	// --- get info about our free clusters
	ps2Result = sceMcGetInfo( Port, 0, NULL, &freeClusters, NULL);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();

	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));

	if( ps2Result == -2)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));

	// --- change to our directory
	ps2Result = sceMcChdir( Port, 0, m_DirName, NULL);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();

	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
	if( ps2Result == -2)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));
	if( ps2Result == -4) // NO DATA ON CARD
	{
		if( freeClusters < spaceNeeded)
			return( ReturnResult( Port, MEMCARD_RESULT_NOSPACE));
		return( ReturnResult( Port, MEMCARD_RESULT_OK));
	}

	// --- now try to open the file
	ps2Result = sceMcOpen( Port, 0, m_DataFileName, SCE_RDONLY);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();

	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
	if( ps2Result <= -5)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));
	if( ps2Result == -2)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));
	if( ps2Result == -4)	// NO DATA ON CARD
	{
		if( freeClusters < spaceNeeded)
			return( ReturnResult( Port, MEMCARD_RESULT_NOSPACE));
		return( ReturnResult( Port, MEMCARD_RESULT_OK));
	}

	// --- now try to close the file
	ps2Result = sceMcClose( ps2Result );
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();

	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
	if( ps2Result <= -4)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));
	if( ps2Result == -2)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));
	return( ReturnResult( Port, MEMCARD_RESULT_OK));
}
	
MEMCARD_RESULT PS2MemCard::CheckSpace( size_t SaveDataSize, u32 &BlocksNeeded)
{
	return( CheckSpace( m_DefaultPort, SaveDataSize, BlocksNeeded));
}

MEMCARD_RESULT PS2MemCard::CheckStatus( u32 Port)
{
//	return( MEMCARD_RESULT_NOCARD); //JMH - SLOW!!!!

	int					ps2Result,
							cmd;

	if( !m_bInitialized)
		return( MEMCARD_RESULT_NOTINIT);

	ps2Result = sceMcGetInfo( Port, 0, NULL, NULL, NULL);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();

	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));

	return( ReturnResult( Port, MEMCARD_RESULT_OK));
}
	
MEMCARD_RESULT PS2MemCard::CheckStatus( void)
{
	return( CheckStatus( m_DefaultPort));
}

MEMCARD_RESULT PS2MemCard::Format( u32 Port)
{

	int					ps2Result,
							cmd;

	if( !m_bInitialized)
		return( MEMCARD_RESULT_NOTINIT);

	ps2Result = sceMcFormat( Port, 0);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();

	if( ps2Result < 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADCARD));

	return( ReturnResult( Port, MEMCARD_RESULT_OK));
}
	
MEMCARD_RESULT PS2MemCard::Format( void)
{
	return( Format( m_DefaultPort));
}

MEMCARD_RESULT PS2MemCard::GetDirectory( u32 Port, char* Filename )
{
	int	ps2Result;
	int	cmd;

	// clear the directory
	memset(m_Directory, 0, sizeof(m_Directory));
	m_NumFiles = 0;

	//change to our game directory
	ps2Result = sceMcChdir( Port, 0, m_DirName, NULL);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();
	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
	if( ps2Result == -2)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));
	if( ps2Result == -4)
		return( ReturnResult( Port, MEMCARD_RESULT_NODATA));

	// get the directory listing now
	ps2Result = sceMcGetDir( Port, 0, Filename, 0, 32, &m_Directory[0] );
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();

	m_NumFiles = SetNumFiles();

	if (ps2Result >= 0)
		return (MEMCARD_RESULT_OK);

	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
	if (ps2Result == -2)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));
	if (ps2Result == -4)
		return( ReturnResult( Port, MEMCARD_RESULT_NODATA));

	return (MEMCARD_RESULT_BADPARAM);
}

int PS2MemCard::SetNumFiles()
{
	int	i;
	int	NumFiles = 0;

	for (i = 0; i < MEMCARD_PS2_MAX_FILES_DIR; i++)
	{
		if ( strcmp((const char*)m_Directory[i].EntryName, ""))
			NumFiles++;
	}
	return (NumFiles);
}

MEMCARD_RESULT PS2MemCard::Load( u32 Port, void *pSaveData, size_t SaveDataSize, int HeaderReadOnly)
{

	char					*pData;

	int					ps2Result,
							ps2CloseResult,
							nBlocks,
							nBytes,
							nCheckSum,
							fd,
							cmd;

	u32					*pCheckSumData,
							checkSum;

	ts_ps2MemCardHdr	*pHdr;

	if( m_aResult[Port] != MEMCARD_RESULT_OK)
		return( m_aResult[Port]);

	//change to our directory
	ps2Result = sceMcChdir( Port, 0, m_DirName, NULL);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();
	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
	if( ps2Result == -2)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));
	if( ps2Result == -4)
		return( ReturnResult( Port, MEMCARD_RESULT_NODATA));

	//open our data file
	ps2Result = sceMcOpen( Port, 0, m_DataFileName, SCE_RDONLY);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();
	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
	if( ps2Result <= -5)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));
	if( ps2Result == -2)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));
	if( ps2Result == -4)
		return( ReturnResult( Port, MEMCARD_RESULT_NODATA));

	fd = ps2Result;

	if (HeaderReadOnly != HEADER_READ_ONLY)
	{
		nBlocks = BytesToBlocks( SaveDataSize + sizeof( ts_ps2MemCardHdr ) );
		nBytes = nBlocks * MEMCARD_BLOCK_SIZE;
	}
	else
	{
		nBlocks = BytesToBlocks( SaveDataSize );
		nBytes = nBlocks * MEMCARD_BLOCK_SIZE;
	}

	pData = BasePtr( ( char * )pSaveData );
	ASSERT( pData != NULL);

	//read our data file
	ps2Result = sceMcRead( fd, pData, nBytes);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();

	ps2CloseResult = sceMcClose( fd);
	if( ps2CloseResult != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2CloseResult = Sync();

	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
	if( ps2Result <= -4)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));
	if( ps2Result == -2)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));
	if( ps2Result != nBytes)
		return( ReturnResult( Port, MEMCARD_RESULT_BADDATA));

	nCheckSum = ( SaveDataSize + ( sizeof(u32) - 1)) / sizeof(u32);

	pHdr = (ts_ps2MemCardHdr *)pData;

	if( pHdr->versionID != m_VersionID)
		return( MEMCARD_RESULT_BADDATA);

	m_aResult[Port] = MEMCARD_RESULT_NEEDDETECT;
	return( MEMCARD_RESULT_OK);
}

MEMCARD_RESULT PS2MemCard::Load( void *pSaveData, size_t SaveDataSize, int HeaderReadOnly)
{
	return( Load( m_DefaultPort, pSaveData, SaveDataSize, HeaderReadOnly));	
}

MEMCARD_RESULT PS2MemCard::Save( u32 Port, void *pSaveData, size_t SaveDataSize)
{

	char					*pData;

	int					ps2Result,
							nBlocks,
							nBytes,
							nCheckSum,
							fd,
							cmd;

	u32					*pCheckSumData,
							checkSum;

	ts_ps2MemCardHdr	*pHdr;

	MEMCARD_RESULT		result;

	while( true)
	{
		//change to our directory
		ps2Result = sceMcChdir( Port, 0, m_DirName, NULL);
		if( ps2Result != 0)
			return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

		ps2Result = Sync();
		if( ps2Result <= -10)
			return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
		if( ps2Result == -2)
			return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));
		if( ps2Result == -4)
		{
			Detect(Port);
			result = Create( Port, SaveDataSize);
			if( result != MEMCARD_RESULT_OK)
				return( ReturnResult( Port, result));
			break;
		}
		if( ps2Result == 0)
			break;
	}

	while( true)
	{
		//open our data file
		ps2Result = sceMcOpen( Port, 0, m_DataFileName, SCE_WRONLY);
		if( ps2Result != 0)
			return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

		ps2Result = Sync();
		if( ps2Result <= -10)
			return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
		if( ps2Result <= -5)
			return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));
		if( ps2Result == -2)
			return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));
		if( ps2Result == -4)
		{
			result = Create( Port, SaveDataSize);
			if( result != MEMCARD_RESULT_OK)
				return( ReturnResult( Port, result));
		}
		if( ps2Result >= 0)
		{
			fd = ps2Result;
			break;
		}
	}

	nBlocks = BytesToBlocks( SaveDataSize + sizeof(ts_ps2MemCardHdr));
	nBytes = nBlocks * MEMCARD_BLOCK_SIZE;

	pData = BasePtr( ( char * )pSaveData );
	ASSERT( pData != NULL);
	
	pHdr = (ts_ps2MemCardHdr *)pData;

	pCheckSumData = (u32 *)( pHdr + 1);
	checkSum = 0;
	pHdr->checkSum = checkSum;
	pHdr->versionID = m_VersionID;

	ps2Result = sceMcWrite( fd, pData, nBytes);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();
	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
	if( ps2Result <= -4)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));
	if( ps2Result == -2)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));

	if( ps2Result != nBytes)
		return( ReturnResult( Port, MEMCARD_RESULT_BADDATA));

	ps2Result = sceMcClose( fd);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();
	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
	if( ps2Result == -4)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));
	if( ps2Result == -2)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));

	m_aResult[Port] = MEMCARD_RESULT_NEEDDETECT;
	return( MEMCARD_RESULT_OK);
}

MEMCARD_RESULT PS2MemCard::Save( void *pBuffer, size_t SaveDataSize)
{
	return( Save( m_DefaultPort, pBuffer, SaveDataSize));
}

MEMCARD_RESULT PS2MemCard::Delete( u32 Port)
{
	int					ps2Result, cmd;

	//change to our directory
	ps2Result = sceMcChdir( Port, 0, m_DirName, NULL);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();

	if (ps2Result == -4 || ps2Result == -2 || ps2Result <= -10)
		return(MEMCARD_RESULT_NODATA);

	// delete our file...

	//open our data file
	ps2Result = sceMcDelete( Port, 0, m_DataFileName);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();

	if(ps2Result <= -10 || ps2Result <= -5 || ps2Result == -2 || ps2Result == -4)
		return(MEMCARD_RESULT_NODATA);

	return( MEMCARD_RESULT_OK);
}

//=============================================================================
// PS2MemCard::DeleteAll
//=============================================================================

MEMCARD_RESULT PS2MemCard::DeleteAll( u32 Port, char* TitleID)
{
	int					ps2Result, cmd;
	int					result = 0;

	//change to our directory
	ps2Result = sceMcChdir( Port, 0, m_DirName, NULL);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();

	if (ps2Result == -4 || ps2Result == -2 || ps2Result <= -10)
		result = 1;

	// delete our file...
	ps2Result = sceMcDelete( Port, 0, m_DataFileName);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();

	if(ps2Result <= -10 || ps2Result <= -5 || ps2Result == -2 || ps2Result == -4)
		result = 1;

	// delete the icon files
//	ps2Result = sceMcDelete( Port, 0, "icon.sys");
//	if( ps2Result != 0)
//		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

//	ps2Result = Sync();

//	if(ps2Result <= -10 || ps2Result <= -5 || ps2Result == -2 || ps2Result == -4)
//		result = 1;

	// delete icons
//	ps2Result = sceMcDelete( Port, 0, "logodat.ico");
//	if( ps2Result != 0)
//		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

//	ps2Result = Sync();

//	if(ps2Result <= -10 || ps2Result <= -5 || ps2Result == -2 || ps2Result == -4)
//		result = 1;

	// delete dummy file
//	ps2Result = sceMcDelete( Port, 0,  TitleID);
//	if( ps2Result != 0)
//		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

//	ps2Result = Sync();

//	if(ps2Result <= -10 || ps2Result <= -5 || ps2Result == -2 || ps2Result == -4)
//		result = 1;

	// if a patch exists, delete it also
//	ps2Result = sceMcDelete( Port, 0,  "patch.pat");
//	if( ps2Result != 0)
//		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

//	ps2Result = Sync();


	//change to root and delete the directory
//	ps2Result = sceMcChdir( Port, 0, ".", NULL);
//	if( ps2Result != 0)
//		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

//	ps2Result = Sync();

//	if (ps2Result == -4 || ps2Result == -2 || ps2Result <= -10)
//		result = 1;

//	ps2Result = sceMcDelete( Port, 0,  m_DirName);
//	if( ps2Result != 0)
//		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

//	ps2Result = Sync();

//	if (ps2Result == -4 || ps2Result == -2 || ps2Result <= -10)
//		result = 1;

	// return..
	if (result)
		return(MEMCARD_RESULT_NODATA);

	return( MEMCARD_RESULT_OK);
}

//=============================================================================
// Win32MemCard::Exists??
//=============================================================================
MEMCARD_RESULT PS2MemCard::Exists(u32 Port)
{
	int					ps2Result, cmd;

	//change to our directory
	ps2Result = sceMcChdir( Port, 0, m_DirName, NULL);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();

	if (ps2Result == -4 || ps2Result == -2 || ps2Result <= -10)
		return(MEMCARD_RESULT_NODATA);

	//open our data file
	ps2Result = sceMcOpen( Port, 0, m_DataFileName, SCE_RDONLY);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();

	if(ps2Result <= -10 || ps2Result <= -5 || ps2Result == -2 || ps2Result == -4)
	{
		sceMcClose(ps2Result);
		ps2Result = Sync();
		return(MEMCARD_RESULT_NODATA);
	}

	sceMcClose(ps2Result);
	ps2Result = Sync();
	return( MEMCARD_RESULT_OK);
}

MEMCARD_RESULT PS2MemCard::Delete( void)
{
	return( Delete( m_DefaultPort));
}

MEMCARD_RESULT PS2MemCard::Create( u32 Port, size_t SaveDataSize)
{

	char					*pData;

	int					ps2Result,
							nBlocks,
							nBytes,
							nCheckSum,
							fd,
							cmd;

	u32					*pCheckSumData,
							checkSum;

	ts_ps2MemCardHdr	*pHdr;

	MEMCARD_RESULT		result;

	//create our directory
	ps2Result = sceMcMkdir( Port, 0, (char *)&m_DirName[1]);//[1] to get past "/"
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();
	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
	if( ps2Result == -2)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));
	if( ps2Result == -3)
		return( ReturnResult( Port, MEMCARD_RESULT_NOSPACE));

	//change to our directory
	ps2Result = sceMcChdir( Port, 0, m_DirName, NULL);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();
	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
	if( ps2Result == -2)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));
	if( ps2Result == -3)
		return( ReturnResult( Port, MEMCARD_RESULT_NOSPACE));

	Detect(Port);
	//create and write the icon.sys file: by our Avalanche convention, it is the first 964 bytes of the icon data file.
	//use ps2iconsys to create a icon.sys file, then concatenate that with the icon file with: copy /b icon.sys+icon.ico icondat.ico

	//first load it in from CD
	FileStream *pStream = FileStream::NewAndOpen( m_pIconFileName, FileStream::READ);

	m_IconFileSize = pStream->Size( );
	
	pData = (char *)memAlloc( m_IconFileSize);

	pStream->Read( pData, m_IconFileSize);

	pStream->Close( );

	delete pStream;

	//the first 964 bytes are the icon.sys file - write it
	sceMcIconSys *pIconSys = (sceMcIconSys *)pData;
	result = WriteFile( Port, m_pIconSysName, pIconSys, sizeof(sceMcIconSys));
	if( result != MEMCARD_RESULT_OK)
		return( result);

	//the rest of the file is the icon itself - write it
	char *pIconData = (char *)(pIconSys+1);//the icon data is 16-byte aligned
	result = WriteFile( Port, m_pIconFileName, pIconData, m_IconFileSize - sizeof(sceMcIconSys));
	if( result != MEMCARD_RESULT_OK)
		return( result);

	memFree(pData);

	//prepare a blank data file for output to the memcard
	nBlocks = BytesToBlocks( SaveDataSize + sizeof(ts_ps2MemCardHdr));
	nBytes = nBlocks * MEMCARD_BLOCK_SIZE;

	pData = (char *)memAlloc( nBytes, 64);
	ASSERT( pData != NULL);
	memset( pData, 0, nBytes);
	
	pHdr = (ts_ps2MemCardHdr *)pData;
	pHdr->checkSum = (u32)-1;			//set to bogus checksum
	pHdr->versionID = 0xbadbad;

/*
	//also set up the redundant second block
	pHdr = (ts_ps2MemCardHdr *)( pData + ( nBlocks * MEMCARD_BLOCK_SIZE));
	pHdr->checkSum = -1;			//set to bogus checksum
	pHdr->versionID = 0xbadbad;*/

	WriteFile( Port, m_DataFileName, pData, nBytes);

	memFree( pData);

	return( MEMCARD_RESULT_OK);
}

MEMCARD_RESULT PS2MemCard::FileCreate( u32 Port, char *pFileName, int *fd)
{
	int					ps2Result;
	int					cmd;

	ps2Result = sceMcOpen( Port, 0, pFileName, SCE_RDWR | SCE_CREAT);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	*fd = ps2Result;

	ps2Result = Sync();

	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
	if( ps2Result <= -4)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));
	if( ps2Result == -2)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));
	if( ps2Result == -3)
		return( ReturnResult( Port, MEMCARD_RESULT_NOSPACE));
	
	return( MEMCARD_RESULT_OK);
}

MEMCARD_RESULT PS2MemCard::FileWrite( u32 Port, int fd, void *pData, int nBytes)
{
	int					ps2Result;
	int					cmd;

	ps2Result = sceMcWrite( fd, pData, nBytes);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();
	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
	if( ps2Result <= -4)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));
	if( ps2Result <= -3)
		return( ReturnResult( Port, MEMCARD_RESULT_NOSPACE));
	if( ps2Result == -2)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));

	if( ps2Result != nBytes)
		return( ReturnResult( Port, MEMCARD_RESULT_BADDATA));

	ps2Result = sceMcFlush( fd);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();
	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
	if( ps2Result == -4)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));
	if( ps2Result == -2)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));

	return( MEMCARD_RESULT_OK);
}

MEMCARD_RESULT PS2MemCard::FileClose( u32 Port, int fd)
{
	int					ps2Result;
	int					cmd;

	ps2Result = sceMcClose( fd);
	if( ps2Result != 0)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));

	ps2Result = Sync();
	if( ps2Result <= -10)
		return( ReturnResult( Port, MEMCARD_RESULT_NOCARD));
	if( ps2Result == -4)
		return( ReturnResult( Port, MEMCARD_RESULT_BADPARAM));
	if( ps2Result == -2)
		return( ReturnResult( Port, MEMCARD_RESULT_UNFORMATTED));

	return( MEMCARD_RESULT_OK);
}

MEMCARD_RESULT PS2MemCard::WriteFile( u32 Port, char *pFileName, void *pData, int nBytes)
{
	int fd;
	MEMCARD_RESULT result;

	result = FileCreate( Port, pFileName, &fd);
	if( result != 	MEMCARD_RESULT_OK)
		return( result);

	result = FileWrite( Port, fd, pData, nBytes);
	if( result != 	MEMCARD_RESULT_OK)
		return( result);

	result = FileClose( Port, fd);
	if( result != 	MEMCARD_RESULT_OK)
		return( result);

	return( MEMCARD_RESULT_OK);
}

MEMCARD_RESULT PS2MemCard::ReturnResult( u32 Port, MEMCARD_RESULT Result )
{
	// try to give cards a unique id
	if ( Result == MEMCARD_RESULT_NOCARD )
		m_cardID[Port] = 0;
	else if ( Result == MEMCARD_RESULT_OK && m_cardID[Port] == 0 )
		m_lastValidCardID[Port] = m_cardID[Port] = (m_lastValidCardID[Port] < 255) ? m_lastValidCardID[Port] + 1 : 1;

	m_aResult[Port] = Result;
	return( Result);
}

