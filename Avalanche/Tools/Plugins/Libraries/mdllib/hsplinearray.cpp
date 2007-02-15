//////////////////////////////////////////////////////////////////////////////////////////
//
// HSPLINE ARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "hsplinearray.h"
#include "hmacros.h"

HSplineArray::HSplineArray(){
	absCpIdxMap = new HIntMap();
	abscpindex = 0;
	model = NULL;
	neighbors.model = NULL;
}

HSplineArray::HSplineArray(const HSplineArray &other) {
	vertices = other.vertices;
	hookList = other.hookList;
	neighbors = other.neighbors;
	splineTable = other.splineTable;
	absCpiTable = other.absCpiTable;
	relCpiTable = other.relCpiTable;
	
	absCpIdxMap = new HIntMap();
	*absCpIdxMap = *other.absCpIdxMap;

	model = other.model;
	classList = other.classList;
}

HSplineArray::~HSplineArray() {
	delete absCpIdxMap;
}

void HSplineArray::clear() {
	abscpindex = 0;
	vertices.clear();
	hookList.clear();
	neighbors.clear();
	splineTable.clear();
	absCpiTable.clear();
	relCpiTable.clear();

	absCpIdxMap = NULL;
	
	model = NULL;
	classList.clear();
}

unsigned HSplineArray::length() const {
	return classList.size();
}

void HSplineArray::append(HSpline spline) {
	this->classList.push_back(spline);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// traverses cpIndex table and finds next available cp index
//////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned HSplineArray::getNewCPindex() {
	return cpIndexTable->size();
}

unsigned HSplineArray::getNewCPindex2() {
	unsigned temp = (*hashCPTable)[abscpindex];
	abscpindex++;
	return temp;
}

unsigned HSplineArray::HashCPIndexToMyCPIndex(unsigned cpindex) {
	return cpIdxMap->getValue(cpindex);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// finalize tangent for coordinate space
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void HSplineArray::finalizeTangent(HVector p2, HVector &it, HVector &ot) {
	it = p2-(it/3.0);
	ot = p2+(ot/3.0);	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// calculate tangents for all splines
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void HSplineArray::calculateTangents() {
	if (classList.size() == 0)
		return;
	for (HSpline *iter = this->begin(); iter != NULL; iter = iter->next())
		iter->calculateTangents();
}

void HSplineArray::setHooks() {
	HVector hookpoint;
	unsigned vi;
	for (HSpline *iter0 = this->begin(); iter0 != NULL; iter0 = iter0->next()) {
		for (HCP *iter1 = iter0->head(); iter1 != NULL; iter1 = iter1->next()) {
			if (iter1->flags.kIsHook) {
				findHookVertex(iter1,hookpoint);
				vi = addVertexToList(hookpoint);
				hookList[iter1->hookIndex].vindex = vi;
			}
		}
	}
}

void HSplineArray::findHookVertex(HCP *hookcp,HVector &point) {
	HHook *hook = hookList.get(hookcp->hookIndex);
	HCP *hookbase = getCP(hook->hookbaseidx);
	if (hook->hookbaseneighboridx.size()==0)
		THROW_FATAL_ERROR("Bad hook neighbor.");
	HCP *hookbaseneighbor = getCP(hook->hookbaseneighboridx[0]);
	HSpline *spline = this->get(hookbase->splineIndex);
	HVector v0,v1,it,ot;

	spline->calculateTangents();

	v0 = getCPPos(hookbase);
	v1 = getCPPos(hookbaseneighbor);

	ot = hookbase->ot;
	it = hookbaseneighbor->it;

	HSplineSegment seg(v0,ot,it,v1);
	point = seg.getPointAtParam(0.5);
}

void HSplineArray::splitHookedSegment(HCP *hookcp,HSplineSegment &seg1,HSplineSegment &seg2) {
	HHook *hook = hookList.get(hookcp->hookIndex);
	HCP *hookbase = getCP(hook->hookbaseidx);
	HCP *hookbaseneighbor = getCP(hook->hookbaseneighboridx[0]);

	HVector v0,v1,v3,it,ot;

	v0 = getCPPos(hookbase);
	v1 = getCPPos(hookbaseneighbor);
	v3 = getHookPos(hook);

	ot = hookbase->ot;
	it = hookbaseneighbor->it;

	HSplineSegment seg(v0,ot,it,v1);
	seg.splitSplineSegment(seg1,seg2);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// This function assigns parent, children, and hook information.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void HSplineArray::setupCPInfo() {
	bool bFinished=false;
	HCP *cp,*cpinstance;
//	HIntIterator iter;
	std::vector<int> childstack;
	bool foundHook;
	unsigned hookidx = 0;
	HHook *hookptr,*hookptr2,*hookptr3,*hookptr4;
	HCP *hookcp,*hookneighborcp,*firstcp;
	unsigned parentidx;
	HCP *oldcp;
	unsigned i;

	unsigned numberofhooks = 0;

//	Get list of parent cp's
	unsigned totalcps = cpIndexTable->size();
	bool *ischild = new bool[totalcps];
	memset(ischild,false,sizeof(bool)*totalcps);

	for (unsigned sidx = 0; sidx < (*this).length(); sidx++) {
		for (unsigned cidx = 0; cidx < (*this)[sidx].length(); cidx++) {
			cp = (this->get(sidx))->get(cidx);
			if (cp->flags.kIsInstance)
				ischild[cp->instanceIndex] = true;
		}
	}

	std::vector<int> parentlist;
	for (i = 0; i < totalcps; i++)
		if (!ischild[i])
			parentlist.push_back(i);
	delete ischild;

	for (i = 0; i < parentlist.size(); i++) {
		cp = getCP(parentlist[i]);

		if (cp->flags.kIsInstance) {

				getChildStack(cp->cpindex,childstack);

////////////////	SETUP PARENT/CHILD INFO
				parentidx = cp->cpindex;
				oldcp = cp;
///////////////////////////////////////////////////////////////////////////////////////////////////

				if (cp->flags.kIsHook) {
					foundHook = true;
					hookidx = cp->cpindex;
					numberofhooks++;
				}

//				for (iter.reset(childstack);!iter.isDone();iter++) {

				for (unsigned j = 0; j < childstack.size(); j++)
				{
	

////////////////	SETUP PARENT/CHILD INFO					
					cpinstance = getCP(childstack[j]);
					oldcp->childIndex = childstack[j];
					oldcp->flags.kHasChild = 1;
					oldcp = cpinstance;
					cpinstance->parentIndex = parentidx;
					cpinstance->flags.kHasParent = 1;
					parentidx = cpinstance->cpindex;


				
////////////////	SETUP HOOK INFO
					if (cpinstance->flags.kIsHook) {
						foundHook = true;
						hookidx = cpinstance->cpindex;
						numberofhooks++;
					}

					if ((cpinstance->flags.kIsHookbase)&&(foundHook)) {
						
//						Assigns HOOK BASE INDEX to HOOK
						hookcp = getCP(hookidx);
						hookptr2 = hookList.get(hookcp->hookIndex);
						hookptr2->hookbaseidx = cpinstance->cpindex;

//						Assigns HOOK INDEX to HOOK BASE
						hookptr3 = hookList.get(cpinstance->hookIndex);
						hookptr3->hookidx = hookidx;

//						Assigns HOOK INDEX to HOOK NEIGHBOR
						hookneighborcp = cpinstance->next();
						hookptr = hookList.get(hookneighborcp->hookIndex);
						if (!hookptr->flags.kIshookbase)
							hookptr->hookidx = hookidx;

//						Assigns HOOK INDEX to HOOK NEIGHBOR
						firstcp = (this->get(hookneighborcp->splineIndex))->head();						
						hookptr4 = hookList.get(firstcp->hookIndex);
						if (!hookptr4->flags.kIshookbase)
							hookptr4->hookidx = hookidx;

//						Assigns HOOKBASE NEIGHBOR INDEX to HOOK
						hookptr2->hookbaseneighboridx = hookptr3->hookbaseneighboridx;
					}
///////////////////////////////////////////////////////////////////////////////////////////////////
			}
		}
		if (numberofhooks > 1) {
			char cpidx[256];
			itoa(cp->cpindex,cpidx,10);
			UString errstrng = UString("Multiple hooks found. [CP #") + UString(cpidx) + UString("]");
			THROW_FATAL_ERROR(errstrng.asChar());
		}
		numberofhooks=0;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// digs through cpstack for 5 point vertex flag
//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool HSplineArray::drillFor5pointVertex(unsigned cpindex,unsigned &index) {

	HCP *cp = getCP(cpindex);

	if (cp->flags.kIsLoop)
		cp = cp->head();

	while (cp->flags.kHasChild)
	{
		if (cp->flags.kIs5PointPatchVertex) {
			index = cp->cpindex;
			return true;
		}
		cp = getChildCP(cp->cpindex);
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// digs through cpstack for hook
//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool HSplineArray::drillForHook(unsigned cpindex,unsigned &index) {

	HCP *cp = getCP(cpindex);

	if (cp->flags.kIsLoop)
		cp = cp->head();

	while (cp->flags.kHasChild)
	{
		if (cp->flags.kIsHook) {
			index = cp->cpindex;
			return true;
		}
		cp = getChildCP(cp->cpindex);
	}
	return false;
}


bool HSplineArray::drillForHook(HIntMap &stackmap,unsigned &index) {
	bool is_hook;
	unsigned cpidx;
	
	for (unsigned i = 0; i < stackmap.length(); i++) {
		cpidx = stackmap.getKey(i);
		is_hook = isHook(cpidx);
		if (is_hook) {
			index = cpidx;
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// digs through cpstack for hookbase
//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool HSplineArray::drillForHookbase(HIntMap &stackmap,unsigned &index,HHook *hook) {
	unsigned cpidx;
	
	for (unsigned i = 0; i < stackmap.length(); i++) {
		cpidx = stackmap.getKey(i);
		if (hook->hookbaseidx == cpidx) {
			index = cpidx;
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// digs through cpstack for hookbase neighbor
//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool HSplineArray::drillForHookbaseNeighbor(HIntMap &stackmap,unsigned &index,HHook *hook) {
	HStatus status;
	unsigned cpidx;
	
	for (unsigned i = 0; i < stackmap.length(); i++) {
		cpidx = stackmap.getKey(i);

		if (hook->hookbaseneighboridx.size() > 1) {
			if ((hook->hookbaseneighboridx[0] == cpidx)||(hook->hookbaseneighboridx[1] == cpidx)) {
				index = cpidx;
				return true;
			}
		}
		else if (hook->hookbaseneighboridx.size() > 0) {
			if (hook->hookbaseneighboridx[0] == cpidx) {
				index = cpidx;
				return true;
			}
		}
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Hook and hookbase test methods
//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool HSplineArray::isHook(unsigned cpindex) {
	HCP *cp = this->getCP(cpindex);
	if (cp->flags.kIsHook)
		return true;
	return false;
}

bool HSplineArray::isHookbase(unsigned cpindex) {
	HCP *cp = this->getCP(cpindex);
	if (cp->flags.kIsHookbase)
		return true;
	return false;
}

bool HSplineArray::isHookbaseNeighbor(unsigned cpindex) {
	HCP *cp = this->getCP(cpindex);
	if (cp->flags.kIsHookbaseNeighbor)
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	cp retrieval methods
//	WARNING: THESE METHODS ARE OBSOLETE
//////////////////////////////////////////////////////////////////////////////////////////////////////////

HCP *HSplineArray::getNextCP(unsigned current) {
	unsigned relCPIndex,splineIndex;
	findSplineRelativeCP(current,relCPIndex,splineIndex);
	return this->getCP(relCPIndex+1,splineIndex);
}

HCP *HSplineArray::getPreviousCP(unsigned current) {
	unsigned relCPIndex,splineIndex;
	findSplineRelativeCP(current,relCPIndex,splineIndex);
	return this->getCP(relCPIndex-1,splineIndex);
}

// Is cp a head cp?
bool HSplineArray::isHeadCP(unsigned cpindex) {
	HCP *cp = getCP(cpindex);
	if (cp->flags.kIsHead)
		return true;
	return false;
}

// Is cp a tail cp?
bool HSplineArray::isTailCP(unsigned cpindex) {
	HCP *cp = getCP(cpindex);
	if (cp->flags.kIsTail)
		return true;
	return false;
}

//	get head cp
HCP *HSplineArray::getHeadCP(unsigned cpindex) {
	unsigned relCPIndex,splineIndex;
	findSplineRelativeCP(cpindex,relCPIndex,splineIndex);
	HSpline *spline = this->get(splineIndex);
	return spline->head();
}

//	get head cp
HCP *HSplineArray::getHeadCP(HCP *cp) {
	HSpline *spline = this->get(cp->splineIndex);
	return spline->head();
}

HCP *HSplineArray::getTailCP(unsigned cpindex) {
	unsigned relCPIndex,splineIndex;
	findSplineRelativeCP(cpindex,relCPIndex,splineIndex);
	HSpline *spline = this->get(splineIndex);
	return spline->tail();
}

//	get head cp
HCP *HSplineArray::getTailCP(HCP *cp) {
	HSpline *spline = this->get(cp->splineIndex);
	return spline->tail();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// uses absolute cp indicies
// -1 means indicies is against spline direction
// 1 means indicies is with spline direction
//////////////////////////////////////////////////////////////////////////////////////////////////////////

int HSplineArray::getSplineDirection(unsigned index1,unsigned index2) {
	unsigned relCPIndex,splineIndex;
	unsigned relCPIndex2,splineIndex2;

	findSplineRelativeCP(index1,relCPIndex,splineIndex);
	findSplineRelativeCP(index2,relCPIndex2,splineIndex2);

	if (splineIndex != splineIndex2) {
		THROW_FATAL_ERROR("Index values do not lie on the same line.[getsplinedirection]");
	}
	
	HSpline *spline = this->get(splineIndex);
	return spline->getDirection(relCPIndex,relCPIndex2);
}

bool HSplineArray::doesCPHookbasePointToIndex(HCP *cp,unsigned idx) {
	if (hookList[cp->hookIndex].hookbaseidx == idx)
		return true;
	return false;
}

bool HSplineArray::doesCPHookbaseNeighborPointToIndex(HCP *cp,unsigned idx) {
	if (hookList[cp->hookIndex].hookbaseneighboridx.size() > 0) {
		if (hookList[cp->hookIndex].hookbaseneighboridx.size() > 1) {
			if ((hookList[cp->hookIndex].hookbaseneighboridx[0] == idx)||
				(hookList[cp->hookIndex].hookbaseneighboridx[1] == idx))
				return true;
		}
		else 
			if (hookList[cp->hookIndex].hookbaseneighboridx[0] == idx)
				return true;
	}
	else {
		THROW_FATAL_ERROR("Empty hookbase neighbor index.");
	}
	return false;
}

// get pointer to spline at index
HSpline *HSplineArray::get(unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[hsplinearray]");
	}
	return &classList[index];
}

HSpline *HSplineArray::begin() {
	return &classList.front();
}

HSpline *HSplineArray::end() {
	return &classList.back();
}

void HSplineArray::updateBackPointers() {
	for (unsigned i = 0; i < this->length(); i++) {
		HSpline *spline = this->get(i);
		spline->splineArray = this;
		for (unsigned j = 0; j < spline->length(); j++) {
			HCP *cp = spline->get(j);
			cp->splineArray = this;
		}
	}
}

HSplineArray &HSplineArray::operator = ( const HSplineArray &other ){
	vertices = other.vertices;
	hookList = other.hookList;
	neighbors = other.neighbors;
	splineTable = other.splineTable;
	absCpiTable = other.absCpiTable;
	relCpiTable = other.relCpiTable;

	absCpIdxMap = new HIntMap();
	*absCpIdxMap = *other.absCpIdxMap;

	model = other.model;
	classList = other.classList;
	return *this;
}

HSpline HSplineArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[hsplinearray]");
	}
	return classList[index];
}

HSpline &HSplineArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[hsplinearray]");
	}
	return classList[index];
}

std::ostream &operator << (std::ostream &os, HSplineArray &other){
	for (HSpline *iter = other.begin(); iter != NULL; iter = iter->next()) {
		os << "{ SPLINE: " << iter->index << " }" << std::endl;
		os << *iter << std::endl;
	}
	return os;
}

