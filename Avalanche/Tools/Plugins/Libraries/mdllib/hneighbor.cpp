//////////////////////////////////////////////////////////////////////////////////////////
//
//	HNEIGHBOR CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include "hneighbor.h"
#include "hpoly.h"
#include "mdlobject.h"

HNeighbor::HNeighbor(){
	memset(&flags,0,sizeof(HNeighbor::vflags));
	vid = 0;
	type = PATCH;
}

HNeighbor::HNeighbor(const HNeighbor &other) {
	memcpy(&this->flags,&other.flags,sizeof(HNeighbor::vflags));
	type = other.type;
	cpids = other.cpids;
	vid = other.vid;
	normals = other.normals;
	uvs = other.uvs;
	nsf = other.nsf;
	nsv = other.nsv;
	nse = other.nse;
	nsfnv = other.nsfnv;
	nsp = other.nsp;
}

HNeighbor::~HNeighbor(){}

void HNeighbor::clear() {
	memset(&flags,0,sizeof(HNeighbor::vflags));
	type = PATCH;
	cpids.clear();
	vid = 0;
	nsv.clear();
	nsf.clear();
	nse.clear();
	nsfnv.clear();
	nsp.clear();
}

int HNeighbor::getNormalIndex(unsigned faceid) {
	for (unsigned i = 0; i < this->nsf.length(); i++)
	{
		if (faceid == this->nsf[i])
		{
			if (this->normals.size() > 0)
			{
				return this->normals[i];

			}
		}
	}
	return -1;
}

int HNeighbor::getUVIndex(unsigned faceid) {
	for (unsigned i = 0; i < this->nsf.length(); i++)
	{
		if (faceid == this->nsf[i])
		{
			if (this->uvs.size() > 0)
			{
				return this->uvs[i];
			}
		}
	}
	return -1;
}

void HNeighbor::getIntOnBothSides(int index,int &indexL,int &indexR,int numIndicies,bool edges)
{
	if (!edges)
	{
		if ((index-1)<0)
			indexL = numIndicies-1;
		else
			indexL = index-1;
	}
	else
	{
		if ((index-1)<0)
			indexL = numIndicies-1;
		else
			indexL = index-1;
	}

	if (!edges)
	{
		if ((index+1)>=numIndicies)
			indexR = 0;
		else
			indexR = index+1;
	}
	else
	{
		indexR = index;
	}
}

void HNeighbor::calcNeighbor(unsigned idx,HPoly &poly)
{
	int numVerts = poly.vertexIndicies.size();

	int indexL,indexR;

	this->vid = poly.vertexIndicies[idx];

	this->type = HNeighbor::POLYGON;

	getIntOnBothSides(idx,indexL,indexR,numVerts);

	this->nsv.push_back(poly.vertexIndicies[indexL]);
	this->nsv.push_back(poly.vertexIndicies[indexR]);
			
	getIntOnBothSides(idx,indexL,indexR,numVerts,true);

	this->nse.push_back(poly.edgeIndicies[indexL]);
	this->nse.push_back(poly.edgeIndicies[indexR]);
			
	if (poly.uvIndicies.size() > 0)
		this->uvs.push_back(poly.uvIndicies[idx]);

	if (poly.normalIndicies.size() > 0)
		this->normals.push_back(poly.normalIndicies[idx]);

	this->nsf.append(poly.polyIndex);
	this->nsfnv.push_back(numVerts);
}

void HNeighbor::calcNeighbor(unsigned idx,HPatch &patch)
{
	int numVerts = patch.cpvIndicies.size();

	this->vid = patch.cpvIndicies[idx];

	this->type = HNeighbor::PATCH;

	int indexL,indexR;
	getIntOnBothSides(idx,indexL,indexR,numVerts);

	this->nsv.push_back(patch.cpvIndicies[indexL]);
	this->nsv.push_back(patch.cpvIndicies[indexR]);
			
	this->nsp.append(patch.patchIndex);
}

HNeighbor &HNeighbor::operator = ( const HNeighbor &other ) {
	memcpy(&this->flags,&other.flags,sizeof(HNeighbor::vflags));
	type = other.type;
	cpids = other.cpids;
	vid = other.vid;
	normals = other.normals;
	uvs = other.uvs;
	nsf = other.nsf;
	nsv = other.nsv;
	nse = other.nse;
	nsfnv = other.nsfnv;
	nsp = other.nsp;
	return *this;
}

std::ostream &operator << (std::ostream &os, HNeighbor &n){
//	if (n.type == HNeighbor::PATCH) {
//		os << "VID ( " << n.vid << " ) ";
//		os << "CPIDS ( " << n.cpids << ") ";
//		os << "NSP: " << n.nsp << " ";
//		os << "NSV: " << n.nsv << " ";
//	}
//	else {
//		os << "NSV: " << n.nsv << " ";
//		os << "NSF: " << n.nsf << " ";
//		os << "NSE: " << n.nse << " ";
//		os << "NSFNV: " << n.nsfnv << " ";
//	}
	return os;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//	HNEIGHBORARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

HNeighborArray::HNeighborArray() {}

HNeighborArray::HNeighborArray(const unsigned size) {
	classList.resize(size);
}

HNeighborArray::~HNeighborArray() {}

void HNeighborArray::clear() {
	classList.clear();
}

unsigned HNeighborArray::length() const {
	return classList.size();
}

unsigned HNeighborArray::append(HNeighbor &n) {
	n.neighbors = this;
	classList.push_back(n);
	return classList.size()-1;
}

void HNeighborArray::resize(const unsigned size) {
	classList.resize(size);
}


HVector HNeighborArray::calcNormal(unsigned vidx) {
	HVector n;
	HVectorArray *vertices;
	if (this->begin()->type == HNeighbor::POLYGON) {
		vertices = &model->polygonArray->vertices;
	}
	else {
		vertices = &model->splineArray.vertices;
	}
	HVector a,b;
	unsigned neighborlength = (*this)[vidx].nsv.size();

	for (unsigned i = 0; i < neighborlength; i+=2) {
		a = *vertices->get((*this)[vidx].nsv[i]) - *vertices->get(vidx);
		if ( (i+1) < neighborlength ) {
			b = *vertices->get((*this)[vidx].nsv[i+1]) - *vertices->get(vidx);
			a.normalize();
			b.normalize();
			n += b^a;
		}
	}
	n.normalize();
	return n;
}

HNeighbor *HNeighborArray::get(unsigned index){
	return &classList[index];
}

HNeighbor *HNeighborArray::begin() {
	return &classList.front();
}

HNeighbor *HNeighborArray::end() {
	return &classList.back();
}

HNeighbor HNeighborArray::operator[](unsigned index) const{
	return classList[index];
}

HNeighbor &HNeighborArray::operator[](unsigned index){
	return classList[index];
}

HNeighborArray &HNeighborArray::operator = ( const HNeighborArray &other ){
	classList = other.classList;
	return *this;
}

std::ostream &operator << (std::ostream &os, HNeighborArray &other){
	for (unsigned i = 0; i < other.length(); i++)
		std::cerr << other[i] << std::endl;
	return os;
}