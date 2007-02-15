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

#ifndef __PATCHREADER_H__
#define __PATCHREADER_H__ 1

typedef int PatcherReadFn(void* handle, char* buf, int offset, int nbytes);

class PatchReader {
public:
	PatchReader();

	void	SetReadFunction(void* handle, PatcherReadFn* func) {m_sourceHandle = handle; m_readFunc = func;}
	int		Read(char* buf, int offset, int nbytes) { return m_readFunc(m_sourceHandle, buf, offset, nbytes); }
	int		Open(const char* name);
	int		Close(void);
	//bool	NeedOpen(void);

	static int ReadBuffer(void* handle, char* buf, int offset, int nbytes);
	static int ReadFile(void* handle, char* buf, int offset, int nbytes);

private:
	void*				m_sourceHandle;					// A handle to be passed to the read function
	PatcherReadFn*		m_readFunc;						// Ptr to read function if source type is SOURCE_CUSTOM
};

#endif // __PATCHREADER_H__
