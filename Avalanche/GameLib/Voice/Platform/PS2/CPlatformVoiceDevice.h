// ---------------------------------------------------------------------------
// File			-> CPlatformVoiceDevice.h
// Author		-> Dwain Skinner
// Created		-> December 15, 2003
// Description	-> PS2 Platform dependent voice device class 
//
//  Copyright (c) 2003, Avalanche Software Inc.
//  All Rights Reserved.
// ---------------------------------------------------------------------------


#ifndef _CPLATFORMVOICEDEVICE_H_
#define _CPLATFORMVOICEDEVICE_H_

#include "Voice/CVoiceDevice.h"
#include "liblgaud.h"

// --- forward declarations
class CVoiceCodec;

class CPlatformVoiceDevice : public CVoiceDevice
{

public:

	CPlatformVoiceDevice										( );
	~CPlatformVoiceDevice									( );

	// --- each platform must have its own creation function
	virtual bool				create						( void );

	// --- each platfrom must have its own update function 
	virtual void				update						( void );

	// --- for creating record/play buffers
	virtual u8					*createBuffer				( u32 size );

	// --- each platform must have its own destroy function
	virtual bool				destroy						( );

	// --- record data from the device
	virtual u32					record						( u8 *pDst, u32 size );

	// --- play data on the device
	virtual bool				play							( u8 *pSrc, u32 size );

	// --- reset the recording stream and start recording
	virtual bool				startRecording				( );

	// --- stop the recording stream
	virtual bool				stopRecording				( );

	// --- start playing the stream
	virtual bool				startPlaying				( );

	// --- stop playing and reset the buffer pointers
	virtual bool				stopPlaying					( );

	// --- get the number of bytes left in the playback buffer
	virtual u32					getPlayBackBytes			( );

	// --- get the number of bytes left in the record buffer
	virtual u32					getRecordedBytes			( );


private:
	// --- load all the neccessary drivers for the device
	bool							loadDeviceDrivers			( void );

	// --- open the device for record and play
	int							openDevice					( void );

	// --- check the state of devices( plugged, unplugged, etc )
	bool							deviceChange				( void );

	// --- send voice packet over the network
	bool							send							( void );

	// --- receive voice packet over the network
	bool							receive						( void );

	// --- compute checksum on the data packet
	u32							calcCheckSum				( u8 *data, u32 size );

	// --- member vars
	int							m_interruptHandlerID;
	lgAudDeviceDesc			m_deviceDescription;
	int							m_enumHint;
	bool							m_recordEnabled;
	bool							m_playEnabled;
	int							m_bytesRecorded;

#ifdef _DEBUG
	// --- debugging vars
	int							m_outputTime;
	int							m_maxBytesRead;
	int							m_maxBytesWritten;
#endif

};




#endif // _CPLATFORMVOICEDEVICE_H_