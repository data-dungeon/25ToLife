// ----------------------------------------------------------------------------
// File			-> NellyMoserCodec.h
// Created		-> June 21, 2004
// Author		-> Dwain Skinner
// Description	-> Derived class for use with the NellyMoser Codec library under NetZ
// ----------------------------------------------------------------------------


#ifndef _NellyMoserCodec_H
#define _NellyMoserCodec_H

#include <platform/core/platformdecl.h>
#include <extensions/avstreams/core/codec.h>

USING_NAMESPACE_QUAZAL;

class NellyMoserCodec: public Codec 
{
public:
	NellyMoserCodec(TimeInterval tiPacketDuration = 20);
	virtual ~NellyMoserCodec();

	virtual const qChar *GetName();
	virtual qUnsignedInt GetNbBytesPerEncodedFrame();
	virtual qUnsignedInt GetNbSamplesPerFrame();

	virtual EncoderState *CreateEncoderState();
	virtual DecoderState *CreateDecoderState();
	virtual void ReleaseEncoderState(EncoderState *pState);
	virtual void ReleaseDecoderState(DecoderState *pState);
	virtual void ResetState(EncoderState *pState);
	virtual void ResetState(DecoderState *pState);

	virtual void Encode(EncoderState *pState, qInt16 *pInputBuffer, qByte *pEncodedBuffer);
	virtual void Decode(DecoderState *pState, qByte *pEncodedBuffer, qInt16 *pOutputBuffer);

	virtual void SetVoiceMaskEffect(	int index, float fPitchWarp, float fPitchOffset, float fSpectrumWarp,
												float fSpectrumOffset, float fSpectrumLedge, float fRobot, 
												float fWhisper, int effect );
	virtual qUnsignedInt32 GetNumFramesPerPacket( ) { return m_uiNbFramesPerPackets; }
	virtual void DecodeMultipleStreams( qByte* pEncodedStream1, qByte* pEncodedStream2, qByte* pEncodedStream3, qByte* pEncodedStream4, qInt16* pOutputBuffer );
	virtual void DecodeSingleStream( qByte* pEncodedStream1, qInt16* pOutputBuffer );

private:
	qUnsignedInt32	m_uiNbFramesPerPackets;
};

#endif




