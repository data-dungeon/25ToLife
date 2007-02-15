//////////////////////////////////////////////////////////////////////////////////////////
//
//	Vector2 CLASS INLINE METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////

inline Vector2::Vector2() {
	x = 0; y = 0;
}

inline Vector2::Vector2(const double X, const double Y) {
	x = X;	y = Y;
}

inline Vector2::Vector2(const Vector2 &other) {
	x = other.x;	y = other.y;
}

inline Vector2::Vector2(const double other) {
	x = other;
	y = other;
}

inline Vector2::~Vector2(){}

inline Vector2 &Vector2::operator = (const Vector2 &other) {
	x = other.x;
	y = other.y;
	return *this;
}

inline Vector2 &Vector2::operator += (const Vector2 &other) {
	x += other.x;
	y += other.y;
	return *this;
}

inline Vector2 &Vector2::operator -= (const Vector2 &other) {
	x -= other.x;
	y -= other.y;
	return *this;
}

inline Vector2 &Vector2::operator /= (const Vector2 &other) {
	x /= other.x;
	y /= other.y;
	return *this;
}

inline Vector2 &Vector2::operator *= (const Vector2 &other) {
	x *= other.x;
	y *= other.y;
	return *this; 
}

inline Vector2 &Vector2::operator *= (const double scale) {
	x *= scale;
	y *= scale;
	return *this;
}

inline Vector2 &Vector2::operator /= (const double scale){
	x /= scale;
	y /= scale;
	return *this;
}

inline void Vector2::Set(const double X, const double Y)
{
	x = X;
	y = Y;
}

inline double Vector2::Dot(const Vector2 &other) const
{
   return x * other.x + y * other.y;
}

inline double Vector2::Length() const{
	return (double)sqrt(x*x + y*y);
}

inline double Vector2::Lengthsq() const{
	return (double)(x*x + y*y);
}

inline Vector2 &Vector2::Normalize(){
	double len = this->Length();
	if (len)
		*this /= len;
	return *this;
}

inline Vector2 Vector2::Normalized() const {
	return Vector2(*this).Normalize();
}

inline Vector2 &Vector2::Negate(){
	x = -x;
	y = -y;
	return *this;
}

//	warning: slow
inline double Vector2::DistanceTo(const Vector2 &other) const {
	return (Vector2(other.x-x,other.y-y).Length());
}

inline Vector2 Vector2::Absolute() const {
	return Vector2( (double)fabs( x ), (double)fabs( y ));
}

inline Vector2 Vector2::Reciprocal()  const {
	return Vector2( 1.0/x, 1.0/y );
}

inline Vector2 Vector2::operator + (const Vector2 &other) const {
	return Vector2(*this) += other;
}

inline Vector2 Vector2::operator - (const Vector2 &other) const {
	return Vector2(*this) -= other;
}

inline Vector2 Vector2::operator - (void) const {
	return Vector2(*this).Negate();
}

inline Vector2 Vector2::operator * (const Vector2 &other) const {
	return Vector2(*this) *= other;
}

inline Vector2 Vector2::operator * (const double d) const {
	return Vector2(*this) *= d;
}

inline Vector2 Vector2::operator / (const double d) const {
	return Vector2(*this) /= d;
}

inline Vector2 Vector2::operator / (const Vector2 &other) const {
	return Vector2(*this) /= other;
}

inline double Vector2::operator | (const Vector2 &other) const {
	return Dot(other);
}

inline bool Vector2::operator == (const Vector2 &other) const {
	if ((x == other.x) && (y == other.y))
		return true;
	return false;
}

inline bool Vector2::operator != (const Vector2 &other) const {
	return !(*this == other);
}

inline bool Vector2::operator < (const Vector2 &other) const {
	if ((x < other.x) && (y < other.y))
		return true;
	return false;
}

inline double Dot(const Vector2 &a, const Vector2 &b) {
	return a.Dot(b);
}

inline Vector2 operator * (const double s, const Vector2 &a) {
	return a * s;
}

inline double Vector2::operator[]( const int index ) const
{
	double n[3];
	n[0] = x;
	n[1] = y;
	return n[index];
}

inline double &Vector2::operator[]( const int index )
{
	double n[3];
	n[0] = x;
	n[1] = y;
	return n[index];
}