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
#include "Voice/VoicePCH.h"

#include "PS2LogitechSink.h"
#include <platform/core/platformimpl.h>
#include "PS2LogitechDevices.h"
#include <liblgaud.h>
#include "cmdcon/cmdcon.h"

USING_NAMESPACE_QUAZAL;

PS2LogitechSink::PS2LogitechSink(const AudioStreamFormat &oFormat, qInt iDeviceIndex):
					m_oAudioStreamFormat(oFormat)
#ifdef AVSTREAMS_PROFILING
					,m_oPUWrite(_T("Headset Write"),5000)
#endif
					,m_curStreamID( 0 )
					,m_curDblBuffer( 0 )
					,m_maxBufSize( 0 )
					,m_maxPCMSize( 0 )
//					,m_dataToDecode( false )
					,m_volume( 1.0f )

{
	m_iDeviceIndex = iDeviceIndex;
#ifdef AVSTREAMS_PROFILING
	m_oPUWrite.EnableLogging(true);
#endif
}


PS2LogitechSink::~PS2LogitechSink() 
{
	if( m_pSinkBuffers )
	{
		qFree( m_pSinkBuffers );
		m_pSinkBuffers = NULL;
	}
	if( m_pPCMData )
	{
		qFree( m_pPCMData );
		m_pPCMData = NULL;
	}
//	m_dataToDecode = false;
}


qBool PS2LogitechSink::createSinkBuffers( int maxStreams, int encodedSize )
{
	// --- create an extra stream of silence
	m_maxStreams	= maxStreams + 1;
	m_encodedSize	= encodedSize;

	// --- the maximum size of the sink buffers,  add 2 for a liner
	m_maxBufSize	= ( m_maxStreams * encodedSize ) + 2;
	if( m_maxBufSize & 0x1 ) m_maxBufSize++;

	// --- create queue buffers
	m_pSinkBuffers = qAlloc( qByte, m_maxBufSize * MAX_QUEUE_SIZE );

	// --- pcm data buffers, are double buffered, add 2 for a liner
	m_maxPCMSize = GetFormat( ).GetPacketSize( );
	m_pPCMData = (qInt16*) qAlloc( qByte, ( m_maxPCMSize * 2 ) + 2 );
	memset( m_pSinkBuffers, 0, m_maxBufSize * MAX_QUEUE_SIZE );
	memset( m_pPCMData, 0, m_maxPCMSize * 2 );
	*( ( unsigned short * )( ( ( qByte * )m_pSinkBuffers + ( m_maxBufSize - 2 ) ) ) ) = 0xdead;
	*( ( unsigned short * )( ( ( qByte * )m_pSinkBuffers + ( m_maxBufSize * 2 ) - 2 ) ) ) = 0xdead;

	// --- initialize our buffer indexes
	ASSERTS( maxStreams <= MAX_SINK_BUFFERS, "Fatal Error, must increase the number of sink buffers" );
	for( int i = 0; i < MAX_SINK_BUFFERS; i++ )
	{
		m_curWriteBufferIndex[ i ] = 0;
		m_curReadBufferIndex[ i ] = 0;
	}
	m_curStreamID = 0;
	m_curDblBuffer= 0;
//	m_dataToDecode = false;
	g_console.Echo("Created Stream Sink buffers...\n" );
}


qBool PS2LogitechSink::Open()
{
	m_hDevice = PS2LogitechDevices::OpenDevice(m_iDeviceIndex,
												GetFormat( ).GetNbChannels( ), 
												GetFormat( ).GetNbSamplesPerSecond( ), 
												GetFormat( ).GetNbBitsPerSample( ) );

	if( m_hDevice != LGAUD_INVALID_DEVICE ) 
	{
		unsigned char vol = ( unsigned char )( m_volume * 100.0f );
		lgAudSetPlaybackVolume(m_hDevice,LGAUD_CH_BOTH,vol);

		m_tiOpenTime = Time::GetTime();
		m_bPlaying = false;
		m_bOpen = true;
//		m_dataToDecode = false;
		return true;
	}
	return false;
}

StreamingDevice::Status PS2LogitechSink::Write(qByte *pSound, qUnsignedInt uiBufferSize)
{
	qInt bufIndex = m_curWriteBufferIndex[ m_curStreamID ];
	qByte *pAdr = ( m_pSinkBuffers + ( m_encodedSize * m_curStreamID ) + ( bufIndex * m_maxBufSize ) );
	ASSERT( pAdr <= ( m_pSinkBuffers + ( m_maxBufSize * MAX_QUEUE_SIZE ) - m_encodedSize ) - 2 );

	//g_console.Echo( "writing to stream index ->%d, Addr->%p, size->%d, bufSize->%d, dblBuf->%d\n", m_curStreamID, pAdr, m_encodedSize, uiBufferSize, bufIndex );
	memcpy( pAdr, pSound, m_encodedSize  );
	ASSERTS( m_curStreamID < m_maxStreams, "Uh oh, we're gonna blow over our allocated streams" );
	ASSERTS( *( ( unsigned short * )( ( qByte * )m_pSinkBuffers + ( m_maxBufSize - 2 ) ) ) == 0xdead, "liner clobered" );
	ASSERTS( *( ( unsigned short * )( ( qByte * )m_pSinkBuffers + ( m_maxBufSize * 2 ) - 2 ) ) == 0xdead, "liner clobered" );

	g_console.Echo( "PS2Logitech Sink, received stream, writing at stream=%d, write queue=%d\n", m_curStreamID, bufIndex );
	// --- go to our next write buffer, wrap around when we need to
	bufIndex++;
	if( bufIndex >= MAX_QUEUE_SIZE ) bufIndex = 0;
	m_curWriteBufferIndex[ m_curStreamID ] = bufIndex;
//	m_dataToDecode = true;
	return Success;
}

qByte *PS2LogitechSink::GetSinkBuffer( qInt index )
{
	ASSERTS( index < m_maxStreams, "index out of range" );
	ASSERTS( *( ( unsigned short * )( ( qByte * )m_pSinkBuffers + ( m_maxBufSize - 2 ) ) ) == 0xdead, "liner clobered" );
	ASSERTS( *( ( unsigned short * )( ( qByte * )m_pSinkBuffers + ( m_maxBufSize * 2 ) - 2 ) ) == 0xdead, "liner clobered" );

	qInt bufIndex = m_curReadBufferIndex[ index ];
	ASSERTS( bufIndex != m_curWriteBufferIndex[ index ], "read index should never be the same as the write index, increase queue size" );

	qByte *pAdr;
	pAdr = m_pSinkBuffers + ( m_encodedSize * index ) + ( bufIndex * m_maxBufSize );
	ASSERT( pAdr <= ( m_pSinkBuffers + ( m_maxBufSize * MAX_QUEUE_SIZE ) - m_encodedSize - 2 ) );

	g_console.Echo( "PS2Logitech Sink, reading at stream=%d, read queue=%d\n", m_curStreamID, bufIndex );
	// --- move to the next read buffer
	bufIndex++;
	if( bufIndex >= MAX_QUEUE_SIZE ) bufIndex = 0;
	m_curReadBufferIndex[ index ] = bufIndex;
	return( pAdr );
}


StreamingDevice::Status PS2LogitechSink::WriteFinal( )
{
	qUnsignedInt32 uiBufferSize = 0;

	ASSERTS( *( ( unsigned short * )( ( qByte * )m_pSinkBuffers + ( m_maxBufSize - 2 ) ) ) == 0xdead, "liner clobered" );
	ASSERTS( *( ( unsigned short * )( ( qByte * )m_pSinkBuffers + ( m_maxBufSize * 2 ) - 2 ) ) == 0xdead, "liner clobered" );

	if (uiBufferSize == 0) {
		uiBufferSize = GetFormat().GetPacketSize();
	}
	qInt iWriteSize = (qInt)uiBufferSize;
	qByte* pSound = ( qByte * )( ( qByte * )m_pPCMData + ( m_maxPCMSize * m_curDblBuffer ) );

	// --- make sure we reset our buffers for next cycle
	resetStreamID( );
//	m_dataToDecode = false;
	//g_console.Echo( "Write Final Called, m_curDblBuffer=%d\n", m_curDblBuffer );
	{
#ifdef AVSTREAMS_PROFILING
		ProfilingUnit::Scope oPUS(m_oPUWrite);
#endif
		if (lgAudWrite(m_hDevice, LGAUD_BLOCKMODE_NOT_BLOCKING, pSound, (qInt*)&iWriteSize) != LGAUD_SUCCESS) {
			return Error;
		}
	}

	if (iWriteSize != (qInt)uiBufferSize) {
		QTRACE(TRACE_ALWAYS,(_T("PS2LogitechSink: Partial write detected")));
	}

	if ((!m_bPlaying) && (Time::GetTime()-m_tiOpenTime > 50)) {
		if (lgAudStartPlayback(m_hDevice) == LGAUD_SUCCESS) {
			QTRACE(TRACE_ALWAYS,(_T("PS2LogitechSink: Playback started")));
			m_bPlaying = true;
		} else {
			QTRACE(TRACE_ALWAYS,(_T("PS2LogitechSink: Cannot start playback")));
			return Error;
		} 
	}

	ASSERTS( *( ( unsigned short * )( ( qByte * )m_pSinkBuffers + ( m_maxBufSize - 2 ) ) ) == 0xdead, "liner clobered" );
	ASSERTS( *( ( unsigned short * )( ( qByte * )m_pSinkBuffers + ( m_maxBufSize * 2 ) - 2 ) ) == 0xdead, "liner clobered" );
	return Success;
}

qBool PS2LogitechSink::Close()
{
	SYSTEMCHECK(m_hDevice != LGAUD_INVALID_DEVICE);
	m_bOpen = false;
	PS2LogitechDevices::CloseDevice(m_hDevice);
//	m_dataToDecode = false;
	return true;
}
