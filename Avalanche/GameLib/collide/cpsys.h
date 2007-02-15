//***************************************************************************
// The collision partition system gives information on how all the world
// partitions are positioned in the world.
//***************************************************************************
#ifndef CPSYS_H
#define CPSYS_H

#include "collide/cube.h"

// Current version number
#define CPS_VERSION	0x00030002

// Forwards
class CollideCollideLayerList;

#if !defined(PS2) || !defined(SN)
#pragma pack(1)
#endif
// Per-partition information
class CollidePartition
{
public:
	AABB		bounds;				// AABB for this partition

	enum
	{
		FLAGS_EMPTY = 0x01		// Set is empty, there is no .cdb
	};
	int 		flags;

#ifdef GCN
	// GCN the bain of humanity
	inline void EndianSwap(void)
	{
		bounds.EndianSwap();
		ByteSwap( flags);
	}
#endif
};
#if !defined(PS2) || !defined(SN)
#pragma pack()
#endif

#if !defined(PS2) || !defined(SN)
#pragma pack(1)
#endif
// The overall partition information
class CollidePartitionSystem
{
public:
	// Current version
	unsigned int 	version;

	// World bounds
	AABB				bounds;

	// Number of partitions
	int 				partitions;

	// Partition data follows this structure
	inline CollidePartition *PartitionArray(void)
		{ return((CollidePartition *)(this + 1)); }
	inline CollidePartition &Partition(int index)
		{ return(PartitionArray()[index]); }

#ifdef GCN
	// GCN the bain of humanity
	inline void EndianSwap(void)
	{
		ByteSwap( version);
		bounds.EndianSwap( );
		ByteSwap( partitions);
		CollidePartition *tp = PartitionArray();
		for (int p = 0; p < partitions; p++)
			tp[p].EndianSwap();
	}
#endif
};
#if !defined(PS2) || !defined(SN)
#pragma pack()
#endif

#endif

