///////////////////////////////////////////////////////////////////////////////
// Wee!
///////////////////////////////////////////////////////////////////////////////
#include "EngineHelper/EngineHelperPCH.h"
#include "EngineHelper/filestress.h"

///////////////////////////////////////////////////////////////////////////////
// You must call LoadMaster or use Test(fileName) later
///////////////////////////////////////////////////////////////////////////////
FileStress::FileStress()
{
	m_filename[0] = '\0';
	m_data = NULL;
	m_size = 0;
}

///////////////////////////////////////////////////////////////////////////////
// load the master copy of the file
///////////////////////////////////////////////////////////////////////////////
FileStress::FileStress(
const char *fileName)
{
	m_filename[0] = '\0';
	m_data = NULL;
	m_size = 0;

	LoadMaster(fileName);
}

///////////////////////////////////////////////////////////////////////////////
	// Done
///////////////////////////////////////////////////////////////////////////////
FileStress::~FileStress()
{
	if (m_data)
		memFree(m_data);
}

///////////////////////////////////////////////////////////////////////////////
// Load the master copy of the file (only does it once)
///////////////////////////////////////////////////////////////////////////////
bool FileStress::LoadMaster(
const char *fileName)
{
	// Duh
	if (!fileName || fileName[0] == '\0')
		return false;

	// Already loaded?
	if (strcmp(fileName, m_filename) == 0)
		return (m_data != NULL);
	else if (m_filename[0]) // Can't change files
		return false;

	// Do it!
	ASSERT(strlen(fileName) < sizeof(m_filename));
	strcpy(m_filename, fileName);

	// Weee!
	FileStream stream;
	if (!stream.Open(m_filename, FileStream::READ))
		return false;

	// How big is this beotch!
	m_size = stream.Size();
	if (m_size <= 0)
		return false;

	// Make it
	m_data = (char *)memAlloc(m_size);
	ASSERT(m_data);

	// Read the data
	int length = stream.Read(m_data, m_size);
	ASSERT(length == m_size);
	if (length < m_size)
	{
		memFree(m_data);
		m_data = NULL;
	}

	// We are done!
	stream.Close();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Read the file again and test the results
///////////////////////////////////////////////////////////////////////////////
bool FileStress::Test()
{
	// Anything to test?
	if (!m_data)
		return false;

	// Load it in
	FileStream stream;
	bool openFile = stream.Open(m_filename, FileStream::READ);
	ASSERT(openFile);
	if (!openFile)
		return false;

	// How big is this beotch!
	int size = stream.Size();
	ASSERT(size == m_size);
	if (size != m_size)
		return false;

	// Make it
	char *newData = (char *)memAlloc(size);
	ASSERT(newData);
	if (!newData)
		return true;

	// Read the data
	int length = stream.Read(newData, size);
	stream.Close();

	// Did we get enough data?
	ASSERT(length == size);
	if (length < m_size)
	{
		memFree(newData);
		return false;
	}

	// Now the real test
	bool isSame = (memcmp(m_data, newData, m_size) == 0);
	ASSERT(isSame);

	memFree(newData);
	return isSame;
}

///////////////////////////////////////////////////////////////////////////////
// Free everyting
///////////////////////////////////////////////////////////////////////////////
void FileStress::Unload()
{
	if (m_data)
		memFree(m_data);

	m_filename[0] = '\0';
	m_data = NULL;
	m_size = 0;
}



