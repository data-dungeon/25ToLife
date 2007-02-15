/*************************************************************************/
// Xbox Memory card framework.
/*************************************************************************/

/******************* includes ****************************/


#include "platform/PlatformPCH.h"

#include "GameHelper/StringHelper.h"
//#define DEBUG_NOSPACE
//#define DEBUG_NOCARD


#define	SAVE_GAME_CHUNK_SIZE		1024		// num bytes per chunk
#define	SIGNED								// sign save games for Xbox TCR

//=============================================================================
// XboxMemCard::XboxMemCard()
//=============================================================================

//u32 XboxMemCard::m_DefaultPort = 0;

XboxMemCard::XboxMemCard( void)	: MemCard( )
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
	
}

XboxMemCard::~XboxMemCard( void )
{
	if( m_pDataFileName ) m_pDataFileName = NULL;
}

//=============================================================================
// XboxMemCard::GetPortText()
//=============================================================================

char * XboxMemCard::GetPortText( u32 Port)
{
	if( Port == 0)
	{
		return( "Hard Drive");
	}
	else
	if( Port == 1)
	{
		return( "Hard Drive");
	}
	return( "Hard Drive ?");
}

//=============================================================================
// XboxMemCard::GetCardText()
//=============================================================================

char * XboxMemCard::GetCardText( void)
{
	return( "Hard Drive");
}

//=============================================================================
// XboxMemCard::GetOtherCardText()
//=============================================================================

char * XboxMemCard::GetOtherCardText( void)
{
	return( "Hard Drive");
}

//=============================================================================
//  XboxMemCard::GetClockTime
//=============================================================================
ts_SystemClock XboxMemCard::GetClockTime( void ) {
	ts_SystemClock clockInformation;
	SYSTEMTIME st;

	// get time UTC
	GetLocalTime(&st);

	//  handle the year
	clockInformation.year = st.wYear;

	// handle the month
	clockInformation.month = st.wMonth;
	
	// handle the day
	clockInformation.day = st.wDay;

	// handle the hour
	clockInformation.hourclock12 = true;
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
// XboxMemCard::Initialize()
//=============================================================================

MEMCARD_RESULT XboxMemCard::Initialize( const char *pIconFileName, const char *pDataFileName, u32 versionID)
{
	m_pDataFileName = m_cDataFileName;
	ASSERT( m_pDataFileName != NULL);
	strcpy( m_pDataFileName, pDataFileName);

	ASSERT( strlen( pDataFileName ) < MAX_FILENAME_SIZE );

	m_VersionID = versionID;

	return MEMCARD_RESULT_OK;
}

//=============================================================================
// XboxMemCard::SetFileName()
//=============================================================================

MEMCARD_RESULT XboxMemCard::SetFileName( const char *pDataFileName )
{
	ASSERT( m_pDataFileName != NULL );
	strcpy( m_pDataFileName, pDataFileName );
	ASSERT( strlen( pDataFileName ) < MAX_FILENAME_SIZE );

	return MEMCARD_RESULT_OK;
}

//=============================================================================
// XboxMemCard::CreateSaveBuffer()
//=============================================================================
char *XboxMemCard::CreateLoadSaveBuffer( size_t size )
{
	// round up to memcard block size for xbox
	int	blocks = GetBlocksForFileSize( size + sizeof( ts_XboxMemCardHdr ));
	int	bytes = blocks * MEMCARD_CLUSTER_SIZE;

	char *pData = new char[ bytes ];
	memset( pData, 0, size + sizeof( ts_XboxMemCardHdr ) );
	return( pData + sizeof( ts_XboxMemCardHdr ) );
}

//=============================================================================
// XboxMemCard::DestroySaveBuffer()
//=============================================================================
MEMCARD_RESULT XboxMemCard::DestroyLoadSaveBuffer( char *pLoadSaveBuffer )
{
	ASSERT( pLoadSaveBuffer );

	delete[ ] BasePtr( pLoadSaveBuffer );

	return( MEMCARD_RESULT_OK );
}

//=============================================================================
// XboxMemCard::DestroySaveBuffer()
//=============================================================================

MEMCARD_RESULT XboxMemCard::Detect( u32 Port)
{
	if( Port == 0 )
	{
		// Port 0 is the hard disk
		return MEMCARD_RESULT_OK;
	}

	const DWORD controllerPort = (Port - 1) / 2;
	const DWORD controllerSlot = (Port - 1) % 2;
	
	// XMountMU only allows 3 >= Port >= 1
	ASSERTS( controllerPort < 4, "Tried to detect memory card on invalid controller port" );

	char drive;
	const DWORD err = XMountMU( controllerPort, controllerSlot, &drive );
	switch( err )
	{
	case ERROR_SUCCESS:
		XUnmountMU( controllerPort, controllerSlot );
		return MEMCARD_RESULT_OK;

	case ERROR_DISK_FULL:
		return MEMCARD_RESULT_NOSPACE;

	case ERROR_UNRECOGNIZED_VOLUME:
		return MEMCARD_RESULT_BADCARD;			// ?? may need to handle over cases here

	case ERROR_DEVICE_NOT_CONNECTED:
		return MEMCARD_RESULT_NOCARD;			// ?? may need to handle over cases here

	default:
		// there is a card, but something went wrong trying to use it and 
		// the specific error wasn't handled above
		ASSERTS( 0, "Unhandled Memory Card Error" );
		return MEMCARD_RESULT_FAILED;
	};
}

//=============================================================================
// XboxMemCard::Load()
//=============================================================================

MEMCARD_RESULT XboxMemCard::Load( void *pSaveData, size_t SaveDataSize, int HeaderReadOnly)
{
	return Load( m_DefaultPort, pSaveData, SaveDataSize, HeaderReadOnly);
}

//=============================================================================
// XboxMemCard::Load()
//=============================================================================

MEMCARD_RESULT XboxMemCard::Load( u32 Port, void *pSaveData, size_t SaveDataSize, int HeaderReadOnly)
{

	ts_XboxMemCardHdr		hdr;
	HANDLE	pHandle;
	size_t	countRead = 0;
	BY_HANDLE_FILE_INFORMATION Info;
	int	ActualPort = 0;
	char	drive;
	bool	unmount = false;
	char	Path[64];

	if( m_Port[m_DefaultPort].m_CardStatus != MEMCARD_RESULT_OK )
		return m_Port[m_DefaultPort].m_CardStatus;

	if( strlen( m_pDataFileName) == 0)
		return MEMCARD_RESULT_NODATA;

	// hard drive or memory unit
	if (Port > 0)
	{
		int	err;

		err = XMountMU( (Port/2)-1, Port-1, &drive);
		if (err != ERROR_SUCCESS)
		{
			if (err == ERROR_DISK_FULL)
				return MEMCARD_RESULT_NOSPACE;
		
			if (err == ERROR_DEVICE_NOT_CONNECTED)
				return MEMCARD_RESULT_NOCARD;			// ?? may need to handle over cases here

			if (err == ERROR_UNRECOGNIZED_VOLUME)
				return MEMCARD_RESULT_BADCARD;			// ?? may need to handle over cases here
		}
		else
			unmount = true;

		sprintf(Path, "%s:\\", &drive);
	}
	else
		sprintf(Path,"U:\\");


	// open the file
	// create comment
	DWORD result = XCreateSaveGame( Path, (WCHAR*)CStringHelper::AsciiToUnicode(m_XboxSaveComment), OPEN_EXISTING, 0, m_pDir, MAX_PATH);
	if (result != ERROR_SUCCESS)
	{
		if (unmount)
			XUnmountMU( (Port/2)-1, Port-1);
		return MEMCARD_RESULT_NODATA;
	}

	strcpy(m_XboxSaveName, m_pDir);
	strcat(m_XboxSaveName, m_pDataFileName);
	pHandle = CreateFile(m_XboxSaveName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );

	if (pHandle == INVALID_HANDLE_VALUE)
	{
		if (unmount)
			XUnmountMU( (Port/2)-1, Port-1);

		CloseHandle(pHandle);
		return MEMCARD_RESULT_NODATA;
	}

	// get file attributes
	BOOL res = GetFileInformationByHandle(pHandle, &Info);

#if defined (SIGNED)
	// Read the signature
	XCALCSIG_SIGNATURE FileSig;
	if( !ReadFile( pHandle, &FileSig, sizeof( FileSig ),(LPDWORD)&countRead, 0 ) )
	{
		if (unmount)
			XUnmountMU( (Port/2)-1, Port-1);

		CloseHandle(pHandle);
		return MEMCARD_RESULT_BADDATA;
	}

	HANDLE hSig = XCalculateSignatureBegin( 0 );
	if( hSig == INVALID_HANDLE_VALUE )
	{
		if (unmount)
			XUnmountMU( (Port/2)-1, Port-1);

		CloseHandle(pHandle);
		return MEMCARD_RESULT_BADDATA;
	}
#endif


	// read the file
	BOOL err = ReadFile(pHandle, &hdr, sizeof hdr, (LPDWORD)&countRead, 0);

	if( (countRead != sizeof(ts_XboxMemCardHdr)) || err == 0)
	{
		if (unmount)
			XUnmountMU( (Port/2)-1, Port-1);

		CloseHandle(pHandle);
		return MEMCARD_RESULT_BADDATA;
	}

	if( hdr.versionID != m_VersionID)
	{
		if (unmount)
			XUnmountMU( (Port/2)-1, Port-1);

		CloseHandle(pHandle);
		return MEMCARD_RESULT_BADDATA;
	}

#if defined (SIGNED) // signing
	int	NumChunks;

	if (HeaderReadOnly != HEADER_READ_ONLY)
		NumChunks = (SaveDataSize / SAVE_GAME_CHUNK_SIZE) + 1;
	else
		NumChunks = (SaveDataSize / SAVE_GAME_CHUNK_SIZE);

	int	ActualSize = NumChunks * SAVE_GAME_CHUNK_SIZE;
	int	TotalRead = 0;
	char*	TempBuff = new char[ActualSize];
	memset(TempBuff, 0, ActualSize);
	char*	pD = (char*)TempBuff;

	for (int i=0; i<NumChunks; i++)
	{
		err = ReadFile(pHandle, pD, SAVE_GAME_CHUNK_SIZE, (LPDWORD)&countRead, 0);
		TotalRead += countRead;
		if (err == 0)
		{
			if (unmount)
				XUnmountMU( (Port/2)-1, Port-1);

			CloseHandle(pHandle);
			return MEMCARD_RESULT_BADCARD;
		}

		// skip first chunk so we can just load headers if we need to
		XCalculateSignatureUpdate( hSig, (BYTE*)pD, SAVE_GAME_CHUNK_SIZE );
		pD += SAVE_GAME_CHUNK_SIZE;

		if (m_Update)
			m_Update();

	}

	ActualSize += (sizeof (hdr) + sizeof (FileSig));

	// Determine the final signature and close sig handle
	XCALCSIG_SIGNATURE Sig;
	XCalculateSignatureEnd( hSig, &Sig );

	memcpy(pSaveData, TempBuff, SaveDataSize);
	delete TempBuff;

	// Compare signatures
	if (HeaderReadOnly != HEADER_READ_ONLY)
	{
		if( memcmp( &Sig, &FileSig, sizeof( Sig ) ) != 0 )
		{
			if (unmount)
				XUnmountMU( (Port/2)-1, Port-1);

			CloseHandle(pHandle);
			return MEMCARD_RESULT_BADDATA;				
		}

		// check size of file
		if (Info.nFileSizeLow > (DWORD)ActualSize + 8)
		{
			if (unmount)
				XUnmountMU( (Port/2)-1, Port-1);

			CloseHandle(pHandle);
			return MEMCARD_RESULT_BADDATA;				
		}
	}

	ActualSize -= (sizeof (hdr) + sizeof (FileSig));

// 	if( TotalRead != ActualSize)
//	{
//		if (unmount)
//			XUnmountMU( (Port/2)-1, Port-1);
//
//		CloseHandle(pHandle);
//			return MEMCARD_RESULT_BADDATA;
//	}

#else
	memcpy( BasePtr( ( char * )pSaveData ), &hdr, sizeof( ts_XboxMemCardHdr ) );
	err = ReadFile(pHandle, pSaveData, SaveDataSize, (LPDWORD)&countRead, 0);
	CloseHandle(pHandle);

	if (unmount)
		XUnmountMU( (Port/2)-1, Port-1);

	if( countRead != SaveDataSize)
		return MEMCARD_RESULT_BADDATA;
#endif

	if (unmount)
		XUnmountMU( (Port/2)-1, Port-1);

	CloseHandle(pHandle);

	return MEMCARD_RESULT_OK;
}


//=============================================================================
// XboxMemCard::Save()
//=============================================================================

MEMCARD_RESULT XboxMemCard::Delete( u32 Port)
{
	strcpy(m_XboxSaveName, m_pDir);
	strcat(m_XboxSaveName, m_pDataFileName);
	BOOL err = DeleteFile(m_XboxSaveName);

	if(err)
		return( MEMCARD_RESULT_OK);
	else
		return( MEMCARD_RESULT_NODATA);
}

MEMCARD_RESULT XboxMemCard::DeleteAll( u32 Port, char* TitleID)
{
	return (MEMCARD_RESULT_OK);
}


MEMCARD_RESULT XboxMemCard::DeleteContainer( u32 Port)
{
	char	Drive[5];
	DWORD	err = 0;

	// get drive letter
	if (Port == 0)
		sprintf(Drive, "U:\\");
	else
	{
		char	Letter[3];
		Letter[0] = (char)(Port + 69);
		Letter[1] = 0;
		sprintf(Drive, "%s:\\", Letter);
	}

	XDeleteSaveGame(Drive, (WCHAR*)CStringHelper::AsciiToUnicode(m_XboxSaveComment));

	if(err == ERROR_SUCCESS)
		return( MEMCARD_RESULT_OK);
	else
		return( MEMCARD_RESULT_NODATA);
}

MEMCARD_RESULT XboxMemCard::Delete( void)
{
	return( Delete( m_DefaultPort));
}

//=============================================================================
// XboxMemCard::GetFileSize
//=============================================================================
int	XboxMemCard::FileSize(HANDLE Handle)
{
	LPDWORD uFileSize = 0;

	GetFileSize(Handle, uFileSize);
	return ((int)uFileSize);
}

//=============================================================================
// XboxMemCard::Exists??
//=============================================================================
MEMCARD_RESULT XboxMemCard::Exists(u32 Port)
{
	HANDLE					pHandle;

	if(m_Port[m_DefaultPort].m_CardStatus != MEMCARD_RESULT_OK &&
		m_Port[m_DefaultPort].m_CardStatus != MEMCARD_RESULT_NODATA )
	{
		return m_Port[m_DefaultPort].m_CardStatus;
	}

	DWORD result = XCreateSaveGame( "U:\\", (WCHAR*)CStringHelper::AsciiToUnicode(m_XboxSaveComment), OPEN_EXISTING, 0, m_pDir, MAX_PATH);

	if (result != ERROR_SUCCESS)
		return MEMCARD_RESULT_NODATA;		// not sure if this is right yet. (PAB)

	// create a file name try to open file
	strcpy(m_XboxSaveName, m_pDir);
	strcat(m_XboxSaveName, m_pDataFileName);
	pHandle = CreateFile( m_XboxSaveName, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	CloseHandle(pHandle);

	if (pHandle == INVALID_HANDLE_VALUE)
		return (MEMCARD_RESULT_NODATA);
	else
		return (MEMCARD_RESULT_OK);
}

//=============================================================================
// XboxMemCard::Save()
//=============================================================================

MEMCARD_RESULT XboxMemCard::Save(void *pSaveData, size_t SaveDataSize)
{
	return Save( m_DefaultPort, pSaveData, SaveDataSize);
}

//=============================================================================
// XboxMemCard::Save()
//=============================================================================

MEMCARD_RESULT XboxMemCard::Save(u32 Port,  void *pSaveData, size_t SaveDataSize)
{
	ts_XboxMemCardHdr		hdr;
	HANDLE					pHandle;
	BOOL						err;
	char						Path[64];
	bool						unmount = false;

#if defined (SIGNED)
	XCALCSIG_SIGNATURE Sig = { 0 };
#endif

	size_t	countWritten = 0;

	if(m_Port[m_DefaultPort].m_CardStatus != MEMCARD_RESULT_OK &&
		m_Port[m_DefaultPort].m_CardStatus != MEMCARD_RESULT_NODATA )
	{
		return m_Port[m_DefaultPort].m_CardStatus;
	}

	int	pSize = 0;
	int	ActualPort = 0;
	char	drive;
	
	// hard drive or memory unit
	if (Port > 0)
	{
		int	err;

		err = XMountMU( (Port/2)-1, Port-1, &drive);

		if (err != ERROR_SUCCESS)
		{
			if (err == ERROR_DISK_FULL)
				return MEMCARD_RESULT_NOSPACE;
		
			if (err == ERROR_DEVICE_NOT_CONNECTED)
				return MEMCARD_RESULT_NOCARD;			// ?? may need to handle over cases here

			if (err == ERROR_UNRECOGNIZED_VOLUME)
				return MEMCARD_RESULT_BADCARD;			// ?? may need to handle over cases here
		}
		else
			unmount = true;

		sprintf(Path, "%s:\\", &drive);
	}
	else
		sprintf(Path,"U:\\");
		
	// create file container
	// we may need to check if it exists already.. if it does, dont recreate?
	DWORD result = XCreateSaveGame( Path, (WCHAR*)CStringHelper::AsciiToUnicode(m_XboxSaveComment), OPEN_ALWAYS, 0, m_pDir, MAX_PATH);

	if (result != ERROR_SUCCESS)
	{
		if (unmount)
			XUnmountMU((Port/2)-1, Port-1);

		if (result == ERROR_CANNOT_MAKE)
			return MEMCARD_RESULT_NOFILES;		
		else
			return MEMCARD_RESULT_NODATA;		
	}

	// create a save game file
	strcpy(m_XboxSaveName, m_pDir);
	strcat(m_XboxSaveName, m_pDataFileName);
	pHandle = CreateFile( m_XboxSaveName, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (pHandle == INVALID_HANDLE_VALUE)
	{
		if (unmount)
			XUnmountMU((Port/2)-1, Port-1);
		CloseHandle(pHandle);
		return MEMCARD_RESULT_NODATA;
	}

#if defined (SIGNED)
	// save out zeroes in the sig portion
	err = WriteFile(pHandle, &Sig, sizeof (Sig), (LPDWORD)&countWritten, 0);

	if (err == 0)
	{
		if (unmount)
			XUnmountMU((Port/2)-1, Port-1);

		CloseHandle(pHandle);
		return MEMCARD_RESULT_BADCARD;
	}
#endif
	
	// write out header
	hdr.versionID = m_VersionID;
	err = WriteFile(pHandle, &hdr, sizeof (hdr), (LPDWORD)&countWritten, 0);

	if( (countWritten != sizeof hdr) || err == 0)
	{
		if (unmount)
			XUnmountMU((Port/2)-1, Port-1);

		CloseHandle(pHandle);
		return MEMCARD_RESULT_BADCARD;
	}

#if defined (SIGNED)
	// write out save data now
	// Start the signature hash
	HANDLE hSig = XCalculateSignatureBegin( 0 );
	if( hSig == INVALID_HANDLE_VALUE )
	{
		if (unmount)
			XUnmountMU((Port/2)-1, Port-1);

		CloseHandle(pHandle);
		return MEMCARD_RESULT_BADCARD;
	}
	
	int	NumChunks = (SaveDataSize / SAVE_GAME_CHUNK_SIZE) + 1;
	int	ActualSize = NumChunks * SAVE_GAME_CHUNK_SIZE;
	int	TotalWritten = 0;

	char*	TempBuff = new char[ActualSize];
	memset(TempBuff, 0, ActualSize);
	memcpy(TempBuff, pSaveData, ActualSize);
	char*	pD = TempBuff;

	for (int i=0; i<NumChunks; i++)
	{
		err = WriteFile(pHandle, pD, SAVE_GAME_CHUNK_SIZE, (LPDWORD)&countWritten, 0);
		TotalWritten += countWritten;
		if (err == 0)
		{
			if (unmount)
				XUnmountMU((Port/2)-1, Port-1);

			CloseHandle(pHandle);
			delete TempBuff;
			return MEMCARD_RESULT_BADCARD;
		}

		XCalculateSignatureUpdate( hSig, (BYTE*)pD, SAVE_GAME_CHUNK_SIZE );

		pD += SAVE_GAME_CHUNK_SIZE;

		if (m_Update)
			m_Update();

	}

	delete TempBuff;

	// Determine the final signature and close sig handle
	XCalculateSignatureEnd( hSig, &Sig );

	// Write the signature at the beginning of the file
	SetFilePointer( pHandle, 0, NULL, FILE_BEGIN );
	WriteFile( pHandle, &Sig, sizeof( Sig ),(LPDWORD)&countWritten, 0 );
	CloseHandle(pHandle);

	if (unmount)
		XUnmountMU((Port/2)-1, Port-1);

	if( TotalWritten != ActualSize)
		return MEMCARD_RESULT_BADCARD;
#else
	err = WriteFile(pHandle, pSaveData, SaveDataSize, (LPDWORD)&countWritten, 0);
	CloseHandle(pHandle);

	if (unmount)
		XUnmountMU((Port/2)-1, Port-1);

	if( countWritten != SaveDataSize)
		return MEMCARD_RESULT_BADCARD;
#endif

	// copy over icon file
	char	iconname[64];
	strcpy(iconname, m_pDir);
	strcat(iconname, "saveimage.xbx");
	err = CopyFile("d:\\data_xbox\\saveimage.xbx", iconname, FALSE);

	// no space..?
	if (err == 0)
		return MEMCARD_RESULT_NODATA;		

	return MEMCARD_RESULT_OK;
}

MEMCARD_RESULT XboxMemCard::GetCardSize( u32 Port, u32 &cardBlocks)
{
	return (MEMCARD_RESULT_BADPARAM);		// pab todo	
}

MEMCARD_RESULT XboxMemCard::GetCardFreeSpace( u32 Port, u32 &freeFiles, u32 &freeBlocks)
{
	ULARGE_INTEGER ulFreeAvail;

	BOOL err = GetDiskFreeSpaceEx( "U:\\", &ulFreeAvail, NULL, NULL );

	freeBlocks = BytesToBlocks((u32)ulFreeAvail.QuadPart);

	if (err == 0)
		return (MEMCARD_RESULT_BADPARAM);		// pab todo	
	else
		return MEMCARD_RESULT_OK;
}

MEMCARD_RESULT XboxMemCard::UpdateFileComment( const char *pComment)
{
	strcpy(m_XboxSaveComment, pComment);
	return MEMCARD_RESULT_OK;
}

MEMCARD_RESULT XboxMemCard::UpdateFileComment( u32 Port, const char *pComment)
{
	strcpy(m_XboxSaveComment, pComment);
	return MEMCARD_RESULT_OK;
}

u32	XboxMemCard::BytesToBlocks(u32 Bytes)
{
	DWORD	ClusterSize;
	int	Blocks;

	ClusterSize = XGetDiskClusterSize("U:\\");

	Blocks = (Bytes + (ClusterSize - 1)) / ClusterSize;

	return (Blocks);
}


//=============================================================================
// XboxMemCard::DefaultPort()
//=============================================================================

MEMCARD_RESULT XboxMemCard::DefaultPort( u32 Port )
{
	if( Port != 0 && Port != 1 )
		return MEMCARD_RESULT_BADPARAM;

	m_DefaultPort = Port;

	return MEMCARD_RESULT_OK;
}

//=============================================================================
// XboxMemCard::CheckSpace()
//=============================================================================

MEMCARD_RESULT XboxMemCard::CheckSpace( u32 Port, size_t SaveDataSize, u32 &BlocksNeeded)
{
	ULARGE_INTEGER ulFreeAvail;
	ulFreeAvail.QuadPart = 0;

	BlocksNeeded = BytesToBlocks((u32)ulFreeAvail.QuadPart);

	if( Port != 0 && Port != 1 )
		return MEMCARD_RESULT_BADPARAM;

	// check space
	if( !GetDiskFreeSpaceEx( "U:\\", &ulFreeAvail, NULL, NULL ) )
		return MEMCARD_RESULT_BADPARAM;

	int SaveBlocks = BytesToBlocks(SaveDataSize) + 3 + 1; // 3 is for metadata/folder etc +1 hack from MS to take care of random bug
	int FreeBlocks = BytesToBlocks((u32)ulFreeAvail.QuadPart);
	if (FreeBlocks > 50000)
		FreeBlocks = 50000;

	if( SaveBlocks > FreeBlocks )
		return MEMCARD_RESULT_NOSPACE;

	return MEMCARD_RESULT_OK;
}

#if 0
int XboxMemCard::GetDirectoryAllCards(void)
{
	int	NumFiles = 0;

	// enumerate the harddrive
	NumFiles = EnumerateSaveGames("U:\\", 0);

}
#endif

//=============================================================================
// XboxMemCard::CheckSpace()
//=============================================================================

MEMCARD_RESULT XboxMemCard::CheckSpace( size_t SaveDataSize, u32 &BlocksNeeded )
{
	return CheckSpace( m_DefaultPort, SaveDataSize, BlocksNeeded );
}

//=============================================================================
// XboxMemCard::CheckStatus()
//=============================================================================

MEMCARD_RESULT XboxMemCard::CheckStatus( u32 Port )
{
	return m_Port[Port].m_CardStatus;
}

//=============================================================================
// XboxMemCard::CheckStatus()
//=============================================================================

MEMCARD_RESULT XboxMemCard::CheckStatus( void )
{
	return CheckStatus( m_DefaultPort );
}

//=============================================================================
// XboxMemCard::Format()
//=============================================================================

MEMCARD_RESULT XboxMemCard::Format( u32 Port )
{
	return m_Port[Port].m_CardStatus;
}

//=============================================================================
// XboxMemCard::Format()
//=============================================================================

MEMCARD_RESULT XboxMemCard::Format( void )
{
	return Format( m_DefaultPort );
}

//=============================================================================
// XboxMemCard::RemoveCard()
//=============================================================================

void XboxMemCard::RemoveCard( u32 Port )
{
	return;
}

//=============================================================================
// XboxMemCard::InsertCard()
//=============================================================================

void XboxMemCard::InsertCard( u32 Port, MEMCARD_RESULT Result )
{

	return;
}

