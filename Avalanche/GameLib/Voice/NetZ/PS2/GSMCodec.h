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


#ifndef _GSMCodec_H
#define _GSMCodec_H

#include <platform/core/platformdecl.h>
#include <extensions/avstreams/core/codec.h>

USING_NAMESPACE_QUAZAL;

//group=Supplied Codecs

/*
Description: This class wraps the GSM (Global System for Mobile communications) codec to the $PRODUCTNAME$ 
API.
Remarks: The supplied codec is part of the HawkVoice Direct Interface (HVDI) cross platform network voice library. 
Refer to {hyperlink:www.hawksoft.com|http://www.hawksoft.com} for further details about the codecs supplied with the software.  
*/
class GSMCodec: public Codec {
	public:
		GSMCodec(TimeInterval tiPacketDuration = 20);
		virtual ~GSMCodec();

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

#ifdef TEST_ENABLED
		// {secret}
		static qBool Test();
#endif

	private:
		qUnsignedInt32	m_uiNbFramesPerPackets;
};


#endif




