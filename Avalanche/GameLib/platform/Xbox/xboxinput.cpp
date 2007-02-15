/***************************************************************************/
// Xbox input driver
/***************************************************************************/

#include "platform/PlatformPCH.h"

#include "platform/Xbox/xboxinput.h"
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#include "cmdcon/cmdcon.h"

/***************************************************************************/
// Start create
/***************************************************************************/
XboxSystemInputDriver::XboxSystemInputDriver(void)
{
	// Clear the controllers
	controllers = 0;
	for (int j = 0; j < MAX_CONTROLLERS; j++)
		controller[j] = NULL;
	numLockedControllers = 0;
	UnlockAllControllers();
}

/***************************************************************************/
// Clean up (remember to call Terminate!!)
/***************************************************************************/
XboxSystemInputDriver::~XboxSystemInputDriver(void)
{
	Terminate();
}

/***************************************************************************/
// Call this at the very beginning to really intialize stuff
/***************************************************************************/
bool XboxSystemInputDriver::Initialize(void)
{
	XDEVICE_PREALLOC_TYPE deviceTypes[] =
	{
	   { XDEVICE_TYPE_GAMEPAD, MAX_CONTROLLERS },
	   { XDEVICE_TYPE_MEMORY_UNIT, 2 }, // Number of memory units we will access simultaneously
		{ XDEVICE_TYPE_VOICE_MICROPHONE, 1 },
		{ XDEVICE_TYPE_VOICE_HEADPHONE, 1 },
		{ XDEVICE_TYPE_HIGHFIDELITY_MICROPHONE, 1 }
	};

	// Init the system
	XInitDevices(sizeof(deviceTypes) / sizeof(XDEVICE_PREALLOC_TYPE), deviceTypes);

	// Create the controllers
	for (int i = 0; i < MAX_CONTROLLERS; i++)
	{
		DWORD port;


		switch (i)
		{
			case 0:
				port = XDEVICE_PORT0;
				break;

			case 1:
				port = XDEVICE_PORT1;
				break;

			case 2:
				port = XDEVICE_PORT2;
				break;

			case 3:
				port = XDEVICE_PORT3;
				break;

			default:
				ASSERT(false);
				break;
		}

		// Create the controller
		controller[i] = new XboxControllerInputDriver(port);
		++controllers;
	}

	return(true);
}

/***************************************************************************/
// Call this at the end to release system resources
/***************************************************************************/
void XboxSystemInputDriver::Terminate(void)
{
	// Clear the controllers
	for (int j = 0; j < MAX_CONTROLLERS; j++)
		if (controller[j])
		{
			delete controller[j];
			controller[j] = NULL;
		}
}

/***************************************************************************/
// Call this to beigin using input
/***************************************************************************/
void XboxSystemInputDriver::BeginInput(void)
{
	// Track changes for the insertions & removals
	DWORD insertions = 0;
	DWORD removals = 0;
	DWORD minsertions = 0;
	DWORD mremovals = 0;
	DWORD hinsertions = 0;
	DWORD hremovals = 0;

	XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &insertions, &removals);
	XGetDeviceChanges( XDEVICE_TYPE_VOICE_HEADPHONE, &hinsertions, &hremovals );
	XGetDeviceChanges( XDEVICE_TYPE_VOICE_MICROPHONE, &minsertions, &mremovals );

	// Do the controller stuff
	for (uint i = 0; i < MAX_CONTROLLERS; i++)
	{
		// Set our inserted and removed flags
		bool inserted = (insertions & 1) != 0;
		bool removed = (removals & 1) != 0;
		bool minserted = (minsertions & 1) != 0;
		bool mremoved = (mremovals & 1) != 0;
		bool hinserted = (hinsertions & 1) != 0;
		bool hremoved = (hremovals & 1) != 0;

		if (controller[i])
		{
			// Notify of changes
			controller[i]->NotifyControlChanges(inserted, removed, i);

			// Call the controller
			controller[i]->BeginInput(i);

			// --- headset
			controller[ i ]->NotifyHeadSetChange( i, minserted, mremoved, hinserted, hremoved );
		}

		// Shift to the next bit in the mask
		insertions = insertions >> 1;
		removals = removals >> 1;
		hinsertions = hinsertions >> 1;
		hremovals = hremovals >> 1;
		minsertions = minsertions >> 1;
		mremovals = mremovals >> 1;
	}

	// update memory units
	XGetDeviceChanges( XDEVICE_TYPE_MEMORY_UNIT, &insertions, &removals );
}

/***************************************************************************/
// Clear the key input (call at the end of processing)
/***************************************************************************/
void XboxSystemInputDriver::DoneInput(void)
{
	// Do the controller stuff
	for (uint i = 0; i < MAX_CONTROLLERS; i++)
		if (controller[i])
			controller[i]->DoneInput();
}

void XboxSystemInputDriver::UnlockAllControllers(void)
{
 	for (int j = 0; j < MAX_CONTROLLERS; j++)
	{
		lockedControllerIndex[j] = CONTROLLER_NOT_LOCKED;
	}
	numLockedControllers = 0;
}

void XboxSystemInputDriver::LockPlayerToController(uint playerNum, int controllerNum)
{
	for (int j = 0; j < MAX_CONTROLLERS; j++)
	{
		if ( lockedControllerIndex[j] == controllerNum )
		{
			ASSERTF(false, ("Controller %d already locked to player %d\n", controllerNum, j+1) );
			return;
		}
	}
	if ( lockedControllerIndex[playerNum] != CONTROLLER_NOT_LOCKED )
	{
		ASSERTF(false, ("Player %d is already locked to controller %d\n", playerNum+1, controllerNum) );
		return;
	}
	lockedControllerIndex[playerNum] = controllerNum;
	numLockedControllers++;
}

void XboxSystemInputDriver::UnlockPlayerFromController(uint playerNum)
{
	if ( lockedControllerIndex[playerNum] == CONTROLLER_NOT_LOCKED )
	{
		ASSERTF(false, ("Player %d is not locked to a controller so can't be unlocked\n", playerNum+1) );
		return;
	}
	numLockedControllers--;
	lockedControllerIndex[playerNum] = CONTROLLER_NOT_LOCKED;
}

ControllerInputDriver *XboxSystemInputDriver::GetController(uint player)
{
	ASSERT(player<MAX_CONTROLLERS);
	if ( player >= MAX_CONTROLLERS )
		return NULL;

	if ( lockedControllerIndex[player] == CONTROLLER_NOT_LOCKED )
	{
		ASSERTF(false, ("Controller for player %d is not locked\n", player+1) );
		// try the first unlocked controller
		ControllerInputDriver *nonNullController = GetFirstUnlockedController();
		if ( nonNullController != NULL )
			return nonNullController;

		// if there aren't anymore unlocked controllers, the first port that is not mapped
		uint i;
		for ( i=0; i<MAX_CONTROLLERS; i++)
		{
			if ( !UnmappedControllerLocked(i) )
				return controller[i];
		}
		// otherwise, just any controller we have will do
		return GetAssertController();
	}
	else
	{
		return controller[lockedControllerIndex[player]];
	}
}

ControllerInputDriver *XboxSystemInputDriver::GetUnlockedController(uint player)
{
	ASSERT(player<MAX_CONTROLLERS);
	if ( player >= MAX_CONTROLLERS )
		return NULL;
	else
		return controller[player];
}

bool XboxSystemInputDriver::ControllerLocked(uint player)
{
	ASSERT(player<MAX_CONTROLLERS);
	if ( player >= MAX_CONTROLLERS )
		return false;
	return ( lockedControllerIndex[player] != CONTROLLER_NOT_LOCKED );
}

int XboxSystemInputDriver::GetLockedControllerIndex(uint player)
{
	ASSERT(player<MAX_CONTROLLERS);
	if ( player >= MAX_CONTROLLERS )
		return CONTROLLER_NOT_LOCKED;
	return ( lockedControllerIndex[player] );
}

bool XboxSystemInputDriver::UnmappedControllerLocked(uint controllerNum)
{
	ASSERT(controllerNum<MAX_CONTROLLERS);
	if ( controllerNum >= MAX_CONTROLLERS )
		return false;

	// look through the list and see if any players have grabbed this controller
	int i;
	for ( i=0; i<MAX_CONTROLLERS; i++)
	{
		if ( lockedControllerIndex[i] == controllerNum )
			return true;
	}
	return false;
}

ControllerInputDriver *XboxSystemInputDriver::GetFirstUnlockedController(void)
{
	uint i;
	for ( i=0; i<MAX_CONTROLLERS; i++)
	{
		if ( !UnmappedControllerLocked(i) && controller[i]->Connected() )
			return controller[i];
	}
	return NULL;
}

int XboxSystemInputDriver::GetFirstUnlockedControllerIndex(void)
{
	uint i;
	for ( i=0; i<MAX_CONTROLLERS; i++)
	{
		if ( !UnmappedControllerLocked(i) && controller[i]->Connected() )
			return i;
	}
	return -1;
}

ControllerInputDriver *XboxSystemInputDriver::GetAssertController(void)
{
	int j;

	// first look for a controller the user has locked to
	for (j = 0; j < MAX_CONTROLLERS; j++)
	{
		if ( lockedControllerIndex[j] != CONTROLLER_NOT_LOCKED )
			return controller[lockedControllerIndex[j]];
	}

	// if none, then look for the first port with a controller in it
	for (j = 0; j < MAX_CONTROLLERS; j++)
	{
		if ( controller[j] != NULL && controller[j]->Connected() )
			return controller[j];
	}

	// no controllers so return NULL
	return controller[0];
//	return NULL;
}

uint XboxSystemInputDriver::GetNumLockedControllers(void)
{
	return(numLockedControllers);
}

