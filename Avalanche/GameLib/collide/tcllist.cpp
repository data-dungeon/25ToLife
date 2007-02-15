///////////////////////////////////////////////////////////////////////////////
// Terrain collide databases are now packed together by layers
///////////////////////////////////////////////////////////////////////////////
#include "collide/CollidePCH.h"
#include "collide/tcllist.h"

///////////////////////////////////////////////////////////////////////////////
// Load in a full TerrainCollide and all the TerrainSectors from disk
///////////////////////////////////////////////////////////////////////////////
TerrainCollideLayerList *TerrainCollideLayerList::Create(
t_Handle dblHandle,
unsigned int &layerMask)
{
	// First try to get ptr to the terrain chunk from the DBL.
	// It may exist but be empty. typically, if this succeeds the data was loaded
	// as part of an uber file
	int dataSize;
	CDBHeader *hdr = (CDBHeader *)dblGetChunkData(DB_CDB, DEFAULT_CHUNK_KEY_STRING,
																	dblHandle, &dataSize);
	if (!hdr || dataSize == 0)
		return NULL;

	// Do the byte swap for GCN
#ifdef GCN
	hdr->EndianSwap();
#endif

	// Create us
	TerrainCollideLayerList *layerList = new TerrainCollideLayerList(layerMask,
																							*hdr, (void *)((unsigned int)hdr + sizeof(CDBHeader)),
																							dblHandle);
	ASSERT(layerList);

	// It's loaded (we are now fat)
	return layerList;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TerrainCollideLayerList::FixDiskLoad(void* data)
{
	CDBHeader* hdr = (CDBHeader*) data;
	ASSERT_PTR(hdr);

	// Do the byte swap for GCN
#ifdef GCN
	hdr->EndianSwap();
#endif

	// Now setup everything
	unsigned int ptr = (unsigned int)hdr + sizeof(CDBHeader);
	for (int i = 0; i < MAX_COLLIDE_LAYERS; i++)
	{
		if (hdr->layerSize[i] != 0)
		{
			TerrainCollideLayer::FixDiskLoad( (TerrainCollideLayer*) ptr );

			// advance to next data
			ptr += hdr->layerSize[i];
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TerrainCollideLayerList::TerrainCollideLayerList(
unsigned int &layerMask) :
m_layerMask(layerMask)
{
	m_layers = 0;
}

///////////////////////////////////////////////////////////////////////////////
// Full create
///////////////////////////////////////////////////////////////////////////////
TerrainCollideLayerList::TerrainCollideLayerList(
unsigned int &layerMask,
CDBHeader &hdr,
void *data,
t_Handle dblHandle) :
m_layerMask(layerMask)
{
	m_layers = 0;

	// Now setup everything
	unsigned int ptr = (unsigned int)data;
	for (int i = 0; i < MAX_COLLIDE_LAYERS; i++)
	{
		if (hdr.layerSize[i] != 0)
		{
			// Setup this layer
			m_layer[m_layers].Initialize(1 << i, (void *)ptr);

			// advance to next data
			ptr += hdr.layerSize[i];

			// We have one more layer
			++m_layers;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TerrainCollideLayerList::~TerrainCollideLayerList()
{
	for (int l = 0; l < m_layers; l++)
		m_layer[l].Terminate();
}

///////////////////////////////////////////////////////////////////////////////
// What the hell does this do?  It's not supported
///////////////////////////////////////////////////////////////////////////////
void TerrainCollideLayerList::Validate(
TriangleSuspect::STriangle *suspectList,
u32 &numSuspects,
u32 maxSuspects)
{
	for (int layerIndex = 0; layerIndex < m_layers; layerIndex++)
	{
		if (m_layer[layerIndex].MatchMask(m_layerMask))
			m_layer[layerIndex].Suspect().Validate(suspectList, numSuspects, maxSuspects);
	}
}
