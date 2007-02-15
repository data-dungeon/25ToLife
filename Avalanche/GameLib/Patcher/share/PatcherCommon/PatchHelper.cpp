//------------------------------------------------------------------------------
// PatchHelper.cpp
//
// Small utility functions for use in the patcher dlls. This file is linked
// into each DLL, so that they don't depend on each other and so the application
// is not forced to link it in. (Therefore, keep it small...)
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------
#include <string.h>	// For strlen

//------------------------------------------------------------------------------
// Safe version of strncpy and strncat. Works similarly to strncpy/strncat
// except (1) the string is always null terminated, and (2) it returns the length of
// the new string, so you can do another copy instead of searching for null and doing
// a strcat. If the string was truncated, it returns how long the entire string should have been.
// This function is the same as bsd strlcpy.
// Copied from "Secure Programming Cookbook" by Viega and Messier because I am too lazy to think. :-)
extern "C" int safestrlcpy(char* dst, const char* src, int size)
{
	char*		dstptr = dst;
	int			tocopy = size;
	const char*	srcptr = src;

	if (tocopy && --tocopy)	{
		do {
			if (!(*dstptr++ = *srcptr++))
				break;
		} while(--tocopy);
	}

	// If tocopy is zero, we are truncating the string
	if (!tocopy) {
		if (size)
			*dstptr = 0;
		while (*srcptr++);
	}
	return (srcptr - src - 1);
}

//------------------------------------------------------------------------------
extern "C" int safestrlcat(char* dst, const char* src, int size)
{
	char*		dstptr = dst;
	int			dstlen, tocopy = size;
	const char*	srcptr = src;

	while (tocopy-- && *dstptr) dstptr++;
	dstlen = dstptr - dst;
	if (!(tocopy = size - dstlen)) return (dstlen + strlen(src));
	while (*srcptr) {
		if (tocopy != 1) {
			*dstptr++ = *srcptr;
			tocopy--;
		}
		srcptr++;
	}
	*dstptr = 0;

	return (dstlen + (srcptr - src));
}
