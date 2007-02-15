/* --------------------------------------------------
 * File    : T_HIERAR.H
 * Created : Fri Feb 12 11:17:19 1999
 * Descript:
 * --------------------------------------------------*/
#ifndef __T_HIERAR_H
#define __T_HIERAR_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/

typedef struct Hierarchy
{
	struct Hierarchy	*pSiblings;
	struct Hierarchy	*pChildren;
	char					*pName;
	int					nVtxIdxs;
	short					*pVtxIdxs;
	void					*pMotion;
} ts_Hierarchy;

/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
#if defined __cplusplus
extern "C" {
#endif

void 					FreeHierarchy( ts_Hierarchy *pBone);

#if defined __cplusplus
}
#endif

#endif __T_HIERAR_H
