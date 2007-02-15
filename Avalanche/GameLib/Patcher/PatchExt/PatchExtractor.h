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

#ifndef __PATCHEXTRACTOR_H__
#define __PATCHEXTRACTOR_H__ 1

#include "PatcherCommon/patchplatform.h"
#include "ContentTable.h"

class PatchExtractor {
protected:
	enum ExtractorState {
		STATE_BUFFER_GOOD,		// The buffer is good and verified
		STATE_BUFFER_FAILED,	// The buffer failed the verify test
		STATE_BUFFER_UNKNOWN	// The buffer has not yet been verified
	};

public:
	// If this engine/platform requires it, do its per-class preamble
	PLATFORM_PREAMBLE;

	// Allow empty constructor to allow instantiation before download
	PatchExtractor() : m_state(STATE_BUFFER_UNKNOWN), m_version(0) {}
	PatchExtractor(char* patchData, int dataSize) {Init(patchData, dataSize);}
	bool Init(char* patchData, int dataSize);

	// See if the verify succeeded (used when constructing with buffer)
	bool IsGood(void) {return m_state == STATE_BUFFER_GOOD;}

	// QueryVersion() returns the current version number as specified by the
	// prepared patch currently in memory. Returns zero on error or if there is
	// no patch available.
	int	QueryVersion(void);

	// GetFileCount is used for status displays, to see how many files are 
	// contained in the patch
	int GetFileCount(void) const {return m_TOC.GetFileCount();}

	// SetSourceXxx tells the patcher from whence we must get the original 
	// source data to be patched.

	// Get the source data from an in-memory buffer
	void SetSourceBuffer(void* ptr) {m_TOC.SetReader(ptr, PatchReader::ReadBuffer);}
	
	// Open and read the data from disk
	void SetSourceFile(void) {m_TOC.SetReader((void*)-1, PatchReader::ReadFile);}		

	// Use the given read function to get data
	void SetSourceReadFunction(PatcherReadFn* ptr, void* handle) {m_TOC.SetReader(handle, ptr);}

	// PatchExists looks at the contents of the downloaded patch and determines whether
	// the named module is modified by the patch. If so, the size of the newly patched 
	// module is returned in reqSize; that size is how big a buffer LoadModule will require.
	bool	PatchExists(const char* name, int& reqSize);

	// PatchItem finds the named item (file) in the downloaded patch, and patches it
	// into the given buffer. If it is called without a named asset, the last found
	// item is patched.
	bool	PatchItem(const char* name, char* toBuf, int toSize);
	bool	PatchItem(char* toBuf, int toSize);

#ifdef PATCH_COMPILER_SN_PS2
	// Load the named DLL into the given buffer and relocate code. If the module
	// has been patched, the patched version is loaded from the patch buffer.
	// Otherwise, it is loaded directly from DVD.
	bool	LoadModule(const char* name, char* buffer, int bufferSize);
	bool	LoadModule(char* buffer, int bufferSize);
#endif // PATCH_COMPILER_SN_PS2

protected:
	ExtractorState		m_state;						// Our current state
	ContentTable		m_TOC;							// Table of contents of the patch
	TOCEntry			m_entry;						// Last found entry in TOC
	unsigned int		m_version;						// Version identifier provided by application
};

#endif // __PATCHEXTRACTOR_H__
