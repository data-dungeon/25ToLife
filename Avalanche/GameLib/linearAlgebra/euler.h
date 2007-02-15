/***************************************************************************/
// some utilties for dealing with Euler angles
//
// NOTE ALL ANGLES ARE IN DEGREES NOT RADIANS
//
// This whole thing would have been cool as a template....
/***************************************************************************/
#ifndef EULER_H
#define EULER_H

#include "Math/Matrix.h"

// everything is done in x,y,z order, for now

class EulerXYZUtility
{
public:
	static inline void Get3x3(Vector3CRef euler, DirCos3x3 &m);
	static inline void Get4x4(Vector3CRef euler, Cartesian4x4 &m);

	static inline Vector3 GetDir(Vector3CRef euler);
	static inline Vector3 GetUp(Vector3CRef euler);
	static inline Vector3 GetRight(Vector3CRef euler);

	static inline void GetRUD(Vector3CRef euler, Vector3 &right, Vector3 &up, Vector3 &dir);
};

// A yxz implementation

class EulerYXZUtility
{
public:
	static inline void Get3x3(Vector3CRef euler, DirCos3x3 &m);
	static inline void Get4x4(Vector3CRef euler, Cartesian4x4 &m);

	static inline Vector3 GetDir(Vector3CRef euler);
	static inline Vector3 GetUp(Vector3CRef euler);
	static inline Vector3 GetRight(Vector3CRef euler);

	static inline void GetRUD(Vector3CRef euler, Vector3 &right, Vector3 &up, Vector3 &dir);
};

// include the implementation
#include "euler.hpp"

#endif
