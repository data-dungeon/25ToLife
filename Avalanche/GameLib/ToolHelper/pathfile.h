//*************************************************************************
// Finally, a utility to manipulate paths and files
//*************************************************************************
#ifndef PATHFILE_H
#define PATHFILE_H

#include <string.h>

class PathFile
{
public:
	// Does this path file have an extension?
	static bool HasExt(const char *pathFile);

	// strip off the extension
	static void StripExt(char *pathFile);

	// Terminate the path with a slash
	static void TerminatePath(char *pathFile);

	// Make a pathfile from a path, file and optional ext
	static char *MakePath(char *pathFile, const char *path, const char *file, const char *ext = NULL);

	// Returns pathFile forcing the extension
	static char *MakePathForceExt(char *pathFile, const char *path, const char *file, const char *ext);

	// does the passed path file have the extension?
	static bool ExtMatch(const char *pathFile, const char *ext);

	// Clean a pathfile (removes all .\'s)
	static void CleanPathFile(char *pathFile);

	// Force a file extention (pass in '.'-whatever for the ext)
	static void ForceExtension(char *pathFile, const char *newExt);

	// Pick off just the filename
	static void JustFileName(const char *pathFile, char *file, bool wantExt = false);
};

#endif
