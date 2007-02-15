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


#include "OpenLPCCodec.h"
#include <platform/core/platformimpl.h>
#include <plugins/core/bitstream.h>
#include "Voice/CodecLib/OpenLPC/openlpc.h"
#include "CTimer.h"

#ifndef CONSUMER_BUILD
#define TIMEIT 
#endif

#ifdef TIMEIT
static float maxEncodeTime=0.0f;
static float maxDecodeTime=0.0f;
#endif


OpenLPCCodec::OpenLPCCodec(qUnsignedInt uiBitRate, TimeInterval tiPacketDuration)
{
	switch (uiBitRate) {
	case 1800:
		m_uiFrameSize = OPENLPC_FRAMESIZE_1_8; 
		m_uiNbFramesPerPackets = tiPacketDuration/31;
		break;
	case 1400:
		m_uiFrameSize = OPENLPC_FRAMESIZE_1_4; 
		m_uiNbFramesPerPackets = tiPacketDuration/40;
		break;
	default:
		SYSTEMCHECK(false);
	}
	if (m_uiNbFramesPerPackets == 0) {
		m_uiNbFramesPerPackets = 1;
	}
	m_oAudioStreamFormat.SetNbSamplesPerPacket(m_uiFrameSize*m_uiNbFramesPerPackets);
}

OpenLPCCodec::~OpenLPCCodec()
{
}


const qChar *OpenLPCCodec::GetName()
{
	return _T("OpenLPC");
}

qUnsignedInt OpenLPCCodec::GetNbSamplesPerFrame()
{
	return m_uiFrameSize*m_uiNbFramesPerPackets;
}

qUnsignedInt OpenLPCCodec::GetNbBytesPerEncodedFrame()
{
	return OPENLPC_ENCODED_FRAME_SIZE*m_uiNbFramesPerPackets;
}


Codec::EncoderState *OpenLPCCodec::CreateEncoderState()
{
	EncoderState *pState = (EncoderState*)qSpecialNew(openlpc_encoder_state);
	ResetState(pState);
	return pState;
}


Codec::DecoderState *OpenLPCCodec::CreateDecoderState()
{
	DecoderState *pState = (DecoderState*)qSpecialNew(openlpc_decoder_state);
	ResetState(pState);
	return pState;
}


void OpenLPCCodec::ReleaseEncoderState(Codec::EncoderState *pState)
{
	qSpecialDelete((openlpc_encoder_state*)pState);
}


void OpenLPCCodec::ReleaseDecoderState(Codec::DecoderState *pState)
{
	qSpecialDelete((openlpc_decoder_state*)pState);
}


void OpenLPCCodec::ResetState(Codec::EncoderState *pState)
{
	init_openlpc_encoder_state((openlpc_encoder_state*)pState,m_uiFrameSize);
}


void OpenLPCCodec::ResetState(Codec::DecoderState *pState)
{
	init_openlpc_decoder_state((openlpc_decoder_state*)pState,m_uiFrameSize);
}

void OpenLPCCodec::Encode(Codec::EncoderState *pState, qInt16 *pInputBuffer, qByte *pEncodedBuffer) 
{
	int nBytesEncoded = 0;

#ifdef TIMEIT
	CTimer tCodec;
	tCodec.Start();
#endif

	for (qUnsignedInt i=0;i<m_uiNbFramesPerPackets;i++) 
	{
		nBytesEncoded += openlpc_encode(pInputBuffer+(i*m_uiFrameSize), pEncodedBuffer+(i*OPENLPC_ENCODED_FRAME_SIZE),(openlpc_encoder_state*)pState);
	}

#ifdef TIMEIT
	float encodeTime = tCodec.Stop( ) / ( float )( ( int )m_uiNbFramesPerPackets );
	if( encodeTime > maxEncodeTime )
	{
		maxEncodeTime = encodeTime;
		printf( "Encode time = %f ms, Frames = %d, BytesEncoded = %d\n", maxEncodeTime, m_uiNbFramesPerPackets, nBytesEncoded );
	}
#endif
}

void OpenLPCCodec::Decode(Codec::DecoderState *pState, qByte *pEncodedBuffer, qInt16 *pOutputBuffer)
{
#ifdef TIMEIT
	CTimer tCodec;
	tCodec.Start();
#endif

	for(qUnsignedInt i=0;i<m_uiNbFramesPerPackets;i++) 
	{
		openlpc_decode(pEncodedBuffer+(i*OPENLPC_ENCODED_FRAME_SIZE), pOutputBuffer+(i*m_uiFrameSize), (openlpc_decoder_state*)pState);
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

#ifdef TEST_ENABLED
qBool OpenLPCCodec::Test()
{
	BEGIN_TESTS;

	{
		OpenLPCCodec oCodec(1400);
		TEST(Codec::PerfTest(&oCodec));
	}
	{
		OpenLPCCodec oCodec(1800);
		TEST(Codec::PerfTest(&oCodec));
	}

	return TESTRESULT;
}
#endif
