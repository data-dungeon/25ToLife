//------------------------------------------------------------------------------
// FilePatcher.h
//
// Windows file patching utility. 
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#ifndef __FILEPATCHER_H__
#define __FILEPATCHER_H__ 1

#include "PatcherCommon/patchplatform.h"
#include "ContentTable.h"
#include "PatchExtractor.h"
#include <string>

class FilePatcher : public PatchExtractor {
public:
	FilePatcher() : m_started(false) {}
	FilePatcher(const std::string& dstDir, char* patchData, int dataSize) {Init(dstDir, patchData, dataSize);}
	bool Init(const std::string& dstDir, char* patchData, int dataSize);

	// Find the next file to be patched and return info about it (for progress
	// dialogs, etc.)
	int FindNextFile(std::string& filename);

	// Patch the next file in the patch. Returns 1 on success, zero if no more files
	// exist, and a negative error code on failure.
	int PatchCurrentFile(void);

private:
	std::string			m_dstDir;						// Directory into which we patch
	bool				m_started;						// True if we have already patched something
};

#endif // __FILEPATCHER_H__
