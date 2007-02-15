////////////////////////////////////////////////////////////////////////////
//
// TupperwareBinaryLoadSave
//
// This file contains the code for loading and saving the tupperware tree to - from
// a binary format
//
////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"

#include "binaryloadsave.h"
#include "stringmanager.h"

#include "Aggregate.h"
#include "List.h"
#include "Log.h"
#include "Iterator.h"
#include "Scalar.h"

////////////////////////////////////////////////////////////////////////////
//
// Writer code
//

TupperwareBinaryWriter::TupperwareBinaryWriter(void) : 
	m_currentWriteBuffer(NULL),
	m_writeBuffer(NULL)
{
	m_stringManager = new TupperwareStringManager;
	m_atomStringIndexArray.SetSize(0,1024);
	m_atomDataSizeArray.SetSize(0,1024);
}


TupperwareBinaryWriter::~TupperwareBinaryWriter() 
{
	assert(!m_currentWriteBuffer);
	assert(!m_writeBuffer);
	delete m_stringManager;
}

inline void TupperwareBinaryWriter::WriteDataToBuffer(const void *pData,unsigned int length)
{
	if (pData && length)
	{
		memcpy(m_currentWriteBuffer,pData,length);
		m_currentWriteBuffer+=length;
	}
	else
	{
		printf("this is a test");
	}
}

// this is the only public function available to the outside
int TupperwareBinaryWriter::Write( const char* fileName, const TupperwareAggregate* root, unsigned int cacheCRC )
{
	FILE *pFile = fopen( fileName, "wb" );

	if ( pFile == NULL )
		return 0;

	TupperwareBinaryWriter writer;
	writer.m_atomLengthTotal = 0;
	writer.PreProcessAggregate(root);

	int stringTableSize;
	char *stringTablePtr;
	writer.m_stringManager->BuildBuffer(&stringTablePtr,stringTableSize);

	unsigned int size = sizeof(TupperwareBinaryMainHeader)+stringTableSize+writer.m_atomLengthTotal;
	unsigned char *pBuffer = new unsigned char[size];

	writer.m_writeBuffer = pBuffer;
	writer.m_currentWriteBuffer = writer.m_writeBuffer; // start at the beginning

	// fill the main header
	TupperwareBinaryMainHeader header;
	header.m_magicNumber = TUPPERWARE_BINARY_FILE_MAGIC_NUMBER;
	header.m_cacheCRC = cacheCRC;
	header.m_revNumber = TUPPERWARE_BINARY_FILE_CURRENT_REVISION;
	header.m_atomLength = writer.m_atomLengthTotal;
	header.m_stringTableLength = stringTableSize;
	for (int i=0;i<10;i++) 
	{
		header.m_reserved[i] = 0;
	}

	writer.WriteDataToBuffer(&header,sizeof(TupperwareBinaryMainHeader));
	writer.WriteDataToBuffer(stringTablePtr,stringTableSize);

	writer.m_currentStringIndex = 0;
	writer.m_currentDataSizeIndex = 0;
	writer.m_parentIndex = 0;
	writer.WriteAggregate( root);

	assert((writer.m_currentWriteBuffer-writer.m_writeBuffer) == size);

	int result = fwrite(pBuffer,size,1,pFile);

	delete [] pBuffer; // done with buffers (since we are saving a file)
	delete [] stringTablePtr;

	writer.m_currentWriteBuffer = NULL;
	writer.m_writeBuffer = NULL;
	fclose(pFile);
	return result;
}

// this version will allocate a buffer and fill it with the data
// the caller is responsible for freeing this memory
int TupperwareBinaryWriter::Write(unsigned char **pBuffer,unsigned int &size,const TupperwareAggregate* root, unsigned int cacheCRC)
{
	TupperwareBinaryWriter writer;
	writer.m_atomLengthTotal = 0;
	writer.PreProcessAggregate(root);

	int stringTableSize;
	char *stringTablePtr;
	writer.m_stringManager->BuildBuffer(&stringTablePtr,stringTableSize);

	size = sizeof(TupperwareBinaryMainHeader)+stringTableSize+writer.m_atomLengthTotal;
	*pBuffer = new unsigned char[size];

	writer.m_writeBuffer = *pBuffer;
	writer.m_currentWriteBuffer = writer.m_writeBuffer; // start at the beginning

	// fill the main header
	TupperwareBinaryMainHeader header;
	header.m_magicNumber = TUPPERWARE_BINARY_FILE_MAGIC_NUMBER;
	header.m_cacheCRC = cacheCRC;
	header.m_revNumber = TUPPERWARE_BINARY_FILE_CURRENT_REVISION;
	header.m_atomLength = writer.m_atomLengthTotal;
	header.m_stringTableLength = stringTableSize;
	for (int i=0;i<10;i++) 
	{
		header.m_reserved[i] = 0;
	}
	writer.WriteDataToBuffer(&header,sizeof(TupperwareBinaryMainHeader));
	writer.WriteDataToBuffer(stringTablePtr,stringTableSize);
	delete [] stringTablePtr; // done with this

	writer.m_currentStringIndex = 0;
	writer.m_currentDataSizeIndex = 0;
	writer.m_parentIndex = 0;
	writer.WriteAggregate( root);
	return 1;
}

void TupperwareBinaryWriter::PreProcessScalar( const TupperwareScalar* scalar )
{
	m_atomLengthTotal += sizeof(AtomBinaryChunk);
	int keyIndex = m_stringManager->AddString(scalar->GetKey());
	m_atomStringIndexArray.Add(keyIndex); // add the indexes to an array to reference later
	const char *name = scalar->GetName();
	if (name)
	{
		int nameIndex = m_stringManager->AddString(name);
		m_atomStringIndexArray.Add(nameIndex);
		m_atomLengthTotal += sizeof(unsigned short);

	}
	TupperwareAtom::DataType type = scalar->GetDataType();

	switch (type)
	{
		case TupperwareAtom::DATA_STRING:
			{
				const char *string = scalar->GetAsString();
				int stringLength = strlen(string);
				m_atomLengthTotal += GetLengthByteSize(stringLength); // count length of string
				m_atomLengthTotal += stringLength; // count string but not null at end
				m_atomDataSizeArray.Add(stringLength); // save string length
			}
			break;
		case TupperwareAtom::DATA_FLOAT:
			m_atomLengthTotal += sizeof(float);
			break;
		case TupperwareAtom::DATA_INT:
			{
				int value = scalar->GetAsInt();
				m_atomLengthTotal += GetDataByteSize(value); // number of bytes the value will need
			}
			break;
		case TupperwareAtom::DATA_BINARY:
			{
				int binaryLength;
				scalar->GetAsBinary(&binaryLength);
				m_atomLengthTotal += GetLengthByteSize(binaryLength); // count length
				m_atomLengthTotal += binaryLength; // count binary data
			}
	}
}

void TupperwareBinaryWriter::PreProcessList( const TupperwareList* list)
{
	m_atomLengthTotal += sizeof(AtomBinaryChunk);
	int keyIndex = m_stringManager->AddString(list->GetKey());
	m_atomStringIndexArray.Add(keyIndex); // add the indexes to an array to reference later
	const char *name = list->GetName();
	if (name)
	{
		int nameIndex = m_stringManager->AddString(name);
		m_atomStringIndexArray.Add(nameIndex);
		m_atomLengthTotal += sizeof(unsigned short);

	}
	TupperwareAtom::DataType type = list->GetDataType();
	int length = list->GetLength();
	switch (type)
	{
		case TupperwareAtom::DATA_STRING:
			{
				const char **stringTable = list->GetAsString();
				int byteLength = 0;
				for (int i=0;i<length;i++)
				{
					byteLength += strlen(stringTable[i]); // count string length
					if (i+1<length)
						byteLength++; // count null at end (but not on last string)
				}
				m_atomDataSizeArray.Add(byteLength); // save byte size of string list
				m_atomLengthTotal+=GetLengthByteSize(byteLength); // count length 
				m_atomLengthTotal+=byteLength; 
			}
			break;
		case TupperwareAtom::DATA_FLOAT:
			{
				m_atomLengthTotal += GetLengthByteSize(length); // count length 
				m_atomLengthTotal += length * sizeof(float);
			}
			break;
		case TupperwareAtom::DATA_INT:
			{
				m_atomLengthTotal += GetLengthByteSize(length); // count length 
				const int *values = list->GetAsInt();
				int byteSize = GetDataByteSize(values,length);
				m_atomLengthTotal += length * byteSize;
				m_atomDataSizeArray.Add(byteSize); // save byte size of int list
			}
			break;
	}
}

void TupperwareBinaryWriter::PreProcessAggregate( const TupperwareAggregate* aggregate)
{
	m_atomLengthTotal += sizeof(AtomBinaryChunk);
	int keyIndex = m_stringManager->AddString(aggregate->GetKey());
	m_atomStringIndexArray.Add(keyIndex); // add the indexes to an array to reference later
	const char *name = aggregate->GetName();
	if (name)
	{
		int nameIndex = m_stringManager->AddString(name);
		m_atomStringIndexArray.Add(nameIndex);
		m_atomLengthTotal += sizeof(unsigned short);

	}

	for (TupperwareIterator i = aggregate->FindAll(); !i.Finished(); i.Next())
	{
		TupperwareAtom *atom = i.GetAsAtom();

		switch (atom->GetAtomType())
		{
			case TupperwareAtom::ATOM_AGGREGATE:
				PreProcessAggregate((TupperwareAggregate *)atom);
				break;
			case TupperwareAtom::ATOM_LIST:
				PreProcessList((TupperwareList *)atom);
				break;
			case TupperwareAtom::ATOM_SCALAR:
				PreProcessScalar((TupperwareScalar *)atom);
				break;
		}
	}
}

void TupperwareBinaryWriter::WriteScalar( const TupperwareScalar* scalar )
{
	AtomBinaryChunk header;
	TupperwareAtom::DataType type = scalar->GetDataType();
	const char *name = scalar->GetName();
	int hasName = name ? 1:0;
	int lengthSize=0;
	int extra=0;
	int length;

	switch (type)
	{
		case TupperwareAtom::DATA_STRING:
			{
				length = m_atomDataSizeArray[m_currentDataSizeIndex++]; // get string length
				lengthSize = GetLengthByteSize(length)-1;
			}
			break;
		case TupperwareAtom::DATA_FLOAT:
			break;
		case TupperwareAtom::DATA_INT:
			{
				int value = scalar->GetAsInt();
				extra = GetDataByteSize(value)-1;
			}
			break;
		case TupperwareAtom::DATA_BINARY:
			{
				int binaryLength;
				scalar->GetAsBinary(&binaryLength);
				lengthSize = GetLengthByteSize(binaryLength)-1;
			}
			break;
	}
	header.m_flags = EncodeFlags((int)TupperwareAtom::ATOM_SCALAR,(int)type,m_parentIndex,hasName,lengthSize,extra);
	header.m_keyIndex = m_atomStringIndexArray[m_currentStringIndex++];
	WriteDataToBuffer(&header,sizeof(AtomBinaryChunk));
	if (hasName)
	{
		unsigned short nameIndex = m_atomStringIndexArray[m_currentStringIndex++];
		WriteDataToBuffer(&nameIndex,sizeof(unsigned short));
	}

	switch (type)
	{
		case TupperwareAtom::DATA_STRING:
			{
				const char *pString = scalar->GetAsString();
				// length and lengthSize are set from above

				WriteDataToBuffer(&length,lengthSize+1); // save length
				if (length)
				{
					WriteDataToBuffer(pString,length); // save string
				}
			}
			break;
		case TupperwareAtom::DATA_FLOAT:
			{
				float value = scalar->GetAsFloat();
				WriteDataToBuffer(&value,sizeof(float));
			}
			break;
		case TupperwareAtom::DATA_INT:
			{
				int value = scalar->GetAsInt();
				WriteDataToBuffer(&value,extra+1);
			}
			break;
		case TupperwareAtom::DATA_BINARY:
			{
				if (extra) // does this atom have a binary hint
				{
					unsigned short filenameHint = m_atomStringIndexArray[m_currentStringIndex++];
					WriteDataToBuffer(&filenameHint,sizeof(unsigned short));
				}
				int binaryLength;
				const void *data = scalar->GetAsBinary(&binaryLength);
				WriteDataToBuffer(&binaryLength,lengthSize+1); // save the length of the binary data
				if (binaryLength)
				{
					WriteDataToBuffer(data,binaryLength); // save the binary data
				}
			}
			break;
	}
}

void TupperwareBinaryWriter::WriteList( const TupperwareList* list)
{
	AtomBinaryChunk header;
	TupperwareAtom::DataType type = list->GetDataType();
	const char *name = list->GetName();
	int hasName = name ? 1:0;
	int lengthSize=0;
	int extra=0;
	int length = list->GetLength();
	int byteLength;

	switch (type)
	{
		case TupperwareAtom::DATA_STRING:
			{
				byteLength = m_atomDataSizeArray[m_currentDataSizeIndex++]; // get string length
				lengthSize = GetLengthByteSize(byteLength)-1;
			}
			break;
		case TupperwareAtom::DATA_FLOAT:
			{
				lengthSize = GetLengthByteSize(length)-1;
			}
			break;
		case TupperwareAtom::DATA_INT:
			{
				lengthSize = GetLengthByteSize(length)-1;
				extra = m_atomDataSizeArray[m_currentDataSizeIndex++]-1; // get byte size
			}
			break;
	}

	header.m_flags = EncodeFlags((int)TupperwareAtom::ATOM_LIST,(int)type,m_parentIndex,hasName,lengthSize,extra);
	header.m_keyIndex = m_atomStringIndexArray[m_currentStringIndex++];
	WriteDataToBuffer(&header,sizeof(AtomBinaryChunk));
	if (hasName)
	{
		unsigned short nameIndex = m_atomStringIndexArray[m_currentStringIndex++];
		WriteDataToBuffer(&nameIndex,sizeof(unsigned short));
	}
	switch (type)
	{
		case TupperwareAtom::DATA_STRING:
			{
				int stringLength = 0;
				const char **stringTable = list->GetAsString();
				WriteDataToBuffer(&byteLength,lengthSize+1); // save length
				int nullAdd;
				for (int i=0;i<length;i++)
				{
					nullAdd = (i+1<length) ? 1 : 0; // only save null at end of string when we are the last one
					stringLength = strlen(stringTable[i])+nullAdd;
					// if the last string is just a null we need to skip the write
					if (stringLength)
					{
						WriteDataToBuffer(stringTable[i],strlen(stringTable[i])+nullAdd);
					}
				}
			}
			break;
		case TupperwareAtom::DATA_FLOAT:
			{
				WriteDataToBuffer(&length,lengthSize+1); // save length
				const float *value = list->GetAsFloat();
				WriteDataToBuffer(value,length*sizeof(float));
			}
			break;
		case TupperwareAtom::DATA_INT:
			{
				WriteDataToBuffer(&length,lengthSize+1); // save length
				const int *value = list->GetAsInt();
				void *packedData = new char [length*(extra+1)]; // allocate the packed int array
				CopyDataUsingByteSize(packedData,value,length,extra+1); // back it in
				WriteDataToBuffer(packedData,length*(extra+1));
				delete[] (char*)packedData;
			}
			break;
	}
}

void TupperwareBinaryWriter::WriteAggregate( const TupperwareAggregate* aggregate)
{
	AtomBinaryChunk header;
	const char *name = aggregate->GetName();
	int hasName = name ? 1:0;

	header.m_flags = EncodeFlags((int)TupperwareAtom::ATOM_AGGREGATE,0,m_parentIndex,hasName,0,0);
	header.m_keyIndex = m_atomStringIndexArray[m_currentStringIndex++];
	WriteDataToBuffer(&header,sizeof(AtomBinaryChunk));
	if (hasName)
	{
		unsigned short nameIndex = m_atomStringIndexArray[m_currentStringIndex++];
		WriteDataToBuffer(&nameIndex,sizeof(unsigned short));
	}
	m_parentIndex++; // our children use a larger parent index
	for (TupperwareIterator i = aggregate->FindAll(); !i.Finished(); i.Next())
	{
		TupperwareAtom *atom = i.GetAsAtom();

		if (atom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE) 
		{
			WriteAggregate((TupperwareAggregate *)atom);
		} 
		else if (atom->GetAtomType() == TupperwareAtom::ATOM_LIST)
		{
			WriteList((TupperwareList *)atom);
		}
		else if (atom->GetAtomType() == TupperwareAtom::ATOM_SCALAR) 
		{
			WriteScalar((TupperwareScalar *)atom);
		}
	}
	m_parentIndex--;
}

unsigned short TupperwareBinaryWriter::EncodeFlags(int atomType,int dataType,int parentIndex,int hasName,int lengthSize,int extra)
{
	unsigned short result = ((atomType<<ATOM_TYPE_SHIFT)&ATOM_TYPE_MASK) |
							((hasName<<ATOM_HAS_NAME_SHIFT)&ATOM_HAS_NAME_MASK) |
							((dataType<<ATOM_DATA_TYPE_SHIFT)&ATOM_DATA_TYPE_MASK) |
							((parentIndex<<ATOM_PARENT_INDEX_SHIFT)&ATOM_PARENT_INDEX_MASK) |
							((lengthSize<<ATOM_DATA_LENGTH_SIZE_SHIFT)&ATOM_DATA_LENGTH_SIZE_MASK) |
							((extra<<ATOM_DATA_EXTRA_SHIFT)&ATOM_DATA_EXTRA_MASK);
	return result;
}

int TupperwareBinaryWriter::GetLengthByteSize(int length)
{
	if (length<0x100)
		return 1;
	else if (length<0x10000)
		return 2;
	else if (length<0x1000000)
		return 3;
	else return 4;
}

int TupperwareBinaryWriter::GetDataByteSize(int value)
{
	if (value>=0)
	{
		if (value<0x80)
			return 1;
		else if (value<0x8000)
			return 2;
		else if (value<0x800000)
			return 3;
		else return 4;
	}
	else
	{
		if (value>= -0x80)
			return 1;
		else if (value>=-0x8000)
			return 2;
		else if (value>=-0x800000)
			return 3;
		else return 4;
	}
}

int TupperwareBinaryWriter::GetDataByteSize(const int *values,int length)
{
	int minByteSize = 1;
	for (int i=0;i<length;i++)
	{
		int byteSize = GetDataByteSize(values[i]);
		if (byteSize>minByteSize)
			minByteSize = byteSize;
	}
	return minByteSize;
}

void TupperwareBinaryWriter::CopyDataUsingByteSize(void *destPtr,const int *source,int length,int byteDataSize)
{
	switch (byteDataSize)
	{
		case 1:
			{
				unsigned char *savePtr = (unsigned char *)destPtr;
				for (int i=0;i<length;i++)
				{
					*savePtr++ = source[i]&0xff;
				}
			}
			break;
		case 2:
			{
				unsigned short *savePtr = (unsigned short *)destPtr;
				for (int i=0;i<length;i++)
				{
					*savePtr++ = source[i]&0xffff;
				}
			}
			break;
		case 3:
			{
				unsigned char *savePtr = (unsigned char *)destPtr;
				for (int i=0;i<length;i++)
				{
					memcpy(savePtr,&source[i],3);
					savePtr+=3;
				}
			}
			break;
		case 4:
			memcpy(destPtr,source,length*sizeof(int));
			break;
	}
}

//
// Reader code
//
TupperwareBinaryReader::TupperwareBinaryReader(const char *pBuffer, int nBufferLength) :
	m_pBuffer(pBuffer),
	m_nBufferLength(nBufferLength)
{
	m_nBufferPos = 0;
	m_stringManager = new TupperwareStringManager;
}


TupperwareBinaryReader::~TupperwareBinaryReader() 
{
	delete m_stringManager;
}

TupperwareAggregate *TupperwareBinaryReader::Read( const char* fileName)
{
	FILE* f = fopen( fileName, "rb" );

	if ( f == NULL )
		return NULL;

	// get length of file
	fseek(f, 0, SEEK_END);
	int fileLength = ftell(f); 
	fseek(f, 0, SEEK_SET);

	if (fileLength==0)
	{
		fclose( f );
		return NULL;
	}

	// allocate a buffer to hold the entire file
	char *pBuffer = new char[fileLength];
	// read it all in
	int bytesRead = fread(pBuffer,1,fileLength,f);
	fclose( f );

	TupperwareBinaryMainHeader header;

	TupperwareBinaryReader reader(pBuffer,fileLength);
	if (!reader.ReadHeader(header))
	{
		return NULL;
	}
	if (!reader.ReadStringTable(header.m_stringTableLength))
	{
		return NULL;
	}

	TupperwareAggregate *pTree = reader.ReadAtoms(header.m_atomLength);
	delete pBuffer;
	return pTree;
}

TupperwareAggregate *TupperwareBinaryReader::Read( const char* pBuffer, int nBufferLength)
{
	TupperwareBinaryMainHeader header;
	TupperwareBinaryReader reader(pBuffer,nBufferLength);
	if (!reader.ReadHeader(header))
	{
		return NULL;
	}
	if (!reader.ReadStringTable(header.m_stringTableLength))
	{
		return NULL;
	}
	return reader.ReadAtoms(header.m_atomLength);
}

TupperwareAggregate *TupperwareBinaryReader::ReadAtoms(int atomLength)
{
	AtomBinaryChunk atomChunk;
	int atomByteCount = 0;
	TupperwareAggregate *root = NULL;
	int atomType;
	int atomDataType;
	int parentIndex;
	int hasName;
	int lengthSize;
	int extra;
	unsigned short name;

	while (atomByteCount<atomLength)
	{
		if (!ReadBlock(&atomChunk,sizeof(AtomBinaryChunk),1))
		{
			return NULL;
		}
		atomByteCount+=	sizeof(AtomBinaryChunk);
		DecodeFlags(atomChunk.m_flags,&atomType,&atomDataType,&parentIndex,&hasName,&lengthSize,&extra);
		if (hasName)
		{
			if (!ReadBlock(&name,sizeof(unsigned short),1))
			{
				return NULL;
			}
			atomByteCount+=	sizeof(unsigned short);
		}
		switch((TupperwareAtom::AtomType)atomType)
		{
			case TupperwareAtom::ATOM_AGGREGATE:
				{
					TupperwareAggregate *aggregate = new TupperwareAggregate;
					aggregate->SetKey(m_stringManager->GetString(atomChunk.m_keyIndex));
					if (hasName)
						aggregate->SetName(m_stringManager->GetString(name));
					if (parentIndex==0)
					{
						assert(root==NULL);
						root = aggregate;
					}
					else
					{
						m_parentArray.SetSize(parentIndex); // kill off invalid parents
						m_parentArray[m_parentArray.GetSize()-1]->AddChild(aggregate); // append to parent
					}
					m_parentArray.Add(aggregate); // add this aggregate to the list of parents
				}
				break;
			case TupperwareAtom::ATOM_LIST:
				{
					TupperwareList *list = new TupperwareList;
					list->SetKey(m_stringManager->GetString(atomChunk.m_keyIndex));
					if (hasName)
						list->SetName(m_stringManager->GetString(name));
					assert(root);
					int length=0;
					if (!ReadBlock(&length,lengthSize+1,1))
					{
						return NULL;
					}
					atomByteCount+=	lengthSize+1; // adjust bytes read for length
					switch ((TupperwareAtom::DataType)atomDataType)
					{
						case TupperwareAtom::DATA_STRING:
							{
								// length is the number of bytes to read
								char *stringTable = new char[length+1]; // allocate 1 more byte for extra null
								// read in the string table
								if (!ReadBlock(stringTable,length,1))
								{
									return NULL;
								}
								stringTable[length] = 0; // place null at end
								atomByteCount+=	length; // adjust bytes read for string table

								TupArray<char *> stringPtrArray;
								int i=0;
								while(i<length)
								{
									// add pointer to string
									stringPtrArray.Add(&stringTable[i]);
									// adjust to start of next string
									i+=strlen(&stringTable[i])+1;
								}
								list->SetAsString((const char **)&stringPtrArray[0],stringPtrArray.GetSize());
								delete [] stringTable;
							}
							break;
						case TupperwareAtom::DATA_FLOAT:
							{
								float *floatTable = new float[length];
								// read in the floats
								if (!ReadBlock(floatTable,sizeof(float)*length,1))
								{
									return NULL;
								}
								atomByteCount+=	sizeof(float)*length; // adjust bytes read for float table
								list->SetAsFloat(floatTable,length);
								delete [] floatTable;
							}
							break;
						case TupperwareAtom::DATA_INT:
							{
								int tablePackedLength = length*(extra+1); // number of ints * bytes per each
								void *packedData = new char[tablePackedLength];
								int *intTable = new int[length];
								// read in the packed ints
								if (!ReadBlock(packedData,tablePackedLength,1))
								{
									return NULL;
								}
								atomByteCount+=	tablePackedLength; // adjust bytes read for int table
								CopyIntsFromPacked(intTable,packedData,length,extra+1);
								list->SetAsInt(intTable,length);
								delete [] intTable;
								delete[] (char*)packedData;
							}
							break;
					}
					m_parentArray.SetSize(parentIndex); // kill off invalid parents
					m_parentArray[m_parentArray.GetSize()-1]->AddChild(list); // append to parent
				}
				break;
			case TupperwareAtom::ATOM_SCALAR:
				{
					TupperwareScalar *scalar = new TupperwareScalar;
					scalar->SetKey(m_stringManager->GetString(atomChunk.m_keyIndex));
					if (hasName)
						scalar->SetName(m_stringManager->GetString(name));
					assert(root);
					switch ((TupperwareAtom::DataType)atomDataType)
					{
						case TupperwareAtom::DATA_STRING:
							{
								int length=0;
								if (!ReadBlock(&length,lengthSize+1,1))
								{
									return NULL;
								}
								atomByteCount+=	lengthSize+1; // adjust bytes read for length
								// length is the number of bytes to read
								char *string = new char[length+1];
								if (length>0)
								{
									// read in the string table
									if (!ReadBlock(string,length,1))
									{
										return NULL;
									}
								}
								string[length] = 0; // add null to end
								atomByteCount+=	length; // adjust bytes read for string table
								scalar->SetAsString(string);
								delete [] string;
							}
							break;
						case TupperwareAtom::DATA_FLOAT:
							{
								float floatValue;
								// read in the float value
								if (!ReadBlock(&floatValue,sizeof(float),1))
								{
									return NULL;
								}
								atomByteCount+=	sizeof(float); // adjust bytes read for float table
								scalar->SetAsFloat(floatValue);
							}
							break;
						case TupperwareAtom::DATA_INT:
							{
								int intValue=0;
								// read in the int
								if (!ReadBlock(&intValue,extra+1,1))
								{
									return NULL;
								}
								FixPackedInt(&intValue,extra+1); // make sure it is correct
								atomByteCount+=	extra+1; // adjust bytes read for float table
								scalar->SetAsInt(intValue);
							}
							break;
						case TupperwareAtom::DATA_BINARY:
							{
								int length=0;
								if (!ReadBlock(&length,lengthSize+1,1))
								{
									return NULL;
								}
								atomByteCount+=	lengthSize+1; // adjust bytes read for length
								void *binaryData = new char[length];
								// read in the binary data
								if (!ReadBlock(binaryData,length,1))
								{
									return NULL;
								}
								atomByteCount+=	length; // adjust bytes read for binary data
								scalar->SetAsBinary(binaryData,length);
								delete[] (char*) binaryData;
							}
							break;
					}
					m_parentArray.SetSize(parentIndex); // kill off invalid parents
					m_parentArray[m_parentArray.GetSize()-1]->AddChild(scalar); // append to parent
				}
				break;
		}
	}
	return root;
}

int TupperwareBinaryReader::GetCRC( const char* fileName, unsigned int *cacheCRC)
{
	FILE* f = fopen( fileName, "rb" );

	if ( f == NULL )
		return 0;

	// fill the main header
	TupperwareBinaryMainHeader header;
	if (!fread(&header,sizeof(TupperwareBinaryMainHeader),1,f))
	{
		fclose(f);
		return 0;
	}
	if (header.m_magicNumber != TUPPERWARE_BINARY_FILE_MAGIC_NUMBER ||
		header.m_revNumber != TUPPERWARE_BINARY_FILE_CURRENT_REVISION)
	{
		fclose(f);
		return 0;
	}
	*cacheCRC = header.m_cacheCRC;
	fclose(f);
	return 1;
}

bool TupperwareBinaryReader::ValidBinaryFile(const char *fileName)
{
	FILE* f = fopen( fileName, "rb" );

	if ( f == NULL )
		return false;

	// fill the main header
	TupperwareBinaryMainHeader header;
	if (!fread(&header,sizeof(TupperwareBinaryMainHeader),1,f))
	{
		fclose(f);
		return false;
	}
	if (header.m_magicNumber != TUPPERWARE_BINARY_FILE_MAGIC_NUMBER ||
		header.m_revNumber != TUPPERWARE_BINARY_FILE_CURRENT_REVISION)
	{
		fclose(f);
		return false;
	}
	fclose(f);
	return true;
}

bool TupperwareBinaryReader::ValidBinaryFile(const char *pBuffer,int nBufferLength)
{
	TupperwareBinaryMainHeader header;

	TupperwareBinaryReader reader(pBuffer,nBufferLength);
	if (!reader.ReadHeader(header))
	{
		return false;
	}
	return true;
}

// read the string table from the buffer
bool TupperwareBinaryReader::ReadStringTable(int stringTableLength)
{
	if (stringTableLength)
	{
		char *pBuffer = new char[stringTableLength];
		// read in the strings into the buffer
		if (!ReadBlock(pBuffer,stringTableLength,1))
		{
			delete [] pBuffer;
			return false;
		}
		m_stringManager->BuildFromBuffer(pBuffer,stringTableLength);
		delete [] pBuffer;
	}
	return true;
}

// read and check header from the buffer
bool TupperwareBinaryReader::ReadHeader(TupperwareBinaryMainHeader &header)
{
	// read in the header
	if (!ReadBlock(&header,sizeof(TupperwareBinaryMainHeader),1))
	{
		return false;
	}
	if (header.m_magicNumber != TUPPERWARE_BINARY_FILE_MAGIC_NUMBER ||
		header.m_revNumber != TUPPERWARE_BINARY_FILE_CURRENT_REVISION)
	{
		return false;
	}
	return true;
}

// read a block of data from the buffer
int TupperwareBinaryReader::ReadBlock(void *pData,int nElementSize,int nElements)
{
	int len = nElementSize * nElements;

	if (m_nBufferPos != m_nBufferLength)
	{
		if (m_nBufferLength - m_nBufferPos > len) 
		{
			memcpy(pData, &m_pBuffer[m_nBufferPos], len);
			m_nBufferPos += len;
			return nElements;
		} 
		else 
		{
			int ret = m_nBufferLength - m_nBufferPos;
			memcpy(pData, &m_pBuffer[m_nBufferPos], ret);
			m_nBufferPos = m_nBufferLength;
			return ret/nElementSize;
		}
	}
	else 
	{
		return 0;
	}
}



void TupperwareBinaryReader::DecodeFlags(unsigned short flags,int *atomType,int *dataType,int *parentIndex,int *hasName,int *lengthSize,int *extra)
{
	*atomType = (flags&ATOM_TYPE_MASK)>>ATOM_TYPE_SHIFT;
	*dataType = (flags&ATOM_DATA_TYPE_MASK)>>ATOM_DATA_TYPE_SHIFT;
	*parentIndex = (flags&ATOM_PARENT_INDEX_MASK)>>ATOM_PARENT_INDEX_SHIFT;
	*hasName = (flags&ATOM_HAS_NAME_MASK)>>ATOM_HAS_NAME_SHIFT;
	*lengthSize = (flags&ATOM_DATA_LENGTH_SIZE_MASK)>>ATOM_DATA_LENGTH_SIZE_SHIFT;
	*extra = (flags&ATOM_DATA_EXTRA_MASK)>>ATOM_DATA_EXTRA_SHIFT;
}

void TupperwareBinaryReader::CopyIntsFromPacked(int *intTable,void *packedData,int length,int byteDataSize)
{
	switch (byteDataSize)
	{
		case 1:
			{
				char *sourcePtr = (char *)packedData;
				int *intPtr = intTable;
				for (int i=0;i<length;i++)
				{
					*intPtr = *sourcePtr;
					intPtr++;
					sourcePtr++;
				}
			}
			break;
		case 2:
			{
				short *sourcePtr = (short *)packedData;
				int *intPtr = intTable;
				for (int i=0;i<length;i++)
				{
					*intPtr = *sourcePtr;
					intPtr++;
					sourcePtr++;
				}
			}
			break;
		case 3:
			{
				unsigned char *sourcePtr = (unsigned char *)packedData;
				int *intPtr = intTable;
				for (int i=0;i<length;i++)
				{
					memcpy(intPtr,sourcePtr,3);
					if (*intPtr&0x00800000)
						*intPtr|=0xff000000; // set the upper byte correctly
					else
						*intPtr&=0x00ffffff;
					sourcePtr+=3;
					intPtr++;
				}
			}
			break;
		case 4:
			memcpy(intTable,packedData,length*sizeof(int));
			break;
	}
}

void TupperwareBinaryReader::FixPackedInt(int *intValue,int byteDataSize)
{
	switch (byteDataSize)
	{
		case 1:
			if (*intValue & 0x80)
			{
				*intValue |= 0xffffff00;
			}
			else
			{
				*intValue &= 0x000000ff;
			}
			break;
		case 2:
			if (*intValue & 0x8000)
			{
				*intValue |= 0xffff0000;
			}
			else
			{
				*intValue &= 0x0000ffff;
			}
			break;
		case 3:
			if (*intValue & 0x800000)
			{
				*intValue |= 0xff000000;
			}
			else
			{
				*intValue &= 0x00ffffff;
			}
			break;
		case 4:
			break;
	}
}

