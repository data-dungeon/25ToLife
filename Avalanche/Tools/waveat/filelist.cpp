//***************************************************************************
// Build a list of files
//***************************************************************************
#ifdef WIN32
#include <io.h>
#endif

#include <toolhelper\pathfile.h>
#include <toolhelper\filelist.h>

//***************************************************************************
//***************************************************************************
FileList::FileList(void) :
files(64, 16, true)
{
}

//***************************************************************************
//***************************************************************************
FileList::~FileList(void)
{
}

//***************************************************************************
// Build it
//***************************************************************************
bool FileList::Build(
char *dir,
char *ext,
bool recurse)
{
	// make sure the path is null terminated
	char path[256];
	strcpy(path, dir);
	PathFile::TerminatePath(path);

	// Do it!
	return(Append(path, ext, recurse));
}

//***************************************************************************
// Add a single file to the list
//***************************************************************************
bool FileList::AddFile(char *pathFile)
{
	ASSERT(pathFile);
	
	// No leading spaces
	while (*pathFile == ' ')
		++pathFile;

	// duh
	if (pathFile[0] == '.' && pathFile[1] == '\\')
		pathFile += 2;

	// Search for it
	for (int i = 0; i < files.Count(); i++)
	{
		if (strcmpi(pathFile, files[0]->Data()) == 0)
			return(true);
	}

	// Make a string for it
	DynString *s = new DynString(pathFile);
	if (!s)
		return(false);

	// Add it
	return(files.Add(s));
}
	
//***************************************************************************
// Delete all files with the given ext
//***************************************************************************
bool FileList::DeleteFiles(char *ext)
{
	bool foundOne = false;

	for (int i = 0; i < files.Count();)
	{	
		DynString *test = files[i];
		if (!PathFile::ExtMatch(test->Data(), ext))
			 ++i;
		else
		{
			foundOne = true;
			files.Delete(i);
		}
	}

	return(foundOne);
}

//***************************************************************************
// Append this dir to the list
//***************************************************************************
bool FileList::Append(
char *path,
char *ext,
bool recurse)
{
#ifdef WIN32
	// make our search path
	DynString pathMask(path);
	pathMask += "*.*";

	// find first
	_finddata_t info;
	long handle = (long)_findfirst(pathMask.Data(), &info);
	if (handle == -1)
		return(false);

	// What do we do with this info?
	int ret;
	do {
		// a dir?
		if ((info.attrib & _A_SUBDIR) != 0)
		{
			if (strcmp(info.name, ".") != 0 && strcmp(info.name, "..") != 0 && recurse)
			{
				DynString temp(path);
				temp += info.name;
				temp += "\\";
				Append(temp.Data(), ext, recurse);
			}
		}

		// Does it match our search?
		else if (ext == NULL || PathFile::ExtMatch(info.name, ext))
		{
			DynString *s = new DynString(path);
			s->Cat(info.name);
			files.Add(s);
		}

		// next one
		ret = _findnext(handle, &info);
	} while (ret == 0);

	// close it
	_findclose(handle);
#endif
	return(true);
}
