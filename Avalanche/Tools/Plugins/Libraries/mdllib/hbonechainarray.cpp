//////////////////////////////////////////////////////////////////////////////////////////
//
// HBONECHAINARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "hbonearray.h"
#include "hbonechainarray.h"
#include "hmacros.h"
#include "hstatus.h"

HBoneChainArray::HBoneChainArray() {}

HBoneChainArray::~HBoneChainArray() {}

void HBoneChainArray::clear() {
	classList.clear();
}

unsigned HBoneChainArray::length() const {
	return classList.size();
}

void HBoneChainArray::resize(const unsigned s) {
	classList.resize(s);
}

unsigned HBoneChainArray::append(HBoneChain &other) {
	classList.push_back(other);
	return classList.size()-1;
}

HBoneChain *HBoneChainArray::get(unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range. [HBone]");
	}
	return &classList[index];
}

HBoneChain *HBoneChainArray::begin() {
	return &classList.front();
}

HBoneChain *HBoneChainArray::end() {
	return &classList.back();
}

// this function reuses old bone pool
void HBoneChainArray::DestroyControlBones(HBoneChainArray &bonechainarray) {
	UString test;
	HBone *bone;
	
	if (this->length() == 0) return;

	HBoneArray *bonearray = this->get(0)->boneArray;
	HBoneChain *bonechain,newchain(bonearray);

	bonechainarray.clear();

	int index;

	for (int i = 0; i < (int)this->length(); i++) {
		bonechain = this->get(i);
		for (int j = 0; j < (int)bonechain->length(); j++) {
			bone = bonechain->get(j);
			test = (bone->name.allLower()).subString(0,4);

			if (test == "cont") {
				//thow out control bones
			}
			else if (test == "prox") {
				//thow out proxy bones
			}
			else {
				index = bonechain->getIndex(j);
				newchain.append(index);
			}
		}
		bonechainarray.append(newchain);
		newchain.clear(bonearray);
	}
}

HBoneChain HBoneChainArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HBoneChainArray]");
	}
	return classList[index];
}

HBoneChain &HBoneChainArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HBoneChainArray]");
	}
	return classList[index];
}

HBoneChainArray &HBoneChainArray::operator = ( const HBoneChainArray &other ){
	classList = other.classList;
	return *this;
}

std::ostream &operator << (std::ostream &os,HBoneChainArray &other){
	for (unsigned i = 0; i < other.length(); i++) {
		os << "Bone Chain Index ( " << i << " )" << std::endl;
		os << other[i] << std::endl;
	}
	return os;
}