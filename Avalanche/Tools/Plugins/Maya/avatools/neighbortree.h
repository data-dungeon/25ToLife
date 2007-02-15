
#ifndef __NeighborTree_H
#define __NeighborTree_H

class Edge
{
public:
	Edge() {
		borderedge = 1;
		adjpolyidx = -1;
		polyidx = -1;
	};
	Edge(const Edge &other)
	{
		borderedge = other.borderedge;
		v0 = other.v0;
		v1 = other.v1;
		adjpolyidx = other.adjpolyidx;;
		polyidx = other.polyidx;;
	};

	Edge(int V0,int V1)
	{
		borderedge = 1;
		v0 = V0;
		v1 = V1;
		adjpolyidx = -1;
		polyidx = -1;
	};

	// operator does identical vertex check as well as matching
	bool operator == ( const Edge &other ) const 
	{
		if (((v0 == other.v0) && (v1 == other.v1))||
			((v1 == other.v0) && (v0 == other.v1)))
			return true;
		return false;		
	};
	
	bool operator != ( const Edge &other ) const { return !(*this == other); };

	friend std::ostream &operator << (std::ostream &os,const Edge &other)
	{
		if (other.borderedge)
			os << "[Border]";
		os << other.v0 << ",";
		os << other.v1 << ", Adj: ";
		os << other.adjpolyidx << std::endl;
		return os;
	};

public:
	int v0,v1;
	int adjpolyidx;
	int polyidx;
	int borderedge;
};

typedef std::vector<Vector> VertexList;
typedef std::vector<Edge> EdgeList;
typedef std::vector<int> IntArray;
typedef std::vector<IntArray> MultiIntArray;

class PolyInfo
{
public:
	PolyInfo() {};

public:
	IntArray m_vids;
	IntArray m_uvids;
	IntArray m_eids;
};

typedef std::vector<PolyInfo> PolyInfoList;

class Neighbor
{
public:
	Neighbor() { corner = 0; border = 0;}

public:

	std::vector<int> sv;		// surrounding vertex
	std::vector<int> sf;		// surrounding face
	std::vector<int> uvs;		// uvs
	std::vector<int> se;		// surrounding edges

//	IntArray m_nse;
//	IntArray m_nsp;
	int corner;
	int border;
};

typedef std::vector<Neighbor> NeighborList;
typedef std::vector<double> DoubleArray;
typedef std::vector<DoubleArray> DoubleMultiArray;

class NeighborTree
{
public:
	NeighborTree();

	int		AddEdgeToList(int v0,int v1,int polyidx);	

	Vector	GetVertexPosition(int index);
	double	DistanceTo(Vector p0,Vector p1);

	int		GetFirstCorner();
	int		GetCornerPoly();
	int		GetStartEdge();
	int		GetFirstBorderEdge();

	int		GetMatchingEdge(int eidx);
	int		GetMatchingEdge(int pidx,int eidx);
	int		GetMatchingEdgePlus1(int eidx);
	int		GetMatchingEdgePlus2(int eidx);
	int		GetMatchingEdgePlus3(int eidx);

	int		GetNextEdge(int eidx,bool &atborder);

	bool	GenerateNeighborInfo(int selectionIndex);
	bool	GenerateTopologyInfo();
	bool	GenerateDistanceInfo();

	bool	IsSelectionCylindrical();
	void	InitVisitedLookup();
	int		HasBeenVisited(int pid);
	void	SetPolyVisited(int pid);
	
	Vector2	CalcPolyUV(unsigned vert,unsigned col,unsigned row);
	Vector2	CalcPolyUVStretch(unsigned vert,unsigned col,unsigned row,double scaletofit);

	int		DoesValueExist(const int value,const IntArray &array);
	bool	GetPolyColAndRow(int pidx,int &col,int &row);
	int		GetVertexOffset(int polyidx,int col,int row);

	void	ShiftIntArray(const int offset,std::vector<int> &array);
	void	ShiftVector2Array(const int offset,std::vector<Vector2> &array);

	friend std::ostream &operator << (std::ostream &os,NeighborTree &other);

	void	GetIntOnBothSides(int index,int &indexL,int &indexR,int len,bool edges = false);
	void	GetIndiciesStartingAt(int index,std::vector<int> indicies, std::vector<int> &result,int flag);

	void	CalcNormals(bool supersmooth);
	Vector	CalcNormal(int vidx);

	int		NumVertices() { return m_vertices.size(); }
	int		NumNeighborFaces() { return m_polyinfolist.size(); }

	std::vector<Vector> *GetVertices() { return &m_vertices; }
	std::vector<Vector> *GetVertexNormals() { return &m_vertexnormals; }
	std::vector<int>	*GetPolyIndicies();
	std::vector<Vector2>*GetUVs() { return &m_uvs; }

	void	GetPolyCounts(MIntArray &polycounts,bool wholemesh);
	void	GetPolyConnects(MIntArray &polyconnects);
	void	GetUVConnects(MIntArray &uvconnects);

	void	GetPolyMaxColRow(int &col,int &row) { col = m_ptopology[0].size(); row = m_ptopology.size(); }

	int		GetUVAtVertexAndFace(int vidx,int pidx);
	int		GetUVAtColAndRow(int vert,int col,int row);
	int		GetEdgesAtVertexAndFace(int vidx,int pidx,int &eidx0,int &eidx1);

	void	GetInteriorEdges(std::vector<int> &edges);

	void	BuildTextureClampData(MFloatArray &uarray,MFloatArray &varray,MIntArray &uvconnects,int nostretch,double scaletofit);

	Neighbor		operator[](unsigned index) const { return m_neighbors[index]; }
	Neighbor		&operator[](unsigned index) { return m_neighbors[index]; }

public:
	std::vector<Vector>	m_vertices;
	std::vector<Vector>	m_vertexnormals;
	std::vector<Vector2>	m_uvs;
	EdgeList					m_edges;
	PolyInfoList			m_polyinfolist;
	NeighborList			m_neighbors;
	
	MultiIntArray			m_vtopology;
	MultiIntArray			m_ptopology;
	DoubleMultiArray		m_ulist,m_vlist;
	int						m_cornercount;

	std::vector<bool>		m_visited;

	std::vector<int>		m_polyindicies;

	int						m_selectedVertexIndex;
};

#include "neighbortree.hpp"

#endif
