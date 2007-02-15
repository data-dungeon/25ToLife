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

#include "DSoundSource.h"
#include <platform/core/platformimpl.h>
#include <platform/core/logdevicedebugoutput.h>
#include <basetsd.h>
#include <commdlg.h>
#include <mmreg.h>
#include <dxerr9.h>

#define NUM_PLAY_NOTIFICATIONS  16

USING_NAMESPACE_QUAZAL;

DSoundSource::DSoundSource(const AudioStreamFormat &oFormat, qUnsignedInt uiNumPackets):
			m_oAudioStreamFormat(oFormat),
			m_uiNumPackets(uiNumPackets)
{
	SYSTEMCHECK(uiNumPackets <= 32);
	m_bInitialized = false;
	m_pDSBuffer = NULL;
	m_pDS = NULL;
	m_uiNextPacket = 0;
	m_bCapturing = false;
	m_uiHighRead = 0;
	m_uiLastRead = 0;

    // Create a static IDirectSound in the CSound class.  
    // Set coop level to DSSCL_PRIORITY, and set primary buffer 
    // format to stereo, 22kHz and 16-bit output.
    if( FAILED( InitializeDirectSound( ) ) )
    {
        QTRACE(TRACE_ALWAYS,(_T("Error Initializing DirectSound")));
        return;
    }
	if (FAILED(CreateCaptureBuffer())) {
		return;
	}
	QTRACE(TRACE_ALWAYS,(_T("DirectSound Initialization successful")));
	m_bInitialized = true;
}


DSoundSource::~DSoundSource() 
{
}


qBool DSoundSource::Open()
{
	if (!m_bInitialized) {
		return false;
	}
	m_uiNextPacket = 0;
	m_bCapturing = false;
	m_uiHighRead = 0;
	m_uiLastRead = 0;
	m_bOpen = true;
	return true;
}


HRESULT DSoundSource::InitializeDirectSound( )
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
    if( FAILED( hr = DirectSoundCreate8( m_playDeviceID, &m_pDS, NULL ) ) )
        return DXTRACE_ERR( TEXT("DirectSoundCreate8"), hr );

    if( FAILED( hr = DirectSoundCaptureCreate( NULL, &m_pDSCapture, NULL ) ) )
        return DXTRACE_ERR( TEXT("DirectSoundCaptureCreate"), hr );   

	return S_OK;
}


HRESULT DSoundSource::CreateCaptureBuffer( ) 
{
    HRESULT hr;

    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;

    DSCBUFFERDESC dscbd;

    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) GetFormat().GetNbChannels(); 
    wfx.nSamplesPerSec  = (DWORD) GetFormat().GetNbSamplesPerSecond(); 
    wfx.wBitsPerSample  = (WORD) GetFormat().GetNbBitsPerSample(); 
    wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);
	//
   // Create the capture buffer
    ZeroMemory( &dscbd, sizeof(dscbd) );
    dscbd.dwSize        = sizeof(dscbd);
    dscbd.dwBufferBytes = m_uiNumPackets * GetFormat().GetPacketSize();
    dscbd.lpwfxFormat   = &wfx; // Set the format during creatation

    if( FAILED( hr = m_pDSCapture->CreateCaptureBuffer( &dscbd, &m_pDSBuffer, NULL ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("CreateCaptureBuffer"), hr );

    return S_OK;
}


StreamingDevice::Status DSoundSource::Read(qByte *pSound, qUnsignedInt uiBufferSize)
{
	if (uiBufferSize == 0) {
		uiBufferSize = GetFormat().GetPacketSize();
	}
	SYSTEMCHECK(uiBufferSize == GetFormat().GetPacketSize());

	DUMMY_PARAM(pSound);
    HRESULT hr; 
    VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
    VOID*   pDSLockedBuffer2      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize2 = 0;    // Size of the locked DirectSound buffer

	qUnsignedInt32 uiPacketSize = GetFormat().GetPacketSize();
	qUnsignedInt32 uiNextReadOffset = (m_uiNextPacket % m_uiNumPackets) * uiPacketSize;

	if ((m_uiNextPacket == 0) && !m_bCapturing) {
		if (FAILED(m_pDSBuffer->Start(DSCBSTART_LOOPING))) {
			QTRACE(TRACE_NEVER,(_T("Failed to record sound")));
			return Error;
		}
		m_bCapturing = true;
	}

	unsigned long uiCurrent = 0xFFFFFFFF;
	unsigned long uiRead;
	m_pDSBuffer->GetCurrentPosition(&uiCurrent, &uiRead);

	if (uiRead < m_uiLastRead) {
		m_uiHighRead += m_uiNumPackets * uiPacketSize;
	}
	m_uiLastRead = uiRead;

	if (m_uiHighRead+uiRead >= ((m_uiNextPacket+1) * uiPacketSize) ) {
		// Lock the buffer down
		hr = m_pDSBuffer->Lock( uiNextReadOffset,  uiPacketSize, 
								&pDSLockedBuffer, &dwDSLockedBufferSize, 
								&pDSLockedBuffer2, &dwDSLockedBufferSize2, 0 );
		if (SUCCEEDED(hr)) {
			QTRACE(TRACE_NEVER,(_T("Read Buffer locked %d-%d,%d-%d, Positions are %d,%d"),
							uiNextReadOffset, 
							uiNextReadOffset + dwDSLockedBufferSize,
							uiNextReadOffset + uiPacketSize, 
							(uiNextReadOffset + uiPacketSize) + dwDSLockedBufferSize2,
							uiCurrent,uiRead));
			CopyMemory( pSound, (BYTE*) pDSLockedBuffer, dwDSLockedBufferSize); 
			if (pDSLockedBuffer2 != NULL) {
				CopyMemory( pSound+dwDSLockedBufferSize, (BYTE*) pDSLockedBuffer2, dwDSLockedBufferSize2); 
			}
			m_uiNextPacket++;
			m_pDSBuffer->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, pDSLockedBuffer2, dwDSLockedBufferSize2 );
		} else {
			QTRACE(TRACE_NEVER,(_T("Cannot lock %d-%d, Positions are %d,%d"),
							uiNextReadOffset, 
							(uiNextReadOffset + uiPacketSize), uiCurrent,uiRead));
			return NotReady;
		}
		return Success;
	} else {
		return NotReady;
	}
}


qBool DSoundSource::Close()
{
    m_pDSBuffer->Stop();
	m_bOpen = false;
	return true;
}


