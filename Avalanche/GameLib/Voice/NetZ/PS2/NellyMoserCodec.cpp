// ----------------------------------------------------------------------------
// File			-> NellyMoserCodec.h
// Created		-> June 21, 2004
// Author		-> Dwain Skinner
// Description	-> Derived class for use with the NellyMoser Codec library under NetZ
// ----------------------------------------------------------------------------

#include <VoicePCH.h>

#include "NellyMoserCodec.h"
#include <platform/core/platformimpl.h>
#include "nellymoser_wrapper.h"
#include "CTimer.h"

#ifndef CONSUMER_BUILD
#define TIMEIT
#endif

static HNMS4 g_NellyMoserHNMS1x4;

#ifdef TIMEIT
static float maxEncodeTime=0;
static float maxDecodeTime=0;
#endif

// ----------------------------------------------------------------------------
NellyMoserCodec::NellyMoserCodec(TimeInterval tiPacketDuration)
{
	m_uiNbFramesPerPackets = tiPacketDuration/20;
	if (m_uiNbFramesPerPackets == 0) {
		m_uiNbFramesPerPackets = 1;
	}
	m_oAudioStreamFormat.SetNbSamplesPerPacket(160*m_uiNbFramesPerPackets);

	// --- initialize NellyMoser state
	nmsInit( );

	// --- create are 1x4 encoder/decoder
	g_NellyMoserHNMS1x4 = nmsCreate4( );
	ASSERT( g_NellyMoserHNMS1x4 );
}


// ----------------------------------------------------------------------------
NellyMoserCodec::~NellyMoserCodec()
{
	if( g_NellyMoserHNMS1x4 ) nmsDestroy4( &g_NellyMoserHNMS1x4 );
	g_NellyMoserHNMS1x4 = NULL;
}


// ----------------------------------------------------------------------------
const qChar *NellyMoserCodec::GetName()
{
	return _T("NellyMoser");
}


// ----------------------------------------------------------------------------
qUnsignedInt NellyMoserCodec::GetNbBytesPerEncodedFrame()
{
	return 8*m_uiNbFramesPerPackets;
}


// ----------------------------------------------------------------------------
qUnsignedInt NellyMoserCodec::GetNbSamplesPerFrame()
{
	return 160*m_uiNbFramesPerPackets;
}


// ----------------------------------------------------------------------------
Codec::EncoderState *NellyMoserCodec::CreateEncoderState()
{
	EncoderState *pState = NULL;
	ResetState(pState);
	return pState;
}


// ----------------------------------------------------------------------------
Codec::DecoderState *NellyMoserCodec::CreateDecoderState()
{
	DecoderState *pState = NULL;
	ResetState(pState);
	return pState;
}


// ----------------------------------------------------------------------------
void NellyMoserCodec::ReleaseEncoderState(Codec::EncoderState *pState)
{
	if( pState )
		free(pState);
}


// ----------------------------------------------------------------------------
void NellyMoserCodec::ReleaseDecoderState(Codec::DecoderState *pState)
{
	if( pState )
		free(pState);
}


// ----------------------------------------------------------------------------
void NellyMoserCodec::ResetState(Codec::EncoderState *pState)
{
}


// ----------------------------------------------------------------------------
void NellyMoserCodec::ResetState(Codec::DecoderState *pState)
{
}


// ----------------------------------------------------------------------------
void NellyMoserCodec::Encode( Codec::EncoderState *pState, qInt16 *pInputBuffer, qByte *pEncodedBuffer ) 
{
	short isVoiced;
	int nBytesEncoded=0;

#ifdef TIMEIT
	CTimer tCodec;
	tCodec.Start();
#endif

	for( qUnsignedInt i = 0; i < m_uiNbFramesPerPackets; i++ ) 
	{
		nBytesEncoded += nmsEncode( g_NellyMoserHNMS1x4, pInputBuffer + ( i * 160 ), pEncodedBuffer + ( i * 8 ), &isVoiced );
	}
#ifdef TIMEIT
	float encodeTime = tCodec.Stop( ) / ( float )( ( int )m_uiNbFramesPerPackets );
	if( encodeTime > maxEncodeTime )
	{
		maxEncodeTime = encodeTime;
		printf(" Encode time = %f ms, Frames = %d, BytesEncoded = %d\n", maxEncodeTime, ((int)m_uiNbFramesPerPackets), nBytesEncoded );
	}
#endif
}


// ----------------------------------------------------------------------------
void NellyMoserCodec::Decode( Codec::DecoderState *pState, qByte *pEncodedBuffer, qInt16 *pOutputBuffer )
{
	memcpy( pOutputBuffer, pEncodedBuffer, m_uiNbFramesPerPackets * 8 );
}


// ----------------------------------------------------------------------------
void NellyMoserCodec::DecodeSingleStream( qByte* pEncodedStream1, qInt16* pOutputBuffer )
{
	unsigned int nBytesConsumed;
	unsigned int nBytesProduced;

#ifdef TIMEIT
	CTimer tCodec;
	tCodec.Start();
#endif

	for( qUnsignedInt i = 0; i < m_uiNbFramesPerPackets; i++ ) 
	{
		int error = nmsDecode( g_NellyMoserHNMS1x4, pEncodedStream1 + ( i * 8 ), &nBytesConsumed, pOutputBuffer + ( i * 160 ), &nBytesProduced );
	}

#ifdef TIMEIT
	float decodeTime = tCodec.Stop( ) / ( float )( ( int )m_uiNbFramesPerPackets );
	if( decodeTime > maxDecodeTime ) 
	{
		maxDecodeTime = decodeTime;
		printf(" Decode time = %f ms, Frames = %d\n", maxDecodeTime, m_uiNbFramesPerPackets );
	}
#endif
}

// ----------------------------------------------------------------------------
void NellyMoserCodec::DecodeMultipleStreams( qByte* pEncodedStream1, qByte* pEncodedStream2, qByte* pEncodedStream3, qByte* pEncodedStream4, qInt16* pOutputBuffer )
{
	unsigned int nBytesConsumed;
	unsigned int nBytesProduced;

	for( qUnsignedInt i = 0; i < m_uiNbFramesPerPackets; i++ ) 
	{
		int error = nmsDecode4x(	g_NellyMoserHNMS1x4,
											pEncodedStream1 + ( i * 8 ), pEncodedStream2 + ( i * 8 ), pEncodedStream3 + ( i * 8 ), pEncodedStream4 + ( i * 8 ),
											&nBytesConsumed, pOutputBuffer + ( i * 160 ), &nBytesProduced );
	}
}


// ----------------------------------------------------------------------------
void NellyMoserCodec::SetVoiceMaskEffect(	int streamIndex, float fPitchWarp, float fPitchOffset, float fSpectrumWarp,
														float fSpectrumOffset, float fSpectrumLedge, float fRobot, 
														float fWhisper, int effect )
{
	ASSERTS( streamIndex < 4, "can only set effect on stream 0-3" );
	nmsSetVoiceEffect4( g_NellyMoserHNMS1x4, streamIndex, fPitchWarp, fPitchOffset, fSpectrumWarp, fSpectrumOffset, fSpectrumLedge, fRobot, fWhisper, effect );
}