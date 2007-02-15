#include <vector>

#ifndef __PatchInfo_H
#define __PatchInfo_H

#include <fstream.h>

class Vector2;

class Edge
{
public:
	struct eflags {
		unsigned long
						kIsBorderEdge				: 1;
	} flags;

	Edge() {
		memset(&flags,0,sizeof(unsigned long));
		adjpatchidx = -1;
		patchidx = -1;
	};
	Edge(const Edge &other) {
		memcpy(&this->flags,&other.flags,sizeof(unsigned long));
		_v0 = other._v0;
		_v1 = other._v1;
		adjpatchidx = other.adjpatchidx;;
		patchidx = other.patchidx;;
	};
	Edge(int v0,int v1) {
		memset(&flags,0,sizeof(unsigned long));
		_v0 = v0;
		_v1 = v1;
		adjpatchidx = -1;
		patchidx = -1;
	};

	Edge &operator = ( const Edge &other ) {
		memcpy(&this->flags,&other.flags,sizeof(unsigned long));
		_v0 = other._v0;
		_v1 = other._v1;
		adjpatchidx = other.adjpatchidx;;
		patchidx = other.patchidx;;
		return *this;
	};

	bool operator == ( const Edge &other ) const {
		if (((_v0 == other._v0) && (_v1 == other._v1))||
			((_v1 == other._v0) && (_v0 == other._v1)))
			return true;
		return false;		
	};
	
	bool operator != ( const Edge &other ) const { return !(*this == other); };

	friend ostream &operator << (ostream &os,const Edge &other) {
		if (other.flags.kIsBorderEdge)
			os << "[Border]";
		os << other._v0 << ",";
		os << other._v1 << ", Adj: ";
		os << other.adjpatchidx << endl;
		return os;
	};

public:
	int _v0,_v1;
	int adjpatchidx;
	int patchidx;
};

typedef std::vector<Vector *> VertexList;
typedef std::vector<Edge> EdgeList;
typedef std::vector<int> IntArray;
typedef std::vector<IntArray> MultiIntArray;

class PatchInfo {
public:
	PatchInfo() {};
	PatchInfo(const PatchInfo &other) {
		_vids = other._vids;
		_eids = other._eids;
	}
	~PatchInfo() {};
	PatchInfo &operator = ( const PatchInfo &other ) {
		_vids = other._vids;
		_eids = other._eids;
		return *this;
	};

	IntArray _vids;
	IntArray _eids;
};

typedef std::vector<PatchInfo> PatchInfoList;

class Neighbor
{
public:
	struct nflags {
		unsigned long
						kIsBorderCorner				: 1;
	} flags;


	Neighbor() { memset(&flags,0,sizeof(unsigned long)); };
	Neighbor(const Neighbor &other) {
		memcpy(&this->flags,&other.flags,sizeof(unsigned long));
		_nse = other._nse;
		_nsp = other._nsp;
	}
	~Neighbor() {};

	Neighbor &operator = ( const Neighbor &other ) {
		memcpy(&this->flags,&other.flags,sizeof(unsigned long));
		_nse = other._nse;
		_nsp = other._nsp;
		return *this;
	};

public:
	IntArray _nse;
	IntArray _nsp;
};

typedef std::vector<Neighbor> NeighborList;
typedef std::vector<double> DoubleArray;
typedef std::vector<DoubleArray> DoubleMultiArray;

class NeighborTree {
public:

	int		addVertexToList(Vector *vp);
	int		addEdgeToList(int v0,int v1,int patchidx);	
	void	SetNeighborsForPatch(PatchInfo &pinfo,int patchidx);

	Vector	GetVertexPosition(const int index);
	double	DistanceTo(const Vector p0,const Vector p1);

	int		GetFirstCorner();
	int		GetCornerPatch();
	int		GetStartEdge();
	int		GetFirstBorderEdge();

	int		GetMatchingEdge(int eidx);
	int		GetMatchingEdge(int pidx,int eidx);
	int		GetMatchingEdgePlus1(int eidx);
	int		GetMatchingEdgePlus2(int eidx);
	int		GetMatchingEdgePlus3(int eidx);

	int		GetNextEdge(int eidx,bool &atborder);

	bool	GenerateNeighborInfo(HashPatches &patches);
	bool	GenerateTopologyInfo();
	bool	GenerateDistanceInfo();

	bool	IsSelectionCylindrical();
	void	InitVisitedLookup();
	int		HasBeenVisited(int pid);
	void	SetPatchVisited(int pid);
	
	Vector2	CalcPatchUV(unsigned vert,unsigned col,unsigned row);
	Vector2	CalcPatchUVStretch(unsigned vert,unsigned col,unsigned row);


	int		DoesValueExist(const int value,const IntArray &array);
	bool	GetPatchColAndRow(int pidx,int &col,int &row);
	int		GetVertexOffset(const int patch,const int col, const int row);
	void	ShiftIntArray(const int offset,std::vector<Vector2> &array);

	friend ostream &operator << (ostream &os,NeighborTree &other);

public:
	std::vector<Vector *>	_vpointers;
	PatchInfoList			_patchinfolist;
	EdgeList				_edges;
	NeighborList			_neighbors;
	MultiIntArray			_vtopology;
	MultiIntArray			_ptopology;
	DoubleMultiArray		_ulist,_vlist;
	int						_cornercount;

	std::vector<bool>		_visited;
};

#include "patchinfo.hpp"

#endif
