/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library "DNAS" package (Release 3.0 version)
 */
/*
 *       Emotion Engine (I/O Processor) Library Sample Program
 *
 *                         - <do_netstart> -
 *
 *                          Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         <do_netstart.h>
 *                 <header for do_netstart function>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Feb,12,2003     komaki       first version
 */

#ifndef _DO_NETSTART_H_
#define _DO_NETSTART_H_

#ifdef __cplusplus
extern "C" {
#endif

int initialize_dnas_dll(bool proxy_state, const char *proxy_host, u_short proxy_port, int timeout);
int process_dnas_dll();
int abort_dnas_dll();

#ifdef __cplusplus
}
#endif

#endif /* _DO_NETSTART_H_ */

