#include "stdafx.h"

HDoubleArray::HDoubleArray() {
	this->clear();
}

HDoubleArray::HDoubleArray(unsigned s){
	classList.reserve(s);
}

HDoubleArray::~HDoubleArray() {
	this->clear();
}

void HDoubleArray::clear() {
	classList.clear();
}

void HDoubleArray::resize(const unsigned s) {
	classList.resize(s);
}

void HDoubleArray::Sort() {
	std::sort(classList.begin(),classList.end());
}

unsigned HDoubleArray::length() const {
	return classList.size();
}

double HDoubleArray::begin() {
	return classList.front();
}

double HDoubleArray::end() {
	return classList.back();
}

unsigned HDoubleArray::beginIndex() {
	return 0;
}

unsigned HDoubleArray::endIndex() {
	return classList.size()-1;
}

void HDoubleArray::append(double value) {
	classList.push_back(value);
}

void HDoubleArray::append_NR(double value) {
	unsigned index;
	DoubleVectorIt location;
	location = std::find(classList.begin(),classList.end(),value);
	if (location != classList.end()) {
		index = location - classList.begin();
		return;
	}
	classList.push_back(value);
}

bool HDoubleArray::value_exists(double value,unsigned &index) {
	DoubleVectorIt location;
	location = std::find(classList.begin(),classList.end(),value);
	if (location != classList.end()) {
		index = location - classList.begin();
		return true;
	}
	return false;
}

double HDoubleArray::max_value() {
	if (this->classList.size() != 0) {
		DoubleVectorIt location;
		location = std::max_element(classList.begin(),classList.end());
		return *location;
	}
	else 
		return 0;
}

const double HDoubleArray::get(unsigned index) {
//	if (index > classList.size()) {
//		THROW_FATAL_ERROR("Index out of range.[HDoublearray]");
//	}
	return classList[index];
}

void HDoubleArray::set(unsigned index,double value) {
//	if (index > classList.size()) {
//		THROW_FATAL_ERROR("Index out of range.[HDoublearray]");
//	}
	this->classList[index] = value;
}

double HDoubleArray::operator[](unsigned index) const{
//	if (index > classList.size()) {
//		THROW_FATAL_ERROR("Index out of range.[HDoublearray]");
//	}
	return classList[index];
}

double &HDoubleArray::operator[](unsigned index){
//	if (index > classList.size()) {
//		THROW_FATAL_ERROR("Index out of range.[HDoublearray]");
//	}
	
	return classList[index];
}

/* this could be cool finish later
HDoubleArray &HDoubleArray::operator = ( const HDoubleArrayList other ){
	IntVectorIt iter;
	for (iter = other.classList.begin(); iter != other.classList.end(); iter++) {
		for (iter2 = *iter
		classList.push_back(value);
	return *this;
}
*/

HDoubleArray &HDoubleArray::operator = ( const HDoubleArray other ){
	classList = other.classList;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
// HDoubleIterator
/////////////////////////////////////////////////////////////////////////////////////////

HDoubleIterator::HDoubleIterator() {}

HDoubleIterator::HDoubleIterator(HDoubleArray &iarray) {
	pointTo = &iarray;
	cl_iterator = pointTo->classList.begin();
}

HDoubleIterator::~HDoubleIterator() {}

bool HDoubleIterator::isDone() {
	return (cl_iterator == pointTo->classList.end());
}

void HDoubleIterator::reset(HDoubleArray &iarray) {
	pointTo = &iarray;
	cl_iterator = pointTo->classList.begin();
}

void HDoubleIterator::reset() {
	cl_iterator = pointTo->classList.begin();
}

void HDoubleIterator::insert(const double value){
	pointTo->classList.insert(cl_iterator,value);
}

unsigned HDoubleIterator::index() {
	return (cl_iterator-pointTo->classList.begin());
}

HDoubleIterator HDoubleIterator::operator ++(int) {
	cl_iterator++;
	return *this;
}

HDoubleIterator HDoubleIterator::operator --(int) {
	cl_iterator--;
	return *this;
}

double *HDoubleIterator::operator * (){
	return &(*cl_iterator);
}

/////////////////////////////////////////////////////////////////////////////////////////
// HDoubleArrayList - array within and array
/////////////////////////////////////////////////////////////////////////////////////////

HDoubleArrayList::HDoubleArrayList() {
	this->clear();
}

HDoubleArrayList::~HDoubleArrayList() {
	this->clear();
}

void HDoubleArrayList::resize(const unsigned s) {
	classList.resize(s);
}

void HDoubleArrayList::clear() {

	classList.clear();
}

unsigned HDoubleArrayList::length() const {
	return classList.size();
}

void HDoubleArrayList::append(HDoubleArray value) {
	classList.push_back(value);
}

bool HDoubleArrayList::value_exists(double value) {
	DoubleVectorArrayIt iter;
	bool exists = false;
	for (iter = classList.begin(); iter != classList.end(); iter++) {
		unsigned index;
		if (iter->value_exists(value,index))
			return true;
	}
	return false;
}

HDoubleArray *HDoubleArrayList::get(unsigned index) {
//	if (index > classList.size()) {
//		THROW_FATAL_ERROR("Index out of range.[HDoublearraylist]");
//	}
	return &classList[index];
}

HDoubleArray HDoubleArrayList::operator[](unsigned index) const{
//	if (index > classList.size()) {
//		THROW_FATAL_ERROR("Index out of range.[HDoublearray]");
//	}
	return classList[index];
}

HDoubleArray &HDoubleArrayList::operator[](unsigned index){
//	if (index > classList.size()) {
//		THROW_FATAL_ERROR("Index out of range.[HDoublearray]");
//	}
	
	return classList[index];
}

HDoubleArrayList &HDoubleArrayList::operator = ( const HDoubleArrayList other ){
	classList = other.classList;
	return *this;
}

std::ostream &operator << (std::ostream &os,HDoubleArray &iarray){
	HDoubleIterator iter(iarray);

	for (;!iter.isDone(); iter++)
		os << **iter << " ";

//	os << endl;

	return os;
}

std::ostream &operator << (std::ostream &os,HDoubleArrayList &ilist){

	for (unsigned i = 0; i < ilist.length(); i++) 
		os << i << ") " << ilist[i] << std::endl;

	return os;
}
