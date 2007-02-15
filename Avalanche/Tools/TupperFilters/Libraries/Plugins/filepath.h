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
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 11:48a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * update for streaming system
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 11/22/02   Time: 11:39a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * added search method
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * File Dependency checks
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 5/17/02    Time: 11:38a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * renamed tupperware string and array
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 1:01p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Added bounding and annotate helper classes
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 5:12p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 */

#ifndef TUPPERFILTERS_LIB_PLUGINS_FILEPATH__H
#define TUPPERFILTERS_LIB_PLUGINS_FILEPATH__H

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
	static bool SearchPathsForFile(const char *pFilename,const char *pSearchPaths,const char *pWorkPath,TupString &destPath);
	static void AddNumberToFileName(TupString &path,int value,int maxFileChars=5,int numDigits=3,bool bUseHex=true);
	static void AddStringToFileName(TupString &path,const char *string);
	static void DeleteFiles(const char *pFileName);
	static void LimitCharactersInFilename(TupString &path,int maxCharacters);

private:
	static bool CompareLetters(char char1,char char2);
};

#endif