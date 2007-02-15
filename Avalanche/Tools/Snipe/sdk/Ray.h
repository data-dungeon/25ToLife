//

#pragma once
#include "../Utility/Vector.h"

class Plane;
class Matrix44;

class Ray 
{
public:
   Vector m_point;
   Vector m_dir;
   
   Ray() {}
   Ray( const Vector &point, const Vector &dir );
   Ray &operator *= ( const Matrix44 &matrix );
   Ray operator * ( const Matrix44 &matrix ) const;
   Vector Intersect( const Plane &plane, BOOL preventprofileplanes=TRUE );
   void IntersectSphere( const Vector &center, float radiussq, Vector &hit0, Vector &hit1 );
   BOOL IsIntersectSphere( const Vector &center, float radiussq );
};
