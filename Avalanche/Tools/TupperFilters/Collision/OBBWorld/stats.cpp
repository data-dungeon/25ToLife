////////////////////////////////////////////////////////////////////////////
//
// stats
//
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: stats.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 3:06p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Memory Problems.
 * Simple Progress Bar Stuff
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 1:32p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * OBBWorld is now online... Just needs to get the node and face
 * attributes
 */

#include "Pch.h"
#include "stats.h"

// Make a global one for everyone
Stats *g_stats;

//***************************************************************************
//***************************************************************************
void Stats::Clear(void)
{
	minQuadrantSize = 0.0f;
	totalInstances = 0;

	instanceObbs = 0;
	instancesProcessed = 0;

	quadLeaves = 0;
	rootObbs = 0;
	obbs = 0;
	redundantObbs = 0;
	maxDepth = 0;

	percentageDone = 0.0f;
}

