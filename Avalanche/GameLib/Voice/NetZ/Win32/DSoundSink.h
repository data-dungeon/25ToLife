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

#ifndef _DSoundSink_H_
#define _DSoundSink_H_

#include <platform/core/platformdecl.h>
#include <platform/core/logdevicedebugoutput.h>
#include <extensions/avstreams/core/sinkdevice.h>
#include <windows.h>
#include <dsound.h>


BEGIN_NAMESPACE_QUAZAL;

class DummyDialog;

class DSoundDeviceEnumerator
{
public:
	DSoundDeviceEnumerator									( );
	~DSoundDeviceEnumerator									( );

	// Here is how we identify a system sound driver ID
	typedef LPGUID SOUNDDRIVERID;

	// Here is a brief description of a sound device
	struct SOUNDDRIVER
	{
		SOUNDDRIVERID	driverID;
		char				description[128];
		char				driverName[128];
	};

	// --- How many are there?
	int							howManyPlay					( );
	int							howManyRecord				( );

	// --- Get a specific driver description (between 0 and HowMany - 1)
	bool							getPlayDriver					( int index, SOUNDDRIVER *description );
	bool							getRecordDriver				( int index, SOUNDDRIVER *description );

protected:
	SOUNDDRIVER					*m_playList;			// List
	int							m_playCount;			// How many are there?
	SOUNDDRIVER					*m_recordList;			// List
	int							m_recordCount;			// How many are there?

	// The enumeration call
	bool							enumerated					( ){ return ( m_playList != NULL && m_recordList != NULL ); }
	bool							enumerate					( );


	// --- for devices that can play
	static BOOL CALLBACK		enumeratePlayCallback	( LPGUID guid, LPCSTR description, LPCSTR module, LPVOID context );

	// --- for devices that can capture sound
	static BOOL CALLBACK		enumerateCaptureCallback( LPGUID guid, LPCSTR description, LPCSTR module, LPVOID context );
};


class DSoundSink: public SinkDevice, public DSoundDeviceEnumerator {
public:
	DSoundSink(HWND hWnd, const AudioStreamFormat &oFormat, qUnsignedInt uiNumPackets = 4);
	virtual ~DSoundSink();

	qBool Open();
	Status Write(NS_QUAZAL qByte *pPCMBuffer, qUnsignedInt uiBufferSize = 0);
	qBool Close();

	const AudioStreamFormat& GetFormat() const { return m_oAudioStreamFormat; }
private:
	HRESULT InitializeDirectSound( HWND hWnd, qUnsignedInt dwCoopLevel );
	HRESULT CreateSoundBuffer();

private:
    LPDIRECTSOUND8 		m_pDS;
    LPDIRECTSOUNDBUFFER m_pDSBuffer;
	qUnsignedInt		m_uiNextPacket;
	qUnsignedInt		m_uiNumPackets;
	DummyDialog			*m_pDialog;
	qBool				m_bInitialized; 
	AudioStreamFormat	m_oAudioStreamFormat;

	SOUNDDRIVERID				m_playDeviceID;
};

END_NAMESPACE_QUAZAL;

#endif
