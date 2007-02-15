/***************************************************************************/
// Xbox controller input
/***************************************************************************/
#ifndef XboxCTROL_H
#define XboxCTROL_H

// Xbox stuff
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#include "platform/ctroller.h"
#include "EngineHelper/timer.h"

// Resolution of our transfer functions
#define TRANSFER_LOOKUPS 		256

// An award for the longest class name perhaps?
class XboxControllerInputDriver : public ControllerInputDriver
{
public:
	XboxControllerInputDriver(DWORD port);
	~XboxControllerInputDriver(void);

	const char *DeviceName();

	// Setup a stick axis
	bool SetupAxis(AnalogAxis axis,
						AxesTransferFunction transferFunction,
						float factor = 1.0f,
						float deadZonePercent = 20.0f,
						float clampZonePercent = 2.0f);

	// Is the controller currently connected?
	bool Connected(void);

	// Turns vibration on/off globally.
	void Vibration( bool allow );

	// Activate the vibration/forcefeedback function of the controller
	// Intensity is specified VIBRATEINTENSITY
	// pan is specified using VIBRATEPAN.
	// Returns true/false based on controller's ability to vibrate
	bool SetVibration(uint intensity, VIBRATEPAN pan = CENTER);

	// Do the vibration for duration milliseconds, with the above intensity/pan
	// values.
	bool SetVibrationDuration(uint duration, uint intensity, VIBRATEPAN pan = CENTER);

	// Make it so there is no vibration
	bool ClearVibration(bool force = false);

	// Notify of controller changes
	void NotifyControlChanges(bool inserted, bool removed, int newPort);

	// The input system calls this to "refresh" input at frame start
	void BeginInput(int i);

	// The input system calls this to "flush" input at frame end
	void DoneInput(void);

	// Internal for focus changes
	void Activate(bool active);

    // For UIX polling of input devices -CJS, Ritual
    HANDLE GetHandle() { return hDevice; }

	 // --- headset changes
	 void NotifyHeadSetChange( int port, bool micInserted, bool micRemoved, bool headInserted, bool headRemoved );

private:

	// Our transfer functions
	float transferLookup[AA_AXES][TRANSFER_LOOKUPS];
	inline void TranslateAxis(AnalogAxis axis, SHORT value, bool flip = false);

	char deviceName[64];

	// Xbox stuff
    HANDLE				hDevice;
	DWORD			  		port;
	XINPUT_FEEDBACK 	feedback;

	// Create/destroy the device
	bool CreateDevice(void);
	void DestroyDevice(void);

	// Vibration support
	uint					vibrationCutoff;

	// set a button state
	inline void SetState(ControllerButton button, bool on);

	// Is this analog button down
	inline bool ButtonDown(const XINPUT_GAMEPAD &gamepad, int button)
		{ return(gamepad.bAnalogButtons[button] > 64); }

	// Get the current time
	inline uint Now(void)
		{ return(g_timer.GetOSTime()); }

#ifdef _DEBUG
	// Debug a button
	void DebugButton(char *name, int index);
#endif

	bool m_headSetInserted;
	bool m_microPhoneInserted;
};

/***************************************************************************/
/***************************************************************************/
inline void XboxControllerInputDriver::TranslateAxis(
AnalogAxis axis,
SHORT value,
bool flip)
{
	int v = 128 + (value / 256);
	if (flip)
		v = 255 - value;
	ASSERT(v >= 0 && v < TRANSFER_LOOKUPS);
	stick[axis] = transferLookup[axis][v];
}

/***************************************************************************/
/***************************************************************************/
inline void XboxControllerInputDriver::SetState(
ControllerButton button,
bool on)
{
	state[button].last = state[button].now;
	state[button].now = on;
}

#endif
