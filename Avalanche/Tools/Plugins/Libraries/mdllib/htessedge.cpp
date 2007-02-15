//////////////////////////////////////////////////////////////////////////////////////////
//
// HTESSEDGE CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "htessedge.h"
#include "mdlobject.h"

HTessEdge::HTessEdge() {
	memset(&flags,0,sizeof(unsigned long));
}

HTessEdge::HTessEdge(const HTessEdge &other){
	memcpy(&this->flags,&other.flags,sizeof(unsigned long));
	vertexIndicies = other.vertexIndicies;
	normalIndicies = other.normalIndicies;
	adj_patchIndicies = other.adj_patchIndicies;
}

HTessEdge::~HTessEdge(){}

void HTessEdge::clear() {
	memset(&flags,0,sizeof(unsigned long));
	vertexIndicies.clear();
	normalIndicies.clear();
}

void HTessEdge::setEdgeVertices(bool reverse,std::vector<int> &edgeIndicies,mdlObject *mdl){
	
	HEdge *edge;
	if (!reverse) {
		edge = mdl->polygonArray->edges.get(edgeIndicies.front());
		vertexIndicies.push_back(edge->vertexIndex0);
		normalIndicies.push_back(edge->normalIndex0);
		unsigned len = edgeIndicies.size()-1;
		for (unsigned i = 0; i < len; i++) {
			edge = mdl->polygonArray->edges.get(edgeIndicies[i]);
			vertexIndicies.push_back(edge->vertexIndex1);
			normalIndicies.push_back(edge->normalIndex1);
		}
		edge = mdl->polygonArray->edges.get(edgeIndicies.back());
		vertexIndicies.push_back(edge->vertexIndex1);
		normalIndicies.push_back(edge->normalIndex1);
	}
	else {
		edge = mdl->polygonArray->edges.get(edgeIndicies.back());
		vertexIndicies.push_back(edge->vertexIndex0);
		normalIndicies.push_back(edge->normalIndex0);
		unsigned len = edgeIndicies.size()-1;
		for (unsigned i = 0; i < len; i++) {
			edge = mdl->polygonArray->edges.get(edgeIndicies[len-i]);
			vertexIndicies.push_back(edge->vertexIndex1);
			normalIndicies.push_back(edge->normalIndex1);
		}
		edge = mdl->polygonArray->edges.get(edgeIndicies.front());
		vertexIndicies.push_back(edge->vertexIndex1);
		normalIndicies.push_back(edge->normalIndex1);
	}
	this->flags.kHasEdgeVertices = 1;
}

HTessEdge &HTessEdge::operator = ( const HTessEdge &other ){
	memcpy(&this->flags,&other.flags,sizeof(unsigned long));
	vertexIndicies = other.vertexIndicies;
	normalIndicies = other.normalIndicies;
	adj_patchIndicies = other.adj_patchIndicies;
	return *this;
}

std::ostream &operator << (std::ostream &os,const HTessEdge &other){
	if (other.flags.kIsBorder) {
			os << "[ kIsBorder ]";
//			os << other.vertexIndicies;
			return os;
	}

	if (other.flags.kIsHookShortSegment)
		os << "[ kIsHookShortSegment ]";
	else if (other.flags.kIsHookLongSegment)
		os << "[ kIsHookLongSegment ]";

//	os << "[ Adj: " << other.adj_patchIndicies << " ]";
//	os << other.vertexIndicies;

	return os;
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// HTESSEDGEARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

HTessEdgeArray::HTessEdgeArray() {}

HTessEdgeArray::HTessEdgeArray(const unsigned size) {
	classList.resize(size);
}

HTessEdgeArray::HTessEdgeArray(const HTessEdgeArray &other) {
	classList = other.classList;
}

HTessEdgeArray::~HTessEdgeArray() {}

void HTessEdgeArray::clear() {
	classList.clear();
}

unsigned HTessEdgeArray::length() const {
	return classList.size();
}

void HTessEdgeArray::resize(const unsigned s) {
	classList.resize(s);
}

unsigned HTessEdgeArray::append(const HTessEdge &other) {
	classList.push_back(other);
	return classList.size()-1;
}

HTessEdge *HTessEdgeArray::get(unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range. [HTessCorner]");
	}
	return &classList[index];
}

HTessEdge *HTessEdgeArray::begin() {
	return &classList.front();
}

HTessEdge *HTessEdgeArray::end() {
	return &classList.back();
}

HTessEdge HTessEdgeArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HTessEdgeArray]");
	}
	return classList[index];
}

HTessEdge &HTessEdgeArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HTessEdgeArray]");
	}
	
	return classList[index];
}

HTessEdgeArray &HTessEdgeArray::operator = ( const HTessEdgeArray &other ){
	classList = other.classList;
	return *this;
}
