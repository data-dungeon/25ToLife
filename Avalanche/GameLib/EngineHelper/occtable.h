/***************************************************************************/
// A file loaded occlusion table
/***************************************************************************/
#ifndef OCCTABLE_H
#define OCCTABLE_H

#include "audio/math/occlude.h"

class SoundOcclusionTable
{
public:
	// These don't do much, as they can't error report
	SoundOcclusionTable();

	// Clear it
	void Clear();

	// Load it!
	bool Load(const char *pathFile);

	// Get at the data
	const SoundOcclusionTest::MaterialProperty *PropertyArray()
		{ return	m_prop; }
	int Properties()
		{ return 256; }

private:
	// THe properties
	SoundOcclusionTest::MaterialProperty	m_prop[256];

	// Utilities
	char *SkipSpaces(char *parse, int &len);
	char *SkipTo(char *parse, int &len, char separator);
};

#endif
