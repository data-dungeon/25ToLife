//--------------------------------------------------------------//
// MemoryCard_Win32.h
//
// Memory Card Handler for TTL (platform specific)
//
//--------------------------------------------------------------//
#ifndef _MEMORYCARD_WIN32_H
#define _MEMORYCARD_WIN32_H

#define	POPUP_DISPLAY_TIME	(2.0f)

namespace	CMemoryCard_Win32
{
	//methods
	static	void	Load();
	static	void	Save();
	static	void	Delete();
	static	void	Exists();
	static	void	HandleResult();
	static	void	GetFilesOnCard();
	static	void	GetFilesOnAllCards();	
	static	MEMCARD_RESULT	WaitOnResult();
	static	void	PollCards();
	static	void	Detect();
	static	void	StartupCheck();
};

#endif	// _MEMORYCARD_WIN32_H