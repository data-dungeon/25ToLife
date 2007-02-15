////////////////////////////////////////////////////////////////////////////
//
// Stats
//
////////////////////////////////////////////////////////////////////////////
#include "Pch.h"
#include "stats.h"

// Make a global one for everyone
Stats g_stats;

//***************************************************************************
//***************************************************************************
void Stats::Clear(void)
{
	instancesProcessed = 0;
	totalInstances = 0;

	triangles = 0;
	degenerates = 0;

	nodes = 0;
	maxDepth = 0;
	pruned = 0;

	rootTriangles = 0;
	maxTrianglesInNode = 0;
	maxTrianglesInNodeDepth = 0;

	smallestNode = -1.0f;
}

