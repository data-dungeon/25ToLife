#include <windows.h>
#include <iostream>
#include "RPatchCompiler.h"
#include "md5.h"
#include "RPatchBuilder.h"
#include "zlib.h"
#include <io.h>
#include <fcntl.h>
#include <crtdbg.h>

using namespace std;

static const int PATCH_FILE_VERSION = 0x0103;

static bool FilesMatch(const string&f1, const string& f2);

// ----------------------------------------------------------------------------
// The RPatchDescriptor class just contains a header (RPatchHeader) and a 
// builder. It is a convenience to avoid all the extra file writing the patcher
// held originally, without having to rewrite everything.
RPatchDescriptor::RPatchDescriptor(RPatchBuilder* builder) : m_builder(builder)
{
}

// ----------------------------------------------------------------------------
RPatchDescriptor::~RPatchDescriptor()
{
	if (m_builder)
		delete m_builder;
}

//------------------------------------------------------------------------------
void RPatchDescriptor::SetBaseName(const std::string& baseName)
{
	strncpy(m_header.filename, baseName.c_str(), 255);
	m_header.filename[255] = 0;
	m_header.fileNameLength = (unsigned char)strlen(m_header.filename);
}

//------------------------------------------------------------------------------
void RPatchDescriptor::SetFinalSize(int size)
{
	m_header.patchedSize = size;
}

//------------------------------------------------------------------------------
void RPatchDescriptor::SetOffset(int offset)
{
	m_header.patchOffset = offset;
}

//------------------------------------------------------------------------------
void RPatchDescriptor::SetOriginalSize(int size)
{
	m_header.originalSize = size;
}

//------------------------------------------------------------------------------
void RPatchDescriptor::SetOriginalSig(unsigned char* sig)
{
	memcpy(m_header.originalSig, sig, 16);
}

//------------------------------------------------------------------------------
void RPatchDescriptor::SetModifiedSig(unsigned char* sig)
{
	memcpy(m_header.modifiedSig, sig, 16);
}

//------------------------------------------------------------------------------
bool RPatchDescriptor::WritePatch(FILE* f)
{
	if (!f || !m_builder)
		return false;

	// Get the final patch size
	int finalSize = m_builder->CalcPatchSize();

	// Allocate a buffer to hold the raw patch
	char* srcBuf = new char[finalSize];
	if (!srcBuf)
		return false;

	// Allocate a buffer to hold the compressed patch
	uLong bufSize = (int)((1.001f * finalSize) + 0.5f) + 12;
	char* dstBuf = new char[bufSize];
	if (!dstBuf)
		return false;

	// Decode the patch into our source buffer
	finalSize = m_builder->WritePatchToBuffer(srcBuf, finalSize);

	// Compress the patch
	compress((Bytef*)dstBuf, &bufSize, (Bytef*)srcBuf, (uLong)finalSize);

	// Save the compressed size and uncompressed size
	m_header.uncompressedLength = finalSize;
	m_header.compressedLength = bufSize;

	// Write the buffer
	return (fwrite(dstBuf, 1, bufSize, f) == bufSize);
}

//------------------------------------------------------------------------------
int RPatchDescriptor::HeaderSize(void) const
{
	register int size = sizeof(m_header.compressedLength);
	size += sizeof(m_header.uncompressedLength);
	size += sizeof(m_header.patchOffset);
	size += sizeof(m_header.patchedSize);
	size += sizeof(m_header.fileNameLength);
	size += m_header.fileNameLength;

	size += sizeof(m_header.originalSize);
	size += sizeof(m_header.originalSig);
	size += sizeof(m_header.modifiedSig);
	return size;
}

//------------------------------------------------------------------------------
bool RPatchDescriptor::WriteHeader(FILE* f)
{
	_ASSERTE(sizeof(int) == 4);	// Just in case...

	return ((fwrite(&m_header.compressedLength,   sizeof(m_header.compressedLength),   1, f) > 0) &&
			(fwrite(&m_header.uncompressedLength, sizeof(m_header.uncompressedLength), 1, f) > 0) &&
			(fwrite(&m_header.patchOffset,        sizeof(m_header.patchOffset),        1, f) > 0) &&
			(fwrite(&m_header.patchedSize,        sizeof(m_header.patchedSize),        1, f) > 0) &&
			(fwrite(&m_header.originalSize,       sizeof(m_header.originalSize),       1, f) > 0) &&
			(fwrite(&m_header.originalSig,        sizeof(m_header.originalSig),        1, f) > 0) &&
			(fwrite(&m_header.modifiedSig,        sizeof(m_header.modifiedSig),        1, f) > 0) &&
			(fwrite(&m_header.fileNameLength,     sizeof(m_header.fileNameLength),     1, f) > 0) &&
			(fwrite(&m_header.filename,           m_header.fileNameLength,             1, f) > 0));
}

// ----------------------------------------------------------------------------
RPatchCompiler::RPatchCompiler(const string& basePath1, const string& basePath2)
	: _basePath1(basePath1)
	, _basePath2(basePath2)
{
}


// ----------------------------------------------------------------------------
RPatchCompiler::~RPatchCompiler()
{
	_ClearDescriptors();
}


// ----------------------------------------------------------------------------
void RPatchCompiler::SetPaths(const string& basePath1, const string& basePath2)
{
	_basePath1 = basePath1;
	_basePath2 = basePath2;
}

// ----------------------------------------------------------------------------
void RPatchCompiler::_ClearDescriptors(void)
{
	list<RPatchDescriptor*>::iterator iter;
	for ( iter = _descriptors.begin(); iter != _descriptors.end(); iter++ )
	{
		RPatchDescriptor* desc = (*iter);
		delete desc;
	}

	_descriptors.clear();
}

// ----------------------------------------------------------------------------
void RPatchCompiler::CalculateMD5(string filename, unsigned char *signature)
{
	int j;
	FILE *in = 0;
	unsigned char buffer[32768];
	struct MD5Context md5c;

	if ((in = fopen(filename.c_str(), "rb")) == NULL) 
	{
		// File not found -- set empty signature
		memset(signature, 0, 16);
		return;
	}

	MD5Init(&md5c);
	while ((j = (int) fread(buffer, 1, sizeof(buffer), in)) > 0) 
	{
		MD5Update(&md5c, buffer, (unsigned) j);	
	}

	MD5Final(signature, &md5c);
	fclose(in);
}

// ----------------------------------------------------------------------------
bool RPatchCompiler::GenerateFileList(list<string>& finalFileList, string pathname, bool recurseDirs)
{
	intptr_t hnd;
	struct _finddata_t filestruct;
	char prevDir[_MAX_PATH];
	unsigned int attrib;
	list<string> fileList;
	
	// Save off our previous directory location and go to the one specified
	GetCurrentDirectory(_MAX_PATH, prevDir);
	if (!SetCurrentDirectory(pathname.c_str()))
		return false;

	// Get the first file / directory
	hnd = _findfirst("*", &filestruct);

	if ( hnd == -1 )
	{
		SetCurrentDirectory(prevDir);
		return false; // Bail out, error
	}


	do // Go through all the files in this directory
	{
		string filename = filestruct.name;
		attrib = GetFileAttributes(filename.c_str());
		
		// Handle files, add them to the temporary file list
		if ( (attrib & FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY) && !(attrib & FILE_ATTRIBUTE_DIRECTORY) ) 
			fileList.push_back(pathname + "\\" + filename);

		// Handle directories / recurse
		if ( (attrib & FILE_ATTRIBUTE_DIRECTORY) && recurseDirs )
		{
			if ( filename != "." && filename != ".." )
			{
				// Recurse with path appended
				if (!GenerateFileList(finalFileList, pathname + "\\" + filename))
				{
					finalFileList.clear();
					return false;
				}
			}
		}
	} while(!_findnext(hnd, &filestruct));

	_findclose(hnd);
	SetCurrentDirectory(prevDir);

	// Finally, merge the fileList from this directory into the final list.
	finalFileList.merge(fileList);
	return true;
}


// ----------------------------------------------------------------------------
string RPatchCompiler::FindMatchingFile(const string& path2File, list<string>& sourceFileList)
{
	string p2File = path2File.substr(_basePath2.length()+1);

	list<string>::iterator iter;
	for ( iter = sourceFileList.begin(); iter != sourceFileList.end(); iter++ )
	{
		string path1File = (*iter);
		string p1File = path1File.substr(_basePath1.length()+1);

		if ( p1File == p2File ) 
			return path1File;
	}

	return "";
}


// ----------------------------------------------------------------------------
int RPatchCompiler::GetFileSize(string filename)
{
	int size = 0;
	FILE *file = fopen(filename.c_str(), "rb");
	if ( !file )
		return 0;

	fseek( file, 0, SEEK_END );	
	size = (int)ftell(file);
	fclose(file);
	return size;
}


// ----------------------------------------------------------------------------
bool RPatchCompiler::CreatePatchFile(const string& file, const string fileMatch)
{
	string finalPatchName;
	string finalDescName;
	string::size_type i;

	int smallestMatchLen = 16;

	RPatchBuilder* patch = new RPatchBuilder;
	if (!patch)
		return false;

	// Extract the base resource name
	string baseName = file.substr(_basePath1.length()+1);

	// Get the size of the final file
	int resultSize = GetFileSize(fileMatch);
	int patchSize = 0;

	patch->LoadOriginalData(file.c_str());
	if (patch->IsError())
	{
		// Original file load failed; generate a complete copy of the new data
		patch->BuildNoCompare(fileMatch.c_str());
		if( patch->IsError() )
			goto PatchFailed;
	}
	else
	{
		// Original file is there, do a normal patch generation
		patch->LoadModifiedData(fileMatch.c_str());
		if( patch->IsError() )
			goto PatchFailed;

		patch->Build( smallestMatchLen );
		if( patch->IsError() )
			goto PatchFailed;

		// Temp hack: to get around the weakness of this algorithm, we
		// check the size of the patch; if it is too big, just include the entire
		// modified file in one chunk
		patchSize = patch->CalcPatchSize();
		if (patchSize > (resultSize + 8))
		{
			patch->Clear();
			patch->BuildNoCompare(fileMatch.c_str());
			if( patch->IsError() )
				goto PatchFailed;
		}
	}

	patch->Verify();
	if( patch->IsError() )
		goto PatchFailed;

	// OK, the patch is good. Let's save it in a descriptor
	RPatchDescriptor* pdesc = new RPatchDescriptor(patch);
	_descriptors.push_back(pdesc);

	// Replace back-slash with forward slash
	for (i = 0; i < baseName.size(); i++)
	{
		if (baseName[i] == '\\')
			baseName[i] = '/';
	}

	// Set the base name and final file size
	pdesc->SetBaseName(baseName);
	pdesc->SetFinalSize(GetFileSize(fileMatch));
	pdesc->SetOriginalSize(GetFileSize(file));

	// Calculate the before and after checksums
	unsigned char sig[16];
	CalculateMD5(file.c_str(), sig);
	pdesc->SetOriginalSig(sig);
	CalculateMD5(fileMatch.c_str(), sig);
	pdesc->SetModifiedSig(sig);

	return true;

PatchFailed:
	if (patch)
		delete patch;
	return false;
}


// ----------------------------------------------------------------------------
int RPatchCompiler::CalculateHeaderSize(void)
{
	int size = 4;	// 4 bytes for patch file version
	size += 16;		// 16 bytes for md5 checksum
	size += 4;		// 4 bytes for version number of resultant patched game
	size += 4;		// 4 bytes for number of patches

	list<RPatchDescriptor*>::iterator iter;
	for ( iter = _descriptors.begin(); iter != _descriptors.end(); iter++ )
	{
		size += (*iter)->HeaderSize();
	}

	return size;
}

// ----------------------------------------------------------------------------
bool RPatchCompiler::PatchDataToBigPatch(FILE *bigPatch)
{
	list<RPatchDescriptor*>::iterator descIter;
	for (descIter = _descriptors.begin(); descIter != _descriptors.end(); descIter++)
	{
		RPatchDescriptor* desc = *descIter;
		desc->SetOffset((int)ftell(bigPatch));
		if (!desc->WritePatch(bigPatch))
			return false;
	}
	return true;
}

// ----------------------------------------------------------------------------
void RPatchCompiler::CalculateBigMD5(FILE* bigPatch, int startOffset, unsigned char *signature)
{
	int j;
	unsigned char buffer[32768];
	struct MD5Context md5c;

	fflush(bigPatch);
	fseek(bigPatch, startOffset, SEEK_SET);

	MD5Init(&md5c);
	while ((j = (int) fread(buffer, 1, sizeof(buffer), bigPatch)) > 0) 
	{
		MD5Update(&md5c, buffer, (unsigned) j);	
	}

	MD5Final(signature, &md5c);
}

// ----------------------------------------------------------------------------
bool RPatchCompiler::WriteBigPatchHeader(FILE *bigPatch, unsigned int version)
{
	// Get a checksum for everything past the signature
	unsigned char signature[16];
	memset(signature, 0, 16);

	rewind(bigPatch);

	// Write the patch file version number
	if (fwrite(&PATCH_FILE_VERSION, 1, 4, bigPatch) != 4)
		return false;

	// Write the checksum placeholder
	if (fwrite(signature, 1, 16, bigPatch) != 16)
		return false;

	// Write out the new post-patch game version
	if (fwrite(&version, 1, 4, bigPatch) != 4)
		return false;

	// Write the number of patches in the big file
	const size_t numPatches = _descriptors.size();
	if (fwrite(&numPatches, sizeof(numPatches), 1, bigPatch) != 1)
		return false;

	// Write out the header for each file
	list<RPatchDescriptor*>::iterator diter;
	for ( diter = _descriptors.begin(); diter != _descriptors.end(); diter++ )
	{
		RPatchDescriptor* desc = *diter;
		if (!desc->WriteHeader(bigPatch))
			return false;
	}

	// Calculate the signature for everything past the signature itself
	CalculateBigMD5(bigPatch, 20, signature);

	// Write out the final signature
	fseek(bigPatch, 4, SEEK_SET);
	if (fwrite(signature, 16, 1, bigPatch) != 1)
		return false;

	return true;
}

// ----------------------------------------------------------------------------
bool RPatchCompiler::CompileToBigPatch(const string& patchName, unsigned int version)
{
	// Get the full header size
	int headerSize = CalculateHeaderSize();

	// Write the big patch file
	FILE* bigPatch = fopen( patchName.c_str(), "wb+" );
	if( !bigPatch )
	{
		//printf("Error: Cannot create big patch file.\n");
		return false;
	}
	
	// Write some dummy data as a placeholder for the header
	for ( int i = 0; i<headerSize; i++ )
	{
		if (fwrite( &i, 1, 1, bigPatch ) != 1)
		{
			fclose(bigPatch);
			return false;
		}
	}

	bool ok = PatchDataToBigPatch(bigPatch) &&
			  WriteBigPatchHeader(bigPatch, version);

	fclose(bigPatch);
	return ok;
}


void RPatchCompiler::GetDifferenceList(list<string>& sourceFileList, list<string>& modFileList, list<string>& differenceList)
{
	list<string>::iterator iter;

	differenceList.clear();
	for (iter = modFileList.begin(); iter != modFileList.end(); iter++)
	{
		string file = *iter;
		string fileMatch = FindMatchingFile(file, sourceFileList);

		if (!(fileMatch.length() && FilesMatch(file, fileMatch)))
		{
			differenceList.push_back(file);
		}
	}
}

// ----------------------------------------------------------------------------
bool RPatchCompiler::CompilePatch(list<string>& filesToPatch, const string& patchName, unsigned int version)
{
	// Start with a clean slate
	_ClearDescriptors();

	list<string>::iterator iter;
	for ( iter = filesToPatch.begin(); iter != filesToPatch.end(); iter++ )
	{
		string file2 = (*iter);

		// Make a path1 name to test against
		string file1 = _basePath1 + "\\" + file2.substr(_basePath2.length()+1);

		if (!CreatePatchFile(file1, file2))
			return false;
	}

	return CompileToBigPatch(patchName, version);
}

//------------------------------------------------------------------------------
static bool FilesMatch(const string&f1, const string& f2)
{
	FILE* file1 = fopen(f1.c_str(), "rb");
	FILE* file2 = fopen(f2.c_str(), "rb");

	if (!file1 && !file2)
		return true;		// Neither is there, call it a match

	if (!file1 || !file2)
		goto CompareFail;

	// Both files exist. Get their sizes
	fseek(file1, 0, SEEK_END);
	int size1 = ftell(file1);
	fseek(file2, 0, SEEK_END);
	int size2 = ftell(file2);
	if (size1 != size2)
		goto CompareFail;

	rewind(file1);
	rewind(file2);

	// Both files are the same size. Do a binary compare
	#define COMPARESIZE 32768
	unsigned char buf1[COMPARESIZE];
	unsigned char buf2[COMPARESIZE];

	size_t r1, r2;
	while (1) {
		r1 = fread(buf1, 1, COMPARESIZE, file1);
		r2 = fread(buf2, 1, COMPARESIZE, file2);
		if (r1 != r2)
			goto CompareFail;

		// End of file, we are done and they match
		if (r1 <= 0)
			break;

		// Compare the buffers
		if (memcmp(buf1, buf2, r1) != 0)
			goto CompareFail;
	}

	// If we made it here, the files match
	fclose(file1);
	fclose(file2);
	return true;

CompareFail:
	if (file1)
		fclose(file1);
	if (file2)
		fclose(file2);
	return false;
}
