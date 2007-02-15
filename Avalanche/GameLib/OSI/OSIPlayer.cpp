


#include "OSIPlayer.hpp"
#include "OSIMatchList.hpp"

const unsigned int STATUS_STRING_SIZE = 256;

///------------------------------------------------------------------
/// Constructor
///------------------------------------------------------------------
OSIPlayer::OSIPlayer( void )
{
	name[ 0 ]						= '\0' ;    ///< Screen name
	lookingToPlay					= false ;	///< Wants to play a game
	isAFriend						= false ;	///< Is on my friends list (and I'm on theirs)
	hasRequestedMyFriendship		= false ;  	///< Has offered me a friend request
	haveRequestedTheirFriendship	= false ;	///< I have offered them a friend request
	clanRole						= OSI_CLAN_ROLE_UNKNOWN;
	currentMatchList				= new OSIMatchList;
	statRating						= 0;
	statRanking						= 0;
	statusString					= 0;
}


///------------------------------------------------------------------
/// Copy constructor
///------------------------------------------------------------------
OSIPlayer::OSIPlayer( const OSIPlayer& copyme )
{
	OSIstrcpy( name, copyme.name ) ;
	id								= copyme.id ;
	state							= copyme.state ;
	clanInfo						= copyme.clanInfo ;
	clanRole						= copyme.clanRole ;
	lookingToPlay					= copyme.lookingToPlay ;
	isAFriend						= copyme.isAFriend ;
	hasRequestedMyFriendship		= copyme.hasRequestedMyFriendship ;
	haveRequestedTheirFriendship	= copyme.haveRequestedTheirFriendship ;
	currentMatchList				= new OSIMatchList( *copyme.currentMatchList ) ;
	statRating						= copyme.statRating;
	statRanking						= copyme.statRanking;
	statusString = 0;
	if( copyme.statusString != 0 )
	{
		statusString = new char[ STATUS_STRING_SIZE ];
		OSIstrncpy( statusString, copyme.statusString, STATUS_STRING_SIZE );
	}
}

///------------------------------------------------------------------
/// Destructor
///------------------------------------------------------------------
OSIPlayer::~OSIPlayer( void )
{
	delete currentMatchList ;
	currentMatchList = NULL ;

	delete[] statusString;
	statusString = NULL;
}

///------------------------------------------------------------------
/// Equals operator
///------------------------------------------------------------------
const OSIPlayer& OSIPlayer::operator=( const OSIPlayer &a )
{
	if( &a == this ) { return *this; }

	OSIstrcpy(name, a.name);
	id								= a.id;
	state							= a.state;
	lookingToPlay					= a.lookingToPlay;
	isAFriend						= a.isAFriend;
	hasRequestedMyFriendship		= a.hasRequestedMyFriendship;
	haveRequestedTheirFriendship	= a.haveRequestedTheirFriendship;
	clanInfo						= a.clanInfo;
	clanRole						= a.clanRole;
	*currentMatchList				= *a.currentMatchList;
	statRating						= a.statRating;
	statRanking						= a.statRanking;
	statusString = 0;
	if( a.statusString != 0 )
	{
		if( statusString == 0 )
		{
			statusString = new char[ STATUS_STRING_SIZE ];
		}
		OSIstrncpy( statusString, a.statusString, STATUS_STRING_SIZE );
	}
	return *this;
}

///------------------------------------------------------------------
/// Get the full name for the player (with clan info if in a clan)
///
/// \param nameBuffer buffer to put name into, must be OSIClanInfo::MAX_FULL_NAME_LENGTH long
///------------------------------------------------------------------
const OSIchar* OSIPlayer::GetFullName( OSIchar* nameBuffer )
{
	return GetFullPlayerName( nameBuffer, clanInfo.tag, name );
}

void OSIPlayer::SetStatusString( const OSIchar* status )
{
	if( status == 0 ) return;

	if( statusString == 0 )
	{
		statusString = new char[ STATUS_STRING_SIZE ];
	}
	OSIstrncpy( statusString, status, STATUS_STRING_SIZE );
}