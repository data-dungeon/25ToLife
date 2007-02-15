/* --------------------------------------------------
 * File    : SOM_WRIT.H
 * Created : Thu Oct  8 15:24:33 1998
 * Descript:
 * --------------------------------------------------*/
#ifndef __SOM_WRIT_H
#define __SOM_WRIT_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <t_som.h>
#include <mpostr.h>
/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
mpostream & WritePSXModel1(mpostream &str, ts_SOM &Model, int iHdrPadBytes);
mpostream & WriteN64Model1(mpostream &str, ts_SOM &Model, int iHdrPadBytes);
mpostream & operator << (mpostream &str, ts_SOM &Model);

#endif __SOM_WRIT_H
