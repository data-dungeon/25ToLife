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

#include "PS2LogitechSource.h"
#include <platform/core/platformimpl.h>
#include "PS2LogitechDevices.h"
#include <liblgaud.h>
#include "cmdcon/cmdcon.h"

#define NUM_PLAY_NOTIFICATIONS  16

USING_NAMESPACE_QUAZAL;

PS2LogitechSource::PS2LogitechSource(const AudioStreamFormat &oFormat, qInt iDeviceIndex):
					m_oAudioStreamFormat(oFormat)
#ifdef AVSTREAMS_PROFILING
					,m_oPURead(_T("Headset Read"),5000)
#endif
{
	m_pBuffer = (qByte*)MemoryManager::Allocate(GetFormat().GetPacketSize());
	m_iDeviceIndex = iDeviceIndex;
#ifdef AVSTREAMS_PROFILING
	m_oPURead.EnableLogging(true);
#endif
}


PS2LogitechSource::~PS2LogitechSource() 
{
	MemoryManager::Free(m_pBuffer);
}


qBool PS2LogitechSource::Open()
{
	m_hDevice = PS2LogitechDevices::OpenDevice(m_iDeviceIndex,
												GetFormat( ).GetNbChannels( ), 
												GetFormat( ).GetNbSamplesPerSecond( ), 
												GetFormat( ).GetNbBitsPerSample( ) );

	SYSTEMCHECK(!m_bOpen);
	if (m_hDevice != LGAUD_INVALID_DEVICE) {
		m_iReadSize = 0;
		if (lgAudStartRecording(m_hDevice) == LGAUD_SUCCESS) {
			m_bOpen = true;
			return true;
		}
	}
	return false;
}


StreamingDevice::Status PS2LogitechSource::Read(unsigned char *pSound, qUnsignedInt uiBufferSize)
{
	if (uiBufferSize == 0) {
		uiBufferSize = GetFormat().GetPacketSize();
	}
	SYSTEMCHECK(uiBufferSize == GetFormat().GetPacketSize());
	SYSTEMCHECK(m_hDevice != LGAUD_INVALID_DEVICE);
	SYSTEMCHECK(m_bOpen);
	
	SYSTEMCHECK(m_iReadSize < (qInt)uiBufferSize);
	qInt iReadSize = uiBufferSize - m_iReadSize;

	qInt iResult;
	{
#ifdef AVSTREAMS_PROFILING
		ProfilingUnit::Scope oPUS(m_oPURead);
#endif
		iResult = lgAudRead(m_hDevice, LGAUD_BLOCKMODE_NOT_BLOCKING, m_pBuffer + m_iReadSize, &iReadSize);
//		iResult = lgAudRead(m_hDevice, LGAUD_BLOCKMODE_BLOCKING, m_pBuffer + m_iReadSize, &iReadSize);

	}
	if (iResult == LGAUD_SUCCESS) {
		m_iReadSize += iReadSize;
		if (m_iReadSize == (qInt)uiBufferSize) {
			memcpy(pSound,m_pBuffer,uiBufferSize);
			m_iReadSize = 0;
			return Success;
		} else {
			return NotReady;
		}
	} else {
		QTRACE(TRACE_ALWAYS,(_T("Error %d reading PS2LogitechSource"),iResult));
		return Error;
	}
	return Error;
}


qBool PS2LogitechSource::Close()
{
	SYSTEMCHECK(m_bOpen);
	SYSTEMCHECK(m_hDevice != LGAUD_INVALID_DEVICE);
	lgAudStopRecording(m_hDevice);
	PS2LogitechDevices::CloseDevice(m_hDevice);
	m_bOpen = false;
	return true;
}

