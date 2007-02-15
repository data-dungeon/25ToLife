#include "stdafx.h"
#include "harg.h"
#include "hmacros.h"
#include "hstatus.h"

HArg::HArg() {
	fvalue = 0.0;
	ivalue = 0;
	arg_string = "";
	bvalue = false;
}

HArg::HArg( UString &argstr ){
	ivalue = argstr.asInt();
	fvalue = argstr.asFloat();
	bvalue = argstr.asBool();
	arg_string = argstr;
}

HArg::~HArg() {}

void HArg::set( UString argstr ) {
	ivalue = argstr.asInt();
	fvalue = argstr.asFloat();
	bvalue = argstr.asBool();
	arg_string = argstr;
}

HArg &HArg::operator = ( const HArg &other ){
	fvalue = other.fvalue;
	ivalue = other.ivalue;
	bvalue = other.bvalue;
	arg_string = other.arg_string;
	return *this;
}

HArgArray::HArgArray() {}

HArgArray::~HArgArray() {}

void HArgArray::clear() {
	classList.clear();
}

unsigned HArgArray::length() const {
	return classList.size();
}

void HArgArray::append(HArg &arg) {
	classList.push_back(arg);
}

long HArgArray::getInt(unsigned index) {
	if (index >= classList.size()){
		THROW_FATAL_ERROR("Index out of range. [HArgarray]");
	}
	return classList[index].ivalue;
}

double HArgArray::getFloat(unsigned index) {
	if (index >= classList.size()){
		THROW_FATAL_ERROR("Index out of range. [HArgarray]");
	}
	return classList[index].fvalue;
}

bool HArgArray::getBool(unsigned index) {
	if (index >= classList.size()){
		THROW_FATAL_ERROR("Index out of range. [HArgarray]");
	}
	return classList[index].bvalue;
}

UString HArgArray::getString(unsigned index) {
	if (index >= classList.size()){
		THROW_FATAL_ERROR("Index out of range.[HArgarray");
	}
	return classList[index].arg_string;
}

UString HArgArray::getWholeString() {
	UString temp = classList[0].arg_string;
	for (unsigned i = 1; i < classList.size(); i++)
		temp = temp + " " + classList[i].arg_string;
	return temp;
}

const HArg &HArgArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range. [HArgarray]");
	}
	return classList[index];
}

HArgArray &HArgArray::operator = ( const HArgArray &other ){
	classList = other.classList;
	return *this;
}