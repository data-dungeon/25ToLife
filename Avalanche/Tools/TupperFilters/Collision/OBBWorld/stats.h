//***************************************************************************
// Stats tracking structure
//***************************************************************************
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
	float minQuadrantSize;
	float shrink;
	float minDimension;
	uint totalInstances;

	uint instanceObbs;
	uint instancesProcessed;

	uint quadLeaves;
	uint rootObbs;
	uint obbs;
	uint redundantObbs;
	uint maxDepth;

	float percentageDone;
};

extern Stats *g_stats;

#endif
