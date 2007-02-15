//////////////////////////////////////////////////////////////////////////////////////////
//
//	HVECTOR CLASS INLINE METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////

inline HVector::HVector() {
	x = 0; y = 0; z = 0;
}

inline HVector::HVector(const double X, const double Y, const double Z) {
	x = X;	y = Y;	z = Z;
}

inline HVector::HVector(const HVector &other) {
	x = other.x;	y = other.y;	z = other.z;
}

inline HVector::HVector(const double d) {
	x = d;
	y = d;
	z = d;
}

inline HVector::~HVector(){}

inline HVector &HVector::operator = (const HVector &other) {
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

inline HVector &HVector::operator += (const HVector &other) {
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

inline HVector &HVector::operator -= (const HVector &other) {
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

inline HVector &HVector::operator /= (const HVector &other) {
	x /= other.x;
	y /= other.y;
	z /= other.z;
	return *this;
}

inline HVector &HVector::operator *= (const HVector &other) {
	x *= other.x;
	y *= other.y;
	z *= other.z;
	return *this; 
}

inline HVector &HVector::operator *= (const double scale) {
	x *= scale;
	y *= scale;
	z *= scale;
	return *this;
}

inline HVector &HVector::operator /= (const double scale){
	x /= scale;
	y /= scale;
	z /= scale;
	return *this;
}

inline HVector &HVector::operator ^= (const HVector &other) {
	return (*this = cross(other));
}

inline HVector HVector::cross(const HVector &v) const {
	return HVector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
}

inline double HVector::dot(const HVector &other) const
{
   return x * other.x + y * other.y + z * other.z;
}

inline double HVector::square() const {
	return dot(*this);
}

inline double HVector::length() const{
	return (double)sqrt(x*x + y*y + z*z);
}

inline double HVector::lengthsq() const{
	return (double)(x*x + y*y + z*z);
}

inline HVector &HVector::normalize(){
	double len = this->length();
	if (len)
		*this /= len;
	return *this;
}

inline HVector HVector::normalized() const {
	return HVector(*this).normalize();
}

inline double HVector::normalizeLength() {
      double len = this->length();
      if (len)
         operator /= (len);
      return len;
}

inline HVector &HVector::negate(){
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

//	warning: slow
inline double HVector::distanceTo(const HVector &other) const {
	return (HVector(other.x-x,other.y-y,other.z-z).length());
}

inline bool HVector::isWithinTolerance(const HVector &other,const double tol) {
	HVector distance(other.x-x,other.y-y,other.z-z);
	if (distance.lengthsq() < (tol * tol))
		return true;
	return false;
}

inline void HVector::_clamp( const double digits,double &value) {
	double n = pow((double)10,digits);
	double min = -n, max = n;
	if (value < min)
		value = min;
	if (value > max)
		value = max;
}

inline HVector &HVector::clamp( const double digits ) {
	_clamp(digits,x);
	_clamp(digits,y);
	_clamp(digits,z);

	return *this;	
}

inline HVector &HVector::degreesToRadians() {
	this->x = this->x * 3.14 / 180;
	this->y = this->y * 3.14 / 180;
	this->z = this->z * 3.14 / 180;
	return *this;
}

inline HVector &HVector::radiansToDegrees() {
	this->x = this->x * 180 / 3.14;
	this->y = this->y * 180 / 3.14;
	this->z = this->z * 180 / 3.14;
	return *this;
}

inline bool HVector::isParallel(const HVector b, const double tol) const {
	double tolsq = tol*tol;
	if ((this->normalized() ^ b.normalized()).lengthsq() < tolsq)
		return true;
	return false;
}

inline bool HVector::isSameDirection(const HVector b, const double tol) const {
	double dotproduct = (this->normalized()).dot(b.normalized());
	if ((dotproduct > (1-tol))&&(dotproduct < 1))
		return true;
	return false;
}

inline bool HVector::isParallelAndSameDirection(const HVector b, const double tol) {
	if (this->isParallel(b,tol))
		if (this->isSameDirection(b,tol))
			return true;
	return false;
}

inline HVector HVector::absolute() const {
	return HVector( (double)fabs( x ), (double)fabs( y ), (double)fabs( z ));
}

inline HVector HVector::reciprocal()  const {
	return HVector( 1.0/x, 1.0/y, 1.0/z );
}

inline void HVector::crossSelf(const HVector &other)     {
	*this = cross( other );
}

inline HVector HVector::operator + (const HVector &other) const {
	return HVector(*this) += other;
}

inline HVector HVector::operator - (const HVector &other) const {
	return HVector(*this) -= other;
}

inline HVector HVector::operator - (void) const {
	return HVector(*this).negate();
}

inline HVector HVector::operator * (const HVector &other) const {
	return HVector(*this) *= other;
}

inline HVector HVector::operator * (const double d) const {
	return HVector(*this) *= d;
}

inline HVector HVector::operator / (const double d) const {
	return HVector(*this) /= d;
}

inline HVector HVector::operator / (const HVector &other) const {
	return HVector(*this) /= other;
}

inline double HVector::operator | (const HVector &other) const {
	return dot(other);
}

inline HVector HVector::operator ^ (const HVector &other) const {
	return cross(other);
}

inline bool HVector::operator == (const HVector &other) const {
	if ((x == other.x) && (y == other.y) && (z == other.z))
		return true;
	return false;
}

inline bool HVector::operator != (const HVector &other) const {
	return !(*this == other);
}

inline bool HVector::operator < (const HVector &other) const {
	if ((x < other.x) && (y < other.y) && (z < other.z))
		return true;
	return false;
}

inline HVector cross(const HVector &a,const HVector &b) {
	return a.cross(b);
}

inline double dot(const HVector &a, const HVector &b) {
	return a.dot(b);
}

inline HVector operator * (const double s, const HVector &a) {
	return a * s;
}

/////////////////////////////////////////////////////////////////////////////////////////
// HVectorArray
/////////////////////////////////////////////////////////////////////////////////////////
inline HVectorArray::HVectorArray() {}

inline HVectorArray::HVectorArray(const unsigned size) {
	classList.resize(size);
}

inline HVectorArray::~HVectorArray() {}

inline void HVectorArray::clear() {
	classList.clear();
}

inline unsigned HVectorArray::length() const {
	return classList.size();
}

inline void HVectorArray::resize(const unsigned size) {
	classList.resize(size);
}

inline unsigned HVectorArray::append(const HVector &pos) {
	classList.push_back(pos);
	return classList.size()-1;
}

//	warning: slow
inline unsigned HVectorArray::appendWeld(const HVector &pos,const double tolerance) {
	unsigned index;
	VectorVectorIt location;

	for (location = classList.begin(); location != classList.end(); location++) {
		if (location->isWithinTolerance(pos,tolerance)) {
			index = location - classList.begin();
			return index;
		}
	}
	classList.push_back(pos);
	return classList.size()-1;
}

//	warning: slow
inline bool HVectorArray::valueExists(const HVector &pos,unsigned &index) const {
	VectorVector::const_iterator location;
	location = std::find(classList.begin(),classList.end(),pos);
	if (location != classList.end()) {
		index = location - classList.begin();
		return true;
	}
	return false;
}

inline HVector *HVectorArray::get(const unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range. [HVector]");
	}
	return &classList[index];
}

inline HVector *HVectorArray::begin() {
	return &classList.front();
}

inline HVector *HVectorArray::end() {
	return &classList.back();
}

inline HVector HVectorArray::operator[](const unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HVectorArray]");
	}
	return classList[index];
}

inline HVector &HVectorArray::operator[](const unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HVectorArray]");
	}	
	return classList[index];
}

inline HVectorArray &HVectorArray::operator = ( const HVectorArray &other ){
	classList = other.classList;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
// HVectorIterator
/////////////////////////////////////////////////////////////////////////////////////////

inline HVectorIterator::HVectorIterator() {}

inline HVectorIterator::HVectorIterator(HVectorArray &other) {
	pointTo = &other;
	cl_iterator = pointTo->classList.begin();
}

inline HVectorIterator::~HVectorIterator() {}

inline bool HVectorIterator::isDone() {
	return (cl_iterator == pointTo->classList.end());
}

inline void HVectorIterator::reset(HVectorArray &other) {
	pointTo = &other;
	cl_iterator = pointTo->classList.begin();
}

inline void HVectorIterator::reset() {
	cl_iterator = pointTo->classList.begin();
}

inline void HVectorIterator::insert(const HVector &other) {
	pointTo->classList.insert(cl_iterator,other);
}

inline unsigned HVectorIterator::index() const {
	return (cl_iterator-pointTo->classList.begin());
}

inline HVectorIterator HVectorIterator::operator ++(int) {
	cl_iterator++;
	return *this;
}

inline HVectorIterator HVectorIterator::operator --(int) {
	cl_iterator--;
	return *this;
}

inline HVector *HVectorIterator::operator * (){
	return &(*cl_iterator);
}

inline std::ostream &operator << (std::ostream &os,const HVector &other){
	os << "[" << other.x << "," << other.y << "," << other.z << "]";
	return os;
}

inline std::ostream &operator << (std::ostream &os,const HVectorArray &other){
	for (unsigned i = 0; i < other.length(); i++)
		os << other[i];
	return os;
}