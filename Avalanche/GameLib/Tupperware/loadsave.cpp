////////////////////////////////////////////////////////////////////////////
//
// TupperwareLoadSave
//
// This file contains the code for loading and saving the tupperware tree in ascii as well as binary
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"
#include "loadsave.h"
#include "binaryloadsave.h"
#include "Reader.h"
#include "Writer.h"
#include "Aggregate.h"

int TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::FormatType type, const char* fileName, const TupperwareAggregate* root, unsigned int cacheCRC)
{
	if (type == BINARY)
	{
		return TupperwareBinaryWriter::Write(fileName,root,cacheCRC);
	}
	else
	{
		return TupperwareWriter::Write(fileName,root);
	}
}

int TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::FormatType type, unsigned char **pBuffer, unsigned int &nBufferLength, const TupperwareAggregate* root, unsigned int cacheCRC)
{
	if (type == BINARY)
	{
		return TupperwareBinaryWriter::Write(pBuffer,nBufferLength,root,cacheCRC);
	}
	else
	{
		// not supported at this time
		return 0;
	}
}

int TupperwareLoadSaveManager::Read(const char* fileName,TupperwareAggregate **root,TupArray<TupperwareReader::FileBuffer> *pFileBuffers)
{
	if (TupperwareBinaryReader::ValidBinaryFile(fileName))
	{
		*root = TupperwareBinaryReader::Read(fileName);
		if (!*root)
		{
			return 0;
		}
	}
	else
	{
		*root = TupperwareReader::Read(fileName,pFileBuffers);
		if (!*root)
		{
			return 0;
		}
	}
	return true;
}

int TupperwareLoadSaveManager::Read(const char* pBuffer,int nBufferLength,TupperwareAggregate **root)
{
	if (TupperwareBinaryReader::ValidBinaryFile(pBuffer,nBufferLength))
	{
		*root = TupperwareBinaryReader::Read(pBuffer,nBufferLength);
		if (!*root)
		{
			return 0;
		}
	}
	else
	{
		*root = TupperwareReader::Read(pBuffer,nBufferLength);
		if (!*root)
		{
			return 0;
		}
	}
	return true;
}

unsigned int TupperwareLoadSaveManager::GetCacheCRC(const char* fileName)
{
	unsigned int crc;
	if (TupperwareBinaryReader::GetCRC(fileName,&crc))
	{
		return crc;
	}
	return 0;
}

TupperwareLoadSaveManager::FormatType TupperwareLoadSaveManager::GetFileFormat(const char* fileName)
{
	if (TupperwareBinaryReader::ValidBinaryFile(fileName))
	{
		return BINARY;
	}
	else
	{
		return ASCII;
	}
};

TupperwareLoadSaveManager::FormatType TupperwareLoadSaveManager::GetFileFormat(const char* pBuffer, int nBufferLength)
{
	if (TupperwareBinaryReader::ValidBinaryFile(pBuffer,nBufferLength))
	{
		return BINARY;
	}
	else
	{
		return ASCII;
	}
};

