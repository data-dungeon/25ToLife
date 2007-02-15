//------------------------------------------------------------------------------
// PatchBuffer.h
//
// Source for game patching library patch buffer. The patch buffer is a linear
// byte array containing data. This class and its enums defines the actual patch
// buffer format.
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#ifndef __PATCHBUFFER_H__
#define __PATCHBUFFER_H__ 1

// Patch file version numbers
#define PATCH_FILE_VERSION 0x0103

// Define int sizes etc. as they exist in the file. Technically it is an error
// to use sizeof(int) etc., since the size in the file does not change by platform.
#define FILE_INT_SIZE 4
#define FILE_CHAR_SIZE 1

// Define the offsets into the patch buffer for the various fields
enum MainOffset {
	BIGPATCH_FILE_VERSION	=  0,	//  4 bytes, Version number of the big patch file
	BIGPATCH_SIGNATURE		=  4,	// 16 bytes, MD5 signature of big patch file
	BIGPATCH_CHECKSUM_START = 20,	// Start the checksum right after the signature

	BIGPATCH_GAME_VERSION	= 20,	//  4 bytes, New version number of game being patched
	BIGPATCH_NUM_PATCHES	= 24,	//  4 bytes, Number of small patches in the big patch

	BIGPATCH_HEADER_SIZE	= 28	// Total number of bytes in the big patch file header
};

// Define the offsets into the individual patches in the big patch file
enum RelativeOffset {
	PATCHREL_COMPRESSED_LEN		=  0,	//  4 bytes, size of compressed patch in buffer
	PATCHREL_UNCOMPRESSED_LEN	=  4,	//  4 bytes, size of uncompressed patch
	PATCHREL_OFFSET				=  8,	//  4 bytes, offset from the beginning of the big patch for patch data
	PATCHREL_PATCHED_SIZE		= 12,	//  4 bytes, size of the resultant resource after patching
	PATCHREL_ORIGINAL_SIZE		= 16,	//  4 bytes, size of the original resource before patching
	PATCHREL_ORIGINAL_SIG		= 20,	// 16 bytes, MD5 signature of original resource
	PATCHREL_PATCHED_SIG		= 36,	// 16 bytes, MD5 signature of resultant resource after patching
	PATCHREL_FILENAME_LEN		= 52,	//  1 byte,  length of resource's name
	PATCHREL_FILENAME			= 53	// PATCHREL_FILENAME_LEN bytes, the resource's file name
};

class PatchBuffer {
public:
	PatchBuffer() {m_base = 0;}
	PatchBuffer(const unsigned char* ptr) {m_base = ptr;}
	void Set(const unsigned char* ptr) {m_base = ptr;}

	//------------------------------------------------------------------------------
	// Extract an int from the buffer. The file is stored low-endian, but not all
	// of the requests are 4-byte aligned which makes the PS2 crash. So always
	// extract one byte at a time to be safe. (This code can also be moved to a
	// high-endian machine without modification.)
	static int	GetInt(const unsigned char* p) {return (p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0];}
	int			GetInt(int loc) {return GetInt(&m_base[loc]);}

	unsigned char	GetByte(int loc) {return m_base[loc];}
	const char*		GetAddr(int loc) {return (const char*)&m_base[loc];}
protected:
	const unsigned char*	m_base;	// base pointer
};

class BigPatchHeader : public PatchBuffer {
public:
	BigPatchHeader(const unsigned char* ptr) : PatchBuffer(ptr) {}
	const unsigned char*	FirstResourceHeader(void) {return m_base + BIGPATCH_HEADER_SIZE;}
};

class ResourceHeader : public PatchBuffer {
public:
	ResourceHeader() : PatchBuffer() {}
	ResourceHeader(const unsigned char* ptr) : PatchBuffer(ptr) {}

	void	GotoNext(void) {m_base = m_base + PATCHREL_FILENAME + m_base[PATCHREL_FILENAME_LEN];}
};

#endif // __PATCHBUFFER_H__
