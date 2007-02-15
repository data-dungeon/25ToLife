#ifndef _INCLUDED_OSI_ACCOUNT_
#define _INCLUDED_OSI_ACCOUNT_

#include "OSIPlatformTypes.h"
#include "OSIHardwareProfile.hpp"
#include "OSIPlayer.hpp"

union OSIAccountID
{
	OSIu32 ps2;
	struct // XUID
	{
		OSIu64	userId;
		OSIu32	userFlags;
	} xBox;
};

inline bool operator ==( const OSIAccountID& lhs, const OSIAccountID& rhs )
{
#ifdef _XBOX
	return( lhs.xBox.userId == rhs.xBox.userId );
#else
	return( lhs.ps2 == rhs.ps2 );
#endif
}

inline bool operator !=( const OSIAccountID& lhs, const OSIAccountID& rhs )
{
	return !(lhs == rhs);
}


/// OSIAccount flags
enum
{
    FLAG_GENDER     = 1,
    FLAG_PROMOS     = 2,
    FLAG_SHARE_INFO = 4,
    FLAG_SAVE_PWD   = 8
};

class OSIAccount
{
    public:
		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

        OSIPlayer				player ;                       // this profile's player data
        OSIchar					password[ OSI_PASSWORD_LENGTH ] ;  // Account password
		OSIchar					email[ OSI_EMAIL_LENGTH ] ;  // Email address
        OSIHardwareProfile		hardwareProfile ;                // Configuration associated with this profile
 		OSIAccountID			id ;

		// Misc Data
		int birthdayMonth;
		int birthdayDay;
		int birthdayYear;
		unsigned char flags;
		OSIchar postalCode[OSI_ZIP_LENGTH];
		int countryCode;

	inline const OSIAccount& OSIAccount::operator=( const OSIAccount	&a )
	{
		if( &a == this ) { return *this; }

		player = a.player;
		id = a.id;

		OSIstrcpy(password, a.password);
		OSIstrcpy(email, a.email);
		OSIstrcpy(postalCode, a.postalCode);

		birthdayMonth = a.birthdayMonth;
		birthdayDay =  a.birthdayDay;
		birthdayYear = a.birthdayYear;
		flags = a.flags;
		countryCode = a.countryCode;

		//hardwareProfile = a.hardwareProfile; // Not used?

		return *this;
	}
};

#endif //_INCLUDED_OSI_ACCOUNT_

