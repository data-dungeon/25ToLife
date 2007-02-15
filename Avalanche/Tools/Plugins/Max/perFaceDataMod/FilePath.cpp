////////////////////////////////////////////////////////////////////////////
//
// Filepath
//
// class to handle file paths
//
////////////////////////////////////////////////////////////////////////////

#include "pfdModPCH.h"
#include "filepath.h"

void FilePath::GetPathStrings(const char *path,char *drive,char *dir,char *fileName,char *ext)
{
	_splitpath(path,drive,dir,fileName,ext);
}

void FilePath::GetPath(const char *pathandfile,char *path)
{
	char fdrive[_MAX_DRIVE];
	char fdir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char fext[_MAX_EXT];

	_splitpath(pathandfile,fdrive,fdir,fname,fext);
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
	char fdir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char fext[_MAX_EXT];
	_splitpath(path,drive,fdir,fname,fext);
}

void FilePath::GetDir(const char *path,char *dir)
{
	char fdrive[_MAX_DRIVE];
	char fname[_MAX_FNAME];
	char fext[_MAX_EXT];
	_splitpath(path,fdrive,dir,fname,fext);
}

void FilePath::GetFileName(const char *path,char *fileName)
{
	char fdrive[_MAX_DRIVE];
	char fdir[_MAX_DIR];
	char fext[_MAX_EXT];
	_splitpath(path,fdrive,fdir,fileName,fext);
}

void FilePath::GetExt(const char *path,char *ext)
{
	char fdrive[_MAX_DRIVE];
	char fdir[_MAX_DIR];
	char fname[_MAX_FNAME];
	_splitpath(path,fdrive,fdir,fname,ext);
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

