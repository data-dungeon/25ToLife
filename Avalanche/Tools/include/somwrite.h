/* --------------------------------------------------
 * File    : \TOOLS\INCLUDE\SOMWRITE.H
 * Created : Wed Sep 16 17:14:17 1998
 * Descript:
 * --------------------------------------------------*/
#ifndef __SOMWRITE_H
#define __SOMWRITE_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <t_types.h>
#include <l_som.h>

/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/
class SOMWrite
{
public:
	virtual ts_SOMUP		ExportUnpacked( t_lflagbits ExportFlags) = 0;
	virtual ts_SOM			ExportPacked( t_lflagbits ExportFlags) = 0;
} ;

/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

#endif __SOMWRITE_H
