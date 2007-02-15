/////////////////////////////////////////////////////////////////////////////////////////
// HDOUBLEARRAY INLINE METHODS
/////////////////////////////////////////////////////////////////////////////////////////
#ifndef __HSTATUS_H
#include "hstatus.h"
#endif
#ifndef __HMACROS_H
#include "hmacros.h"
#endif
#include <algorithm>

inline HDoubleArray::HDoubleArray() {}

inline HDoubleArray::HDoubleArray(const unsigned size){
	classList.reserve(size);
}

inline HDoubleArray::~HDoubleArray() {}

inline void HDoubleArray::clear() {
	classList.clear();
}

inline void HDoubleArray::remove(const unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HDoubleArray]");
	}
	classList.erase(classList.begin() + index);
}

inline void HDoubleArray::resize(const unsigned size) {
	classList.resize(size);
}

inline void HDoubleArray::reserve(const unsigned size) {
	classList.reserve(size);
}

inline void HDoubleArray::sort() {
	std::sort(classList.begin(),classList.end());
}

inline unsigned HDoubleArray::length() const {
	return classList.size();
}

inline double HDoubleArray::begin() const {
	return classList.front();
}

inline double HDoubleArray::end() const {
	return classList.back();
}

inline void HDoubleArray::append(const double value) {
	classList.push_back(value);
}

inline void HDoubleArray::append(const HDoubleArray &other) {
	for (unsigned i = 0; i < other.length(); i++)
		classList.push_back(other[i]);
}

inline bool HDoubleArray::valueExists(const double value,unsigned &index) {
	DoubleVectorIt location;
	location = std::find(classList.begin(),classList.end(),value);
	if (location != classList.end()) {
		index = location - classList.begin();
		return true;
	}
	return false;
}

inline double HDoubleArray::maxValue() {
	if (this->classList.size() != 0) {
		DoubleVectorIt location;
		location = std::max_element(classList.begin(),classList.end());
		return *location;
	}
	else 
		return 0;
}

inline HDoubleArray &HDoubleArray::unify(const HDoubleArray &a,const HDoubleArray &b) {
	unsigned a_ix = 0, b_ix = 0;

	while ((a_ix < a.length())&&(b_ix < b.length())) {
		if (a[a_ix] < b[b_ix]) {
			classList.push_back(a[a_ix]);
			a_ix++;
		} else if (a[a_ix] == b[b_ix]) {
			classList.push_back(a[a_ix]);
			a_ix++;
			b_ix++;
		} else {
			classList.push_back(b[b_ix]);
			b_ix++;
		}
	}

	while (a_ix < a.length()) {
		classList.push_back(a[a_ix]);
		a_ix++;
	}

	while (b_ix < b.length()) {
		classList.push_back(b[b_ix]);
		b_ix++;
	}
	return *this;
}

inline double HDoubleArray::get(const unsigned index) const {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HDoubleArray]");
	}
	return classList[index];
}

inline void HDoubleArray::set(const unsigned index,const double value) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HDoubleArray]");
	}
	this->classList[index] = value;
}

inline double HDoubleArray::operator[](const unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HDoubleArray]");
	}
	return classList[index];
}

inline double &HDoubleArray::operator[](const unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HDoubleArray]");
	}
	return classList[index];
}

inline HDoubleArray &HDoubleArray::operator = (const HDoubleArray &other){
	classList = other.classList;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
// HDoubleIterator
/////////////////////////////////////////////////////////////////////////////////////////

inline HDoubleIterator::HDoubleIterator() {}

inline HDoubleIterator::HDoubleIterator(HDoubleArray &other) {
	pointTo = &other;
	cl_iterator = pointTo->classList.begin();
}

inline HDoubleIterator::~HDoubleIterator() {}

inline bool HDoubleIterator::isDone() {
	return (cl_iterator == pointTo->classList.end());
}

inline void HDoubleIterator::reset(HDoubleArray &other) {
	pointTo = &other;
	cl_iterator = pointTo->classList.begin();
}

inline void HDoubleIterator::reset() {
	cl_iterator = pointTo->classList.begin();
}

inline void HDoubleIterator::insert(const double value){
	pointTo->classList.insert(cl_iterator,value);
}

inline unsigned HDoubleIterator::index() {
	return (cl_iterator-pointTo->classList.begin());
}

inline HDoubleIterator HDoubleIterator::operator ++(int) {
	cl_iterator++;
	return *this;
}

inline HDoubleIterator HDoubleIterator::operator --(int) {
	cl_iterator--;
	return *this;
}

inline double *HDoubleIterator::operator * (){
	return &(*cl_iterator);
}

inline std::ostream &operator << (std::ostream &os,HDoubleArray &other){
	HDoubleIterator iter(other);
	for (;!iter.isDone(); iter++)
		os << **iter << " ";
	return os;
}
