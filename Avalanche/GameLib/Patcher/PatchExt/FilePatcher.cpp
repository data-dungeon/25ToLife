//------------------------------------------------------------------------------
// FilePatcher.cpp
//
// Windows file patching utility. 
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#include <windows.h>
#include "FilePatcher.h"

//------------------------------------------------------------------------------
// Helper classes
//------------------------------------------------------------------------------
class TmpBuf {
public:
	TmpBuf(int size) {m_buf = (size > 0) ? new char [size] : 0;}
	~TmpBuf() {if (m_buf) delete [] m_buf;}
	char* Ptr(void) {return m_buf;}
private:
	char* m_buf;
};

//------------------------------------------------------------------------------
class PushDir {
public:
	PushDir(const char* dir) {
		GetCurrentDirectory(MAX_PATH, m_prevDir);
		m_good = (SetCurrentDirectory(dir) != 0);
	}
	~PushDir() {
		if (m_good)
			SetCurrentDirectory(m_prevDir);
	}
	bool IsGood(void) const {return m_good;}
private:
	char	m_prevDir[MAX_PATH];
	bool	m_good;
};

//------------------------------------------------------------------------------
// Initialize this FilePatcher
bool FilePatcher::Init(const std::string& dstDir, char* patchData, int dataSize)
{
	m_dstDir = dstDir;
	m_started = false;
	return PatchExtractor::Init(patchData, dataSize);
}

//------------------------------------------------------------------------------
// Find the next file to be patched and return info about it (for progress
// dialogs, etc.)
int FilePatcher::FindNextFile(std::string& filename)
{
	if (m_state != STATE_BUFFER_GOOD)
		return -1;

	m_entry.Clear();

	ContentTable::EntryStatus found = m_started ? m_TOC.GetNextEntry(m_entry) : m_TOC.GetFirstEntry(m_entry);
	if (found != ContentTable::ENTRY_FOUND)
	{
		return (found == ContentTable::ENTRY_NOT_FOUND) ? 0 : -1;
	}

	// We have started the crawl...
	m_started = true;

	// Return the filename of the file to be patched
	filename = m_entry.GetName();
	return 1;
}

//------------------------------------------------------------------------------
// Patch the next file in the patch. Returns 1 on success, zero if no more files
// exist, and a negative error code on failure.
int FilePatcher::PatchCurrentFile(void)
{
	// Got a good entry, let's try to patch it.
	int outsize = m_entry.CalculateFinalSize();

	// Set our working dir to the destination
	PushDir setDir(m_dstDir.c_str());
	if (!setDir.IsGood())
		return -1;

	// Make a full path out of our filename
	std::string path = m_dstDir + "\\" + m_entry.GetName();

	// Allocate an buffer
	TmpBuf buf(outsize);
	if (!buf.Ptr())
		return -1;

	// Patch into the buffer
	if (!PatchItem(buf.Ptr(), outsize))
		return -1;

	// Write out the new file
	FILE* f = fopen(path.c_str(), "wb");

	// If the open failed, we try to write out to a different name and shuffle
	// the files with a move (so we can overwrite an open file)
	bool shuffle = (f == NULL);
	std::string tmpfile;
	if (shuffle)
	{
		tmpfile = path + "fptmp";
		f = fopen(tmpfile.c_str(), "wb");

		// If this file open failed, something else is wrong, like a bad path
		if (!f)
			return -1;
	}

	bool good = (fwrite(buf.Ptr(), 1, outsize, f) == outsize);
	fclose(f);

	if (shuffle && good)
	{
		// Shuffle the files around
		std::string delfile = path + "fpdel";
		if (MoveFileEx(path.c_str(), delfile.c_str(), MOVEFILE_REPLACE_EXISTING) &&
			MoveFileEx(tmpfile.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING))
		{
			// Delete the del file on next reboot
			MoveFileEx(delfile.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
		}
	}

	// Done
	return good ? 1 : -1;
}
