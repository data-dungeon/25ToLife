////////////////////////////////////////////////////////////////////////////
//
// Filepath
//
// class to handle file paths
//
////////////////////////////////////////////////////////////////////////////

#pragma once
#define FILEPATH__H

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

