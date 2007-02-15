
#include "TTLPCH.h"
#include "osi/xbox/OSIXbox.hpp"
#include "osi/xbox/OSIXboxPlayersListItem.hpp"

// NOTE: the header for this class is in OSI and this file is in TTL.  
// This is because OSI needs full access to the class definition, 
// but can't deal with properly compiling the code because it uses TTL-specific
// voice code.

OSIXboxPlayersListItem::OSIXboxPlayersListItem()
{
	XUID zero = {0, 0};
	Initialize( zero, "" );
	_netzParticipantId = INVALID_PARTICIPANT;
}

void OSIXboxPlayersListItem::Initialize( XUID xuid, const char* gamerTag, int portNumber )
{
	_xuid		= xuid;
	_port		= portNumber;		// -1 means remote user

	// Note that this _snwprintf call also converts the gamertag to Unicode
	// We print the text we want displayed for the player. This may be just
	// the player's gamertag, or it may include other information.
	if( XOnlineIsUserGuest( _xuid.dwUserFlags ) )
	{
		// For guests, it is advisable to print something that indicates that
		// they are a guest. The 'Score' field should be omitted if it is
		// from a leaderboard, since that makes no sense for a guest.
		_snwprintf( _text, TEXT_LEN, L"Guest of %S", gamerTag );
	}
	else
	{
		// For regular (non-guest) players you can just print the gamer tag
		// or you can display additional information, such as a ranking,
		// current score in this game, etc.
		_snwprintf( _text, TEXT_LEN, L"%S", gamerTag );
	}
	
	// Ensure that the string is definitely terminated.
	_text[ TEXT_LEN-1 ] = 0;

	Referee& ref = g_referee;
	CPlayerAgent* pAgent;
	uint8 id = INVALID_PARTICIPANT;
	while( ( id = ref.GetNextPlayer( id ) ) != INVALID_PARTICIPANT )
	{
		pAgent = ref.GetPlayerAgent( id );
		if( !memcmp( &xuid, pAgent->GetAccountId(), sizeof(xuid) ) )
		{
			_netzParticipantId = id;
			break;
		}
	}
	ASSERT( _netzParticipantId != INVALID_PARTICIPANT );
}

STDMETHODIMP_(UIX_VOICE_STATUS_TYPE) 
OSIXboxPlayersListItem::GetVoiceStatus() CONST
{
	if( INVALID_PARTICIPANT == _netzParticipantId ) { return UIX_VOICE_STATUS_NONE; }

	if( g_referee.IsParticipantVoiceReDirected(_netzParticipantId) )
		return UIX_VOICE_STATUS_SPEAKERS;
	else if( g_referee.IsParticipantHeadsetConnected(_netzParticipantId) )
		return UIX_VOICE_STATUS_COMMUNICATOR;
	else
		return UIX_VOICE_STATUS_NONE;
}

STDMETHODIMP_(BOOL) 
OSIXboxPlayersListItem::IsTalking() CONST
{
	if( INVALID_PARTICIPANT == _netzParticipantId ) { return FALSE; }

	return g_referee.IsParticipantTalking(_netzParticipantId);
}
