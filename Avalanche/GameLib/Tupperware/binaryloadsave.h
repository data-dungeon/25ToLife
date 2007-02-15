////////////////////////////////////////////////////////////////////////////
//
// TupperwareBinaryLoadSave
//
// This file contains the code for loading and saving the tupperware tree to - from
// a binary format
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWAREBINARYLOADSAVE_H
#define TUPPERWAREBINARYLOADSAVE_H

////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#ifndef TUPPERWARE_ARRAY_H
#include "tuparray.h"
#endif

////////////////////////////////////////////////////////////////////////////

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareList;
class TupperwareScalar;
class TupperwareStringManager;

////////////////////////////////////////////////////////////////////////////

const unsigned int TUPPERWARE_BINARY_FILE_MAGIC_NUMBER = 0x45017629;
const float TUPPERWARE_BINARY_FILE_CURRENT_REVISION = 1.0f;

#pragma pack (push, SAVE, 1)

class TupperwareBinaryMainHeader
{
public:
	unsigned int m_magicNumber; // magic number to signify this is a tupperware binary file 0x45017629
	float m_revNumber; // which revision the file is
	unsigned int m_cacheCRC; // used for caching a tree to disk for later retrevial
	int m_stringTableLength; // how many bytes the string table has in it
	int m_atomLength; // how many bytes for the atoms (the tree)

	int m_reserved[10]; // for future use
};

class AtomBinaryChunk
{
public:
	unsigned short m_flags;
	unsigned short m_keyIndex;
};

#pragma pack (pop, SAVE)

// ATOM m_flags specification
// ppppppeelldddntt
// 8421842184218421
// pppppp = parent index
// ee = extra data - used for binary filename hint flag and bytes per element in others
// ll = length value size
// ddd = data type
// n = has name flag
// tt = atom type

const int ATOM_TYPE_MASK = 0x0003;
const int ATOM_TYPE_SHIFT = 0;
const int ATOM_HAS_NAME_MASK = 0x0004;
const int ATOM_HAS_NAME_SHIFT = 2;
const int ATOM_DATA_TYPE_MASK = 0x0038;
const int ATOM_DATA_TYPE_SHIFT = 3;
const int ATOM_DATA_LENGTH_SIZE_MASK = 0x00c0;
const int ATOM_DATA_LENGTH_SIZE_SHIFT = 6;
const int ATOM_DATA_EXTRA_MASK = 0x0300;
const int ATOM_DATA_EXTRA_SHIFT = 8;
const int ATOM_PARENT_INDEX_MASK = 0xfc00;
const int ATOM_PARENT_INDEX_SHIFT = 10;

// all atoms add "AtomBinaryChunk"
// all atoms add "unsigned short nameIndex" if the bit in m_flags is set for if it has a name

// list adds "1-4 byte length" number of floats or ints or number of characters in strings
// list adds length*sizeof(float) when float data type
// list adds length*(1-4 bytes) when int data type (use extra to encode the number of bytes)
// list adds length when string type (all the strings are combined into one list) (terminating null is not saved)

// scalar adds "1-4 bytes" length" when a string data type
// scalar adds "1-4 bytes" length" when binary type
// scalar adds "unsigned short" hint string index when a extra bit set on a binary type
// scalar adds sizeof(float) when a float data type
// scalar adds "1-4 bytes" when a int data type
// scalar adds length when a string data type (terminating null is not saved)
// scalar adds length when a binary data type


class TupperwareBinaryWriter 
{
public:
	static int Write( const char* fileName, const TupperwareAggregate* root, unsigned int cacheCRC=0 );
	static int Write(unsigned char **pBuffer,unsigned int &size,const TupperwareAggregate* root, unsigned int cacheCRC=0 );

private:
	TupperwareBinaryWriter();
	~TupperwareBinaryWriter();

	void PreProcessScalar( const TupperwareScalar* scalar );
	void PreProcessList( const TupperwareList* list);
	void PreProcessAggregate( const TupperwareAggregate* aggregate);

	void WriteScalar( const TupperwareScalar* scalar );
	void WriteList( const TupperwareList* list);
	void WriteAggregate( const TupperwareAggregate* aggregate);

	unsigned short EncodeFlags(int atomType,int dataType,int parentIndex,int hasName,int lengthSize,int extra);
	int GetLengthByteSize(int length);
	int GetDataByteSize(int value);
	int GetDataByteSize(const int *values,int length);
	void CopyDataUsingByteSize(void *destPtr,const int *source,int length,int byteDataSize); 

private:
	unsigned char *m_writeBuffer;
	unsigned char *m_currentWriteBuffer;

	void WriteDataToBuffer(const void *pData,unsigned int length);

	TupperwareStringManager *m_stringManager;
	int m_atomLengthTotal;
	int m_currentStringIndex;
	int m_currentDataSizeIndex;
	int m_parentIndex;
	TupArray<int> m_atomStringIndexArray; // keep track of my string indexes from the first pass
	TupArray<int> m_atomDataSizeArray; // keep track of the int lists data size values from first pass
};

class TupperwareBinaryReader 
{
public:
	static int GetCRC( const char *fileName, unsigned int *cacheCRC);
	static TupperwareAggregate *Read( const char *fileName);
	static TupperwareAggregate *Read( const char *pBuffer,int nBufferLength);
	static bool ValidBinaryFile(const char *fileName);
	static bool ValidBinaryFile(const char *pBuffer,int nBufferLength);

private:
	TupperwareBinaryReader(const char *pBuffer, int nBufferLength);
	~TupperwareBinaryReader();

	bool ReadStringTable(int stringTableLength);
	TupperwareAggregate *ReadAtoms(int atomLength);

	void DecodeFlags(unsigned short flags,int *atomType,int *dataType,int *parentIndex,int *hasName,int *lengthSize,int *extra);
	void CopyIntsFromPacked(int *intTable,void *packedData,int length,int byteDataSize);
	void FixPackedInt(int *intValue,int byteDataSize);
	bool ReadHeader(TupperwareBinaryMainHeader &header);
	int ReadBlock(void *pData,int nElementSize,int nElements);

private:
	const char *m_pBuffer;
	int m_nBufferLength;
	int m_nBufferPos;

	TupperwareStringManager *m_stringManager;
	TupArray<TupperwareAggregate *> m_parentArray; // keep track of the aggregate parents
};


////////////////////////////////////////////////////////////////////////////

#endif
