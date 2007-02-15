#ifndef _INCLUDED_OSI_HARDWARE_PROFILE_
#define _INCLUDED_OSI_HARDWARE_PROFILE_

#include "OSIPlatformTypes.h"

union OSIHardwareProfileID
{
	OSIu32						ps2 ;
	OSIu32						xBox ;
};


/**
 * Describes a network configuration.  On the PS2, this is the
 * name of the configuration that is used to connect to the internet.
 * Some platforms (PC) don't need conifgurations because it is handled
 * by the OS.
 */
class OSIHardwareProfile
{
	public:
		static void*					operator new( size_t size )								{ return OSImalloc( size ); }
		static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

		OSIHardwareProfileID	id ;
		OSIchar					name[ OSI_PLAYER_NAME_LENGTH ] ;       // name of a network configuration
};

#endif // _INCLUDED_OSI_HARDWARE_PROFILE_