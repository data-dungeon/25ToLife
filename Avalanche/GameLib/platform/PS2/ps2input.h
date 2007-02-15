/***************************************************************************/
// PS2 Input driver
/***************************************************************************/
#ifndef PS2INPUT_H
#define PS2INPUT_H

#include "platform/systemi.h"
#include "platform/PS2/ps2ctrol.h"
class CMouse;

// Maximum number of controllers we want to support (just 1 for now)
#define MAX_CONTROLLERS 	2
#define MAX_MICE 1

class PS2SystemInputDriver : public SystemInputDriver
{
public:
	// Basic initialization, but remember to call Initialize
	PS2SystemInputDriver(void);

	// Clean up (remember to call Terminate!!)
	~PS2SystemInputDriver(void);

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

	// Get a controller interface (these pointers are "static", you don't delete them)
	ControllerInputDriver *GetController(uint player)
		{ return(player < MAX_CONTROLLERS ? controller[player] : 0); }

private:
	// Controller stuff
	uint controllers;
	ControllerInputDriver	*controller[MAX_CONTROLLERS];
};

#endif
