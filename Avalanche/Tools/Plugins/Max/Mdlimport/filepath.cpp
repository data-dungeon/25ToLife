////////////////////////////////////////////////////////////////////////////
//
// Filepath
//
// class to handle file paths
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: filepath.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 1:19p
 * Updated in $/Avalanche/tools/Plugins/Max/Mdlimport
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

#include <stdlib.h>
#include <direct.h>

#include "max.h"
#include "filepath.h"

void FilePath::GetPathStrings(const TCHAR *path,TCHAR *drive,TCHAR *dir,TCHAR *fileName,TCHAR *ext)
{
	_splitpath(path,drive,dir,fileName,ext);
}

void FilePath::GetPath(const TCHAR *pathandfile,TCHAR *path)
{
	TCHAR fdrive[_MAX_DRIVE];
	TCHAR fdir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR fext[_MAX_EXT];

	_splitpath(pathandfile,fdrive,fdir,fname,fext);
	_makepath(path,fdrive,fdir,NULL,NULL);
}

void FilePath::ChangeExtension(TSTR &path,const TCHAR *extension)
{
	TCHAR fdrive[_MAX_DRIVE];
	TCHAR fdir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR fext[_MAX_EXT];
	TCHAR fpath[_MAX_PATH];

	_splitpath(path,fdrive,fdir,fname,fext);
	_makepath(fpath,fdrive,fdir,fname,extension);
	path = fpath;
}

void FilePath::GetDrive(const TCHAR *path,TCHAR *drive)
{
	TCHAR fdir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR fext[_MAX_EXT];
	_splitpath(path,drive,fdir,fname,fext);
}

void FilePath::GetDir(const TCHAR *path,TCHAR *dir)
{
	TCHAR fdrive[_MAX_DRIVE];
	TCHAR fname[_MAX_FNAME];
	TCHAR fext[_MAX_EXT];
	_splitpath(path,fdrive,dir,fname,fext);
}

void FilePath::GetFileName(const TCHAR *path,TCHAR *fileName)
{
	TCHAR fdrive[_MAX_DRIVE];
	TCHAR fdir[_MAX_DIR];
	TCHAR fext[_MAX_EXT];
	_splitpath(path,fdrive,fdir,fileName,fext);
}

void FilePath::GetExt(const TCHAR *path,TCHAR *ext)
{
	TCHAR fdrive[_MAX_DRIVE];
	TCHAR fdir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	_splitpath(path,fdrive,fdir,fname,ext);
}

void FilePath::GetPathsToFile(TSTR &relativePath,TSTR &fullPath,TSTR &name,TSTR &ext,const TCHAR *filename,const TCHAR *workFilename)
{
	TCHAR spath[_MAX_PATH];

	TCHAR wpath[_MAX_PATH];
	TCHAR fpath[_MAX_PATH];
	GetPath(workFilename,wpath);

	_getcwd(spath,_MAX_PATH); // get current path
	_chdir(wpath); // set current path to working path
	_fullpath(fpath,filename,_MAX_PATH);
	_chdir(spath); // restore current path
	fullPath = fpath;
	RelativePath(relativePath,name,ext,fpath,wpath);
}

void FilePath::GetPathsToFile(TSTR &relativePath,TSTR &fullPath,TSTR &name,TSTR &ext,const TCHAR *filename)
{
	TCHAR wpath[_MAX_PATH];
	TCHAR fpath[_MAX_PATH];
	_getcwd(wpath,_MAX_PATH); // get current path
	strcat(wpath,"\\"); // append backslash so that it doesn't confuse RelativePath
	_fullpath(fpath,filename,_MAX_PATH);
	fullPath = fpath;
	RelativePath(relativePath,name,ext,fpath,wpath);
}


void FilePath::RelativePath(TSTR &savePath,TSTR &name,TSTR &ext,const TCHAR *fullPath,const TCHAR *workPath)
{
	TCHAR fdrive[_MAX_DRIVE];
	TCHAR fdir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR fext[_MAX_EXT];
	_splitpath(fullPath,fdrive,fdir,fname,fext);
	name = fname;
	ext = fext;
	TCHAR wdrive[_MAX_DRIVE];
	TCHAR wdir[_MAX_DIR];
	TCHAR wname[_MAX_FNAME];
	TCHAR wext[_MAX_EXT];
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
			TCHAR rpath[_MAX_PATH];
			_makepath(rpath,NULL,&fdir[i],name,ext);
			savePath = rpath;
			return;
		}
		else
		{
			TCHAR rpath[_MAX_PATH];
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

bool FilePath::CompareLetters(TCHAR char1,TCHAR char2)
{
	if (char1>='a' && char1<='z')
		char1=(char1-'a')+'A';
	if (char2>='a' && char2<='z')
		char2=(char2-'a')+'A';
	return char1==char2;

}

