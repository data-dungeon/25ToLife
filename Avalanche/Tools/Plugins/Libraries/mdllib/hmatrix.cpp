//////////////////////////////////////////////////////////////////////////////////////////
//
// HMATRIX CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "hmatrix.h"
#include "hmacros.h"
#include "hstatus.h"

HMatrix::HMatrix(const unsigned cols,const unsigned rows,mType matrix_type) {
	type = matrix_type;
	maxcols = cols;
	maxrows = rows;
	if (matrix_type == INTEGER)
		ivalues.resize(cols * rows);
	else if (matrix_type == VECTOR)
		vvalues.resize(cols * rows);
}

HMatrix::HMatrix(const unsigned cols,const unsigned rows,mType matrix_type,const int value) {
	type = matrix_type;
	maxcols = cols;
	maxrows = rows;
	if (matrix_type == INTEGER)
		ivalues.resize(cols * rows);
	else if (matrix_type == VECTOR)
		vvalues.resize(cols * rows);
	fill(value);
}

HMatrix::HMatrix(const HMatrix &other){
	type = other.type;
	if (type == INTEGER)
		ivalues = other.ivalues;
	else if (type == VECTOR)
		vvalues = other.vvalues;
	maxcols = other.maxcols;
	maxrows = other.maxrows;
}

HMatrix::~HMatrix(){}

void HMatrix::clear() {
	if (type == INTEGER)
		ivalues.clear();
	else if (type == VECTOR)
		vvalues.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
//	INTEGER METHODS
//////////////////////////////////////////////////////////////////////////////////////////

void HMatrix::set(const unsigned index,const int value) {
	if (index >= ivalues.size()) {
		THROW_FATAL_ERROR("Index out of range.[HMATRIX]");
	}
	ivalues[index] = value;
}

int HMatrix::getInt(const unsigned index) const {
	if (index >= ivalues.size()) {
		THROW_FATAL_ERROR("Index out of range.[HMATRIX]");
	}
	return ivalues[index];	
}

void HMatrix::set(const unsigned col, const unsigned row,const int value) {
	set((row * maxcols) + col,value);
}

int HMatrix::getInt(const unsigned col, const unsigned row) const {
	return getInt((row * maxcols) + col);	
}

void HMatrix::fill(const int value) {
	for (unsigned i = 0; i < ivalues.size(); i++)
		ivalues[i] = value;
}

void HMatrix::fillColumn(const unsigned col,const std::vector<int> &list,bool reverse) {
	if (list.size() != maxcols) {
		THROW_FATAL_ERROR("Index out of range.[HMATRIX]");
	}
	int lastelement = 0;
	if (reverse)
		lastelement = list.size()-1;
	for (unsigned i = 0; i < maxrows; i++)
		set(col,i,list[abs((int)(lastelement-i))]);
}

void HMatrix::fillRow(const unsigned row,const std::vector<int> &list,bool reverse) {
	if (list.size() != maxcols) {
		THROW_FATAL_ERROR("Index out of range.[HMATRIX]");
	}
	int lastelement = 0;
	if (reverse)
		lastelement = list.size()-1;
	for (unsigned i = 0; i < maxcols; i++)
		set(i,row,list[abs((int)(lastelement-i))]);
}

void HMatrix::fillColumn(const unsigned col,const int value) {
	for (unsigned i = 0; i < maxrows; i++)
		set(col,i,value);
}

void HMatrix::fillRow(const unsigned row,const int value) {
	for (unsigned i = 0; i < maxcols; i++)
		set(i,row,value);
}

void HMatrix::getColumn(const unsigned col,std::vector<int> &list) {
	list.clear();
	for (unsigned i = 0; i < list.size(); i++)
		list.push_back(getInt(col,i));
}

void HMatrix::getRow(const unsigned row,std::vector<int> &list) {
	list.clear();
	for (unsigned i = 0; i < list.size(); i++)
		list.push_back(getInt(i,row));
}

//////////////////////////////////////////////////////////////////////////////////////////
//	VECTOR METHODS
//////////////////////////////////////////////////////////////////////////////////////////

void HMatrix::set(const unsigned index,const HVector pos) {
	if (index >= vvalues.length()) {
		THROW_FATAL_ERROR("Index out of range.[HMATRIX]");
	}
	vvalues[index] = pos;
}

HVector HMatrix::getVector(const unsigned index) const {
	if (index >= vvalues.length()) {
		THROW_FATAL_ERROR("Index out of range.[HMATRIX]");
	}
	return vvalues[index];	
}

void HMatrix::set(const unsigned col, const unsigned row,const HVector pos) {
	set((row * maxcols) + col,pos);
}

HVector HMatrix::getVector(const unsigned col, const unsigned row) const {
	return getVector((row * maxcols) + col);	
}

void HMatrix::fill(const HVector pos) {
	for (unsigned i = 0; i < ivalues.size(); i++)
		vvalues[i] = pos;
}

void HMatrix::fillColumn(const unsigned col,const HVectorArray &list,bool reverse) {
	if (list.length() != maxcols) {
		THROW_FATAL_ERROR("Index out of range.[HMATRIX]");
	}
	int lastelement = 0;
	if (reverse)
		lastelement = list.length()-1;
	for (unsigned i = 0; i < maxrows; i++)
		set(col,i,list[abs((int)(lastelement-i))]);
}

void HMatrix::fillRow(const unsigned row,const HVectorArray &list,bool reverse) {
	if (list.length() != maxcols) {
		THROW_FATAL_ERROR("Index out of range.[HMATRIX]");
	}
	int lastelement = 0;
	if (reverse)
		lastelement = list.length()-1;
	for (unsigned i = 0; i < maxcols; i++)
		set(i,row,list[abs((int)(lastelement-i))]);
}

void HMatrix::fillColumn(const unsigned col,const HVector pos) {
	for (unsigned i = 0; i < maxrows; i++)
		set(col,i,pos);
}

void HMatrix::fillRow(const unsigned row,const HVector pos) {
	for (unsigned i = 0; i < maxcols; i++)
		set(i,row,pos);
}

void HMatrix::getColumn(const unsigned col,HVectorArray &list) {
	list.clear();
	for (unsigned i = 0; i < list.length(); i++)
		list.append(getVector(col,i));
}

void HMatrix::getRow(const unsigned row,HVectorArray &list) {
	list.clear();
	for (unsigned i = 0; i < list.length(); i++)
		list.append(getVector(i,row));
}

HMatrix &HMatrix::operator = ( const HMatrix &other ){
	type = other.type;
	if (type == INTEGER)
		ivalues = other.ivalues;
	else if (type == VECTOR)
		vvalues = other.vvalues;
	maxcols = other.maxcols;
	maxrows = other.maxrows;
	return *this;
}

std::ostream &operator << (std::ostream &os, const HMatrix &other){
	for (unsigned row = 0; row < other.maxrows; row++) {
		for (unsigned col = 0; col < other.maxcols; col++) {
			if (other.type == HMatrix::INTEGER)
				os << other.getInt(col,row) << " ";
			else if (other.type == HMatrix::VECTOR)
				os << other.getVector(col,row) << " ";
		}
		os << std::endl;
	}
	return os;
}