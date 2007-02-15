/***************************************************************************/
// Sound driver enumerator class
/***************************************************************************/
#ifndef SDRIVERENUM_H
#define SDRIVERENUM_H

#define WIN32_LEAN_AND_MEAN
#ifdef _XBOX
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#else
#include <windows.h>
#include <mmsystem.h>
#endif
#include <dsound.h>

// Enumerate the available sound devices in the system
class SoundDriverEnumerator
{
public:
	// Create stuff
	SoundDriverEnumerator();
	~SoundDriverEnumerator();

	// Here is how we identify a system sound driver ID
	typedef LPGUID SOUNDDRIVERID;

	// Here is a brief description of a sound device
	struct SOUNDDRIVER
	{
		SOUNDDRIVERID	driverID;
		char				description[128];
		char				driverName[128];
	};

	// How many are there?
	int HowMany();

	// Get a specific driver description (between 0 and HowMany - 1)
	bool GetDriver(int index, SOUNDDRIVER *description);

protected:
	SOUNDDRIVER		*list;		// List
	int				howMany;		// How many are there?

	// The enumeration call
	bool Enumerated()
		{ return (list != NULL); }
	bool Enumerate();

	// The internal enumeration call
	static BOOL CALLBACK EnumerateCallback(LPGUID guid, LPCSTR description, LPCSTR module, LPVOID context);
};

#endif















