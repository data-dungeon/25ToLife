///////////////////////////////////////////////////////////////////////////////
// Terrain collide databases are now packed together by layers
///////////////////////////////////////////////////////////////////////////////
#include "collide/CollidePCH.h"
#include "collide/tclayer.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TerrainCollideLayer::TerrainCollideLayer()
{
	m_mask = 0;
	m_hdr = NULL;
#ifdef TERRAINTRIANGLE_LEAN
	m_vertex = NULL;
	m_vertexGroup = 0;
#endif
	m_triangle = NULL;
	m_root = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Initialize
///////////////////////////////////////////////////////////////////////////////
void TerrainCollideLayer::Initialize(
unsigned int mask,
void *data)
{
	ASSERT(mask != 0);
	ASSERT(data);

	// Save the mask
	m_mask = mask;

	// Setup all the pointers into the data
	m_hdr = (OctTreeHeader *)data;

	// Do we have vertex tables?
#ifdef TERRAINTRIANGLE_LEAN
	// Make sure the format is right
	ASSERT(m_hdr->version == (OTH_VERSION | OTH_VERTEXARRAY));
	ASSERT(m_hdr->vertices > 0);
	
	// Setup the pointers
	m_vertex = (Vector3Packed *)(m_hdr + 1);
	m_triangle = (TerrainTriangle *)((u8 *)(m_hdr + 1) + m_hdr->vertices * sizeof(Vector3Packed));
#else
	// Make sure the format is right
	ASSERT(m_hdr->version == OTH_VERSION);
	ASSERT(m_hdr->vertices == 0);

	// Setup the triangle array
	m_triangle = (TerrainTriangle *)(m_hdr + 1);
#endif

	// Point at the tree
	m_root = (OctTreeNode *)&m_triangle[Triangles()];

	// Setup the test ID stuff
	m_testID.Initialize(m_triangle, Triangles());

	// Setup the suspect stuff
	m_suspect.Initialize(m_triangle, Triangles());
}

///////////////////////////////////////////////////////////////////////////////
// Initialize off disk
///////////////////////////////////////////////////////////////////////////////
void TerrainCollideLayer::FixDiskLoad(void* data)
{
	ASSERT_PTR(data);

	// Setup all the pointers into the data
	OctTreeHeader* hdr = (OctTreeHeader*) data;
#ifdef GCN
	hdr->EndianSwap();
#endif

	// Do we have vertex tables?
#ifdef TERRAINTRIANGLE_LEAN
	// Make sure the format is right
	ASSERT(hdr->version == (OTH_VERSION | OTH_VERTEXARRAY));
	ASSERT(hdr->vertices > 0);
	
	// Setup the pointers
	Vector3Packed*   vertex   = (Vector3Packed*) (hdr + 1);
	TerrainTriangle* triangle = (TerrainTriangle*) ((u8 *)(hdr + 1) + hdr->vertices * sizeof(Vector3Packed));

#ifdef GCN
	for( int vtx = 0; vtx < hdr->vertices; vtx++)
	{
		vertex[vtx].EndianSwap();
	}
#endif

	// Allocate a vertex group
	u8 vertexGroup = TerrainTriangleLean::AddVertexGroup(vertex);

	// fixup the triangles
	for (int t = 0; t < (int)hdr->triangles; t++)
		triangle[t].DiskFixup(vertexGroup);
#else
	// Make sure the format is right
	ASSERT(hdr->version == OTH_VERSION);
	ASSERT(hdr->vertices == 0);

	// Setup the triangle array
	TerrainTriangle* triangle = (TerrainTriangle*) (hdr + 1);

	// fixup the triangles
	for (int t = 0; t < (int)hdr->triangles; t++)
		triangle[t].DiskFixup();
#endif

	// Point at the tree
	OctTreeNode* root = (OctTreeNode*) &triangle[hdr->triangles];

	// Fixup everything
	root->DiskFixup(data);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TerrainCollideLayer::Terminate()
{
#ifdef TERRAINTRIANGLE_LEAN
	TerrainTriangleLean::RemoveVertexGroup(m_vertex);
#endif
}
