/******************* includes ****************************/

#include "Game/GamePCH.h"





// ---------------------------------------------------------------------------
//	FUNCTION: Load

bool FontInfoFile::Load(char *filename)
{
	/* Load in the entire file into a block */
	s32 size;
	bool ret = Media::LoadStatic(filename, &d_fontInfo, sizeof(d_fontInfo), &size, NULL);

	// check that file is not corrupt.
	ASSERT(size == sizeof(FontInfo));

	return ret;
}
