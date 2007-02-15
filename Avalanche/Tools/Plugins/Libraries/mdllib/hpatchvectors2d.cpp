#include "Stdafx.h"
#include "hpatchvectors2d.h"

/////////////////////////////////////////////////////////////////////////////////////////
HPatchVectors2D::HPatchVectors2D(){}

HPatchVectors2D::HPatchVectors2D(const HPatchVectors2D &other) {
	pv = other.pv;
}

HPatchVectors2D::~HPatchVectors2D(){}


void HPatchVectors2D::rotateElementsClockwise90() {
	HVector2DArray n = this->pv;

	for (unsigned y = 0; y < 4; y++)
		for (unsigned x = 0; x < 4; x++)
				this->pv[(x*4)+y] = n[(y*4)+x];
}

void HPatchVectors2D::mirrorElementsX() {
	HVector2DArray n = this->pv;

	for (unsigned y = 0; y < 4; y++)
		for (unsigned x = 0; x < 4; x++)
				this->pv[(y*4)+(3-x)] = n[(y*4)+x];
}

void HPatchVectors2D::mirrorElementsY() {
	HVector2DArray n = this->pv;

	for (unsigned y = 0; y < 4; y++)
		for (unsigned x = 0; x < 4; x++)
				this->pv[((3-y)*4)+x] = n[(y*4)+x];
}

void HPatchVectors2D::rotateElementsClockwise(unsigned rot) {

	mirrorElementsX();

	if (rot == 1) {
		rotateElementsClockwise90();
	}
	else if (rot == 2) {
		mirrorElementsY();
	}
	if (rot == 3) {
		rotateElementsClockwise90();
		mirrorElementsX();	
	}
}

HPatchVectors2D &HPatchVectors2D::operator = ( const HPatchVectors2D &other ){
	pv = other.pv;
	return *this;
}

HPatchVectors2DArray::HPatchVectors2DArray() {}

HPatchVectors2DArray::~HPatchVectors2DArray() {}

void HPatchVectors2DArray::clear() {
	classList.clear();
}

unsigned HPatchVectors2DArray::length() const {
	return classList.size();
}

void HPatchVectors2DArray::append(HPatchVectors2D &pVectors) {
	classList.push_back(pVectors);
}

HPatchVectors2D HPatchVectors2DArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HPatchVectors2DArray]");
	}
	return classList[index];
}

HPatchVectors2D &HPatchVectors2DArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HPatchVectors2DArray]");
	}
	return classList[index];
}

HPatchVectors2DArray &HPatchVectors2DArray::operator = ( const HPatchVectors2DArray &other ){
	classList = other.classList;
	return *this;
}

std::ostream &operator << (std::ostream &os,HPatchVectors2D &v){
	os << v.pv;
	return os;
}

std::ostream &operator << (std::ostream &os,HPatchVectors2DArray &varray){
	os << "Patch vector Array length = " << varray.length() << std::endl;
	for (unsigned i = 0;i < varray.length();i++)
		os << varray[i];
	return os;
}