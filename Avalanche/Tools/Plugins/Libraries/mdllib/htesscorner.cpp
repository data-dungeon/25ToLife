//////////////////////////////////////////////////////////////////////////////////////////
//
// HTESSCORNER CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "htesscorner.h"
#include "hmacros.h"
#include "hstatus.h"

HTessCorner::HTessCorner() {
	memset(&flags,0,sizeof(unsigned long));
	vertexIndex = 0;
	normalIndex = 9999;
}

HTessCorner::HTessCorner(const HTessCorner &other){
	memcpy(&this->flags,&other.flags,sizeof(unsigned long));
	vertexIndex = other.vertexIndex;
	normalIndex = other.normalIndex;
}

HTessCorner::~HTessCorner(){}

void HTessCorner::clear() {
	memset(&flags,0,sizeof(unsigned long));
	vertexIndex = 0;
	normalIndex = 9999;
}

void HTessCorner::setVertexIndex(unsigned vi) {
	this->vertexIndex = vi;
	this->flags.kHasVertexIndex = 1;
}	

void HTessCorner::setNormalIndex(unsigned ni) {
	this->normalIndex = ni;
}

HTessCorner &HTessCorner::operator = ( const HTessCorner &other ){
	memcpy(&this->flags,&other.flags,sizeof(unsigned long));
	vertexIndex = other.vertexIndex;
	normalIndex = other.normalIndex;
	return *this;
}

std::ostream &operator << (std::ostream &os, HTessCorner &other){
	return os;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// HTESSCORNERARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

HTessCornerArray::HTessCornerArray() {}

HTessCornerArray::HTessCornerArray(const unsigned size) {
	classList.resize(size);
}

HTessCornerArray::HTessCornerArray(const HTessCornerArray &other) {
	classList = other.classList;
}

HTessCornerArray::~HTessCornerArray() {}

void HTessCornerArray::clear() {
	classList.clear();
}

unsigned HTessCornerArray::length() const {
	return classList.size();
}

void HTessCornerArray::resize(const unsigned s) {
	classList.resize(s);
}

unsigned HTessCornerArray::append(const HTessCorner &other) {
	classList.push_back(other);
	return classList.size()-1;
}

HTessCorner *HTessCornerArray::get(unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range. [HTessCorner]");
	}
	return &classList[index];
}
HTessCorner *HTessCornerArray::begin() {
	return &classList.front();
}

HTessCorner *HTessCornerArray::end() {
	return &classList.back();
}

HTessCorner HTessCornerArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HTessCornerArray]");
	}
	return classList[index];
}

HTessCorner &HTessCornerArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HTessCornerArray]");
	}
	
	return classList[index];
}

HTessCornerArray &HTessCornerArray::operator = ( const HTessCornerArray &other ){
	classList = other.classList;
	return *this;
}

std::ostream &operator << (std::ostream &os,const HTessCornerArray &other){
	for (unsigned i = 0; i < other.length(); i++)
		os << other[i] << std::endl;
	return os;
}