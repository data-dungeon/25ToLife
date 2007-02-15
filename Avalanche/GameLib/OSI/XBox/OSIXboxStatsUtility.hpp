#pragma once

//-----------------------------------------------------------------------------
//! \brief This class provides a clean way for OSI code to handle all the 
//! \brief translation and data management involved with the storage and 
//! \brief retrieval of OSI stats on Xbox Live.
//! \note !!Keep in mind the Xbox terminology!!  
//! \note	"SPEC" = download operation
//! \note	"PROC" = upload operation
//! \note My naming is a little inconsistent - I use "spec"/"download" and 
//! \note "proc"/"upload" interchangeably.
class StatsAdapter
{
private:

	enum SpecIndex 
	{
		SPECINDEX_PLAYER_0,
		SPECINDEX_PLAYER_64,
		NUM_SPECINDEX
	};

	enum ProcIndex 
	{
		PROCINDEX_PLAYER_INC_0,
		PROCINDEX_PLAYER_INC_64,
		
		NUM_PROCINDEX_PLAYER,
		
		PROCINDEX_CLAN_INC_0 = NUM_PROCINDEX_PLAYER,
		PROCINDEX_CONDITIONAL_RATING_LESS_NEG_1600,
		PROCINDEX_CLAN_RESET_RATING,

		NUM_PROCINDEX_CLAN
	};

	//-------------------------------------------------------------------------
	//! \name Constants and LUTs
	//! \{

	//! \brief The xbox stat ID to use when something weird happens; see use 
	//! \brief for clarification.
	static const WORD s_DummyStatID = 1;

	//! \brief The number of stats downloaded for each player.
	static const int s_NumDownloadStatsPerPlayer = 49;

	//! \brief The number of XONLINE_STAT_SPEC structures allocated per player 
	//! \brief during download.
	static const int s_NumDownloadSpecsPerPlayer = NUM_SPECINDEX;					

	//! \brief The number of XONLINE_STAT_PROC structures allocated per player 
	//! \brief during upload.
	static const int s_NumUploadProcs = NUM_PROCINDEX_CLAN;					

	//! \brief The number of stats to upload and increment for each player.
	static const int s_NumUploadIncStatsPerPlayer = 101;								

	//! \brief The number of stats to upload and increment for each clan.
	static const int s_NumUploadIncStatsPerClan = 2;

	//! \brief The max number stats that can be stored in a single leaderboard 
	//! \brief for any individual player or clan.
	static const int s_MaxStatsPerLeaderboard = XONLINE_STAT_MAX_STATS_IN_SPEC;

	//! \brief The number of leaderboards it takes to store all the OSI stats 
	//! \brief for any individual player.
	static const int s_NumPlayerLeaderboards = 2;								

	//! \brief The number of stats downloaded for each clan.
	static const int s_NumDownloadStatsPerClan = 3;

	//! \brief The number of XONLINE_STAT_SPEC structures allocated per clan 
	//! \brief during download.
	static const int s_NumDownloadSpecsPerClan = 1;								

	//! \brief The max number of players that can be combined into a single 
	//! \brief stats download operation.
	static const int s_MaxPlayersPerDownload = 10;								

	//! \brief The max number of clans that can be combined into a single 
	//! \brief stats download operation.
	static const int s_MaxClansPerDownload = 10;								

	//! \brief How many XONLINE_STAT_SPEC structures to allocate for players.
	static const int s_MaxPlayerDownloadSpecsPerRequest
		= StatsAdapter::s_MaxPlayersPerDownload 
		* StatsAdapter::s_NumDownloadSpecsPerPlayer;

	//! \brief How many XONLINE_STAT structures to allocate for players.
	static const int s_MaxPlayerDownloadStatsPerRequest 
		= StatsAdapter::s_MaxPlayersPerDownload 
		* StatsAdapter::s_NumDownloadStatsPerPlayer;

	//! \brief How many XONLINE_STAT_SPEC structures to allocate for clans.
	static const int s_MaxClanDownloadSpecsPerRequest 
		= StatsAdapter::s_MaxClansPerDownload 
		* StatsAdapter::s_NumDownloadSpecsPerClan;

	//! \brief How many XONLINE_STAT structures to allocate for clans.
	static const int s_MaxClanDownloadStatsPerRequest 
		= StatsAdapter::s_MaxClansPerDownload 
		* StatsAdapter::s_NumDownloadStatsPerClan;

	//! \brief The list of OSI stats to download per player.
	static const WORD s_PlayerStatsToDownload[ s_NumDownloadStatsPerPlayer ];

	//! \brief The list of OSI stats to download per clan.
	static const WORD s_ClanStatsToDownload[ s_NumDownloadStatsPerClan ];	
	
	//! \brief The list of OSI stats to upload and increment per player.
	static const WORD s_PlayerIncStatsToUpload[ s_NumUploadIncStatsPerPlayer ];
	
	//! \brief The list of OSI stats to upload and increment per clan.
	static const WORD s_ClanIncStatsToUpload[ s_NumUploadIncStatsPerClan ];

	//! \}
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	//! \name Download helpers.
	//! \{

	inline 
	bool 
	IsDownloadPlayersComplete()
	const;

	inline
	bool
	IsDownloadClansComplete()
	const;
	
	HRESULT
	ContinueDownloadPlayers(
		XONLINETASK_HANDLE& handle );

	HRESULT
	ContinueDownloadClans(
		XONLINETASK_HANDLE& handle );

	HRESULT
	ProcessDownloadPlayersResult(
		const XONLINETASK_HANDLE& handle );

	HRESULT
	ProcessDownloadClansResult(
		const XONLINETASK_HANDLE& handle );

	//! \}
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	//! \name Data structures that store data in the format that Live wants it.
	//! \{
	struct UploadData
	{
		XONLINE_STAT_PROC procs[ StatsAdapter::s_NumUploadProcs ];
		XONLINE_STAT resetClanRatingStat;
		XONLINE_STAT statPlayerInc
			[ StatsAdapter::s_NumUploadIncStatsPerPlayer 
			+ StatsAdapter::s_NumUploadIncStatsPerClan ];

		#ifdef _DEBUG
		BYTE instanceNumberAtBegin;
		#endif //_DEBUG
	};

	template < 
		int TMaxDownloadSpecsPerRequest, 
		int TMaxDownloadStatsPerRequest 
	> struct DownloadData 
	{
		//! \brief Set by BeginDownload.
		DWORD numTotal;

		//! \brief Set by ContinueDownload.
		DWORD numStarted;

		//! \brief Set by ProcessDownloadResults
		DWORD numFinished;

		//! \brief Set by ContinueDownload, used again by ProcessDownloadResults.
		XONLINE_STAT_SPEC specs[ TMaxDownloadSpecsPerRequest ];

		//! \brief Set by ContinueDownload, used again by ProcessDownloadResults.
		XONLINE_STAT stats[ TMaxDownloadStatsPerRequest ];

		#ifdef _DEBUG
		BYTE instanceNumberAtBegin;
		#endif //_DEBUG
	};

	typedef DownloadData< 
		s_MaxPlayerDownloadSpecsPerRequest, 
		s_MaxPlayerDownloadStatsPerRequest 
	> DownloadPlayerData;

	typedef DownloadData< 
		s_MaxClanDownloadSpecsPerRequest, 
		s_MaxClanDownloadStatsPerRequest 
	> DownloadClanData;

	struct DownloadLadderData
	{
		XUID				pivotXUID;
		DWORD				firstRank;
		DWORD				numRanks;
		XONLINE_STAT_USER*	users;
		inline DownloadLadderData();
		inline ~DownloadLadderData();

		#ifdef _DEBUG
		BYTE instanceNumberAtBegin;
		#endif //_DEBUG
	};

	//! \}
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	//! \brief All data needed to upload stats.
	UploadData*				m_upload;				

	//! \brief All data needed to download player stats.
	DownloadPlayerData*		m_playerDownload;		

	//! \brief All data needed to download clan stats.
	DownloadClanData*		m_clanDownload;			

	//! \brief All data needed to download ladder info.
	DownloadLadderData*		m_ladderDownload;		

	//! \brief The list of player for which stats are being processed.
	OSIPlayerStatsList*		m_playerList;			

	//! \brief The list of clans for which stats are being processed.
	OSIClanInfoList*		m_clanInfoList;				

	//-------------------------------------------------------------------------
	
	//-------------------------------------------------------------------------
	//! \name Debugging
	//! \{
	#ifdef _DEBUG
	//! \brief s_statsAdapterInstanceNumber keeps track of how many times this 
	//! \brief class has been instantiated.  It's used to assert that data 
	//! \brief that's being used is the same data every time.  I.E., it makes 
	//! \brief sure that the stats adapter hasn't been deleted and recreated 
	//! \brief between begin/finish/continue function calls.
	static BYTE s_statsAdapterInstanceNumber;
	#endif //_DEBUG
	//! \}
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	//! \name Singleton Functionality
	//! \{
private:
	static StatsAdapter* m_singletonPtr;
	inline StatsAdapter();
	inline ~StatsAdapter();
public:
	inline static void OSICreateSingleton();
	inline static void OSIDestroySingleton();
	
	// There's a global macro called "GetSingleton"
	inline static StatsAdapter* OSIGetSingleton(); 
	//! \}
	//-------------------------------------------------------------------------
	
public:
	//! \brief The ID of the leaderboard where the first 64 stats are stored 
	//! \brief for players.
	static const int s_LeaderboardIdForPlayerStats0to63 = 1;								

	//! \brief The ID of the leaderboard where the next 64 stats are stored 
	//! \brief for players.
	static const int s_LeaderboardIdForPlayerStats64to127 = 2;								

	//! \brief The ID of the leaderboard where the first 64 stats are stored 
	//! \brief for clans.
	static const int s_LeaderboardIdForClanStats0to63 = 3;								


	//-------------------------------------------------------------------------
	//! \name Functions for converting PLAYER stats OSI->Xbox and vice-versa.
	//! \{

	inline static OSIPlayerStats::StatValueType
	GetDefaultPlayerStatValue( 
		const WORD& id );

	inline static WORD 
	ConvertPlayerOsiStatIDToXbox( 
		const WORD osiStatID );

	inline static WORD 
	ConvertPlayerXboxStatIDToOsi( 
		const WORD xboxStatID,
		const DWORD leaderboardID );

	inline static void 
	ConvertPlayerOsiStatToXbox( 
		XONLINE_STAT& stat, 
		const WORD osiStatID, 
		const OSIPlayerStats::StatValueType value );

	inline static void 
	ConvertPlayerXboxStatToOSI( 
		XONLINE_STAT& stat,
		const DWORD leaderboardID,
		WORD& osiStatID,
		OSIPlayerStats::StatValueType& value );

	//! \}
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	//! \name Functions for converting CLAN stats OSI->Xbox and vice-versa.
	//! \{

	inline static OSIClanStats::StatValueType
	GetDefaultClanStatValue( 
		const WORD& id );

	inline static WORD 
	ConvertClanOsiStatIDToXbox( 
		const WORD osiStatID );

	inline static WORD 
	ConvertClanXboxStatIDToOsi( 
		const WORD xboxStatID );

	inline static void 
	ConvertClanOsiStatToXbox( 
		XONLINE_STAT& stat, 
		const WORD osiStatID, 
		const OSIClanStats::StatValueType value );

	inline static void 
	ConvertClanXboxStatToOSI( 
		XONLINE_STAT& stat,
		WORD& osiStatID,
		OSIClanStats::StatValueType& value );

	//! \}
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	//! \name Functions for cleanly filling in common or large structures.
	//! \{

	inline static void
	FillSpec(
		XONLINE_STAT_SPEC& spec,
		const XUID xuid,
		const DWORD leaderboardID,
		const DWORD statCount,
		XONLINE_STAT* const statPtr );

	inline static void 
	FillUpdate( 
		XONLINE_STAT_UPDATE& update, 
		const XUID xuid, 
		const DWORD leaderboardID, 
		const DWORD statCount, 
		XONLINE_STAT* const statPtr );

	inline static void 
	FillStatLong( 
		XONLINE_STAT& stat, 
		const WORD id, 
		const LONG value );

	inline static void 
	FillStatLongLong( 
		XONLINE_STAT& stat, 
		const WORD id, 
		const LONGLONG value );

	inline static DWORD
	FillPlayerStats( 
		XONLINE_STAT* statArray,
		const DWORD leaderboardId,
		const WORD* const statOsiIdArray,
		const DWORD statOsiIdArrayCount,
		const OSIPlayerStats* playerStats );

	inline static DWORD
	CountPlayerStats(
		const DWORD leaderboardId,
		const WORD* statOsiIdArray,
		const DWORD statOsiIdArrayCount );

	inline static DWORD
	FillClanStats( 
		XONLINE_STAT* statArray,
		const DWORD leaderboardId,
		const WORD* const statOsiIdArray,
		const DWORD statOsiIdArrayCount,
		const OSIClanStats* clanStats );

	//! \}
	//-------------------------------------------------------------------------



	//! \brief Use BeginUpload to begin an upload for a single player and clan.  You should pump
	//! \brief the resulting handle until it returns success, then call FinishUpload.  If the
	//! \brief online task fails, don't call FinishUpload.
	//! \return The HRESULT returned by XOnlineStatWriteEx.
	HRESULT 
	BeginUpload( 
		XONLINETASK_HANDLE& handle, 
		OSIPlayerStats& playerStats, 
		OSIClanStats& clanStats );
	
	//! \brief You should pump the handle resulting from BeginUpload until it returns success, then
	//! \brief call FinishUpload to complete the process.  If the online task fails, don't call this.
	HRESULT
	FinishUpload( 
		const XONLINETASK_HANDLE& handle );

	//! \brief Use BeginDownload to begin a download for a list of players and clans.  
	//! \param handle OUT-The handle that XOnlineStatRead creates; pump it with XOnline functions.
	//! \param playerList The player list where results should end up.
	//! \return The HRESULT returned by XOnlineStatRead.
	HRESULT 
	BeginDownload( 
		XONLINETASK_HANDLE& handle,
		OSIPlayerStatsList* playerList, 
		OSIClanInfoList* clanList );

	//! \brief You should pump the handle resulting from BeginDownload until it returns success, then
	//! \brief call ProcessDownloadResult to process the results of the download.  If the online task
	//! \brief fails, don't call this.
	//! \param handle The handle that BeginDownload or ContinueDownload produced.
	//! \return S_OK or The HRESULT returned by the first failing call to XOnlineStatReadGetResults.
	HRESULT
	ProcessDownloadResult( 
		const XONLINETASK_HANDLE& handle );

	//! \brief After calling ProcessDownloadResult, call IsDownloadComplete to see if 
	//! \brief there are more stats to download.
	//! \return true If ContinueDownloadPlayer should be called again.
	inline 
	bool 
	IsDownloadComplete() 
	const;

	//! \brief If BeginDownload and ProcessDownloadStatResults have both been called and
	//! \brief IsDownloadComplete returns true, then call ContinueDownload to kick off the 
	//! \brief next download operation.  Then pump the handle just like with BeginDownload 
	//! \brief and rinse/repeat until IsDownloadComplete returns false.
	//! \param handle OUT-The handle that XOnlineStatRead creates.
	//! \return The HRESULT returned by XOnlineStatRead.
	HRESULT 
	ContinueDownload( 
		XONLINETASK_HANDLE& handle );

	//! \brief Use BeginDownloadPlayerLadder to get a list of players from the ladder that
	//! \brief match the given criteria.  You should pump the resulting handle until it 
	//! \brief returns success, then call FinishDownloadPlayerLadder.  If the online task
	//! \brief fails, don't call FinishDownloadPlayerLadder.
	//! \param handle OUT- The handle returned by XOnlineStatLeaderEnumerate.
	//! \param pivotPlayer The OSIPlayerID of the player who should be close to the middle of the results.
	//! \param numRanks How many players surrounding pivotPlayer to retrieve, INCLUDING pivotPlayer.
	//! \param playerList Where the results should go.
	//! \return The HRESULT returned by XOnlineStatLeaderEnumerate.
	HRESULT 
	BeginDownloadPlayerLadder( 
		XONLINETASK_HANDLE& handle, 
		OSIPlayerID& pivotPlayer, 
		const DWORD numRanks, 
		OSIPlayerStatsList* playerList );

	//! \brief Use BeginDownloadPlayerLadder to get a list of players from the ladder that
	//! \brief match the given criteria.  You should pump the resulting handle until it 
	//! \brief returns success, then call FinishDownloadPlayerLadder.  If the online task
	//! \brief fails, don't call FinishDownloadPlayerLadder.
	//! \param handle OUT- The handle returned by XOnlineStatLeaderEnumerate.
	//! \param firstRank The lowest-numbered rank to get.  If this is 20 and numRanks is 5, you get the players ranked 20-25 inclusive.
	//! \param numRanks How many players to retrieve, INCLUDING pivotPlayer.
	//! \param playerList Where the results should go.
	//! \return The HRESULT returned by XOnlineStatLeaderEnumerate.
	HRESULT
	BeginDownloadPlayerLadder( 
		XONLINETASK_HANDLE& handle, 
		const DWORD firstRank, 
		const DWORD numRanks, 
		OSIPlayerStatsList* playerList );

	//! \brief When the online task handle resulting from BeginDownloadPlayerLadder returns success, call 
	//! \brief FinishDownloadPlayerLadder to parse the results and fill in the player list that was
	//! \brief passed in to BeginDownloadPlayerLadder.  The resulting list's players will only have a 
	//! \brief valid name and ID.  Further stats should be retrieved via BeginDownload.
	//! \return S_OK or the HRESULT returned by a failed call to XOnlineStatLeaderEnumerateGetResults.
	HRESULT
	FinishDownloadPlayerLadder( 
		const XONLINETASK_HANDLE& handle );

	//! \brief Use BeginDownloadClanLadder to get a list of clans from the ladder that 
	//! \brief match the given critera.  You should pump the resulting handle until it
	//! \brief returns success, then call FinishDownloadClanLadder.  If the online task
	//! \brief falis, don't call FinishDownloadClanLadder.
	//! \param handle OUT- The handle returned by XOnlineStatLeaderEnumerate.
	//! \param firstRank The lowest-numbered rank to get.
	//! \param numRanks How many clans to retrieve, INCLUDING pivotClan.
	//! \param clanList Where the results should go.
	//! \return The HRESULT returned by XOnlineStatLeaderEnumerate.
	HRESULT
	BeginDownloadClanLadder(
		XONLINETASK_HANDLE& handle,
		const DWORD firstRank,
		const DWORD numRanks,
		OSIClanInfoList* clanList );

	//! \brief When the online task handle from BeginDownloadClanLadder returns success, call
	//! \brief FinishDownloadClanLadder to parse the results and fill in the clan list that was
	//! \brief passed in to BeginDownloadClanLadder.  The resulting list's clans will have valid
	//! \brief stats and ID.  Additional clan info should be retrieved with the ClansAdapter.
	//! \return S_OK or the HRESULT returned by a failed call to XOnlineStatLeaderEnumerateGetResults.
	HRESULT
	FinishDownloadClanLadder(
		const XONLINETASK_HANDLE& handle );
};

#include "OSIXboxStatsUtility.inline.hpp"
