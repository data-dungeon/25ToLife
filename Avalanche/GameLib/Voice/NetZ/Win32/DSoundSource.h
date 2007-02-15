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

#ifndef _DSoundSource_H_
#define _DSoundSource_H_

#include <platform/core/platformdecl.h>
#include <extensions/avstreams/core/sourcedevice.h>
#include <dsound.h>
#include "DSoundSink.h"

BEGIN_NAMESPACE_QUAZAL;

class DSoundSource: public SourceDevice, public DSoundDeviceEnumerator {
public:
	DSoundSource(const AudioStreamFormat &oFormat, qUnsignedInt dwNumPackets = 3);
	~DSoundSource();

	qBool Open();
	Status Read(NS_QUAZAL qByte *pPCMBuffer, qUnsignedInt uiBufferSize = 0);
	qBool Close();
	qBool IsEmpty() { return false; }

	const AudioStreamFormat& GetFormat() const { return m_oAudioStreamFormat; }
private:
	HRESULT InitializeDirectSound();
	HRESULT CreateCaptureBuffer();

private:
    LPDIRECTSOUND8 		m_pDS;
	LPDIRECTSOUNDCAPTURE       m_pDSCapture;
    LPDIRECTSOUNDCAPTUREBUFFER m_pDSBuffer;
	qUnsignedInt				m_uiNextPacket;
	qUnsignedInt				m_uiNumPackets;
	qUnsignedInt				m_uiLastRead;
	qUnsignedInt				m_uiHighRead;
	qBool				m_bCapturing;
	qBool				m_bInitialized;
	AudioStreamFormat	m_oAudioStreamFormat;

	SOUNDDRIVERID				m_playDeviceID;
};

END_NAMESPACE_QUAZAL;

#endif
