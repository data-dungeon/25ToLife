//------------------------------------------------------------------------------
// ContentTable.h
//
// Source for game patching library table of contents class.
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#ifndef __CONTENTTABLE_H__
#define __CONTENTTABLE_H__ 1

#include "PatcherCommon/patchplatform.h"
#include "PatchReader.h"
#include "PatchBuffer.h"

class ContentTable;

#define FILE_CHECKSUM_SIZE 16	// MD5 checksum is 16 bytes

// A single entry in the Content Table
class TOCEntry {
	// Just let ContentTable set our members directly rather than providing access,
	// since no one else needs access.
	friend ContentTable;

	// Where each segment of data in the patch should come from
	enum DataSource
	{
		DATA_INVALID	= 0xff,
		DATA_ORIGINAL	= 0,
		DATA_MODIFIED	= 1,
		DATA_LITERAL	= 2
	};

public:
	PLATFORM_PREAMBLE;

	TOCEntry() {Clear();}
	void Clear(void);

	// Calculate the final size of a patch generated for this entry
	int		CalculateFinalSize(void);

	// Apply the patch stored in patchBuf into the given buffer outBuf.
	int		ApplyPatch(char* patchBuf, int patchBufSize, char* outBuf, int outBufSize);

	char*	GetName(void) {return m_filename;}

private:
	// Private functions
	// _MakeFileName does system-specific mangling of filenames. Must be implemented for
	// each platform.
	static bool _MakeFileName(char* dst, const char* src, int dstLen, int cpyLen);


	// Actual data. (v2) indicates that the field is only needed for patch file 
	// version 0x0102 (or maybe later)
	unsigned int	m_compressedDataSize;		// (v3)Size of data in buffer for this entry
	unsigned int	m_uncompressedDataSize;		// Size of data after uncompressing
	unsigned int	m_patchOffset;				// Offset from beginning of buffer to the patch data
	unsigned int	m_finalSize;				// Size of final file after applying patch
	unsigned int	m_originalSize;				// (v2)Size of the original file to which the patch is being applied
	unsigned char	m_originalSig[FILE_CHECKSUM_SIZE];	// (v2) MD5 signature of original data being patched
	unsigned char	m_modifiedSig[FILE_CHECKSUM_SIZE];	// (v2) MD5 signature of final patched file
	char			m_filename[256 + 16];	// Name of file being patched, with room for extras
	ContentTable*	m_parentTOC;			// The table of contents that owns this entry
	bool			m_useSigs;				// TRUE if we want to check signatures
};

// A table of contents for a patch
class ContentTable {
public:
	// Return values for FindEntry
	enum EntryStatus {
		ENTRY_FOUND,			// The requested entry is in the patch, and the information was returned
		ENTRY_NOT_FOUND,		// The requested entry is not in the patch, or the last entry was already returned
		ENTRY_ERROR_NOT_BUILT,	// The patch table of contents has not been built yet
		ENTRY_ERROR_VERSION,	// The version number of the patch is not recognized
		ENTRY_ERROR_CHECKSUM,	// The authentication code does not match the computed value
	};

	PLATFORM_PREAMBLE;

	ContentTable() : m_baseBuf(0), m_buflen(0), m_nFiles(0), m_currentFile(0) {}

	// Build a fast-access table of contents
	bool	Build(char* buf, int bufsize);

	// Verify the contents of the patch data using HMACs/Checksums/etc.
	bool	Verify(unsigned int& version);

	// Find the named file in the patch
	EntryStatus	FindEntry(const char* name, TOCEntry& ent);

	// Get the first entry in the patch, and reset the pointer to the current entry.
	EntryStatus GetFirstEntry(TOCEntry& ent);

	// Get the next entry in the patch. Returns ENTRY_NOT_FOUND if there are no more entries
	EntryStatus GetNextEntry(TOCEntry& ent);

	// Invalidate the table of contents. Do this when the buffer is deleted or
	// replace to avoid bad pointers!
	void	Invalidate(void);

	// Set the current source reading parameters
	void			SetReader(void* handle, PatcherReadFn* func) {m_reader.SetReadFunction(handle, func);}
	PatchReader&	GetReader(void) {return m_reader;}

	// Helper function; read an entire file into the given buffer using the current reader
	int		ReadFile(const char* name, char* buffer, int bufferSize);

	// Helper inlines to avoid redundant storage
	char*	GetBuf(void) const {return m_baseBuf;}
	int		GetLen(void) const {return m_buflen;}
	int		GetFileCount(void) const {return m_nFiles;}
protected:
	// Data
	char*			m_baseBuf;				// Ptr to beginning of entire patch buffer
	ResourceHeader	m_curFileHdr;			// Ptr to header info of next file to be fetched
	unsigned int	m_patchFileVersion;		// Patch file version number
	int				m_buflen;				// Length of buffer in bytes
	int				m_nFiles;				// Number of patched files in patch (set by GetFirstEntry)
	int				m_currentFile;			// next file to be referenced
	PatchReader		m_reader;				// Default source reading function
};

#endif // __CONTENTTABLE_H__