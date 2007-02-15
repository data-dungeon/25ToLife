////////////////////////////////////////////////////////////////////////////
//
// TriMeshMap
//
// Class to handle the map channels for the triangle meshes
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_TRI_MESHMAP_H
#define TUPPER3D_TRI_MESHMAP_H

class TupTriMeshMap
{
public:
	enum MapChannels
	{
		MAP_CHANNEL_ALPHA = -2,
		MAP_CHANNEL_ILLUMINATION		= -1,
		MAP_CHANNEL_VERTEX_COLOR		= 0,
		MAP_CHANNEL_TEXTURE_UV_START	= 1,

		MAP_CHANNEL_MIN = -2,
	};

	enum MapFlags
	{
		MESHMAP_FLAG_USED				= 0x0001,
		MESHMAP_FLAG_TEXTURE			= 0x0002,
		MESHMAP_FLAG_VERTEXCOLOR	= 0x0004,
		MESHMAP_FLAG_USER				= 0x0100,
	};

private:
	// Private Default constructor so no one can use it
	TupTriMeshMap(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		MAP_NAME,
		MAP_FLAGS,
		MAP_VERTS,
		MAP_INDICES,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	TupTriMeshMap(TupperwareAggregate *pObject);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddMapName(const char *name);
	TupperwareScalar *AddMapFlags(int mapFlags);
	TupperwareList *AddMapVerts(const float *pVerts,int numVerts);
	TupperwareList *AddMapIndices(const int *pVerts,int numFaces);

// get methods
	TupperwareScalar *GetMapName(char **name);
	int GetMapChannel(void);
	TupperwareScalar *GetMapFlags(int &mapFlags);
	TupperwareList *GetMapVerts(float **pVerts,int &numVerts);
	TupperwareList *GetMapIndices(int **pIndices,int &numFaces);

private:
// add methods
	TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupTriMeshMap::ChildrenAtoms index);
	TupperwareList *AddIntList(const int *pInts,int numInts,TupTriMeshMap::ChildrenAtoms index);

// get methods
	TupperwareList *GetFloatList(float **pfloats,int &numFloats,TupTriMeshMap::ChildrenAtoms index);
	TupperwareList *GetIntList(int **pInts,int &numInts,TupTriMeshMap::ChildrenAtoms index);
};

#endif
