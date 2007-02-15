/***************************************************************************/
// Make an obb set from an instance
/***************************************************************************/
#ifndef MAKEOBB_H
#define MAKEOBB_H

// Change these if the widths of the bit fields change
#define MAX_OBBSPERINSTANCE		64

class MakeOBB
{
	TupNode &m_tupNode;
	TupTriMesh &m_tupTriMesh;

	// vertex colors
	ts_bRGBA *m_pColorArray;
	int *m_pColorIndices;
	int m_numColorFaces;

	// face attributes
	TupArray<FaceCollisionManager::Attributes> &m_faceAttributeArray;
	TupArray<int> &m_faceAttributeIndices;

	DynList<OBBStaticInstance>	m_obbList;

public:
	// Make an instance into an OBB
	MakeOBB(TupNode &tupNode,TupTriMesh &tupMesh,TupperwareAggregate *pVertexColorMapAgg,TupArray<FaceCollisionManager::Attributes> &faceAttributeArray,TupArray<int> &faceAttributeIndices);
	~MakeOBB(void);

	// Build them, return how many we have
	// shrinkage is what to multiply the size of the box by.  1.0 does nothing.
	int Build(const Vector3 *shrinkage = NULL, float minDimension = 0.0f);

	// What is the count?
	int Count(void) { return(m_obbList.Count()); }

	// Get an obb (you own it now, it is gone from this list)
	// Returns NULL when there are no more
	OBBStaticInstance *Get(void);

private:

	// Strip the orientation from this 4x4
	static void StripOrientation(Graphics4x4 &localToWorld, DirCos3x3 &orientation);

	// Simple vertex
	struct Vertex
	{
		Vector3	m_vertex;
		u8 m_surfaceType;
		u8 m_obbValue;
		ts_bRGBA m_color;
		bool operator==(const Vertex &src) const
		{
			return (m_vertex==src.m_vertex && 
					m_color.R==src.m_color.R && 
					m_color.G==src.m_color.G && 
					m_color.B==src.m_color.B && 
					m_color.A==src.m_color.A && 
					m_surfaceType==src.m_surfaceType && 
					m_obbValue==src.m_obbValue);
		}
	};

	struct OBBType
	{
		u8 m_surfaceType;
		u8 m_obbValue;
	};

	int CountVertices(void);

	// Modifies verts to the correct number
	Vertex *BuildVertexArray(int &numVerts,Graphics4x4 &localToWorld);

	static int CountOBBs(Vertex *vertexArray, int numVerts, OBBType *obbTypeArray);

	// Does this vertex match this type?
	inline static bool Match(const OBBType &type, const Vertex &v) { 
		return(type.m_surfaceType == v.m_surfaceType &&
			   type.m_obbValue == v.m_obbValue); }

	// Find this OBBtype in the array (-1 it's not there)
	static int FindOBBType(OBBType *obbTypeArray, int numObbTypes, u8 material, u8 obbIndex);

	// other utilties
	static bool ComputeCentroid(Vertex *vertexArray, int numVerts, Vector3 &centroid, OBBType &obbType);
	static int UntransformVertices(Vertex *vertexArray, int numVerts, DirCos3x3 &orientation, Vector3 &centroid, OBBType &type, Vector3 *untransformedVertex);
	static void ComputeAABB(Vector3 *untransformedVertexArray, int numVerts, AABB &aabb);
	static u16 ComputeColor(Vertex *vertexArray, int numVerts, OBBType &obbType);
};

#endif
