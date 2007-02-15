/* --------------------------------------------------
 * File    : DLLSTUFF.H
 * Created : Wed Feb 10 17:08:21 1999
 * Descript:
 * --------------------------------------------------*/
#ifndef __DLLSTUFF_H
#define __DLLSTUFF_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <t_som.h>
#include <t_struct.h>

/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/
typedef bool (*ReadFunc)( const char *, ts_SOMUP **, ts_CSList *, stat_func);
typedef void (*FreeFunc)( ts_SOMUP *, ts_CSList *);


/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
void				FindDrvr( char *pFName, ReadFunc *pRead, FreeFunc *pFree);
void 				FreeNames( char **ppNames, int iNames);
void 				WriteTextureListFile( char *pFileName, char **ppFileNames, char **ppTextureNames, int iNames);

char **			BuildPathedTextureNames( char **ppTextureNames, int iNames, char *pAltPath);
ts_RBImage *	ReadTextureFiles( char **ppTextureNames, int iNames);

#endif __DLLSTUFF_H
