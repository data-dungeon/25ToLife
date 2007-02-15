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

#ifndef _PS2LogitechDevices_H_
#define _PS2LogitechDevices_H_

#include <platform/core/platformimpl.h>

BEGIN_NAMESPACE_QUAZAL;

class PS2LogitechDevices: public RootObject {
	static const qInt Any = -1;
public:
	static qBool	Init(const qChar *usbd_argv = _T("conf=2048\0"), const qChar *lgaud_argv = _T("maxstream=32768"));
	static qInt 	OpenDevice(qInt iDeviceID = Any,
			 			qUnsignedInt uiNbChannels = 1, 
			 			qUnsignedInt uiNbSamplesPerSec = 8000, 
						qUnsignedInt uiNbBitsPerSample = 16);
	static 	qBool	CloseDevice(qInt hDevice);
#ifdef TEST_ENABLED
	 static qBool Test();
#endif

private:
	 static	qBool	s_bInitialized;

};


END_NAMESPACE_QUAZAL;

#endif
