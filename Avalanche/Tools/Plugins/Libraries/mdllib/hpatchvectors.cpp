#include "Stdafx.h"
#include "hstatus.h"
#include "hpatchvectors.h"
#include "hmacros.h"

/////////////////////////////////////////////////////////////////////////////////////////
HPatchVectors::HPatchVectors(){
	memset(&flags,0,sizeof(HPatchVectors::pvFlags));
	subPatchIndex = -1;
	patchIndex = -1;
}

HPatchVectors::HPatchVectors(const HPatchVectors &other) {
	memcpy(&this->flags,&other.flags,sizeof(unsigned long));
	pv = other.pv;
	subPatchIndex = other.subPatchIndex;
	patchIndex = other.patchIndex;
	for (unsigned i = 0; i < 4; i++) 
		tessedges[i] = other.tessedges[i];
}

HPatchVectors::~HPatchVectors(){}

void HPatchVectors::clear() {
	memset(&flags,0,sizeof(HPatchVectors::pvFlags));
	pv.clear();
	subPatchIndex = -1;
	patchIndex = -1;
}

void HPatchVectors::append(HVector v) {
	pv.classList.push_back(v);
}

HPatchVectors &HPatchVectors::operator = ( HVector *other ){
	pv.clear();
	for (unsigned i = 0; i < 16; i++)
		pv.append(other[i]);
	return *this;
}

HPatchVectors &HPatchVectors::operator = ( const HPatchVectors &other ){
	memcpy(&this->flags,&other.flags,sizeof(unsigned long));
	pv = other.pv;
	subPatchIndex = other.subPatchIndex;
	patchIndex = other.patchIndex;
	for (unsigned i = 0; i < 4; i++) 
		tessedges[i] = other.tessedges[i];
	return *this;
}

HPatchVectorsArray::HPatchVectorsArray() {}

HPatchVectorsArray::~HPatchVectorsArray() {}

void HPatchVectorsArray::clear() {
	classList.clear();
}

unsigned HPatchVectorsArray::length() const {
	return classList.size();
}

unsigned HPatchVectorsArray::append(const HPatchVectors &pVectors) {
	classList.push_back(pVectors);
	return classList.size()-1;
}

HPatchVectors *HPatchVectorsArray::get(unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[hpatchvectorsarray");
	}
	return &classList[index];
}

HPatchVectors *HPatchVectorsArray::begin() {
	return &classList.front();
}

HPatchVectors *HPatchVectorsArray::end() {
	return &classList.back();
}

HPatchVectors HPatchVectorsArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HPatchVectorsArray]");
	}
	return classList[index];
}

HPatchVectors &HPatchVectorsArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HPatchVectorsArray]");
	}
	return classList[index];
}

HPatchVectorsArray &HPatchVectorsArray::operator = ( const HPatchVectorsArray &other ){
	classList = other.classList;
	return *this;
}

std::ostream &operator << (std::ostream &os,const HPatchVectors &other){
//	os << v.pv;
	for (unsigned j = 0; j < 4; j++)
		os << j << ") " << other.tessedges[j] << std::endl;
	return os;
}

std::ostream &operator << (std::ostream &os,const HPatchVectorsArray &other){
	for (unsigned i = 0;i < other.length(); i++) {
		std::cerr << "Patch Vector (" << i << ")" << std::endl;
		os << other[i];
	}
	return os;
}