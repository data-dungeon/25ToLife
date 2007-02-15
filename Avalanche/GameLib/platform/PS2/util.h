/****************************************************************************

 Module:
 util.h

 Description:
 Sample code to get data from mouse on Sony PlayStation(tm)2 
 using Logitech Mouse/Keyboard SDK

 COPYRIGHT (C) 2002 LOGITECH, INC. ALL RIGHTS RESERVED.

 This program is a trade secret of LOGITECH, and it is not to be
 reproduced, published, disclosed to others, copied, adapted,
 distributed or displayed without the prior authorization of LOGITECH.

 Licensee agrees to attach or embed this notice on all copies of the
 program, including partial copies or modified versions thereof.

****************************************************************************/

#ifndef INCLUDE_UTIL_H
#define INCLUDE_UTIL_H

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

const char *get_event_type(u_short event_type);
const char *get_error(int err_code);
int len_strcpy(char *pdststr, const char *psrcstr);
char *myitoa(int iVal, char *pStr);
void GetDeviceLocationString(char *pLocationString, const char *ptopology);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif


#endif

