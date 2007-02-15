/***************************************************************************/
// Xbox Input driver
/***************************************************************************/
#ifndef XboxINPUT_H
#define XboxINPUT_H

#include "platform/systemi.h"
#include "platform/Xbox/xboxctrol.h"

// Maximum number of controllers we want to support
#ifdef MAX_CONTROLLERS
#undef MAX_CONTROLLERS
#endif // MAXX_CONTROLLERS
#define MAX_CONTROLLERS 	4

class XboxSystemInputDriver : public SystemInputDriver
{
public:
	// Basic initialization, but remember to call Initialize
	XboxSystemInputDriver(void);

	// Clean up (remember to call Terminate!!)
	~XboxSystemInputDriver(void);

	// Call this at the very beginning to really intialize stuff
	bool Initialize(void);

	// Call this at the end to release system resources
	void Terminate(void);

	// Call this to beigin using input
	void BeginInput(void);

	// Clear the key input (call at the end of processing)
	void DoneInput(void);

	// How many controlles are there?
	uint Controllers(void)
		{ return(controllers); }

	virtual void UnlockAllControllers(void);
	virtual void LockPlayerToController(uint player, int controllerNum);
	virtual void UnlockPlayerFromController(uint playerNum);
	virtual bool ControllerLocked(uint player);
	virtual bool UnmappedControllerLocked(uint controllerNum);
	virtual int GetLockedControllerIndex(uint player);
	virtual int GetFirstUnlockedControllerIndex(void);
	virtual ControllerInputDriver *GetController(uint player);
	virtual ControllerInputDriver *GetUnlockedController(uint player);
	virtual ControllerInputDriver *GetAssertController(void);
	virtual ControllerInputDriver *GetFirstUnlockedController(void);
	virtual uint GetNumLockedControllers(void);

private:
	// Controller stuff
	uint								controllers;
	XboxControllerInputDriver	*controller[MAX_CONTROLLERS];
	int								lockedControllerIndex[MAX_CONTROLLERS];
	uint								numLockedControllers;
};

#endif
