///////////////////////////////////////////////////////////////////////////////
// Object databases now are packed together by layers
///////////////////////////////////////////////////////////////////////////////
#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/ObjectLayer.h"

// Gamelib includes //
#include "collide/ObjectQT.h"

#if 0

// Engine includes //
#include "Layers/LayersStruct.h"
#include "Layers/Assert/Assert.h"
#include "Game/Database/DBFile.h"

#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ObjectLayerList::~ObjectLayerList(void)
{
}

///////////////////////////////////////////////////////////////////////////////
// Methods for creating the tree
///////////////////////////////////////////////////////////////////////////////
ObjectLayerList *ObjectLayerList::Create(
t_Handle dblHandle,
unsigned int &layerMask)
{
	// Get the data from the dbl
	int dataSize;
	ODBHeader *hdr = (ODBHeader *)dblGetChunkData(DB_ODB,
			DEFAULT_CHUNK_KEY_STRING, dblHandle, &dataSize);

	// Did we get a chunk
	if (!hdr || dataSize == 0)
		return NULL;

	// Make sure the version is kosher
	ASSERT(hdr->version == CURRENT_ODB_VERSION);

	// Create us
	ObjectLayerList *layerList = new ObjectLayerList(layerMask, *hdr,
			(void *)((unsigned int)hdr + sizeof(ODBHeader)));
	ASSERT(layerList);

	// It's loaded (we are now fat)
	return layerList;
}

///////////////////////////////////////////////////////////////////////////////
// Fix Disk load
///////////////////////////////////////////////////////////////////////////////
void ObjectLayerList::FixDiskLoad(void* data)
{
	ODBHeader* hdr = (ODBHeader*) data;
	ASSERT_PTR(hdr);

	// Do the byte swap for GCN
#ifdef GCN
	hdr->EndianSwap();
#endif

	// Make sure the version is kosher
	ASSERT(hdr->version == CURRENT_ODB_VERSION);

	// Now setup everything
	unsigned int ptr = (unsigned int)hdr + sizeof(ODBHeader);
	for (int i = 0; i < MAX_OBJECT_LAYERS; i++)
	{
		if (hdr->layerSize[i] != 0)
		{
			// Fixup this layer
			ObjectQuadTreeNode* root = (ObjectQuadTreeNode*) ptr;
			root->Fixup();

			// advance to next data
			ptr += hdr->layerSize[i];
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Build a list of Objects
///////////////////////////////////////////////////////////////////////////////
void ObjectLayerList::BuildList(
CCollisionStackArray<CStaticCollisionObject*>* io_list,
Vector3CRef i_start,
Vector3CRef i_chord) const
{
	for (int i = 0; i < layers; i++)
	{
		if (layerMask & layer[i].mask)
			layer[i].root->BuildList(io_list, i_start, i_chord);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Build a list of Objects
///////////////////////////////////////////////////////////////////////////////
void ObjectLayerList::BuildList(
CCollisionStackArray<CStaticCollisionObject*>* io_list,
const XZRect &i_rect) const
{
	for (int i = 0; i < layers; i++)
	{
		if (layerMask & layer[i].mask)
			layer[i].root->BuildList(io_list, i_rect);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Private so they have to call create
///////////////////////////////////////////////////////////////////////////////
ObjectLayerList::ObjectLayerList(
unsigned int &mask) :
layerMask(mask)
{
	layers = 0;
}

///////////////////////////////////////////////////////////////////////////////
// Full create
///////////////////////////////////////////////////////////////////////////////
ObjectLayerList::ObjectLayerList(
unsigned int &mask,
ODBHeader &hdr,
void *data) :
layerMask(mask)
{
	layers = 0;

	// Now setup everything
	unsigned int ptr = (unsigned int)data;
	for (int i = 0; i < MAX_OBJECT_LAYERS; i++)
	{
		if (hdr.layerSize[i] != 0)
		{
			// Setup this layer
			layer[layers].root = (ObjectQuadTreeNode *)ptr;
			layer[layers].mask = 1 << i;

			// Initialize the tree
#ifdef _DEBUG
			layer[layers].root->Validate();
#endif

			// Print out tree if it seems screwed up.
#ifdef OBJECT_QT_WRITE_FILE
			if (layers == 0)
			{
				FILE *fp = fopen("objectqt.txt", "w");
				layer[layers].root->PrintTree(0, "", fp);
				fclose(fp);
			}
#endif

			// advance to next data
			ptr += hdr.layerSize[i];

			// We have one more layer
			++layers;
		}
	}
}
