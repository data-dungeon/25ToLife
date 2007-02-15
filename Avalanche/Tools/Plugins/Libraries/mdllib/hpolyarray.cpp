//////////////////////////////////////////////////////////////////////////////////////////
//
// HPOLYARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "hpolyarray.h"
#include "hmacros.h"
#include "hstatus.h"
#include "mdlobject.h"

HPolyArray::HPolyArray() {
	neighbors.model = NULL;
	tjuncts = NULL;
}

HPolyArray::HPolyArray(const HPolyArray &other) {
	vertices = other.vertices;
	normals = other.normals;
	uvs = other.uvs;
	edges = other.edges;
	neighbors = other.neighbors;
	shaders = other.shaders;
	maps = other.maps;
	model = other.model;
	classList = other.classList;
}

HPolyArray::~HPolyArray() {}

//////////////////////////////////////////////////////////////////////////////////////////
//	Basic functions
//////////////////////////////////////////////////////////////////////////////////////////

void HPolyArray::clear() {
	vertices.clear();
	normals.clear();
	uvs.clear();
	edges.clear();
	neighbors.clear();
	shaders.clear();
	maps.clear();
	model = NULL;
	tjuncts = NULL;
	classList.clear();
}

unsigned HPolyArray::length() const {
	return classList.size();
}

//////////////////////////////////////////////////////////////////////////////////////////
//	Functions for adding polygons
//////////////////////////////////////////////////////////////////////////////////////////

void HPolyArray::getIntOnBothSides(int index,int &indexL,int &indexR,int numIndicies,bool edges)
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

unsigned HPolyArray::append(HPoly &poly,bool addneighborinfo) {
	// SETS NEIGHBORS
	////////////////////////////////////////////////////////////////////////////////
	poly.polyIndex = classList.size();
	poly.polyArray = this;

	if (addneighborinfo)
	{
		int numVerts = poly.vertexIndicies.size();
		HNeighbor *neighbor;
		for (unsigned i = 0; i < (unsigned)numVerts; i++)
		{
			neighbor = &this->neighbors[poly.vertexIndicies[i]];
			neighbor->calcNeighbor(i,poly);
		}
	}

	classList.push_back(poly);
	return classList.size()-1;
}

void HPolyArray::remove(unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HPolyArray]");
	}
//	deletePolygonNeighborInfo(index);
	classList.erase(classList.begin() + index);
}

void HPolyArray::triangulate() {

	HPolyArray *other = new HPolyArray();
	other->model = this->model;

	other->vertices = this->vertices;
	other->edges = this->edges;
	other->normals = this->normals;
	other->uvs = this->uvs;

	HPoly *poly;
	for (unsigned i = 0; i < this->length(); i++) {
		poly = this->get(i);
		if (poly->vertexIndicies.size() > 0) {
			if (poly->vertexIndicies.size() > 3) {
				HPoly one,two;

				one.vertexIndicies.push_back(poly->vertexIndicies[0]);
				one.vertexIndicies.push_back(poly->vertexIndicies[2]);
				one.vertexIndicies.push_back(poly->vertexIndicies[3]);

				one.normalIndicies.push_back(poly->normalIndicies[0]);
				one.normalIndicies.push_back(poly->normalIndicies[2]);
				one.normalIndicies.push_back(poly->normalIndicies[3]);

				one.uvIndicies.push_back(poly->uvIndicies[0]);
				one.uvIndicies.push_back(poly->uvIndicies[2]);
				one.uvIndicies.push_back(poly->uvIndicies[3]);
			
				one.edgeIndicies.push_back(poly->edgeIndicies[2]);
				one.edgeIndicies.push_back(poly->edgeIndicies[3]);
				one.edgeIndicies.push_back(this->edges.append(HEdge()));

				two.vertexIndicies.push_back(poly->vertexIndicies[0]);
				two.vertexIndicies.push_back(poly->vertexIndicies[1]);
				two.vertexIndicies.push_back(poly->vertexIndicies[2]);

				two.normalIndicies.push_back(poly->normalIndicies[0]);
				two.normalIndicies.push_back(poly->normalIndicies[1]);
				two.normalIndicies.push_back(poly->normalIndicies[2]);

				two.uvIndicies.push_back(poly->uvIndicies[0]);
				two.uvIndicies.push_back(poly->uvIndicies[1]);
				two.uvIndicies.push_back(poly->uvIndicies[2]);

				two.edgeIndicies.push_back(poly->edgeIndicies[0]);
				two.edgeIndicies.push_back(poly->edgeIndicies[1]);
				two.edgeIndicies.push_back(this->edges.append(HEdge()));

				one.shaderIndicies = poly->shaderIndicies;
				two.shaderIndicies = poly->shaderIndicies;

				other->append(one,false);
				other->append(two,false);
			}
			else {
				HPoly one;

				one.vertexIndicies.push_back(poly->vertexIndicies[0]);
				one.vertexIndicies.push_back(poly->vertexIndicies[1]);
				one.vertexIndicies.push_back(poly->vertexIndicies[2]);

				one.normalIndicies.push_back(poly->normalIndicies[0]);
				one.normalIndicies.push_back(poly->normalIndicies[1]);
				one.normalIndicies.push_back(poly->normalIndicies[2]);

				one.uvIndicies.push_back(poly->uvIndicies[0]);
				one.uvIndicies.push_back(poly->uvIndicies[1]);
				one.uvIndicies.push_back(poly->uvIndicies[2]);

				one.edgeIndicies.push_back(poly->edgeIndicies[0]);
				one.edgeIndicies.push_back(poly->edgeIndicies[1]);
				one.edgeIndicies.push_back(poly->edgeIndicies[2]);

				one.shaderIndicies = poly->shaderIndicies;

				other->append(one,false);
			}
		}
	}
	other->shaders = this->shaders;
	other->maps = this->maps;

	mdlObject *temp = this->model;
	delete this;
	temp->polygonArray = other;
}

//////////////////////////////////////////////////////////////////////////////////////////
//	Functions for fixing T-Junctions
//////////////////////////////////////////////////////////////////////////////////////////

void HPolyArray::splitPoly(HTJunction &tjunct) {
	unsigned i;
	HPoly *poly,newpoly;
	HPolyArray newpolys;
	unsigned faceIdx,edgestart=9999;
	std::vector<int> common_faces;
	bool successful;

//	Get polygon for splitting
	this->neighbors[tjunct.long_edge.vertexIndex0].nsf.commonElements(this->neighbors[tjunct.long_edge.vertexIndex1].nsf,common_faces);
	if (common_faces.size() == 0) {
		std::cerr << "Can't find common face between vertices." << std::endl;
		return;
	}
	faceIdx = common_faces[0];
	poly = this->get(faceIdx);

//	Find the vertex index where the long edge starts
	successful = findEdgeOnPoly(tjunct.long_edge,*poly,edgestart);
	if (!successful) {
		THROW_FATAL_ERROR("Couldn't split poly to fix t-junction.\n");
	}

	newpolys.clear();
//	Loop through each polygon in split [quad is i == 0]
	for (i = 0; i < tjunct.short_edges.length(); i++)
	{
		deriveSplitPoly(i,edgestart,tjunct,poly,newpoly);
		newpolys.append(newpoly,false);
	}

//	deriveNPoly(edgestart,tjunct,poly,newpoly);
//	newpolys.append(newpoly,false);

	std::vector<int> vtxIndicies(poly->vertexIndicies);

	poly->vertexIndicies.clear();
	poly->normalIndicies.clear();
	poly->uvIndicies.clear();

	evaluatePolygonNeighbors(vtxIndicies);
	
	// add split faces
	for (i = 0; i < newpolys.length(); i++)
		this->append(newpolys[i],true);
}

void HPolyArray::evaluatePolygonNeighbors(std::vector<int> &vtxIndicies) {
	for (unsigned i = 0; i < vtxIndicies.size(); i++)
		evaluateVertexNeighbors(vtxIndicies[i]);
}

void HPolyArray::evaluateVertexNeighbors(unsigned vertex_index) {
	HNeighbor *n;
	std::vector<int> nsfIndicies;

	n = this->neighbors.get(vertex_index);

	bool belong = false;
	for (unsigned i = 0; i < n->nsf.length(); i++) {
		belong = doesFaceBelongToVertex(n->nsf[i],vertex_index);
		if (!belong)
			nsfIndicies.push_back(i);
	}

	for (i = 0; i < nsfIndicies.size();i++)
		deleteNeighborFace(n,nsfIndicies[i]);
}

bool HPolyArray::doesFaceBelongToVertex(unsigned poly_index,unsigned vertex_index) {
	HPoly *poly;

	poly = this->get(poly_index);
	for (unsigned i = 0; i < poly->vertexIndicies.size(); i++)
		if (poly->vertexIndicies[i] == vertex_index)
			return true;
	return false;
}

void HPolyArray::deleteNeighborFace(HNeighbor *n,unsigned nsf_index) {
	n->nsv.erase(n->nsv.begin() + (nsf_index * 2));
	n->nsv.erase(n->nsv.begin() + (nsf_index * 2));
	
	n->nse.erase(n->nse.begin() + (nsf_index * 2));
	n->nse.erase(n->nse.begin() + (nsf_index * 2));

	if (normals.length() > 0)
		n->normals.erase(n->normals.begin() + nsf_index);

	if (uvs.length() > 0)
		n->uvs.erase(n->uvs.begin() + nsf_index);

	n->nsf.remove(nsf_index);
	n->nsfnv.erase(n->nsfnv.begin() + nsf_index);
}

bool HPolyArray::findEdgeOnPoly(HEdge edge,HPoly &poly,unsigned &startidx) {
	HEdge *deltaedge;
	for (unsigned i = 0;i < poly.edgeIndicies.size(); i++) {
		deltaedge = this->edges.get(poly.edgeIndicies[i]);
		if (edge == *deltaedge) {
			startidx = i;
			return true;
		}
	}
	return false;
}

bool HPolyArray::getFacesFromEdge(HEdge *edge,std::vector<int> &faceindicies)
{
	this->neighbors[edge->vertexIndex0].nsf.commonElements(this->neighbors[edge->vertexIndex1].nsf,faceindicies);
	if (faceindicies.size() == 0)
		return false;
	return true;
}

void HPolyArray::deriveNPoly(unsigned edgestart,HTJunction &tjunct,HPoly *poly,HPoly &newpoly)
{
	std::vector<int> faceindicies;

	newpoly.clear();

	getFacesFromEdge(&tjunct.long_edge,faceindicies);
	int faceidx = faceindicies[0];
	
	int numshortedges = tjunct.short_edges.length();

	HEdge *edge = &tjunct.short_edges[numshortedges-1];
	newpoly.vertexIndicies.push_back(edge->vertexIndex1);
	
	getFacesFromEdge(edge,faceindicies);
	faceidx = faceindicies[0];	
	int uvidx = this->neighbors[edge->vertexIndex1].getUVIndex(faceidx);
	if (uvidx>=0)
		newpoly.uvIndicies.push_back(uvidx);

	int nidx = this->neighbors[edge->vertexIndex1].getNormalIndex(faceidx);
	if (nidx>=0)
		newpoly.normalIndicies.push_back(nidx);

	for (unsigned i = 0; i < (unsigned)numshortedges; i++)
	{
		HEdge *edge = &tjunct.short_edges[numshortedges-i-1];
		newpoly.vertexIndicies.push_back(edge->vertexIndex0);

		getFacesFromEdge(edge,faceindicies);
		
		int faceidx = faceindicies[0];

		int uvidx = this->neighbors[edge->vertexIndex0].getUVIndex(faceidx);
		if (uvidx>=0)
			newpoly.uvIndicies.push_back(uvidx);

		int nidx = this->neighbors[edge->vertexIndex0].getNormalIndex(faceidx);
		if (nidx>=0)
			newpoly.normalIndicies.push_back(nidx);
	}

	int idx = edgestart+1;

	int numVerts = poly->vertexIndicies.size();

	for (i = 0; i < (unsigned)(numVerts-2); i++)
	{
		if (++idx >= numVerts)
		{
			idx = idx % numVerts;
		}

		int debug = poly->vertexIndicies[idx];
		
		newpoly.vertexIndicies.push_back(poly->vertexIndicies[idx]);
		if (newpoly.normalIndicies.size())
			newpoly.normalIndicies.push_back(poly->normalIndicies[idx]);
		if (newpoly.uvIndicies.size())
			newpoly.uvIndicies.push_back(poly->uvIndicies[idx]);
	}

	for (i = 0; i < newpoly.vertexIndicies.size()-1; i++)
	{
		HEdge newedge(newpoly.vertexIndicies[i],newpoly.vertexIndicies[i+1]);
		newpoly.edgeIndicies.push_back(this->edges.append(newedge));
	}

	HEdge newedge(newpoly.vertexIndicies[newpoly.vertexIndicies.size()-1],newpoly.vertexIndicies[0]);
	newpoly.edgeIndicies.push_back(this->edges.append(newedge));

	newpoly.patchIndex = poly->patchIndex;
	newpoly.polyIndex = poly->polyIndex;
	newpoly.shaderIndicies = poly->shaderIndicies;
}

void HPolyArray::deriveSplitPoly(unsigned idx, unsigned edgestart, HTJunction &tjunct, HPoly *poly, HPoly &newpoly)
{				
	unsigned order4pt[] = {0,1,2,3,0,1,2,3,0,1,2,3,0};
	unsigned order3pt[] = {0,1,2,0,1,2,0,1,2,0};
	unsigned *order;
	unsigned vidx[4],nidx[4],uvidx[4];
	unsigned faceIdx;
	std::vector<int> common_faces;
	std::vector<int> vidxs,nidxs,uvidxs;
	bool skipnormals = false, skipuvs = false;
	bool isOriginalPoly4point = false;
	bool isFirstPoly = (idx == 0);
	bool isLastPoly = (idx == (tjunct.short_edges.length()-1));

	newpoly.clear();

	if (poly->vertexIndicies.size() > 3)
		order = order4pt;
	else
		order = order3pt;

	if (poly->vertexIndicies.size()>3)
		isOriginalPoly4point = true;

//	Short edge vertex indicies
	vidx[0] = tjunct.short_edges[idx].vertexIndex0;
	vidx[1] = tjunct.short_edges[idx].vertexIndex1;
	vidx[2] = poly->vertexIndicies[order[edgestart+2]];
	vidx[3] = poly->vertexIndicies[order[edgestart+3]];

//	Get face index for short edge
	this->neighbors[vidx[0]].nsf.commonElements(this->neighbors[vidx[1]].nsf,common_faces);
	if (common_faces.size() == 0) {
		std::cerr << "Can't find common face between vertices." << std::endl;
		return;
	}
	faceIdx = common_faces[0];

	skipnormals = poly->normalIndicies.size()==0;
	skipuvs = poly->uvIndicies.size()==0;

//	Get normals and uv's for short edge
	if (!skipnormals) {
		int n0 = this->neighbors[vidx[0]].getNormalIndex(faceIdx);
		if (n0<0) {
			THROW_FATAL_ERROR("Couldn't get normal index.[deriveSplitPoly]");
		}
		int n1 = this->neighbors[vidx[1]].getNormalIndex(faceIdx);
		if (n1<0) {
			THROW_FATAL_ERROR("Couldn't get normal index.[deriveSplitPoly]");
		}
		nidx[0] = n0;
		nidx[1] = n1;
		nidx[2] = poly->normalIndicies[order[edgestart+2]];
		nidx[3] = poly->normalIndicies[order[edgestart+3]];
	}

	if (!skipuvs) {
		HVector pos0 = this->vertices[poly->vertexIndicies[order[edgestart]]];
		HVector	pos1 = this->vertices[poly->vertexIndicies[order[edgestart+1]]];

		HVector tj0 = this->vertices[tjunct.short_edges[idx].vertexIndex0];
		HVector tj1 = this->vertices[tjunct.short_edges[idx].vertexIndex1];

		double r0 = ((tj0 - pos0).length())/((pos1 - pos0).length());
		double r1 = ((tj1 - pos0).length())/((pos1 - pos0).length());

		HVector2D uv0 = this->uvs[poly->uvIndicies[order[edgestart]]];
		HVector2D uv1 = this->uvs[poly->uvIndicies[order[edgestart+1]]];
			
		uvidx[0] = this->uvs.append(((uv1 - uv0) * r0) + uv0);
		uvidx[1] = this->uvs.append(((uv1 - uv0) * r1) + uv0);

		uvidx[2] = poly->uvIndicies[order[edgestart+2]];
		uvidx[3] = poly->uvIndicies[order[edgestart+3]];

	}

//	Add first and second vertex
	newpoly.vertexIndicies.push_back(vidx[1]);
	newpoly.vertexIndicies.push_back(vidx[0]);

//	First edge
	HEdge firstedge = HEdge(vidx[1],vidx[0]);
	memcpy(&firstedge.flags, &tjunct.short_edges[idx].flags,sizeof(HEdge::eflags));
	firstedge.adjpatchidx = tjunct.short_edges[idx].patchidx;
	firstedge.patchidx = poly->patchIndex;
//	firstedge.addCPVertexIndicies(tjunct.short_edges[idx].cpvIndex0,tjunct.short_edges[idx].cpvIndex1);
	newpoly.edgeIndicies.push_back(this->edges.append(firstedge));

	if (!skipnormals) {
		HVector avg = this->normals[poly->normalIndicies[order[edgestart]]];
		avg += this->normals[poly->normalIndicies[order[edgestart+1]]];
		
		if (nidx[0] != poly->normalIndicies[order[edgestart+1]])
			*this->normals.get(nidx[0]) = avg;
		
		if (nidx[1] != poly->normalIndicies[order[edgestart]])
			*this->normals.get(nidx[1]) = avg;
		
		newpoly.normalIndicies.push_back(nidx[1]);
		newpoly.normalIndicies.push_back(nidx[0]);
		
	}

	if (!skipuvs) {
		newpoly.uvIndicies.push_back(uvidx[1]);
		newpoly.uvIndicies.push_back(uvidx[0]);
	}
	HEdge nextedge,*temp;

	if (isFirstPoly) {
		if (isOriginalPoly4point) {
//			First poly
//			Splitting quad poly
//			Makes quad poly

//			Third vertex
///////////////////////////////////////////////////////////////////////////////////////
			newpoly.vertexIndicies.push_back(vidx[2]);
			if (!skipnormals)
				newpoly.normalIndicies.push_back(nidx[2]);
			if (!skipuvs)
				newpoly.uvIndicies.push_back(uvidx[2]);

			temp = this->edges.get(poly->edgeIndicies[order[edgestart+1]]);
			nextedge = HEdge(vidx[0],vidx[2]);
			memcpy(&nextedge.flags,&temp->flags,sizeof(HEdge::eflags));
			nextedge.patchidx = temp->patchidx;
			nextedge.adjpatchidx = temp->adjpatchidx;
//			nextedge.addCPVertexIndicies(temp->cpvIndex0,temp->cpvIndex1);
			newpoly.edgeIndicies.push_back(this->edges.append(nextedge));

//			Fourth vertex
///////////////////////////////////////////////////////////////////////////////////////
			newpoly.vertexIndicies.push_back(vidx[3]);
			if (!skipnormals)
				newpoly.normalIndicies.push_back(nidx[3]);
			if (!skipuvs)
				newpoly.uvIndicies.push_back(uvidx[3]);

			temp = this->edges.get(poly->edgeIndicies[order[edgestart+2]]);
			nextedge = HEdge(vidx[2],vidx[3]);
			memcpy(&nextedge.flags,&temp->flags,sizeof(HEdge::eflags));
			nextedge.patchidx = temp->patchidx;
			nextedge.adjpatchidx = temp->adjpatchidx;
//			nextedge.addCPVertexIndicies(temp->cpvIndex0,temp->cpvIndex1);
			newpoly.edgeIndicies.push_back(this->edges.append(nextedge));

//			Last edge
///////////////////////////////////////////////////////////////////////////////////////
			nextedge = HEdge(vidx[3],vidx[1]);
			nextedge.patchidx = poly->patchIndex;
			nextedge.adjpatchidx = 0;
			newpoly.edgeIndicies.push_back(this->edges.append(nextedge));

		}
		else {
//			First poly
//			Splitting tri poly
//			Makes tri poly

//			Third vertex
///////////////////////////////////////////////////////////////////////////////////////
			newpoly.vertexIndicies.push_back(vidx[2]);
			if (!skipnormals)
				newpoly.normalIndicies.push_back(nidx[2]);
			if (!skipuvs)
				newpoly.uvIndicies.push_back(uvidx[2]);

			temp = this->edges.get(poly->edgeIndicies[order[edgestart+1]]);
			nextedge = HEdge(vidx[0],vidx[2]);
			memcpy(&nextedge.flags,&temp->flags,sizeof(HEdge::eflags));
			nextedge.patchidx = temp->patchidx;
			nextedge.adjpatchidx = temp->adjpatchidx;
//			nextedge.addCPVertexIndicies(temp->cpvIndex0,temp->cpvIndex1);
			newpoly.edgeIndicies.push_back(this->edges.append(nextedge));

//			Last edge
///////////////////////////////////////////////////////////////////////////////////////
			nextedge = HEdge(vidx[2],vidx[1]);
			nextedge.patchidx = poly->patchIndex;
			nextedge.adjpatchidx = 0;
			newpoly.edgeIndicies.push_back(this->edges.append(nextedge));

		}
	}
	else {
		if (isOriginalPoly4point) {
//			Not first poly
//			Splitting quad poly
//			Makes tri poly

//			Third vertex
///////////////////////////////////////////////////////////////////////////////////////
			newpoly.vertexIndicies.push_back(vidx[3]);
			if (!skipnormals)
				newpoly.normalIndicies.push_back(nidx[3]);
			if (!skipuvs)
				newpoly.uvIndicies.push_back(uvidx[3]);

			nextedge = HEdge(vidx[0],vidx[3]);
			nextedge.patchidx = poly->patchIndex;
			nextedge.adjpatchidx = 0;
			newpoly.edgeIndicies.push_back(this->edges.append(nextedge));

			nextedge = HEdge(vidx[3],vidx[1]);
			if (isLastPoly) {
				temp = this->edges.get(poly->edgeIndicies[order[edgestart+3]]);
				memcpy(&nextedge.flags,&temp->flags,sizeof(HEdge::eflags));
				nextedge.patchidx = temp->patchidx;
				nextedge.adjpatchidx = temp->adjpatchidx;
//				nextedge.addCPVertexIndicies(temp->cpvIndex0,temp->cpvIndex1);
			}
			else {
				nextedge = HEdge(vidx[3],vidx[1]);
				nextedge.patchidx = poly->patchIndex;
				nextedge.adjpatchidx = 0;
			}
			newpoly.edgeIndicies.push_back(this->edges.append(nextedge));
		}
		else {
//			Not first poly
//			Splitting tri poly
//			Makes tri poly

//			Third vertex
///////////////////////////////////////////////////////////////////////////////////////
			newpoly.vertexIndicies.push_back(vidx[2]);
			if (!skipnormals)
				newpoly.normalIndicies.push_back(nidx[2]);
			if (!skipuvs)
				newpoly.uvIndicies.push_back(uvidx[2]);

			if (isLastPoly) {
				nextedge = HEdge(vidx[0],vidx[2]);
				nextedge.patchidx = poly->patchIndex;
				nextedge.adjpatchidx = 0;
				newpoly.edgeIndicies.push_back(this->edges.append(nextedge));
			}
			else {
				nextedge = HEdge(vidx[0],vidx[2]);
				nextedge.patchidx = poly->patchIndex;
				nextedge.adjpatchidx = 0;
				newpoly.edgeIndicies.push_back(this->edges.append(nextedge));
			}
			nextedge = HEdge(vidx[2],vidx[1]);
			nextedge.patchidx = poly->patchIndex;
			nextedge.adjpatchidx = 0;
			newpoly.edgeIndicies.push_back(this->edges.append(nextedge));
		}
	}
	newpoly.patchIndex = poly->patchIndex;
	newpoly.polyIndex = poly->polyIndex;
	newpoly.shaderIndicies = poly->shaderIndicies;
}

//////////////////////////////////////////////////////////////////////////////////////////
//	Main function for fixing T-Junctions
//////////////////////////////////////////////////////////////////////////////////////////

void HPolyArray::fixTJunctions()
{
	addToTJunctionList();
	for (unsigned i = 0; i < this->tjuncts->length(); i++)
		splitPoly((*this->tjuncts)[i]);
}
//////////////////////////////////////////////////////////////////////////////////////////
//
//	Functions for identifying T-Junctions
//
//////////////////////////////////////////////////////////////////////////////////////////

void HPolyArray::addToTJunctionList()
{
	for (unsigned i = 0; i < vertices.length(); i++)
		rotateAroundVertex(i);
}

bool HPolyArray::rotateAroundVertex(unsigned pivot)
{
	unsigned neidx = 0;
	HEdge edge,nextedge;
	std::vector<int> edgechecklist;
	for (unsigned i = 0; i < this->neighbors[pivot].nse.size(); i+=2) {
		edge = this->edges[this->neighbors[pivot].nse[i]];			//for debug
		getNextEdgeClockwise(pivot,i,neidx);//doesn't need to be successful, needs to keep looping
		nextedge = this->edges[this->neighbors[pivot].nse[neidx]];	//for debug
	}
	return true;	
}

bool HPolyArray::getNextEdgeClockwise(unsigned pivot,unsigned eidx,unsigned &neidx)
{
	bool successful;
	successful = walkEdgePairs(pivot,eidx,neidx);
	if (!successful) {
		successful = walkTJunctionMethod(pivot,eidx,neidx);
		if (!successful)
			return false;
	}
	return true;
}

bool HPolyArray::walkEdgePairs(unsigned pivot,unsigned eidx,unsigned &neidx) {
	HEdge *e,*edge,*nextedge;

	unsigned mod = eidx % 2;

	if ((eidx+1) >= this->neighbors[pivot].nse.size()) {
		THROW_FATAL_ERROR("Index exceeded edge neighbors.");
	}

	edge = this->edges.get(this->neighbors[pivot].nse[eidx+1]);

	unsigned edgeidx;

	if (mod == 0) {
		for (unsigned i = 0; i < this->neighbors[pivot].nse.size(); i+=2) {
			edgeidx = this->neighbors[pivot].nse[i];
			e = this->edges.get(edgeidx);
			if (*edge == *e) {
				neidx = i;
				nextedge = this->edges.get(this->neighbors[pivot].nse[neidx]);
				return true;
			}
		}
	}
	else {
		THROW_FATAL_ERROR("Couldn't walk edge pairs.");
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//	Method traverses tjunctions using hook flags.
//////////////////////////////////////////////////////////////////////////////////////////

bool HPolyArray::walkTJunctionMethod(unsigned pivot,unsigned eidx,unsigned &neidx)
{
	HEdge deltaedge,nextedge;
	bool finished=false,successful,intersection;
	HEdgeArray shortedges;
	unsigned idx;
	HTJunction tjunct;
	unsigned timeout = 0;

	HEdge edge = this->edges[this->neighbors[pivot].nse[eidx+1]];

	if (!edge.flags.kIsHookShortEdge)
		return false;

	shortedges.append(edge);

	unsigned startadjacent = edge.adjpatchidx;
	
	while (!finished)
	{
		if (timeout++ > 100)
			break;

		successful = nextEdgeOnHook(edge,nextedge,idx);
		if (!successful)
			return false;
		
		if (nextedge.adjpatchidx != startadjacent)
			return false;

		edge = nextedge;

		shortedges.append(nextedge);

		deltaedge.vertexIndex0 = pivot;
		deltaedge.vertexIndex1 = nextedge.vertexIndex1;

		intersection = isIntersecting(pivot,deltaedge,idx);
		if (intersection)
		{
			tjunct.long_edge = deltaedge;
			tjunct.short_edges = shortedges;
			this->tjuncts->append(tjunct);
			neidx = idx;
			return true;
		}
	}
	return false;
}

bool HPolyArray::nextEdgeOnHook(HEdge edge,HEdge &nextedge,unsigned &nseidx)
{
	HEdge deltaedge;
	unsigned pivot = edge.vertexIndex1;
	HNeighbor *n = this->neighbors.get(pivot);

	for (unsigned k = 0; k < n->nse.size(); k++)
	{
		deltaedge = this->edges[n->nse[k]];

		if (deltaedge != edge)
		{
			if (deltaedge.flags.kIsHookShortEdge)
			{
					nextedge = deltaedge;
					nseidx = k;
					return true;
			}
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//	Geometry traversal and analyzing functions
//////////////////////////////////////////////////////////////////////////////////////////

HVector HPolyArray::getVector(HEdge &other) {
	return (this->vertices[other.vertexIndex1] - this->vertices[other.vertexIndex0]);
}

bool HPolyArray::findOtherParallelEdge(unsigned pivot,HEdge edge, HEdge &nextedge,unsigned &nseidx, double tol) {
	bool successful;

	if (pivot != edge.vertexIndex0)
		edge.invert();

	HVector match = getVector(edge);

	nextedge.vertexIndex0 = pivot;

	successful = nextVertexParallelToThisVector(pivot, edge.vertexIndex1, match, nextedge.vertexIndex1, nseidx, tol);
	if (successful) {
		HEdge *temp = this->edges.get(this->neighbors[pivot].nse[nseidx]);
		memcpy(&nextedge.flags,&temp->flags,sizeof(HEdge::eflags));
		nextedge.adjpatchidx = temp->adjpatchidx;
		nextedge.patchidx = temp->patchidx;
//		nextedge.addCPVertexIndicies(temp->cpvIndex0,temp->cpvIndex1);
	}
	return successful;
}

bool HPolyArray::isIntersecting(unsigned pivot, HEdge delta_edge, unsigned &nseidx) {
	HEdge *match;
	HNeighbor *n = this->neighbors.get(pivot);

	for (unsigned k = 0; k < n->nse.size(); k++) {
		match = &this->edges[n->nse[k]];
		if (*match == delta_edge) {
			delta_edge = *match;
			nseidx = k;
			return true;
		}
	}
	return false;
}
 
bool HPolyArray::nextParallelEdge(HEdge edge,HEdge &nextedge,double tol) {
	bool successful;
	
	nextedge.vertexIndex0 = edge.vertexIndex1;
	successful = nextParallelVertex(edge.vertexIndex1,edge.vertexIndex0,nextedge.vertexIndex1,tol);

	return successful;
}

bool HPolyArray::nextEdgeParallelToThisEdge(HEdge edge,HEdge thisEdge,HEdge &nextedge,double tol) {
	bool successful;
	unsigned nseidx;

	HVector match = getVector(thisEdge);
	
	nextedge.vertexIndex0 = edge.vertexIndex1;

	successful = nextVertexParallelToThisVector(edge.vertexIndex1, edge.vertexIndex0, match, nextedge.vertexIndex1, nseidx,tol);
	if (successful) {
		HEdge *temp = this->edges.get(this->neighbors[edge.vertexIndex1].nse[nseidx]);
		memcpy(&nextedge.flags,&temp->flags,sizeof(HEdge::eflags));
		nextedge.adjpatchidx = temp->adjpatchidx;
		nextedge.patchidx = temp->patchidx;
//		nextedge.addCPVertexIndicies(temp->cpvIndex0,temp->cpvIndex1);
	}
	return successful;
}

bool HPolyArray::nextPerpindicularEdge(HEdge edge, HEdge &nextedge,double tol) {
	bool successful;
	
	nextedge.vertexIndex0 = edge.vertexIndex1;
	successful = nextPerpindicularVertex(edge.vertexIndex1,edge.vertexIndex0,nextedge.vertexIndex1,tol);

	return successful;	
}

bool HPolyArray::isEdgeParallel(HEdge edge0,HEdge edge1,double tol) {
	HVector a = getVector(edge0);
	HVector b = getVector(edge1);

	if (a.isParallel(b,tol))
		return true;
	return false;
}

bool HPolyArray::isEdgeParallelAndSameDirection(HEdge edge0,HEdge edge1,double tol) {
	HVector a = getVector(edge0);
	HVector b = getVector(edge1);

	if (a.isParallelAndSameDirection(b,tol)) {
		return true;
	}
	return false;
}

bool HPolyArray::isEdgeGoingSameDirection(HEdge edge0,HEdge edge1,double tol) {
	HVector a = getVector(edge0);
	HVector b = getVector(edge1);

	if (a.isSameDirection(b,tol)) {
		return true;
	}
	return false;
}

bool HPolyArray::nextParallelVertex(unsigned vidx, unsigned previdx, unsigned &nvidx,double tol) {

	unsigned nidx;
	HVector direction;
	HVector a,b;

	direction = this->vertices[vidx] - this->vertices[previdx];

	if (direction.lengthsq() != 0) {
		for (unsigned i = 0; i < this->neighbors[vidx].nsv.size(); i++) {

			nidx = this->neighbors[vidx].nsv[i];
			if (nidx == previdx)
				continue;

			HVector a = direction;
			HVector b = this->vertices[nidx]-this->vertices[vidx];

			if (a.isParallelAndSameDirection(b,tol)) {
					nvidx = nidx;
					return true;
			}
		}
	}

	return false;
}

bool HPolyArray::nextVertexParallelToThisVector(unsigned vidx, unsigned previdx, HVector thisVector, unsigned &nvidx, unsigned &nseidx, double tol) {
	unsigned nidx;
	HVector a,b;

	if (thisVector.lengthsq() != 0) {
		for (unsigned i = 0; i < this->neighbors[vidx].nsv.size(); i++) {

			nidx = this->neighbors[vidx].nsv[i];
			if (nidx == previdx)
				continue;

			HVector a = thisVector;
			HVector b = this->vertices[nidx]-this->vertices[vidx];

			if (a.isParallelAndSameDirection(b,tol)) {
				nvidx = nidx;
				nseidx = i;
				return true;
			}
		}
	}

	return false;
}

bool HPolyArray::nextPerpindicularVertex(unsigned vidx, unsigned previdx, unsigned &nvidx,double tol) {
	unsigned nidx;
	HVector direction;
	HVector a,b;

	direction = this->vertices[vidx] - this->vertices[previdx];

	if (direction.lengthsq() != 0) {
		for (unsigned i = 0; i < this->neighbors[vidx].nsv.size(); i++) {

			nidx = this->neighbors[vidx].nsv[i];
			if (nidx == previdx)
				continue;

			HVector a = direction;
			HVector b = this->vertices[nidx]-this->vertices[vidx];

			if (a.isParallelAndSameDirection(b,tol)) {
				nvidx = nidx;
				return true;
			}
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//	Misc functions
//////////////////////////////////////////////////////////////////////////////////////////

// Super Smooth
void HPolyArray::recalcNormals() {
	normals.clear();
	normals.resize(vertices.length());
	for (unsigned i = 0; i < vertices.length(); i++) {
		HVector normal;
		for (unsigned j = 0; j < this->neighbors[i].nsv.size(); j+=2) {
			normal += this->neighbors.calcNormal(this->neighbors[i].nsv[j]);
		}
		normals[i] = (normal + this->neighbors.calcNormal(i)).normalized();
	}
}


int HPolyArray::circlePivotForNextEdge(unsigned pivot,int eidx,int dir) {

	unsigned mod = eidx % 2;

	int neidx;
	if (dir > 0)
	{
		eidx ++;

		neidx = findAdjacentEdge(pivot,eidx);

		return neidx;
	}
	else if (dir < 0)
	{
		neidx = findAdjacentEdge(pivot,eidx);

		if (neidx < 0)
			return -1;

		if ((neidx-1) < 0)
			neidx = this->neighbors[pivot].nse.size()-2;
		else
			neidx --;

		return neidx;
	}
	return -1;
}

int HPolyArray::findAdjacentEdge(const int pivot,int eidx)
{
	if ((eidx <0) && (eidx >(int)this->neighbors[pivot].nse.size()-1))
		return -1;

	HEdge *tmp;
	HEdge *edge = this->edges.get(this->neighbors[pivot].nse[eidx]);

	int edgecount = this->neighbors[pivot].nse.size();

	int offset;

	unsigned mod = eidx % 2;
	if (!mod)
		offset = 1;
	else
		offset = 0;

	for (int i = offset; i < edgecount; i+=2)
	{
		tmp = this->edges.get(this->neighbors[pivot].nse[i]);
		
		if ((*edge == *tmp)&&(i != eidx))
		{ 
			return i;
		}
	}
	return -1;
}
/*
void HPolyArray::recalcNormals()
{
	HPolyArray *other = new HPolyArray();

	other->model = this->model;
	other->vertices = this->vertices;
	other->edges = this->edges;
	other->uvs = this->uvs;
	other->shaders = this->shaders;
	other->maps = this->maps;

	other->normals.clear();
	other->classList.clear();

	HVector normal;

	HPoly *pPoly;
	for (unsigned i = 0; i < this->length(); i++)
	{
		HPoly poly;
		pPoly = this->get(i);
		if (pPoly->vertexIndicies.length() > 0)
		{
			int numvertices = pPoly->vertexIndicies.length();

			poly.vertexIndicies = pPoly->vertexIndicies;
			poly.edgeIndicies = pPoly->edgeIndicies;
			poly.uvIndicies = pPoly->uvIndicies;
			poly.shaderIndicies = pPoly->shaderIndicies;

			poly.normalIndicies.resize(numvertices);
			
			HVector facenormal,vect0,vect1;
			for (int j = 0; j < numvertices; j++)
			{
				vect0 = this->getVector(this->edges[pPoly->edgeIndicies[j]]);
				if ((j+1)<numvertices)
					vect1 = this->getVector(this->edges[pPoly->edgeIndicies[j+1]]);
				else
					vect1 = this->getVector(this->edges[pPoly->edgeIndicies[0]]);
				
				facenormal += vect0.normalize() ^ vect1.normalize();
			}
			facenormal = facenormal.normalized();
					

			for (j = 0; j < numvertices; j++)
			{
				HEdge *edge = this->edges.get(pPoly->edgeIndicies[j]);		
				if (edge->flags.kHardEdge)
				{
					int index = other->normals.append(facenormal);
					poly.normalIndicies[j] = index;
				}
				else
				{
					normal = calculateNormal(edge->vertexIndex0,i);
					int index = other->normals.append(normal);
					poly.normalIndicies[j] = index;
				}
			}

			other->append(poly,false);
		}
	}

	mdlObject *temp = this->model;
	delete this;
	temp->polygonArray = other;
}
*/

HVector HPolyArray::calculateNormal(unsigned pivot,int faceidx) {
	int eidx = 0,neidx = 0;
	HEdge *edge,*nextedge;

	HVector normal = HVector(0,0,0);
	HVector vectA,vectB;

	bool successful = false;

//  Find first first edge belonging to faceidx
	for (int i = 0; i < (int)this->neighbors[pivot].nsf.length(); i++)
	{
		if (this->neighbors[pivot].nsf[i] == faceidx)
		{
			successful = true;
			break;
		}
	}
	
	if (!successful)
		return normal;

	bool flag;

//  Rotate clockwise through neighboring edges around pivot
	int startedgeidx = eidx = i*2;
	HEdge *startedge = this->edges.get(this->neighbors[pivot].nse[startedgeidx]);
	int cnt = 0;

	do {
		flag = false;
		edge = this->edges.get(this->neighbors[pivot].nse[eidx]);
		vectA = getVector(*edge);
		neidx = circlePivotForNextEdge(pivot,eidx,1);

		if (neidx < 0)
		{
			eidx = eidx + 1;
			flag = true;
		}
		else
		{
			eidx = neidx;
		}

		nextedge = this->edges.get(this->neighbors[pivot].nse[eidx]);

		vectB = getVector(*nextedge);

		if (flag)
			vectB = vectB.negate();

		normal += vectB.normalize() ^ vectA.normalize();

		if ((flag)||(nextedge->flags.kHardEdge)) break;
		
		if (cnt++>100)
		{
			THROW_FATAL_ERROR("Calculating normals timed out.");
		}
			
	} while (nextedge != startedge);
	cnt = 0;

	// Only if we didn't circle all the way around the pivot
	if (nextedge != startedge)
	{
		eidx = startedgeidx;
		do {
			edge = this->edges.get(this->neighbors[pivot].nse[eidx]);

			if (edge->flags.kHardEdge)
				break;
			
			vectA = getVector(*edge);
			neidx = circlePivotForNextEdge(pivot,eidx,-1);

			if (neidx < 0)
				break;
			else
			{
				nextedge = this->edges.get(this->neighbors[pivot].nse[neidx]);
				if (nextedge->flags.kHardEdge)
					break;
			}

			eidx = neidx;

			nextedge = this->edges.get(this->neighbors[pivot].nse[eidx]);	

			vectB = getVector(*nextedge);
			normal += vectA.normalize() ^ vectB.normalize();
			if (cnt++>100)
			{
				THROW_FATAL_ERROR("Calculating normals timed out.");
			}
		} while (nextedge != startedge);
	}

	normal = normal.normalize();
	return normal;
}

void HPolyArray::displaceVertices(double factor) {
	HVector newvertex;	
	for (unsigned i = 0; i < vertices.length(); i++) {
		newvertex = displaceVertex(vertices[i],normals[i],factor);
		vertices[i] = newvertex;
	}
}

HVector HPolyArray::displaceVertex(HVector &position, HVector normal, double factor) {
	HVector newpos = normal.normalize();
	newpos *= factor;
	newpos += position;
	return newpos;
}

int HPolyArray::circlePivotForSmoothingGroups(int pivot,std::vector<SmoothingGroups> &smoothingGroups)
{
	int eidx,neidx;
	HEdge *edge,*nextedge;

//  Rotate clockwise through neighboring edges around pivot

	unsigned long flag = 0;

	int numFaces = this->neighbors[pivot].nsf.length();
	for (int i = 0; i < numFaces; i++)
		flag |= 1 << i;


	int startedgeidx = eidx = 0;
	HEdge *startedge = this->edges.get(this->neighbors[pivot].nse[startedgeidx]);

	int cnt = 0;

	do {
		SmoothingGroup SG;
		int cnt1 = 0;
		do { //Circle to shading group start
			nextedge = NULL;
			edge = this->edges.get(this->neighbors[pivot].nse[eidx]);

			if (edge->flags.kHardEdge)
				break;

			neidx = circlePivotForNextEdge(pivot,eidx,-1);
			if (neidx < 0)
				break;
			nextedge = this->edges.get(this->neighbors[pivot].nse[neidx]);
			if (nextedge->flags.kHardEdge)
				break;
			eidx = neidx;

			if (cnt1++ > 100)
				return 0;
//				THROW_FATAL_ERROR("Smoothing Group Timed Out.");

		} while (neidx != startedgeidx);

		if (nextedge)
			if (nextedge->flags.kHardEdge)
				eidx = neidx;

		startedgeidx = eidx;
		
		startedge = this->edges.get(this->neighbors[pivot].nse[startedgeidx]);
		
		int facebit;
		cnt1 = 0;
		
		do { // Circle through shading group

			facebit = eidx/2;
			flag -= 1 << facebit;

			int faceindex = this->neighbors[pivot].nsf[eidx/2];

			SG.Add(this->neighbors[pivot].nsf[eidx/2]);
			edge = this->edges.get(this->neighbors[pivot].nse[eidx]);
			neidx = circlePivotForNextEdge(pivot,eidx,1);
			if (neidx < 0)
				break;
			nextedge = this->edges.get(this->neighbors[pivot].nse[neidx]);
			if (nextedge->flags.kHardEdge)
				break;
			eidx = neidx;

			if (cnt1++ > 100)
				return 0;
//				THROW_FATAL_ERROR("Smoothing Group Timed Out.");

		} while (neidx != startedgeidx);

		if ((neidx == startedgeidx)&&(nextedge->flags.kHardEdge))
		{
			SmoothingGroup aSG,bSG;
			int SGSize = SG.Size();
			aSG.Resize(SGSize-1);
			bSG.Resize(SGSize-1);
			for (int i = 0; i < (SGSize-1); i++)
			{
				aSG[i] = SG[i];
				bSG[i] = SG[SGSize-i-1];
			}
			smoothingGroups[pivot].Add(aSG);
			smoothingGroups[pivot].Add(bSG);

			return 0;
		}

		smoothingGroups[pivot].Add(SG);

		for (int j = 0; j < numFaces; j++)
		{
			if ( (1<<j) & flag ){
				eidx = j * 2;
				break;
			}
		}
		if (cnt++ > 100)
			return 0;
//			THROW_FATAL_ERROR("Smoothing Group Timed Out.");

	} while (flag != 0);
	return 1;
}

unsigned long HPolyArray::getNextAvailableBit(unsigned long id,unsigned long dontuse,bool &successful)
{
	successful = false;
	for (int i = 0; i < 32; i++)
	{
		unsigned long bit = 1<<i;

		if ((!(id & bit)) && (!(bit & dontuse)))
		{
			successful = true;
			return bit;
		}
	}
	return 0;
}

void HPolyArray::calculateSmoothingGroups()
{
	std::vector<SmoothingGroups> smoothingGroupTable;
	smoothingGroupTable.resize(this->vertices.length());

	int result;
	for (int i = 0; i < (int)this->vertices.length(); i++)
	{
		result = circlePivotForSmoothingGroups(i,smoothingGroupTable);
		if (!result)
			return;
	}

	for (i = 0; i < (int)this->vertices.length(); i++)
	{
		if (smoothingGroupTable[i].Size()>1)
			_calculateSmoothingGroups(smoothingGroupTable[i]);
	}

	for (i = 0; i < (int)this->vertices.length(); i++)
	{
		if (smoothingGroupTable[i].Size()==1)
			_calculateSmoothingGroups(smoothingGroupTable[i]);
	}
}

void HPolyArray::_calculateSmoothingGroups(SmoothingGroups &smoothingGroups)
{

	int NumSmoothingGroups = smoothingGroups.Size();

//  Loop through Shading Groups
	for (int i = 0; i < smoothingGroups.Size(); i++)
	{
		unsigned long common = 0,dontuse = 0;

//		Get result: AND Current Group
		common = smoothingGroups[i].AndSG(this);

//		Loop through Other Groups
		for (int j = 0; j < smoothingGroups.Size(); j++)
		{
//			Skip current group
			if (j == i) continue;
			
//			Get result: OR Other Groups "DontUse" flags
			dontuse |= smoothingGroups[j].OrSG(this);
		}

//		Get result: OR Current Group "DontUse" flags
		dontuse |= smoothingGroups[i].OrDU(this);

		unsigned long flag;
		bool successful;

		flag = getNextAvailableBit(~common,dontuse,successful);

		if (flag == 0)
			flag = getNextAvailableBit(0,dontuse,successful);

//      Set face: OR Current group with flag
		smoothingGroups[i].OrSG(flag,this);


//		Loop through Other Groups,OR "DontUse" flags with result from of above		
		for (j = 0; j < smoothingGroups.Size(); j++)
		{
			if (j == i) continue;

			smoothingGroups[j].OrDU(flag,this);
		}
	}
}

unsigned long SmoothingGroup::AndSG(HPolyArray *polygons)
{
	unsigned long result = 0;
	for (int i = 0; i < (int)m_faceIndicies.size(); i++)
	{
		int faceIndex = m_faceIndicies[i];
		HPoly *poly = polygons->get(faceIndex);
		result &= poly->smoothGrpID;
	}
	return result;
}

unsigned long SmoothingGroup::OrDU(HPolyArray *polygons)
{
	unsigned long result = 0;
	for (int i = 0; i < (int)m_faceIndicies.size(); i++)
	{
		int faceIndex = m_faceIndicies[i];
		HPoly *poly = polygons->get(faceIndex);
		result |= poly->dontuse;
	}
	return result;
}

unsigned long SmoothingGroup::OrSG(HPolyArray *polygons)
{
	unsigned long result = 0;
	for (int i = 0; i < (int)m_faceIndicies.size(); i++)
	{
		int faceIndex = m_faceIndicies[i];
		HPoly *poly = polygons->get(faceIndex);
		result |= poly->smoothGrpID;
	}
	return result;
}

void SmoothingGroup::OrSG(unsigned long flag,HPolyArray *polygons)
{
	for (int i = 0; i < (int)m_faceIndicies.size(); i++)
	{
		int faceIndex = m_faceIndicies[i];
		HPoly *poly = polygons->get(faceIndex);
		poly->smoothGrpID |= flag;
	}	
}

void SmoothingGroup::OrDU(unsigned long flag,HPolyArray *polygons)
{
	for (int i = 0; i < (int)m_faceIndicies.size(); i++)
	{
		int faceIndex = m_faceIndicies[i];
		HPoly *poly = polygons->get(faceIndex);
		poly->dontuse |= flag;
	}	
}

//////////////////////////////////////////////////////////////////////////////////////////
//	More basic functions
//////////////////////////////////////////////////////////////////////////////////////////

HPoly *HPolyArray::get(unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HPolyarray]");
	}
	return &classList[index];
}

HPoly *HPolyArray::begin() {
	return &classList.front();
}

HPoly *HPolyArray::end() {
	return &classList.back();
}

HPoly HPolyArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HPolyarray]");
	}
	return classList[index];
}

HPoly &HPolyArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HPolyarray]");
	}
	return classList[index];
}

HPolyArray &HPolyArray::operator = ( const HPolyArray &other ){
	vertices = other.vertices;
	normals = other.normals;
	uvs = other.uvs;
	edges = other.edges;
	neighbors = other.neighbors;
	shaders = other.shaders;
	maps = other.maps;
	model = other.model;
	classList = other.classList;
	return *this;
}

std::ostream &operator << (std::ostream &os, HPolyArray &polys){
/*
	os << "TOTAL POLYGONS: " << polys.length() << std::endl << std::endl;

	for (unsigned i = 0; i < polys.length(); i++) {
		if (polys[i].vertexIndicies.size() > 0) {
			os << polys[i];
			for (unsigned k = 0; k < polys[i].edgeIndicies.length(); k++)
				os << "  Edge(" << polys[i].edgeIndicies[k] << ") " << polys.edges[polys[i].edgeIndicies[k]] << std::endl;

			for (unsigned j = 0; j < polys[i].vertexIndicies.length(); j++)
				os << "  Vertex(" << polys[i].vertexIndicies[j] << ") Neighbor: " << polys.neighbors[polys[i].vertexIndicies[j]] << std::endl;
			os << std::endl;
		}
		else {
			os << "Polygon( " << i << " ) Is Empty!" << std::endl << std::endl;
		}
	}
*/
	return os;
}