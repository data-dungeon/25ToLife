//------------------------------------------------------------------------------
// PatchExtractor.h
//
// Source for game patching library.
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

// Enable/disable our debug print statements
//#define PATCH_PRINT_ENABLED

#include "PatchExtractor.h"

//------------------------------------------------------------------------------
// Initialize a patch extractor. The patch data buffer must already contain
// decrypted patch data.
bool PatchExtractor::Init(char* patchData, int dataSize)
{
	// Build a fast table of contents for the patch
	m_TOC.Build(patchData, dataSize);

	// Verify the integrity of the patch data
	if (!m_TOC.Verify(m_version))
	{
		PATCH_PRINT("Verify of patch integrity failed, setting error\n");
		m_state = STATE_BUFFER_FAILED;
	}
	else
	{
		m_state = STATE_BUFFER_GOOD;
	}

	return m_state == STATE_BUFFER_GOOD;
}

//------------------------------------------------------------------------------
// QueryVersion() returns the current version number as specified by the
// prepared patch currently in memory. Returns zero on error or if there is
// no patch available.
int PatchExtractor::QueryVersion(void)
{
	// Return the version number we got when verifying the patch
	return (m_state == STATE_BUFFER_GOOD) ? m_version : 0;
}

//------------------------------------------------------------------------------
// PatchExists looks at the contents of the downloaded patch and determines whether
// the named module is modified by the patch. If so, the size of the newly patched 
// module is returned in reqSize; that size is how big a buffer LoadModule will require.
//
// Returns true if the patch exists, false if the patch does not exist or if there
// was an error.
bool PatchExtractor::PatchExists(const char* name, int& reqSize)
{
	// Default the size to zero
	reqSize = 0;

	// If we don't need any updates, don't bother searching.
	if (m_state != STATE_BUFFER_GOOD)
		return false;

	// OK, search the current patch in memory to see if the given module is there.
	m_entry.Clear();
	ContentTable::EntryStatus found = m_TOC.FindEntry(name, m_entry);
	if (found == ContentTable::ENTRY_FOUND)
	{
		reqSize = m_entry.CalculateFinalSize();
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------
// PatchItem finds the named item (file) in the downloaded patch, and patches it
// into the given buffer.
bool PatchExtractor::PatchItem(const char* name, char* toBuf, int toSize)
{
	if (m_state != STATE_BUFFER_GOOD)
		return false;

	m_entry.Clear();
	ContentTable::EntryStatus found = m_TOC.FindEntry(name, m_entry);
	if (found != ContentTable::ENTRY_FOUND)
		return false;

	// OK, we found the patch, so try to apply it
	return PatchItem(toBuf, toSize);
}

//------------------------------------------------------------------------------
// Patch the last found item into the buffer
bool PatchExtractor::PatchItem(char* toBuf, int toSize)
{
	int finalSize = m_entry.CalculateFinalSize();

	// If the final size is zero, that means no item was found
	if (finalSize <= 0)
	{
		PATCH_PRINT("PatchExtractor::PatchItem -- No item was found; better check your function returns!\n");
		return false;
	}

	// Make sure our buffer is big enough before we begin (?? Or just fail later...)
	if (finalSize > toSize)
	{
		PATCH_PRINT("PatchExtractor::PatchItem -- Buffer of %d bytes is not big enough!!\n", toSize);
		return false;
	}

	// Patch the module into the buffer
	int stat = m_entry.ApplyPatch(m_TOC.GetBuf(), m_TOC.GetLen(), toBuf, toSize);
	
	// Clear our entry to avoid accidentally reapplying the same patch again
	m_entry.Clear();
	
	return (stat == 0);
}
