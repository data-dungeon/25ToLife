// ---------------------------------------------------------------------------
// File			-> CVoiceDevice.cpp
// Author		-> Dwain Skinner
// Created		-> December 15, 2003
// Description	-> Platform independent voice device class implementation
// ---------------------------------------------------------------------------

// Precompiled header
#include "Voice/VoicePCH.h"


// ---------------------------------------------------------------------------
CVoiceDevice::CVoiceDevice( )
{
	m_status = ( eDeviceStatus )( DEVICE_NODEVICE | DEVICE_DISABLED );
	m_device = DEVICE_INVALID;
}


// ---------------------------------------------------------------------------
CVoiceDevice::~CVoiceDevice( )
{
}

// ---------------------------------------------------------------------------
bool CVoiceDevice::create( void )
{
	ASSERTS( false, "This must be defined by the platform" );
	return( false );
}

// ---------------------------------------------------------------------------
bool CVoiceDevice::destroy( void )
{
	ASSERTS( false, "This must be defined by the platform" );
	return( false );
}

// ---------------------------------------------------------------------------
void CVoiceDevice::update( void )
{
	ASSERTS( false, "This must be defined by the platform" );
}

// ---------------------------------------------------------------------------
u8 *CVoiceDevice::createBuffer( u32 size )
{
	ASSERTS( false, "This must be defined by the platform" );
	return( NULL );
}

// ---------------------------------------------------------------------------
u32 CVoiceDevice::record( u8 *pDst, u32 size )
{
	ASSERTS( false, "This must be defined by the platform" );
	return( 0 );
}

// ---------------------------------------------------------------------------
bool CVoiceDevice::play( u8 *pSrc, u32 size )
{
	ASSERTS( false, "This must be defined by the platform" );
	return( false );
}

// ---------------------------------------------------------------------------
bool CVoiceDevice::startRecording( )
{
	ASSERTS( false, "This must be defined by the platform" );
	return( false );
}

// ---------------------------------------------------------------------------
bool CVoiceDevice::stopRecording( )
{
	ASSERTS( false, "This must be defined by the platform" );
	return( false );
}

// ---------------------------------------------------------------------------
bool CVoiceDevice::startPlaying( )
{
	ASSERTS( false, "This must be defined by the platform" );
	return( false );
}

// ---------------------------------------------------------------------------
bool CVoiceDevice::stopPlaying( )
{
	ASSERTS( false, "This must be defined by the platform" );
	return( false );
}

// ---------------------------------------------------------------------------
u32 CVoiceDevice::getPlayBackBytes( )
{
	ASSERTS( false, "This must be defined by the platform" );
	return( 0 );
}

// ---------------------------------------------------------------------------
u32 CVoiceDevice::getRecordedBytes( )
{
	ASSERTS( false, "This must be defined by the platform" );
	return( 0 );
}
