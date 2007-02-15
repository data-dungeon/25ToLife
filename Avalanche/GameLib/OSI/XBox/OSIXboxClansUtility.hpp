#pragma once

//-----------------------------------------------------------------------------
//! \brief This class provides a clean way for OSI code to handle all the 
//! \brief translation and data management involved with the storage and 
//! \brief retrieval of clan/team information on Xbox Live.
//! \note !!Keep in mind the Xbox terminology!!  
//! \note	"TEAM" == clan
class ClansAdapter
{
private:

	//! \brief Where all the clan data ends up (in player->clanInfo).
	OSIPlayerList* m_playerList;
	OSIClanInfoList* m_clanList;

	//! \brief A structure that holds all data for downloading clan data 
	//! \brief for a single player.
	struct DownloadClanData
	{
		XONLINETASK_HANDLE	taskHandle;
		OSIu64				id;
		OSIu32				flags;
		HRESULT				hr;

		#ifdef _DEBUG
		BYTE instanceNumberAtBegin;
		#endif
	};

	DWORD				m_downloadClanDataTotal;
	DWORD				m_downloadClanDataFinished;
	DownloadClanData*	m_downloadClanData;

	void
	ParseDownloadClanDataResult(
		DownloadClanData* const dataPtr,
		OSIPlayer* const player );

	void
	ParseDownloadClanDataResult(
		DownloadClanData* const dataPtr,
		OSIClanInfo* const clan	);

	//-------------------------------------------------------------------------
	//! \name Debugging
	//! \{
	#ifdef _DEBUG
	//! \brief s_clansAdapterInstanceNumber keeps track of how many times this 
	//! \brief class has been instantiated.  It's used to assert that data 
	//! \brief that's being used is the same data every time.  I.E., it makes 
	//! \brief sure that the clans adapter hasn't been deleted and recreated 
	//! \brief between begin/finish/continue function calls.
	static BYTE s_clansAdapterInstanceNumber;
	#endif //_DEBUG
	//! \}
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	//! \name Singleton Functionality
	//! \{
private:
	static ClansAdapter* m_singletonPtr;
	inline ClansAdapter();
	inline ~ClansAdapter();
public:
	inline static void OSICreateSingleton();
	inline static void OSIDestroySingleton();
	
	// There's a global macro called "GetSingleton"
	inline static ClansAdapter* OSIGetSingleton(); 
	//! \}
	//-------------------------------------------------------------------------

public:

	void		BeginDownloadClanInfo( OSIPlayerList* players );
	void		BeginDownloadClanInfo( OSIClanInfoList* clans );
	void		ContinueDownloadClanInfo();
	void		FinishDownloadClanInfo();
	inline bool IsDownloadClanInfoComplete() const;

};

#include "OSIXboxClansUtility.inline.hpp"