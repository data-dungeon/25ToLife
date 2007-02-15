// ----------------------------------------------------------------------------
// File				-> CDllLoader.h
// Created			-> Dec 22, 2004
// Author			-> Dwain Skinner
// Description		-> SN DLL loader
// ----------------------------------------------------------------------------


#ifndef _CDLLLOADER_H
#define _CDLLLOADER_H

#define DLL_MAX_NAME_LEN	16
#define DLL_MAX_LOADED		8

class CDllLoader
{
public:
	CDllLoader();
	~CDllLoader();
	bool	Load( const char* filename, bool trayCheck );
	bool	Unload( const char* filename );
	bool	IsLoaded( const char* filename );

	struct LoadedDLLInfo
	{
		char	name[ DLL_MAX_NAME_LEN ];
		void*	addr;
		u32	size;
	};
	int	GetLoadedDLLs( LoadedDLLInfo** info, u32* totalSize );

private:
	void				AddLoaded( const char* filename, void* addr, u32 size );
	void				RemoveLoaded( int index );
	int				GetLoadedIndex( const char* filename );

	s32				m_numDLL;
	u32				m_memUsed;
	s32				m_lastError;
	LoadedDLLInfo	m_loadedDLLs[ DLL_MAX_LOADED ];
};

extern CDllLoader g_DllLoader;

#endif // _CDLLOADER_H
