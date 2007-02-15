//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// shellCore header file
//
// $Workfile: shellCore.h $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

#ifndef _SHELL_CORE_H
#define _SHELL_CORE_H

#include <eetypes.h>
#include <eekernel.h>
#include <eestruct.h>

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include <sifdev.h>
#include <sif.h>
#include <sifrpc.h>

#include <libmtap.h>
#include <libpad.h>
#include <libgraph.h>
#include <libmc.h>
#include <libcdvd.h>

#ifdef __cplusplus
#include "Layers/Assert/Assert.h"
#endif

#define _SHELLCORE_VERSION_ 1000

#if 0

typedef float EShellResult;
#define SHELL_FAIL (0.1f)
#define SHELL_SUCCESS (1.0f)

#else

typedef enum EShellResult
{
	SHELL_FAIL = 0,
	SHELL_SUCCESS = 1,
	SHELL_INDETERMINATE = -1
} EShellResult;

#endif

#include "shellDma.h"
#include "shellFile.h"
#include "shellMemCard.h"
#include "shellPad.h"
#include "shellDisp.h"
#include "shellAssert.h"
#include "shellMemory.h"
//#include "shellvu0math.h"

#define GET_ALIGNMENT(a) ((((int)a-1)&~((int)a))+1)
#define bss_align(val)  __attribute__ ((aligned(val))) __attribute__ ((section (".bss")))
#define data_align(val) __attribute__ ((aligned(val)))
#define min(a,b) ((a<b)?a:b)
#define max(a,b) ((a>b)?a:b)

#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

void shellCoreSetIdleRoutine(void(*routine)(int),int data);
void shellCoreCallIdleRoutine();
void shellCoreAsciiToJis(unsigned char* dest, const char* string);
int shellCoreJisToAscii(char* destination, const unsigned char* string);
int shellCoreAddDmacHandler(int cause,int (*handler)(int ca,void*arg,void*addr),int next,void* arg);
int shellCoreAddIntcHandler(int cause,int (*handler)(int ca,void*arg,void*addr),int next,void* arg);
void shellCoreRemoveIntcHandler(int cause,int id);
void shellCoreRemoveDmacHandler(int cause,int id);

#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif
