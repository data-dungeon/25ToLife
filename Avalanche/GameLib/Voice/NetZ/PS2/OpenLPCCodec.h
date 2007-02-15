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


#ifndef _OpenLPCCodec_H
#define _OpenLPCCodec_H

#include <platform/core/platformdecl.h>
#include <extensions/avstreams/core/codec.h>
#include <plugins/core/bitstream.h>
#include "Voice/CodecLib/OpenLPC/openlpc.h"

USING_NAMESPACE_QUAZAL;

//group=Supplied Codecs

/*
Description: This class wraps the OpenLPC codec to the $PRODUCTNAME$ API.
Remarks: The supplied codec is part of the HawkVoice Direct Interface (HVDI) cross platform network voice library. 
Refer to {hyperlink:www.hawksoft.com|http://www.hawksoft.com} for further details about the codecs supplied with the software.  
*/
class OpenLPCCodec: public Codec {
	public:

		/*
		Parameters:
		uiBitRate: The bit rate of this codec may be either 1800 or 1400. The default value is 1800 bits per second.
		tiPacketDuration: The packet duration, which is the length of time in the audio stream that a packet 
		represents. The default value is 31 ms.
		Remarks: Unless you are running at a relatively high frame rate (in the order of 30 fps) you may not 
		be able to call VoiceChannel::Dispatch more frequently than the default packet duration of this Codec.
		This is likely to happen during development so to avoid choppy sound you should increase the packet 
		duration when you construct a OpenLPCCodec object. In release mode the packet duration should be reset  
		back to the default value.
		*/
		OpenLPCCodec(qUnsignedInt uiBitRate = 1800, TimeInterval tiPacketDuration = 31);
		virtual ~OpenLPCCodec();

		virtual const qChar *GetName();
		virtual qUnsignedInt GetNbSamplesPerFrame();
		virtual qUnsignedInt GetNbBytesPerEncodedFrame();

		virtual EncoderState *CreateEncoderState();
		virtual DecoderState *CreateDecoderState();
		virtual void ReleaseEncoderState(EncoderState *pState);
		virtual void ReleaseDecoderState(DecoderState *pState);
		virtual void ResetState(EncoderState *pState);
		virtual void ResetState(DecoderState *pState);

		virtual void Encode(EncoderState *pState, qInt16 *pInputBuffer, qByte *pEncodedBuffer);
		virtual void Decode(DecoderState *pState, qByte *pEncodedBuffer, qInt16 *pOutputBuffer);

#ifdef TEST_ENABLED
		// {secret}
		static qBool Test();
#endif

private:
		qUnsignedInt m_uiFrameSize;	
		qUnsignedInt m_uiNbFramesPerPackets;	
};

#endif




