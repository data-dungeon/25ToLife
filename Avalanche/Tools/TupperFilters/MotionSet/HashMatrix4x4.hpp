/////////////////////////////////////////////////////////////////////////////////////////
// HashMatrix4x4 INLINE METHODS
/////////////////////////////////////////////////////////////////////////////////////////

inline HashMatrix4x4::HashMatrix4x4(void)
{
   memset(v,0,sizeof(HashVector4)*4);
}

inline HashMatrix4x4::HashMatrix4x4(const float m[4][4])
{
   v[0] = HashVector4((double)m[0][0],(double)m[0][1],(double)m[0][2],(double)m[0][3]);
   v[1] = HashVector4((double)m[1][0],(double)m[1][1],(double)m[1][2],(double)m[1][3]);
   v[2] = HashVector4((double)m[2][0],(double)m[2][1],(double)m[2][2],(double)m[2][3]);
   v[3] = HashVector4((double)m[3][0],(double)m[3][1],(double)m[3][2],(double)m[3][3]);
}


inline HashMatrix4x4::HashMatrix4x4(const double m[4][4])
{
   v[0] = HashVector4(m[0][0],m[0][1],m[0][2],m[0][3]);
   v[1] = HashVector4(m[1][0],m[1][1],m[1][2],m[1][3]);
   v[2] = HashVector4(m[2][0],m[2][1],m[2][2],m[2][3]);
   v[3] = HashVector4(m[3][0],m[3][1],m[3][2],m[3][3]);
}

inline HashMatrix4x4::HashMatrix4x4(float *pMatrix44)
{
   v[0] = HashVector4(pMatrix44[0],pMatrix44[1],pMatrix44[2],pMatrix44[3]);
   v[1] = HashVector4(pMatrix44[4],pMatrix44[5],pMatrix44[6],pMatrix44[7]);
   v[2] = HashVector4(pMatrix44[8],pMatrix44[9],pMatrix44[10],pMatrix44[11]);
   v[3] = HashVector4(pMatrix44[12],pMatrix44[13],pMatrix44[14],pMatrix44[15]);
}

inline HashMatrix4x4::HashMatrix4x4(const HashVector4 v0,const HashVector4 v1,const HashVector4 v2, const HashVector4 v3)
{
   v[0] = v0;
   v[1] = v1;
   v[2] = v2;
   v[3] = v3;
}

inline HashMatrix4x4::HashMatrix4x4( double f00, double f01, double f02, double f03,
                                     double f10, double f11, double f12, double f13,
                                     double f20, double f21, double f22, double f23,
                                     double f30, double f31, double f32, double f33 )
{
   v[0][0] = f00;
   v[0][1] = f01;
   v[0][2] = f02;
   v[0][3] = f03;

   v[1][0] = f10;
   v[1][1] = f11;
   v[1][2] = f12;
   v[1][3] = f13;

   v[2][0] = f20;
   v[2][1] = f21;
   v[2][2] = f22;
   v[2][3] = f23;

   v[3][0] = f30;
   v[3][1] = f31;
   v[3][2] = f32;
   v[3][3] = f33;
}

inline HashMatrix4x4::HashMatrix4x4(const HashMatrix4x4 &other)
{
   memcpy(v,other.v,sizeof(HashVector4)*4);
}

inline HashMatrix4x4::HashMatrix4x4(const double scalar)
{
   v[0] = HashVector4(scalar,scalar,scalar,scalar);
   v[1] = HashVector4(scalar,scalar,scalar,scalar);
   v[2] = HashVector4(scalar,scalar,scalar,scalar);
   v[3] = HashVector4(scalar,scalar,scalar,scalar);
}

inline HashMatrix4x4::HashMatrix4x4(const HashQuaternion &quat)
{	
   SetIdentity();

   double X2,  Y2,  Z2;
   double XX2, YY2, ZZ2;
   double XY2, XZ2, XW2;
   double YZ2, YW2, ZW2;

   X2  = 2.0f * quat.v.x;
   XX2 = X2   * quat.v.x;
   XY2 = X2   * quat.v.y;
   XZ2 = X2   * quat.v.z;
   XW2 = X2   * quat.w;

   Y2  = 2.0f * quat.v.y;
   YY2 = Y2   * quat.v.y;
   YZ2 = Y2   * quat.v.z;
   YW2 = Y2   * quat.w;

   Z2  = 2.0f * quat.v.z;
   ZZ2 = Z2   * quat.v.z;
   ZW2 = Z2   * quat.w;

   (*this)[0][0] = 1.0f - YY2 - ZZ2;
   (*this)[1][0] = XY2  - ZW2;
   (*this)[2][0] = XZ2  + YW2;

   (*this)[0][1] = XY2  + ZW2;
   (*this)[1][1] = 1.0f - XX2 - ZZ2;
   (*this)[2][1] = YZ2  - XW2;

   (*this)[0][2] = XZ2  - YW2;
   (*this)[1][2] = YZ2  + XW2;
   (*this)[2][2] = 1.0f - XX2 - YY2;

   (*this)[3][0] = (*this)[3][1] = (*this)[3][2] = (*this)[1][3] = (*this)[2][3];
   (*this)[3][3] = 1.0;
}

inline HashMatrix4x4 &HashMatrix4x4::operator = (const HashMatrix4x4 &other) 
{
   memcpy(v,other.v,sizeof(HashVector4)*4);
   return *this;
}

inline HashMatrix4x4 &HashMatrix4x4::operator += (const HashMatrix4x4 &other) 
{
   v[0] += other.v[0];
   v[1] += other.v[1];
   v[2] += other.v[2];
   v[3] += other.v[3];
   return *this;
}

inline HashMatrix4x4 &HashMatrix4x4::operator -= ( const HashMatrix4x4 &other ) 
{
   v[0] -= other.v[0];
   v[1] -= other.v[1];
   v[2] -= other.v[2];
   v[3] -= other.v[3];
   return *this;
}

inline HashMatrix4x4 &HashMatrix4x4::operator *= ( const HashMatrix4x4 &other ) 
{
   HashMatrix4x4 temp;

   temp[0][0] = (other[0][0] * (*this)[0][0]) + (other[1][0] * (*this)[0][1]) + (other[2][0] * (*this)[0][2]) + (other[3][0] * (*this)[0][3]);
   temp[1][0] = (other[0][0] * (*this)[1][0]) + (other[1][0] * (*this)[1][1]) + (other[2][0] * (*this)[1][2]) + (other[3][0] * (*this)[1][3]);
   temp[2][0] = (other[0][0] * (*this)[2][0]) + (other[1][0] * (*this)[2][1]) + (other[2][0] * (*this)[2][2]) + (other[3][0] * (*this)[2][3]);
   temp[3][0] = (other[0][0] * (*this)[3][0]) + (other[1][0] * (*this)[3][1]) + (other[2][0] * (*this)[3][2]) + (other[3][0] * (*this)[3][3]);

   temp[0][1] = (other[0][1] * (*this)[0][0]) + (other[1][1] * (*this)[0][1]) + (other[2][1] * (*this)[0][2]) + (other[3][1] * (*this)[0][3]);
   temp[1][1] = (other[0][1] * (*this)[1][0]) + (other[1][1] * (*this)[1][1]) + (other[2][1] * (*this)[1][2]) + (other[3][1] * (*this)[1][3]);
   temp[2][1] = (other[0][1] * (*this)[2][0]) + (other[1][1] * (*this)[2][1]) + (other[2][1] * (*this)[2][2]) + (other[3][1] * (*this)[2][3]);
   temp[3][1] = (other[0][1] * (*this)[3][0]) + (other[1][1] * (*this)[3][1]) + (other[2][1] * (*this)[3][2]) + (other[3][1] * (*this)[3][3]);

   temp[0][2] = (other[0][2] * (*this)[0][0]) + (other[1][2] * (*this)[0][1]) + (other[2][2] * (*this)[0][2]) + (other[3][2] * (*this)[0][3]);
   temp[1][2] = (other[0][2] * (*this)[1][0]) + (other[1][2] * (*this)[1][1]) + (other[2][2] * (*this)[1][2]) + (other[3][2] * (*this)[1][3]);
   temp[2][2] = (other[0][2] * (*this)[2][0]) + (other[1][2] * (*this)[2][1]) + (other[2][2] * (*this)[2][2]) + (other[3][2] * (*this)[2][3]);
   temp[3][2] = (other[0][2] * (*this)[3][0]) + (other[1][2] * (*this)[3][1]) + (other[2][2] * (*this)[3][2]) + (other[3][2] * (*this)[3][3]);

   temp[0][3] = (other[0][3] * (*this)[0][0]) + (other[1][3] * (*this)[0][1]) + (other[2][3] * (*this)[0][2]) + (other[3][3] * (*this)[0][3]);
   temp[1][3] = (other[0][3] * (*this)[1][0]) + (other[1][3] * (*this)[1][1]) + (other[2][3] * (*this)[1][2]) + (other[3][3] * (*this)[1][3]);
   temp[2][3] = (other[0][3] * (*this)[2][0]) + (other[1][3] * (*this)[2][1]) + (other[2][3] * (*this)[2][2]) + (other[3][3] * (*this)[2][3]);
   temp[3][3] = (other[0][3] * (*this)[3][0]) + (other[1][3] * (*this)[3][1]) + (other[2][3] * (*this)[3][2]) + (other[3][3] * (*this)[3][3]);

   *this = temp;

   return *this;
}

inline HashMatrix4x4 &HashMatrix4x4::operator *= ( double scalar ) 
{
   v[0] *= scalar;
   v[1] *= scalar;
   v[2] *= scalar;
   v[3] *= scalar;
   return *this; 
}

inline HashMatrix4x4 HashMatrix4x4::operator + ( const HashMatrix4x4 &other ) const 
{
   return HashMatrix4x4(*this) += other;
}

inline HashMatrix4x4 HashMatrix4x4::operator - ( const HashMatrix4x4 &other ) const 
{
   return HashMatrix4x4(*this) -= other;
}

inline HashMatrix4x4 HashMatrix4x4::operator * ( const HashMatrix4x4 &other ) const 
{
   return HashMatrix4x4(*this) *= other;
}

inline HashMatrix4x4 HashMatrix4x4::operator * ( double scalar ) const 
{
   return HashMatrix4x4(*this) *= scalar;
}

inline bool HashMatrix4x4::operator == ( const HashMatrix4x4 &other ) const 
{
   if (memcmp(v,other.v,sizeof(HashVector4)*4))
      return true;
   return false;
}

inline bool HashMatrix4x4::operator == ( const double scalar ) const 
{
   double temp[4][4] = {{scalar,scalar,scalar,scalar},
   {scalar,scalar,scalar,scalar},
   {scalar,scalar,scalar,scalar},
   {scalar,scalar,scalar,scalar}};
   if (memcmp(v,temp,sizeof(HashVector4)*4))
      return true;
   return false;;
}

inline bool HashMatrix4x4::operator != ( const HashMatrix4x4 &other ) const 
{
   return !(*this == other);
}

inline void HashMatrix4x4::Set(const double m[4][4])
{
   v[0] = HashVector4(m[0][0],m[0][1],m[0][2],m[0][3]);
   v[1] = HashVector4(m[1][0],m[1][1],m[1][2],m[1][3]);
   v[2] = HashVector4(m[2][0],m[2][1],m[2][2],m[2][3]);
   v[3] = HashVector4(m[3][0],m[3][1],m[3][2],m[3][3]);
}

inline void HashMatrix4x4::SetIdentity()
{
   double identity[4][4] = {{ 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 }};
   (*this) = HashMatrix4x4(identity);
}

inline HashMatrix4x4 HashMatrix4x4::GetIdentityMatrix()
{
   double identity[4][4] = {{ 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 }};
   return HashMatrix4x4(identity);
}

inline HashVector3 HashMatrix4x4::GetTranslation() const
{
   return HashVector3((*this)[3][0],(*this)[3][1],(*this)[3][2]);
}

inline HashVector3 HashMatrix4x4::operator * ( const HashVector3 &other ) const
{
   HashVector4 vec4( other.x, other.y, other.z, 1.0f );
   HashVector4 v0(v[0][0],v[1][0],v[2][0],v[3][0]);
   HashVector4 v1(v[0][1],v[1][1],v[2][1],v[3][1]);
   HashVector4 v2(v[0][2],v[1][2],v[2][2],v[3][2]);
   return HashVector3( v0|vec4, v1|vec4, v2|vec4 );
}

inline HashVector4 HashMatrix4x4::operator * ( const HashVector4 &other ) const
{
   return HashVector4( v[0]|other, v[1]|other, v[2]|other, v[3]|other );
}

inline HashMatrix4x4 HashMatrix4x4::Transpose()
{
   return HashMatrix4x4( HashVector4( v[0][0], v[1][0], v[2][0], v[3][0] ),
      HashVector4( v[0][1], v[1][1], v[2][1], v[3][1] ),
      HashVector4( v[0][2], v[1][2], v[2][2], v[3][2] ),
      HashVector4( v[0][3], v[1][3], v[2][3], v[3][3] ));
}

/*
inline HashMatrix4x4 GetXRotateMatrix( double anglerad )
{//row major
   double ca = cos( anglerad ),
      sa = sin( anglerad );
   return HashMatrix4x4( 1.f, 0.f, 0.f, 0.f,
      0.f,  ca, -sa, 0.f,
      0.f,  sa,  ca, 0.f,
      0.f, 0.f, 0.f, 1.f);
}

inline HashMatrix4x4 GetYRotateMatrix( double anglerad )
{//row major
   double ca = cos( anglerad ),
      sa = sin( anglerad );
   return HashMatrix4x4(  ca, 0.f,  sa, 0.f,
      0.f, 1.f, 0.f, 0.f,
      -sa, 0.f,  ca, 0.f,
      0.f, 0.f, 0.f, 1.f);
}

inline HashMatrix4x4 GetZRotateMatrix( double anglerad )
{//row major
   double ca = cos( anglerad ),
      sa = sin( anglerad );
   return HashMatrix4x4(  ca, -sa, 0.f, 0.f,
      sa,  ca, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      0.f, 0.f, 0.f, 1.f);
}
*/
inline HashMatrix4x4 GetYXRotateMatrix( double yanglerad, double xanglerad )
{// row major
   double cy = cos( yanglerad ),
      sy = sin( yanglerad ),
      cx = cos( xanglerad ),
      sx = sin( xanglerad );

   return HashMatrix4x4(  cy, sy*sx, sy*cx, 0.f,
      0.f,    cx,   -sx, 0.f,
      -sy, cy*sx, cy*cx, 0.f,
      0.f,   0.f,   0.f, 1.f);
}

inline HashMatrix4x4 GetYXZRotateMatrix( double yanglerad, double xanglerad, double zanglerad )
{//row major
   double cx = cos( xanglerad ),
      sx = sin( xanglerad ),
      cy = cos( yanglerad ),
      sy = sin( yanglerad ),
      cz = cos( zanglerad ),
      sz = sin( zanglerad );

   return HashMatrix4x4( cy*cz+sy*sx*sz, -cy*sz+sy*sx*cz, sy*cx, 0.f,
      cx*sz,           cx*cz,       -sx,  0.f,
      -sy*cz+cy*sx*sz, sy*sz+cy*sx*cz, cy*cx, 0.f,
      0.f,             0.f,        0.f,  1.f);
}


inline HashMatrix4x4 GetXRotateMatrix(const double anglerad)
{
   double sine = sin(anglerad),cosine = cos(anglerad);
   //	double xrotmatrow[4][4] = {{1, 0, 0, 0}, {0, cosine, -sine, 0}, {0, sine, cosine ,0 }, {0, 0, 0, 1}};
   double xrotmatcol[4][4] = {{1, 0, 0, 0}, {0, cosine, sine, 0}, {0, -sine, cosine ,0 }, {0, 0, 0, 1}};//column major
   return HashMatrix4x4(xrotmatcol);
}

inline HashMatrix4x4 GetYRotateMatrix(const double anglerad)
{
   double sine = sin(anglerad),cosine = cos(anglerad);
   //	double yrotmatrow[4][4] = {{ cosine, 0, sine, 0}, {0, 1, 0, 0}, {-sine ,0, cosine, 0 }, {0, 0, 0, 1}};
   double yrotmatcol[4][4] = {{ cosine, 0, -sine, 0}, {0, 1, 0, 0}, {sine ,0, cosine, 0 }, {0, 0, 0, 1}};//column major
   return HashMatrix4x4(yrotmatcol);
}

inline HashMatrix4x4 GetZRotateMatrix(const double anglerad)
{
   double sine = sin(anglerad),cosine = cos(anglerad);
   //	double zrotmatrow[4][4] = {{ cosine, -sine, 0, 0}, {sine, cosine, 0, 0}, {0, 0, 1, 0 }, {0, 0, 0, 1}};
   double zrotmatcol[4][4] = {{ cosine, sine, 0, 0}, {-sine, cosine, 0, 0}, {0, 0, 1, 0 }, {0, 0, 0, 1}};//column major
   return HashMatrix4x4(zrotmatcol);
}

inline HashMatrix4x4 GetTranslationMatrix(const HashVector3 &other)
{
   double mat[4][4] = {{ 1, 0, 0, 0 }, {0, 1, 0, 0 }, { 0, 0, 1, 0 }, { other.x, other.y, other.z, 1 }};
   return HashMatrix4x4(mat);
}


inline HashMatrix4x4 GetScaleMatrix(const HashVector3 &other)
{
   double mat[4][4] = {{ other.x, 0, 0, 0 }, { 0, other.y, 0, 0 }, { 0, 0, other.z, 0 }, { 0, 0, 0, 1 }};
   return HashMatrix4x4(mat);
}

inline HashVector3 HashMatrix4x4::GetXAxis() const
{
   return HashVector3(v[0][0],v[0][1],v[0][2]);
}

inline HashVector3 HashMatrix4x4::GetYAxis() const
{
   return HashVector3(v[1][0],v[1][1],v[1][2]);
}

inline HashVector3 HashMatrix4x4::GetZAxis() const
{
   return HashVector3(v[2][0],v[2][1],v[2][2]);
}


inline HashQuaternion::HashQuaternion(const HashMatrix4x4 &matrix44)
{
   double tr,s;
   int i;

   tr = matrix44.v[0][0] + matrix44.v[1][1] + matrix44.v[2][2];
   if (tr > 0.0f)
   {
      s = sqrt(tr + 1.0f);
      w = s * 0.5f;
      s = 0.5f / s;

      v.Set((matrix44.v[1][2] - matrix44.v[2][1]) * s,
         (matrix44.v[2][0] - matrix44.v[0][2]) * s,
         (matrix44.v[0][1] - matrix44.v[1][0]) * s);
   }
   else
   {
      // find largest value on diagonal

      i = 0;
      if (matrix44.v[1][1] > matrix44.v[0][0])
         i = 1;
      if ((matrix44.v[2][2] > matrix44.v[0][0]) && (matrix44.v[2][2] > matrix44.v[1][1]))
         i = 2;

      switch(i)
      {
      case 0:
         s = sqrt(matrix44.v[0][0] - matrix44.v[1][1] - matrix44.v[2][2] + 1.0f);
         v.x = ( s * 0.5f);
         assert(s != 0.0f);
         s = 0.5f / s;
         v.y = ( (matrix44.v[0][1] + matrix44.v[1][0]) * s);
         v.z = ( (matrix44.v[0][2] + matrix44.v[2][0]) * s);
         w   = (matrix44.v[1][2] - matrix44.v[2][1]) * s;
         break;
      case 1:
         s = sqrt(matrix44.v[1][1] - matrix44.v[2][2] - matrix44.v[0][0] + 1.0f);
         v.y = ( s * 0.5f);
         assert(s != 0.0f);
         s = 0.5f / s;
         v.x = ( (matrix44.v[1][0] + matrix44.v[0][1]) * s);
         v.z = ( (matrix44.v[1][2] + matrix44.v[2][1]) * s);
         w   = (matrix44.v[2][0] - matrix44.v[0][2]) * s;
         break;
      case 2:
         s = sqrt(matrix44.v[2][2] - matrix44.v[0][0] - matrix44.v[1][1] + 1.0f);
         v.z = ( s * 0.5f);
         assert(s != 0.0f);
         s = 0.5f / s;
         v.x = ( (matrix44.v[2][0] + matrix44.v[0][2]) * s);
         v.y = ( (matrix44.v[2][1] + matrix44.v[1][2]) * s);
         w   = (matrix44.v[0][1] - matrix44.v[1][0]) * s;
         break;
      default:
         assert(FALSE);
      }
   }
}


inline double HashMatrix4x4::Det2x2(double _11, double _12,double _21, double _22)
{
   return(_11 * _22 - _21 * _12);
}

inline double HashMatrix4x4::Det3x3(double _11, double _12, double _13,double _21, double _22, double _23,double _31, double _32, double _33)							  
{
   return(
      _11 * Det2x2(_22, _23, _32, _33) -
      _12 * Det2x2(_21, _23, _31, _33) +
      _13 * Det2x2(_21, _22, _31, _32)
      );	
}

inline double HashMatrix4x4::Det4x4()
{
   return( 
      (*this)[0][0] * Det3x3((*this)[1][1], (*this)[2][1], (*this)[3][1], (*this)[1][2], (*this)[2][2], (*this)[3][2], (*this)[1][3], (*this)[2][3], (*this)[3][3]) -

      (*this)[1][0] * Det3x3((*this)[0][1], (*this)[2][1], (*this)[3][1], (*this)[0][2], (*this)[2][2], (*this)[3][2], (*this)[0][3], (*this)[2][3], (*this)[3][3]) +

      (*this)[2][0] * Det3x3((*this)[0][1], (*this)[1][1], (*this)[3][1], (*this)[0][2], (*this)[1][2], (*this)[3][2], (*this)[0][3], (*this)[1][3], (*this)[3][3]) -

      (*this)[3][0] * Det3x3((*this)[0][1], (*this)[1][1], (*this)[2][1], (*this)[0][2], (*this)[1][2], (*this)[2][2], (*this)[0][3], (*this)[1][3], (*this)[2][3])
      );
}

inline void HashMatrix4x4::Adj4x4(HashMatrix4x4 &out)
{
   out[0][0] =  Det3x3((*this)[1][1], (*this)[2][1], (*this)[3][1], (*this)[1][2], (*this)[2][2], (*this)[3][2], (*this)[1][3], (*this)[2][3], (*this)[3][3]);
   out[0][1] = -Det3x3((*this)[0][1], (*this)[2][1], (*this)[3][1], (*this)[0][2], (*this)[2][2], (*this)[3][2], (*this)[0][3], (*this)[2][3], (*this)[3][3]);
   out[0][2] =  Det3x3((*this)[0][1], (*this)[1][1], (*this)[3][1], (*this)[0][2], (*this)[1][2], (*this)[3][2], (*this)[0][3], (*this)[1][3], (*this)[3][3]);
   out[0][3] = -Det3x3((*this)[0][1], (*this)[1][1], (*this)[2][1], (*this)[0][2], (*this)[1][2], (*this)[2][2], (*this)[0][3], (*this)[1][3], (*this)[2][3]);

   out[1][0] = -Det3x3((*this)[1][0], (*this)[2][0], (*this)[3][0], (*this)[1][2], (*this)[2][2], (*this)[3][2], (*this)[1][3], (*this)[2][3], (*this)[3][3]);
   out[1][1] =  Det3x3((*this)[0][0], (*this)[2][0], (*this)[3][0], (*this)[0][2], (*this)[2][2], (*this)[3][2], (*this)[0][3], (*this)[2][3], (*this)[3][3]);
   out[1][2] = -Det3x3((*this)[0][0], (*this)[1][0], (*this)[3][0], (*this)[0][2], (*this)[1][2], (*this)[3][2], (*this)[0][3], (*this)[1][3], (*this)[3][3]);
   out[1][3] =  Det3x3((*this)[0][0], (*this)[1][0], (*this)[2][0], (*this)[0][2], (*this)[1][2], (*this)[2][2], (*this)[0][3], (*this)[1][3], (*this)[2][3]);

   out[2][0] =  Det3x3((*this)[1][0], (*this)[2][0], (*this)[3][0], (*this)[1][1], (*this)[2][1], (*this)[3][1], (*this)[1][3], (*this)[2][3], (*this)[3][3]);
   out[2][1] = -Det3x3((*this)[0][0], (*this)[2][0], (*this)[3][0], (*this)[0][1], (*this)[2][1], (*this)[3][1], (*this)[0][3], (*this)[2][3], (*this)[3][3]);
   out[2][2] =  Det3x3((*this)[0][0], (*this)[1][0], (*this)[3][0], (*this)[0][1], (*this)[1][1], (*this)[3][1], (*this)[0][3], (*this)[1][3], (*this)[3][3]);
   out[2][3] = -Det3x3((*this)[0][0], (*this)[1][0], (*this)[2][0], (*this)[0][1], (*this)[1][1], (*this)[2][1], (*this)[0][3], (*this)[1][3], (*this)[2][3]);

   out[3][0] = -Det3x3((*this)[1][0], (*this)[2][0], (*this)[3][0], (*this)[1][1], (*this)[2][1], (*this)[3][1], (*this)[1][2], (*this)[2][2], (*this)[3][2]);
   out[3][1] =  Det3x3((*this)[0][0], (*this)[2][0], (*this)[3][0], (*this)[0][1], (*this)[2][1], (*this)[3][1], (*this)[0][2], (*this)[2][2], (*this)[3][2]);
   out[3][2] = -Det3x3((*this)[0][0], (*this)[1][0], (*this)[3][0], (*this)[0][1], (*this)[1][1], (*this)[3][1], (*this)[0][2], (*this)[1][2], (*this)[3][2]);
   out[3][3] =  Det3x3((*this)[0][0], (*this)[1][0], (*this)[2][0], (*this)[0][1], (*this)[1][1], (*this)[2][1], (*this)[0][2], (*this)[1][2], (*this)[2][2]);
}

inline HashMatrix4x4 HashMatrix4x4::Inverse()
{
   HashMatrix4x4 inv,adj;

   double det, invDet;

   // Does the inverse exist
   det = Det4x4();
   if (det == 0.0f)
      return inv;

   // Now figure it out
   Adj4x4(adj);
   invDet = 1.0f / det;

   inv[0][0] = adj[0][0] * invDet;	
   inv[1][0] = adj[1][0] * invDet;
   inv[2][0] = adj[2][0] * invDet;
   inv[3][0] = adj[3][0] * invDet;

   inv[0][1] = adj[0][1] * invDet;
   inv[1][1] = adj[1][1] * invDet;
   inv[2][1] = adj[2][1] * invDet;
   inv[3][1] = adj[3][1] * invDet;

   inv[0][2] = adj[0][2] * invDet;
   inv[1][2] = adj[1][2] * invDet;
   inv[2][2] = adj[2][2] * invDet;
   inv[3][2] = adj[3][2] * invDet;

   inv[0][3] = adj[0][3] * invDet;
   inv[1][3] = adj[1][3] * invDet;
   inv[2][3] = adj[2][3] * invDet;
   inv[3][3] = adj[3][3] * invDet;

   return inv;
}

inline void HashMatrix4x4::Decompose(HashVector3 &translation,HashVector3 &scale,HashVector3 &shear,HashVector3 &rotation)
{
   HashVector3 xVector((*this)[0][0],(*this)[0][1],(*this)[0][2]);
   HashVector3 yVector((*this)[1][0],(*this)[1][1],(*this)[1][2]);
   HashVector3 zVector((*this)[2][0],(*this)[2][1],(*this)[2][2]);

   // TRANSLATION

   translation.x = (*this)[3][0];
   translation.y = (*this)[3][1];
   translation.z = (*this)[3][2];

   // for now just take out the perspective stuff
   (*this)[0][3] = (*this)[1][3] = (*this)[2][3] = 0.0;
   (*this)[3][3] = 1.0;

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

   HashVector3 cross;
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

