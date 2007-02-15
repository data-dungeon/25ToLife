//////////////////////////////////////////////////////////////////////////////////////////
//
//	HNEIGHBOR CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HNEIGHBOR_H
#define __HNEIGHBOR_H

#ifndef __HINTARRAY_H
#include "hintarray.h"
#endif
#ifndef __HVECTOR_H
#include "hvector.h"
#endif

class HPoly;
class HPatch;
class HNeighborArray;
class mdlObject;

class HNeighbor
{
public:
	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS FLAGS
	////////////////////////////////////////////////////////////////////////////////////////

	struct vflags {
		unsigned long	kVertexIsPatchCorner			: 1;
	} flags;

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS METHODS
	////////////////////////////////////////////////////////////////////////////////////////

	HNeighbor();
	HNeighbor(const HNeighbor &other);
	~HNeighbor();

	void				clear();

	void				calcNeighbor(unsigned idx,HPoly &poly);
	void				calcNeighbor(unsigned idx,HPatch &patch);

	void				getIntOnBothSides(int index,int &indexL,int &indexR,int numIndicies,bool edges = false);

	int					getNormalIndex(unsigned faceid);
	int					getUVIndex(unsigned faceid);

	HNeighbor			&operator = ( const HNeighbor &other );
	friend std::ostream		&operator << (std::ostream &os,HNeighbor &n);

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS VARIABLES
	////////////////////////////////////////////////////////////////////////////////////////

	// all next indicies are in a clockwise order

	// faceids indicies matches every three next indicies
	// this lets you figure out which face belongs to which
	// series of next indicies

	enum neighbortype {PATCH, POLYGON} type;

	unsigned vid;				// vertex index
	HIntMap cpids;				// cp indicies
	std::vector<int>	normals;// normals
	std::vector<int>	uvs;	// uvs
	
	std::vector<int>	nsv;	// nsv stands for next surrounding vertex
	HIntArray			nsf;	// next surrounding face
	std::vector<int>	nse;	// next surrounding edge
	std::vector<int>	nsfnv;	// next surrounding face, number of vertices
	HIntMap				nsp;	// next surrounding patch

	HNeighborArray *neighbors;
};

class HNeighborArray
{
public:
	HNeighborArray();
	HNeighborArray(const unsigned size);
	~HNeighborArray();

	void				clear();
	unsigned			length() const;
	unsigned			append(HNeighbor &n);
	void				resize(const unsigned size);

	HVector				calcNormal(unsigned vidx);

	HNeighbor			*get(unsigned index);
	HNeighbor			*begin();
	HNeighbor			*end();

	HNeighbor			operator[](unsigned index) const;
	HNeighbor			&operator[](unsigned index);
	HNeighborArray		&operator = ( const HNeighborArray &other );
	friend std::ostream		&operator << (std::ostream &os,HNeighborArray &other);

	mdlObject			*model;

	std::vector<HNeighbor> classList;
};

#endif
