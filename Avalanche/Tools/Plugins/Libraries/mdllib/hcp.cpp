//////////////////////////////////////////////////////////////////////////////////////////
//
//	HCP CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hcp.h"
#include "hspline.h"
#include "hsplinearray.h"
#include "hmacros.h"

HCP::HCP() {
	memset(&flags,0,sizeof(HCP::cpflags));
	vindex = -1;
	cpindex = -1;
	splineIndex = 0;
	splineArray = NULL;
	hookIndex = 0;
	parentIndex = 0;
	childIndex = 0;
	instanceIndex = 0;
	index = 0;
}

HCP::HCP(const HCP &other) {
	memcpy(&this->flags,&other.flags,sizeof(HCP::cpflags));
	this->vindex = other.vindex;
	this->cpindex = other.cpindex;
	this->inBias = other.inBias;
	this->outBias = other.outBias;
	this->instanceIndex = other.instanceIndex;
	this->it = other.it;
	this->ot = other.ot;
	this->splineIndex = other.splineIndex;
	this->splineArray = other.splineArray;
	this->hookIndex = other.hookIndex;
	this->parentIndex = other.parentIndex;
	this->childIndex = other.childIndex;
	this->index = other.index;
}

HCP::~HCP(){}

void HCP::clear() {
	memset(&flags,0,sizeof(HCP::cpflags));
	vindex = -1;
	cpindex = -1;
	splineIndex = 0;
	splineArray = NULL;
	it = HVector(0,0,0);
	ot = HVector(0,0,0);
	hookIndex = 0;
	parentIndex = 0;
	childIndex = 0;
	instanceIndex = 0;
	index = 0;
}

void HCP::create(HVector pos) {
	memset(&flags,0,sizeof(HCP::cpflags));

	// add to vertex table
	this->vindex = splineArray->addVertexToList(pos);

	// get new CP index
	this->cpindex = this->splineArray->getNewCPindex();

	// add to cp index table
	this->splineArray->cpIndexTable->push_back(this->cpindex);
}

void HCP::create(HCP other) {
	memset(&flags,0,sizeof(HCP::cpflags));

	this->inBias = other.inBias;
	this->outBias = other.outBias;

	this->it = other.it;
	this->ot = other.ot;

	this->instanceIndex = 0;
	this->flags.kIsInstance = 0;

	this->splineIndex = other.splineIndex;
	this->splineArray = other.splineArray;

	// add to vertex table
	this->vindex = splineArray->addVertexToList(HVector(0,0,0));

	// get new CP index
	this->cpindex = this->splineArray->getNewCPindex();

	// add to cp index table
	this->splineArray->cpIndexTable->push_back(this->cpindex);
	
	this->flags.kIsSmooth = other.flags.kIsSmooth;
}

void HCP::createInstance(unsigned cpidx) {
	memset(&flags,0,sizeof(HCP::cpflags));

	this->flags.kIsInstance = 1;
	this->instanceIndex = cpidx;

	this->vindex = -1;

	// get new CP index
	this->cpindex = this->splineArray->getNewCPindex();

	// add to cp index table
	this->splineArray->cpIndexTable->push_back(this->cpindex);
}

void HCP::createInstance(HCP other) {
	memset(&flags,0,sizeof(HCP::cpflags));

	this->flags.kIsSmooth = other.flags.kIsSmooth;

	this->flags.kIsInstance = 1;
	this->instanceIndex = other.cpindex;

	this->inBias = other.inBias;
	this->outBias = other.outBias;
	this->it = other.it;
	this->ot = other.ot;
	this->splineIndex = other.splineIndex;
	this->splineArray = other.splineArray;
	this->vindex = -1;

	// get new CP index
	this->cpindex = this->splineArray->getNewCPindex();

	// add to cp index table
	this->splineArray->cpIndexTable->push_back(this->cpindex);
}

void HCP::create_looping_cp(HCP other) {
	memset(&flags,0,sizeof(HCP::cpflags));

	this->inBias = other.inBias;
	this->outBias = other.outBias;

	this->it = other.it;
	this->ot = other.ot;

	this->instanceIndex = 0;
	this->flags.kIsLoop = 1;

	this->splineIndex = other.splineIndex;
	this->splineArray = other.splineArray;

	// add to vertex table
	this->vindex = other.vindex;

	// get new CP index
	this->cpindex = this->splineArray->getNewCPindex();

	// add to cp index table
	this->splineArray->cpIndexTable->push_back(this->cpindex);
	
	this->flags.kIsSmooth = other.flags.kIsSmooth;
}

///////////////////////////////////////////////////////////////////////////////////////////
//	EXPLANATION FOR EXCEPTION:
//	USE HASH'S NUMBERING CONVENTION
///////////////////////////////////////////////////////////////////////////////////////////

void HCP::create_UHNC(HVector pos) {//special case for renumbering hash cp's
	// add to vertex table
	this->vindex = splineArray->addVertexToList(pos);
	// get new cp index
	this->cpindex = splineArray->getNewCPindex2();
}

void HCP::createInstance_UHNC(unsigned cpidx) {//special case for renumbering hash cp's
	this->flags.kIsInstance = 1;
	this->instanceIndex = cpidx;
	this->vindex = -1;
	// get new cp index
	this->cpindex = splineArray->getNewCPindex2();
}

///////////////////////////////////////////////////////////////////////////////////////////

void HCP::setFlagsFromOldFlags(unsigned long flags)
{
/*
	struct {
        unsigned long       issmooth                : 1,   // 1 
							isflip                  : 1,   // 2 
							isloop                  : 1,   // 3 
							isdependent             : 1,   // 4 
							ishook                  : 1,   // 5 
							ishide                  : 1,   // 6 
							isgroup                 : 1,   // 7 
							isprevskin              : 1,   // 8 
							isclonegroup            : 1,   // 9 
							islocked                : 1,   // 10
							isdmconvert			  : 1,   // 11
							isdmuscle				  : 1,   // 12
							isendhook               : 1,   // 13
							isneedsdrawn            : 1,   // 14
							isdit					  : 1,   // 15
							isdot					  : 1,   // 16
							isassociatedgrouped     : 1,   // 17
							isnextskin              : 1,   // 18
							ismuscleconvert         : 1,   // 19
							isdscreenit			  : 1,   // 20
							isdscreenot			  : 1,   // 21
							isdscreenp			  : 1,   // 22
							ismusclecp              : 1,   // 23
							UNUSED1                   : 1,   // 24
							UNUSED2                   : 1,   // 25
							UNUSED3                   : 1,   // 26
							UNUSED4                   : 1,   // 27
							isdmusclebias			  : 1,   // 28
							isdmp					  : 1,   // 29
							isdmit				  : 1,   // 30
							isdmot				  : 1,   // 31
							ishookbase              : 1;   // 32
	} oldflags;
*/
     struct {
         unsigned long issmooth                : 1,   // 1 
                     isflip                  : 1,   // 2 
                     isloop                  : 1,   // 3 
                     U1						: 1,   // so that muscle motion does not flip with bias normals
                     ishook                  : 1,   // 5 
                     ishide                  : 1,   // 6 
                     isgroup                 : 1,   // 7 
                     isprevskin              : 1,   // 8 
                     isclonegroup            : 1,   // 9 
                     islocked                : 1,   // 10
                     m_ishookcomputed        : 1,   // 11
                     m_ismodelhookcomputed   : 1,   // 12
                     isendhook               : 1,   // 13
                     U2					    : 1,   // 14
                     m_iscachedtangents      : 1,   // 15
                     ishookbase              : 1,   // 16
                     isassociatedgrouped     : 1,   // 17
                     isnextskin              : 1,   // 18
                     m_isbiasnormal          : 1,   // which default tangent algorithm is to be used
                     U14                     : 5,   // 24
                     m_screentangentcounter  : 8;   // 32
	 } oldflags;


	HCP::cpflags n;
	memset(&n,0,sizeof(HCP::cpflags));
	memcpy(&oldflags,&flags,sizeof(unsigned long));
	
	if (oldflags.issmooth)
		n.kIsSmooth = 1;
	if (oldflags.isloop)
		n.kIsLoop = 1;
	if (oldflags.ishook)
		n.kIsHook = 1;
	if (oldflags.ishookbase)
		n.kIsHookbase = 1;

	setFlags(n);
}

void HCP::setFlags(unsigned long flags) {
	memcpy(&this->flags,&flags,sizeof(HCP::cpflags));
}

void HCP::setFlags(HCP::cpflags flags) {
	memcpy(&this->flags,&flags,sizeof(HCP::cpflags));
}

unsigned long HCP::getFlags() {
	unsigned long flags;
	memcpy(&flags,&this->flags,sizeof(HCP::cpflags));
	return flags;
}

void HCP::initialize_hook(HHook hook) {
	bool flag;

	if (this->flags.kIsHook) {
		hook.flags.kIshook = 1;
		flag = true;
	}
	if (this->flags.kIsHookbase) {
		hook.flags.kIshookbase = 1;
		flag = true;
	}
	if (this->flags.kIsHookbaseNeighbor) {
		hook.flags.kIshookbaseneighbor = 1;
		flag = true;
	}
	if (flag)
		this->hookIndex = this->splineArray->hookList.append(hook);
}

/*
HCP *HCP::next() {
	HSpline *spline = splineArray->get(this->splineIndex);
	HCP *cp = this;
	if (++cp > spline->tail())
		return NULL;
	return cp;
}

HCP *HCP::previous() {
	HSpline *spline = splineArray->get(this->splineIndex);
	HCP *cp = this;
	if (--cp < spline->head())
		return NULL;
	return cp;
}
*/

HCP *HCP::next() {
	if (this == splineArray->get(this->splineIndex)->tail())
		return NULL;
	return this+1;
}

HCP *HCP::previous() {
	if (this == splineArray->get(this->splineIndex)->head())
		return NULL;
	return this-1;
}

HCP *HCP::head() {
	HSpline *spline = splineArray->get(this->splineIndex);
	return spline->head();
}

HCP *HCP::tail() {
	HSpline *spline = splineArray->get(this->splineIndex);
	return spline->tail();
}

HCP &HCP::operator = ( const HCP &other ){
	memcpy(&this->flags,&other.flags,sizeof(HCP::cpflags));
	this->vindex = other.vindex;
	this->cpindex = other.cpindex;
	this->inBias = other.inBias;
	this->outBias = other.outBias;
	this->instanceIndex = other.instanceIndex;
	this->it = other.it;
	this->ot = other.ot;
	this->splineIndex = other.splineIndex;
	this->splineArray = other.splineArray;
	this->hookIndex = other.hookIndex;
	this->parentIndex = other.parentIndex;
	this->childIndex = other.childIndex;
	this->index = other.index;
	return *this;
}

std::ostream &operator << (std::ostream &os, HCP &cp){
	os << "Relative index: " << cp.index << std::endl;
	os << "* CP Index: " << cp.cpindex << std::endl;

	if (!cp.flags.kIsInstance)
		os << "Vertex Index: " << cp.vindex << std::endl;

	if (cp.flags.kHasParent)
		os << "* Parent Index: " << cp.parentIndex << " ";
	if (cp.flags.kHasChild)
		os << "Child Index: " << cp.childIndex << " ";

	if ((cp.flags.kHasParent)||(cp.flags.kHasChild))
		os << std::endl;

	os << "Flags: ";
	if (cp.flags.kIsSmooth)
		os << "kIsSmooth ";
	if (cp.flags.kIsHead)
		os << "kIsHead ";
	if (cp.flags.kIsTail)
		os << "kIsTail ";
	if (cp.flags.kIsHashLoop)
		os << "kIsHashLoop ";
	if (cp.flags.kIsLoop)
		os << "kIsLoop ";
	if (cp.flags.kIsHook)
		os << "kIsHook ";
	if (cp.flags.kIsHookbase)
		os << "kIsHookbase ";
	if (cp.flags.kIsHookbaseNeighbor)
		os << "kIsHookbaseNeighbor ";
	if (cp.flags.kIs5PointPatchVertex)
		os << "kIs5PointPatchVertex ";
	
	os << std::endl;

	if ((cp.flags.kIsHook)||(cp.flags.kIsHookbase)||(cp.flags.kIsHookbaseNeighbor))
		os << cp.splineArray->hookList[cp.hookIndex];
	return os;
}
