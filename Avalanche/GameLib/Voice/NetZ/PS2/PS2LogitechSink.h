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

#ifndef _PS2LogitechSink_H_
#define _PS2LogitechSink_H_

#include <extensions/avstreams/core/sinkdevice.h>

BEGIN_NAMESPACE_QUAZAL;
#define MAX_SINK_BUFFERS 4
#define MAX_QUEUE_SIZE 3

class PS2LogitechSink: public SinkDevice {
public:
	static const int AnyDevice = -1;
public:
	PS2LogitechSink(const AudioStreamFormat &oFormat, qInt iDeviceIndex = AnyDevice);
	virtual ~PS2LogitechSink();

	 qBool 			Open();
	 Status		 	Write(qByte *pPCMBuffer, qUnsignedInt uiBufferSize = 0);
	 Status			DecodeAndFlush( );
	 qBool 			Close();

	 qInt			GetDeviceHandle() { return m_hDevice; }

	const AudioStreamFormat& GetFormat() const { return m_oAudioStreamFormat; }
	void				resetStreamID( void ) { m_curStreamID = 0; m_curDblBuffer ^= 1; }
	qBool				createSinkBuffers( int maxStreams, int encodedSize );
	qByte*			GetSinkBuffer( qInt index );
	Status			WriteFinal( );
	qInt16*			GetPCMBuffer( ) { return ( qInt16 * )( ( ( qByte * )m_pPCMData + ( m_curDblBuffer * m_maxPCMSize ) ) ); }
//	qBool				HaveDataToDecode( ) { return m_dataToDecode; }
	inline qBool	HaveDataToDecode( );
	inline qBool	HaveDataToDecodeOnStreams( int maxIndex );
	inline qBool	HaveDataToDecode( int index );
	inline void		flushStream( int index );
	qInt				GetCurNumStreams( ) { return( m_curStreamID ); }
	qByte*			GetExtraEncodeBuffer( qInt index ) { return( m_pSinkBuffers + ( index * m_maxBufSize ) + ( m_encodedSize * ( m_maxStreams - 1 ) ) ); }
	void				SetIncomingPacketStreamIndex( qInt index ) { m_curStreamID = index; };
	void				SetVolume( float volume ) { m_volume = volume;}

private:
	qBool				OpenDevice(qInt iDeviceID);
	qBool				QueueWrite();
	qBool				WriteCompleted(qInt *iResult);

private:
	qInt				m_curWriteBufferIndex[ MAX_SINK_BUFFERS ];
	qInt				m_curReadBufferIndex[ MAX_SINK_BUFFERS ];
	qInt				m_curDblBuffer;
	qInt				m_curStreamID;
	qInt				m_maxStreams;
	qInt				m_encodedSize;
	qInt				m_maxBufSize;
	qInt				m_maxPCMSize;
	qByte*			m_pSinkBuffers;
	qInt16*			m_pPCMData;
	qInt				m_hDevice;
	qInt				m_iDeviceIndex;
	Time				m_tiOpenTime;
	qBool				m_bPlaying;
	AudioStreamFormat	m_oAudioStreamFormat;
//	qBool				m_dataToDecode;
	float				m_volume;

#ifdef AVSTREAMS_PROFILING
	 ProfilingUnit		m_oPUWrite;
#endif

};

inline qBool PS2LogitechSink::HaveDataToDecode( )
{
	for( int i = 0; i < MAX_SINK_BUFFERS; i++ )
	{
		if( m_curReadBufferIndex[ i ] != m_curWriteBufferIndex[ i ] ) return( true );
	}
	return( false );
}

inline qBool PS2LogitechSink::HaveDataToDecode( int index )
{
	ASSERTS( index < MAX_SINK_BUFFERS, "index out of range" );
	return( ( m_curReadBufferIndex[ index ] != m_curWriteBufferIndex[ index ] ) ? true : false );
}

inline qBool PS2LogitechSink::HaveDataToDecodeOnStreams( int maxIndex )
{
	ASSERTS( maxIndex <= MAX_SINK_BUFFERS, "index out of range" );
	for( int i = 0; i < maxIndex; i++ )
	{
		if( m_curReadBufferIndex[ i ] != m_curWriteBufferIndex[ i ] ) return( true );
	}
	return( false );
}

inline void PS2LogitechSink::flushStream( int index )
{
	ASSERTS( index < MAX_SINK_BUFFERS, "index out of range" );
	m_curReadBufferIndex[ index ] = 0;
	m_curWriteBufferIndex[ index ] = 0;
}
END_NAMESPACE_QUAZAL;

#endif
