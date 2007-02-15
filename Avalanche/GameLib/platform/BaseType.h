/*************************************/
/*                                   */
/*   BaseType.h                      */
/*   new game engine  02/17/00       */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   basic data types                */
/*                                   */
/*************************************/

/* basic types for the game engine. this file may vary from processor to processor to satisfy bit width requirements */

#ifndef P_BASETYPE_H
#define P_BASETYPE_H

#include <stddef.h>	// system standard types
#ifndef GCN
#include <sys/types.h>	// more system types
#endif

#if defined (_XBOX)
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#endif

#if defined (WIN32) && !defined (_XBOX)
#include <windows.h>	// for LARGE_INTEGER
#endif

#ifdef PS2
#include <eetypes.h>	// more system types
#ifdef CW
#pragma fast_fptosi on	//will generate cvt.w.s instead of fptosi()
#pragma float_constants on	//forces all constants to be single point
#endif CW
#if defined(PS2) && defined(SN)
#include <sn_registers.h>
#endif
#endif //PS2
#ifdef GCN
#include <Dolphin/types.h>
#endif

#if !defined(NOLAYERS)
#include "Layers/Assert/Assert.h"
#else
#include <Assert.h>
#define ASSERT(x) assert(x)
#endif

#if defined ( __cplusplus )
extern "C" {
#endif

// basic integer types-- use these instead of short, int, long to guarantee bit widths
typedef signed char		s8;
typedef unsigned char	u8;
typedef short				s16;
typedef unsigned short	u16;
#ifdef WIN32
typedef long				s32;
typedef unsigned long		u32;
typedef __int64				s64;
typedef unsigned __int64	u64;
#elif defined(PS2)
typedef int					s32;
typedef unsigned int		u32;
typedef long				s64;		// on PS2, long is 64 bits (idiots!)
typedef unsigned long	u64;		// on PS2, long is 64 bits (idiots!)
typedef u_long128			u128;
typedef long128			s128;
#endif //PS2

// Some basic integer types
typedef signed char		int8;
typedef unsigned char	uint8;
typedef short				int16;
typedef unsigned short	uint16;
#ifdef WIN32
typedef long				int32;
typedef unsigned long	uint32;
#elif defined(PS2)
typedef int					int32;
typedef unsigned int		uint32;
#elif defined(GCN)
typedef long				int32;
typedef unsigned long	uint32;
#endif //PS2

// Some basic floating point
typedef float				float32;
typedef double				float64;

// Conveniences
typedef uint8				byte;
#ifdef WIN32	// on PS2, declared in types.h
typedef unsigned int		uint;
typedef unsigned short	ushort;
#endif //WIN32
#ifdef GCN	// on PS2, declared in types.h
typedef unsigned int		uint;
typedef unsigned short	ushort;

typedef unsigned int		u_int;

#define CHECK_ALIGN_32B(buffer) ASSERT( ( (u32)( buffer) & 0x1f) == 0)

#endif //WIN32


// Some nice constants
#ifndef MAXINT
#define MAXINT		  		(0x7fffffff)
#endif

#define MAXUINT	  		(0xffffffff)

#define MINFLOAT32		(3.39e-38f)
#define MAXFLOAT32		(3.39e38f)

#define MINFLOAT32	  	(3.39e-38f)
#define MAXFLOAT32		(3.39e38f)

#define MINFLOAT64 		(1.8e-308)
#define MAXFLOAT64 		(1.8e308)

#define MINFLOAT	  		MINFLOAT32
#define MAXFLOAT	  		MAXFLOAT32

#ifndef TRUE
#define TRUE (1==1)
#endif
#ifndef FALSE
#define FALSE (0==1)
#endif
#ifndef true
#define true (1==1)
#endif
#ifndef false
#define false (0==1)
#endif

/* bool type-- defined for c++ */

#ifndef __cplusplus
typedef unsigned char bool;
#endif //__cplusplus

/* alignment */

/*** for pc ***/

#define SIMD_ALIGN					16		// simd instructions require 16-byte alignment-- 4 floats

/* check alignment */

#define IS_SIMD_ALIGNED(expr)		(((int) expr & (SIMD_ALIGN - 1)) == 0)

/*** for ps2 ***/

#define BYTES_PER_QUAD_WORD		16		// a quad word is 4 32-bit words, or 128 bits, or 16 bytes
#define WORDS_PER_QUAD_WORD		4
#define DMA_ALIGN						16
#define VU_ALIGN						16
#define CACHE_ALIGN					64		// cache lines are 64 bytes

/*** for sn systems ***/
#if defined(PS2) && defined(SN)
// sn systems doesn't support the #pragma pack(n) directive,
// so, use this define instead _ON EACH MEMBER TO BE PACKED_
#define PRAGMAPACK	__attribute__ ((packed))
#else
// looks like all the other compilers support #pragma pack(n),
// so make this a nop for them.
#define PRAGMAPACK
#endif

/* check alignment */

#define IS_DMA_ALIGNED(expr)		(((int) expr & (DMA_ALIGN - 1)) == 0)
#define IS_VU_ALIGNED(expr)		(((int) expr & (VU_ALIGN - 1)) == 0)
#define IS_CACHE_ALIGNED(expr)	(((int) expr & (CACHE_ALIGN - 1)) == 0)

/* for ps2 vram snapshots */

#define PSMCT16_VRAM_PAGE_WIDTH	64	// in pixels
#define PSMCT16_BYTES_PER_PIXEL	2

/* fomr old Avalanche\GameLib\Basetype.h */

#ifdef WIN32
#ifndef CDECL
#define CDECL __cdecl
#endif
#define ALIGNVU
#define ALIGNDMA
#else
#define CDECL
#define ALIGNVU		__attribute__((aligned(16)))
#define ALIGNDMA     __attribute__((aligned(64)))
#endif

#if defined(WIN32)
#define CODERESIDENT_DATA_ALIGN
#elif defined(PS2)
#define CODERESIDENT_DATA_ALIGN	__attribute__((aligned(32)))
#elif defined(GCN)
#define CODERESIDENT_DATA_ALIGN	ATTRIBUTE_ALIGN(32)
#endif

// Stuff to help with alignment
#define Align_Mod16(s)		((((s) ^ 0xf) + 1) & 0xf)
#define Align_RoundTo16(s)	(s + Align_Mod16(s))
#define Align_Mod4(s)		((((s) ^ 0x3) + 1) & 0x3)
#define Align_RoundTo4(s)	(s + Align_Mod4(s))

// These MUST BE GIVEN A POWER OF 2 OR THEY WILL NOT WORK!
#define Align_ModN(s, N)		((((s) ^ ((N) - 1)) + 1) & ((N) - 1))
#define Align_RoundToN(s, N)	(s + Align_ModN(s, N))

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#define ARRAYOF(k) (sizeof(k)/sizeof((k)[0]))

#if defined ( __cplusplus )
}
#endif

#endif //P_BASETYPE_H
