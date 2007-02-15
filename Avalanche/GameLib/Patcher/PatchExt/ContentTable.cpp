//------------------------------------------------------------------------------
// ContentTable.cpp
//
// Source for game patching library table of contents class.
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#include <string.h>
#include <ctype.h>

// Enable/disable debug printing
//#define PATCH_PRINT_ENABLED

#include "ContentTable.h"
#include "PatcherCommon/PatchHelper.h"
#include "md5.h"
#include "PatchBuffer.h"
#include "zlib.h"

// Size of a temporary buffer created on the stack
#define TMPBUFSIZE 1024

//------------------------------------------------------------------------------
inline bool _FilenamesMatch(const char* name1, const char* name2, int maxLength)
{
	// need unsigned for tolower to work correctly.
	unsigned char* a = (unsigned char*) name1;
	unsigned char* b = (unsigned char*) name2;
	#define CONVERT(x) ((x) == '\\' ? '/' : tolower(x))
	for (int i = 0; i < maxLength; i++)
	{
		if (a[i] == 0)
			return b[i] == 0;
		if (CONVERT(a[i]) != CONVERT(b[i]))
			return false;
	}
	return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// TOCEntry
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void TOCEntry::Clear(void)
{
	memset(this, 0, sizeof(TOCEntry));
}

//------------------------------------------------------------------------------
// Calculate the final size of a patch generated for this entry
int	TOCEntry::CalculateFinalSize(void)
{
	return m_finalSize;
}

//------------------------------------------------------------------------------
// Apply the patch into the given buffer. Return negative number on error, zero
// on success.
int TOCEntry::ApplyPatch(char* patchBuf, int patchBufSize, char* outBuf, int outBufSize)
{
	PATCH_PRINT("TOCEntry::ApplyPatch called\n");

	// Make sure this toc entry has been set up
	if ((m_compressedDataSize == 0) || (m_patchOffset == 0) || (m_finalSize == 0))
	{
		PATCH_PRINT("TOCEntry::ApplyPatch: Entry not properly initialized!\n");
		return -1;
	}

	// Make sure the patch spec keeps us within the patch buffer
	if ((m_patchOffset + m_compressedDataSize) > (unsigned int)patchBufSize)
	{
		PATCH_PRINT("TOCEntry::ApplyPatch: Buffer size problem!\n");
		return -2;
	}

	// Transmogrify the file name if necessary
	char filename[256+16];
	_MakeFileName(filename, m_filename, sizeof(filename), 0);

	PATCH_PRINT("TOCEntry::ApplyPatch: Opening %s...\n", filename);

	// Find and open the original file if necessary
	PatchReader& pReader = m_parentTOC->GetReader();
	bool openOK = pReader.Open(filename) >= 0;
	if (openOK)
		PATCH_PRINT("TOCEntry::ApplyPatch: Opened OK\n");
	else
		PATCH_PRINT("TOCEntry::ApplyPatch: Error opening '%s'; trying without source...\n", filename);

	// If we are supposed to verify the input data, do so. If the open failed,
	// the sig should be all zeroes, but we can probably skip that check
	if (m_useSigs && openOK && m_originalSize)
	{
		struct MD5Context md5c;
		unsigned char sig[16];
		unsigned char tmpBuf[TMPBUFSIZE];

		MD5Init(&md5c);

#if defined(PS2)
		unsigned int offset = 0;
		while (offset < m_originalSize)
		{
			int readSize = (m_originalSize - offset);
			if (readSize > TMPBUFSIZE)
				readSize = TMPBUFSIZE;
			pReader.Read((char*)tmpBuf, offset, readSize);
			MD5Update(&md5c, tmpBuf, readSize);
			offset += readSize;
		}
#else
		unsigned int offset = 0;
		unsigned int maxreadamt = (m_finalSize > m_originalSize) ? m_finalSize : m_originalSize;
		while (offset < maxreadamt)
		{
			int readSize = pReader.Read((char*)tmpBuf, offset, TMPBUFSIZE);
			if (readSize <= 0)
				break;
			MD5Update(&md5c, tmpBuf, readSize);
			offset += readSize;
		}
#endif

		MD5Final(sig, &md5c);

		// Compare our calculated signature to that stored
		if (memcmp(m_originalSig, sig, 16) != 0)
		{
			// Our compare failed. We do one more test: if the original file
			// has a signature that matches what the final signature should be,
			// then it has already been patched (really only needed for Windows;
			// should never happen on PS2)
			if (memcmp(m_modifiedSig, sig, 16) != 0)
				return -3;
			else
			{
				// The signature matches our final modified file; just copy the
				// input to the output without patching
				pReader.Read(outBuf, 0, m_finalSize);
				return 0;
			}
		}
	}

	// Location of the patch to be decompressed
	const unsigned char* patchMemory = (const unsigned char*)patchBuf + m_patchOffset;

	// OK, the input data checks out, and we are now ready to decompress the patch info
	z_stream strm;
	memset(&strm, 0, sizeof(strm));
	strm.next_in = (Bytef*)patchMemory;
	strm.avail_in = m_compressedDataSize;
	if (inflateInit(&strm) != Z_OK)
		return -6;

	int zstat = Z_OK;

	char* destIter = outBuf;
	char* destIterEnd = outBuf + m_finalSize;

	while ((zstat == Z_OK) && (destIter < destIterEnd))
	{
		unsigned char cmdBuf[2 * FILE_INT_SIZE];
		strm.next_out = cmdBuf;
		strm.avail_out = 2 * FILE_INT_SIZE;
		zstat = inflate(&strm, Z_SYNC_FLUSH);
		if ((zstat != Z_OK) && (zstat != Z_STREAM_END))
			goto ErrorEnd;

		unsigned int sourceAndOffset = PatchBuffer::GetInt(cmdBuf);
		unsigned int len = PatchBuffer::GetInt(&cmdBuf[FILE_INT_SIZE]);

		DataSource source = (DataSource)(sourceAndOffset & 3);
		unsigned int offset = sourceAndOffset >> 2;

		switch (source)
		{
			case DATA_LITERAL:
				strm.next_out = (Bytef*)destIter;
				strm.avail_out = len;
				zstat = inflate(&strm, Z_SYNC_FLUSH);
				if ((zstat != Z_OK) && (zstat != Z_STREAM_END))
					goto ErrorEnd;
				break;

			case DATA_ORIGINAL:
				if (openOK)
					pReader.Read(destIter, offset, len);
				else
				{
					PATCH_PRINT("ERROR! Unable to open source file, and it is needed!\n");
					return -4;
				}
				break;

			case DATA_MODIFIED:
				memmove( destIter, outBuf + offset, len );
				break;

			default:
				PATCH_PRINT("Error! Unrecognized data source in patch; ignoring!\n");
				break;
		}
		destIter += len;
	}

	inflateEnd(&strm);

	if (openOK)
		pReader.Close();

	// If we need to, check the checksum of the result against that stored
	if (m_useSigs)
	{
		struct MD5Context md5c;
		unsigned char sig[16];

		MD5Init(&md5c);
		MD5Update(&md5c, (unsigned char*)outBuf, m_finalSize);
		MD5Final(sig, &md5c);

		// Compare our calculated signature to that stored
		if (memcmp(m_modifiedSig, sig, 16) != 0)
			return -5;
	}

	return 0;

ErrorEnd:
	inflateEnd(&strm);
	if (openOK)
		pReader.Close();
	return -7;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// ContentTable
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Build a fast-access table of contents. If we start having big patch files
// with lots of entries, this is where we would build a hash table or sort
// the entries. Right now it isn't worth it.
bool ContentTable::Build(char* buf, int bufsize)
{
	m_baseBuf = buf;
	m_buflen = bufsize;
	m_reader.SetReadFunction((void*)-1, PatchReader::ReadFile);
	return true;
}

//------------------------------------------------------------------------------
bool ContentTable::Verify(unsigned int& version)
{
	PATCH_PRINT("Inside ContentTable::Verify. m_baseBuf=0x%x, m_buflen=%d\n", (unsigned int)m_baseBuf, m_buflen);

	// Need to build first...
	if (!m_baseBuf || (m_buflen <= 0))
		return false;

	BigPatchHeader hdr((const unsigned char*)m_baseBuf);

	// We only know how to read one version now...
	if (hdr.GetInt(BIGPATCH_FILE_VERSION) != PATCH_FILE_VERSION)
		return false;

	// The ptr points to the 16 byte md5 checksum.
	const char* storedSig = hdr.GetAddr(BIGPATCH_SIGNATURE);

	// Calc a checksum for everything after the checksum itself
	struct MD5Context md5c;
	unsigned char newSig[16];
	MD5Init(&md5c);
	MD5Update(&md5c, (const unsigned char*)hdr.GetAddr(BIGPATCH_CHECKSUM_START), m_buflen - 20);
	MD5Final(newSig, &md5c);

	// See if the stored signature matches our calculated signature
	if (memcmp(storedSig, newSig, 16) != 0)
		return false;

	// Signature verification worked. Store the version number
	version = hdr.GetInt(BIGPATCH_GAME_VERSION);

	// Save the number of files for later
	m_nFiles = hdr.GetInt(BIGPATCH_NUM_PATCHES);

	return true;
}

//------------------------------------------------------------------------------
ContentTable::EntryStatus ContentTable::FindEntry(const char* name, TOCEntry& ent)
{
	PATCH_PRINT("ContentTable::FindEntry called, looking for %s\n", name);

	// Need to build first...
	if (!m_baseBuf || (m_buflen <= 0))
		return ENTRY_ERROR_NOT_BUILT;

	// Current position in the buffer
	BigPatchHeader bigHdr((const unsigned char*)m_baseBuf);

	// We only know how to read one version at this point...
	if (bigHdr.GetInt(BIGPATCH_FILE_VERSION) != PATCH_FILE_VERSION)
		return ENTRY_ERROR_VERSION;

	// Get the number of files in the patch
	int nFiles = bigHdr.GetInt(BIGPATCH_NUM_PATCHES);
	PATCH_PRINT("ContentTable::FindEntry -- Patch contains %d files\n", nFiles);
	if (nFiles <= 0)
		return ENTRY_NOT_FOUND;	// Something really wrong; bad data?

	// Brute force search through all patched files for now. Eliminates memory
	// headaches at the cost of speed...
	int f;	// Current file number
	ResourceHeader hdr(bigHdr.FirstResourceHeader());
	for (f = 0; f < nFiles; f++) {
		const char* namePtr = hdr.GetAddr(PATCHREL_FILENAME);
		int namelen = (int)hdr.GetByte(PATCHREL_FILENAME_LEN);

		if (_FilenamesMatch(name, namePtr, namelen)) {
			ent.m_compressedDataSize = hdr.GetInt(PATCHREL_COMPRESSED_LEN);
			ent.m_uncompressedDataSize = hdr.GetInt(PATCHREL_UNCOMPRESSED_LEN);
			ent.m_patchOffset	= hdr.GetInt(PATCHREL_OFFSET);
			ent.m_finalSize		= hdr.GetInt(PATCHREL_PATCHED_SIZE);
			ent.m_originalSize  = hdr.GetInt(PATCHREL_ORIGINAL_SIZE);
			memcpy(ent.m_originalSig, hdr.GetAddr(PATCHREL_ORIGINAL_SIG), FILE_CHECKSUM_SIZE);
			memcpy(ent.m_modifiedSig, hdr.GetAddr(PATCHREL_PATCHED_SIG), FILE_CHECKSUM_SIZE);
			ent.m_useSigs = true;

			memcpy(ent.m_filename, namePtr, namelen);
			ent.m_filename[namelen] = '\0';
			ent.m_parentTOC = this;
			return ENTRY_FOUND;
		}

		// Next entry
		hdr.GotoNext();
	}

	return ENTRY_NOT_FOUND;
}

//------------------------------------------------------------------------------
ContentTable::EntryStatus ContentTable::GetFirstEntry(TOCEntry& ent)
{
	PATCH_PRINT("ContentTable::GetFirstEntry called\n");

	// Need to build first...
	if (!m_baseBuf || (m_buflen <= 0))
		return ENTRY_ERROR_NOT_BUILT;

	// Get the main header and number of patches
	BigPatchHeader bigHdr((unsigned char*)m_baseBuf);
	m_nFiles = bigHdr.GetInt(BIGPATCH_NUM_PATCHES);
	if (m_nFiles <= 0)
		return ENTRY_NOT_FOUND;	// Something really wrong; bad data?

	m_currentFile = 0;
	m_curFileHdr.Set(bigHdr.FirstResourceHeader());

	return GetNextEntry(ent);
}

//------------------------------------------------------------------------------
ContentTable::EntryStatus ContentTable::GetNextEntry(TOCEntry& ent)
{
	// If we are at the end of the list, bail
	if (m_currentFile >= m_nFiles)
		return ENTRY_NOT_FOUND;

	ent.m_compressedDataSize	= m_curFileHdr.GetInt(PATCHREL_COMPRESSED_LEN);
	ent.m_uncompressedDataSize	= m_curFileHdr.GetInt(PATCHREL_UNCOMPRESSED_LEN);
	ent.m_patchOffset			= m_curFileHdr.GetInt(PATCHREL_OFFSET);
	ent.m_finalSize				= m_curFileHdr.GetInt(PATCHREL_PATCHED_SIZE);
	int namelen					= (int)m_curFileHdr.GetByte(PATCHREL_FILENAME_LEN);

	memcpy(ent.m_filename, m_curFileHdr.GetAddr(PATCHREL_FILENAME), namelen);
	ent.m_filename[namelen] = '\0';

	ent.m_originalSize = m_curFileHdr.GetInt(PATCHREL_ORIGINAL_SIZE);
	memcpy(ent.m_originalSig, m_curFileHdr.GetAddr(PATCHREL_ORIGINAL_SIG), FILE_CHECKSUM_SIZE);
	memcpy(ent.m_modifiedSig, m_curFileHdr.GetAddr(PATCHREL_PATCHED_SIG), FILE_CHECKSUM_SIZE);
	ent.m_useSigs = true;
	ent.m_parentTOC = this;

	// Increment to next entry for next time
	m_currentFile++;
	m_curFileHdr.GotoNext();

	return ENTRY_FOUND;
}

//------------------------------------------------------------------------------
void ContentTable::Invalidate(void)
{
	m_buflen = 0;
}

//------------------------------------------------------------------------------
// Helper function; read an entire file into the given buffer using the current reader
int ContentTable::ReadFile(const char* name, char* buffer, int bufferSize)
{
	char filename[256+16];
	TOCEntry::_MakeFileName(filename, name, sizeof(filename), 0);

	int res = m_reader.Open(filename);
	if (res < 0)
	{
		PATCH_PRINT("ContentTable::ReadFile: Error %d opening %s\n", res, filename);
		return res;
	}

	// read the whole thing in one chunk
	res = m_reader.Read(buffer, 0, bufferSize);
	if (res <= 0)
	{
		PATCH_PRINT("ContentTable::ReadFile: Error %d reading from %s\n", res, filename);
	}

	m_reader.Close();
	return res;
}
