////////////////////////////////////////////////////////////////////////////
//
// Filepath
//
// class to handle file paths
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: filepath.h $
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 6/20/03    Time: 3:37p
 * Created in $/Avalanche/tools/Plugins/Maya/mapAttrib/mapAttribWin32
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/09/02    Time: 10:23a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * moved to TupStrings
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 2:04p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Relative Path Support for Images
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 5:12p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 */

#ifndef __FILEPATH__H
#define __FILEPATH__H

#ifndef TUPPERWARE_STRING_H
#include "tupperware/string.h"
#endif

class FilePath
{
public:
	static void GetPathStrings(const char *path,char *drive,char *dir,char *fileName,char *ext);
	static void GetPath(const char *pathandfile,char *path);
	static void GetDrive(const char *path,char *drive);
	static void GetDir(const char *path,char *dir);
	static void GetFileName(const char *path,char *fileName);
	static void GetExt(const char *path,char *ext);
	static void ChangeExtension(TupString &path,const char *extension);
	static void GetPathsToFile(TupString &relativePath,TupString &fullPath,TupString &name,TupString &ext,const char *filename,const char *workFilename);
	static void GetPathsToFile(TupString &relativePath,TupString &fullPath,TupString &name,TupString &ext,const char *filename);
	static void RelativePath(TupString &savePath,TupString &name,TupString &ext,const char *fullPath,const char *workPath);
private:
	static bool CompareLetters(char char1,char char2);
};

#endif