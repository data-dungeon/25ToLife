/***************************************************************************/
// Manage a list of streams stored in hogs
/***************************************************************************/
#include "Audio/AudioPCH.h"

/***************************************************************************/
/***************************************************************************/
BankLibrary::BankLibrary()
{
	m_fileID = -1;
	m_entry = NULL;
	m_entries = 0;
}

/***************************************************************************/
// Mount us
/***************************************************************************/
bool BankLibrary::Mount()
{
	// The filename
	char pathFile[256];
	strcpy(pathFile, "sound\\spu.hog");

	// Load the hog into memory
	Hog hog;
	bool ret = hog.Mount(pathFile, false);
	if (!ret)
		return false;
	hog.OverrideCheckLocal(CHECKLOCAL_NEVER);

	// Process the data
	ret = BuildEntries(hog);

	// Close them
	hog.Unmount();

	// Did the build work?
	if (!ret)
		return false;

	// Create a fileID for a file (return -1 if not there)
	char nativeFileName[128];
	Media::MakeFileName(pathFile, nativeFileName);
	TRACE("Mounting bank hog %s ", nativeFileName);
	m_fileID = MultiStream::OpenFileID(nativeFileName, &m_size);
	TRACE("fileID = %d, size = %d\n", m_fileID, m_size);
	ASSERTS(m_fileID >= 0, "IOP cound not mount bank hog!");
	if (m_fileID < 0)
		return false;

	// Force the file ID
	for (int i = 0; i < m_entries; i++)
		m_entry[i].fileID = m_fileID;

	// Out of here
	return true;
}

/***************************************************************************/
// Unmount us
/***************************************************************************/
void BankLibrary::Unmount()
{
	// Close the IOP file
	MultiStream::CloseFileID(m_fileID);

	// Kill the array
	delete [] m_entry;
	m_entry = NULL;
	m_entries = 0;
}

/***************************************************************************/
// Find an entry, returns NULL if not there
/***************************************************************************/
const BankLibraryEntry *BankLibrary::Find(
const char *name)
{
	// Should do a bsearch here
	for (int i = 0; i < m_entries; i++)
	{
		if (strcmpi(name, m_entry[i].name) == 0 && m_entry[i].offset >= 0)
			return &m_entry[i];
	}

	return NULL;
}

/***************************************************************************/
/***************************************************************************/
bool BankLibrary::BuildEntries(
Hog &hog)
{
	// How many files are there?
	m_entries = hog.Files();
	if (!m_entries)
		return false;

	// Okay create the array
	m_entry = new BankLibraryEntry[m_entries];
	if (!m_entry)
		return false;

	// Build up the array
	for (int e = 0; e < m_entries; e++)
	{
		const char *fileName = hog.FileName(e);
		ASSERT(fileName);

		// Build up the entry
		strcpy(m_entry[e].name, fileName);
		m_entry[e].fileID = -1;
		m_entry[e].offset = -1; // assume it didn't work

		// Read the offsets
		if (!hog.FileOffsetSize(fileName, m_entry[e].offset, m_entry[e].size))
		{
			ASSERT(false);
			continue;
		}
	}

	// done
	return true;
}
