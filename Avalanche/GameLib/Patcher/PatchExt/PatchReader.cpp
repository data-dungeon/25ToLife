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

//------------------------------------------------------------------------------
PatchReader::PatchReader()
{
	PATCH_PRINT("|||||||||||||||||||||||||PatchReader Constructor called!\n");
	m_sourceHandle = (void*)-1;
	m_readFunc = PatchReader::ReadFile;
}
