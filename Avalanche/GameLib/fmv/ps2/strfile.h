/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                    - easy mpeg streaming -
 *
 *                         Version 0.20
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                           strfile.h
 *                 header file for file streaming
 *
 *       Version        Date            Design   Log
 *  --------------------------------------------------------------------
 *       0.10           02.29.2000      umemura  the first version
 *       0.20           08.21.2001      mikedean non-blocking
 */
#ifndef _STRFILE_H_
#define _STRFILE_H_

#include <eetypes.h>
#include <libcdvd.h>

// ////////////////////////////////////////////////////////////////
//
//  Structure to read data from CD/DVD or HD
//
typedef struct
{
	int 				fileID;
	int 				streamID;
	int 				size;
	bool				streamStarted;
	int				loaded;
	bool				nextTransfer;
} StrFile;

int strFileOpen(char *filename);
int strFileClose(void);
int strFileRead(void *buff, int size);

#endif // _STRFILE_H_
