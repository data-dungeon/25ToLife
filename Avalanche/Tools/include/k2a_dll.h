/* --------------------------------------------------
 * File    : K2A_DLL.H
 * Created : Mon Feb  8 15:57:48 1999
 * Descript:
 * --------------------------------------------------*/
#ifndef __K2A_DLL_H
#define __K2A_DLL_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <t_struct.h>
#include <t_som.h>
#include <t_hierar.h>
/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/
typedef void (*stat_func)( char * pPri, char * pAlt, int iProgress);

/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/
#define IMP_TEXNAMES					0x00000001

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
#ifdef _MSC_VER

#ifdef __cplusplus
extern "C" {
#endif
 __declspec( dllexport) bool _cdecl _K2ARead( const char *pcFName, ts_SOMUP **ppModel, ts_CSList *pTexs, ts_Hierarchy **ppBones, stat_func pStatFunc);
 __declspec( dllexport) void _cdecl _K2AFree( ts_SOMUP *pModel, ts_CSList *pTexs, ts_Hierarchy *pBones);
 __declspec( dllexport) bool _cdecl _K2AReadNoBones( const char *pcFName, ts_SOMUP **ppModel, ts_CSList *pTexs, int *pNumObjs, stat_func pStatFunc);
 __declspec( dllexport) void _cdecl _K2AFreeNoBones( ts_SOMUP *pModel, ts_CSList *pTexs, ts_Hierarchy *pBones);
#ifdef __cplusplus
}
#endif

#else

extern "C" __declspec( dllexport) bool _cdecl K2ARead( const char *pcFName, ts_SOMUP **ppModel, ts_CSList *pTexs, ts_Hierarchy **ppBones, stat_func pStatFunc);
extern "C" __declspec( dllexport) void _cdecl K2AFree( ts_SOMUP *pModel, ts_CSList *pTexs, ts_Hierarchy *pBones);
extern "C" __declspec( dllexport) bool _cdecl K2AReadNoBones( const char *pcFName, ts_SOMUP **ppModel, ts_CSList *pTexs, int *pNumObjs, stat_func pStatFunc);
extern "C" __declspec( dllexport) void _cdecl K2AFreeNoBones( ts_SOMUP *pModel, ts_CSList *pTexs, ts_Hierarchy *pBones);

#endif _MSC_VER

#endif __K2A_DLL_H
