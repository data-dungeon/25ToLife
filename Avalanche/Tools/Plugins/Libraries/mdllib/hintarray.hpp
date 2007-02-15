/////////////////////////////////////////////////////////////////////////////////////////
// HINTARRAY INLINE METHODS
/////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HSTATUS_H
#include "hstatus.h"
#endif
#ifndef __HMACROS_H
#include "hmacros.h"
#endif
#include <algorithm>

inline HIntArray::HIntArray()
{
	classList.clear();
	imap.clear();
}

inline HIntArray::~HIntArray() {}

inline void HIntArray::clear() {
	classList.clear();
	imap.clear();
}

inline void HIntArray::remove(const unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HIntarray]");
	}
	imap.deleteKey((*this)[index]);
	classList.erase(classList.begin() + index);
}

//inline void HIntArray::resize(const unsigned size) {
//	classList.resize(size);
//}

inline void HIntArray::sort() {
	std::sort(classList.begin(),classList.end());
}

inline unsigned HIntArray::length() const {
	return classList.size();
}

inline int HIntArray::front() const {
	return classList.front();
}

inline int HIntArray::back() const {
	return classList.back();
}

inline void HIntArray::append(const int key) {
	classList.push_back(key);
	imap.setValue(key,classList.size());
}

//inline void HIntArray::append(const HIntArray &keys) {
//	for (unsigned i = 0; i < keys.length(); i++)
//	{
//		classList.push_back(keys[i]);
//		imap.setValue(keys[i],classList.size());
//	}
//}

inline bool HIntArray::valueExists(const int key,unsigned &index){
//	IntVectorIt location;
//	location = (int *)std::find(classList.begin(),classList.end(),key);
//	if (location != classList.end()) {
//		index = location - classList.begin();
//		return true;
//	}

	if (imap.hasKey(key))
	{
		index = (unsigned)imap.getValue(key);
		return true;
	}
	return false;
}

inline void HIntArray::commonElements(HIntArray &b,HIntArray &c) {
	IntVectorIt iter;
	bool exists;
	unsigned index;
	c.clear();
	for (iter = classList.begin(); iter != classList.end(); iter++) {
		exists = b.valueExists(*iter,index);
		if (exists)
			c.append(*iter);
	}
}

inline void HIntArray::commonElements(HIntArray &b,std::vector<int> &c) {
	IntVectorIt iter;
	bool exists;
	unsigned index;
	c.clear();
	for (iter = classList.begin(); iter != classList.end(); iter++) {
		exists = b.valueExists(*iter,index);
		if (exists)
			c.push_back(*iter);
	}
}

inline void HIntArray::commonElements(HIntArray &b,HIntArray &c,HIntArray &idxs) {
	IntVectorIt iter;
	bool exists;
	unsigned index;
	c.clear(); idxs.clear();
	for (iter = classList.begin(); iter != classList.end(); iter++) {
		exists = b.valueExists(*iter,index);
		if (exists) {
			c.append(*iter);
			idxs.append(iter-classList.begin());
		}
	}
}

//	faster
inline void HIntArray::reverse(HIntArray &newlist) {
	unsigned len = this->length();
	newlist.clear();
	newlist.classList.resize(len);
	for (unsigned i = 0; i < len; i++)
		newlist.classList[i] = this->classList[len-i-1];
	newlist.imap = imap;
}

inline void HIntArray::reverse() {
	HIntArray temp(*this);
	unsigned len = this->length();
	this->clear();
	this->classList.resize(len);
	for (unsigned i = 0; i < len; i++)
		this->classList[i] = temp.classList[len-i-1];
	imap = temp.imap;
}

inline void HIntArray::reorder(HIntArray &newarray, const HIntArray &order) {
	newarray.clear();
	for (unsigned i = 0; i < (*this).length(); i++)
		newarray.append((*this)[order[i]]);
}

inline int HIntArray::maxValue() {
	if (this->classList.size() != 0) {
		IntVectorIt location;
		location = std::max_element(classList.begin(),classList.end());
		return *location;
	}
	else 
		return 0;
}

inline int HIntArray::operator[](const unsigned index) const {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HIntarray]");
	}
	return classList[index];
}

inline int &HIntArray::operator[](const unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HIntarray]");
	}
	return classList[index];
}

inline HIntArray &HIntArray::operator = (const HIntArray &other) {
	classList = other.classList;
	imap = other.imap;
	return *this;
}

// THIS COULD USE SOME RETHINKING, LATER.
inline bool HIntArray::operator == (HIntArray &other)  const {
//	IntVectorIt location;
	bool exists;
	unsigned idx;

	if (this->length() != other.length())
		return false;

	for (unsigned i = 0; i < this->length(); i++) {
		exists = other.valueExists((*this)[i],idx);
		if (!exists)
			return false;
//		location = (int *)std::find(classList.begin(),classList.end(),(*this)[i]);
//		if (location == classList.end())
//			return false;
	}
	return true;
}

inline bool HIntArray::operator != (HIntArray &other) const {
	return !(*this == other);
}

/////////////////////////////////////////////////////////////////////////////////////////
//	HIntIterator
/////////////////////////////////////////////////////////////////////////////////////////

inline HIntIterator::HIntIterator() {}

inline HIntIterator::HIntIterator(HIntArray &other) {
	pointTo = &other;
	cl_iterator = pointTo->classList.begin();
}

inline HIntIterator::~HIntIterator() {}

inline bool HIntIterator::isDone() {
	return (cl_iterator == pointTo->classList.end());
}

inline void HIntIterator::reset(HIntArray &other) {
	pointTo = &other;
	cl_iterator = pointTo->classList.begin();
}

inline void HIntIterator::reset() {
	cl_iterator = pointTo->classList.begin();
}

inline void HIntIterator::insert(const int value){
	pointTo->classList.insert(cl_iterator,value);
}

inline unsigned HIntIterator::index() const {
	return (cl_iterator-pointTo->classList.begin());
}

inline HIntIterator HIntIterator::operator ++(int) {
	cl_iterator++;
	return *this;
}

inline HIntIterator HIntIterator::operator --(int) {
	cl_iterator--;
	return *this;
}

inline int *HIntIterator::operator * (){
	return &(*cl_iterator);
}

inline std::ostream &operator << (std::ostream &os,const HIntArray &other){
	os << "[ ";
	for (unsigned i = 0; i < other.length();i++)
		os << other[i] << " ";
	os << "]";
	return os;
}

/////////////////////////////////////////////////////////////////////////////////////////
// HIntArrayList
/////////////////////////////////////////////////////////////////////////////////////////

inline HIntArrayList::HIntArrayList() {}

inline HIntArrayList::~HIntArrayList() {}

inline void HIntArrayList::resize(const unsigned size) {
	classList.resize(size);
}

inline void HIntArrayList::clear() {
	classList.clear();
}

inline unsigned HIntArrayList::length() const {
	return classList.size();
}

inline void HIntArrayList::append(HIntArray &other) {
	classList.push_back(other);
}

inline bool HIntArrayList::valueExists(int value) {
	IntVectorArrayIt iter;
	bool exists = false;
	for (iter = classList.begin(); iter != classList.end(); iter++) {
		unsigned index;
		if (iter->valueExists(value,index))
			return true;
	}
	return false;
}

inline HIntArray *HIntArrayList::get(unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HIntArrayList]");
	}
	return &classList[index];
}

inline HIntArray HIntArrayList::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HIntArrayList]");
	}
	return classList[index];
}

inline HIntArray &HIntArrayList::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HIntArrayList]");
	}
	return classList[index];
}

inline HIntArrayList &HIntArrayList::operator = ( const HIntArrayList &other ){
	classList = other.classList;
	return *this;
}

inline std::ostream &operator << (std::ostream &os,HIntArrayList &other){
	for (unsigned i = 0; i < other.length(); i++) 
		os << i << ") " << other[i] << std::endl;
	return os;
}