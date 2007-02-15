/* --------------------------------------------------
 * File    : IREADDLL.H
 * Created : Mon Feb  8 15:57:48 1999
 * Descript:
 * --------------------------------------------------*/
#ifndef __IREADDLL_H
#define __IREADDLL_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <t_struct.h>
#include <t_som.h>

/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/
// GetRGB flags
#define RB_GET_RGB			0x00000000
#define RB_GET_RGBA			0x00000001
#define RB_GET_PACKED555	0x00000002
#define RB_GET_PACKED565	0x00000003
#define RB_GET_RGB_M			0x00000004
#define RB_GET_PACKED_M		0x00000005
#define RB_GET_INTERNAL		0x00000007

#define RB_GET_MASK			0x00000007

typedef void (*stat_func)( char * pPri, char * pAlt, int iProgress);

/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/
#define IMP_TEXNAMES					0x00000001

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
#ifdef _MSC_VER

extern "C" __declspec( dllexport) t_error _cdecl _FindImage( const char *pcBaseName, char *pName);
extern "C" __declspec( dllexport) t_error _cdecl _GetImage( const char *pcBaseName, ts_RBImage **ppImage, t_lflagbits lFlags);

#else

extern "C" __declspec( dllexport) t_error _cdecl FindImage( const char *pcBaseName, char *pName);
extern "C" __declspec( dllexport) t_error _cdecl GetImage( const char *pcBaseName, ts_RBImage **ppImage, t_lflagbits lFlags);

#endif _MSC_VER

#endif __IREADDLL_H
