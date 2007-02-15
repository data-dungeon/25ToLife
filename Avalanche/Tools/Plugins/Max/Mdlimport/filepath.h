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
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/21/01   Time: 3:55p
 * Updated in $/Avalanche/tools/Plugins/Max/Mdlimport
 * Cho Import matrix working
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/16/01   Time: 4:24p
 * Updated in $/Avalanche/tools/Plugins/Max/Mdlimport
 * loads models into world (no position yet)
 */
#ifndef __FILEPATH__H
#define __FILEPATH__H

#include <tchar.h>

class FilePath
{
public:
	static void GetPathStrings(const TCHAR *path,TCHAR *drive,TCHAR *dir,TCHAR *fileName,TCHAR *ext);
	static void GetPath(const TCHAR *pathandfile,TCHAR *path);
	static void GetDrive(const TCHAR *path,TCHAR *drive);
	static void GetDir(const TCHAR *path,TCHAR *dir);
	static void GetFileName(const TCHAR *path,TCHAR *fileName);
	static void GetExt(const TCHAR *path,TCHAR *ext);
	static void ChangeExtension(TSTR &path,const TCHAR *extension);
	static void GetPathsToFile(TSTR &relativePath,TSTR &fullPath,TSTR &name,TSTR &ext,const TCHAR *filename,const TCHAR *workFilename);
	static void GetPathsToFile(TSTR &relativePath,TSTR &fullPath,TSTR &name,TSTR &ext,const TCHAR *filename);
	static void RelativePath(TSTR &savePath,TSTR &name,TSTR &ext,const TCHAR *fullPath,const TCHAR *workPath);
private:
	static bool CompareLetters(TCHAR char1,TCHAR char2);
};

#endif