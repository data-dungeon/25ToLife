//////////////////////////////////////////////////////////////////////////////////////////
//
// HMAP CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hmap.h"
#include "hmacros.h"
#include "hstatus.h"

HMap::HMap() {
	type = HMap::NONE;
	mapArray = NULL;
}

HMap::HMap(const HMap &other) {
	name = other.name;
	path = other.path;
	type = other.type;
	repeat = other.repeat;
	mapArray = other.mapArray;
}

HMap::~HMap() {}


void HMap::clear() {
	name = "";
	path = "";
	type = HMap::NONE;
	repeat.x = 0;
	repeat.y = 0;
	mapArray = NULL;
}

HMap *HMap::next() {
	if (this == mapArray->end())
		return NULL;
	return this+1;
}

HMap *HMap::previous() {
	if (this == mapArray->begin())
		return NULL;
	return this-1;
}

HMap *HMap::begin() {
	return mapArray->begin();
}

HMap *HMap::end() {
	return mapArray->end();
}

HMap &HMap::operator = ( const HMap &other ){
	name = other.name;
	path = other.path;
	type = other.type;
	repeat = other.repeat;
	mapArray = other.mapArray;
	return *this;
}

bool HMap::operator == ( const HMap &other ) const {
	if (name != other.name)
		return false;
//	if (path != other.path)
//		return false;
	if (type != other.type)
		return false;
	if (repeat != other.repeat)
		return false;
	return true;
}

bool HMap::operator != ( const HMap &other ) const {
	return !(*this == other);
}

std::ostream &operator << (std::ostream &os, HMap &other){
	os << "MAP [ " << other.path.asChar() << " ] ";

	if (other.type == HMap::COLORMAP)
		os << "[ COLORMAP ] ";
	if (other.type == HMap::COOKIECUTMAP)
		os << "[ COOKIECUTMAP ] ";
	if (other.type == HMap::DIFFUSEMAP)
		os << "[ DIFFUSEMAP ] ";

	os << "repeat: " << other.repeat;
	os << std::endl;
	
	return os;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// HMAP ARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

HMapArray::HMapArray() {}

HMapArray::~HMapArray() {}

void HMapArray::clear() {
	classList.clear();
}

unsigned HMapArray::length() const {
	return classList.size();
}

// SLOW!
unsigned HMapArray::add(HMap &map) {
	int i = 0;
	for (HMap *iter = this->begin(); iter != NULL; iter=iter->next()) {
		HMap *tempmap = iter;
		if (*iter == map)
			return i;
		i++;
	}
	map.mapArray = this;
	classList.push_back(map);
	return classList.size()-1;
}

unsigned HMapArray::append(HMap map) {
	map.mapArray = this;
	classList.push_back(map);
	return classList.size()-1;
}

HMap *HMapArray::get(unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.");
	}
	return &classList[index];
}

HMap *HMapArray::begin() {
	return &classList.front();
}

HMap *HMapArray::end() {
	return &classList.back();
}

HMapArray &HMapArray::operator = ( const HMapArray &other ){
	classList = other.classList;
	return *this;
}

bool HMapArray::operator == ( const HMapArray &other ) const {
	for (unsigned i = 0; i < other.length(); i++)
		if ((*this)[i] != other[i])
			return false;
	return true;
}

bool HMapArray::operator != ( const HMapArray &other ) const {
	return !(*this == other);
}

HMap HMapArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HMapArray]");
	}
	return classList[index];
}

HMap &HMapArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HMapArray]");
	}
	
	return classList[index];
}

std::ostream &operator << (std::ostream &os, HMapArray &other){
	for (unsigned i = 0; i < other.length(); i++) {
		os << "Index [ " << i << " ]" << std::endl;
		os << other[i] << std::endl;
	}
	return os;
}

