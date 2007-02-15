////////////////////////////////////////////////////////////////////////////
//
// TupMatrix
//
// classes to handle matrices
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupMatrix.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/11/03    Time: 4:06p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/08/03    Time: 4:59p
 * Updated in $/Avalanche/tools/Tupper3d
 */

#ifndef TUPPER3D_TUP_MATRIX_H
#define TUPPER3D_TUP_MATRIX_H

#ifndef TUPPER3D_TUP_VERTEX_H
#include "TupVertex.h"
#endif

template<class TYPE>
class TupMatrix4x4
{
public:
	TYPE _11,_21,_31,_41; // these are in row,column names in opengl format
	TYPE _12,_22,_32,_42; // in opengl the order is row first
	TYPE _13,_23,_33,_43;
	TYPE _14,_24,_34,_44;

	TupMatrix4x4(void);
	TupMatrix4x4( const TupMatrix4x4 &s );  // copy constructor
	TupMatrix4x4& operator=( const TupMatrix4x4 &s );  // assignment operator
	bool Inverse(TupMatrix4x4 &inv);
	void Transpose(TupMatrix4x4 &result);
	void Identity(void);
	void ResetRotationScale(void);
	void SafeMultiply(TupMatrix4x4 &by,TupMatrix4x4 &destMatrix);
	void Multiply(TupMatrix4x4 &by,TupMatrix4x4 &destMatrix);
	void TransformVertices(TupArray<TupVertex3<TYPE> > &vertexArray);
	void TransformVertex(TupVertex3<TYPE> &vertex);
	void TransformNormals(TupArray<TupVertex3<TYPE> > &normalsArray);
	void TransformNormal(TupVertex3<TYPE> &vertex);
	void Rotate(TYPE angle,TupVertex3<TYPE> axis,TupMatrix4x4 &destMatrix);
	void RotateTo(TupVertex3<TYPE> sourceVector,TupVertex3<TYPE> destVector, TupMatrix4x4 &destMatrix);
	void Translate(TupVertex3<TYPE> vector,TupMatrix4x4 &destMatrix);
	void Scale(TupVertex3<TYPE> vector,TupMatrix4x4 &destMatrix);
	void ResetComponents(unsigned long flags);
	void Shear(TupVertex3<TYPE> vector,TupMatrix4x4 &destMatrix);
	void SetXRotation(TYPE angle);
	void SetYRotation(TYPE angle);
	void SetZRotation(TYPE angle);
	void RotateX(TYPE angle,TupMatrix4x4 &destMatrix);
	void RotateY(TYPE angle,TupMatrix4x4 &destMatrix);
	void RotateZ(TYPE angle,TupMatrix4x4 &destMatrix);
	void SetAxes(TupVertex3<TYPE> xAxis,TupVertex3<TYPE> yAxis,TupVertex3<TYPE> zAxis);
	bool IsIdentity(void) const;
	bool Compare( const TupMatrix4x4 &s,TYPE tolerance = (TYPE)0) const;
private:
	// Return the det of a 2x2
	inline static TYPE Det2x2(TYPE _11, TYPE _12, TYPE _21, TYPE _22);
	// Return the det of a 3x3
	inline static TYPE Det3x3(TYPE _11, TYPE _12, TYPE _13, 
										TYPE _21, TYPE _22, TYPE _23, 
										TYPE _31, TYPE _32, TYPE _33);
	// Return the det of a 4x4
	inline TYPE Det4x4(void);
	// Make the adjunct matrix
	inline void Adj4x4(TupMatrix4x4 &result);

};

typedef TupMatrix4x4<float> TupMatrix4x4f;
typedef TupMatrix4x4<double> TupMatrix4x4d;

class TupMatrix4x4Utils
{
public:
	static TupMatrix4x4f Convert(TupMatrix4x4d source);
	static TupMatrix4x4d Convert(TupMatrix4x4f source);
};


// here is the implemtation
#include "TupMatrix.hpp"

#endif