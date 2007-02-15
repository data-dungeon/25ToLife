//------------------------------------------------------------------------------
// PatchReader.h
//
// Class for reading original source data while patching
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

// Enable/disable debug printing
//#define PATCH_PRINT_ENABLED

#include "PatcherCommon/patchplatform.h"
#include "PatchReader.h"
#include <string.h>

// Until a proper error code system is in place, use these placeholders...
#define PATCHREADER_BUFFER_NOT_ALLOCATED	-1
#define PATCHREADER_FILE_NOT_FOUND			-2
#define PATCHREADER_FILE_NOT_OPEN			-3
#define PATCHREADER_FILE_SEEK_ERROR			-4

//------------------------------------------------------------------------------
// Conditional open: open the file only if we are using our own file reader
int PatchReader::Open(const char* name)
{
	PATCH_PRINT("PatchReader::Open: handle=%d, Read function=0x%x\n", (int)m_sourceHandle, (unsigned int)m_readFunc);

	if (m_readFunc == PatchReader::ReadFile)
	{
		// Bit of a hack... For windows, just force the FILE pointer to null before opening
		m_sourceHandle = 0;

		FILE* f = (FILE*)m_sourceHandle;
		if (f)
			return 0;

		f = fopen(name, "rb");
		if (!f)
			return PATCHREADER_FILE_NOT_FOUND;

		PATCH_PRINT("PatchReader::Open: Opened successfully\n");

		m_sourceHandle = (void*)f;
	}
	else if (m_readFunc == PatchReader::ReadBuffer)
	{
		// If we are reading from memory, make sure the buffer has been set
		if (!m_sourceHandle)
			return PATCHREADER_BUFFER_NOT_ALLOCATED;
	}
	return 0;
}

//------------------------------------------------------------------------------
int PatchReader::Close(void)
{
	if ((m_readFunc == PatchReader::ReadFile) && m_sourceHandle)
	{
		fclose((FILE*)m_sourceHandle);
		m_sourceHandle = (void*)-1;
	}
	return 0;
}

//------------------------------------------------------------------------------
int PatchReader::ReadBuffer(void* handle, char* buf, int offset, int nbytes)
{
	const char* p = (const char*)handle;
	if (p)
	{
		PATCH_PRINT("PatchReader::ReadBuffer: Copying %d bytes from buffer\n", nbytes);
		memcpy(buf, &p[offset], nbytes);
		return nbytes;
	}
	PATCH_PRINT("PatchReader::ReadBuffer: Read called with null source buffer!\n");
	return -1;
}

//------------------------------------------------------------------------------
int PatchReader::ReadFile(void* handle, char* buf, int offset, int nbytes)
{
	FILE* f = (FILE*)handle;
	if (!f)
		return PATCHREADER_FILE_NOT_OPEN;

	if (fseek(f, offset, SEEK_SET))
		return PATCHREADER_FILE_SEEK_ERROR;

	PATCH_PRINT("PatchReader::ReadFile: Reading %d bytes from file\n", nbytes);
	return fread(buf, 1, nbytes, f);
}
