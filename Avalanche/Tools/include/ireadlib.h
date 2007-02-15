/* --------------------------------------------------
 * File    : IREADLIB.H
 * Created : Tue Sep 22 13:52:57 1998
 * Descript:
 * --------------------------------------------------*/
#ifndef __IREADLIB_H
#define __IREADLIB_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <fstream.h>

#include <readbase.h>
/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/
enum
{
	RI_ERR_NONE = 0,
	RI_ERR_IREAD,
	RI_ERR_NOREADER,
	RI_ERR_UNSUPPORTED,
	RI_ERR_UNKNOWN
} ;

/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/
extern const char			*pcExtList[];

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

ifstream *GetStream( const char *pcFName);
ReadBase *GetReader( const char *pcExt);
t_error	FindImage( const char *pcBaseName, char *pFName);
t_error	GetImage( const char *pcBaseName, ts_RBImage **ppImage, t_lflagbits Flags);

#endif __IREADLIB_H
