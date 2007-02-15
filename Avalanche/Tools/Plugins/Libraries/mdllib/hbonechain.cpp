//////////////////////////////////////////////////////////////////////////////////////////
//
// HBONE CHAIN CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hbonechain.h"
#include "hbonearray.h"
#include "hmacros.h"
#include "hstatus.h"

HBoneChain::HBoneChain() {}

HBoneChain::HBoneChain(HBoneArray *other) {
	boneArray = other;
}

HBoneChain::~HBoneChain() {}

void HBoneChain::clear(HBoneArray *other) {
	boneArray = other;
	classList.clear();
}

unsigned HBoneChain::length() const {
	return classList.size();
}

void HBoneChain::resize(const unsigned s) {
	classList.resize(s);
}

unsigned HBoneChain::append(unsigned boneindex) {
	classList.push_back(boneindex);
	return classList.size()-1;
}

void HBoneChain::remove(unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HBoneChain]");
	}
	classList.erase(classList.begin() + index);
}

unsigned HBoneChain::getIndex(unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range. [HBoneChain]");
	}
	return classList[index];
}

HBone *HBoneChain::get(unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range. [HBoneChain]");
	}
	return boneArray->get(classList[index]);
}

HBone *HBoneChain::begin() {
	return boneArray->get(classList.front());
}

HBone *HBoneChain::end() {
	return boneArray->get(classList.back());
}

HBone HBoneChain::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HBoneChain]");
	}
	return *boneArray->get(classList[index]);
}

HBone &HBoneChain::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HBoneChain]");
	}

	return *boneArray->get(classList[index]);
}
HBoneChain &HBoneChain::operator = ( const HBoneChain &other ){
	classList = other.classList;
	return *this;
}

std::ostream &operator << (std::ostream &os,HBoneChain &other){
	os << "[[ ";
	for (unsigned i = 0; i < other.length(); i++)
		os << other[i];
	os << " ]]" << std::endl;
	return os;
}