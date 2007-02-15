// ---------------------------------------------------------------------------
// File			-> CPlatformVoiceDevice.h
// Author		-> Dwain Skinner
// Created		-> December 15, 2003
// Description	-> Win32 Platform dependent voice device class 
// ---------------------------------------------------------------------------


#ifndef _CPLATFORMVOICEDEVICE_H_
#define _CPLATFORMVOICEDEVICE_H_

#include "Voice/CVoiceDevice.h"

//#define WIN32_LEAN_AND_MEAN

//#include <windows.h>
//#include <mmsystem.h>
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#include <dsound.h>

class CVoiceDeviceEnumerator
{
public:
	CVoiceDeviceEnumerator									( );
	~CVoiceDeviceEnumerator									( );

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



class CPlatformVoiceDevice : public CVoiceDevice, public CVoiceDeviceEnumerator
{

public:

	CPlatformVoiceDevice										( );
	~CPlatformVoiceDevice									( );

	virtual bool				create						( void );

	// --- platform specific buffer creation, ( for play, record buffers )
	virtual u8					*createBuffer				( u32 size );

	// --- device updating, plugged, unplugged, record, play, etc
	virtual void				update						( void );

	// --- record data from the device
	u32							record						( u8 *pDst, u32 size );

	// --- play data on the device
	bool							play							( u8 *pSrc, u32 size );

	// --- platform specific destroy function
	bool							destroy						( void );

	// --- reset the input stream and start recording
	bool							startRecording				( void );

	// --- stop recording
	bool							stopRecording				( void );

	// --- start playing
	bool							startPlaying				( void );

	// --- stop playing
	bool							stopPlaying					( void );

	// --- get the number of bytes left in the playback buffer
	virtual u32					getPlayBackBytes			( );

	// --- get the number of bytes left in the record buffer
	virtual u32					getRecordedBytes				( );

	// --- used for internal direct sound buffer size
	#define PLAY_BUFFER_SIZE_SECONDS 2
	#define RECORD_BUFFER_SIZE_SECONDS 2

private:
	SOUNDDRIVERID				m_playDeviceID;
	SOUNDDRIVERID				m_recordDeviceID;
//	SoundDriverServices		*services;		// Services (internal interface)
//	HWND							hWndOwner;		// Our owning window
	LPDIRECTSOUND				m_pDirectSoundPlay;		// The direct sound play object
//pab	LPDIRECTSOUNDCAPTURE		m_pDirectSoundRecord;	// The direct sound record object
//	SOUNDPRIORITY				priority;		// Our sound driver's system priority
	LPDIRECTSOUNDBUFFER 		m_pPrimaryBuffer;	// primary buffer
	bool							stereo;			// Are we in stereo?
	unsigned int				sampleRate;

	DSBUFFERDESC				m_playBufferDesc;
//pab	DSCBUFFERDESC				m_recordBufferDesc;
	LPDIRECTSOUNDBUFFER		m_pPlayBuffer;
//pab	LPDIRECTSOUNDCAPTUREBUFFER	m_pRecordBuffer;
	WAVEFORMATEX				m_playWaveFormat;
	WAVEFORMATEX				m_recordWaveFormat;
	DWORD							m_nextReadOffset;
	DWORD							m_nextCaptureOffset;
};




#endif // _CPLATFORMVOICEDEVICE_H_
