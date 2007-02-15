/***************************************************************************/
// Manage a list of streams stored in hogs
/***************************************************************************/
#ifndef STREAMLIB_H
#define STREAMLIB_H

#include "Layers/mstream/mstream.h"
#include "EngineHelper/hog.h"
#include "container/dllist.h"

// We create a whole list of these
class StreamVolumeEntry
{
public:
	SOUND_MIH_INFO    header;			// The header info
	int					fileID;			// IOP file ID
	int					offset;			// -1 if invalid
	int					size;
};

#pragma pack(0)
struct SMALL_SOUND_MIH_INFO
{
	u8		NumberOfFiles;			/* Number of files interleaved into a single .MIB file */
	u16	Tracks;					/* Amount of Tracks contained in .MIB file */
	u16 	Pitch;					/* Pitch (in hz) to play streams in .MIB file */
	u16 	EndOffset;				/* Offset from start of final packet to end of data. */
	u32	TrackSize;				/* .MIB Track size */
};
#pragma pack()

// The stuff for a stream volume
class StreamVolume
{
public:
	StreamVolume();
	~StreamVolume()
		{ Unmount(); }

	// Mount us
	bool Mount(const char *libName);

	// Unmount us
	void Unmount();

	// Find an entry, returns false if not there
	bool Find(const char *name, StreamVolumeEntry &entry);

	// Find a const ref to a string, NULL if not there
	const char *GetTokenPtr(const char *name);

private:
	int						m_fileID;		// Our IOP file ID

	class VolumeEntry
	{
	public:
		char						name[14];
		SMALL_SOUND_MIH_INFO header;			// The header info
		int						offset;			// -1 if invalid
		int						size;
	};

	VolumeEntry					*m_entry; 		// Our array of entries
	int							m_entries;		// How many entries are there?

	bool BuildEntries(Hog &header, Hog &data);
	static int VolumeEntryCompare(const void *e1, const void *e2);
	bool LoadHeader(Hog &header, const char *file, SOUND_MIH_INFO &info);
	void StripFilename(const char *pathFile, char *file);

	void ConvertMIH(const SMALL_SOUND_MIH_INFO &small, SOUND_MIH_INFO &big);
	void ConvertMIH(const SOUND_MIH_INFO &big, SMALL_SOUND_MIH_INFO &small);

public:
	StreamVolume			*prev, *next;
};

// The stream library (a list of stream volumes)
class StreamLibrary
{
public:
	StreamLibrary();
	~StreamLibrary()
		{ UnmountAll(); }

	// Mount us
	bool Mount(const char *libName);

	// Unmount everything
	void UnmountAll();

	// Find an entry, returns NULL if not there
	bool Find(const char *name, StreamVolumeEntry &entry);

	// Find a const ref to a string, NULL if not there
	const char *GetTokenPtr(const char *name);

private:
	// The list of volumes
	DoublyLinkedList<StreamVolume>	m_volumeList;
};

#endif


