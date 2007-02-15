//-------------------------------------------------------------------------------------------------
// OSIPlatformTypes.hpp
//
// Provides multi-platform typedefs.
//-------------------------------------------------------------------------------------------------
#ifndef OSIPLATFORMTYPES_INC
#define OSIPLATFORMTYPES_INC


//-------------------------------------------------------------------------------------------------
// Included header files
//-------------------------------------------------------------------------------------------------
#include "OSIchar.hpp"

const int OSI_PLAYER_NAME_LENGTH	= 16 ;		// Names are 15 characters plus NULL terminator
const int OSI_LEVEL_NAME_LENGTH		= 32 ;		// Level names are up to 31 characters plus NULL terminator
const int OSI_CLAN_NAME_LENGTH		= 16 ;		// Clan names are 15 characters plus NULL terminator
const int OSI_CLAN_TAG_LENGTH		= 4 ;		// Clan tags are 3 characters plus NULL terminator
const int OSI_PASSWORD_LENGTH		= 16 ;		// Passwords are 15 characters plus NULL terminator
const int OSI_EMAIL_LENGTH			= 256 ;		// Email addresses are 255 characters plus NULL terminator
const int OSI_YEAR_LENGTH			= 5 ;		// 4 digits plus NULL terminator
const int OSI_ZIP_LENGTH			= 10 ;		// 9 digits plus NULL terminator
const int OSI_PLAYER_FULL_NAME_LENGTH	= OSI_PLAYER_NAME_LENGTH + OSI_CLAN_TAG_LENGTH + 5 ; // <tag>:name plus NULL terminator
const int OSI_STRING_LENGTH_LONG	= 255;		// For server description names, etc.
const int OSI_STRING_LENGTH_HUGE	= 512;		// For error messages, etc.
const int OSI_MAX_RECENT_PLAYERS	= 16;		// _recentPlayerList max size before popping off players

extern void* OSImalloc( size_t size );
extern void  OSIfree( void* ptr );

//-------------------------------------------------------------------------------------------------
// XBox-Specific
//
#ifdef _XBOX
//-------------------------------------------------------------------------------------------------
typedef unsigned int		OSIu32;
typedef unsigned __int64	OSIu64;


//-------------------------------------------------------------------------------------------------
// PS2-Specific
//
#else // !_XBOX
//-------------------------------------------------------------------------------------------------
typedef unsigned int		OSIu32;
typedef unsigned long		OSIu64;

#endif // !_XBOX





#endif // OSIPLATFORMTYPES_INC
