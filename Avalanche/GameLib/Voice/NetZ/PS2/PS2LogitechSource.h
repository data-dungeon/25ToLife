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

#ifndef _PS2LogitechSource_H_
#define _PS2LogitechSource_H_

#include <extensions/avstreams/core/sourcedevice.h>

BEGIN_NAMESPACE_QUAZAL;

class PS2LogitechSource: public SourceDevice {
public:
	static const int AnyDevice = -1;
public:
	PS2LogitechSource(const AudioStreamFormat &oFormat, qInt iDeviceIndex = AnyDevice);
	virtual ~PS2LogitechSource();

	 qBool 			Open();
	 Status		 	Read(qByte *pPCMBuffer, qUnsignedInt uiBufferSize = 0);
	 qBool 			Close();
	 qBool 			IsEmpty() { return false; }

	 qInt			GetDeviceHandle() { return m_hDevice; }

	const 			AudioStreamFormat& GetFormat() const { return m_oAudioStreamFormat; }
private:
	 qBool			OpenDevice(qInt iDeviceID);

private:
	 qInt			m_hDevice;
	 qInt			m_iDeviceIndex;
	 qInt			m_iReadSize;
	 qByte			*m_pBuffer;
	 AudioStreamFormat	m_oAudioStreamFormat;

#ifdef AVSTREAMS_PROFILING
	 ProfilingUnit		m_oPURead;
#endif
};

END_NAMESPACE_QUAZAL;

#endif
