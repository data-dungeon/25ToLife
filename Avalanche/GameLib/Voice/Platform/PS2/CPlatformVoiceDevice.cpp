// ---------------------------------------------------------------------------
// File			-> CPlatformVoiceDevice.cpp
// Author		-> Dwain Skinner
// Created		-> December 15, 2003
// Description	-> PS2 Platform dependent voice device class implementation
//
//  Copyright (c) 2003, Avalanche Software Inc.
//  All Rights Reserved.
// ---------------------------------------------------------------------------

// Precompiled header
#include "Voice/VoicePCH.h"

// --- Local includes

// --- platform includes
#include <eekernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <libnet.h>
#include <libmrpc.h>

// --- standard codec sampling parameters (8khz, 16bit, mono)
#define IN_SAMPLING_FREQUENCY 8000
#define IN_SAMPLING_BITS 16
#define IN_SAMPLING_CHANNELS 1
#define RECORDLENGTH 350     // milliseconds  
#define LGRECORD_BUFFER_SIZE ( 20 * 1024 )
#define LGPLAY_BUFFER_SIZE ( 20 * 1024 )


// ---------------------------------------------------------------------------
CPlatformVoiceDevice::CPlatformVoiceDevice( )
{
	bool success = false;

	// --- assume no device yet
	setStatus( ( eDeviceStatus )( DEVICE_NODEVICE | DEVICE_DISABLED ) );
	setDevice( LGAUD_INVALID_DEVICE );
	m_enumHint			= LGAUD_HINT_NOENUMNEEDED;
	m_recordEnabled	= false;
	m_playEnabled		= false;

#ifdef _DEBUG
	m_maxBytesRead		= 0;
	m_maxBytesWritten	= 0;
	m_outputTime		= DEBUG_OUTPUT_FREQUENCY;
#endif


	// --- lets create the device and initialize it
	if( create( ) )
	{
		setStatus( ( eDeviceStatus )DEVICE_CREATED );
	}
}

// ---------------------------------------------------------------------------
CPlatformVoiceDevice::~CPlatformVoiceDevice( )
{
}

// ---------------------------------------------------------------------------
u8 *CPlatformVoiceDevice::createBuffer( u32 size )
{
	u8 *pBuf = new u8[ size ];
	return( pBuf );
}

// ---------------------------------------------------------------------------
void CPlatformVoiceDevice::update( void )
{
	// --- see if a device was added or removed
	if( deviceChange( ) )
	{
		// --- open the new device
		openDevice( );
	}

#ifdef _DEBUG
	if( m_outputTime == 0 )
	{
		dbgPrint( "Max bytes read from Headset = %4d...\n", m_maxBytesRead );
		m_outputTime = DEBUG_OUTPUT_FREQUENCY;
	}
	m_outputTime--;
#endif
}

// ---------------------------------------------------------------------------
bool CPlatformVoiceDevice::create( void )
{
	bool success;

	// --- load the device drivers
	success = loadDeviceDrivers( );

	return( success );
}

// ---------------------------------------------------------------------------
bool CPlatformVoiceDevice::destroy( )
{
	return( true );
}


// ---------------------------------------------------------------------------
bool CPlatformVoiceDevice::loadDeviceDrivers( void )
{
	bool success = false;
	char usbd_argv[] = "conf=1024\0";
	char lgaud_argv[32];
	sprintf(lgaud_argv, "thpri=10\0rpc=1\0maxstream=%d", LGRECORD_BUFFER_SIZE );
	strcat(lgaud_argv, "\0");

	PS2App *app = PS2App::IAm();

	// --- load the usb drivers
	if( ( success = app->LoadModule( "IRX\\USBD.IRX", sizeof( usbd_argv ), usbd_argv ) ) )
	{
		// --- load the logitech headset audio drivers
		success = app->LoadModule( "IRX\\LGAUD.IRX", strlen( lgaud_argv ) + 2, lgaud_argv );
	}

	// --- Init liblgaud
	lgAudInit( memAlloc, memFree );
	return( success );
}

// ---------------------------------------------------------------------------
// Find the first connected headset and open it
int CPlatformVoiceDevice::openDevice( void )
{
	int	nResult = LGAUD_SUCCESS;

	// --- see if we already have a valid device opened and close it
	int currentDevice = getDevice( );
	if( currentDevice != LGAUD_INVALID_DEVICE)
	{
		lgAudClose( currentDevice );
		currentDevice = LGAUD_INVALID_DEVICE;
		setDevice( currentDevice );
		dbgPrint( "USB Headset Removed...\n" );
	}

	// --- enumerate all the valid devices
	int d = 0;
	while( nResult == LGAUD_SUCCESS )
	{
		// --- look for a device
		nResult = lgAudEnumerate( d, &m_deviceDescription );
		
		// --- we found one
		if( nResult == LGAUD_SUCCESS )
		{
			lgAudOpenParam openParam;
			openParam.Mode											= LGAUD_MODE_PLAYRECORD;
			openParam.RecordingFormat.Channels				= IN_SAMPLING_CHANNELS;
			openParam.RecordingFormat.BitResolution		= IN_SAMPLING_BITS;
			openParam.RecordingFormat.SamplingRate			= IN_SAMPLING_FREQUENCY;
			openParam.RecordingFormat.BufferMilliseconds = RECORDLENGTH;
			openParam.PlaybackFormat							= openParam.RecordingFormat;
			if( lgAudOpen( d, &openParam, &currentDevice ) == LGAUD_SUCCESS )
			{
				lgAudSetRecordingVolume( currentDevice, LGAUD_CH_BOTH, 70 );
				lgAudSetPlaybackVolume( currentDevice, LGAUD_CH_BOTH, 80 );
				lgAudStartRecording( currentDevice );
				lgAudStartPlayback( currentDevice );
				m_recordEnabled = true;
				m_playEnabled = true;
				dbgPrint( "USB Headset found and opened...\n" );
				break;
			}
		}
		d++;

		// --- logitech documentation says we should restart the enumeration if we get this error
		// --- typically means a device was unplugged during the enumeration
		if( nResult == LGAUD_ERR_DEVICE_LOST )
		{
			d = 0;
		}
	}

	ASSERTS( ( nResult == LGAUD_ERR_NO_MORE_DEVICES ) || ( nResult == LGAUD_SUCCESS ), "Error enumerating USB devices" );

	// --- set the current device if any
	setDevice( currentDevice );
	return( currentDevice );
}

// ---------------------------------------------------------------------------
bool CPlatformVoiceDevice::deviceChange( void )
{
	int result;

	// --- enumeration of the devices is needed
	if( m_enumHint == LGAUD_HINT_ENUMNEEDED )
	{
		m_enumHint = LGAUD_HINT_NOENUMNEEDED;
		return true;
	}

	// --- so far, no positive enumeration hint has been received
	// --- schedule the receipt of a hint
	result = lgAudEnumHint( &m_enumHint );
	ASSERTS( result == LGAUD_SUCCESS, "Logitech Audio enumeration hint error" );
	return false;
}

// ---------------------------------------------------------------------------
u32 CPlatformVoiceDevice::record( u8 *pDst, u32 size )
{
	int currentDevice = getDevice( );
	m_bytesRecorded	= 0;

	if( currentDevice != LGAUD_INVALID_DEVICE )
	{
		int localBytesRecorded	= size;
		if( lgAudRead( currentDevice, LGAUD_BLOCKMODE_NOT_BLOCKING, pDst, &localBytesRecorded ) == LGAUD_SUCCESS )
		{
#ifdef _DEBUG
			if( localBytesRecorded > m_maxBytesRead ) m_maxBytesRead = localBytesRecorded;
#endif
			// --- keep track of how much data we read in
			m_bytesRecorded = localBytesRecorded;

		}
	}
	return( m_bytesRecorded );
}

// ---------------------------------------------------------------------------
bool CPlatformVoiceDevice::play( u8 *pSrc, u32 size )
{
	if( size <= 0 ) return( false );

	int currentDevice = getDevice( );

	if( currentDevice != LGAUD_INVALID_DEVICE )
	{
		int localSize = size;
		while( lgAudWrite( currentDevice, LGAUD_BLOCKMODE_NOT_BLOCKING, pSrc, &localSize ) != LGAUD_SUCCESS )
//		while( lgAudWrite( currentDevice, LGAUD_BLOCKMODE_BLOCKING, pSrc, &localSize ) != LGAUD_SUCCESS )
		{
			ASSERTS( false, "error writing to headset device" );
		}
	}
	return( true );
}

// ---------------------------------------------------------------------------
bool CPlatformVoiceDevice::startRecording( )
{
	int currentDevice = getDevice( );
	int ret;

	if( currentDevice != LGAUD_INVALID_DEVICE )
	{
		ret = lgAudStartRecording( currentDevice );
	}
	return( ret == LGAUD_SUCCESS );
}

// ---------------------------------------------------------------------------
bool CPlatformVoiceDevice::stopRecording( )
{
	int currentDevice = getDevice( );
	int ret;

	if( currentDevice != LGAUD_INVALID_DEVICE )
	{
		ret = lgAudStopRecording( currentDevice );
	}
	return( ret == LGAUD_SUCCESS );
}

// ---------------------------------------------------------------------------
bool CPlatformVoiceDevice::startPlaying( )
{
	return( true );
}

// ---------------------------------------------------------------------------
bool CPlatformVoiceDevice::stopPlaying( )
{
	return( true );
}

// ---------------------------------------------------------------------------
u32 CPlatformVoiceDevice::getPlayBackBytes( )
{
	int sizeLeft = 0;
	int currentDevice = getDevice( );
	int ret;

	if( currentDevice != LGAUD_INVALID_DEVICE )
	{
		ret = lgAudGetRemainingPlaybackBytes( currentDevice, &sizeLeft );
		ASSERTS( ret == LGAUD_SUCCESS, "unable to get play back bytes" );
	}
	return( sizeLeft );
}

static u32 lastSize = 1;
// ---------------------------------------------------------------------------
u32 CPlatformVoiceDevice::getRecordedBytes( )
{
	int sizeLeft = 0;
	int currentDevice = getDevice( );
	int ret;

	if( currentDevice != LGAUD_INVALID_DEVICE )
	{
		ret = lgAudGetAvailableRecordingBytes( currentDevice, &sizeLeft );
		ASSERTS( ret == LGAUD_SUCCESS, "unable to get recorded bytes" );
/*		if( sizeLeft == lastSize  )
		{
			startRecording( );
		}
		lastSize = sizeLeft;*/
	}
	return( sizeLeft );
}