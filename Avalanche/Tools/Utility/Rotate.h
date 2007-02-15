#pragma once

#include "Vector.h"
#include "Quaternion.h"

class Rotate {
public:   
   float    		swivel, tilt, roll;
   //                x      z     y

   Rotate() {}
   Rotate( const float scalar ) { swivel = tilt = roll = scalar; }
   Rotate( const float pswivel, const float ptilt, const float proll )
    { swivel = pswivel; tilt = ptilt; roll = proll; }
   int operator == ( const Rotate &a ) const { return swivel==a.swivel && tilt == a.tilt && roll==a.roll; }
   int operator != ( const Rotate &a ) const { return !(*this == a); }   
};

class RotateSphere;
class RotateEuler;

class RotateVector {
public:
   Vector    		      end;
   float                roll;
   
   RotateVector() {}
   RotateVector( const Vector &pend, float proll ) { end=pend; roll=proll; }
   ~RotateVector() {}
   RotateVector( const RotateSphere &sphere );
   RotateVector( const RotateEuler &euler );
   RotateVector( const Quaternion &quat );

   friend int operator != ( const RotateVector &a, const RotateVector &b )
    { return a.end!=b.end || a.roll!=b.roll; }
};

class RotateSphere {
public:
   float                latitude,
                        longitude,
                        roll;
   RotateSphere() {}
   ~RotateSphere() {}
   RotateSphere( const Vector &vec );
   RotateSphere( const RotateVector &rotateend );
   RotateSphere( const Quaternion &quat );
   RotateSphere( const Quaternion &quat, Bone *bone );
};

class RotateEuler { 
public:   
   float    		x, y, z;

   RotateEuler() {}
   ~RotateEuler() {}
   RotateEuler( float px, float py, float pz ) { x=px; y=py; z=pz; }
   RotateEuler( const Matrix44 &t );
   RotateEuler( const RotateVector &rotateend );
   RotateEuler( const Quaternion &quat );
   void ToEuler( const Matrix44 &t );
   RotateEuler operator * ( float scale ) { RotateEuler other; other.x = x * scale; other.y  = y * scale; other.z = z * scale; return other; }
   float operator[]( int i ) const
   {
      return (&x)[i];
   }
   float &operator[]( int i )
   {
      return (&x)[i];
   }
};

class TSR {
public:
   Vector  		      translate;
   Vector    		   scale;
   RotateVector		rotate;
   
   friend int operator != ( const TSR &a, const TSR &b )
    { return a.rotate!=b.rotate || a.translate!=b.translate || a.scale!=b.scale; }
   void SetDefault()
    { translate.Set(0.0f); scale.Set(1.0f); rotate.end.Set(0.0f,0.0f,1.0f); rotate.roll=0.0f; }
};

class TSQ {
public:
   Vector  		      translate;
   Vector    		   scale;
   Quaternion  		quat;
   float             roll;
   BOOL              m_isrollcomputed;
   
   TSQ(){}
   TSQ(const Vector &ptranslate, const Vector &pscale, const Quaternion &pquat)
   {
      translate = ptranslate;
      scale = pscale;
      quat = pquat;
   }
   
   int operator != ( const TSQ &b ) const
    { return quat!=b.quat || translate!=b.translate || scale!=b.scale; }
   void SetDefault()
    { translate.Set(0.0f); scale.Set(1.0f); quat.SetIdentity(); }
   TSQ &operator += ( const TSQ &other )
   {
      translate += other.translate;
      scale += other.scale;
      quat += other.quat;
      return *this;
   }
   TSQ &operator *= ( float scalar )
   {
      translate *= scalar;
      scale *= scalar;
      quat *= scalar;
      return *this;
   }
   void PostBlend()
   {
      quat.Normalize();
   }
};

#include "Rotate.hpp"