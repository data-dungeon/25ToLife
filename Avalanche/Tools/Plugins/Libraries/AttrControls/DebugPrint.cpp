//
//	DebugPrint.cpp
//	

/*
 * $History: DebugPrint.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/19/02   Time: 2:22p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 */

#include "stdafx.h"
#include "DebugPrint.h"


void ATDebugPrint(const TCHAR *format, ...) 
{
	TCHAR buf[512];
	va_list args;
	va_start(args,format);
	_vsntprintf(buf,512,format,args);
	va_end(args);
	OutputDebugString(buf);
}
