#pragma once

#ifndef _SMLMATH_H
#include <smlmath.h>
#endif

class RotateEuler;

// SMLMatrix4f is a ROW MAJOR matrix class!
class Matrix44 : public SMLMatrix4f
{
public:
   Matrix44();
   Matrix44( const Matrix44 &other );
   Matrix44( float m[16] );
   Matrix44( double m[16], bool bTransposed=false );
   Matrix44( float f00, float f01, float f02, float f03,
             float f10, float f11, float f12, float f13,
             float f20, float f21, float f22, float f23,
             float f30, float f31, float f32, float f33 );

   Matrix44( const RotateEuler &re );
   ~Matrix44();

   BOOL operator == ( const Matrix44 &other ) const;
   BOOL operator != ( const Matrix44 &other ) const;
	Matrix44 &operator *= ( const Matrix44 &other );
   Matrix44 operator * (const Matrix44 &other) const;
   Vector operator * ( const Vector &other ) const;

   float GetAt(UINT col,UINT row) { return data[col][row]; }
   void SetAt(UINT col,UINT row,float fValue) { data[col][row] = fValue; }
 
   Matrix44 Transpose() const;
   Matrix44 Inverse() const;
   Matrix44 InvertRotateTranslate() const;  // THE MATRIX MUST HAVE NO SCALE

   void GetMatrix33( Matrix44 &matrix33 ) const;
   Vector GetXAxis() const;
   Vector GetYAxis() const;
   Vector GetZAxis() const;

   void GetTranslation(Vector &translate) const;
   void SetTranslation( const Vector &t );

   void GetScale(Vector &scale) const;
   void SetScale( const Vector &s );

   void GetFloatMatrix(float m[16],bool bTransposed=false) const;
   void GetDoubleMatrix(double m[16],bool bTransposed=false) const;

   void TransformPoint(const Vector &src,Vector &dst) const;

   void Decompose(Vector &translation,Vector &scale,Vector &shear,Vector &rotation) const;

   // Creation helper functions
   friend const Matrix44 &Identity44();
   friend Matrix44 Translatef( float x, float y, float z );
   friend Matrix44 TranslateV( const Vector &v );
   friend Matrix44 Scalef( float x, float y, float z );
   friend Matrix44 ScaleV( const Vector &v );
   friend Matrix44 RotateVRadians( float angle, const Vector &axis );
   friend Matrix44 Rotatef( float angle, float x, float y, float z );
   friend Matrix44 RotateV( float angle, const Vector &axis );
   friend Matrix44 Rotate2V( const Vector &from, const Vector &to );
   friend Matrix44 XRotate( float angle );
   friend Matrix44 YRotate( float angle );
   friend Matrix44 ZRotate( float angle );
   friend Matrix44 XRotateRadians( float anglerad );
   friend Matrix44 YRotateRadians( float anglerad );
   friend Matrix44 ZRotateRadians( float anglerad );
   friend Matrix44 YXRotateRadians( float yangle, float xangle );
   friend Matrix44 YXZRotateRadians( float yangle, float xangle, float zangle );
   friend Matrix44 YXZRotate( float yangle, float xangle, float zangle );
   friend Matrix44 YXZRotate( Vector angle );

   friend void TransformVectorNoTranslate( const Matrix44 &m, Vector &v );

   friend Matrix44 RotateAngleAxis(double anglerad,Vector axis);
   friend Matrix44 Rotate2VMatrix(Vector from, Vector to);
   friend Matrix44 VectorsToStandard(const Vector &becomezdir,const Vector &becomeydir);
};

#include "Matrix44.hpp"
