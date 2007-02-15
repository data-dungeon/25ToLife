//////////////////////////////////////////////////////////////////////////////////////////
//
//	HashVector2 CLASS INLINE METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////

inline HashVector2::HashVector2() {
	x = 0; y = 0;
}

inline HashVector2::HashVector2(const double X, const double Y) {
	x = X;	y = Y;
}

inline HashVector2::HashVector2(const HashVector2 &other) {
	x = other.x;	y = other.y;
}

inline HashVector2::HashVector2(const double other) {
	x = other;
	y = other;
}

inline HashVector2::~HashVector2(){}

inline HashVector2 &HashVector2::operator = (const HashVector2 &other) {
	x = other.x;
	y = other.y;
	return *this;
}

inline HashVector2 &HashVector2::operator += (const HashVector2 &other) {
	x += other.x;
	y += other.y;
	return *this;
}

inline HashVector2 &HashVector2::operator -= (const HashVector2 &other) {
	x -= other.x;
	y -= other.y;
	return *this;
}

inline HashVector2 &HashVector2::operator /= (const HashVector2 &other) {
	x /= other.x;
	y /= other.y;
	return *this;
}

inline HashVector2 &HashVector2::operator *= (const HashVector2 &other) {
	x *= other.x;
	y *= other.y;
	return *this; 
}

inline HashVector2 &HashVector2::operator *= (const double scale) {
	x *= scale;
	y *= scale;
	return *this;
}

inline HashVector2 &HashVector2::operator /= (const double scale){
	x /= scale;
	y /= scale;
	return *this;
}

inline void HashVector2::Set(const double X, const double Y)
{
	x = X;
	y = Y;
}

inline double HashVector2::Dot(const HashVector2 &other) const
{
	return x * other.x + y * other.y;
}

inline double HashVector2::Length() const{
	return (double)sqrt(x*x + y*y);
}

inline double HashVector2::LengthSq() const{
	return (double)(x*x + y*y);
}

inline HashVector2 &HashVector2::Normalize(){
	double len = this->Length();
	if (len)
		*this /= len;
	return *this;
}

inline HashVector2 HashVector2::Normalized() const {
	return HashVector2(*this).Normalize();
}

inline HashVector2 &HashVector2::Negate()
{
	x = -x;
	y = -y;
	return *this;
}

inline double HashVector2::DistanceTo(const HashVector2 &other) const 
{
   return (HashVector2(other.x-x,other.y-y).Length());
}

inline double HashVector2::Sum(void) const
{
	return x+y;
}


inline HashVector2 HashVector2::Absolute() const 
{
	return HashVector2( (double)fabs( x ), (double)fabs( y ));
}

inline HashVector2 HashVector2::Reciprocal()  const {
	return HashVector2( 1.0f/x, 1.0f/y );
}

inline HashVector2 HashVector2::operator + (const HashVector2 &other) const {
	return HashVector2(*this) += other;
}

inline HashVector2 HashVector2::operator - (const HashVector2 &other) const {
	return HashVector2(*this) -= other;
}

inline HashVector2 HashVector2::operator - (void) const {
	return HashVector2(*this).Negate();
}

inline HashVector2 HashVector2::operator * (const HashVector2 &other) const {
	return HashVector2(*this) *= other;
}

inline HashVector2 HashVector2::operator * (const double d) const {
	return HashVector2(*this) *= d;
}

inline HashVector2 HashVector2::operator / (const double d) const {
	return HashVector2(*this) /= d;
}

inline HashVector2 HashVector2::operator / (const HashVector2 &other) const {
	return HashVector2(*this) /= other;
}

inline double HashVector2::operator | (const HashVector2 &other) const {
	return Dot(other);
}

inline bool HashVector2::operator == (const HashVector2 &other) const {
	if ((x == other.x) && (y == other.y))
		return true;
	return false;
}

inline bool HashVector2::operator != (const HashVector2 &other) const {
	return !(*this == other);
}

inline bool HashVector2::operator < (const HashVector2 &other) const {
	if ((x < other.x) && (y < other.y))
		return true;
	return false;
}

inline double Dot(const HashVector2 &a, const HashVector2 &b) {
	return a.Dot(b);
}

inline HashVector2 operator * (const double s, const HashVector2 &a) {
	return a * s;
}

inline double HashVector2::operator[]( const int index ) const
{
	double n[3];
	n[0] = x;
	n[1] = y;
	return n[index];
}

inline double &HashVector2::operator[]( const int index )
{
	double n[3];
	n[0] = x;
	n[1] = y;
	return n[index];
}

inline void HashVector2::Tween(HashVector2 &pointFill,const HashVector2 &point1,const HashVector2 &point2,double percent)
{
	pointFill = point1 + ((point2-point1) * percent);
}

inline void HashVector2::Tween(HashVector2 &pointFill,const HashVector2 &point1,const HashVector2 &control1,const HashVector2 &control2,const HashVector2 &point2,double t)
{
	double s(((double)1)-t);
	// calculate squares
	double t2(t * t);
	double t3(t2 * t);
	double s2(s * s);
	double s3(s2 * s);
	double t2s3(t2*s*((double)3));
	double ts23(t*s2*((double)3));

	// equation is t^3*p2 + 3*t^2*s*c2 + 3*t*s^2*c1 + s^3*p1
	pointFill = (t3*point2)+(t2s3*control2)+(ts23*control1)+(s3*point1);
}

// Spherical Linear interpolation
void HashVector2::Slerp(HashVector2 &pointFill,const HashVector2 &point1,const HashVector2 &point2,double percent)
{
	double cos_a(point1.Dot(point2));
	double angle((double)acos((double)cos_a));
	if (angle> -0.005 && angle < 0.005)
	{
		pointFill = point1;
		return;
	}
	double sin_a((double)sin((double)angle));
	double invSin(((double)1)/sin_a);
	double coef0((double)sin((double)((((double)1)-percent)*angle))*invSin);
	double coef1((double)sin((double)(percent*angle))*invSin);
	pointFill = point1*coef0 + point2*coef1;
}

// Spherical Bezier interpolation
void HashVector2::Sberp(HashVector2 &pointFill,const HashVector2 &point1,const HashVector2 &control1,const HashVector2 &control2,const HashVector2 &point2,double percent)
{
	HashVector2 p12,p23,p34,p1223,p2334;
	Slerp(p12,point1,control1,percent);
	Slerp(p23,control1,control2,percent);
	Slerp(p34,control2,point2,percent);

	Slerp(p1223,p12,p23,percent);
	Slerp(p2334,p23,p34,percent);
	Slerp(pointFill,p1223,p2334,percent);
}
