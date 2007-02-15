//------------------------------------------------------------------------------
// PatchHelper.h
//
// Small utility functions for use in the patcher dll
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#ifndef __PATCHER_PATCHHELPER_H__
#define __PATCHER_PATCHHELPER_H__ 1

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Safe versions of strcpy and strcat
int safestrlcpy(char* dst, const char* src, int size);
int safestrlcat(char* dst, const char* src, int size);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __PATCHER_PATCHHELPER_H__
