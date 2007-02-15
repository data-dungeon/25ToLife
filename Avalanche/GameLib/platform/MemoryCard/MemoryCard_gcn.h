//--------------------------------------------------------------//
// MemoryCard_GCN.h
//
// Memory Card Handler for TTL (platform specific)
//
//--------------------------------------------------------------//
#ifndef _MEMORYCARD_GCN_H
#define _MEMORYCARD_GCN_H

#define	POPUP_DISPLAY_TIME	(3.0f)
#define	MC_MAX_FILES_DIR		18

namespace	CMemoryCard_GCN
{
	//methods
	static	void	Detect();
	static	void	Load();
	static	void	Save();
	static	void	Delete();
	static	void	Format();
	static	void	HandleResult();
	static	void	Exists();
	static	void	GetFilesOnCard();
	static	void	GetFilesOnAllCards();
	static	int	BlocksNeededForSave();
	static	void	WaitOnResult();
	static	void	PollCards();
	static	void	StartupCheck();
};

#endif	// _MEMORYCARD_GCN_H