//////////////////////////////////////////////////////////////////////////////////////////
//
// HTJUNCTION CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "htjunction.h"
#include "hstatus.h"
#include "hmacros.h"

HTJunction::HTJunction() {}

HTJunction::HTJunction(const HTJunction &other){
	long_edge = other.long_edge;
	short_edges = other.short_edges;
}

HTJunction::~HTJunction(){}

void HTJunction::clear() {
	long_edge.clear();
	short_edges.clear();
}

HTJunction &HTJunction::operator = ( const HTJunction &other ){
	long_edge = other.long_edge;
	short_edges = other.short_edges;
	return *this;
}

std::ostream &operator << (std::ostream &os, HTJunction &other){
	os << "Long edge: " << other.long_edge << std::endl;
	os << "Short edges: " << other.short_edges << std::endl;
	return os;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// HTJUNCTIONARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

HTJunctionArray::HTJunctionArray() {}

HTJunctionArray::~HTJunctionArray() {}

void HTJunctionArray::clear() {
	classList.clear();
}

unsigned HTJunctionArray::length() const {
	return classList.size();
}

void HTJunctionArray::resize(const unsigned size) {
	classList.resize(size);
}

unsigned HTJunctionArray::append(HTJunction &other) {
	classList.push_back(other);
	return classList.size()-1;
}

HTJunction *HTJunctionArray::get(unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range. [HTJunction]");
	}
	return &classList[index];
}

HTJunction *HTJunctionArray::begin() {
	return &classList.front();
}

HTJunction *HTJunctionArray::end() {
	return &classList.back();
}

HTJunction HTJunctionArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HTJunctionArray]");
	}
	return classList[index];
}

HTJunction &HTJunctionArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HTJunctionArray]");
	}
	
	return classList[index];
}

HTJunctionArray &HTJunctionArray::operator = ( const HTJunctionArray &other ){
	classList = other.classList;
	return *this;
}

std::ostream &operator << (std::ostream &os,HTJunctionArray &other){
	for (unsigned i = 0; i < other.length(); i++)
		os << other[i] << std::endl;
	return os;
}