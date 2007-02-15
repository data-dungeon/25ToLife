//***************************************************************************
// Stats tracking structure
//***************************************************************************
#ifndef STATS_H
#define STATS_H

#include <toolhelper\dynstr.h>

class Stats
{
public:
	Stats(void) { Clear(); }
	~Stats(void) {}

	// Clear all the stats
	void Clear(void);

	void SetFile(char *fileName)
		{ currentFile.Cpy(fileName); }

	void SetStatus(char *newStatus);
	void ClearStatus(void);

	// the stats themselves (too lazy to make methods for access...)
	char *startDir;
	char *outputFile;
	int totalFiles;
	int totalSize;

	DynString currentFile;
	int onFile;
	bool done;

	DynString status;
	float percentageDone;
};

extern Stats *g_stats;

#endif
