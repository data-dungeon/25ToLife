/*************************************/
/*                                   */
/*   g_MiscPools.h                   */
/*   big juju prototype  06/01/99    */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   miscellaneous pool systems      */
/*                                   */
/*************************************/

#ifndef __G_MISCPOOLS_H
#define __G_MISCPOOLS_H

#include "Game/Database/DataTypes.h"	// for dh field in ts_PoolMatrix, ts_PoolFrustum
#include "Game/Misc/Frustum.h"			// for Frustum field is ts_PoolFrustum
#include "Game/Database/Instance.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/******************* structures **************************/

/* pool matrix stucture */

struct ts_PoolMatrix
{
	ts_PriDataHeader		dh;			// data header-- MUST BE FIRST-- see comments in datatypes.h
	ts_PoolMatrix			*pNext;		// linked list pointer-- does not have to be first
	void						*pOwner;
#ifdef PS2
	u8							u8Pad[4];	// matrix must be 16-byte aligned on ps2
#endif //PS2
	Graphics4x4				mMatrix;
};

/* pool frustum stucture */

struct ts_PoolFrustum
{
	ts_PoolFrustum			*pNext;		// linked list pointer-- does not have to be first
	ts_Frustum				Frustum;
};

/* pool instance stucture */

struct ts_PoolInstance
{
	ts_PoolInstance		*pNext;		// linked list pointer-- does not have to be first
	void						*pOwner;
#ifdef PS2
	u8							u8Pad[8];	// instance must be 16-byte aligned on ps2 (because it contains a matrix)
#endif //PS2
	CInstance				Instance;
};

/******************* global variables ********************/

/******************* global prototypes *******************/

int MatrixPool__Init(int nMaxMatrices);
void MatrixPool__Reset(void);
void MatrixPool__Shutdown(void);
ts_PoolMatrix *MatrixPool__Alloc(void *pOwner);
void MatrixPool__Free(ts_PoolMatrix *pMatrix);

int FrustumPool__Init(int nMaxFrustums);
void FrustumPool__Reset(void);
void FrustumPool__Shutdown(void);
ts_PoolFrustum *FrustumPool__Alloc();
void FrustumPool__Free(ts_PoolFrustum *pFrustum);

int InstancePool__Init(int nMaxInstances);
void InstancePool__Reset(void);
void InstancePool__Shutdown(void);
ts_PoolInstance *InstancePool__Alloc(void *pOwner);
void InstancePool__Free(ts_PoolInstance *pFrustum);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __G_MISCPOOLS_H
