// 
// (c) Copyright 1998-2004, Quazal Technologies Inc.
// ALL RIGHTS RESERVED
// 
// Net-Z, Eterna, Rendez-Vous, Duplicated Object and Duplication Space 
// are trademarks of Quazal.
// 
// Use of this software indicates acceptance of the End User licence
// Agreement for this product. A copy of the EULA is available from
// www.quazal.com.
//   

#include "DSoundSink.h"
#include <platform/core/platformimpl.h>
#include <platform/core/objectthread.h>
#include <platform/core/logdevicedebugoutput.h>
#include <basetsd.h>
#include <commdlg.h>
#include <mmreg.h>
#include <dxerr9.h>
#include "Resource.h"

#define NUM_PLAY_NOTIFICATIONS  16

BEGIN_NAMESPACE_QUAZAL; 

class DummyDialog {
public:
	DummyDialog(): m_oDialogThread(_T("DialogThread"))
	{
		m_oDialogThread.Launch(this, &DummyDialog::Create, 0);
		while (s_hWnd == NULL) { Platform::Sleep(100); }
	}

	~DummyDialog() {
		EndDialog(s_hWnd,0);
		m_oDialogThread.Wait();
	}
	void Create(qInt) {
    	qInt iErr = DialogBox( NULL, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DummyDialog::MainDlgProc );
		if (iErr < 2) {
		}
	}

	static INT CALLBACK MainDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		s_hWnd = hDlg;
		return DefWindowProc(hDlg, msg, wParam, lParam);
	}

	HWND GetWnd() { return s_hWnd; }

private:
	static HWND						s_hWnd;
	ObjectThread<DummyDialog,qInt> 	m_oDialogThread;
};

END_NAMESPACE_QUAZAL;


USING_NAMESPACE_QUAZAL;

HWND DummyDialog::s_hWnd = NULL;


DSoundSink::DSoundSink(HWND hWnd, const AudioStreamFormat &oFormat, qUnsignedInt uiNumPackets):
			m_oAudioStreamFormat(oFormat),
			m_uiNumPackets(uiNumPackets)
{
	SYSTEMCHECK(uiNumPackets <= 32);
	m_bOpen = false;
	m_bInitialized = false;
	m_pDSBuffer = NULL;
	m_pDS = NULL;
	if (hWnd == NULL) {
		m_pDialog = new DummyDialog();
		hWnd = m_pDialog->GetWnd();
	} else {
		m_pDialog = NULL;
	}
	
	//
    // Create a static IDirectSound in the CSound class.  
    // Set coop level to DSSCL_PRIORITY, and set primary buffer 
    // format to stereo, 22kHz and 16-bit output.
    if( FAILED( InitializeDirectSound( hWnd, DSSCL_PRIORITY ) ) )
    {
        QTRACE(TRACE_ALWAYS,(_T("Error Initializing DirectSound")));
        return;
    }
    
	if ( FAILED(CreateSoundBuffer())) {
		return;
	}

	QTRACE(TRACE_ALWAYS,(_T("DirectSound Initialization successful")));

	m_bInitialized = true;
}


DSoundSink::~DSoundSink() 
{
	if (m_pDialog) {
		delete m_pDialog;
	}
}


HRESULT DSoundSink::InitializeDirectSound( HWND hWnd, qUnsignedInt dwCoopLevel )
{
    HRESULT             hr;
	 // --- lets find our headset to play sound on

	 m_playDeviceID = NULL;
	 for( int i = 0; i < howManyPlay( ); i++ )
	 {
		 DSoundDeviceEnumerator::SOUNDDRIVER desc;
		 getPlayDriver( i, &desc );
		 for( unsigned int i = 0; i < strlen( desc.description ); i++ )
		 {
			 desc.description[ i ] = toupper( desc.description[ i ] );
		 }

		 // --- look for a headset
		 if( strstr( desc.description, "HEADSET" ) )
		 {
			 // --- Do the create
			 m_playDeviceID = desc.driverID;
			 break;
		 }
	 }

    // Create IDirectSound using the primary sound device
    if( FAILED( hr = DirectSoundCreate8(  m_playDeviceID, &m_pDS, NULL ) ) )
        return DXTRACE_ERR( TEXT("DirectSoundCreate8"), hr );

    // Set DirectSound coop level 
    if( FAILED( hr = m_pDS->SetCooperativeLevel( hWnd, dwCoopLevel ) ) )
        return DXTRACE_ERR( TEXT("SetCooperativeLevel"), hr );   

    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;

    // Get the primary buffer 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;
       
    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
        return DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );

    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) GetFormat().GetNbChannels(); 
    wfx.nSamplesPerSec  = (qUnsignedInt) GetFormat().GetNbSamplesPerSecond(); 
    wfx.wBitsPerSample  = (WORD) GetFormat().GetNbBitsPerSample(); 
    wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = (qUnsignedInt) (wfx.nSamplesPerSec * wfx.nBlockAlign);

    if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
        return DXTRACE_ERR( TEXT("SetFormat"), hr );

   // delete pDSBPrimary; qUnsignedInt dwNumPackets, qUnsignedInt dwPacketSize

    return S_OK;
}


HRESULT DSoundSink::CreateSoundBuffer() 
{
    HRESULT hr;

    if( m_pDS == NULL )
        return DXTRACE_ERR( TEXT("CreateSoundBuffer"), CO_E_NOTINITIALIZED );

    qUnsignedInt               dwDSBufferSize = NULL;

    // Figure out how big the DSound buffer should be 
    dwDSBufferSize = m_uiNumPackets * GetFormat().GetPacketSize();

    // Set up the direct sound buffer.  Request the NOTIFY flag, so
    // that we are notified as the sound buffer plays.  Note, that using this flag
    // may limit the amount of hardware acceleration that can occur. 
    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) GetFormat().GetNbChannels(); 
    wfx.nSamplesPerSec  = (qUnsignedInt) GetFormat().GetNbSamplesPerSecond(); 
    wfx.wBitsPerSample  = (WORD) GetFormat().GetNbBitsPerSample(); 
    wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = (qUnsignedInt) (wfx.nSamplesPerSec * wfx.nBlockAlign);

    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = DSBCAPS_GETCURRENTPOSITION2;
    dsbd.dwBufferBytes   = dwDSBufferSize;
    dsbd.guid3DAlgorithm = GUID_NULL;
    dsbd.lpwfxFormat     = &wfx;

    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &m_pDSBuffer, NULL ) ) )
    {
        // If wave format isn't then it will return 
        // either DSERR_BADFORMAT or E_INVALIDARG
        if( hr == DSERR_BADFORMAT || hr == E_INVALIDARG )
            return DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );

        return DXTRACE_ERR( TEXT("CreateSounStreamingDevice::NotReadydBuffer"), hr );
    }

	return S_OK;
}


qBool DSoundSink::Open()
{
	if (!m_bInitialized) {
		return false;
	}
    m_pDSBuffer->SetCurrentPosition(0);
	m_uiNextPacket = 0;
	m_bOpen = true;
	return true;
}


StreamingDevice::Status DSoundSink::Write(qByte *pSound, qUnsignedInt32 uiBufferSize)
{
	SYSTEMCHECK(m_bOpen);
	if (uiBufferSize == 0) {
		uiBufferSize = GetFormat().GetPacketSize();
	}
	SYSTEMCHECK(uiBufferSize == GetFormat().GetPacketSize());

    HRESULT hr; 
    VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
    VOID*   pDSLockedBuffer2      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize2 = 0;    // Size of the locked DirectSound buffer

	qUnsignedInt uiPacketSize = GetFormat().GetPacketSize();
	BYTE byBitsPerSample = (BYTE)GetFormat().GetNbBitsPerSample();

	qUnsignedInt32 uiNextWriteOffset = (m_uiNextPacket % m_uiNumPackets) * uiPacketSize;

	unsigned long uiCurrent, uiWrite;
	m_pDSBuffer->GetCurrentPosition(&uiCurrent, &uiWrite);

	qUnsignedInt32 uiCurrentPacket = (uiCurrent / uiPacketSize);
	if ((m_uiNextPacket > 0) && 
		(uiCurrentPacket == (m_uiNextPacket % m_uiNumPackets))) {
		QTRACE(TRACE_NEVER,(_T("Overflow")));
		return NotReady;
	} 
	
    // Lock the buffer down
    hr = m_pDSBuffer->Lock( uiNextWriteOffset,  uiPacketSize, 
		                    &pDSLockedBuffer, &dwDSLockedBufferSize, 
   			                &pDSLockedBuffer2, &dwDSLockedBufferSize2, 0 );
	SYSTEMCHECK(dwDSLockedBufferSize == uiPacketSize);
	if (DSERR_BUFFERLOST == hr) 
	{ 
		m_pDSBuffer->Restore(); 
		hr = m_pDSBuffer->Lock( uiNextWriteOffset,  uiPacketSize, 
		                    &pDSLockedBuffer, &dwDSLockedBufferSize, 
   			                &pDSLockedBuffer2, &dwDSLockedBufferSize2, 0 );
		QTRACE(TRACE_ALWAYS,(_T("Buffer Lost")));
	} 
	if (SUCCEEDED(hr)) {
		QTRACE(TRACE_NEVER,(_T("Writing %d(%d), Current %d, Write %d"),
					uiNextWriteOffset, m_uiNextPacket, uiCurrent, uiWrite));
		// Wav is blank, so just fill with silence
		if (pSound == NULL) {
			// This is the Sound Of Silence -- Simon and Garfunkel
			FillMemory( (BYTE*) pDSLockedBuffer, dwDSLockedBufferSize, byBitsPerSample == 8 ? 128 : 0 ) ;
		} else {
			CopyMemory( (BYTE*) pDSLockedBuffer, pSound, dwDSLockedBufferSize); 
		}
		if (m_uiNextPacket == 2) {
			if (FAILED(m_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING))) {
				QTRACE(TRACE_ALWAYS,(_T("Failed to play sound")));
				return Error;
			} else {
				QTRACE(TRACE_NEVER,(_T("Starting Playback")));
			}
		}
		m_uiNextPacket++;
		m_pDSBuffer->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, pDSLockedBuffer2, dwDSLockedBufferSize2 );
    } else {
		QTRACE(TRACE_NEVER,(_T("Cannot lock %d-%d, Positions are %d,%d"),
						uiNextWriteOffset, 
						(uiNextWriteOffset + uiPacketSize), uiCurrent,uiWrite));
		return NotReady;
	}

	return Success;
}


qBool DSoundSink::Close()
{
    m_pDSBuffer->Stop();
	m_bOpen = false;
	return true;
}

// ---------------------------------------------------------------------------
DSoundDeviceEnumerator::DSoundDeviceEnumerator()
{
	m_playList = NULL;
	m_playCount = 0;
	m_recordList = NULL;
	m_recordCount = 0;
}

// ---------------------------------------------------------------------------
DSoundDeviceEnumerator::~DSoundDeviceEnumerator()
{
	free( m_playList );
	free( m_recordList );
}

// ---------------------------------------------------------------------------
int DSoundDeviceEnumerator::howManyPlay( )
{
	// --- Have we made the list yet?
	if( !enumerated( ) )
		enumerate();

	// --- How many are there?
	return m_playCount;
}

// ---------------------------------------------------------------------------
int DSoundDeviceEnumerator::howManyRecord( )
{
	// --- Have we made the list yet?
	if( !enumerated( ) )
		enumerate();

	// --- How many are there?
	return m_recordCount;
}

// ---------------------------------------------------------------------------
bool DSoundDeviceEnumerator::getPlayDriver( int index, SOUNDDRIVER *description )
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
bool DSoundDeviceEnumerator::getRecordDriver( int index, SOUNDDRIVER *description )
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
bool DSoundDeviceEnumerator::enumerate( )
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
BOOL CALLBACK DSoundDeviceEnumerator::enumeratePlayCallback( LPGUID guid, LPCSTR description,	LPCSTR module,	LPVOID context )
{
	// --- get a pointer to me
	DSoundDeviceEnumerator *me = ( DSoundDeviceEnumerator * )context;

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
BOOL CALLBACK DSoundDeviceEnumerator::enumerateCaptureCallback( LPGUID guid, LPCSTR description,	LPCSTR module,	LPVOID context )
{
	// --- get a pointer to me
	DSoundDeviceEnumerator *me = ( DSoundDeviceEnumerator * )context;

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

#if 0
int DSoundSink::getHeadsetDevice( )
{
	// --- lets find our headset to play sound on
	for( int i = 0; i < howManyPlay( ); i++ )
	{
		DSoundDeviceEnumerator::SOUNDDRIVER desc;
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
		}
	}
}

#endif
