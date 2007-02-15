//*************************************************************************
// Finally, a utility to manipulate paths and files
//*************************************************************************

/******************* includes ****************************/

#include "EngineHelper/EngineHelperPCH.h"





//***************************************************************************
// Does this file have an extension?
//***************************************************************************
bool PathFile::HasExt(
const char *pathFile)
{
	int len = (int)strlen(pathFile);
	while (--len > 0 && pathFile[len] != '\\')
		if (pathFile[len] == '.')
			return(true);

	return(false);
}

//***************************************************************************
// Strip off the extension on a file
//***************************************************************************
void PathFile::StripExt(
char *pathFile)
{
	int len = (int)strlen(pathFile);
	while (--len > 0 && pathFile[len] != '\\')
	{
		if (pathFile[len] == '.')
		{
			pathFile[len] = '\0';
			break;
		}
	}
}

//***************************************************************************
// Terminate the path with a slash
//***************************************************************************
void PathFile::TerminatePath(
char *pathFile)
{
	if (pathFile[strlen(pathFile) - 1] != '\\')
		strcat(pathFile, "\\");
}

//***************************************************************************
// Returns pathFile
//***************************************************************************
char *PathFile::MakePath(
char *pathFile,
const char *path,
const char *file,
const char *ext)
{
	if (!path)
		pathFile[0] = '\0';
	else
	{
		strcpy(pathFile, path);
		if (pathFile[strlen(pathFile) - 1] != '\\')
			strcat(pathFile, "\\");
	}

	strcat(pathFile, file);

	if ((ext != NULL) && !(strstr(file, ".")))
	{
		if (ext[0] != '.')
			strcat(pathFile, ".");
		strcat(pathFile, ext);
	}

	return(pathFile);
}

//***************************************************************************
// Returns pathFile forcing the extension
//***************************************************************************
char *PathFile::MakePathForceExt(
char *pathFile,
const char *path,
const char *file,
const char *ext)
{
	if (!path)
		pathFile[0] = '\0';
	else
	{
		strcpy(pathFile, path);
		if (pathFile[strlen(pathFile) - 1] != '\\')
			strcat(pathFile, "\\");
	}

	strcat(pathFile, file);

	if (ext != NULL)
	{
		if (HasExt(pathFile))
			StripExt(pathFile);
		if (ext[0] != '.')
			strcat(pathFile, ".");
		strcat(pathFile, ext);
	}

	return(pathFile);
}

//***************************************************************************
// does the passed path file have the extension?
//***************************************************************************
bool PathFile::ExtMatch(
const char *pathFile,
const char *ext)
{
	// find the extension
	int len = (int)strlen(pathFile);
	while (--len > 0 && pathFile[len] != '\\')
	{
		if (pathFile[len] == '.')
			return(strcmpi(&pathFile[len + 1], ext) == 0);
	}

	// no match
	return(false);
}

//***************************************************************************
// Clean a pathfile (removes all .\'s)
//***************************************************************************
void PathFile::CleanPathFile(
char *pathFile)
{	
	int len = (int)strlen(pathFile);
	int i = 0;
	while (i < len)
	{
		// Our friend?
		if (pathFile[i] == '.' && pathFile[i + 1] == '\\')
		{
			// delete these two	
			memcpy(&pathFile[i], &pathFile[i + 2], len - i - 2);
			len -= 2;
			pathFile[len] = '\0';
		}
		else
			++i;
	}
}

//***************************************************************************
// Force a file extention (pass in '.'-whatever for the ext)
//***************************************************************************
void PathFile::ForceExtension(
char *pathFile,
const char *newExt)
{
	StripExt(pathFile);
	if (newExt && *newExt)
		strcat(pathFile, newExt);
}

//***************************************************************************
// Pick off just the filename
//***************************************************************************
void PathFile::JustFileName(
const char *pathFile,
char *file,
bool wantExt)
{
	// Save the length
	int len = (int)strlen(pathFile) - 1;

	// Find the beginning of the name, or a slash
	while (len > 0 && pathFile[len] != '\\')
		--len;
	if (pathFile[len] == '\\')
		++len;

	// Now copy to the dot or the end
	int fileLen = 0;
	while (pathFile[len] != '\0' && (wantExt || pathFile[len] != '.'))
		file[fileLen++] = pathFile[len++];
	file[fileLen] = '\0';
}
