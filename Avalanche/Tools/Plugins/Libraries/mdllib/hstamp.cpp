#include "Stdafx.h"
#include "HStamp.h"
#include "hmacros.h"

HStamp::HStamp(){}

HStamp::~HStamp(){}

void HStamp::clear() {
	name = "";
	uvs.clear();
	patchIndicies.clear();
}

HStamp &HStamp::operator = ( const HStamp &other ){
	name = other.name;
	patchIndicies = other.patchIndicies;
	uvs = other.uvs;
	return *this;
}

std::ostream &operator << (std::ostream &os, HStamp &other){
	os << "Stamp [ " << other.name.asChar() << " ] " << std::endl;
//	if (other.patchIndicies.length() > 0)
//		os << other.patchIndicies;
	os << std::endl;
	return os;
}

HStampArray::HStampArray() {}

HStampArray::~HStampArray() {}

void HStampArray::clear() {
	classList.clear();
}

unsigned HStampArray::length() const {
	return classList.size();
}

unsigned HStampArray::append(HStamp stamp) {
	classList.push_back(stamp);
	return classList.size()-1;
}

HStamp *HStampArray::get(unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range. [hstamparray]");
	}
	return &classList[index];
}

HStamp *HStampArray::begin() {
	return &classList.front();
}

HStamp *HStampArray::end() {
	return &classList.back();
}

HStampArray &HStampArray::operator = ( const HStampArray &other ){
	classList = other.classList;
	return *this;
}

HStamp HStampArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HStampArray]");
	}
	return classList[index];
}

HStamp &HStampArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HStampArray]");
	}
	
	return classList[index];
}

std::ostream &operator << (std::ostream &os, HStampArray &other){
	for (unsigned i = 0; i < other.length(); i++) {
		os << "Index (" << i << ")" << std::endl;
		os << other[i] << std::endl;
	}
	return os;
}
