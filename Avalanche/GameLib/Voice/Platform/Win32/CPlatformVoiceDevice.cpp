// ---------------------------------------------------------------------------
// File			-> CPlatformVoiceDevice.cpp
// Author		-> Dwain Skinner
// Created		-> December 15, 2003
// Description	-> Win32 Platform dependent voice device class implementation
// ---------------------------------------------------------------------------

// Precompiled header	
#include "Voice/VoicePCH.h"

// --- Local includes
#include "CPlatformVoiceDevice.h"

#include "dxutil.h"
#include "dsutil.h"

CWaveFile*                 g_pWaveFile;
static u32						recordTime = 30*10;

#define WRITE_FILE false

// ---------------------------------------------------------------------------
CPlatformVoiceDevice::CPlatformVoiceDevice( ) : 
	m_playDeviceID( 0 ), 
	m_recordDeviceID( 0 ),
	m_pDirectSoundPlay( NULL ), 
	m_pDirectSoundRecord( NULL ),
	m_pPrimaryBuffer( NULL ),
	m_nextCaptureOffset( 0 ),
	m_writePos( 0 ),
	m_lastWritePos( 0 ),
	m_lastPlayBackSize( 0 )
{	
	setStatus( ( eDeviceStatus )( DEVICE_NODEVICE | DEVICE_DISABLED ) );

	// --- setup our play and record buffer descriptions
	memset( &m_playBufferDesc, 0, sizeof( m_playBufferDesc ) );
	memset( &m_recordBufferDesc, 0, sizeof( m_playBufferDesc ) );
	memset( &m_playWaveFormat, 0, sizeof( m_playWaveFormat ) );
	memset( &m_recordWaveFormat, 0, sizeof( m_recordWaveFormat ) );
	m_playWaveFormat.wFormatTag		= WAVE_FORMAT_PCM;
	m_playWaveFormat.nChannels			= 1;
	m_playWaveFormat.nSamplesPerSec	= 8000;
	m_playWaveFormat.wBitsPerSample	= 16;
	m_playWaveFormat.nBlockAlign		= ( m_playWaveFormat.nChannels * ( m_playWaveFormat.wBitsPerSample / 8 ) );
	m_playWaveFormat.nAvgBytesPerSec	= m_playWaveFormat.nBlockAlign * m_playWaveFormat.nSamplesPerSec;
	m_playBufferDesc.dwSize				= sizeof( m_playBufferDesc );
	m_playBufferDesc.dwFlags			= DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 ;
	m_playBufferDesc.dwBufferBytes	= PLAY_BUFFER_SIZE_SECONDS * m_playWaveFormat.nAvgBytesPerSec;
	m_playBufferDesc.lpwfxFormat		= &m_playWaveFormat;

	m_recordWaveFormat.wFormatTag			= WAVE_FORMAT_PCM;
	m_recordWaveFormat.nChannels			= 1;
	m_recordWaveFormat.nSamplesPerSec	= 8000;
	m_recordWaveFormat.wBitsPerSample	= 16;
	m_recordWaveFormat.nBlockAlign		= ( m_recordWaveFormat.nChannels * ( m_recordWaveFormat.wBitsPerSample / 8 ) );
	m_recordWaveFormat.nAvgBytesPerSec	= m_recordWaveFormat.nBlockAlign * m_recordWaveFormat.nSamplesPerSec;
	m_recordBufferDesc.dwSize				= sizeof( m_recordBufferDesc );
	m_recordBufferDesc.dwFlags				= 0;
	m_recordBufferDesc.dwBufferBytes		= RECORD_BUFFER_SIZE_SECONDS * m_recordWaveFormat.nAvgBytesPerSec;
	m_recordBufferDesc.lpwfxFormat		= &m_recordWaveFormat;

	// --- lets create the device and initialize it
	if( create( ) )
	{

		setStatus( ( eDeviceStatus )DEVICE_CREATED );

#if WRITE_FILE
		HRESULT hr;

		g_pWaveFile = new CWaveFile;
		ASSERTS( g_pWaveFile, "unable to create wave file object" );

		// Get the format of the capture buffer in g_wfxCaptureWaveFormat
		WAVEFORMATEX wfxCaptureWaveFormat;
		ZeroMemory( &wfxCaptureWaveFormat, sizeof(WAVEFORMATEX) );
		m_pRecordBuffer->GetFormat( &wfxCaptureWaveFormat, sizeof(WAVEFORMATEX), NULL );

		// open the wave file
		if( FAILED( hr = g_pWaveFile->Open( "c:\\testwave.wav", &wfxCaptureWaveFormat, WAVEFILE_WRITE ) ) )
		{
			ASSERTS( false, "could not open the wave file" );
		}
#endif
	}
}

// ---------------------------------------------------------------------------
CPlatformVoiceDevice::~CPlatformVoiceDevice( )
{
}

// ---------------------------------------------------------------------------
bool CPlatformVoiceDevice::create( void )
{
	// --- lets find our headset to play sound on
	for( int i = 0; i < howManyPlay( ); i++ )
	{
		CVoiceDeviceEnumerator::SOUNDDRIVER desc;
		getPlayDriver( i, &desc );
		for( u32 i = 0; i < strlen( desc.description ); i++ )
		{
			desc.description[ i ] = toupper( desc.description[ i ] );
		}

		// --- look for a headset
		if( strstr( desc.description, "HEADSET" ) )
		{
			// --- Do the create
			m_playDeviceID = desc.driverID;
			HRESULT ret = DirectSoundCreate( m_playDeviceID, &m_pDirectSoundPlay, NULL );
			if( ret != DS_OK ) return( false );
			
			// --- get caps
			if( m_pDirectSoundPlay )
			{
				DSCAPS caps;
				memset( &caps, 0 , sizeof( DSCAPS ) );
				caps.dwSize = sizeof( DSCAPS );
				ret = m_pDirectSoundPlay->GetCaps( &caps );
				if( ret != DS_OK ) return( false );

				// --- create play buffer
				ret = m_pDirectSoundPlay->CreateSoundBuffer( &m_playBufferDesc, &m_pPlayBuffer, NULL );
				ASSERTS( ret == DS_OK, "unable to create direct sound play object" );
				if( ret != DS_OK ) return( false );

				WindowsApp *winApp = WindowsApp::IAm();
//				m_pDirectSoundPlay->SetCooperativeLevel( winApp->GethWnd( ) , DSSCL_NORMAL );
				m_pDirectSoundPlay->SetCooperativeLevel( winApp->GethWnd( ) , DSSCL_PRIORITY );


				DSBUFFERDESC        dsbdesc;
				// Obtain primary buffer 
				ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
				dsbdesc.dwSize  = sizeof(DSBUFFERDESC);
				dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

				ret = m_pDirectSoundPlay->CreateSoundBuffer( &dsbdesc, &m_pPrimaryBuffer, NULL );
				m_pPrimaryBuffer->SetFormat( &m_playWaveFormat );

				break;
			}
		}
	}

	// --- lets find our headset to record sound on
	for( int i = 0; i < howManyRecord( ); i++ )
	{
		CVoiceDeviceEnumerator::SOUNDDRIVER desc;
		getRecordDriver( i, &desc );
		for( u32 i = 0; i < strlen( desc.description ); i++ )
		{
			desc.description[ i ] = toupper( desc.description[ i ] );
		}

		// --- look for a headset
		if( strstr( desc.description, "HEADSET" ) )
		{
			// --- Do the create
			m_recordDeviceID = desc.driverID;
			HRESULT ret = DirectSoundCaptureCreate( m_recordDeviceID, &m_pDirectSoundRecord, NULL );
			if( ret != DS_OK ) return( false );

			// --- get caps
			if( m_pDirectSoundRecord )
			{
				DSCCAPS caps;
				memset( &caps, 0 , sizeof( DSCCAPS ) );
				caps.dwSize = sizeof( DSCCAPS );
				ret = m_pDirectSoundRecord->GetCaps( &caps );
				if( ret != DS_OK ) return( false );

				// --- create record buffer
				ret = m_pDirectSoundRecord->CreateCaptureBuffer( &m_recordBufferDesc, &m_pRecordBuffer, NULL );
				if( ret != DS_OK ) return( false );
				break;
			}
		}
	}

	return( true );
}

// ---------------------------------------------------------------------------
bool CPlatformVoiceDevice::destroy( void )
{
	if( m_pDirectSoundPlay )
	{
		m_pDirectSoundPlay->Release( );
		m_pDirectSoundPlay = NULL;
		if( m_pPlayBuffer )
		{
			m_pPlayBuffer->Release( );
			m_pPlayBuffer = NULL;
		}
	}

	if( m_pDirectSoundRecord )
	{
		m_pDirectSoundRecord->Release( );
		m_pDirectSoundRecord = NULL;
		if( m_pRecordBuffer )
		{
			m_pRecordBuffer->Release( );
			m_pRecordBuffer = NULL;
		}
	}

#if WRITE_FILE
	if( g_pWaveFile )
	{
		g_pWaveFile->Close( );
		delete g_pWaveFile;
		g_pWaveFile = NULL;
	}
#endif
	return( true );
}

// ---------------------------------------------------------------------------
void CPlatformVoiceDevice::update( )
{

}

// ---------------------------------------------------------------------------
u32 CPlatformVoiceDevice::record( u8 *pDst, u32 size )
{
	HRESULT	ret;
	VOID*		pCaptureData    = NULL;
	DWORD		captureLength;
	VOID*		pCaptureData2   = NULL;
	DWORD		captureLength2;
	DWORD		readPos;
	DWORD		capturePos;
	LONG		lLockSize;
	u32		sizeRead = 0;

#if WRITE_FILE
	UINT dataWrote;

	if( recordTime == 0 ) return( 0 );
#endif

	// --- make sure direct sound object and capture buffer is valid
	if( m_pDirectSoundRecord && m_pRecordBuffer )
	{
		// --- start recording?
		if( 0 ) // !isRecording( ) )
		{
			addStatus( DEVICE_RECORDING );
			ret = m_pRecordBuffer->Stop( );
			ASSERTS( ret == DS_OK, "unable to stop recording on headset" );
			ret = m_pRecordBuffer->Start( DSCBSTART_LOOPING );
			ASSERTS( ret == DS_OK, "unable to start recording on headset" );
			m_nextCaptureOffset = 0;
		}
		else
		{
#if WRITE_FILE
			recordTime--;
			if( recordTime == 0 ) 
			{
				removeStatus( DEVICE_RECORDING );
				g_pWaveFile->Close( );
				return( 0 );
			}
#endif
			// --- get current capture and read positions
			if( FAILED( ret = m_pRecordBuffer->GetCurrentPosition( &capturePos, &readPos ) ) )
			{
				ASSERTS( false, "GetCurrentPosition( ) - failed" );
				return( 0 );
			}

			// --- determine how much of the buffer we must lock
			lLockSize = readPos - m_nextCaptureOffset;
			if( lLockSize < 0 )
				lLockSize += m_recordBufferDesc.dwBufferBytes;

			// --- is any data ready?
			if( lLockSize == 0 )
				return( 0 );

			if( lLockSize > ( LONG )size )
				lLockSize = size;

			// --- Lock the capture buffer down
			if( FAILED( ret = m_pRecordBuffer->Lock( m_nextCaptureOffset, lLockSize, 
				&pCaptureData, &captureLength, 
				&pCaptureData2, &captureLength2, 0L ) ) )
			{
				ASSERTS( false, "Lock( ) - failed" );
				return( 0 );
			}

			// --- this is a problem try restarting the capture next frame
			if( lLockSize >= ( 1024 * 16 ) )
			{
				m_pRecordBuffer->Unlock( pCaptureData,  captureLength, pCaptureData2, captureLength2 );
				removeStatus( DEVICE_RECORDING );
				return 0;
			}

			// --- grap the data
			ASSERTS( lLockSize < ( 1024 * 16 ), "record buffer overflow" );
			memcpy( pDst, pCaptureData, captureLength );
			sizeRead = captureLength;
			
			// Write the data into the wav file
#if WRITE_FILE
			if( FAILED( ret = g_pWaveFile->Write( captureLength, 
					(BYTE*)pCaptureData, 
					&dataWrote ) ) )
			{
				ASSERTS( false, "write failed" );
			}
			ASSERTS( captureLength == dataWrote, "mismatch" );
#endif

			// Move the capture offset along
			m_nextCaptureOffset += captureLength; 
			m_nextCaptureOffset %= m_recordBufferDesc.dwBufferBytes; // Circular buffer

			if( pCaptureData2 != NULL )
			{
#if WRITE_FILE
				// Write the data into the wav file
					if( FAILED( ret = g_pWaveFile->Write( captureLength2, 
						(BYTE*)pCaptureData2, 
						&dataWrote ) ) )
					{
						ASSERTS( false, "write failed" );
					}

					ASSERTS( captureLength2 == dataWrote, "mismatch" );
#endif
				memcpy( pDst + sizeRead, pCaptureData2, captureLength2 );
				sizeRead += captureLength2;

				// Move the capture offset along
				m_nextCaptureOffset += captureLength2; 
				m_nextCaptureOffset %= m_recordBufferDesc.dwBufferBytes; // Circular buffer
			}

			m_pRecordBuffer->Unlock( pCaptureData,  captureLength, pCaptureData2, captureLength2 );
		}
	}
	return( sizeRead );
}

// ---------------------------------------------------------------------------
bool CPlatformVoiceDevice::play( u8 *pSrc, u32 size )
{
	HRESULT	ret;
	VOID*		pWriteData    = NULL;
	DWORD		writeLength;
	VOID*		pWriteData2   = NULL;
	DWORD		writeLength2;
	DWORD		writePos;
	u32			sizeRead = 0;
	DWORD		status;
	DWORD		lockLength;
	DWORD		lockLength2;


	// --- make sure we have data to write
	if( size == 0 ) return( true );

	// --- make sure direct sound object and play buffer is valid
	if( m_pDirectSoundPlay && m_pPlayBuffer )
	{
		if( FAILED( ret = m_pPlayBuffer->GetStatus( &status ) ) )
		{
			ASSERTS( false, "unable to get status" );
			return false;
		}
		// --- restore buffer if needed
		if( status == DSBSTATUS_BUFFERLOST )
		{
			if( FAILED( ret = m_pPlayBuffer->Restore( ) ) ) 
			{
				ASSERTS( false, "could not restore" );
				return false;
			}
		}


		// --- our own cursor
		writePos = m_writePos;

		// --- Lock the buffer down
		if( FAILED( ret = m_pPlayBuffer->Lock( writePos, size, 
			&pWriteData, &lockLength, 
			&pWriteData2, &lockLength2, 0L ) ) )
//			&pWriteData2, &lockLength2, DSBLOCK_ENTIREBUFFER ) ) )
		{
			ASSERTS( false, "Lock( ) - failed" );
			return false;
		}

		writeLength = lockLength;
		writeLength2 = lockLength2;
		if( writeLength > size )
			writeLength = size;

		// --- write the data into the buffer
		memcpy( pWriteData, pSrc, writeLength );
		if( pWriteData2 != NULL )
		{
			writeLength2 = size - writeLength;
			memcpy( pWriteData2, pSrc + writeLength, writeLength2 );
		}
		
		// --- unlock the buffer
		if( FAILED( ret = m_pPlayBuffer->Unlock( pWriteData,  lockLength, pWriteData2, lockLength2 ) ) )
		{
			ASSERTS( false, "UnLock( ) - failed" );
			return false;
		}

		// --- update our write position
		m_writePos += size;
		if( m_writePos >= m_playBufferDesc.dwBufferBytes )
			m_writePos -= m_playBufferDesc.dwBufferBytes;

	}
	return true;
}

// ---------------------------------------------------------------------------
u8 *CPlatformVoiceDevice::createBuffer( u32 size )
{

	u8 *pBuf = new u8[ size ];

	ASSERTS( pBuf, "could not create device buffer" );
	dbgPrint( "Creating device buffer of %d bytes...\n", size );
	return( pBuf );
}

// ---------------------------------------------------------------------------
CVoiceDeviceEnumerator::CVoiceDeviceEnumerator()
{
	m_playList = NULL;
	m_playCount = 0;
	m_recordList = NULL;
	m_recordCount = 0;
}

// ---------------------------------------------------------------------------
CVoiceDeviceEnumerator::~CVoiceDeviceEnumerator()
{
	free( m_playList );
	free( m_recordList );
}

// ---------------------------------------------------------------------------
int CVoiceDeviceEnumerator::howManyPlay( )
{
	// --- Have we made the list yet?
	if( !enumerated( ) )
		enumerate();

	// --- How many are there?
	return m_playCount;
}

// ---------------------------------------------------------------------------
int CVoiceDeviceEnumerator::howManyRecord( )
{
	// --- Have we made the list yet?
	if( !enumerated( ) )
		enumerate();

	// --- How many are there?
	return m_recordCount;
}

// ---------------------------------------------------------------------------
bool CVoiceDeviceEnumerator::getPlayDriver( int index, SOUNDDRIVER *description )
{
	// --- Have we made the list yet?
	if( !enumerated( ) )
		enumerate();

	// --- In bounds?
	if( index < 0 || index >= m_playCount )
		return false;

	// --- Give it to them
	*description = m_playList[ index ];
	return true;
}

// ---------------------------------------------------------------------------
bool CVoiceDeviceEnumerator::getRecordDriver( int index, SOUNDDRIVER *description )
{
	// --- Have we made the list yet?
	if( !enumerated( ) )
		enumerate();

	// --- In bounds?
	if( index < 0 || index >= m_recordCount )
		return false;

	// --- Give it to them
	*description = m_recordList[ index ];
	return true;
}

// ---------------------------------------------------------------------------
bool CVoiceDeviceEnumerator::enumerate( )
{
	HRESULT playRet = 0;
	HRESULT recordRet = 0;

	// ---Enumerate play devices
	if( m_playList == NULL )
	{
		playRet = DirectSoundEnumerate( ( LPDSENUMCALLBACK )enumeratePlayCallback, this );
	}

	// --- enumerate record devices
	if( m_recordList == NULL )
	{
		recordRet = DirectSoundCaptureEnumerate( ( LPDSENUMCALLBACK )enumerateCaptureCallback, this );
	}
	return ( ( playRet == DS_OK ) && ( recordRet == DS_OK ) );
}

// ---------------------------------------------------------------------------
BOOL CALLBACK CVoiceDeviceEnumerator::enumeratePlayCallback( LPGUID guid, LPCSTR description,	LPCSTR module,	LPVOID context )
{
	// --- get a pointer to me
	CVoiceDeviceEnumerator *me = ( CVoiceDeviceEnumerator * )context;

	// --- Yes, this is inefficient, but I don't care
	if( !me->m_playList )
		me->m_playList = ( SOUNDDRIVER * )malloc( sizeof( SOUNDDRIVER ) );
	else
	{
		SOUNDDRIVER *temp = (SOUNDDRIVER *)realloc( me->m_playList, ( me->m_playCount + 1 ) * sizeof( SOUNDDRIVER ) );
		if( !temp )
			return FALSE;
		me->m_playList = temp;
	}

	// --- Store it off
	SOUNDDRIVER *dest = &me->m_playList[ me->m_playCount ];
	dest->driverID = guid;
	strncpy( dest->description, description, sizeof( dest->description ) - 1 );
	dest->description[ sizeof(dest->description) - 1] = '\0';
	strncpy( dest->driverName, module, sizeof( dest->driverName ) - 1 );
	dest->driverName[ sizeof( dest->driverName ) - 1 ] = '\0';

	// -- One more
	me->m_playCount++;
	return TRUE;
}

// ---------------------------------------------------------------------------
BOOL CALLBACK CVoiceDeviceEnumerator::enumerateCaptureCallback( LPGUID guid, LPCSTR description,	LPCSTR module,	LPVOID context )
{
	// --- get a pointer to me
	CVoiceDeviceEnumerator *me = ( CVoiceDeviceEnumerator * )context;

	// --- Yes, this is inefficient, but I don't care
	if( !me->m_recordList )
		me->m_recordList = ( SOUNDDRIVER * )malloc( sizeof( SOUNDDRIVER ) );
	else
	{
		SOUNDDRIVER *temp = (SOUNDDRIVER *)realloc( me->m_recordList, ( me->m_recordCount + 1 ) * sizeof( SOUNDDRIVER ) );
		if( !temp )
			return FALSE;
		me->m_recordList = temp;
	}

	// --- Store it off
	SOUNDDRIVER *dest = &me->m_recordList[ me->m_recordCount ];
	dest->driverID = guid;
	strncpy( dest->description, description, sizeof( dest->description ) - 1 );
	dest->description[ sizeof(dest->description) - 1] = '\0';
	strncpy( dest->driverName, module, sizeof( dest->driverName ) - 1 );
	dest->driverName[ sizeof( dest->driverName ) - 1 ] = '\0';

	// -- One more
	me->m_recordCount++;
	return TRUE;
}

// ---------------------------------------------------------------------------
bool CPlatformVoiceDevice::startRecording( )
{
	int ret;

	if( m_pDirectSoundRecord && m_pRecordBuffer )
	{
		addStatus( DEVICE_RECORDING );
		ret = m_pRecordBuffer->Start( DSCBSTART_LOOPING );
		ASSERTS( ret == DS_OK, "unable to start recording on headset" );
		return( true );
	}
	return( false );
}

// ---------------------------------------------------------------------------
bool CPlatformVoiceDevice::stopRecording( )
{
	int ret;
	u32 recordPos;
	u32 readPos;

	if( m_pDirectSoundRecord && m_pRecordBuffer )
	{
		removeStatus( DEVICE_RECORDING );
		ret = m_pRecordBuffer->Stop( );
		ASSERTS( ret == DS_OK, "unable to stop recording on headset" );
		if( FAILED( ret = m_pRecordBuffer->GetCurrentPosition( &recordPos, &readPos ) ) )
		{
			ASSERTS( false, "GetCurrentPosition( ) - failed" );
			return( 0 );
		}
		m_nextCaptureOffset = recordPos;
		return( true );
	}
	return( false );
}

// ---------------------------------------------------------------------------
bool CPlatformVoiceDevice::stopPlaying( )
{
	int ret;
	u32 playPos;
	u32 writePos;

	if( !isPlaying( ) ) return( true );

	if( m_pDirectSoundRecord && m_pPlayBuffer )
	{
		removeStatus( DEVICE_PLAYING );
		if( FAILED( ret = m_pPlayBuffer->Stop( ) ) )
		{
			ASSERTS( false, "Stop( ) - failed" );
			return false;
		}
		if( FAILED( ret = m_pPlayBuffer->GetCurrentPosition( &playPos, &writePos ) ) )
		{
			ASSERTS( false, "GetCurrentPosition( ) - failed" );
			return( 0 );
		}
		m_writePos = playPos;
		return( true );
	}
	return( false );
}

// ---------------------------------------------------------------------------
bool CPlatformVoiceDevice::startPlaying( )
{
	int ret;
	u32 playPos;
	u32 writePos;

	if( isPlaying( ) ) return( true );

	if( m_pDirectSoundRecord && m_pPlayBuffer )
	{
		if( FAILED( ret = m_pPlayBuffer->GetCurrentPosition( &playPos, &writePos ) ) )
		{
			ASSERTS( false, "GetCurrentPosition( ) - failed" );
			return( 0 );
		}
		getPlayBackBytes( );
		addStatus( DEVICE_PLAYING );
		ret = m_pPlayBuffer->Play( 0, 0, DSCBSTART_LOOPING );
		ASSERTS( ret != DSERR_BUFFERLOST, "Direct Sound Play - buffer lost" );
		ASSERTS( ret != DSERR_INVALIDCALL, "Direct Sound Play - invalid call" );
		ASSERTS( ret != DSERR_INVALIDPARAM, "Direct Sound Play - invalid param" );
		ASSERTS( ret != DSERR_PRIOLEVELNEEDED, "Direct Sound Play - priority level not set" );
		ASSERTS( ret == DS_OK, "Direct Sound Play unknown error" );
		return( true );
	}
	return( false );
}

// ---------------------------------------------------------------------------
u32 CPlatformVoiceDevice::getPlayBackBytes( )
{
	u32 playPos;
	u32 writePos;
	int size = 0;
	int ret;

	if( m_pDirectSoundPlay && m_pPlayBuffer )
	{
		if( FAILED( ret = m_pPlayBuffer->GetCurrentPosition( &playPos, &writePos ) ) )
		{
			ASSERTS( false, "GetCurrentPosition( ) - failed" );
			return( 0 );
		}
		size = m_writePos - playPos;
		if( size < 0 )
			size += m_playBufferDesc.dwBufferBytes;
	}

	// --- have we stoped putting data into the play buffer?
	if( m_writePos == m_lastWritePos )
	{
		// --- we just wrapped
		if( size > ( int )m_lastPlayBackSize )
		{
			size = 0;
		}
	}

	m_lastWritePos = m_writePos;
	m_lastPlayBackSize = size;
	return( size );
}

// ---------------------------------------------------------------------------
u32 CPlatformVoiceDevice::getRecordedBytes( )
{
	u32 recordPos;
	u32 readPos;
	u32 size = 0;
	int ret;

	if( m_pDirectSoundRecord && m_pRecordBuffer )
	{
		if( FAILED( ret = m_pRecordBuffer->GetCurrentPosition( &recordPos, &readPos ) ) )
		{
			ASSERTS( false, "GetCurrentPosition( ) - failed" );
			return( 0 );
		}
		size = readPos - m_nextCaptureOffset;
		if( size < 0 )
			size += m_recordBufferDesc.dwBufferBytes;
	}
	return( size );
}