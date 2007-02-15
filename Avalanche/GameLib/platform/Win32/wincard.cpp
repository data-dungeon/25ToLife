/*************************************************************************/
// Win32 Memory card framework.
/*************************************************************************/

/******************* includes ****************************/

#include "platform/PlatformPCH.h"
#ifdef DIRECTX_PC
#include <shlobj.h>
#include "platform/keyctrl.h"

#define SAVE_GAME_LOCATION "\\25 to life"

#endif


//#define DEBUG_NOSPACE
//#define DEBUG_NOCARD

//=============================================================================
// WinMemCard::WinMemCard()
//=============================================================================

u32 WinMemCard::m_DefaultPort = 0;

WinMemCard::WinMemCard( void)	: MemCard( )
{
	m_pDataFileName = NULL;
	#ifdef DEBUG_NOCARD
	m_Port[0].m_CardStatus = MEMCARD_RESULT_NOCARD;
	m_Port[1].m_CardStatus = MEMCARD_RESULT_NOCARD;
	#endif
	#ifdef DEBUG_NOSPACE
	m_Port[0].m_CardStatus = MEMCARD_RESULT_NOSPACE;
	m_Port[1].m_CardStatus = MEMCARD_RESULT_NOSPACE;
	#endif

	m_FakeNoCard = 0;
	m_FakeNoSpace = 0;
}

WinMemCard::~WinMemCard( void )
{
	if( m_pDataFileName ) m_pDataFileName = NULL;
}

//=============================================================================
// WinMemCard::ToggleNoCard
//=============================================================================
void	WinMemCard::ToggleNoCard()
{
	m_FakeNoCard = ~m_FakeNoCard;
}

//=============================================================================
// WinMemCard::ToggleNoSpace
//=============================================================================
void	WinMemCard::ToggleNoSpace()
{
	m_FakeNoSpace = ~m_FakeNoSpace;
}

//=============================================================================
// WinMemCard::GetPortText()
//=============================================================================

char * WinMemCard::GetPortText( u32 Port)
{
	if( Port == 0)
	{
		return( "MEMORY CARD Slot A");
	}
	return( "Memory Card Slot ?");
}

//=============================================================================
// WinMemCard::GetCardText()
//=============================================================================

char * WinMemCard::GetCardText( void)
{
	return( "PC Hard Drive");
}

//=============================================================================
// WinMemCard::GetOtherCardText()
//=============================================================================

char * WinMemCard::GetOtherCardText( void)
{
	return( "memory card");
}

//=============================================================================
//  WinMemCard::GetClockTime
//=============================================================================
ts_SystemClock WinMemCard::GetClockTime( void ) {
	ts_SystemClock clockInformation;
	SYSTEMTIME st;

	GetSystemTime(&st); 
	//  handle the year
	clockInformation.year = st.wYear;

	// handle the month
	clockInformation.month = st.wMonth;
	
	// handle the day
	clockInformation.day = st.wDay;

	// handle the hour
	clockInformation.hourclock12 = false;
	clockInformation.hour = st.wHour;

	// handle the minutes
	clockInformation.minute = st.wMinute;

	// handle the seconds
	clockInformation.second = st.wSecond;

	// handle the format of the date  TGS - GCN seems to like this format
	clockInformation.dateformat = MEMCARD_DATE_MMDDYYYY;

	return ( clockInformation );
}

//=============================================================================
// WinMemCard::Initialize()
//=============================================================================

MEMCARD_RESULT WinMemCard::Initialize( const char *pIconFileName, const char *pDataFileName, u32 versionID)
{
	MEM_SET_ONE_SHOT_NEW_NAME("MemCard file");
	m_pDataFileName = m_cDataFileName;
	ASSERT( m_pDataFileName != NULL);
	strcpy( m_pDataFileName, pDataFileName);

	ASSERT( strlen( pDataFileName ) < MAX_FILENAME_SIZE );

	m_VersionID = versionID;

	return MEMCARD_RESULT_OK;
}

//=============================================================================
// WinMemCard::SetFileName()
//=============================================================================

MEMCARD_RESULT WinMemCard::SetFileName( const char *pDataFileName )
{
	MEM_SET_ONE_SHOT_NEW_NAME("MemCard file");
	ASSERT( m_pDataFileName != NULL );
	strcpy( m_pDataFileName, pDataFileName );
	ASSERT( strlen( pDataFileName ) < MAX_FILENAME_SIZE );

	return MEMCARD_RESULT_OK;
}

//=============================================================================
// WinMemCard::UpdateFileComment() - directory on win32
//=============================================================================

MEMCARD_RESULT WinMemCard::UpdateFileComment( const char *pComment)
{
	strcpy(m_Win32Dir, pComment);
	return MEMCARD_RESULT_OK;
}

//=============================================================================
// WinMemCard::SetFileComment() - directory on win32
//=============================================================================

MEMCARD_RESULT WinMemCard::UpdateFileComment( u32 Port, const char *pComment)
{
	strcpy(m_Win32Dir, pComment);
	return MEMCARD_RESULT_OK;
}

//=============================================================================
// WinMemCard::CreateSaveBuffer()
//=============================================================================
char *WinMemCard::CreateLoadSaveBuffer( size_t size )
{
	u32 sizeWithHeader = size + sizeof(ts_winMemCardHdr);

	char *pData = new char[ sizeWithHeader ];
	memset( pData, 0, sizeWithHeader );

	return pData + sizeof(ts_winMemCardHdr);
}

//=============================================================================
// WinMemCard::DestroySaveBuffer()
//=============================================================================
MEMCARD_RESULT WinMemCard::DestroyLoadSaveBuffer( char *pLoadSaveBuffer )
{
	ASSERT( pLoadSaveBuffer );

	void* header = BasePtr( pLoadSaveBuffer );

	delete[] header;

	return( MEMCARD_RESULT_OK );
}

//=============================================================================
// WinMemCard::Load()
//=============================================================================

MEMCARD_RESULT WinMemCard::Load( u32 Port, void *pSaveData, size_t SaveDataSize, int HeaderReadOnly)
{
	return Load( pSaveData, SaveDataSize, HeaderReadOnly);
}

//=============================================================================
// WinMemCard::Load()
//=============================================================================

MEMCARD_RESULT WinMemCard::Load( void *pSaveData, size_t SaveDataSize, int HeaderReadOnly)
{

	ts_winMemCardHdr		hdr;

	FILE		*pFile;

	size_t	countRead;

	if( m_Port[m_DefaultPort].m_CardStatus != MEMCARD_RESULT_OK )
		return m_Port[m_DefaultPort].m_CardStatus;

	if( strlen( m_pDataFileName) == 0)
		return MEMCARD_RESULT_NODATA;

#ifdef DIRECTX_PC
	// we want to use the "my documents" directory for
	// our PC save game and settings location
	char Path[MAX_PATH];
	char myDocumentsLocation[MAX_PATH];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, myDocumentsLocation);
	ASSERT(result == S_OK);
	strcat(myDocumentsLocation, SAVE_GAME_LOCATION);
	sprintf(Path, "%s\\SaveGames\\%s\\%s", myDocumentsLocation, m_Win32Dir, m_pDataFileName);
#else
	char	Path[256];
	sprintf(Path, "SaveGames\\%s\\%s", m_Win32Dir, m_pDataFileName);
#endif
	pFile = fopen( Path, "rb");
	if( pFile == NULL)
	{
		return MEMCARD_RESULT_NODATA;
	}

	countRead = fread( &hdr, 1, sizeof (hdr), pFile);
	if( countRead != sizeof(ts_winMemCardHdr))
	{
		fclose( pFile);
		return MEMCARD_RESULT_BADDATA;
	}

	if( hdr.versionID != m_VersionID)
	{
		fclose( pFile);
		return MEMCARD_RESULT_BADDATA;
	}

	memcpy( BasePtr( ( char * )pSaveData ), &hdr, sizeof( ts_winMemCardHdr ) );
	countRead = fread( pSaveData, 1, SaveDataSize, pFile);
	fclose( pFile);

//	if( countRead != SaveDataSize)
//	{
//		return MEMCARD_RESULT_BADDATA;
//	}
#ifdef DIRECTX_PC
	// if we've loaded our profile, we also need to load our keymap
	#include "platform/keyctrl.h"
	ControllerInputDriver* controller = g_input.GetController( 0 );
	KeyControllerInputDriver* kcid = static_cast<KeyControllerInputDriver*>( controller );
	sprintf(Path, "%s\\SaveGames\\%s\\keymap.ini", myDocumentsLocation, m_Win32Dir);
	kcid->LoadButtonMap(Path);
#endif

	return MEMCARD_RESULT_OK;
}

//=============================================================================
// Win32MemCard::Exists??
//=============================================================================
MEMCARD_RESULT WinMemCard::Exists(u32 Port)
{
	FILE*		fp;

	if(m_Port[m_DefaultPort].m_CardStatus != MEMCARD_RESULT_OK &&
		m_Port[m_DefaultPort].m_CardStatus != MEMCARD_RESULT_NODATA )
	{
		return m_Port[m_DefaultPort].m_CardStatus;
	}

	// create new filename
#ifdef DIRECTX_PC
	// we want to use the "my documents" directory for
	// our PC save game and settings location
	char Path[MAX_PATH];
	char myDocumentsLocation[MAX_PATH];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, myDocumentsLocation);
	ASSERT(result == S_OK);
	strcat(myDocumentsLocation, SAVE_GAME_LOCATION);
	sprintf(Path, "%s\\SaveGames\\%s\\%s", myDocumentsLocation, m_Win32Dir, m_pDataFileName);
#else
	char	Path[256];
	sprintf(Path, "SaveGames\\%s\\%s", m_Win32Dir, m_pDataFileName);
#endif
	fp = fopen( Path, "rb");
	if( fp == NULL)
		return (MEMCARD_RESULT_NODATA);
	else
	{
		fclose(fp);
		return (MEMCARD_RESULT_OK);
	}
}

//=============================================================================
// WinMemCard::Save()
//=============================================================================

MEMCARD_RESULT WinMemCard::Save( u32 Port, void *pSaveData, size_t SaveDataSize)
{
	return Save( pSaveData, SaveDataSize);
}

MEMCARD_RESULT WinMemCard::Delete( u32 Port)
{
	// set path
#ifdef DIRECTX_PC
	// we want to use the "my documents" directory for
	// our PC save game and settings location
	char Path[MAX_PATH];
	char myDocumentsLocation[MAX_PATH];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, myDocumentsLocation);
	ASSERT(result == S_OK);
	strcat(myDocumentsLocation, SAVE_GAME_LOCATION);
	sprintf(Path, "%s\\SaveGames\\%s\\%s", myDocumentsLocation, m_Win32Dir, m_pDataFileName);
#else
	char	Path[256];
	sprintf(Path, "SaveGames\\%s\\%s", m_Win32Dir, m_pDataFileName);
#endif
	DeleteFile(Path);

	// delete the directory now
#ifdef DIRECTX_PC
	// before we delete the directory, we need to delete our keymap file
	sprintf(Path, "%s\\SaveGames\\%s\\%s", myDocumentsLocation, m_Win32Dir, "keymap.ini");
	DeleteFile(Path);
	// we want to use the "my documents" directory for
	// our PC save game and settings location
	sprintf(Path, "%s\\SaveGames\\%s", myDocumentsLocation, m_Win32Dir );
#else
	sprintf(Path, "SaveGames\\%s", m_Win32Dir);
#endif
	if (RemoveDirectory ( Path))
		return( MEMCARD_RESULT_OK);
	else
		return( MEMCARD_RESULT_FAILED);
}

MEMCARD_RESULT WinMemCard::Delete( void)
{
	return( Delete( m_DefaultPort));
}

MEMCARD_RESULT WinMemCard::DeleteAll( u32 Port, char* TitleID)
{
	return (MEMCARD_RESULT_OK);
}

//=============================================================================
// WinMemCard::Save()
//=============================================================================

MEMCARD_RESULT WinMemCard::Save( void *pSaveData, size_t SaveDataSize)
{

	ts_winMemCardHdr		hdr;

	FILE						*pFile;

	size_t	countWritten;

	if(m_Port[m_DefaultPort].m_CardStatus != MEMCARD_RESULT_OK &&
		m_Port[m_DefaultPort].m_CardStatus != MEMCARD_RESULT_NODATA )
	{
		return m_Port[m_DefaultPort].m_CardStatus;
	}

	// create new filename
#ifdef DIRECTX_PC
	// we want to use the "my documents" directory for
	// our PC save game and settings location
	char Path[MAX_PATH];
	char myDocumentsLocation[MAX_PATH];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, myDocumentsLocation);
	ASSERT(result == S_OK);
	strcat(myDocumentsLocation, SAVE_GAME_LOCATION);
	sprintf(Path, "%s\\SaveGames", myDocumentsLocation);
	mkdir(Path);
	sprintf(Path, "%s\\SaveGames\\%s", myDocumentsLocation, m_Win32Dir);
	mkdir(Path);
	sprintf(Path, "%s\\SaveGames\\%s\\%s", myDocumentsLocation, m_Win32Dir, m_pDataFileName);
#else
	char	Path[256];
	mkdir("SaveGames");
	sprintf(Path, "SaveGames\\%s", m_Win32Dir);
	mkdir(Path);
	sprintf(Path, "SaveGames\\%s\\%s", m_Win32Dir, m_pDataFileName);
#endif
	pFile = fopen( Path, "wb");
	if( pFile == NULL)
	{
		return MEMCARD_RESULT_NODATA;
	}

	hdr.versionID = m_VersionID;

	countWritten = fwrite( &hdr, 1, sizeof hdr, pFile);
	if( countWritten != sizeof hdr)
	{
		fclose( pFile);
		return MEMCARD_RESULT_BADCARD;
	}

	countWritten = fwrite( pSaveData, 1, SaveDataSize, pFile);
	fclose( pFile);
	if( countWritten != SaveDataSize)
	{
		return MEMCARD_RESULT_BADCARD;
	}

	InsertCard( 0, MEMCARD_RESULT_OK );

	return MEMCARD_RESULT_OK;
}

//=============================================================================
// WinMemCard::DefaultPort()
//=============================================================================

MEMCARD_RESULT WinMemCard::DefaultPort( u32 Port )
{
	if( Port != 0 && Port != 1 )
		return MEMCARD_RESULT_BADPARAM;

	m_DefaultPort = Port;

	return MEMCARD_RESULT_BADPARAM;
}

//=============================================================================
// WinMemCard::Detected()
//=============================================================================

MEMCARD_RESULT WinMemCard::Detect( u32 Port )
{
	if( Port != 0 )
		return MEMCARD_RESULT_BADPARAM;

	if (m_FakeNoCard)
		return MEMCARD_RESULT_NOCARD;

	return MEMCARD_RESULT_OK;
}

//=============================================================================
// WinMemCard::CheckSpace()
//=============================================================================

MEMCARD_RESULT WinMemCard::CheckSpace( u32 Port, size_t SaveDataSize, u32 &BlocksNeeded)
{
	ULARGE_INTEGER ulFreeAvail;
	BlocksNeeded = (SaveDataSize + 1023) / 1024;

	if( Port != 0)
		return MEMCARD_RESULT_BADPARAM;

	if( m_Port[Port].m_CardStatus == MEMCARD_RESULT_NOSPACE )
		return MEMCARD_RESULT_NOSPACE;

	// check space
	if( !GetDiskFreeSpaceEx(getenv(WIN32_PLATFORM_ENV), &ulFreeAvail, NULL, NULL ) )
		return MEMCARD_RESULT_BADPARAM;

	if( ulFreeAvail.QuadPart < SaveDataSize )
		return MEMCARD_RESULT_NOSPACE;

	if (m_FakeNoSpace)
		return MEMCARD_RESULT_NOSPACE;

	return MEMCARD_RESULT_OK;
}

//=============================================================================
// WinMemCard::CheckSpace()
//=============================================================================

MEMCARD_RESULT WinMemCard::CheckSpace( size_t SaveDataSize, u32 &BlocksNeeded )
{
	return CheckSpace( m_DefaultPort, SaveDataSize, BlocksNeeded );
}

//=============================================================================
// WinMemCard::CheckStatus()
//=============================================================================

MEMCARD_RESULT WinMemCard::CheckStatus( u32 Port )
{
	if( Port != 0)
		return MEMCARD_RESULT_BADPARAM;

	switch( m_Port[Port].m_CardStatus )
	{
	case MEMCARD_RESULT_NODATA:
	case MEMCARD_RESULT_UNFORMATTED:
	case MEMCARD_RESULT_NOSPACE:
	case MEMCARD_RESULT_NOFILES:
	case MEMCARD_RESULT_BADDATA:
	case MEMCARD_RESULT_OTHERMARKET:
	case MEMCARD_RESULT_OTHERDEVICE:
		return MEMCARD_RESULT_OK;
	}

	return m_Port[Port].m_CardStatus;
}

//=============================================================================
// WinMemCard::CheckStatus()
//=============================================================================

MEMCARD_RESULT WinMemCard::CheckStatus( void )
{
	return CheckStatus( m_DefaultPort );
}

//=============================================================================
// WinMemCard::Format()
//=============================================================================

MEMCARD_RESULT WinMemCard::Format( u32 Port )
{
	if( Port != 0)
		return MEMCARD_RESULT_BADPARAM;

	if( m_Port[Port].m_CardStatus == MEMCARD_RESULT_UNFORMATTED )
		InsertCard( Port, MEMCARD_RESULT_NODATA );
	
	return m_Port[Port].m_CardStatus;
}

//=============================================================================
// WinMemCard::Format()
//=============================================================================

MEMCARD_RESULT WinMemCard::Format( void )
{
	return Format( m_DefaultPort );
}

//=============================================================================
// WinMemCard::RemoveCard()
//=============================================================================

void WinMemCard::RemoveCard( u32 Port )
{
	if( Port == 0)
		m_Port[Port].m_CardStatus = MEMCARD_RESULT_NOCARD;

	return;
}

//=============================================================================
// WinMemCard::InsertCard()
//=============================================================================

void WinMemCard::InsertCard( u32 Port, MEMCARD_RESULT Result )
{
	if( Port == 0)
		m_Port[Port].m_CardStatus = Result;

	#ifdef DEBUG_NOCARD
	if( Port == 0) m_Port[Port].m_CardStatus = MEMCARD_RESULT_NOCARD;
	#endif
	#ifdef DEBUG_NOSPACE
	if( Port == 0) m_Port[Port].m_CardStatus = MEMCARD_RESULT_NOSPACE;
	#endif

	return;
}

//=============================================================================
// WinMemCard::MakeString()
//=============================================================================

char * WinMemCard::ResultAsText( MEMCARD_RESULT Result )
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


