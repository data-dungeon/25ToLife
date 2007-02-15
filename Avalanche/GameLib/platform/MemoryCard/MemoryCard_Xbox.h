//--------------------------------------------------------------//
// MemoryCard_Xbox.h
//
// Memory Card Handler for TTL (platform specific)
//
//--------------------------------------------------------------//
#ifndef _MEMORYCARD_XBOX_H
#define _MEMORYCARD_XBOX_H

#define	POPUP_DISPLAY_TIME	(3.0f)
#define	MC_MAX_FILES_DIR		255

namespace	CMemoryCard_Xbox
{
	//methods
	static	void	Detect();
	static	void	Load();
	static	void	Save();
	static	void	Delete();
	static	void	HandleResult();
	static	void	Exists();
	static	void	GetFilesOnCard();
	static	void	GetFilesOnAllCards();			// HD and memory units
	static	void	Dashboard();
	static	int	BlocksNeededForSave();
	static	int	EnumerateSaveGames(char* Drive, int StartIndex);
	static	MEMCARD_RESULT	WaitOnResult();
	static	void	PollCards();
	static	void	StartupCheck();
};

#endif	// _MEMORYCARD_XBOX_H