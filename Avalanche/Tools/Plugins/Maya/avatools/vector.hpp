//////////////////////////////////////////////////////////////////////////////////////////
//
//	Vector CLASS INLINE METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////

inline Vector::Vector() {
	x = 0; y = 0; z = 0;
}

inline Vector::Vector(const double X, const double Y, const double Z) {
	x = X;	y = Y;	z = Z;
}

inline Vector::Vector(const Vector &other) {
	x = other.x;	y = other.y;	z = other.z;
}

inline Vector::Vector(const double scalar) {
	x = scalar;
	y = scalar;
	z = scalar;
}

inline Vector::~Vector(){}

inline Vector &Vector::operator = (const Vector &other) {
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

inline Vector &Vector::operator += (const Vector &other) {
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

inline Vector &Vector::operator -= (const Vector &other) {
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

inline Vector &Vector::operator /= (const Vector &other) {
	x /= other.x;
	y /= other.y;
	z /= other.z;
	return *this;
}

inline Vector &Vector::operator *= (const Vector &other) {
	x *= other.x;
	y *= other.y;
	z *= other.z;
	return *this; 
}

inline Vector &Vector::operator *= (const double scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}

inline Vector &Vector::operator /= (const double scalar){
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return *this;
}

inline Vector &Vector::operator ^= (const Vector &other) {
	return (*this = Cross(other));
}

inline void Vector::Set(const double X, const double Y, const double Z)
{
	x = X;
	y = Y;
	z = Z;
}

inline Vector Vector::Cross(const Vector &other) const {
	return Vector(y*other.z - z*other.y, z*other.x - x*other.z, x*other.y - y*other.x);
}

inline double Vector::Dot(const Vector &other) const
{
   return x * other.x + y * other.y + z * other.z;
}

inline double Vector::Square() const {
	return Dot(*this);
}

inline double Vector::Length() const{
	return (double)sqrt(x*x + y*y + z*z);
}

inline double Vector::LengthSq() const{
	return (double)(x*x + y*y + z*z);
}

inline Vector &Vector::Normalize(){
	double len = this->Length();
	if (len)
		*this /= len;
	return *this;
}

inline Vector Vector::Normalized() const {
	return Vector(*this).Normalize();
}

inline double Vector::NormalizeLength() {
      double len = this->Length();
      if (len)
         operator /= (len);
      return len;
}

inline Vector &Vector::Negate(){
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

//	warning: slow
inline double Vector::DistanceTo(const Vector &other) const {
	return (Vector(other.x-x,other.y-y,other.z-z).Length());
}

inline bool Vector::IsWithinTolerance(const Vector &other,const double tol) {
	Vector distance(other.x-x,other.y-y,other.z-z);
	if (distance.LengthSq() < (tol * tol))
		return true;
	return false;
}

inline bool Vector::IsParallel(const Vector &other) const {
	double dot = (this->Normalized()).Dot(other.Normalized());
	if (dot == 1.0)
		return true;
	return false;
}

inline bool Vector::IsParallel(const Vector &other, const double tol) const {
	double dot = (this->Normalized()).Dot(other.Normalized());
	if ((dot > -tol) && (dot < tol))
		return true;
	return false;
}

inline bool Vector::IsSameDirection(const Vector &other, const double tol) const {
	double dot = (this->Normalized()).Dot(other.Normalized());
	if ((dot > 1.0 - tol)&&(dot < 1.0 + tol))
		return true;
	return false;
}

inline bool Vector::IsParallelAndSameDirection(const Vector &other, const double tol) {
	if (this->IsParallel(other,tol))
		if (this->IsSameDirection(other,tol))
			return true;
	return false;
}

inline Vector Vector::Absolute() const {
	return Vector( (double)fabs( x ), (double)fabs( y ), (double)fabs( z ));
}

inline Vector Vector::Reciprocal()  const {
	return Vector( 1.0/x, 1.0/y, 1.0/z );
}

inline void Vector::CrossSelf(const Vector &other)     {
	*this = Cross( other );
}

// returns radians
inline double Vector::AngleBetweenVectors(const Vector &other)
{
	return acos((*this).Dot(other));	// angle = inverse cosine ( A dot B )
}

// roll is ignored
inline void Vector::GetRotation(Vector &rotation)
{
	rotation.y = RADTODEG * atan2( this->x, this->z );
	rotation.x = RADTODEG * atan2( -this->y, sqrt(this->x*this->x + this->z*this->z));
}

inline Vector Vector::operator + (const Vector &other) const {
	return Vector(*this) += other;
}

inline Vector Vector::operator - (const Vector &other) const {
	return Vector(*this) -= other;
}

inline Vector Vector::operator - (void) const {
	return Vector(*this).Negate();
}

inline Vector Vector::operator * (const Vector &other) const {
	return Vector(*this) *= other;
}

inline Vector Vector::operator * (const double scalar) const {
	return Vector(*this) *= scalar;
}

inline Vector Vector::operator / (const double scalar) const {
	return Vector(*this) /= scalar;
}

inline Vector Vector::operator / (const Vector &other) const {
	return Vector(*this) /= other;
}

inline double Vector::operator | (const Vector &other) const {
	return Dot(other);
}

inline Vector Vector::operator ^ (const Vector &other) const {
	return Cross(other);
}

inline bool Vector::operator == (const Vector &other) const {
	if ((x == other.x) && (y == other.y) && (z == other.z))
		return true;
	return false;
}

inline bool Vector::operator != (const Vector &other) const {
	return !(*this == other);
}

inline bool Vector::operator < (const Vector &other) const {
	if ((x < other.x) && (y < other.y) && (z < other.z))
		return true;
	return false;
}

inline Vector Cross(const Vector &a,const Vector &b) {
	return a.Cross(b);
}

inline double Dot(const Vector &a, const Vector &b) {
	return a.Dot(b);
}

inline Vector operator * (const double scalar, const Vector &other) {
	return other * scalar;
}

inline double Vector::operator[]( const int index ) const
{
   return (&x)[index];
}

inline double &Vector::operator[]( const int index )
{
   return (&x)[index];
}