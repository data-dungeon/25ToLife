//------------------------------------------------------------------------------
// ContentTable_win.cpp
//
// Windows-specific source for game patching library table of contents class.
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#include "ContentTable.h"
#include "PatcherCommon/PatchHelper.h"

//------------------------------------------------------------------------------
// Don't need to mangle the name for windows, just copy it.
bool TOCEntry::_MakeFileName(char* dst, const char* src, int dstLen, int cpyLen)
{
	if (cpyLen <= 0)
		cpyLen = dstLen;

	int len = safestrlcpy(dst, src, cpyLen);
	if (len >= cpyLen)
		return false;

	int i;
	for (i = len - 1; i >= 0; i--)
	{
		if (dst[i] == '/') dst[i] = '\\';
	}
	return true;
}
