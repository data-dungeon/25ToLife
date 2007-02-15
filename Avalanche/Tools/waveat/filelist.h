//***************************************************************************
// Build a list of files
//***************************************************************************
#ifndef FILELIST_H
#define FILELIST_H

#include <container\dynlist.h>
#include <toolhelper\dynstr.h>

class FileList
{
public:
	FileList(void);
	~FileList(void);

	// Clear the list 
	inline void Clear(void)
		{ files.Clear(); }

	// Build it (if not Clear'd, it appends)
	// ext = NULL means all files.
	// Note: ext is the EXTENSION!  i.e. scr, dbl, NOT *.scr *.dbl
	bool Build(char *dir, char *ext, bool recurse = false);

	// Add a single file to the list
	bool AddFile(char *pathFile);
	
	// Delete all files with the given ext
	bool DeleteFiles(char *ext);

	// How many files did we find?
	inline uint Count(void)
		{ return(files.Count()); }

	// Get them!
	inline DynString *Get(uint index)
		{ return(files[index]); }

private:
	DynList<DynString>	files;

	// Append this dir to the list
	bool Append(char *path, char *ext, bool recurse);
};

#endif
