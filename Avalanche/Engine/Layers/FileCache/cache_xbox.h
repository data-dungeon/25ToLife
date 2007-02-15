//--------------------------------------------------------------//
// cache_xbox.h
//
// Caching Class.. allows files to be caches as loaded to
// alternate storage
//
//--------------------------------------------------------------//
#ifndef _CACHE_XBOX_H
#define _CACHE_XBOX_H

class	CFileCache
{
public:

	// init functions
	static	bool	InitCacheSystem(char *Drive, int MaxFilesize, bool format);	
	static	bool	SetMountDrive(char *Drive);
	static	void	MountDrive(bool format);
	static	bool	UnmountDrive();
	static	bool	CreateDirectories(char** DirList);

	// access functions
	static	bool	CacheFile(char* Dst, char* Src, bool overwrite);
	static	bool	CacheFileProgress(char* Dst, char* Src, bool overwrite);
	static	bool	CacheFileAsync(char* Dst, char* Src, bool overwrite);

	// timestamp
	static	bool	CreateTimestamp(void);
	static	bool	ReadTimestamp(void);

	// utility functions
	static	void	RegisterUpdate(void* Function);

};

#endif	// _CACHE_XBOX_H