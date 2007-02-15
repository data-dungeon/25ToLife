/***************************************************************************/
// The main terrain collision object
/***************************************************************************/
#ifndef OCTTREE_H
#define OCTTREE_H

#include "collide/StackArray.h"
#include "collide/tclayer.h"
#include "linearAlgebra/semiseg.h"
#include "collide/obb.h"

//#define OCTREELINEDEBUG

class ListOfTriangleLists
{
public:
	inline ListOfTriangleLists();

	// Give me memory!
	inline void Initialize(CCollisionStack *stack);

	// Clean up memory
	inline void Terminate();

	// Add a triangle list
	inline void Add(TerrainTriangle **triList, int triangles);

	// How many lists are there?
	inline int Lists();

	// Reset the enumeration
	inline void ResetEnumeration();

	// Get the next triangle
	inline TerrainTriangle *GetNextTriangle(u32 testID);

private:
	CCollisionStackEndArray<TerrainTriangleList> m_list;

	// Iteration
	int							m_onList;
	int							m_onTriangle;
};

//////////////////////// OctTreeAABBIterator ////////////////////////
class OctTreeAABBIterator
{
public:
	inline AABB &Bounds();

	// Get it going
	bool Initialize(TerrainCollideLayer &layer, CCollisionStack *stack);
	void Terminate() {m_triList.Terminate();}

	// Get at the triangles
	inline TerrainTriangle *GetNextTriangle(u32 testID);

private:
	AABB											 m_bounds;
	ListOfTriangleLists						 m_triList;

	// Walk the tree!!!
	void Walk(const Octant &octant, const OctTreeNode *node, const AABB &aabb);
};

//////////////////////// OctTreeLineIterator ////////////////////////
class OctTreeLineIterator
{
public:
	inline void Setup(Vector3CRef start, Vector3CRef end);

	bool Initialize(TerrainCollideLayer &layer, CCollisionStack *stack);
	void Terminate() {m_triList.Terminate();}

	// Get at the triangles
	TerrainTriangle *GetNextTriangle(u32 testID);

	// You have to tell me where the intersection was
	// as it might not have been in the current octant
	void FoundEnd(const Vector3 &intersection);

#ifdef OCTREELINEDEBUG
	static void Draw(bool showOctants = true, bool showTris = true);
	static void ClearTrace();
	static void StartTrace();
	static void EndTrace();
#else
	static void Draw(bool showOctants = true, bool showTris = true) {}
	static void ClearTrace() {}
	static void StartTrace() {}
	static void EndTrace() {}
#endif

private:
	// The actual line we are working with
	Vector3													m_start, m_end;
	Vector3													m_clipStart, m_clipEnd;
	SemiSegment												m_semiSeg;
	Vector3													m_currentCenter;
	float														m_leafSize;
	inline void ToCenter(Vector3CRef pos);
	inline void UpdateCenter(Cube::Face exit);
#ifdef OCTREELINEDEBUG
	int														m_onLeaf;
#endif

	// The tree
	Octant													m_rootOctant;
	const OctTreeNode										*m_root;
	uint														m_leafDepth;

	// The stack
	CCollisionStack										*m_stack;

	// Iteration control
	bool														m_haveEndPoint;
	Vector3													m_endPoint;
	bool														m_doneWalk;
	OctTreeNodeStack										m_leafStack;
	inline bool HaveValidEnd();

	// The current batch of triangles
	ListOfTriangleLists									m_triList;

	// Build the data
	int NextLeaf();
	FullOctTreeNode *LeafNode(Vector3CRef point);
#ifdef ANALADDLEAVES
	void AddAllTriangles(const FullOctTreeNode &node);
#else
	void AddAllTriangles(const ExpandedOctTreeNode &node);
#endif

	// Debugging
#ifdef OCTREELINEDEBUG
#define OLI_MAXDEBUGOCTANTS		32
#define OLI_MAXDEBUGLINES 			4
#define OLI_MAXDEBUGTRILISTS		128
#define OLI_MAXDEBUGPOINTS  		4
	static int						s_trace;
	static Octant 					s_debugLeaf[OLI_MAXDEBUGOCTANTS];
	static int						s_debugLeafColor[OLI_MAXDEBUGOCTANTS];
	static int						s_debugLeaves;
	static Vector3  				s_debugLine[OLI_MAXDEBUGLINES][2];
	static int						s_debugLines;
	static TerrainTriangleList s_debugTriList[OLI_MAXDEBUGTRILISTS];
	static int						s_debugTriLists;
	static Vector3					s_debugPoint[OLI_MAXDEBUGPOINTS];
	static int						s_debugPoints;

	static void DebugAddLine(const Vector3 &start, const Vector3 &end);
	static void DebugAddLeaf(const Octant &leaf, int color = 0);
	static void DebugAddTriangleList(TerrainTriangle **triList, int triangles);
	static void DebugAddPoint(const Vector3 &p);
#endif
};

//////////////////////// OctTreeAABBIterator ////////////////////////
class OctTreeOBBIterator
{
public:
	inline void Setup(const AABB &aabb);
	inline void Setup(const AABB &aabb, const OBB &obb, const Vector3 *corners);

	// Get it going
	bool Initialize(TerrainCollideLayer &layer, CCollisionStack *stack);
	void Terminate() {m_triList.Terminate();}

	// Get at the triangles
	inline TerrainTriangle *GetNextTriangle(u32 testID);

private:
	AABB							m_aabb;
	const OBB					*m_obb;
	const Vector3				*m_corner;
	ListOfTriangleLists		m_triList;

	// Walk the tree!!!
	void WalkAABB(const Octant &octant, const OctTreeNode *node);
	void WalkOBB(const Octant &octant, const OctTreeNode *node);
};

//////////////////////// OctTreeExactLineIterator ////////////////////////
class OctTreeLineExactIterator
{
public:
	inline void Setup(Vector3CRef start, Vector3CRef end);

	bool Initialize(TerrainCollideLayer &layer, CCollisionStack *stack);
	void Terminate() {m_triList.Terminate();}

	// Get at the triangles
	TerrainTriangle *GetNextTriangle(u32 testID);

	// You have to tell me where the intersection was
	// as it might not have been in the current octant
	void FoundEnd(const Vector3 &intersection);

private:
	// The actual line we are working with
	Vector3													m_start, m_end;
	Vector3													m_clipStart, m_clipEnd;
	SemiSegment												m_semiSeg;
	Vector3													m_currentCenter;
	float														m_leafSize;
	inline void ToCenter(Vector3CRef pos);
	inline void UpdateCenter(Cube::Face exit);

	// The tree
	Octant													m_rootOctant;
	const OctTreeNode										*m_root;
	uint														m_leafDepth;

	// The stack
	CCollisionStack										*m_stack;

	// Iteration control
	bool														m_haveEndPoint;
	Vector3													m_endPoint;
	bool														m_doneWalk;
	OctTreeNodeStack										m_leafStack;
	inline bool HaveValidEnd();

	// The current batch of triangles
	ListOfTriangleLists									m_triList;

	// Build the data
	int NextLeaf();
	FullOctTreeNode *LeafNode(Vector3CRef point);
	void AddAllTriangles(const FullOctTreeNode &node);
};

// Include our inlines!
#include "collide/octtree.hpp"

#endif
