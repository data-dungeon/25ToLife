//////////////////////////////////////////////////////////////////////////////////////////
//
// HBONE ARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hbonearray.h"
#include "hmacros.h"
#include "hstatus.h"

HBoneArray::HBoneArray() {}

HBoneArray::~HBoneArray() {}

void HBoneArray::clear() {
	classList.clear();
}

unsigned HBoneArray::length() const {
	return classList.size();
}

void HBoneArray::resize(const unsigned s) {
	classList.resize(s);
}

unsigned HBoneArray::append(HBone &other) {
	other.boneArray = this;
	classList.push_back(other);
	return classList.size()-1;
}

HBone *HBoneArray::get(unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range. [HBone]");
	}
	return &classList[index];
}

HBone *HBoneArray::begin() {
	return &classList.front();
}

HBone *HBoneArray::end() {
	return &classList.back();
}

HBone HBoneArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HBoneArray]");
	}
	return classList[index];
}

HBone &HBoneArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HBoneArray]");
	}
	return classList[index];
}

HBoneArray &HBoneArray::operator = ( const HBoneArray &other ){
	classList = other.classList;
	return *this;
}

std::ostream &operator << (std::ostream &os,HBoneArray &other){
	for (unsigned i = 0; i < other.length(); i++)
		os << other[i] << std::endl;
	return os;
}