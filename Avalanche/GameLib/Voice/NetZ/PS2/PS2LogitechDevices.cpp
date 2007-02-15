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

#include "PS2LogitechDevices.h"
#include <platform/core/platformimpl.h>
#include <platform/stack/ps2common/ps2module.h>
#include <liblgaud.h>

USING_NAMESPACE_QUAZAL;

qBool PS2LogitechDevices::s_bInitialized = false;

qBool PS2LogitechDevices::Init(const qChar *usbd_argv, const qChar *lgaud_argv)
{
	if (!s_bInitialized) {
		if (PS2Module::Load(_T("usbd"),(_tcslen(usbd_argv)+1)*sizeof(qChar),usbd_argv) &&
			PS2Module::Load(_T("lgaud"),(_tcslen(lgaud_argv)+1)*sizeof(qChar),lgaud_argv) &&
			(lgAudInit(NULL,NULL) == LGAUD_SUCCESS)) {
			s_bInitialized = true;
		}
	}
	return s_bInitialized;
}


qInt PS2LogitechDevices::OpenDevice(qInt iDeviceIndex, qUnsignedInt uiNbChannels, 
									qUnsignedInt uiNbSamplesPerSec, qUnsignedInt uiNbBitsPerSample)
{
	if (iDeviceIndex == Any) {
		struct lgAudDeviceDesc oDeviceDesc;
		qInt iIndex = 0;
		qInt hDevice = LGAUD_INVALID_DEVICE;
		qUnsignedInt iResult = LGAUD_SUCCESS;
		while ((iResult != LGAUD_ERR_NO_MORE_DEVICES) && (hDevice == LGAUD_INVALID_DEVICE)) {
			iResult =  (qUnsignedInt)lgAudEnumerate(iIndex,&oDeviceDesc);
			QTRACE(TRACE_NEVER,(_T("Error %x trying to enumerate index %d"),iResult, iIndex));
			if (iResult == LGAUD_SUCCESS) {
				hDevice = OpenDevice(iIndex,uiNbChannels,uiNbSamplesPerSec,uiNbBitsPerSample);
			}
			iIndex++;
		}
		return hDevice;
	} else {
		qInt hDevice = LGAUD_INVALID_DEVICE;
		lgAudOpenParam oOpenParam;

		oOpenParam.Mode = LGAUD_MODE_PLAYRECORD;

		oOpenParam.RecordingFormat.Channels = uiNbChannels;
		oOpenParam.RecordingFormat.BitResolution = uiNbBitsPerSample;
		oOpenParam.RecordingFormat.SamplingRate = uiNbSamplesPerSec;
		oOpenParam.RecordingFormat.BufferMilliseconds = 100;

		oOpenParam.PlaybackFormat.Channels = uiNbChannels;
		oOpenParam.PlaybackFormat.BitResolution = uiNbBitsPerSample;
		oOpenParam.PlaybackFormat.SamplingRate = uiNbSamplesPerSec;
		oOpenParam.PlaybackFormat.BufferMilliseconds = 500;

		qUnsignedInt iResult = lgAudOpen(iDeviceIndex, &oOpenParam, &hDevice);
		if (iResult == LGAUD_SUCCESS) {
			QTRACE(TRACE_NEVER,(_T("Device %d opened"),iDeviceIndex));
			return hDevice;
		} else { 
			QTRACE(TRACE_NEVER,(_T("Error %x opening Device %d"),iResult, iDeviceIndex));
			return LGAUD_INVALID_DEVICE;
		}
	}
}	


qBool PS2LogitechDevices::CloseDevice(qInt hDevice)
{
	return lgAudClose(hDevice) == LGAUD_SUCCESS;
}

#ifdef TEST_ENABLED

#include "PS2LogitechSource.h"

qBool PS2LogitechDevices::Test()
{
	BEGIN_TESTS

	const char usbd_argv[] = "conf=2048\0"; 
	const char lgaud_argv[] = "maxstream=32768";
	TEST(PS2Module::Load(_T("usbd"),sizeof(usbd_argv),usbd_argv));
    TEST(PS2Module::Load(_T("lgaud"),sizeof(lgaud_argv),lgaud_argv));
	TEST(lgAudInit(NULL,NULL) == LGAUD_SUCCESS);

	Platform::Sleep(2000);

	{
		lgAudOpenParam oOpenParam;

		oOpenParam.Mode = LGAUD_MODE_PLAYRECORD;
		oOpenParam.RecordingFormat.Channels = 1;
		oOpenParam.RecordingFormat.BitResolution = 16;
		oOpenParam.RecordingFormat.SamplingRate = 8000;
		oOpenParam.RecordingFormat.BufferMilliseconds = 100;
		oOpenParam.PlaybackFormat.Channels = 1;
		oOpenParam.PlaybackFormat.BitResolution = 16;
		oOpenParam.PlaybackFormat.SamplingRate = 8000;
		oOpenParam.PlaybackFormat.BufferMilliseconds = 100;

		struct lgAudDeviceDesc oDeviceDesc;
		TEST(lgAudEnumerate(0,&oDeviceDesc) == LGAUD_SUCCESS);

		qInt hDevice;
		TEST(lgAudOpen(0, &oOpenParam, &hDevice) == LGAUD_SUCCESS);

		TEST(lgAudStartRecording(hDevice) == LGAUD_SUCCESS);
		qByte pBuffer[1024];
		for (int i=0; i<30; i++) {
			qInt iSize = 180*2;
			qInt iResult = 0;
			if ((iResult = lgAudARead(hDevice, LGAUD_BLOCKMODE_BLOCKING, pBuffer, &iSize)) == LGAUD_SUCCESS) {
				if (i < 29) {
					while(true) {
						if ((iResult = lgAudASync(LGAUD_ASYNC_MODE_NOWAIT, &iResult)) == 0) {
						   QTRACE(TRACE_ALWAYS,(_T("Async Read Completed, Read %d bytes, Result = %d"),iSize, iResult));
						   break;
						} else {
							Platform::Sleep(1);
						}
					}		
				}
			} else {
			   QTRACE(TRACE_ALWAYS,(_T("Async Read Failed with error %d"), iResult));
			}
		}
		TEST(lgAudStopRecording(hDevice) == LGAUD_SUCCESS);
		Platform::Sleep(2000);

		TEST(lgAudStartRecording(hDevice) == LGAUD_SUCCESS);
		for (int i=0; i<30; i++) {
			qInt iSize = 180*2;
			qInt iResult = 0;
			if ((iResult = lgAudARead(hDevice, LGAUD_BLOCKMODE_BLOCKING, pBuffer, &iSize)) == LGAUD_SUCCESS) {
				while(true) {
					if ((iResult = lgAudASync(LGAUD_ASYNC_MODE_NOWAIT, &iResult)) == 0) {
					   QTRACE(TRACE_ALWAYS,(_T("Async Read Completed, Read %d bytes, Result = %d"),iSize, iResult));
					   break;
					} else {
						Platform::Sleep(1);
					}
				}		
			} else {
			   QTRACE(TRACE_ALWAYS,(_T("Async Read Failed with error %d"), iResult));
			}
		}
		TEST(lgAudStopRecording(hDevice) == LGAUD_SUCCESS);

		TEST(lgAudClose(hDevice) == LGAUD_SUCCESS);
	}

	{
		PS2LogitechSource oSource(AudioStreamFormat(180));

		qByte pBuffer[1024];
		TEST(oSource.Open());
		for (int i=0; i<30; i++) {
			while (oSource.Read(pBuffer) != StreamingDevice::Success) { Platform::Sleep(1); }
		}
		TEST(oSource.Close());

		TEST(oSource.Open());
		for (int i=0; i<30; i++) {
			while (oSource.Read(pBuffer) != StreamingDevice::Success) { Platform::Sleep(1); }
		}
		TEST(oSource.Close());
	}

	return TESTRESULT;
}

#endif
