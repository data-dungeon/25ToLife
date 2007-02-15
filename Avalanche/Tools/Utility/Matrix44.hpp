#include "Vector4.h"
#include "Vector2.h"
#include "Rotate.h"

inline Matrix44::Matrix44()
{
   Identity();
}

inline Matrix44::Matrix44( const Matrix44 &other ) :
 SMLMatrix4f( other )
{
}

inline Matrix44::Matrix44(float m[16])
{
   identity = false;
   memcpy(data,m,sizeof(float)*16);
}

inline Matrix44::Matrix44(double m[16],bool bTransposed/*=false*/)
{
   identity = false;

   if (bTransposed)
   {
      data[0][0] = (float)m[0];
      data[1][0] = (float)m[1];
      data[2][0] = (float)m[2];
      data[3][0] = (float)m[3];

      data[0][1] = (float)m[4];
      data[1][1] = (float)m[5];
      data[2][1] = (float)m[6];
      data[3][1] = (float)m[7];

      data[0][2] = (float)m[8];
      data[1][2] = (float)m[9];
      data[2][2] = (float)m[10];
      data[3][2] = (float)m[11];

      data[0][3] = (float)m[12];
      data[1][3] = (float)m[13];
      data[2][3] = (float)m[14];
      data[3][3] = (float)m[15];
   }
   else
   {
      data[0][0] = (float)m[0];
      data[0][1] = (float)m[1];
      data[0][2] = (float)m[2];
      data[0][3] = (float)m[3];

      data[1][0] = (float)m[4];
      data[1][1] = (float)m[5];
      data[1][2] = (float)m[6];
      data[1][3] = (float)m[7];

      data[2][0] = (float)m[8];
      data[2][1] = (float)m[9];
      data[2][2] = (float)m[10];
      data[2][3] = (float)m[11];

      data[3][0] = (float)m[12];
      data[3][1] = (float)m[13];
      data[3][2] = (float)m[14];
      data[3][3] = (float)m[15];
   }
}

inline Matrix44::Matrix44( float f00, float f01, float f02, float f03,
                    float f10, float f11, float f12, float f13,
                    float f20, float f21, float f22, float f23,
                    float f30, float f31, float f32, float f33 )
{
   identity = false;

   data[0][0] = f00;
   data[0][1] = f01;
   data[0][2] = f02;
   data[0][3] = f03;

   data[1][0] = f10;
   data[1][1] = f11;
   data[1][2] = f12;
   data[1][3] = f13;

   data[2][0] = f20;
   data[2][1] = f21;
   data[2][2] = f22;
   data[2][3] = f23;

   data[3][0] = f30;
   data[3][1] = f31;
   data[3][2] = f32;
   data[3][3] = f33;
}

inline Matrix44::Matrix44( const RotateEuler &re )
{
   identity = false;

   *this = YXZRotate( re.y, re.x, re.z );
}

inline Matrix44::~Matrix44()
{
}

inline void Matrix44::GetMatrix33( Matrix44 &matrix33 ) const
{
   matrix33 = *this;

   matrix33.data[0][3] = 0.0;
   matrix33.data[1][3] = 0.0;
   matrix33.data[2][3] = 0.0;

   matrix33.data[3][0] = 0.0;
   matrix33.data[3][1] = 0.0;
   matrix33.data[3][2] = 0.0;

   matrix33.data[3][3] = 0.0;
}

inline Vector Matrix44::GetXAxis() const
{
   return Vector( data[0][0], data[1][0], data[2][0] );
}

inline Vector Matrix44::GetYAxis() const
{
   return Vector( data[0][1], data[1][1], data[2][1] );
}

inline Vector Matrix44::GetZAxis() const
{
   return Vector( data[0][2], data[1][2], data[2][2] );
}

inline BOOL Matrix44::operator == ( const Matrix44 &other ) const
{
   if (identity && other.identity)
      return TRUE;
   else
      return memcmp( data, other.data, sizeof(float)*16 ) == 0;
}

inline BOOL Matrix44::operator != ( const Matrix44 &other ) const
{
   if (identity && other.identity)
      return FALSE;
   else
      return memcmp( data, other.data, sizeof(float)*16 ) != 0;
}

inline Matrix44 &Matrix44::operator *= ( const Matrix44 &other )
{
   Matrix44 temp(*this);
   Multiply(temp,other);
   return *this;
}

inline Matrix44 Matrix44::operator * ( const Matrix44 &other ) const
{
   return Matrix44(*this) *= other;
}

inline Vector Matrix44::operator * ( const Vector &other ) const
{
   SMLVec3f srcvect(other.x,other.y,other.z);
   SMLVec3f dstvect;
   SMLMatrix4f::TransformPoint(srcvect,dstvect);
   return Vector(dstvect.x,dstvect.y,dstvect.z);
}

inline Matrix44 Matrix44::Transpose() const
{
   Matrix44 result( *this );
   result.SMLMatrix4f::Transpose();
   return result;
}

inline Matrix44 Matrix44::Inverse() const
{
   Matrix44 result;
   result.Invert((Matrix44 &)*this);
   return result;
}      

inline Matrix44 Matrix44::InvertRotateTranslate() const  // THE MATRIX MUST HAVE NO SCALE
{
   Matrix44 rotate = Transpose();
   Vector translate;
   GetTranslation( translate );
   rotate.SetTranslation( rotate * -translate );
   return rotate;
}

inline void Matrix44::GetTranslation( Vector &translate ) const
{
   translate.x = data[0][3];
   translate.y = data[1][3];
   translate.z = data[2][3];
}

inline void Matrix44::SetTranslation( const Vector &t )
{
   identity = false;

   data[0][3] = t.x;
   data[1][3] = t.y;
   data[2][3] = t.z;
}

inline void Matrix44::GetScale( Vector &scale ) const
{
   scale.x = data[0][0];
   scale.y = data[1][1];
   scale.z = data[2][2];
}

inline void Matrix44::SetScale( const Vector &s )
{
   identity = false;

   data[0][0] = s.x;
   data[1][1] = s.y;
   data[2][2] = s.z;
}

inline void Matrix44::GetFloatMatrix(float m[16],bool bTransposed/*=false*/) const
{
   if (bTransposed)
   {
      // returns column major
      m[0] = data[0][0];
      m[1] = data[1][0];
      m[2] = data[2][0];
      m[3] = data[3][0];

      m[4] = data[0][1];
      m[5] = data[1][1];
      m[6] = data[2][1];
      m[7] = data[3][1];

      m[8] = data[0][2];
      m[9] = data[1][2];
      m[10] = data[2][2];
      m[11] = data[3][2];

      m[12] = data[0][3];
      m[13] = data[1][3];
      m[14] = data[2][3];
      m[15] = data[3][3];
   }
   else// stored internally as row major
      memcpy(m,data,sizeof(float) * 16);
}

inline void Matrix44::GetDoubleMatrix(double m[16],bool bTransposed/*=false*/) const
{
   if (bTransposed)
   {
      m[0] = (double)data[0][0];
      m[1] = (double)data[1][0];
      m[2] = (double)data[2][0];
      m[3] = (double)data[3][0];

      m[4] = (double)data[0][1];
      m[5] = (double)data[1][1];
      m[6] = (double)data[2][1];
      m[7] = (double)data[3][1];

      m[8] = (double)data[0][2];
      m[9] = (double)data[1][2];
      m[10] = (double)data[2][2];
      m[11] = (double)data[3][2];

      m[12] = (double)data[0][3];
      m[13] = (double)data[1][3];
      m[14] = (double)data[2][3];
      m[15] = (double)data[3][3];
   }
   else
   {
      m[0] = (double)data[0][0];
      m[1] = (double)data[0][1];
      m[2] = (double)data[0][2];
      m[3] = (double)data[0][3];

      m[4] = (double)data[1][0];
      m[5] = (double)data[1][1];
      m[6] = (double)data[1][2];
      m[7] = (double)data[1][3];

      m[8] = (double)data[2][0];
      m[9] = (double)data[2][1];
      m[10] = (double)data[2][2];
      m[11] = (double)data[2][3];

      m[12] = (double)data[3][0];
      m[13] = (double)data[3][1];
      m[14] = (double)data[3][2];
      m[15] = (double)data[3][3];
   }
}

inline Vector2 &Vector2::operator *= (const Matrix44 &m)
{
   SMLVec3f srcvect(x,y,0);
   SMLVec3f dstvect;
   ((SMLMatrix4f)m).TransformPoint(srcvect,dstvect);
   x = dstvect.x;
   y = dstvect.y;
   return *this; 
}

inline Vector &Vector::operator *= (const Matrix44 &m)
{
   SMLVec3f srcvect(x,y,z);
   SMLVec3f dstvect;
   ((SMLMatrix4f)m).TransformPoint(srcvect,dstvect);
   x = dstvect.x;
   y = dstvect.y;
   z = dstvect.z;
   return *this; 
}

inline Vector4 &Vector4::operator *= (const Matrix44 &m)
{
   SMLVec4f srcvect(x,y,z,w);
   SMLVec4f dstvect;
   ((SMLMatrix4f)m).Transform(srcvect,dstvect);
   x = dstvect.x;
   y = dstvect.y;
   z = dstvect.z;
   w = dstvect.w;
   return *this; 
}

inline void Matrix44::TransformPoint(const Vector &src,Vector &dst) const
{
   SMLVec3f src_simdv(src.x,src.y,src.z);
   SMLVec3f dst_simdv;
   SMLMatrix4f::TransformPoint(src_simdv,dst_simdv);
   dst.x = dst_simdv.x;
   dst.y = dst_simdv.y;
   dst.z = dst_simdv.z;
}

// this doesnt seem to be working to well for RowMajor
inline void Matrix44::Decompose(Vector &translation,Vector &scale,Vector &shear,Vector &rotation) const
{
#if 0//column major?
   Vector xVector(data[0][0],data[0][1],data[0][2]);
   Vector yVector(data[1][0],data[1][1],data[1][2]);
   Vector zVector(data[2][0],data[2][1],data[2][2]);
#else// row major?
   Vector xVector(data[0][0],data[1][0],data[2][0]);
   Vector yVector(data[0][1],data[1][1],data[2][1]);
   Vector zVector(data[0][2],data[1][2],data[2][2]);
#endif
   // TRANSLATION

#if 0//column major?
   translation.x = data[3][0];
   translation.y = data[3][1];
   translation.z = data[3][2];
#else// row major?
   translation.x = data[0][3];
   translation.y = data[1][3];
   translation.z = data[2][3];
#endif

   // for now just take out the perspective stuff
   //data[3][0] = data[3][1] = data[3][2] = 0.0;
   //data[3][3] = 1.0;

   // GET SHEAR AND SCALE

   // Compute X m_scale factor and normalize first row. 
   scale.x = xVector.Length();
   if (scale.x!=0.0)
      xVector/=scale.x;

   // Compute XY m_shear factor and make 2nd row orthogonal to 1st. 
   shear.x = xVector.Dot(yVector);
   yVector += (xVector*-shear.x);


   // Now, compute Y m_scale and normalize 2nd row. 
   scale.y = yVector.Length();
   if (scale.y!=0.0)
   {
      yVector/=scale.y;
      shear.x /= scale.y;
   }

   // Compute XZ and YZ shears, orthogonalize 3rd row. 

   shear.y = xVector.Dot(zVector);
   zVector += (xVector*-shear.y);
   shear.z = yVector.Dot(zVector);
   zVector += (yVector*-shear.z);

   // Next, get Z m_scale and normalize 3rd row. 
   scale.z = zVector.Length();

   if (scale.z!=0.0)
   {
      zVector/=scale.z;
      shear.y /= scale.z;
      shear.z /= scale.z;
   }

   Vector cross;
   cross = yVector.Cross(zVector);//yVector.CrossProduct(zVector,cross);

   if (xVector.Dot(cross) < 0.0)
   {
      // invert the scales as well
      scale *= -1.0;
      xVector *= -1.0;
      yVector *= -1.0;
      zVector *= -1.0;
   }

   // GET ROTATIONS
   // Rotation Order is X Y Z
   rotation.y = (float)asin(-xVector.z);

   if (cos(rotation.y) != 0 )
   {
      rotation.x = (float)(atan2(yVector.z, zVector.z)*RADTODEG);
      rotation.z = (float)(atan2(xVector.y, xVector.x)*RADTODEG);
   }
   else
   {
      rotation.x = (float)(atan2(yVector.x, yVector.y)*RADTODEG);
      rotation.z = 0.0;
   }
   rotation.y*=(float)RADTODEG;	
}

// Creation helper functions

inline const Matrix44 &Identity44()
{
   static Matrix44 identity44;
   return identity44;
}                    

inline Matrix44 Translatef( float x, float y, float z )
{
   return Matrix44( 1.0f, 0.0f, 0.0f,    x,
                    0.0f, 1.0f, 0.0f,    y,
                    0.0f, 0.0f, 1.0f,    z,
                    0.0f, 0.0f, 0.0f, 1.0f
                   );
}

inline Matrix44 TranslateV( const Vector &v )
{
   return Translatef( v.x, v.y, v.z );
}

inline Matrix44 Scalef( float x, float y, float z )
{
   return Matrix44(    x, 0.0f, 0.0f, 0.0f,
                    0.0f,    y, 0.0f, 0.0f,
                    0.0f, 0.0f,    z, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f
                   );
}

inline Matrix44 ScaleV( const Vector &v )
{
   return Scalef( v[0], v[1], v[2] );
}

inline Matrix44 RotateVRadians( float anglerad, const Vector &axis )
{
   float c = cos( anglerad ),
         s = sin( anglerad ),
         t = 1.0f - c;
   Vector v = axis;
         
   v.Normalize();
   return Matrix44( t*v[0]*v[0]+c,      t*v[0]*v[1]-s*v[2], t*v[0]*v[2]+s*v[1], 0.0f,
                    t*v[0]*v[1]+s*v[2], t*v[1]*v[1]+c,      t*v[1]*v[2]-s*v[0], 0.0f,
                    t*v[0]*v[2]-s*v[1], t*v[1]*v[2]+s*v[0], t*v[2]*v[2]+c,      0.0f,
                    0.0f,               0.0f,               0.0f,               1.0f );
}

inline Matrix44 Rotatef( float angle, float x, float y, float z )
{
   return RotateV( angle, Vector( x, y, z ));
}

inline Matrix44 RotateV( float angle, const Vector &axis )
{
   return RotateVRadians( angle / RADIANS, axis ); 
}

inline Matrix44 Rotate2V( const Vector &from, const Vector &to )
{
   float numerator = from | to;
   float denominator = from.Length() * to.Length();
   float radangle;
   Vector axis;

   if ( fabs(numerator - denominator) < .00001) // SAME DIR
      return Identity44();
   if ( fabs(numerator + denominator) < .00001 ) { // OPPOSITE DIR
      radangle = 3.141519f;
      if (from.Normalized().Absolute() != Vector(1,0,0))
         axis = from ^ Vector(1,0,0);
      else
         axis = from ^ Vector(0,1,0);
   }
   else {
      float ratio = numerator / denominator;
      if (ratio >= 1.0f)
         return Identity44();
      radangle = acos( ratio );
      axis = from ^ to;
   }
   return RotateVRadians( radangle, axis );
}

inline Matrix44 XRotate( float angle )
{
   return XRotateRadians( angle / RADIANS ); 
}

inline Matrix44 YRotate( float angle )
{
   return YRotateRadians( angle / RADIANS ); 
}

inline Matrix44 ZRotate( float angle )
{
   return ZRotateRadians( angle / RADIANS ); 
}

inline Matrix44 XRotateRadians( float anglerad )
{
   float ca = (float)cos( anglerad ),
         sa = (float)sin( anglerad );
   return Matrix44( 1.f, 0.f, 0.f, 0.f,
                    0.f,  ca, -sa, 0.f,
                    0.f,  sa,  ca, 0.f,
                    0.f, 0.f, 0.f, 1.f);
}

inline Matrix44 YRotateRadians( float anglerad )
{
   float ca = (float)cos( anglerad ),
         sa = (float)sin( anglerad );
   return Matrix44(  ca, 0.f,  sa, 0.f,
                    0.f, 1.f, 0.f, 0.f,
                    -sa, 0.f,  ca, 0.f,
                    0.f, 0.f, 0.f, 1.f);
}

inline Matrix44 ZRotateRadians( float anglerad )
{
   float ca = (float)cos( anglerad ),
         sa = (float)sin( anglerad );
   return Matrix44(  ca, -sa, 0.f, 0.f,
                     sa,  ca, 0.f, 0.f,
                    0.f, 0.f, 1.f, 0.f,
                    0.f, 0.f, 0.f, 1.f);
}

inline Matrix44 YXRotateRadians( float yangle, float xangle )
{
   float cy = cos( yangle ),
         sy = sin( yangle ),
         cx = cos( xangle ),
         sx = sin( xangle );

   return Matrix44(  cy, sy*sx, sy*cx, 0.f,
                    0.f,    cx,   -sx, 0.f,
                    -sy, cy*sx, cy*cx, 0.f,
                    0.f,   0.f,   0.f, 1.f);
}

inline Matrix44 YXZRotateRadians( float yangle, float xangle, float zangle )
{
   float cx = cos( xangle ),
         sx = sin( xangle ),
         cy = cos( yangle ),
         sy = sin( yangle ),
         cz = cos( zangle ),
         sz = sin( zangle );

   return Matrix44( cy*cz+sy*sx*sz, -cy*sz+sy*sx*cz, sy*cx, 0.f,
                        cx*sz,           cx*cz,       -sx,  0.f,
                    -sy*cz+cy*sx*sz, sy*sz+cy*sx*cz, cy*cx, 0.f,
                         0.f,             0.f,        0.f,  1.f);
}

inline Matrix44 YXZRotate( float yangle, float xangle, float zangle ) 
{
   return YXZRotateRadians( yangle/RADIANS, xangle/RADIANS, zangle/RADIANS ); 
}

inline Matrix44 YXZRotate( Vector angle ) 
{
   return YXZRotateRadians( angle.y/RADIANS, angle.x/RADIANS, angle.z/RADIANS ); 
}

inline void TransformVectorNoTranslate( const Matrix44 &m, Vector &v )
{
   v.Set( m.data[0][0] * v.x + m.data[0][1] * v.y + m.data[0][2] * v.z, 
          m.data[1][0] * v.x + m.data[1][1] * v.y + m.data[1][2] * v.z,
          m.data[2][0] * v.x + m.data[2][1] * v.y + m.data[2][2] * v.z );
}

inline Matrix44 RotateAngleAxis(double anglerad,Vector axis)
{
   axis.Normalize();
   float sine = (float)sin(anglerad);
   float cosine = (float)cos(anglerad);

   Matrix44 matrix;

   matrix.Identity();

   matrix.Set(0,0,axis.x*axis.x*(1.0f-cosine)+cosine);
   matrix.Set(0,1,axis.x*axis.y*(1.0f-cosine)-(axis.z*sine));
   matrix.Set(0,2,axis.x*axis.z*(1.0f-cosine)+(axis.y*sine));

   matrix.Set(1,0,axis.y*axis.x*(1.0f-cosine)+(axis.z*sine));
   matrix.Set(1,1,axis.y*axis.y*(1.0f-cosine)+cosine);
   matrix.Set(1,2,axis.y*axis.z*(1.0f-cosine)-(axis.x*sine));

   matrix.Set(2,0,axis.x*axis.z*(1.0f-cosine)-(axis.y*sine));
   matrix.Set(2,1,axis.y*axis.z*(1.0f-cosine)+(axis.x*sine));
   matrix.Set(2,2,axis.z*axis.z*(1.0f-cosine)+cosine);

   return matrix;
}

inline Matrix44 Rotate2VMatrix(Vector from, Vector to)
{
   from.Normalize();
   to.Normalize();
   float numerator = from | to;
   float denominator = from.Length() * to.Length();
   float radangle;
   Vector axis;

   if ( fabs(numerator - denominator) < .00001) // SAME DIR
   {
      Matrix44 identity;
      identity.Identity();
      return identity;
   }
   if ( fabs(numerator + denominator) < .00001 ) // OPPOSITE DIR
   {
      radangle = 3.141519f;
      if (from.Normalized().Absolute() != Vector(1,0,0))
         axis = from ^ Vector(1,0,0);
      else
         axis = from ^ Vector(0,1,0);
   }
   else 
   {
      float ratio = numerator / denominator;
      if (ratio >= 1.0f)
      {
         Matrix44 identity;
         identity.Identity();
         return identity;
      }
      radangle = acos( ratio );
      axis = from ^ to;
   }
   return RotateAngleAxis( radangle, axis);
}

inline Matrix44 VectorsToStandard(const Vector &becomezdir,const Vector &becomeydir)
{
   Matrix44 vectostandard = Rotate2VMatrix( becomezdir, Vector( 0.0f, 0.0f, 1.0f ));
   Vector rollvec = vectostandard * becomeydir;
   float roll = (float)atan2( -rollvec.x, rollvec.y );
   if (roll)
      vectostandard = ZRotateRadians(-roll) * vectostandard;
   return vectostandard;
}