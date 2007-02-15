//////////////////////////////////////////////////////////////////////////////////////////
//
// MDLOBJECT ARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mdlobjectarray.h"
#include "hstatus.h"
#include "hmacros.h"

mdlObjectArray::mdlObjectArray() {}

mdlObjectArray::mdlObjectArray(const mdlObjectArray &other) {
	classList = other.classList;
}

mdlObjectArray::~mdlObjectArray() {}

void mdlObjectArray::clear() {
	classList.clear();
}

unsigned mdlObjectArray::length() const {
	return classList.size();
}

void mdlObjectArray::resize(const unsigned s) {
	classList.resize(s);
}

unsigned mdlObjectArray::append(mdlObject &other) {
	classList.push_back(other);
	return classList.size()-1;
}

mdlObject *mdlObjectArray::get(unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range. [HBone]");
	}
	return &classList[index];
}

mdlObject *mdlObjectArray::begin() {
	return &classList.front();
}

mdlObject *mdlObjectArray::end() {
	return &classList.back();
}

mdlObject mdlObjectArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[mdlObjectArray]");
	}
	return classList[index];
}

mdlObject &mdlObjectArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[mdlObjectArray]");
	}
	return classList[index];
}

mdlObjectArray &mdlObjectArray::operator = ( const mdlObjectArray &other ){
	classList = other.classList;
	return *this;
}

std::ostream &operator << (std::ostream &os,mdlObjectArray &other){
	for (unsigned i = 0; i < other.length(); i++)
		os << "mdlObject ( " << i << " ) " << other[i] << std::endl;
	return os;
}