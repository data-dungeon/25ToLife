//////////////////////////////////////////////////////////////////////////////////////////
//
// HPOLYARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HPOLYARRAY_H
#define __HPOLYARRAY_H

#ifndef __HPOLY_H
#include "hpoly.h"
#endif
#ifndef __HEDGE_H
#include "hedge.h"
#endif
#ifndef __HNEIGHBOR_H
#include "hneighbor.h"
#endif
#ifndef __HTJUNCTION_H
#include "htjunction.h"
#endif
#ifndef __HVECTORMAP_H
#include "hvectormap.h"
#endif
#ifndef __UIFLAGS_H
#include "uiflags.h"
#endif
#ifndef __HSHADER_H
#include "hshader.h"
#endif
#ifndef __HMAP_H
#include "hmap.h"
#endif

class mdlObject;

class SmoothingGroup
{
public:
	SmoothingGroup() {};
	void Add(const int faceindex) { m_faceIndicies.push_back(faceindex); }
	int Size() { return m_faceIndicies.size(); }
	void Resize(unsigned size) { m_faceIndicies.resize(size); }

//  returns result
	unsigned long AndSG(HPolyArray *polygons);
	unsigned long OrSG(HPolyArray *polygons);
	unsigned long OrDU(HPolyArray *polygons);

//  sets face flags
	void OrSG(unsigned long flag,HPolyArray *polygons);
	void OrDU(unsigned long flag,HPolyArray *polygons);

	int operator[] ( unsigned index ) const { return m_faceIndicies[index]; }
	int& operator[] ( unsigned index ) { return m_faceIndicies[index]; }
	std::vector<int> m_faceIndicies;

};

class SmoothingGroups
{
public:
	SmoothingGroups() {};
	void Add(const SmoothingGroup &SmoothingGroup) { m_smoothingGroups.push_back(SmoothingGroup); }
	int Size() { return m_smoothingGroups.size(); }
	void Resize(unsigned size) { m_smoothingGroups.resize(size); }
	SmoothingGroup operator[] ( unsigned index ) const { return m_smoothingGroups[index]; }
	SmoothingGroup &operator[] ( unsigned index ) { return m_smoothingGroups[index]; }
	std::vector<SmoothingGroup> m_smoothingGroups;
};

class HPolyArray
{
public:

	HPolyArray ();
	HPolyArray(const HPolyArray &other);
	~HPolyArray ();

	void				clear(void);
	unsigned			length(void) const;

	unsigned			append(HPoly &poly,bool addneighborinfo);
	void				remove(unsigned index);

	HPoly				*HPolyArray::begin();
	HPoly				*HPolyArray::end();

	HPoly				*get(unsigned index);
	HPoly				operator[]( unsigned index ) const;
 	HPoly&	 			operator[]( unsigned index );
	HPolyArray			&operator = ( const HPolyArray &other );
	friend std::ostream		&operator << (std::ostream &os,HPolyArray &polys);

//////////////////////////////////////////////////////////////////////////////////////////
//	fix t-junction methods
//////////////////////////////////////////////////////////////////////////////////////////
	void				fixTJunctions();
	void				addToTJunctionList();
	bool				rotateAroundVertex(unsigned vidx);
	bool				getNextEdgeClockwise(unsigned pivot,unsigned eidx,unsigned &neidx);
	bool				nextEdgeOnHook(HEdge edge,HEdge &nextedge,unsigned &nseidx);
	bool				walkEdgePairs(unsigned pivot,unsigned eidx,unsigned &neidx);
	bool				walkTJunctionMethod(unsigned pivot,unsigned eidx,unsigned &neidx);

	HVector				getVector(HEdge &other);

//////////////////////////////////////////////////////////////////////////////////////////
//	neighbor traversing methods
//////////////////////////////////////////////////////////////////////////////////////////
	bool				nextParallelVertex(unsigned vidx, unsigned previdx, unsigned &nvidx,double tol);
	bool				nextParallelEdge(HEdge edge,HEdge &nextedge,double tol);
	bool				nextPerpindicularEdge(HEdge edge, HEdge &nextedge,double tol);
	bool				nextPerpindicularVertex(unsigned vidx, unsigned previdx, unsigned &nvidx,double tol);
	bool				findOtherParallelEdge(unsigned pivot,HEdge edge, HEdge &nextedge,unsigned &nseidx,double tol);
	bool				nextEdgeParallelToThisEdge(HEdge edge,HEdge thisEdge,HEdge &nextedge,double tol);
	bool				nextVertexParallelToThisVector(unsigned vidx, unsigned previdx, HVector thisVector, unsigned &nvidx, unsigned &nseidx, double tol);
	bool				isIntersecting(unsigned pivot,HEdge delta_edge,unsigned &nseidx);
	bool				isEdgeParallel(HEdge edge0,HEdge edge1,double tol);
	bool				isEdgeParallelAndSameDirection(HEdge edge0,HEdge edge1,double tol);
	bool				isEdgeGoingSameDirection(HEdge edge0,HEdge edge1,double tol);

	int					circlePivotForNextEdge(unsigned pivot,int eidx,int dir = 1);
	int					findAdjacentEdge(const int pivot,int eidx);


//////////////////////////////////////////////////////////////////////////////////////////
//	split poly methods
//////////////////////////////////////////////////////////////////////////////////////////
	void				splitPoly(HTJunction &tjunct);
	void				deriveSplitPoly(unsigned idx,unsigned edgestart,HTJunction &tjunct,HPoly *poly,HPoly &newpoly);
	bool				findEdgeOnPoly(HEdge edge,HPoly &poly,unsigned &startidx);
	void				evaluatePolygonNeighbors(std::vector<int> &vtxIndicies);	
	void				evaluateVertexNeighbors(unsigned vertex_index);	
	bool				doesFaceBelongToVertex(unsigned poly_index,unsigned vertex_index);	
	void				deleteNeighborFace(HNeighbor *n,unsigned nsf_index);

	void				deriveNPoly(unsigned edgestart,HTJunction &tjunct,HPoly *poly,HPoly &newpoly);
	bool				getFacesFromEdge(HEdge *edge,std::vector<int> &faceindicies);
	
//////////////////////////////////////////////////////////////////////////////////////////
//	misc methods
//////////////////////////////////////////////////////////////////////////////////////////

	void				recalcNormals();
	void				displaceVertices(double factor);
	HVector				displaceVertex(HVector &position, HVector normal, double factor);
	void				triangulate();
	HVector				calculateNormal(unsigned pivot,int faceidx);

	void				calculateSmoothingGroups();
	int					circlePivotForSmoothingGroups(int pivot,std::vector<SmoothingGroups> &smoothingGroups);
	void				_calculateSmoothingGroups(SmoothingGroups &SmoothingGroups);

	unsigned long		getNextAvailableBit(unsigned long id,unsigned long dontuse,bool &successful);

	void				getIntOnBothSides(int index,int &indexL,int &indexR,int numIndicies,bool edges = false);
	
	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS VARIABLES
	////////////////////////////////////////////////////////////////////////////////////////

	HVectorArray		vertices;	// vertices
	HVectorArray		normals;	// normals
	HVector2DArray		uvs;		// uv
	HNeighborArray		neighbors;	// neighbors
	HEdgeArray			edges;		// edges
	HTJunctionArray		*tjuncts;	// tjunctions
	HShaderArray		shaders;	// shaders
	HMapArray			maps;		// texturemaps
	mdlObject			*model;

	std::vector<HPoly> classList;
};

#endif
