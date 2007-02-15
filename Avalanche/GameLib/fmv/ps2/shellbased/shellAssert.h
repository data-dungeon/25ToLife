//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// Macros for your debugging pleasure
//
// $Workfile: shellAssert.h $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------


#ifdef __cplusplus
extern "C" {
#endif

#undef WARNING
#undef PRINT

#ifndef __cplusplus
#undef ASSERTS
#define LOCKUP() asm __volatile__("break 1;")
#endif

#ifdef NDEBUG

#if __GNUC__ == 2 && __GNUC_MINOR__ <= 95
#define PRINT(A...) ((void)0)
#define WARNING(e,F...) ((void)0)
#ifndef __cplusplus
#define ASSERTS(e,...) ((void)0)
#define ASSERTF(e,f) ((void)0)
#endif
#else
#define PRINT(...) ((void)0)
#define WARNING(e,...) ((void)0)
#ifndef __cplusplus
#define ASSERTS(e,...) ((void)0)
#endif
#endif

#else

#if __GNUC__ == 2 && __GNUC_MINOR__ <= 95
#define PRINT(A...) do{scePrintf("%s(%d) : ",__FILE__,__LINE__); scePrintf(##A); scePrintf("\n");} while(0)
#define WARNING(e,F...) do{if(!(e)) {scePrintf("Warning in \"%s\" at line %d\n(%s) == FALSE\n", __FILE__, __LINE__, #e); scePrintf(##F);scePrintf("\n");}} while(0)
#ifndef __cplusplus
#define ASSERTS(e,F...) do{if(!(e)) {scePrintf("Assert in \"%s\" at line %d\n(%s) == FALSE \n", __FILE__, __LINE__, #e); scePrintf(##F);scePrintf("\n"); LOCKUP();}} while(0)
#define ASSERTF(e,F)    do{if(!(e)) {scePrintf("Assert in \"%s\" at line %d\n(%s) == FALSE \n", __FILE__, __LINE__, #e); scePrintf F ;  scePrintf("\n"); LOCKUP();}} while(0)
#endif
#else
#define PRINT(...) do{scePrintf("%s(%d) : ",__FILE__,__LINE__); scePrintf(__VA_ARGS__);scePrintf("\n");} while(0)
#define WARNING(e,...) do{if(!(e)) {scePrintf("Warning in \"%s\" at line %d\n(%s) == FALSE\n", __FILE__, __LINE__, #e); scePrintf(__VA_ARGS__);scePrintf("\n");}} while(0)
#ifndef __cplusplus
#define ASSERTS(e,...) do{if(!(e)) {scePrintf("Assert in \"%s\" at line %d\n(%s) == FALSE \n", __FILE__, __LINE__, #e); scePrintf(__VA_ARGS__);scePrintf("\n"); LOCKUP();}} while(0)
#define ASSERTF(e,F)   do{if(!(e)) {scePrintf("Assert in \"%s\" at line %d\n(%s) == FALSE \n", __FILE__, __LINE__, #e); scePrintf F ;          scePrintf("\n"); LOCKUP();}} while(0)
#endif
#endif


#endif /* NDEBUG */

#ifdef __cplusplus
}
#endif
