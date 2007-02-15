/***************************************************************************/
// Windows controller input
/***************************************************************************/
#ifndef WINCTROL_H
#define WINCTROL_H

// Override the DirectInput version (see dinput.h for more info).
#define DIRECTINPUT_VERSION 0x0700

#include <dinput.h>
#include "platform/ctroller.h"
#include "platform/Win32/wininput.h"

// Resolution of our transfer functions
#define TRANSFER_LOOKUPS 		256

// An award for the longest class name perhaps?
class WindowsControllerInputDriver : public ControllerInputDriver
{
public:
	WindowsControllerInputDriver(HWND hWnd, LPDIRECTINPUTDEVICE joystickDevice, int deviceNumber);
	~WindowsControllerInputDriver();

	// What is the device name of this controller, you have to worry about
	// foriegn versions of this	
	const char *DeviceName();

	// Setup a stick axis
	bool SetupAxis(AnalogAxis axis,
						AxesTransferFunction transferFunction,
						float factor = 1.0f,
						float deadZonePercent = 2.0f,
						float clampZonePercent = 5.0f);

	// Is the controller currently connected?
	bool Connected();

	// The input system calls this to "refresh" input at frame start
	void BeginInput();

	// The input system calls this to "flush" input at frame end
	void DoneInput();

	// Internal for focus changes
	void Activate(bool active);

	// should we observe focus changes to enable/disable ourselves?
	virtual bool ObserveFocus();

private:
	// Direct input shlock
	LPDIRECTINPUTDEVICE	device;
	LPDIRECTINPUTDEVICE2	device2;

	// setup the device
	bool SetupDevice(HWND hWnd);
	bool SetupAxis(DWORD diAxis);
	bool axisValid[8];

	// pay attention to the window focus to enable/disable input?
	bool observeFocus;

	// Our transfer functions
	float transferLookup[AA_AXES][TRANSFER_LOOKUPS];

	// Our setup
	void LoadProfile(int deviceNumber);
	char deviceName[64];

	// The button mapping array
	int buttonMap[CB_BUTTONS];
	int axisMap[AA_AXES];

	// Get a button mapping from the ini file
	static char *buttonMapFile[MAX_CONTROLLERS];
	bool GetProfileButton(int deviceNumber, char *buttonName, int &value);
	bool GetProfileAxis(int deviceNumber, char *axisName, int &value);
	bool GetProfileBool(int deviceNumber, char *boolName, bool &value);

  	// Get a transferfunction/setup for each stick
	AxesTransferFunction	initTransferFunction[AA_AXES];
	float						initFactor[AA_AXES];
	float 					initDeadZone[AA_AXES];
	float 					initClampZone[AA_AXES];
	bool 						invert[AA_AXES];
	bool GetProfileStick(int deviceNumber, int stick);

	// set a button state
	inline void SetState( ControllerButton button, bool on);

};

/***************************************************************************/
/***************************************************************************/
inline void WindowsControllerInputDriver::SetState( ControllerButton button, bool on)
{
	state[button].last = state[button].now;
	state[button].now = on;
}
#endif

