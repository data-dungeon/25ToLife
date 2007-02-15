#include "stdafx.h"
#include "hgroup.h"
#include "hmacros.h"
#include "hpatch.h"
#include "hpatcharray.h"

HGroup::HGroup(){
	memset(&flags,0,sizeof(unsigned long));
	name = "";
}

HGroup::HGroup(const HGroup &other) {
	memcpy(&this->flags,&other.flags,sizeof(unsigned long));
	name = other.name;
	patchIndicies = other.patchIndicies;
	decalIndicies = other.decalIndicies;
	attribute = other.attribute;
}

HGroup::~HGroup(){}

void HGroup::clear() {
	memset(&flags,0,sizeof(unsigned long));
	name = "";
	patchIndicies.clear();
	decalIndicies.clear();
	attribute.clear();
}

bool HGroup::hasDecals() {
	if (this->decalIndicies.size() > 0)
		return true;
	return false;
}

HGroup &HGroup::operator = ( const HGroup &other ){
	memcpy(&this->flags,&other.flags,sizeof(HGroup::gflags));
	name = other.name;
	patchIndicies = other.patchIndicies;
	decalIndicies = other.decalIndicies;
	attribute = other.attribute;
	return *this;
}

std::ostream &operator << (std::ostream &os,HGroup &other){
	os << "Group [" << other.name.asChar() << "] ";
	os << std::endl;
	return os;
}

HGroupArray::HGroupArray() {}

HGroupArray::HGroupArray(const HGroupArray &other) {
	classList = other.classList;
}

HGroupArray::~HGroupArray() {}

void HGroupArray::clear() {
	classList.clear();
}

unsigned HGroupArray::length() const {
	return classList.size();
}

unsigned HGroupArray::append(const HGroup &other) {
	classList.push_back(other);
	return classList.size()-1;
}

HGroup *HGroupArray::get(unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.");
	}
	return &classList[index];
}

HGroup *HGroupArray::begin() {
	return &classList.front();
}

HGroup *HGroupArray::end() {
	return &classList.back();
}

HGroup HGroupArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HGroupArray]");
	}
	return classList[index];
}

HGroup &HGroupArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HGroupArray]");
	}
	
	return classList[index];
}

HGroupArray &HGroupArray::operator = ( const HGroupArray &other ){
	classList = other.classList;
	return *this;
}
