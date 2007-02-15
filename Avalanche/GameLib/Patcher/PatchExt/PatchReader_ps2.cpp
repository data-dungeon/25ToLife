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

#include <eekernel.h>
#include <sifdev.h>

#include "PatcherCommon/patchplatform.h"
#include "PatchReader.h"

//------------------------------------------------------------------------------
// Conditional open: open the file only if we are using our own file reader
int PatchReader::Open(const char* name)
{
	PATCH_PRINT("PatchReader::Open: handle=%d, Read function=0x%x\n", (int)m_sourceHandle, (unsigned int)m_readFunc);

	if (m_readFunc == PatchReader::ReadFile)
	{
		int f = (int)m_sourceHandle;
		if (f >= 0)
			return f;

		f = sceOpen(name, SCE_RDONLY);
		if (f < 0)
			return f;

		PATCH_PRINT("PatchReader::Open: Opened successfully\n");

		m_sourceHandle = (void*)f;
	}
	else if (m_readFunc == PatchReader::ReadBuffer)
	{
		// If we are reading from memory, make sure the buffer has been set
		if (!m_sourceHandle)
			return -1;
	}
	return 0;
}

//------------------------------------------------------------------------------
int PatchReader::Close(void)
{
	int status = 0;
	if (m_readFunc == PatchReader::ReadFile)
	{
		status = sceClose((int)m_sourceHandle);
		m_sourceHandle = (void*)-1;
	}
	return status;
}

////------------------------------------------------------------------------------
//bool PatchReader::NeedOpen(void)
//{
//	int handle = (int)m_sourceHandle;
//	return ((handle == -1) && (m_readFunc == PatchReader::ReadFile));
//}

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
	int f = (int)handle;
	if (f < 0)
		return -1;

	if (sceLseek(f, offset, SCE_SEEK_SET) < 0)
		return -2;

	PATCH_PRINT("PatchReader::ReadFile: Reading %d bytes from file\n", nbytes);
	return sceRead(f, buf, nbytes);
}
