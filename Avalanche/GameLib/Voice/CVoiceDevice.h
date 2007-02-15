// ---------------------------------------------------------------------------
// File			-> CVoiceDevice.h
// Author		-> Dwain Skinner
// Created		-> December 15, 2003
// Description	-> Platform independent voice device class 
// ---------------------------------------------------------------------------


#ifndef _CVOICEDEVICE_H_
#define _CVOICEDEVICE_H_

class CVoiceDevice
{

public:
	CVoiceDevice												( );
	virtual ~CVoiceDevice									( );

	// --- device status
	enum eDeviceStatus
	{
		DEVICE_NODEVICE	= 0x01,
		DEVICE_CREATED		= 0x02,
		DEVICE_DISABLED	= 0x04,
		DEVICE_ENABLED		= 0x08,
		DEVICE_RECORDING	= 0x10,
		DEVICE_PLAYING		= 0x20,
	};

	// --- defines
	#define DEVICE_INVALID -1

	// --- Device status queries
	bool							deviceExists				( )			{ return( ( m_status & DEVICE_CREATED ) == DEVICE_CREATED ); }
	bool							isEnabled					( )			{ return( ( m_status & DEVICE_ENABLED ) == DEVICE_ENABLED ); }
	bool							isDisabled					( )			{ return( ( m_status & DEVICE_DISABLED ) == DEVICE_DISABLED ); }
	bool							isRecording					( )			{ return( ( m_status & DEVICE_RECORDING ) == DEVICE_RECORDING ); }
	bool							isPlaying					( )			{ return( ( m_status & DEVICE_PLAYING ) == DEVICE_PLAYING ); }


	// --- platform specific creation
	virtual bool				create						( void );

	// --- platform specific buffer creation, ( for play, record buffers )
	virtual u8					*createBuffer				( u32 size );

	// --- device updating, plugged, unplugged, record, play, etc
	virtual void				update						( void );

	// --- device record
	virtual u32					record						( u8 *pDst, u32 size );

	// --- device play
	virtual bool				play							( u8 *pSrc, u32 size );

	// --- device destruction
	virtual bool				destroy						( );

	// --- resets the input stream and starts recording
	virtual bool				startRecording				( );

	// --- stops recording
	virtual bool				stopRecording				( );

	// --- start playing the stream
	virtual bool				startPlaying				( );

	// --- stop playing and reset the buffer pointers
	virtual bool				stopPlaying					( );

	// --- get the number of bytes left in the playback buffer
	virtual u32					getPlayBackBytes			( );

	// --- get the number of bytes left in the record buffer
	virtual u32					getRecordedBytes			( );

protected:
	void							setStatus					( eDeviceStatus status )	{ m_status = status; }
	void							addStatus					( eDeviceStatus status )	{ m_status = ( eDeviceStatus )( m_status | status ); }
	void							removeStatus				( eDeviceStatus status )	{ m_status = ( eDeviceStatus )( m_status & ( ~status ) ); }
	void							setDevice					( int device )					{ m_device = device; }
	int							getDevice					( )								{ return m_device; }

private:
	eDeviceStatus				m_status;					// current device status
	int							m_device;					// handle to the device;
};


#endif // _CVOICEDEVICE_H
