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


#include "GSMCodec.h"
#include <platform/core/platformimpl.h>
#include "Voice/CodecLib/GSM/gsm.h"
#include "Voice/CodecLib/GSM/private.h"
#include "CTimer.h"

#ifndef CONSUMER_BUILD
#define TIMEIT
#endif

#ifdef TIMEIT
static float maxEncodeTime=0.0f;
static float maxDecodeTime=0.0f;
#endif


GSMCodec::GSMCodec(TimeInterval tiPacketDuration)
{
	m_uiNbFramesPerPackets = tiPacketDuration/20;
	if (m_uiNbFramesPerPackets == 0) {
		m_uiNbFramesPerPackets = 1;
	}
	m_oAudioStreamFormat.SetNbSamplesPerPacket(160*m_uiNbFramesPerPackets);
}

GSMCodec::~GSMCodec()
{
}


const qChar *GSMCodec::GetName()
{
	return _T("GSM");
}



qUnsignedInt GSMCodec::GetNbBytesPerEncodedFrame()
{
	return 33*m_uiNbFramesPerPackets;
}


qUnsignedInt GSMCodec::GetNbSamplesPerFrame()
{
	return 160*m_uiNbFramesPerPackets;
}


Codec::EncoderState *GSMCodec::CreateEncoderState()
{
	EncoderState *pState = (EncoderState*)malloc(sizeof(gsm_state)); //qSpecialNew(gsm_state);
	ResetState(pState);
	return pState;
}


Codec::DecoderState *GSMCodec::CreateDecoderState()
{
	DecoderState *pState = (DecoderState*)malloc(sizeof(gsm_state)); //qSpecialNew(gsm_state);
	ResetState(pState);
	return pState;
}


void GSMCodec::ReleaseEncoderState(Codec::EncoderState *pState)
{
	free(pState);
}


void GSMCodec::ReleaseDecoderState(Codec::DecoderState *pState)
{
	free(pState);
}


void GSMCodec::ResetState(Codec::EncoderState *pState)
{
	gsm_state_init((gsm_state*)pState);
}

void GSMCodec::ResetState(Codec::DecoderState *pState)
{
	gsm_state_init((gsm_state*)pState);
}


void GSMCodec::Encode(Codec::EncoderState *pState, qInt16 *pInputBuffer, qByte *pEncodedBuffer) 
{
	int nBytesEncoded = 0;

#ifdef TIMEIT
	CTimer tCodec;
	tCodec.Start();
#endif

	for (qUnsignedInt i=0;i<m_uiNbFramesPerPackets;i++) 
	{
		nBytesEncoded += gsm_encode((gsm_state*)pState, pInputBuffer+(i*160), pEncodedBuffer+(i*33));
	}

#ifdef TIMEIT
	float encodeTime = tCodec.Stop( ) / ( float )( ( int )m_uiNbFramesPerPackets);
	if( encodeTime > maxEncodeTime )
	{
		maxEncodeTime = encodeTime;
		printf( "Encode time = %f ms, Frames = %d, BytesEncoded = %d\n", maxEncodeTime, m_uiNbFramesPerPackets, nBytesEncoded );
	}
#endif

}


void GSMCodec::Decode(Codec::DecoderState *pState, qByte *pEncodedBuffer, qInt16 *pOutputBuffer)
{
#ifdef TIMEIT
	CTimer tCodec;
	tCodec.Start();
#endif

	for (qUnsignedInt i=0;i<m_uiNbFramesPerPackets;i++) 
	{
		gsm_decode((gsm_state*)pState, pEncodedBuffer+(i*33), pOutputBuffer+(i*160));
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
qBool GSMCodec::Test()
{
	BEGIN_TESTS;

	GSMCodec oCodec;
	TEST(Codec::PerfTest(&oCodec));

	return TESTRESULT;
}
#endif

