//////////////////////////////////////////////////////////////////////////////////////////
//
//	HVECTOR2D CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////
#include <math.h>
#include "hstatus.h"
#include "hmacros.h"

inline HVector2D::HVector2D(){
	x = 0; y = 0;
}

inline HVector2D::HVector2D(double X, double Y) {
	x = X;	y = Y;
}

inline HVector2D::HVector2D(const HVector2D &other){
	x = other.x;	y = other.y;
}

inline HVector2D::HVector2D(const double other){
	x = other;
	y = other;
}

inline HVector2D::~HVector2D(){}

inline HVector2D &HVector2D::operator = (const HVector2D &other){
	x = other.x;
	y = other.y;
	return *this;
}

inline HVector2D &HVector2D::operator += (const HVector2D &other){
	x += other.x;
	y += other.y;
	return *this;
}

inline HVector2D &HVector2D::operator -= (const HVector2D &other){
	x -= other.x;
	y -= other.y;
	return *this;
}

inline HVector2D &HVector2D::operator /= (const HVector2D &other){
	x /= other.x;
	y /= other.y;
	return *this;
}

inline HVector2D &HVector2D::operator *= (const HVector2D &other){
	x *= other.x;
	y *= other.y;
	return *this; 
}

inline HVector2D &HVector2D::operator *= (const double scale){
	x *= scale;
	y *= scale;
	return *this;
}

inline HVector2D &HVector2D::operator /= (const double scale){
	x /= scale;
	y /= scale;
	return *this;
}

inline double HVector2D::dot(const HVector2D &other) const {
   return x * other.x + y * other.y;
}

inline double HVector2D::square() const {
	return dot(*this);
}

inline double HVector2D::length() const{
	return (double)sqrt(x*x + y*y);
}

inline HVector2D &HVector2D::normalize(){
	double len = length();
	if (len)
		*this /= len;
	return *this;
}

inline HVector2D HVector2D::normalized() const {
	return HVector2D(*this).normalize();
}
/*
inline double HVector2D::normalizeLength() const {
	double len = length();
	if (len)
		operator /= (len);
	return len;
}
*/
inline HVector2D & HVector2D::negate( void ){
	x = -x;
	y = -y;
	return *this;
}

inline double HVector2D::distanceTo( HVector2D p) const {
	HVector2D n(p.x-x,p.y-y);
	return (n.length());
}

inline HVector2D HVector2D::absolute() const {
	return HVector2D( (double)fabs( x ), (double)fabs( y ));
}

inline HVector2D HVector2D::reciprocal()  const {
	return HVector2D( 1.0/x, 1.0/y);
}

inline HVector2D HVector2D::operator + (const HVector2D &other) const {
	return HVector2D(*this) += other;
}

inline HVector2D HVector2D::operator - (const HVector2D &other) const {
	return HVector2D(*this) -= other;
}

inline HVector2D HVector2D::operator - (void) const {
	return HVector2D(*this).negate();
}

inline HVector2D HVector2D::operator * (const HVector2D &other) const {
	return HVector2D(*this) *= other;
}

inline HVector2D HVector2D::operator * (double scale) const {
	return HVector2D(*this) *= scale;
}

inline HVector2D HVector2D::operator / (double scale) const {
	return HVector2D(*this) /= scale;
}

inline HVector2D HVector2D::operator / (const HVector2D &other) const {
	return HVector2D(*this) /= other;
}

inline double HVector2D::operator | (const HVector2D &other) const {
	return dot(other);
}

inline bool HVector2D::operator == (const HVector2D &other) const {
	return ((x == other.x)&&(y == other.y));
}

inline bool HVector2D::operator == (const double other) const {
	return ((x == other)&&(y == other));
}

inline bool HVector2D::operator != (const HVector2D &other) const {
	return !(*this == other);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//	HVECTOR2DARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

inline HVector2DArray::HVector2DArray() {}

inline HVector2DArray::~HVector2DArray() {}

inline void HVector2DArray::clear() {
	classList.clear();
}

inline unsigned HVector2DArray::length() const {
	return classList.size();
}

inline unsigned HVector2DArray::append(const HVector2D &pos) {
	classList.push_back(pos);
	return classList.size()-1;
}

inline void HVector2DArray::resize(const unsigned size) {
	classList.resize(size);
}

inline HVector2D *HVector2DArray::get(const unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range. [HVector2D]");
	}
	return &classList[index];
}

inline HVector2D HVector2DArray::operator[](const unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HVector2DArray]");
	}
	return classList[index];
}

inline HVector2D &HVector2DArray::operator[](const unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HVector2DArray]");
	}
	
	return classList[index];
}

inline HVector2DArray &HVector2DArray::operator = (const HVector2DArray &other){
	classList = other.classList;
	return *this;
}

inline double dot( const HVector2D &a, const HVector2D &b ) {
	return a.dot(b);
}

inline HVector2D operator * ( const double s, const HVector2D &a ) {
	return a * s;
}

/////////////////////////////////////////////////////////////////////////////////////////
// HVector2DIterator

inline HVector2DIterator::HVector2DIterator() {}

inline HVector2DIterator::HVector2DIterator(HVector2DArray &other) {
	pointTo = &other;
	cl_iterator = pointTo->classList.begin();
}

inline HVector2DIterator::~HVector2DIterator() {}

inline bool HVector2DIterator::isDone() {
	return (cl_iterator == pointTo->classList.end());
}

inline void HVector2DIterator::reset(HVector2DArray &other) {
	pointTo = &other;
	cl_iterator = pointTo->classList.begin();
}

inline void HVector2DIterator::reset() {
	cl_iterator = pointTo->classList.begin();
}

inline void HVector2DIterator::insert(const HVector2D pos){
	pointTo->classList.insert(cl_iterator,pos);
}

inline unsigned HVector2DIterator::index() {
	return (cl_iterator-pointTo->classList.begin());
}

inline HVector2DIterator HVector2DIterator::operator ++(int) {
	cl_iterator++;
	return *this;
}

inline HVector2DIterator HVector2DIterator::operator --(int) {
	cl_iterator--;
	return *this;
}

inline HVector2D *HVector2DIterator::operator * (){
	return &(*cl_iterator);
}

inline std::ostream &operator << (std::ostream &os,HVector2D &other){
	os << "[" << other.x << "," << other.y << "]";
	return os;
}

inline std::ostream &operator << (std::ostream &os,HVector2DArray &other){
	HVector2DIterator iter(other);
	for (;!iter.isDone();iter++)
		os << **iter;
	return os;
}