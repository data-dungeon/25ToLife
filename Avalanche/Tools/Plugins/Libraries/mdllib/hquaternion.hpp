#include "hquaternion.h"
#include  <math.h>

inline HQuaternion::HQuaternion( const HVector &vec, double scalar ){
      v = vec;
      w = scalar;
}

inline HQuaternion::HQuaternion( double x, double y, double z, double s ){                                                                  
	v.x = x;
	v.y = y;
	v.z = z;
	w = s;
}

inline HQuaternion::HQuaternion( const HQuaternion &q ){
	v = q.v;
	w = q.w;
}

inline HQuaternion::HQuaternion( double radangle, const HVector &axis ){
	SetAngleAxis( radangle, axis );
}

inline HQuaternion RotateVQ( double angle, const HVector &axis ){
	return HQuaternion( angle/RADIANS, axis );
}

inline HVector HQuaternion::GetAxis() const{
      if ( w != 1.0f )
         return v / (float)sin( acos(w) );
      else
         return v;
}

inline void HQuaternion::SetAngleAxis( double radangle, const HVector &axis ){
	v = axis;
	v.normalize();
	w = (double)cos( radangle / 2 );
	v *= (double)sin( radangle / 2 );
}

inline float HQuaternion::Norm() const
{
	return (float)sqrt(Dot(*this,*this));
}

inline HQuaternion &HQuaternion::Normalize()
{
	*this /= Norm();
	return *this;
}  
 
inline HQuaternion HQuaternion::Normalized() const
{
	return *this / Norm();
}   

inline float HQuaternion::NormalizeLength( void )
{
	float length = Norm();
	if (length)
		*this /= length;
	return length;
}

inline HQuaternion HQuaternion::Inverse() const
{
	return Conjugate(); // / Norm();  ASSUME ALL OUR QUATS ARE NORMALIZED
}

inline HQuaternion HQuaternion::Conjugate() const
{
	return HQuaternion( -v, w );
}


inline HQuaternion &HQuaternion::Negate( void )
{
	v.negate();
	w = -w;
	return *this;
}

inline int HQuaternion::operator == ( const HQuaternion &a ) const {
	return (v == a.v) && (w == a.w);
}

inline int HQuaternion::operator != ( const HQuaternion &a ) const {
	return !(*this == a);
}
 
inline HQuaternion &HQuaternion::operator *= ( const HQuaternion &q ) {
	*this = *this * q;
	return *this;
}

inline HQuaternion &HQuaternion::operator *= ( double scalar ) {
	v*=scalar;
	w*=scalar;
	return *this;
}

inline HQuaternion &HQuaternion::operator /= ( double scalar ) {
	v/=scalar;
	w/=scalar;
	return *this;
}

inline HQuaternion &HQuaternion::operator += ( const HQuaternion &q2 ) {
	v+=q2.v;
	w+=q2.w;
	return *this;
}

inline HQuaternion HQuaternion::operator - ( void ) const {
	return HQuaternion(*this).Negate();
}

inline double &HQuaternion::operator[]( int i ) {
	return (&v.x)[i];
}
   
//inline HQuaternion HQuaternion::operator * ( const HQuaternion &b ) const;

inline HQuaternion HQuaternion::operator * ( double scalar ) const {
	return HQuaternion( v * scalar, w * scalar );
}

inline HQuaternion operator * ( double scalar, const HQuaternion &q ) {
	return HQuaternion( q.v * scalar, q.w * scalar );
}

inline HQuaternion HQuaternion::operator / ( double scalar ) const {
	return HQuaternion( v / scalar, w / scalar );
}

inline HQuaternion HQuaternion::operator + ( const HQuaternion &b ) const {
	return HQuaternion( v + b.v, w + b.w );
}

inline HQuaternion HQuaternion::operator - ( const HQuaternion &b ) const {
	return HQuaternion( v - b.v, w - b.w );
}

inline HVector HQuaternion::operator * ( const HVector &v ) const {
	return (*this * HQuaternion(v,0.0) * Inverse()).v;
}

inline HQuaternion HQuaternion::operator * ( const HQuaternion &b ) const
{
	return HQuaternion( (v^b.v) + (w*b.v) + (v*b.w), w*b.w - (v|b.v) );
}

inline HVector &HVector::operator *= ( const HQuaternion &q )
{
   HQuaternion q2( (q.v^(*this)) + (q.w*(*this)), -q.v|(*this));  // FASTER 24*, 17+
   *this = (q2.v*q.w)-(q2.w*q.v)-(q2.v^q.v);
   return *this;
}

inline HQuaternion &HQuaternion::operator = ( const HQuaternion &other )
{
	v = other.v;
	w = other.w;
	return *this;
}
