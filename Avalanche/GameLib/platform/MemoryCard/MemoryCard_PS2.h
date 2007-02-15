//--------------------------------------------------------------//
// MemoryCard_PS2.h
//
// Memory Card Handler for TTL (platform specific)
//
//--------------------------------------------------------------//
#ifndef _MEMORYCARD_PS2_H
#define _MEMORYCARD_PS2_H

#define	POPUP_DISPLAY_TIME	(3.0f)
#define	MC_MAX_FILES_DIR		16
#define	MC_RESERVED_FILES		3		// icon.sys, logo.sys, <dirname>

namespace	CMemoryCard_PS2
{
	//methods
	static	void	Detect();
	static	void	DetectWithResult();
	static	void	DetectOnStart();
	static	void	Load();
	static	MEMCARD_RESULT	WillSaveFit();
	static	void	Save();
	static	void	Delete();
	static	void	Format();
	static	void	HandleResult();
	static	void	Exists();
	static	void	GetFilesOnCard();
	static	void	GetFilesOnAllCards();
	static	int	BlocksNeededForSave();
	static	MEMCARD_RESULT	WaitOnResult(bool Detect = false);
	static	void	PollCards();
	static	void	StartupCheck();
	static	void	LoadNetwork();
	static	void	StartupControllerCheck();
};

#endif	// _MEMORYCARD_PS2_H