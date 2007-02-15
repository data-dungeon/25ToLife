////////////////////////////////////////////////////////////////////////////
//
// Stats
//
////////////////////////////////////////////////////////////////////////////
#ifndef STATS_H
#define STATS_H

class Stats
{
public:
	Stats(void) { Clear(); }
	~Stats(void) {}

	// Clear all the stats
	void Clear(void);

	// the stats themselves (too lazy to make methods for access...)

	// Current instance
	uint instancesProcessed;
	uint totalInstances;

	// Database stats
	uint triangles;
	uint degenerates;

	// Tree stats
	uint nodes;
	uint maxDepth;
	uint pruned;

	uint rootTriangles;
	uint maxTrianglesInNode;
	uint maxTrianglesInNodeDepth;

	float smallestNode;
};

extern Stats g_stats;

#endif
