////////////////////////////////////////////////////////////////////////////
//
// Filepath
//
// class to handle file paths
//
////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "filepath.h"
#include "TokenizeString.h"

void FilePath::GetPathStrings(const char *path,char *drive,char *dir,char *fileName,char *ext)
{
	_splitpath(path,drive,dir,fileName,ext);
}

void FilePath::GetPath(const char *pathandfile,char *path)
{
	char fdrive[_MAX_DRIVE];
	char fdir[_MAX_DIR];

	_splitpath(pathandfile,fdrive,fdir,NULL,NULL);
	_makepath(path,fdrive,fdir,NULL,NULL);
}

void FilePath::ChangeExtension(TupString &path,const char *extension)
{
	char fdrive[_MAX_DRIVE];
	char fdir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char fext[_MAX_EXT];
	char fpath[_MAX_PATH];

	_splitpath(path,fdrive,fdir,fname,fext);
	_makepath(fpath,fdrive,fdir,fname,extension);
	path = fpath;
}

void FilePath::GetDrive(const char *path,char *drive)
{
	_splitpath(path,drive,NULL,NULL,NULL);
}

void FilePath::GetDir(const char *path,char *dir)
{
	_splitpath(path,NULL,dir,NULL,NULL);
}

void FilePath::GetFileName(const char *path,char *fileName)
{
	_splitpath(path,NULL,NULL,fileName,NULL);
}

void FilePath::GetExt(const char *path,char *ext)
{
	_splitpath(path,NULL,NULL,NULL,ext);
}

void FilePath::GetPathsToFile(TupString &relativePath,TupString &fullPath,TupString &name,TupString &ext,const char *filename,const char *workFilename)
{
	char spath[_MAX_PATH];

	char wpath[_MAX_PATH];
	char fpath[_MAX_PATH];
	GetPath(workFilename,wpath);

	_getcwd(spath,_MAX_PATH); // get current path
	_chdir(wpath); // set current path to working path
	_fullpath(fpath,filename,_MAX_PATH);
	_chdir(spath); // restore current path
	fullPath = fpath;
	RelativePath(relativePath,name,ext,fpath,wpath);
}

void FilePath::GetPathsToFile(TupString &relativePath,TupString &fullPath,TupString &name,TupString &ext,const char *filename)
{
	char wpath[_MAX_PATH];
	char fpath[_MAX_PATH];
	_getcwd(wpath,_MAX_PATH); // get current path
	strcat(wpath,"\\"); // append backslash so that it doesn't confuse RelativePath
	_fullpath(fpath,filename,_MAX_PATH);
	fullPath = fpath;
	RelativePath(relativePath,name,ext,fpath,wpath);
}


void FilePath::RelativePath(TupString &savePath,TupString &name,TupString &ext,const char *fullPath,const char *workPath)
{
	char fdrive[_MAX_DRIVE];
	char fdir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char fext[_MAX_EXT];
	_splitpath(fullPath,fdrive,fdir,fname,fext);
	name = fname;
	ext = fext;
	char wdrive[_MAX_DRIVE];
	char wdir[_MAX_DIR];
	char wname[_MAX_FNAME];
	char wext[_MAX_EXT];
	_splitpath(workPath,wdrive,wdir,wname,wext);
	// if drives are different
	if (stricmp(fdrive,wdrive))
	{
		savePath = fullPath;
		return;
	}
	else
	{
		int i=0;
		// scan through directories until a different one is found
		while (wdir[i] && fdir[i] && CompareLetters(wdir[i],fdir[i]))
			i++;
		if (!wdir[i]) // if work directory done
		{
			char rpath[_MAX_PATH];
			_makepath(rpath,NULL,&fdir[i],name,ext);
			savePath = rpath;
			return;
		}
		else
		{
			char rpath[_MAX_PATH];
			rpath[0] = '\0';
			int j=i;
			while (wdir[j])
			{
				// count the # of backslashes left and add a directory back string
				if (wdir[j]=='\\' || wdir[j]=='/')
				{
					strcat(rpath,"..\\");
				}
				j++;
			}
			j=i;
			while (fdir[j-1]!='\\'&&fdir[j-1]!='/') j--;
			savePath = rpath;
			savePath += &fdir[j];
			savePath += fname;
			savePath += fext;
		}
	}
}

bool FilePath::CompareLetters(char char1,char char2)
{
	if (char1>='a' && char1<='z')
		char1=(char1-'a')+'A';
	if (char2>='a' && char2<='z')
		char2=(char2-'a')+'A';
	return char1==char2;

}

bool FilePath::SearchPathsForFile(const char *pFilename,const char *pSearchPaths,const char *pWorkPath,TupString &destPath)
{
	char fname[_MAX_FNAME];
	char fext[_MAX_EXT];
	_splitpath(pFilename,NULL,NULL,fname,fext); // get the filename and extension

	TupArray<TupString> searchPathArray;
	TokenizeString::BuildArray(pSearchPaths,";",searchPathArray);
	int numPaths = searchPathArray.GetSize();
	for (int pathIndex=0;pathIndex<numPaths;pathIndex++)
	{
		TupString relativePath,fullPath,name,ext;
		TupString tryFilePath = searchPathArray[pathIndex];
		if (tryFilePath[tryFilePath.GetLength()-1]!='\\')
			tryFilePath+= "\\";
		char tdrive[_MAX_DRIVE];
		char tdir[_MAX_DIR];
		_splitpath(tryFilePath,tdrive,tdir,NULL,NULL); // get the try drive and directory

		if (tryFilePath.GetLength())
		{
			char fpath[_MAX_PATH];
			// make a path using tryFilePath
			_makepath(fpath,tdrive,tdir,fname,fext);
			// get a full path using the work path
			GetPathsToFile(relativePath,fullPath,name,ext,fpath,pWorkPath);
			// see if we can access it
			if (_access(fullPath,0)==0)
			{
				destPath = fullPath;
				return true;
			}
		}
	}
	return false;
}

void FilePath::AddNumberToFileName(TupString &path,int value,int maxFileChars,int numDigits,bool bUseHex)
{
	// Don't do anything if they are on zero
	if (value == 0)
		return;

	char fdrive[_MAX_DRIVE];
	char fdir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char fext[_MAX_EXT];
	char fpath[_MAX_PATH];

	char newFname[_MAX_FNAME];

	_splitpath(path,fdrive,fdir,fname,fext);

	// limit the length of the filename
	if (maxFileChars>=0 && maxFileChars<_MAX_FNAME)
	{
		fname[maxFileChars] = '\0';	
	}

	char formatString[16];
	if (bUseHex)
		sprintf(formatString,"%%s%%0%dx",numDigits);
	else
		sprintf(formatString,"%%s%%0%dd",numDigits);
	sprintf(newFname,formatString,fname,value);
	_makepath(fpath,fdrive,fdir,newFname,fext);
	path = fpath;
}

void FilePath::AddStringToFileName(TupString &path,const char *string)
{
	char fdrive[_MAX_DRIVE];
	char fdir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char fext[_MAX_EXT];
	char fpath[_MAX_PATH];

	_splitpath(path,fdrive,fdir,fname,fext);
	strcat(fname,string);
	_makepath(fpath,fdrive,fdir,fname,fext);
	path = fpath;
}

void FilePath::DeleteFiles(const char *pFileName)
{
	char fdrive[_MAX_DRIVE];
	char fdir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char fext[_MAX_EXT];
	char fpath[_MAX_PATH];
	_splitpath(pFileName,fdrive,fdir,fname,fext);

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = ::FindFirstFile(pFileName, &findFileData);

	if ( hFind != INVALID_HANDLE_VALUE )
	{
		_splitpath(findFileData.cFileName,NULL,NULL,fname,fext);
		_makepath(fpath,fdrive,fdir,fname,fext);
		::DeleteFile(fpath);
		while (FindNextFile( hFind, &findFileData))
		{
			_splitpath(findFileData.cFileName,NULL,NULL,fname,fext);
			_makepath(fpath,fdrive,fdir,fname,fext);
			::DeleteFile(fpath);
		}
	}
}

void FilePath::LimitCharactersInFilename(TupString &path,int maxCharacters)
{
	char fdrive[_MAX_DRIVE];
	char fdir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char fext[_MAX_EXT];
	char fpath[_MAX_PATH];
	_splitpath(path,fdrive,fdir,fname,fext);
	if (maxCharacters>=0 && maxCharacters<_MAX_FNAME)
	{
		fname[maxCharacters] = '\0';	
	}
	_makepath(fpath,fdrive,fdir,fname,fext);
	path = fpath;
}

