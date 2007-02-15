#include "stdafx.h"
#include "hdecal.h"
#include "hmacros.h"
#include "hstatus.h"

HDecal::HDecal() {
//	decal_name = "";
	name = "";
	imageIndex = 0;
	type = HDecal::COLORMAP;
	value = 1;
	repeat.x = 1; repeat.y = 1;
	seamless = false;
}

HDecal::~HDecal() {}

void HDecal::clear() {
	decal_name = "";
	name = "";
	imageIndex = 0;
	type = HDecal::COLORMAP;
	value = 1;
	repeat.x = 1;
	repeat.y = 1;
	seamless = false;
	stampIndicies.clear();
}

bool HDecal::hasStamps() {
	if (this->stampIndicies.size() > 0)
		return true;
	return false;
}

HDecal &HDecal::operator = ( const HDecal &other ){
	decal_name = other.decal_name;
	name = other.name;
	imageIndex = other.imageIndex;
	type = other.type;
	value = other.value;
	repeat = other.repeat;
	seamless = other.seamless;
	stampIndicies = other.stampIndicies;
	return *this;
}

std::ostream &operator << (std::ostream &os, HDecal &other){
//	os << "DECAL [ " << other.decal_name.asChar() << " ] ";

//	if (other.type == HDecal::COLORMAP)
//		os << "[ COLORMAP ] ";
//	if (other.type == HDecal::COOKIECUTMAP)
//		os << "[ COOKIECUTMAP ] ";
//	if (other.type == HDecal::DIFFUSEMAP)
//		os << "[ DIFFUSEMAP ] ";
//	os << "repeat: " << other.repeat;

//	os << std::endl;

//	os << "Stamp Indicies:" << other.stampIndicies << std::endl;
	
	return os;
}

HDecalArray::HDecalArray() {}

HDecalArray::~HDecalArray() {}

void HDecalArray::clear() {
	classList.clear();
}

unsigned HDecalArray::length() const {
	return classList.size();
}

unsigned HDecalArray::append(HDecal decal) {
	classList.push_back(decal);
	return classList.size()-1;
}

HDecal *HDecalArray::get(unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.");
	}
	return &classList[index];
}

HDecal *HDecalArray::begin() {
	return &classList.front();
}

HDecal *HDecalArray::end() {
	return &classList.back();
}

HDecalArray &HDecalArray::operator = ( const HDecalArray &other ){
	classList = other.classList;
	return *this;
}

HDecal HDecalArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HDecalArray]");
	}
	return classList[index];
}


HDecal &HDecalArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HDecalArray]");
	}
	
	return classList[index];
}

std::ostream &operator << (std::ostream &os, HDecalArray &other){
	for (unsigned i = 0; i < other.length(); i++){
		os << "Index (" << i << ")" << std::endl;
		os << other[i] << std::endl;
	}
	return os;
}


