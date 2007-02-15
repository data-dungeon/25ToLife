//***************************************************************************
// Stats tracking structure
//***************************************************************************
#ifndef STATS_H
#define STATS_H

#include <Platform/BaseType.h>
#include <toolhelper/dynstr.h>

class Stats
{
public:
	Stats(void) { Clear(); }
	~Stats(void) {}

	// Clear all the stats
	void Clear(void);

	void SetInstance(char *instanceName)
		{ currentInstance.Cpy(instanceName); }

	void SetStatus(char *newStatus);
	void ClearStatus(void);

	// the stats themselves (too lazy to make methods for access...)
	char *inputFile;
	char *outputFile;
	uint totalInstances;

	DynString currentInstance;
	uint onInstance;
	bool done;

	DynString status;
	float percentageDone;
};

extern Stats *g_stats;

#endif
