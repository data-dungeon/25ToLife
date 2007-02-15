#ifndef _INCLUDED_OSIPLAYERID_
#define _INCLUDED_OSIPLAYERID_

#include "OSIPlatformTypes.h"

///////////////////////////////////////////////////////////////////////////////////////
/// Multi-platform unique ID for a player.
///////////////////////////////////////////////////////////////////////////////////////
union OSIPlayerID
{
	OSIu32 ps2;

	// XUID - Keep in mind that this structure may not be packed and the sizeof this
	// whole union may be 16 bytes (it is on xbox/debug at least).
	struct 
	{
		OSIu64	userId;
		OSIu32	userFlags;
	} xBox;

	OSIPlayerID( OSIu64 xboxId = 0, OSIu32 xboxFlags = 0 )
	{
		xBox.userId = xboxId;
		xBox.userFlags = xboxFlags;
	}

	OSIPlayerID( OSIu32 ps2Id )
	{
		ps2 = ps2Id;
	}

	bool isValid() const	{ return xBox.userId != 0 ; }
	void setInvalid()		{ xBox.userId	= 0 ; }
};

inline bool operator ==( const OSIPlayerID& lhs, const OSIPlayerID& rhs )
{
#ifdef _XBOX
	return( lhs.xBox.userId == rhs.xBox.userId );
#else
	return( lhs.ps2 == rhs.ps2 );
#endif
}

inline bool operator !=( const OSIPlayerID& lhs, const OSIPlayerID& rhs )
{
	return !(lhs == rhs);
}

#endif //_INCLUDED_OSIPLAYERID_ 