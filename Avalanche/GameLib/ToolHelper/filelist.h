//***************************************************************************
// Build a list of files
//***************************************************************************
#ifndef FILELIST_H
#define FILELIST_H

#include "container/dynlist.h"
#include "ToolHelper/dynstr.h"

class FileList
{
public:
	FileList();
	~FileList();

	// Clear the list
	inline void Clear()
		{ files.Clear(); }

	// Build it (if not Clear'd, it appends)
	// ext = NULL means all files.
	// Note: ext is the EXTENSION!  i.e. scr, dbl, NOT *.scr *.dbl
	bool Build(const char *dir, const char *ext, bool recurse = false);

	// Add a single file to the list
	bool AddFile(const char *pathFile);

	// Delete all files with the given ext
	bool DeleteFiles(const char *ext);

	// Delete all files with the given ext
	bool DeleteSubstring(const char *substring);

	// How many files did we find?
	inline uint Count()
		{ return(files.Count()); }

	// Get them!
	inline DynString *Get(uint index)
		{ return(files[index]); }

private:
	DynList<DynString>	files;

	// Append this dir to the list
	bool Append(const char *path, const char *ext, bool recurse);

	// A substring test
	static const char *strstri(const char *string, const char *key);
};

#endif
