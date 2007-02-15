////////////////////////////////////////////////////////////////////////////
//
// TriangleStrip
//
// Class to handle the triangle strip data for the meshes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: triangleStrip.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/27/02    Time: 2:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Triangle Strip classes
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_TRIANGLE_STRIP_H
#define TUPPER3D_TRIANGLE_STRIP_H

class TupTriangleStripPool
{
	// Private Default constructor so no one can use it
	TupTriangleStripPool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pTriangleStripPool; // the pool that knows about all the child atoms

public:
	TupTriangleStripPool(TupperwareAggregate *pObject); // use this one
	~TupTriangleStripPool();

// add methods
	int AddTriangleStrip(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddTriangleStrip(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetTriangleStrip(int index); // return the aggregate using a integer index
	int GetNumTriangleStrips(void);
};

class TupTriangleStrip
{
	// Private Default constructor so no one can use it
	TupTriangleStrip(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		FACE_INDICES,
		FACE_POINT_INDICES,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	TupTriangleStrip(TupperwareAggregate *pObject);

	void BuildAtomCache(void);

// add methods
	TupperwareList *AddFaceIndices(const int *pFaceIndices,int numFaceIndices);
	TupperwareList *AddFacePointIndices(const int *pFacePointIndices,int numFacePointIndices);

// get methods
	TupperwareList *GetFaceIndices(int **pFaceIndices,int &numFaceIndices);
	TupperwareList *GetFacePointIndices(int **pFacePointIndices,int &numFacePointIndices);

private:
// add methods
	TupperwareList *AddIntList(const int *pInts,int numInts,TupTriangleStrip::ChildrenAtoms index);

// get methods
	TupperwareList *GetIntList(int **pInts,int &numInts,TupTriangleStrip::ChildrenAtoms index);
};

#endif
