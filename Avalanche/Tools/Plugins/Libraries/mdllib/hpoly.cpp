//////////////////////////////////////////////////////////////////////////////////////////
//
// HPOLY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "hpoly.h"
#include "hedge.h"
#include "hpolyarray.h"

HPoly::HPoly() {
	polyIndex = 0;
	polyArray = NULL;
	smoothGrpID = 0;
	dontuse = 0;
}

HPoly::HPoly(const HPoly &other){
	patchIndex = other.patchIndex;
	polyIndex = other.polyIndex;
	polyArray = other.polyArray;
	vertexIndicies = other.vertexIndicies;
	normalIndicies = other.normalIndicies;
	uvIndicies = other.uvIndicies;
	edgeIndicies = other.edgeIndicies;
	shaderIndicies = other.shaderIndicies;

	smoothGrpID = other.smoothGrpID;
	dontuse = other.dontuse;
}

HPoly::~HPoly(){}

void HPoly::clear() {
	patchIndex = 0;
	polyIndex = 0;
	vertexIndicies.clear();
	normalIndicies.clear();
	uvIndicies.clear();
	edgeIndicies.clear();
	shaderIndicies.clear();

	smoothGrpID = 0;
	dontuse = 0;
}

bool HPoly::hasVertices() {
	if (vertexIndicies.size() > 0)
		return true;
	return false;
}

bool HPoly::hasNormals() {
	if (normalIndicies.size() > 0)
		return true;
	return false;
}

bool HPoly::hasUVs() {
	if (uvIndicies.size() > 0)
		return true;
	return false;
}

HPoly *HPoly::next() {
	if (this == polyArray->end())
		return NULL;
	return this+1;
}

HPoly *HPoly::previous() {
	if (this == polyArray->begin())
		return NULL;
	return this-1;
}

HPoly *HPoly::begin() {
	return polyArray->begin();
}

HPoly *HPoly::end() {
	return polyArray->end();
}

HPoly &HPoly::operator = ( const HPoly &other ){
	patchIndex = other.patchIndex;
	polyIndex = other.polyIndex;
	polyArray = other.polyArray;
	vertexIndicies = other.vertexIndicies;
	normalIndicies = other.normalIndicies;
	uvIndicies = other.uvIndicies;
	edgeIndicies = other.edgeIndicies;
	shaderIndicies = other.shaderIndicies;

	smoothGrpID = other.smoothGrpID;
	dontuse = other.dontuse;

	return *this;
}

std::ostream &operator << (std::ostream &os, HPoly &poly){
//	if (poly.vertexIndicies.size() > 0) {
//		os << "Polygon( " << poly.polyIndex << " )" << "[ Patch Index " << poly.patchIndex << "]" << std::endl;
//		os << "  Vertex Indicies: " << poly.vertexIndicies << std::endl;
//		os << "  Normal Indicies: " << poly.normalIndicies << std::endl;
//		os << "  UV Indicies: " << poly.uvIndicies << std::endl;
//		for (unsigned i = 0; i < poly.shaderIndicies.length(); i++)
//			os << poly.polyArray->shaders[poly.shaderIndicies[i]] << std::endl;
//	}
//	else
//		os << "Polygon( " << poly.polyIndex << " ) Is Empty!" << std::endl;
	return os;
}