/***************************************************************************/
// Windows input driver
/***************************************************************************/
#ifndef WININPUT_H
#define WININPUT_H

// Override the DirectInput version (see dinput.h for more info).
#define DIRECTINPUT_VERSION 0x0700

#include <dinput.h>

#include "platform/systemi.h"
#include "platform/Win32/winctrol.h"

class WindowsSystemInputDriver : public SystemInputDriver
{
public:
	// Basic initialization, but remember to call Initialize
	WindowsSystemInputDriver();

	// Clean up (remember to call Terminate!!)
	~WindowsSystemInputDriver();

	// Call this at the very beginning to really intialize stuff
	bool Initialize(HINSTANCE hInst, HWND hWnd);

	// Call this at the end to release system resources
	void Terminate();

	// Call this to beigin using input
	void BeginInput();

	// Clear the key input (call at the end of processing)
	void DoneInput();

	// Assume keyboard, someday check
	bool KeyboardConnected()
		{ return true; }

	// Was this button pressed last frame?
	bool ButtonPressed(ButtonCode button)
		{ return(!state[button].last && state[button].now); }

	bool ButtonReleased(ButtonCode button)
		{ return(state[button].last && !state[button].now); }

	// Is this key currenly down?
	bool ButtonDown(ButtonCode button)
		{ return(state[button].now); }

	// Assume mouse, someday check
	bool MouseConnected()
		{ return true; }

	// Mouse position stuff
	int MouseX()
		{ return(mouseX); }
	int MouseY()
		{ return(mouseY); }

#ifdef DIRECTX_PC
	bool LeftMouseButtonDown() 
		{ return asyncState[ASYNC_MOUSELEFT]; }
	bool RightMouseButtonDown()
		{ return asyncState[ASYNC_MOUSERIGHT]; }
    bool wasMouseMoved()
        { return (mouseInputLastFrame); }
	int	 wasMouseWheelMoved()
		{	if (asyncState[ASYNC_MOUSEWHEELUP]) return 1;
			else if (asyncState[ASYNC_MOUSEWHEELDOWN]) return -1;
			return 0; }
#endif

	// How many controlles are there?
	uint Controllers()
		{ return(controllers); }

	// Get a controller interface (these pointers are "static", you don't delete them)
	ControllerInputDriver *GetController(uint player)
		{ return(player < MAX_CONTROLLERS ? controller[player] : 0); }

	// Watch keyboard messages
	void HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	// Direct input stuff
	LPDIRECTINPUT				dInput;
	HWND							hWnd;

	// Here is how we track each keyboard button
	typedef struct
	{
		bool		last, now;
	} BUTTONSTATE;

	// All of our states
	BUTTONSTATE	state[BC_BUTTONCODES];

	// Mouse cursor stuff
	int mouseX, mouseY;

#ifdef DIRECTX_PC
    bool mouseInputLastFrame;
#endif
	
	// The key to button mapper
	int16	keyToButtonMap[256];
	int 	mapEntries;

	// Load the joystick map file
	static void LoadDeviceMap();
	struct DeviceMap
	{
		char 	name[260];
		int	controller;
		bool	inUse;
	};
	static DeviceMap deviceMap[MAX_CONTROLLERS];
	static int deviceMaps;
	static int FindDevice(const char *deviceName);
	static bool HaveDeviceMap()
		{ return deviceMaps > 0; }

	// Enumerate and create the controller objects
	static BOOL CALLBACK EnumJoysticks(LPCDIDEVICEINSTANCE devInst, LPVOID context);

	// Controller stuff
	uint controllers;
	ControllerInputDriver	*controller[MAX_CONTROLLERS];

	// Asynchronous mouse stuff
	typedef enum
	{
		ASYNC_MOUSELEFT = 0,
		ASYNC_MOUSEMIDDLE,
		ASYNC_MOUSERIGHT,
		ASYNC_MOUSEWHEELUP,
		ASYNC_MOUSEWHEELDOWN,
		ASYNC_ENTRIES
	} ASYNC_STATES;
	bool	asyncState[ASYNC_ENTRIES];
};

#endif
