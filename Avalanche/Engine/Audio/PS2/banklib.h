/***************************************************************************/
// Manage a list of banks stored in hogs
/***************************************************************************/
#ifndef BANKLIB_H
#define BANKLIB_H

#include "Layers/mstream/mstream.h"
#include "EngineHelper/hog.h"

// We create a whole list of these
class BankLibraryEntry
{
public:
	char					name[20];		// The bank name
	int					fileID;			// IOP file ID
	int					offset;			// -1 if invalid
	int					size;
};

// The stuff for a stream volume
class BankLibrary
{
public:
	BankLibrary();
	~BankLibrary()
		{ Unmount(); }

	// Mount us
	bool Mount();

	// Unmount us
	void Unmount();

	// file size
	int Size()
		{ return m_size; }

	// Find an entry, returns NULL if not there
	const BankLibraryEntry *Find(const char *name);

private:
	int						m_fileID;		// Our IOP file ID
	int						m_size; 			// overall file size
	BankLibraryEntry 		*m_entry; 		// Our array of entries
	int						m_entries;		// How many entries are there?

	bool BuildEntries(Hog &hog);
};

#endif


