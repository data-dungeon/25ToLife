#include "pch.hpp"
#include "OSIXbox.hpp"
#include "OSIXboxClansUtility.hpp"

ClansAdapter* ClansAdapter::m_singletonPtr = 0;
#ifdef _DEBUG
BYTE ClansAdapter::s_clansAdapterInstanceNumber = 0;
#endif //_DEBUG

//-----------------------------------------------------------------------------
void
ClansAdapter::
BeginDownloadClanInfo( 
	OSIClanInfoList* clans )
//-----------------------------------------------------------------------------
{
	if( !clans || clans->count() == 0 )
	{
		ASSERT( clans && clans->count() > 0 );
		return;
	}

	// Copy the pointer to the clan list.
	m_clanList = clans;

	// Initialize the count/size variables
	m_downloadClanDataTotal = m_clanList->count();
	m_downloadClanDataFinished = 0;

	// Allocate memory for download data (each item is 32 bytes, 3k at worst)
	if( m_downloadClanData ) { delete [] m_downloadClanData; }
	m_downloadClanData = new DownloadClanData[ m_downloadClanDataTotal ];

	// Iterate through all players, translate the OSIPlayer data into the
	// DownloadClanData, then kick off the XOnline call to get their clan info.
	DownloadClanData* downloadDataPtr = m_downloadClanData;
	const DWORD controllerPort = 
		OSI::getSingletonPtr()->getActivePlayer()->state.currentControllerIndex;
	OSIClanInfo* clan = 0;
	for( m_clanList->begin()
		; clan = m_clanList->getCurrent()
		; m_clanList->next(), ++downloadDataPtr )
	{
		// initialize download data
		ZeroMemory( downloadDataPtr, sizeof( *downloadDataPtr ) );
		#ifdef _DEBUG
		m_downloadClanData->instanceNumberAtBegin = s_clansAdapterInstanceNumber;
		#endif //_DEBUG

		downloadDataPtr->id = clan->clanID.xBox.clanId;
		downloadDataPtr->flags = clan->clanID.xBox.userFlags;
		XUID xuid;
		OSIClanID_to_XUID( clan->clanID, xuid );

		// kick off request for clan info
		downloadDataPtr->hr = XOnlineTeamEnumerate(
			controllerPort,
			1,
			&xuid,
			0,
			&downloadDataPtr->taskHandle );

		// handle special case for tasks that complete immediately
		if( FAILED( downloadDataPtr->hr ) 
			|| (XONLINETASK_S_SUCCESS == downloadDataPtr->hr) )
		{
			++m_downloadClanDataFinished;
		}
	}
}

//-----------------------------------------------------------------------------
void	
ClansAdapter::
BeginDownloadClanInfo( 
	OSIPlayerList* players )
//-----------------------------------------------------------------------------
{
	// Nothing to do?
	if( !players || players->count() == 0 )
	{
		ASSERT( players && players->count() > 0 );
		return;
	}

	// Copy the pointer to the player list.
	m_playerList = players;

	// Initialize count/size variables.
	m_downloadClanDataTotal = m_playerList->count();
	m_downloadClanDataFinished = 0;

	// Allocate memory for download data (each item is 32 bytes, 3k at worst)
	if( m_downloadClanData ) { delete [] m_downloadClanData; }
	m_downloadClanData = new DownloadClanData[ m_downloadClanDataTotal ];

	// Iterate through all players, translate the OSIPlayer data into the
	// DownloadClanData, then kick off the XOnline call to get their clan info.
	DownloadClanData* downloadDataPtr = m_downloadClanData;
	const DWORD controllerPort = 
		OSI::getSingletonPtr()->getActivePlayer()->state.currentControllerIndex;
	OSIPlayer* player = 0;
	for( m_playerList->begin()
		; player = m_playerList->getCurrent()
		; m_playerList->next(), ++downloadDataPtr )
	{
		// initialize download data
		ZeroMemory( downloadDataPtr, sizeof( *downloadDataPtr ) );
		#ifdef _DEBUG
		m_downloadClanData->instanceNumberAtBegin = s_clansAdapterInstanceNumber;
		#endif //_DEBUG

		downloadDataPtr->id = player->id.xBox.userId;
		downloadDataPtr->flags = player->id.xBox.userFlags;
		XUID xuid;
		OSIPlayerID_to_XUID( player->id, xuid );

		// kick off request for clan info
		downloadDataPtr->hr = XOnlineTeamEnumerateByUserXUID(
			controllerPort,
			xuid,
			0,
			&downloadDataPtr->taskHandle );

		// handle special case for tasks that complete immediately
		if( FAILED( downloadDataPtr->hr ) 
			|| (XONLINETASK_S_SUCCESS == downloadDataPtr->hr) )
		{
			++m_downloadClanDataFinished;
		}
	}
}

//-----------------------------------------------------------------------------
void
ClansAdapter::
ContinueDownloadClanInfo()
//-----------------------------------------------------------------------------
{
	ASSERT( m_downloadClanData );
	#ifdef _DEBUG
	ASSERT( (m_downloadClanData->instanceNumberAtBegin
		== s_clansAdapterInstanceNumber) );
	#endif //_DEBUG

	// Shoudln't be calling this if everything is finished.
	if( IsDownloadClanInfoComplete() )
	{
		ASSERT( !IsDownloadClanInfoComplete() );
		return;
	}

	// Process all of the taskHandles that haven't finished yet.
	for( DWORD i = 0; i < m_downloadClanDataTotal; i++ )
	{
		DownloadClanData* const dataPtr = &m_downloadClanData[ i ];

		// Don't use SUCCEEDED macro here since XONLINETASK_S_RUNNING == S_OK.
		if( FAILED( dataPtr->hr ) || (XONLINETASK_S_SUCCESS == dataPtr->hr) )
		{
			continue;
		}

		dataPtr->hr = XOnlineTaskContinue( dataPtr->taskHandle );

		// If the task just finished, count it as done.
		// Results are parsed in FinishDownloadClanInfo.
		if( FAILED( dataPtr->hr ) || (XONLINETASK_S_SUCCESS == dataPtr->hr) )
		{
			++m_downloadClanDataFinished;
		}
	}
}

//-----------------------------------------------------------------------------
void
ClansAdapter::
FinishDownloadClanInfo()
//-----------------------------------------------------------------------------
{
	ASSERT( m_downloadClanData );
	#ifdef _DEBUG
	ASSERT( (m_downloadClanData->instanceNumberAtBegin
		== s_clansAdapterInstanceNumber) );
	#endif //_DEBUG

	// Shouldn't be calling this until everything is finished.
	if( !IsDownloadClanInfoComplete() )
	{
		ASSERT( IsDownloadClanInfoComplete() );
		return;
	}

	// Iterate through the results and update the player list accordingly.
	for( DWORD i = 0; i < m_downloadClanDataTotal; i++ )
	{
		DownloadClanData* const dataPtr = &m_downloadClanData[ i ];
		
		ASSERT( FAILED( dataPtr->hr ) 
			|| (XONLINETASK_S_SUCCESS == dataPtr->hr) );

		// Find the player or clan in the OSI list.
		// m_playerList is valid if BeginDownload was called with a player list.
		if( m_playerList )
		{
			OSIPlayer* player = m_playerList->findPlayerByID( OSIPlayerID( dataPtr->id, dataPtr->flags ) );
			if( !player ) 
			{
				ASSERT( player );
				continue; 
			}
			ParseDownloadClanDataResult( dataPtr, player );
			XOnlineTaskClose( dataPtr->taskHandle );
			dataPtr->taskHandle = (XONLINETASK_HANDLE) INVALID_HANDLE;
		}

		// m_clanList is valid if BeginDownload was called with a clan info list.
		if( m_clanList )
		{
			OSIClanInfo* clan = m_clanList->findClanInfoByID( OSIClanID( dataPtr->id, dataPtr->flags ) );
			if( !clan )
			{
				ASSERT( clan );
				continue;
			}
			ParseDownloadClanDataResult( dataPtr, clan );
			XOnlineTaskClose( dataPtr->taskHandle );
			dataPtr->taskHandle = (XONLINETASK_HANDLE) INVALID_HANDLE;
		}
	}

	// This is some special case code that will update the local player's clan
	// info if it is downloaded.
	if( m_playerList )
	{
		OSIXbox* osix = (OSIXbox*) OSI::getSingletonPtr();
		ASSERT( osix );
		OSIPlayer* localPlayer = osix->getActivePlayer();
		ASSERT( localPlayer );
		OSIPlayer* localPlayerWithClanInfo = m_playerList->findPlayerByID( localPlayer->id );
		if( localPlayerWithClanInfo )
		{
			localPlayer->clanInfo = localPlayerWithClanInfo->clanInfo;
		}
	}
}

//-----------------------------------------------------------------------------
void
ClansAdapter::
ParseDownloadClanDataResult(
	DownloadClanData* const dataPtr,
	OSIPlayer* const player )
//-----------------------------------------------------------------------------
{
	// Declare a blank claninfo struct to use for assigning to players' 
	// claninfo that have no clan info or for which the data download failed
	// for whatever reason.  The constructor handles initialization.
	const OSIClanInfo blankClanInfo;

    if( XONLINETASK_S_SUCCESS != dataPtr->hr )
	{
		player->clanInfo = blankClanInfo;
		return;
	}

	// There should only be one team, but allocate more than that to prevent 
	// stack smashing if something goes weird.
	XUID xuid[ XONLINE_MAX_TEAM_COUNT ];
	DWORD count = 0;

	// XOnlineTeamGetDetails has the desired info, but we have to call 
	// GetResults even though we don't really care about what it does.
	// Xbox developer support says that GetResults internally does some memory 
	// copies that XOnlineTeamGetDetails relies on.
	dataPtr->hr = XOnlineTeamEnumerateGetResults(
		dataPtr->taskHandle, 
		&count, 
		xuid );

	ASSERT( count <= 1 );
	if( (1 != count) || (S_OK != dataPtr->hr) ) { return; }
	
	XONLINE_TEAM teamInfo;

	dataPtr->hr = XOnlineTeamGetDetails(
		dataPtr->taskHandle,
		xuid[0],
		&teamInfo );

	if( S_OK != dataPtr->hr ) { return; }

	OSIstrncpy( player->clanInfo.tag, 
		teamInfo.TeamProperties.wszTeamName, 
		NUM_ARRAY_ELEMENTS( player->clanInfo.tag ) );
	OSIstrncpy( player->clanInfo.name, 
		teamInfo.TeamProperties.wszDescription, 
		NUM_ARRAY_ELEMENTS( player->clanInfo.name ) );
	player->clanInfo.memberCount = teamInfo.dwMemberCount;
	XUID_to_OSIClanID( teamInfo.xuidTeam, player->clanInfo.clanID );

	// I don't fill in the leadername here because the only time that information
	// is used is in the "my clan" screen, which uses 
	// OSI::requestClanUpdateMyInformation, and getting that information
	// requires enumerating all of the members and iterating them to find
	// who has the 0xfffffffff permissions.  Doing that for this data, right now,
	// is overkill and will just slow down this process.
	player->clanInfo.leaderName[0] = 0;

}


//-----------------------------------------------------------------------------
void
ClansAdapter::
ParseDownloadClanDataResult(
	DownloadClanData* const dataPtr,
	OSIClanInfo* const clan )
//-----------------------------------------------------------------------------
{
	// Declare a blank claninfo struct to use for assigning to players' 
	// claninfo that have no clan info or for which the data download failed
	// for whatever reason.  The constructor handles initialization.
	const OSIClanInfo blankClanInfo;

    if( XONLINETASK_S_SUCCESS != dataPtr->hr )
	{
		*clan = blankClanInfo;
		return;
	}

	// There should only be one team, but allocate more than that to prevent 
	// stack smashing if something goes weird.
	XUID xuid[ XONLINE_MAX_TEAM_COUNT ];
	DWORD count = 0;

	// XOnlineTeamGetDetails has the desired info, but we have to call 
	// GetResults even though we don't really care about what it does.
	// Xbox developer support says that GetResults internally does some memory 
	// copies that XOnlineTeamGetDetails relies on.
	dataPtr->hr = XOnlineTeamEnumerateGetResults(
		dataPtr->taskHandle, 
		&count, 
		xuid );

	ASSERT( count <= 1 );
	if( (1 != count) || (S_OK != dataPtr->hr) ) { return; }
	
	XONLINE_TEAM teamInfo;

	dataPtr->hr = XOnlineTeamGetDetails(
		dataPtr->taskHandle,
		xuid[0],
		&teamInfo );

	if( S_OK != dataPtr->hr ) { return; }

	OSIstrncpy( clan->tag, 
		teamInfo.TeamProperties.wszTeamName, 
		NUM_ARRAY_ELEMENTS( clan->tag ) );
	OSIstrncpy( clan->name, 
		teamInfo.TeamProperties.wszDescription, 
		NUM_ARRAY_ELEMENTS( clan->name ) );
	clan->memberCount = teamInfo.dwMemberCount;
	XUID_to_OSIClanID( teamInfo.xuidTeam, clan->clanID );

	// I don't fill in the leadername here because the only time that information
	// is used is in the "my clan" screen, which uses 
	// OSI::requestClanUpdateMyInformation, and getting that information
	// requires enumerating all of the members and iterating them to find
	// who has the 0xfffffffff permissions.  Doing that for this data, right now,
	// is overkill and will just slow down this process.
	clan->leaderName[0] = 0;
}






















