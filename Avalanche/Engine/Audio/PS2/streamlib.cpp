/***************************************************************************/
// Manage a list of streams stored in hogs
/***************************************************************************/
#include "Audio/AudioPCH.h"

/***************************************************************************/
/***************************************************************************/
StreamVolume::StreamVolume()
{
	m_fileID = -1;
	m_entry = NULL;
	m_entries = 0;
}

/***************************************************************************/
// Mount us
/***************************************************************************/
bool StreamVolume::Mount(
const char *libName)
{
	// The filename
	char pathFile[256];

	// Load the MIH hog into memory
	Hog header;
	sprintf(pathFile, "stream\\%sh.hog", libName);
	//TRACE("Mounting stream hog %s.\n", pathFile);
	bool ret = header.Mount(pathFile);
	if (!ret)
		return false;
	header.OverrideCheckLocal(CHECKLOCAL_NEVER);

	// Mount the data hog
	Hog data;
	sprintf(pathFile, "stream\\%sd.hog", libName);
	//TRACE("Mounting stream hog %s.\n", pathFile);
	ret = data.Mount(pathFile, false);
	if (!ret)
		return false;
	data.OverrideCheckLocal(CHECKLOCAL_NEVER);

	// Process the data
	//TRACE("Building stream hog entries list.\n");
	ret = BuildEntries(header, data);

	// Close them
	header.Unmount();
	data.Unmount();

	// Did the build work?
	if (!ret)
		return false;

	// Create a fileID for a file (return -1 if not there)
	char nativeFileName[128];
	Media::MakeFileName(pathFile, nativeFileName);
	TRACE("Mounting IOP stream hog %s ", nativeFileName);
	m_fileID = MultiStream::OpenFileID(nativeFileName);
	TRACE("fileID = %d.\n", m_fileID);
	ASSERTS(m_fileID >= 0, "IOP cound not mount data hog!");
	if (m_fileID < 0)
		return false;

	// Out of here
	return true;
}

/***************************************************************************/
// Unmount us
/***************************************************************************/
void StreamVolume::Unmount()
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
bool StreamVolume::Find(
const char *name,
StreamVolumeEntry &entry)
{
	char stripped[32];
	StripFilename(name, stripped);

	VolumeEntry key;
	ASSERT(strlen(stripped) <= sizeof(key.name));
	memset(key.name, '\0', sizeof(key.name));
	strncpy(key.name, stripped, sizeof(key.name));

	const VolumeEntry *found = (const VolumeEntry *)bsearch(&key, m_entry, m_entries, sizeof(VolumeEntry), VolumeEntryCompare);
	if (!found)
		return false;

	// We found it!
	ConvertMIH(found->header, entry.header);
	entry.fileID = m_fileID;
	entry.offset = found->offset;
	entry.size = found->size;
	return true;
}

/***************************************************************************/
// Find a const ref to a string, NULL if not there
/***************************************************************************/
const char *StreamVolume::GetTokenPtr(
const char *name)
{
	char stripped[32];
	StripFilename(name, stripped);

	VolumeEntry key;
	ASSERT(strlen(stripped) <= sizeof(key.name));
	memset(key.name, '\0', sizeof(key.name));
	strncpy(key.name, stripped, sizeof(key.name));

	const VolumeEntry *found = (const VolumeEntry *)bsearch(&key, m_entry, m_entries, sizeof(VolumeEntry), VolumeEntryCompare);
	if (!found)
		return NULL;

	// Give them the token!
	return found->name;
}

/***************************************************************************/
/***************************************************************************/
bool StreamVolume::BuildEntries(
Hog &header,
Hog &data)
{
	// How many files are there?
	m_entries = header.Files();
	if (!m_entries)
		return false;

	// Okay create the array
	m_entry = new VolumeEntry[m_entries];
	if (!m_entry)
		return false;

	// Build up the array
	for (int e = 0; e < m_entries; e++)
	{
		const char *fileName = header.FileName(e);
		ASSERT(fileName);

		// Build up the entry
		char stripped[32];
		StripFilename(fileName, stripped);
		ASSERT(strlen(stripped) <= sizeof(m_entry[e].name));
		memset(m_entry[e].name, '\0', sizeof(m_entry[e].name));
		strncpy(m_entry[e].name, stripped, sizeof(m_entry[e].name));
		m_entry[e].offset = -1; // assume it didn't work
		m_entry[e].size = -1; // assume it didn't work

		// Read the header
		SOUND_MIH_INFO mihHeader;
		if (!LoadHeader(header, fileName, mihHeader))
		{
			TRACE("***** Warning: Stream (MIH) is weird %s\n", fileName);
			continue;
		}
		ConvertMIH(mihHeader, m_entry[e].header);

		// Read the offsets
		char dataFile[256];
		strcpy(dataFile, fileName);
		dataFile[strlen(dataFile) - 1] = 'B';
		if (!data.FileOffsetSize(dataFile, m_entry[e].offset, m_entry[e].size))
		{
			TRACE("***** Warning: data (MIB) missing for %s\n", dataFile);
			continue;
		}
	}

	// Sort
	qsort(m_entry, m_entries, sizeof(VolumeEntry), VolumeEntryCompare);

	// done
	return true;
}

/***************************************************************************/
/***************************************************************************/
int StreamVolume::VolumeEntryCompare(
const void *e1,
const void *e2)
{
	const VolumeEntry *entry1 = (const VolumeEntry *)e1;
	const VolumeEntry *entry2 = (const VolumeEntry *)e2;

	return strncmp(entry1->name, entry2->name, sizeof(entry1->name));
}

/***************************************************************************/
/***************************************************************************/
bool StreamVolume::LoadHeader(
Hog &header,
const char *file,
SOUND_MIH_INFO &info)
{
	// Open it
	HOGFILE hf = header.FileOpen(file);
	if (!hf)
		return false;

	// How big is the file?
	int size = header.FileSize(hf);

	// Make sure it is less than our statically allocated buffer
	char streamHeaderBuffer[128];
	ASSERT(size < sizeof(streamHeaderBuffer));

	// Whoa something is wrong (probably can't happen with hog)
	if (size <= 0)
	{
		header.FileClose(hf);
		return false;
	}

	// Read in the block
	bool ret = header.FileRead(streamHeaderBuffer, size, hf);
	header.FileClose(hf);

	// Failure on reading
	if (!ret)
		return false;

	// Convert it
	SOUND_GetMIHInfo((u_int)streamHeaderBuffer, &info, 0);

	return true;
}

/***************************************************************************/
/***************************************************************************/
void StreamVolume::StripFilename(
const char *pathFile,
char *file)
{
	// Save the length
	int len = strlen(pathFile) - 1;

	// Find the beginning of the name, or a slash
	while (len > 0 && pathFile[len] != '\\')
		--len;
	if (pathFile[len] == '\\')
		++len;

	// Now copy to the dot or the end
	int fileLen = 0;
	while (pathFile[len] != '\0' && pathFile[len] != '.')
		file[fileLen++] = pathFile[len++];
	file[fileLen] = '\0';

	// uppercase for faster compares
	strupr(file);
}

/***************************************************************************/
/***************************************************************************/
void StreamVolume::ConvertMIH(
const SMALL_SOUND_MIH_INFO &small,
SOUND_MIH_INFO &big)
{
	big.MIHSize = sizeof(big);
	big.MIHPad = 0;
	big.NumberOfFiles = small.NumberOfFiles;
	big.ID = 0;
	big.TrackSize = small.TrackSize;
	big.Tracks = small.Tracks;
	big.Pitch = small.Pitch;
	big.EndOffset = small.EndOffset;
}

/***************************************************************************/
/***************************************************************************/
void StreamVolume::ConvertMIH(
const SOUND_MIH_INFO &big,
SMALL_SOUND_MIH_INFO &small)
{
	ASSERT(big.NumberOfFiles < 255);
	small.NumberOfFiles = big.NumberOfFiles;

	ASSERT(big.Tracks < 0xffff);
	small.Tracks = big.Tracks;

	ASSERT(big.Pitch < 0xffff);
	small.Pitch = big.Pitch;

	ASSERT(big.EndOffset < 0xffff);
	small.EndOffset = big.EndOffset;

	small.TrackSize = big.TrackSize;
}

/***************************************************************************/
// The stream library (a list of stream volumes)
/***************************************************************************/
StreamLibrary::StreamLibrary() :
m_volumeList(false)
{
}

/***************************************************************************/
// Mount us
/***************************************************************************/
bool StreamLibrary::Mount(
const char *libName)
{
	// Create a new one
	StreamVolume *volume = new StreamVolume();
	if (!volume)
		return false;

	// Mount it
	if (!volume->Mount(libName))
	{
		delete volume;
		return false;
	}

	// Add it to the list
	m_volumeList.AddTail(volume);
	return true;
}

/***************************************************************************/
// Unmount everything
/***************************************************************************/
void StreamLibrary::UnmountAll()
{
	StreamVolume *volume = m_volumeList.Head();
	while (volume)
	{
		// Clean it up
		m_volumeList.Unlink(volume);
		volume->Unmount();
		delete volume;

		// next!
		volume = m_volumeList.Head();
	}
}

/***************************************************************************/
// Find an entry, returns NULL if not there
/***************************************************************************/
bool StreamLibrary::Find(
const char *name,
StreamVolumeEntry &entry)
{
	StreamVolume *volume = m_volumeList.Head();
	while (volume)
	{
		// Here?
		if (volume->Find(name, entry))
			return true;

		// Try the next one
		volume = volume->next;
	}

	// Not in any of the volumes
	ASSERT(false);
	return false;
}


/***************************************************************************/
// Find a const ref to a string, NULL if not there
/***************************************************************************/
const char *StreamLibrary::GetTokenPtr(
const char *name)
{
	StreamVolume *volume = m_volumeList.Head();
	while (volume)
	{
		// Here?
		const char *token = volume->GetTokenPtr(name);
		if (token)
			return token;

		// Try the next one
		volume = volume->next;
	}

	// Not in any of the volumes
	return NULL;
}




