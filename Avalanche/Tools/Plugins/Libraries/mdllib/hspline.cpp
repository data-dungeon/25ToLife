//////////////////////////////////////////////////////////////////////////////////////////
//
// HSPLINE CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "hspline.h"
#include "hsplinearray.h"
#include "hmacros.h"
#include "hquaternion.h"

HSpline::HSpline() {
	splineArray = NULL;
}

HSpline::HSpline(const HSpline &other) {
	classList = other.classList;
	splineArray = other.splineArray;
	index = other.index;
}

HSpline::~HSpline() {}

void HSpline::clear() {
	classList.clear();
	splineArray = NULL;
	index = 0;
}

unsigned HSpline::length() const {
	return classList.size();
}

unsigned HSpline::append(const HCP &cp) {
	this->classList.push_back(cp);
	return classList.size()-1;
}

unsigned HSpline::add(HCP &cp) {
	cp.index = classList.size();
	cp.splineIndex = this->index;
	this->classList.push_back(cp);
	this->splineArray->relCpiTable.push_back(classList.size()-1);
	this->splineArray->absCpiTable.push_back(cp.cpindex);
	this->splineArray->splineTable.push_back(this->index);
	this->splineArray->absCpIdxMap->setValue(cp.cpindex,this->splineArray->absCpiTable.size()-1);
	return classList.size()-1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// calculates tangents for spline and places them in each cp's in & out.
// WARNING: these tangents are not finalized! Finalization takes place after getSplineSegment
// manually by user.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void HSpline::calculateTangents(void) {
	HVector p0, p1, p2, p3, it, ot, gammaplane;
	HBias inBias, outBias;
	HCP *cp;
	HHookArray *hooks = &this->splineArray->hookList;
	HSplineArray *splines = this->splineArray;

//	If straight line
	if (classList.size() < 3) {
		p0 = splineArray->getCPPos(this->head());
		p1 = splineArray->getCPPos((this->head())->next());
		(this->head())->ot = (p1 - p0) * 0.5f;
		((this->head())->next())->it = (p1 - p0) * 0.5;
		return;
	}

	cp = this->head();
	calcHeadTangent(cp);
	cp = cp->next();
	while (!cp->flags.kIsTail) {
		calcMiddleTangent(cp);
		cp = cp->next();
	}
	calcTailTangent(cp);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calculates tangent for head cp
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void HSpline::calcHeadTangent(HCP *cp) {
	HVector gammaplane;
	HBias outBias;
	HVector ot;
	HVector p0,p1,p2;
	HHookArray *hooks = &this->splineArray->hookList;

	if (!cp->flags.kIsHook)
		p0 = splineArray->getCPPos(cp); //get first vertex
	else
		p0 = splineArray->getHookPos(cp->hookIndex);
	
	p1 = splineArray->getCPPos(cp->next()); //get next vertex

	if (!((cp->next())->next())->flags.kIsHook)
		p2 = splineArray->getCPPos((cp->next())->next()); //get next next vertex
	else
		p2 = splineArray->getHookPos(((cp->next())->next())->hookIndex);
	
	outBias = cp->outBias;

	if ((outBias.alpha != 0) || (outBias.gamma != 0) || (outBias.magnitude != 0)) {
		if (cp->flags.kIsSmooth) {
			ot = calcSmoothDefaultEndTangent(p0, p1, p2, &gammaplane);
			calcHBias(ot,outBias,gammaplane);
		}
		else { // No bias for peaked end tangent
			ot = calcPeakedDefaultEndTangent(p0, p1, p2, NULL);
		}
	}
	else{ // no bias
		if (cp->flags.kIsSmooth) {
			ot = calcSmoothDefaultEndTangent(p0, p1, p2, NULL);
		}
		else {
			ot = calcPeakedDefaultEndTangent(p0, p1, p2, NULL);
		}
	}
	ot.negate();
	cp->ot = ot;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calculates tangent for tail cp.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void HSpline::calcTailTangent(HCP *cp) {
	HVector gammaplane;
	HBias inBias,outBias;
	HVector it;
	HVector p0,p1,p2;

	if (cp->flags.kIsLoop) {
		calcLoopingTangent(cp);
		return;
	}

	HHookArray *hooks = &this->splineArray->hookList;

	if (!cp->flags.kIsHook)
		p0 = splineArray->getCPPos(cp); //get last vertex
	else
		p0 = splineArray->getHookPos(cp->hookIndex);

	p1 = splineArray->getCPPos(cp->previous()); //get previous vertex

	if (!((cp->previous())->previous())->flags.kIsHook)
		p2 = splineArray->getCPPos((cp->previous())->previous()); //get previous previous vertex
	else
		p2 = splineArray->getHookPos(((cp->previous())->previous())->hookIndex);

	inBias = cp->inBias;

	if ((inBias.alpha != 0) || (inBias.gamma != 0) || (inBias.magnitude != 0)) {
		if (cp->flags.kIsSmooth) {
			it = calcSmoothDefaultEndTangent(p0, p1, p2, &gammaplane);
			calcHBias(it,inBias,gammaplane);
		}
		else { // No bias for peaked end tangent
			it = calcPeakedDefaultEndTangent(p0, p1, p2, NULL);
		}
	}
	else{ // no bias
		if (cp->flags.kIsSmooth) {
			it = calcSmoothDefaultEndTangent(p0, p1, p2, NULL);
		}
		else {
			it = calcPeakedDefaultEndTangent(p0, p1, p2, NULL);
		}
	}
	cp->it = it;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calculates tangents for a looping cp.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void HSpline::calcLoopingTangent(HCP *cp) {
	HVector gammaplane;
	HBias inBias,outBias;
	HVector p0,p1,p2;
	HVector it,ot;

	inBias = cp->inBias;
	outBias = (cp->head())->outBias;

	p0 = splineArray->getCPPos(cp->previous());
	p1 = splineArray->getCPPos(cp->head());
	p2 = splineArray->getCPPos((cp->head())->next());

	if (((inBias.alpha != 0) || (inBias.gamma != 0) || (inBias.magnitude != 0)) ||
		((outBias.alpha != 0) || (outBias.gamma != 0) || (outBias.magnitude != 0))) {
		if (cp->flags.kIsSmooth) {
			calcSmoothDefaultTangent(p0, p1, p2, it, ot, &gammaplane);
			calcHBias(it,inBias,gammaplane);
			calcHBias(ot,outBias,gammaplane);
		}
		else {//IN BIAS AND OUT BIAS IS FLIPPED
			calcPeakedDefaultTangent(p0, p1, p2, it, ot, &gammaplane);
			calcHBias(ot,inBias,gammaplane);
			calcHBias(it,outBias,gammaplane);
		}
	}
	else{ // no bias
		if (cp->flags.kIsSmooth) {
			calcSmoothDefaultTangent(p0, p1, p2, it, ot, NULL);
		}
		else {
			calcPeakedDefaultTangent(p0, p1, p2, it, ot, NULL);
		}
	}
	cp->it = it;
	(cp->head())->ot = ot;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	calculates tangents for a cp in the middle of a spline.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void HSpline::calcMiddleTangent(HCP *cp) {
	HVector gammaplane;
	HBias inBias,outBias;
	HVector p0,p1,p2;
	HVector it,ot;

	inBias = cp->inBias;
	outBias = cp->outBias;

	HHookArray *hooks = &this->splineArray->hookList;

	if (!(cp->previous())->flags.kIsHook)
		p0 = splineArray->getCPPos(cp->previous()); //get last vertex
	else
		p0 = splineArray->getHookPos((cp->previous())->hookIndex);
	
	p1 = splineArray->getCPPos(cp); //get previous vertex

	if (!(cp->next())->flags.kIsHook)
		p2 = splineArray->getCPPos(cp->next()); //get previous previous vertex
	else
		p2 = splineArray->getHookPos((cp->next())->hookIndex);

	if (((inBias.alpha != 0) || (inBias.gamma != 0) || (inBias.magnitude != 0)) ||
		((outBias.alpha != 0) || (outBias.gamma != 0) || (outBias.magnitude != 0))) {
		if (cp->flags.kIsSmooth) {
			calcSmoothDefaultTangent(p0, p1, p2, it, ot, &gammaplane);
			calcHBias(it,inBias,gammaplane);
			calcHBias(ot,outBias,gammaplane);
		}
		else {//IN BIAS AND OUT BIAS IS FLIPPED
			calcPeakedDefaultTangent(p0, p1, p2, it, ot, &gammaplane);
			calcHBias(it,inBias,gammaplane);
			calcHBias(ot,outBias,gammaplane);
		}
	}
	else{ // no bias
		if (cp->flags.kIsSmooth) {
			calcSmoothDefaultTangent(p0, p1, p2, it, ot, NULL);
		}
		else {
			calcPeakedDefaultTangent(p0, p1, p2, it, ot, NULL);
		}
	}
	cp->it = ot;
	cp->ot = it;
}

// only excepts realtive cp indices on a given spline
int HSpline::getDirection(unsigned cpi1, unsigned cpi2) {
	if (cpi2 > cpi1)
		return 1;
	return -1;
}

HCP *HSpline::get(unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range. [hspline]");
	}
	return &classList[index];
}

HCP *HSpline::head() {
	return &classList.front();
}

HCP *HSpline::tail() {
	return &classList.back();
}

HSpline *HSpline::next() {
	HSpline *spline = this;
	if (++spline > this->splineArray->end())
		return NULL;
	return spline;
}

HSpline *HSpline::previous() {
	HSpline *spline = this;
	if (--spline < this->splineArray->begin())
		return NULL;
	return spline;
}

HCP HSpline::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[hspline]");
	}
	return classList[index];
}

HCP &HSpline::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[hspline]");
	}
	return classList[index];
}

HSpline &HSpline::operator = ( const HSpline &other ){
	classList = other.classList;
	splineArray = other.splineArray;
	index = other.index;
	return *this;
}

std::ostream &operator << (std::ostream &os, HSpline &spline){
	for (HCP *iter = spline.head(); iter != NULL; iter = iter->next())
		os << *iter << std::endl;
	return os;
}
