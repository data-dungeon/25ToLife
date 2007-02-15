///////////////////////////////////////////////////////////////////////////////
// Object databases now are packed together by layers
///////////////////////////////////////////////////////////////////////////////
#ifndef OBJECT_LAYER_H
#define OBJECT_LAYER_H

#include "Math/Vector.h"
#include "Layers/LayersStruct.h"

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

class XZRect;
class ObjectQuadTreeNode;
class CStaticCollisionObject;
template<class T> class CCollisionStackArray;

#define CURRENT_ODB_VERSION	0x0101
#define MAX_OBJECT_LAYERS			32

#if !defined(PS2) || !defined(SN)
#pragma pack(1)
#endif
class ODBHeader
{
public:
	unsigned int	version;
	unsigned int	layerSize[MAX_OBJECT_LAYERS]; 	// Zero means no layer present

	// Must be 32-byte algined
	char				pad[28];

#ifdef GCN
	void EndianSwap(void)
	{
		ByteSwap(version);
		for (int i = 0; i < MAX_OBJECT_LAYERS; i++)
			ByteSwap(layerSize[i]);
	}
#endif
};
#if !defined(PS2) || !defined(SN)
#pragma pack()
#endif

class ObjectLayerList
{
public:
	~ObjectLayerList(void);

	// Methods for creating the tree
	static ObjectLayerList *Create(t_Handle dblHandle, unsigned int &layerMask);
	static void FixDiskLoad(void* data);

	// Build a list of objects
	void BuildList(CCollisionStackArray<CStaticCollisionObject*>* io_list,
			Vector3CRef i_start, Vector3CRef i_chord) const;
	void BuildList(CCollisionStackArray<CStaticCollisionObject*>* io_list,
			const XZRect &i_rect) const;

private:
	// Private so they have to call create
	ObjectLayerList(unsigned int &mask);
	ObjectLayerList(unsigned int &mask, ODBHeader &hdr, void *data);

	// The current mask for testing
	unsigned int				&layerMask;

	struct Layer
	{
		unsigned int			mask;
		ObjectQuadTreeNode	*root;
	};

	// An array of layer's and their mask's
	// This array is not SPARSE, it is packed
	Layer			layer[MAX_OBJECT_LAYERS];
	int			layers;

public:
	// We appear in a list!
	ObjectLayerList		*prev, *next;
};

#endif
