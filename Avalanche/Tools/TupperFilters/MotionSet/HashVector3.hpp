//////////////////////////////////////////////////////////////////////////////////////////
//
//	HashVector3 CLASS INLINE METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////

inline HashVector3::HashVector3()
{
	x = 0; y = 0; z = 0;
}

inline HashVector3::HashVector3(const double X, const double Y, const double Z)
{
	x = X;	y = Y;	z = Z;
}

inline HashVector3::HashVector3(const HashVector3 &other)
{
	x = other.x;	y = other.y;	z = other.z;
}

inline HashVector3::HashVector3(const double scalar)
{
	x = scalar;
	y = scalar;
	z = scalar;
}

inline HashVector3::~HashVector3()
{
}

inline HashVector3 &HashVector3::operator = (const HashVector3 &other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

inline HashVector3 &HashVector3::operator += (const HashVector3 &other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

inline HashVector3 &HashVector3::operator -= (const HashVector3 &other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

inline HashVector3 &HashVector3::operator /= (const HashVector3 &other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;
	return *this;
}

inline HashVector3 &HashVector3::operator *= (const HashVector3 &other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;
	return *this; 
}

inline HashVector3 &HashVector3::operator *= (const double scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}

inline HashVector3 &HashVector3::operator /= (const double scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return *this;
}

inline HashVector3 &HashVector3::operator ^= (const HashVector3 &other)
{
	return (*this = Cross(other));
}

inline void HashVector3::Set(const double X, const double Y, const double Z)
{
	x = X;
	y = Y;
	z = Z;
}

inline HashVector3 HashVector3::Cross(const HashVector3 &other) const
{
	return HashVector3(y*other.z - z*other.y, z*other.x - x*other.z, x*other.y - y*other.x);
}

inline double HashVector3::Dot(const HashVector3 &other) const
{
	return x * other.x + y * other.y + z * other.z;
}

inline double HashVector3::Square() const
{
	return Dot(*this);
}

inline double HashVector3::Length() const
{
	return (double)sqrt(x*x + y*y + z*z);
}

inline double HashVector3::LengthSq() const
{
	return (double)(x*x + y*y + z*z);
}

inline HashVector3 &HashVector3::Normalize()
{
	double len = this->Length();
	if (len)
		*this /= len;
	return *this;
}

inline HashVector3 HashVector3::Normalized() const
{
	return HashVector3(*this).Normalize();
}

inline double HashVector3::NormalizeLength()
{
	double len = this->Length();
	if (len)
		operator /= (len);
	return len;
}

inline HashVector3 &HashVector3::Negate()
{
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

inline double HashVector3::DistanceTo(const HashVector3 &other) const
{
	return (HashVector3(other.x-x,other.y-y,other.z-z).Length());
}

inline HashVector3 HashVector3::Absolute() const
{
	return HashVector3( (double)fabs( x ), (double)fabs( y ), (double)fabs( z ));
}

inline HashVector3 HashVector3::Reciprocal()  const
{
	return HashVector3( 1.0f/x, 1.0f/y, 1.0f/z );
}

inline void HashVector3::CrossSelf(const HashVector3 &other)
{
	*this = Cross( other );
}

inline HashVector3 HashVector3::operator + (const HashVector3 &other) const
{
	return HashVector3(*this) += other;
}

inline HashVector3 HashVector3::operator - (const HashVector3 &other) const
{
	return HashVector3(*this) -= other;
}

inline HashVector3 HashVector3::operator - (void) const
{
	return HashVector3(*this).Negate();
}

inline HashVector3 HashVector3::operator * (const HashVector3 &other) const
{
	return HashVector3(*this) *= other;
}

inline HashVector3 HashVector3::operator * (const double scalar) const
{
	return HashVector3(*this) *= scalar;
}

inline HashVector3 HashVector3::operator / (const double scalar) const
{
	return HashVector3(*this) /= scalar;
}

inline HashVector3 HashVector3::operator / (const HashVector3 &other) const
{
	return HashVector3(*this) /= other;
}

inline double HashVector3::operator | (const HashVector3 &other) const
{
	return Dot(other);
}

inline HashVector3 HashVector3::operator ^ (const HashVector3 &other) const
{
	return Cross(other);
}

inline bool HashVector3::operator == (const HashVector3 &other) const
{
	if ((x == other.x) && (y == other.y) && (z == other.z))
		return true;
	return false;
}

inline bool HashVector3::operator != (const HashVector3 &other) const
{
	return !(*this == other);
}

inline bool HashVector3::operator < (const HashVector3 &other) const
{
	if ((x < other.x) && (y < other.y) && (z < other.z))
		return true;
	return false;
}

inline HashVector3 Cross(const HashVector3 &a,const HashVector3 &b)
{
	return a.Cross(b);
}

inline double Dot(const HashVector3 &a, const HashVector3 &b)
{
	return a.Dot(b);
}

inline HashVector3 operator * (const double scalar, const HashVector3 &other)
{
	return other * scalar;
}

inline double HashVector3::operator[]( const int index ) const
{
	return (&x)[index];
}

inline double &HashVector3::operator[]( const int index )
{
	return (&x)[index];
}