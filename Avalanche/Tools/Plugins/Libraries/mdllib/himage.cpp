#include "stdafx.h"
#include "himage.h"
#include "hmacros.h"
#include "hstatus.h"

HImage::HImage() {
	backColor.r = 0.0;	backColor.g = 0.0;	backColor.b = 0.0;
}

HImage::HImage(const HImage &other) {
	name = other.name;
	pathname = other.pathname;
	backColor = other.backColor;
}

HImage::~HImage() {}

HImage &HImage::operator = ( const HImage &other ){
	name = other.name;
	pathname = other.pathname;
	backColor = other.backColor;
	return *this;
}

HImageArray::HImageArray() {}

HImageArray::~HImageArray() {}

void HImageArray::clear() {
	classList.clear();
}

unsigned HImageArray::length() const {
	return classList.size();
}

void HImageArray::append(HImage &texmap) {
	texmap.model = this->model;
	classList.push_back(texmap);
}

bool HImageArray::findImageIndexWithName(UString &imagename,unsigned &index) {
	HImageIterator iter(*this);
	for (;!iter.isDone(); iter++){
		HImage *image = *iter;
		if ((**iter).name == imagename) {
			index = iter.index();
			return true;
		}
	}
	return false;
}

HImage *HImageArray::get(unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.");
	}
	return &classList[index];
}

HImageArray &HImageArray::operator = ( const HImageArray &other ){
	classList = other.classList;
	return *this;
}

HImage HImageArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HImagearray]");
	}
	return classList[index];
}

HImage &HImageArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HImagearray]");
	}
	
	return classList[index];
}

/////////////////////////////////////////////////////////////////////////////////////////
// HImageIterator
/////////////////////////////////////////////////////////////////////////////////////////

HImageIterator::HImageIterator() {}

HImageIterator::HImageIterator(HImageArray &iarray) {
	pointTo = &iarray;
	cl_iterator = pointTo->classList.begin();
}

HImageIterator::~HImageIterator() {}

bool HImageIterator::isDone() {
	return (cl_iterator == pointTo->classList.end());
}

void HImageIterator::reset(HImageArray &iarray) {
	pointTo = &iarray;
	cl_iterator = pointTo->classList.begin();
}

void HImageIterator::reset() {
	cl_iterator = pointTo->classList.begin();
}

void HImageIterator::insert(HImage image){
	pointTo->classList.insert(cl_iterator,image);
}

unsigned HImageIterator::index() {
	return (cl_iterator-pointTo->classList.begin());
}

HImageIterator HImageIterator::operator ++(int) {
	cl_iterator++;
	return *this;
}

HImageIterator HImageIterator::operator --(int) {
	cl_iterator--;
	return *this;
}

HImage *HImageIterator::operator * (){
	return &(*cl_iterator);
}

