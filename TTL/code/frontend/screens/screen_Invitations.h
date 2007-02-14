#ifndef _SCREEN_FRIENDINVITE_H_
#define _SCREEN_FRIENDINVITE_H_

#include "frontend/screens/screen_BaseClass.h"

namespace Invitation
{
	typedef enum
	{
		FriendInvite_Sent,
		FriendInvite_Received,
		GameInvite_Sent,
		GameInvite_Received,
		ClanInvite_Sent,
		ClanInvite_Received,
	}
	Icon;
};

class ScreenInvitations : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenInvitations );

	public:
		ScreenInvitations( void );

		void SetInvite( Invitation::Icon Icon, const char* pMessage );
		void SetIconOpacity( unsigned char Alpha );
		void SetMessageOpacity( unsigned char Alpha );

	protected:
		virtual void OnScreenUpdate( void );

		unsigned int m_TimeIconStarted;
		bool m_IconActive;
};

#endif // _SCREEN_FRIENDINVITE_H_