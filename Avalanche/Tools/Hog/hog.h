//*************************************************************************
// Virtual file system
//*************************************************************************
#ifndef HOG_H
#define HOG_H

#define NOLAYERS

#include <container\darray.h>
#include <toolhelper\dynstr.h>
#include <toolhelper\filelist.h>
#include <toolhelper\cmdline.h>

#include "stats.h"
#include "hogtoc.h"

class Hog
{
public:
	Hog(int argc, char *argv[]);
	~Hog();

	// Terminate
	void Process();

private:
	// Internal proto's
	bool ParseCommandLine(int argc, char *argv[]);
	bool EnumerateFilesDirs();
	bool EnumerateKills();
	bool EnumerateSubstringKills();
	bool EnumeratePathFilters();
	bool EnumerateMasks(char *forDir, bool recurse);
	bool BuildDir(char *dir, char *ext, bool recurse);

	void ErrorExit(char *reason, int code = 1);
	void UsageExit(bool full = false, int code = 99);

	// process a file
	bool ProcessFile(char *file);

	// Does this file already exist in the TOC?
	int FileExistsInTOC(const char *file);

	// Write the datafile
	bool WriteFile(char *outFile);

	// Build the report
	bool WriteReport(char *reportFile);

	// Write the listing
	bool WriteListing(char *listFile);

	// The command line parser
	CommandLine							commandLine;

	// Our status display
	Stats									stats;

	// The main parameters
	char 									*outfile;
	char									*reportfile;
	char									*listfile;
	char									*baseDir;
	char									*startDir;
	int									alignment;
	bool									justFileName;
	bool									outputBigEndian;
	bool									beQuiet;
	bool									waitOnError;
	bool									encrypt;

	// stats
	int									totalFileSize;

	// the directory tree
	FileList								fileList;

	// Stuff for path filters
	DynList<DynString>				pathFilter;
	void FilterPath(char *file);

	// The unsorted contents
	struct TOCENTRY
	{
		char		*name;
		void		*data;
		uint		size;
		uint		dataOffset;
		uint		crc;
	};
	DynamicArray<TOCENTRY>			contents;
	DynList<DynString>				originalFile;

	// The index for sorting the contents
	struct TOCINDEX
	{
		TOCENTRY							*entry;
	};
	TOCINDEX *BuildIndex();
	static int __cdecl TOCINDEX_Compare(const void *e1, const void *e2);

	// The errors we encountered
	DynList<DynString>				errors;
	bool AddError(char *error, char *file, bool warning = false);

	// Utility for computing a CRC
	static uint ComputeCRC(const void *data, int size);
};

#endif
