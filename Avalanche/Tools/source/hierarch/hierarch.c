/* --------------------------------------------------
 * File    : HIERARCH.C
 * Created : Fri Feb 12 11:44:07 1999
 * Descript:
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <stdio.h>
#include <malloc.h>
#include <t_macros.h>
#include <t_hierar.h>

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
void 					FreeHierarchy( ts_Hierarchy *pBone);

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
void FreeHierarchy( ts_Hierarchy *pBone)
{

	SFREE( pBone->pName);
	SFREE( pBone->pVtxIdxs);

	if( pBone->pChildren)
		FreeHierarchy( pBone->pChildren);

	if( pBone->pSiblings)
		FreeHierarchy( pBone->pSiblings);

	free( pBone);
}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
