#include "stdafx.h"
#include "hintarray.h"

#include <maya/MGlobal.h>
#include "macros.h"

#include <algorithm>

HIntArray::HIntArray() {
	this->clear();
}

HIntArray::HIntArray(unsigned s){
	classList.reserve(s);
}

HIntArray::~HIntArray() {
	this->clear();
}

void HIntArray::zero() {
	for (unsigned i = 0; i < classList.size(); i++)
		classList[i] = 0;
}

void HIntArray::clear() {
	classList.clear();
}

void HIntArray::resize(const unsigned s) {
	classList.resize(s);
}

void HIntArray::Sort() {
	std::sort(classList.begin(),classList.end());
}

unsigned HIntArray::length() const {
	return classList.size();
}

int HIntArray::begin() {
	return classList.front();
}

int HIntArray::end() {
	return classList.back();
}

unsigned HIntArray::beginIndex() {
	return 0;
}

unsigned HIntArray::endIndex() {
	return classList.size()-1;
}

void HIntArray::append(int value) {
	classList.push_back(value);
}

void HIntArray::append_NR(int value) {
	unsigned index;
	IntVectorIt location;
	location = std::find(classList.begin(),classList.end(),value);
	if (location != classList.end()) {
		index = location - classList.begin();
		return;
	}
	classList.push_back(value);
}

bool HIntArray::value_exists(int value,unsigned &index) {
	IntVectorIt location;
	location = std::find(classList.begin(),classList.end(),value);
	if (location != classList.end()) {
		index = location - classList.begin();
		return true;
	}
	return false;
}

int HIntArray::max_value() {
	if (this->classList.size() != 0) {
		IntVectorIt location;
		location = std::max_element(classList.begin(),classList.end());
		return *location;
	}
	else 
		return 0;
}

const int HIntArray::get(unsigned index) {
	if (index >= classList.size()) {
//		THROW_FATAL_ERROR("Index out of range.[HIntarray::get]");
		DISPLAY_ERROR("Index out of range. [HIntArray::get]");
	}
	return classList[index];
}

void HIntArray::set(unsigned index,int value) {
//	if (index > classList.size()) {
//		THROW_FATAL_ERROR("Index out of range.[HIntarray]");
//	}
	this->classList[index] = value;
}

int HIntArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
//		THROW_FATAL_ERROR("Index out of range.[HIntarray::[]]");
		DISPLAY_ERROR("Index out of range. [HIntArray::[]]");
	}
	return classList[index];
}

int &HIntArray::operator[](unsigned index){
	if (index >= classList.size()) {
//		THROW_FATAL_ERROR("Index out of range.[HIntarray::[]]");
		DISPLAY_ERROR("Index out of range. [HIntArray::[]]");
	}
	return classList[index];
}

/* this could be cool finish later
HIntArray &HIntArray::operator = ( const HIntArrayList other ){
	IntVectorIt iter;
	for (iter = other.classList.begin(); iter != other.classList.end(); iter++) {
		for (iter2 = *iter
		classList.push_back(value);
	return *this;
}
*/

HIntArray &HIntArray::operator = ( const HIntArray other ){
	classList = other.classList;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
// HIntIterator
/////////////////////////////////////////////////////////////////////////////////////////

HIntIterator::HIntIterator() {}

HIntIterator::HIntIterator(HIntArray &iarray) {
	pointTo = &iarray;
	cl_iterator = pointTo->classList.begin();
}

HIntIterator::~HIntIterator() {}

bool HIntIterator::isDone() {
	return (cl_iterator == pointTo->classList.end());
}

void HIntIterator::reset(HIntArray &iarray) {
	pointTo = &iarray;
	cl_iterator = pointTo->classList.begin();
}

void HIntIterator::reset() {
	cl_iterator = pointTo->classList.begin();
}

void HIntIterator::insert(const int value){
	pointTo->classList.insert(cl_iterator,value);
}

unsigned HIntIterator::index() {
	return (cl_iterator-pointTo->classList.begin());
}

HIntIterator HIntIterator::operator ++(int) {
	cl_iterator++;
	return *this;
}

HIntIterator HIntIterator::operator --(int) {
	cl_iterator--;
	return *this;
}

int *HIntIterator::operator * (){
	return &(*cl_iterator);
}

/////////////////////////////////////////////////////////////////////////////////////////
// HIntArrayList - array within and array
/////////////////////////////////////////////////////////////////////////////////////////

HIntArrayList::HIntArrayList() {
	this->clear();
}

HIntArrayList::~HIntArrayList() {
	this->clear();
}

void HIntArrayList::resize(const unsigned s) {
	classList.resize(s);
}

void HIntArrayList::clear() {

	classList.clear();
}

unsigned HIntArrayList::length() const {
	return classList.size();
}

void HIntArrayList::append(HIntArray value) {
	classList.push_back(value);
}

bool HIntArrayList::value_exists(int value) {
	IntVectorArrayIt iter;
	bool exists = false;
	for (iter = classList.begin(); iter != classList.end(); iter++) {
		unsigned index;
		if (iter->value_exists(value,index))
			return true;
	}
	return false;
}

HIntArray *HIntArrayList::get(unsigned index) {
	if (index >= classList.size()) {
//		THROW_FATAL_ERROR("Index out of range.[HIntarray::get]");
		DISPLAY_ERROR("Index out of range. [HIntArrayList::get]");
	}
	return &classList[index];
}

HIntArray HIntArrayList::operator[](unsigned index) const{
	if (index >= classList.size()) {
//		THROW_FATAL_ERROR("Index out of range.[HIntarray]");
		DISPLAY_ERROR("Index out of range.[HIntarrayList::[]]");
	}
	return classList[index];
}

HIntArray &HIntArrayList::operator[](unsigned index){
	if (index >= classList.size()) {
//		THROW_FATAL_ERROR("Index out of range.[HIntarray]");
		DISPLAY_ERROR("Index out of range.[HIntarrayList::[]]");
	}
	
	return classList[index];
}

HIntArrayList &HIntArrayList::operator = ( const HIntArrayList other ){
	classList = other.classList;
	return *this;
}

std::ostream &operator << (std::ostream &os,HIntArray &iarray){
	HIntIterator iter(iarray);

	for (;!iter.isDone(); iter++)
		os << **iter << " ";

//	os << endl;

	return os;
}

std::ostream &operator << (std::ostream &os,HIntArrayList &ilist){

	for (unsigned i = 0; i < ilist.length(); i++) 
		os << i << ") " << ilist[i] << std::endl;

	return os;
}
