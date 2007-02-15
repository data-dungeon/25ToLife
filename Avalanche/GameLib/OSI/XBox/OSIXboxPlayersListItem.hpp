
#pragma once

// NOTE: the header for this class is in OSI and this file is in TTL.  
// This is because OSI needs full access to the class definition, 
// but can't deal with properly compiling the code because it uses TTL-specific
// voice code.

class OSIXboxPlayersListItem : public ITitlePlayersListItem
{
public:
	OSIXboxPlayersListItem();
	OSIXboxPlayersListItem( XUID xuid, const char* gamerTag, int portNumber = -1 )
	{
		Initialize( xuid, gamerTag, portNumber );
	}
	
	void Initialize( XUID xuid, const char* gamerTag, int portNumber = -1 );

	STDMETHODIMP_(CONST WCHAR*) GetName() CONST { return _text; }
	STDMETHODIMP_(DWORD)		GetBitflags() CONST { return 0; }
	STDMETHODIMP_(CONST XUID*)  GetXUID() CONST { return &_xuid; }
	STDMETHODIMP_(UIX_VOICE_STATUS_TYPE) GetVoiceStatus() CONST;
	STDMETHODIMP_(BOOL) IsTalking() CONST;

	// User defined sort function to allow sorting players by whatever criteria the
	// game wants.
	STDMETHODIMP_(INT) Compare(CONST ITitlePlayersListItem *B) CONST
	{
		return 0;
	}

public:
	static const int TEXT_LEN = XONLINE_GAMERTAG_SIZE;

	XUID					_xuid;
	WCHAR					_text[ TEXT_LEN ];
	DWORD					_port;
	uint8					_netzParticipantId;
};
