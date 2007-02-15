/* --------------------------------------------------
 * File    : m_Quatern.h
 * Created : Thursday February 11, 1999 9:10 A.M.
 * Descript:
 * --------------------------------------------------*/

#ifndef __M_QUATERN_H
#define __M_QUATERN_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/

/* system includes */
#include "Math/Vector.h"

/* engine includes */


/* --------------------------------------------------
 * Forward declares
 * --------------------------------------------------*/

class Graphics4x4;
class Cartesian4x4;

/* --------------------------------------------------
 * Types
 * --------------------------------------------------*/

struct ts_Quatvw
{
	union {
		struct {
			Vector3Packed	V;
			float		W;
		};
#ifdef PS2
		u128 v128;
#endif
	};
};

/* --------------------------------------------------
 * Macros
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
void qatIdentity(ts_Quatvw *pQuat);
bool qatNormalize(ts_Quatvw *pQuat);

void qatFromMat(const Graphics4x4 *pMatrix, ts_Quatvw *pQuat);
void qatToRotMat(const ts_Quatvw *pQuat, Cartesian4x4 *pMat);

void qatFromEuler(float roll, float pitch, float yaw, ts_Quatvw *pQuat);
void qatFromAxisAngle(const Vector3 *pAxis, float angle, ts_Quatvw *pQuat);

void qatAdd(const ts_Quatvw *pA, const ts_Quatvw *pB, ts_Quatvw *pResult);
void qatSub(const ts_Quatvw *pA, const ts_Quatvw *pB, ts_Quatvw *pResult);
void qatMul(const ts_Quatvw *pA, const ts_Quatvw *pB, ts_Quatvw *pResult);
void qatVectMul( const ts_Quatvw *pA, const Vector3 *pB, Vector3 *pResult);

void qatSlerpBetter(ts_Quatvw *pFrom, ts_Quatvw *pTo, float t, ts_Quatvw *pResult);
void qatSlerp(ts_Quatvw *pFrom, ts_Quatvw *pTo, float t, ts_Quatvw *pResult);
void qatLerpBetter(ts_Quatvw *pFrom, ts_Quatvw *pTo, float t, ts_Quatvw *pResult);
void qatLerp(ts_Quatvw *pFrom, ts_Quatvw *pTo, float t, ts_Quatvw *pResult);

/* --------------------------------------------------
 * Override macros
 * --------------------------------------------------*/
#if defined(PS2) && defined(SN)
#define EXCLUDE_GENERIC_QUATERNION_TO_MATRIX
#define EXCLUDE_GENERIC_QUATERNION_ADD
#define EXCLUDE_GENERIC_QUATERNION_SUBTRACT
#define EXCLUDE_GENERIC_QUATERNION_NORMALIZE

#include "Math/PS2/Quaternsn.hpp"
#endif

#endif __M_QUATERN_H
