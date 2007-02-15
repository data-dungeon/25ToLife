//------------------------------------------------------------------------------
// ContentTable_ps2.cpp
//
// PS2-specific source for game patching library table of contents class.
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#include "ContentTable.h"
#include "PatcherCommon/PatchHelper.h"

//------------------------------------------------------------------------------
bool TOCEntry::_MakeFileName(char* dst, const char* src, int dstLen, int cpyLen)
{
#ifdef CDROM
	if (cpyLen <= 0)
		cpyLen = dstLen - 8;
	if ((safestrlcpy(dst, "cdrom0:\\", dstLen) >= dstLen) ||
		(safestrlcpy(&dst[8], src, cpyLen) >= cpyLen))
		return false;

	// Replace forward slash with back-slash (necessary?)
	int i;
	for (i = 8; dst[i]; i++)
	{
		if (dst[i] == '/')
			dst[i] = '\\';
		// Not sure if toupper is needed for CDROM reading. Let's try without,
		// because the system toupper causes SN link problems so we would have
		// to roll our own.
		//else
		//	dst[i] = toupper(dst[i]);
	}

	return (safestrlcat(dst, ";1", dstLen) < dstLen);
#else
	if (cpyLen <= 0)
		cpyLen = dstLen - 6;
	return ((safestrlcpy(dst, "host0:", dstLen) < dstLen) &&
		(safestrlcpy(&dst[6], src, cpyLen) < cpyLen));
#endif
}
