//////////////////////////////////////////////////////////////////////////////////////////
//
// HEDGE CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hedge.h"
#include "mdlobject.h"

HEdge::HEdge() {
	memset(&flags,0,sizeof(HEdge::eflags));
	vertexIndex0 = 0;
	vertexIndex1 = 0;
	normalIndex0 = 0;
	normalIndex1 = 0;
//	cpvIndex0 = 0;
//	cpvIndex1 = 0;
	adjpatchidx = 0;
	patchidx = 0;
	model = NULL;
}

HEdge::HEdge( const HEdge &other ){
	memcpy(&this->flags,&other.flags,sizeof(unsigned long));
	vertexIndex0 = other.vertexIndex0;
	vertexIndex1 = other.vertexIndex1;
	normalIndex0 = other.normalIndex0;
	normalIndex1 = other.normalIndex1;
//	cpvIndex0 = other.cpvIndex0;
//	cpvIndex1 = other.cpvIndex1;
	patchidx = other.patchidx;
	adjpatchidx = other.adjpatchidx;
	model = other.model;
}

HEdge::HEdge(const unsigned vidx0,const unsigned vidx1,
			 const unsigned nidx0,const unsigned nidx1) {
	memset(&this->flags,0,sizeof(HEdge::eflags));
	vertexIndex0 = vidx0;
	vertexIndex1 = vidx1;
	normalIndex0 = nidx0;
	normalIndex1 = nidx1;
//	cpvIndex0 = 0;
//	cpvIndex1 = 0;
	adjpatchidx = 0;
	patchidx = 0;
	model = NULL;
}

HEdge::HEdge(unsigned vidx0, unsigned vidx1) {
	memset(&this->flags,0,sizeof(HEdge::eflags));
	vertexIndex0 = vidx0;
	vertexIndex1 = vidx1;
	normalIndex0 = 0;
	normalIndex1 = 0;
//	cpvIndex0 = 0;
//	cpvIndex1 = 0;
	adjpatchidx = 0;
	patchidx = 0;
	model = NULL;
}

HEdge::~HEdge(){}

void HEdge::clear() {
	memset(&flags,0,sizeof(HEdge::eflags));
	vertexIndex0 = 0;
	vertexIndex1 = 0;
	normalIndex0 = 0;
	normalIndex1 = 0;
//	cpvIndex0 = 0;
//	cpvIndex1 = 0;
	adjpatchidx = 0;
	patchidx = 0;
	model = NULL;
}

void HEdge::invert() {
	unsigned temp = vertexIndex0;
	vertexIndex0 = vertexIndex1;
	vertexIndex1 = temp;
	temp = normalIndex0;
	normalIndex0 = normalIndex1;
	normalIndex1 = temp;
//	temp = cpvIndex0;
//	cpvIndex0 = cpvIndex0;
//	cpvIndex1 = temp;
}

void HEdge::addVertexIndicies(unsigned vi0,unsigned vi1){
	vertexIndex0 = vi0;
	vertexIndex1 = vi1;
}

//void HEdge::addNormalIndicies(unsigned ni0,unsigned ni1){
//	normalIndex0 = ni0;
//	normalIndex1 = ni1;
//}

//void HEdge::addCPVertexIndicies(unsigned cpi0,unsigned cpi1) {
//	cpvIndex0 = cpi0;
//	cpvIndex1 = cpi1;
//}

//bool HEdge::isCPVIndiciesEqual(HEdge &other) {
//	if (((this->cpvIndex0 == other.cpvIndex0) && (this->cpvIndex1 == other.cpvIndex1))||
//		((this->cpvIndex0 == other.cpvIndex1) && (this->cpvIndex1 == other.cpvIndex0)))
//		return true;
//	return false;
//}

HEdge &HEdge::operator = ( const HEdge &other ){
	memcpy(&this->flags,&other.flags,sizeof(HEdge::eflags));
	vertexIndex0 = other.vertexIndex0;
	vertexIndex1 = other.vertexIndex1;
	normalIndex0 = other.normalIndex0;
	normalIndex1 = other.normalIndex1;
//	cpvIndex0 = other.cpvIndex0;
//	cpvIndex1 = other.cpvIndex1;
	patchidx = other.patchidx;
	adjpatchidx = other.adjpatchidx;
	model = other.model;
	return *this;
}

bool HEdge::operator == ( const HEdge &other ) const {
	if (((vertexIndex0 == other.vertexIndex0) && (vertexIndex1 == other.vertexIndex1))||
		((vertexIndex1 == other.vertexIndex0) && (vertexIndex0 == other.vertexIndex1)))
		return true;
	return false;
}

bool HEdge::operator != ( const HEdge &other ) const {
	return !(*this == other);
}

std::ostream &operator << (std::ostream &os, HEdge &e){
//	os << "[ " << e.vertexIndex0 << " " << e.vertexIndex1 << " ]";
//	if (e.flags.kOnPatchEdge)
//		os << "[OnPatchEdge]";
//	if (e.flags.kIsHookShortEdge)
//		os << "[IsHookShortEdge]";
//	if (e.flags.kIsHookLongEdge)
//		os << "[IsHookLongEdge]";
//	if (e.flags.kOnPatchEdge) {
//		os << "[ PI: " << e.patchidx << "] ";
//		os << "[ AE: " << e.adjpatchidx << "] ";
//		os << "[ CP0: " << e.model->splineArray.cpneighbors[e.cpvIndex0].cpids << " ]";
//		os << "[ CP1: " << e.model->splineArray.cpneighbors[e.cpvIndex1].cpids << " ]";
//	}
	return os;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// HEDGEARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

HEdgeArray::HEdgeArray() {}

HEdgeArray::HEdgeArray(const HEdgeArray &other) {
	classList = other.classList;
}

HEdgeArray::HEdgeArray(const unsigned s) {
	this->clear();
	classList.resize(s);
}

HEdgeArray::~HEdgeArray() {}

void HEdgeArray::clear() {
	classList.clear();
}

unsigned HEdgeArray::length() const {
	return classList.size();
}

void HEdgeArray::resize(const unsigned s) {
	classList.resize(s);
}

unsigned HEdgeArray::append(const HEdge &other) {
	classList.push_back(other);
	return classList.size()-1;
}

bool HEdgeArray::findEdge(HEdge &other,unsigned &index) {
	for (unsigned i = 0; i < this->length(); i++) {
		if (other == (*this)[i]) {
			index = i;
			return true;
		}
	}
	return false;
}

HEdge *HEdgeArray::get(unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range. [HEdgeArray]");
	}
	return &classList[index];
}
HEdge *HEdgeArray::begin() {
	return &classList.front();
}

HEdge *HEdgeArray::end() {
	return &classList.back();
}

HEdge HEdgeArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HEdgeArray]");
	}
	return classList[index];
}

HEdge &HEdgeArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HEdgeArray]");
	}
	return classList[index];
}

HEdgeArray &HEdgeArray::operator = ( const HEdgeArray &other ){
	classList = other.classList;
	return *this;
}

std::ostream &operator << (std::ostream &os,const HEdgeArray &other){
	for (unsigned i = 0; i < other.length(); i++)
		os << "[" << i << "] " << other[i] << std::endl;
	return os;
}