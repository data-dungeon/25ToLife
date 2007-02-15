
#pragma once

///----------------------------------------------------------------------------
//! \brief This class handles asynchronous retrieval of a player's state on
//! \brief live, including if they are online, if they are playing, which game
//! \brief they are playing in, and if that game is joinable.
///----------------------------------------------------------------------------
class PlayerPresenceHelper
{
public:

	PlayerPresenceHelper();
	~PlayerPresenceHelper();

	//! \brief Purge all tasks and memory used by this class.
	void 
	Reset();

	//! \brief Begin getting presence info for another player.
	//! \note There is no RemovePlayer, so you must Reset() and re-add. This
	//! \note is because the Live API doesn't have RemovePlayer.
	void
	AddPlayer(
		const OSIPlayerID& playerID );

	//! \brief Call this every frame to pump any tasks that might be active.
	void 
	Update();

	//! \return True if the specified player's info has changed since the last
	//! \return call to GetPlayerInfo.
	//! \note Once this returns true, it will continue to return true until 
	//! \note someone calls SetIsPlayerInfoNew( id, false ).
	bool 
	IsPlayerInfoNew( 
		const OSIPlayerID& playerID )
	const;

	//! \brief Call this to flag a player's info as updated or not.
	void
	SetIsPlayerInfoNew(
		const OSIPlayerID& playerID, 
		const bool isNew );

	//! \brief Get the latest info for the specified player.
	void 
	GetPlayerInfo( 
		OSIPlayerID& playerID, 
		bool& isOnline, 
		bool& isPlaying, 
		bool& isJoinable, 
		OSIMatchID& matchID )
	const;

private:

	XONLINETASK_HANDLE m_presenceTask;

	//! \brief Holds the XUIDs of all players that should be registered with
	//! \brief XOnlinePresenceAdd along with some flags.
	//! \note AddPlayer inserts to this list, and nothing removes from it
	//! \note because there is no support for it in the Live API.
	struct PresenceUser
	{ 
		PresenceUser( const OSIPlayerID& id ) 
		{
			OSIPlayerID_to_XUID( id, xuid );
			isUpdated = false;
		}
		XONLINE_PRESENCE presence;
		XONLINE_PRESENCE oldPresence;
		XUID xuid;
		bool isUpdated;
	};
	DynamicArray< PresenceUser* > m_presenceUsers;
};

