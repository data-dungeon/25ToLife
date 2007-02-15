//////////////////////////////////////////////////////////////////////////////////////////
//
// BIAS CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hbias.h"

HBias::HBias(){
	alpha = 0.0;
	gamma = 0.0;
	magnitude = 0.0;
}

HBias::HBias(const HBias &other) {
	alpha = other.alpha;
	gamma = other.gamma;
	magnitude = other.magnitude;
}

HBias::~HBias(){}

void HBias::clear() {
	alpha = 0.0;
	gamma = 0.0;
	magnitude = 0.0;
}

HBias &HBias::operator = ( const HBias &other ){
	alpha = other.alpha;
	gamma = other.gamma;
	magnitude = other.magnitude;
	return *this;
}