#include "ttlpch.h"
#include "frontend/screens/screen_Invitations.h"
#include "data/screens/FriendsAndPlayers.h"

ScreenInvitations::ScreenInvitations( void )
{
	m_TimeIconStarted = 0;
	m_IconActive = false;
}

void ScreenInvitations::SetInvite( Invitation::Icon Icon, const char* pMessage )
{
	SetText( ID_INVITATIONS_MESSAGE, pMessage );

	Sprite* pIcon = GetSprite( ID_INVITATIONS_ICON );

	if( pIcon )
	{
		int IconId = 0;

		switch( Icon )
		{
		case Invitation::FriendInvite_Sent:
			IconId = ID_INVITATIONS_FRIENDSENT;
			break;

		case Invitation::FriendInvite_Received:
			IconId = ID_INVITATIONS_FRIENDRECEIVED;
			break;

		case Invitation::GameInvite_Sent:
			IconId = ID_INVITATIONS_GAMESENT;
			break;

		case Invitation::GameInvite_Received:
			IconId = ID_INVITATIONS_GAMERECEIVED;
			break;

		case Invitation::ClanInvite_Sent:
			IconId = ID_INVITATIONS_CLANSENT;
			break;

		case Invitation::ClanInvite_Received:
			IconId = ID_INVITATIONS_CLANRECEIVED;
			break;
		}

		Sprite* pSrcIcon = GetSprite( IconId );

		if( pSrcIcon )
		{
			float x, y;
			int r, g, b, a;
			pIcon->GetPosition( x, y );
			pIcon->GetFamilyColor( r, g, b, a );
			pIcon->Copy( pSrcIcon );
			pIcon->SetPosition( x, y );
			pIcon->SetFamilyColor( r, g, b, a );
			pIcon->Show();
		}
		else
		{
			pIcon->Hide();
		}
	}

	return;
}

void ScreenInvitations::SetIconOpacity( unsigned char Alpha )
{
	Sprite* pIcon = GetSprite( ID_INVITATIONS_ICON );

	if( pIcon )
		pIcon->m_state.familycolor.a = Alpha;

	return;
}

void ScreenInvitations::SetMessageOpacity( unsigned char Alpha )
{
	Sprite* pMessage = GetSprite( ID_INVITATIONS_MESSAGE );

	if( pMessage )
		pMessage->m_state.familycolor.a = Alpha;

	return;
}

void ScreenInvitations::OnScreenUpdate( void )
{
	ScreenBaseClass::OnScreenUpdate();
}