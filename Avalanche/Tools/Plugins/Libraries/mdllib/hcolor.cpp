#include "stdafx.h"
#include "hcolor.h"
////////////////////////////////////////////////////////////////////////////////////////
// value ranges from 0-1
////////////////////////////////////////////////////////////////////////////////////////

HColor::HColor(){
	r = 0; g = 0; b = 0; a = 0;	
}

HColor::HColor( double red, double green, double blue ) {
	r = red;	g = green;	b = blue; a = 1;
}

HColor::HColor( double red, double green, double blue, double alpha ) {
	r = red;	g = green;	b = blue; a = alpha;
}

HColor::HColor(double value) {
	r = value;	g = value;	b = value; a = 1;
}

HColor::~HColor(){}

HColor &HColor::operator = ( double value ){
	r = value;
	g = value;
	b = value;
	a = 1;
	return *this;
}

HColor &HColor::operator = ( const HColor &other ){
	r = other.r;
	g = other.g;
	b = other.b;
	a = other.a;
	return *this;
}

bool HColor::operator == ( const HColor &other ) const {
	if ( r == other.r )
		if ( g == other.g )
			if ( b == other.b )
				return true;

	return false;
}

bool HColor::operator != ( const HColor &other ) const {
	return !(*this == other);
}

std::ostream &operator << (std::ostream &os, HColor &other){
	os << "[ " << other.r << "," << other.g << "," << other.b << " ]";
	return os;
}