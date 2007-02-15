/***************************************************************************/
// PS2 input driver
/***************************************************************************/

/******************* includes ****************************/

#include "platform/PlatformPCH.h"




/***************************************************************************/
// Start create
/***************************************************************************/
PS2SystemInputDriver::PS2SystemInputDriver(void)
{
	// Clear the controllers
	controllers = 0;
	for (int j = 0; j < MAX_CONTROLLERS; j++)
		controller[j] = NULL;
}

/***************************************************************************/
// Clean up (remember to call Terminate!!)
/***************************************************************************/
PS2SystemInputDriver::~PS2SystemInputDriver(void)
{
	Terminate();
}

/***************************************************************************/
// Call this at the very beginning to really intialize stuff
/***************************************************************************/
bool PS2SystemInputDriver::Initialize(void)
{
	// Init the system
 	scePadInit(0);

	// Create the first controller
	controller[0]  = new PS2ControllerInputDriver(0);
	controller[1]  = new PS2ControllerInputDriver(1);

	return(true);
}

/***************************************************************************/
// Call this at the end to release system resources
/***************************************************************************/
void PS2SystemInputDriver::Terminate(void)
{
	// Clear the controllers
	for (int j = 0; j < MAX_CONTROLLERS; j++)
		if (controller[j])
		{
			delete controller[j];
			controller[j] = NULL;
		}

	// Shut down the library
	scePadEnd();
}

/***************************************************************************/
// Call this to beigin using input
/***************************************************************************/
void PS2SystemInputDriver::BeginInput(void)
{
	// Do the controller stuff
	for (uint i = 0; i < MAX_CONTROLLERS; i++)
		if (controller[i])
			controller[i]->BeginInput();

}

/***************************************************************************/
// Clear the key input (call at the end of processing)
/***************************************************************************/
void PS2SystemInputDriver::DoneInput(void)
{
	// Do the controller stuff
	for (uint i = 0; i < MAX_CONTROLLERS; i++)
		if (controller[i])
			controller[i]->DoneInput();

}

