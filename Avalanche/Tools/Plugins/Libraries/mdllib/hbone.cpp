//////////////////////////////////////////////////////////////////////////////////////////
//
// HBONE CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HBone.h"
#include "HBoneChain.h"
#include "HBoneArray.h"

HBone::HBone() {
	memset(&flags,0,sizeof(HBone::bflags));
	parentIndex = 0;
	ikParentIndex = 0;
	chainIndex = -1;
	V9Bone = false;
}

HBone::HBone( const HBone &other ){
	memcpy(&flags,&other.flags,sizeof(HBone::bflags));
	name = other.name;
	cps = other.cps;
	pivot = other.pivot;
	end = other.end;
	rollhandle = other.rollhandle;
	parentIndex = other.parentIndex;
	ikParentIndex = other.ikParentIndex;
	boneArray = other.boneArray;
	chainIndex = other.chainIndex;
	rotate = other.rotate;
	length = other.length;
	falloff = other.falloff;
	color = other.color;
	V9Bone = other.V9Bone;
}

HBone::~HBone(){}

HVector HBone::getRollHandle() {
//	if (!V9Bone)
//		return rollhandle;

	HVector v(0,1,0);
	TransformVector(rotate,v);
	return (v*length)+pivot;//rollhandlelength = 0.40*bonelength
}

HVector HBone::getEnd() {
//	if (!V9Bone)
//		return end;

	HVector v(0,0,1);
	TransformVector(rotate,v);
	return (v*length)+pivot;
}

void HBone::clear() {
	memset(&flags,0,sizeof(HBone::bflags));
	name = "";
	cps.clear();
	pivot = HVector(0,0,0);
	end = HVector(0,0,0);
	rollhandle = HVector(0,0,0);
	parentIndex = 0;
	ikParentIndex = 0;
	chainIndex = -1;
	V9Bone=false;
}

HBone *HBone::previous() {
	if (this->flags.kHasParent)
		return boneArray->get(this->parentIndex);
	else
		return NULL;
}

HBone &HBone::operator = ( const HBone &other ){
	memcpy(&flags,&other.flags,sizeof(HBone::bflags));
	name = other.name;
	cps = other.cps;
	pivot = other.pivot;
	end = other.end;
	rollhandle = other.rollhandle;
	parentIndex = other.parentIndex;
	ikParentIndex = other.ikParentIndex;
	boneArray = other.boneArray;
	chainIndex = other.chainIndex;
	rotate = other.rotate;
	length = other.length;
	falloff = other.falloff;
	color = other.color;
	V9Bone = other.V9Bone;
	return *this;
}

std::ostream &operator << (std::ostream &os, HBone &other){
	os << "Bone [ " << other.name.asChar();
	if (other.flags.kHasParent)
		os << " Parent:" << other.boneArray->get(other.parentIndex)->name.asChar() << " ] ";
	else
		os << " ]";
//	os << "CP's: " << other.cps << std::endl;
	return os;
}