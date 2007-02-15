////////////////////////////////////////////////////////////////////////////
//
// TupperwareLoadSave
//
// This file contains the code for loading and saving the tupperware tree in ascii as well as binary
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARE_LOADSAVE_H
#define TUPPERWARE_LOADSAVE_H

#ifndef TUPPERWAREREADER_H
#include "Reader.h"
#endif

class TupperwareAggregate;

class TupperwareLoadSaveManager
{
public:
	enum FormatType
	{
		ASCII,
		BINARY,
	};

	static int Write(TupperwareLoadSaveManager::FormatType type, const char *fileName, const TupperwareAggregate* root, unsigned int cacheCRC=0 );
	static int Write(TupperwareLoadSaveManager::FormatType type, unsigned char **pBuffer, unsigned int &nBufferLength, const TupperwareAggregate* root, unsigned int cacheCRC=0 );
	static int Read(const char* fileName,TupperwareAggregate **root,TupArray<TupperwareReader::FileBuffer> *pFileBuffers = NULL);
	static int Read(const char* pBuffer, int nBufferLength, TupperwareAggregate **root);
	static unsigned int GetCacheCRC(const char* fileName);
	static TupperwareLoadSaveManager::FormatType GetFileFormat(const char* fileName);
	static TupperwareLoadSaveManager::FormatType GetFileFormat(const char* pBuffer, int bufferLength);
};

#endif
