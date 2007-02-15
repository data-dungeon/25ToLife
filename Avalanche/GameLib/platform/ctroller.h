/***************************************************************************/
// Controller driver
/***************************************************************************/
#ifndef CTROLLER_H
#define CTROLLER_H

#include "platform/BaseType.h"

// Based
enum ControllerButton
{
	// CB_INVALID needs to be first in the enum, since this is going to be
	// the default for any unspecified fields that use ControllerButtons
	CB_INVALID,
	CB_LEFT, CB_DOWN,	CB_RIGHT, CB_UP,
	CB_SQUARE, CB_X, CB_O, CB_TRI,
	CB_L1,               	// This is the white XBox button
	CB_R1,						// This is the black XBox button
	CB_L2,
	CB_R2,
	CB_START, CB_SELECT,
	CB_STICK1, CB_STICK2,
	CB_Z,                           // the Z button on the gamecube.  this is necessary because with the current mappings, there is
									// no way to trigger anything on the Z button, since it acts as a shift turning the right
									// triggers into L2 and R2 instead of L1 and R1.
#ifdef DIRECTX_PC
	CB_ESCAPE,						// we need unique escape button functionality.
#endif
	CB_BUTTONS
};

enum AnalogAxis
{
	// AA_INVALID needs to be first in the enum, since this is going to be
	// the default for any unspecified fields that use ControllerButtons
	AA_INVALID,
	AA_X1, AA_Y1,

	AA_X2, AA_Y2,

#ifdef WIN32
	// These are for debugging purposes on PC
	AA_THROTTLE,
	AA_RUDDER,
#endif

	// The buttons can be read in analog mode too
	AA_LEFT,	AA_RIGHT, AA_UP, AA_DOWN,
	AA_TRI, AA_O, AA_X, AA_SQUARE,
	AA_L1, AA_R1, AA_L2, AA_R2,

	AA_AXES
};

// Transfer function for mapping stick to output
enum AxesTransferFunction
{
	ATF_LINEAR,
	ATF_PARABOLA,
	ATF_EXPONENTIAL,
	ATF_POWER
};


class ControllerInputDriver
{
public:
	ControllerInputDriver();
	virtual ~ControllerInputDriver() {}

	// What is the device name of this controller, you have to worry about
	// foriegn versions of this
	virtual const char *DeviceName() = 0;

	// Setup a stick axis
	virtual bool SetupAxis(AnalogAxis axis,
									AxesTransferFunction transferFunction,
									float factor,
									float deadZonePercent,
									float clampZonePercent) = 0;

	// Is the controller currently connected?
	virtual bool Connected() = 0;

	// Was this button just pressed last frame?
	inline bool ButtonPressed(ControllerButton button)
		{ return(state[button].last == false && state[button].now == true); }

	// Was this button just released last frame?
	inline bool ButtonReleased(ControllerButton button)
		{ return(state[button].last == true && state[button].now == false); }

	// Is this button currenly down?
	inline bool ButtonDown(ControllerButton button)
		{ return(state[button].now); }

	// Sticks return from -1.0 to 1.0
	inline float StickPosition(AnalogAxis axis)
		{ return(stick[axis]); }

	// Reset all button pressed states
	void ClearPressed();

	// force an update of current state into previous
	void UpdatePressed();
	void EatButtonPresses()
		{ UpdatePressed(); }

	// Queries the current state of the button hardware. Does not affect
	// ButtonPressed, ButtonReleased, etc.
	virtual bool ButtonPeek(ControllerButton button);

	// Was any button pressed?
	bool AnyButtonPressed();

	// Was reset pressed?
	// Returns true if L2+R2+START+SELECT are currently down
	// This does not check the "system" reset state
	bool SoftResetPoll();

	// Determine if player is moving stick or pressing buttons
	bool ControlActivity(float analogThreshold = 0.01f);

	// Turns vibration on/off globally.
	void Vibration(bool allow);

		// Turns vibration on/off globally.
	bool GetVibrationFlag() { return (allowVibration); };

	virtual u8 getKey( void ) { return 0; }
	virtual bool isShiftPressed( void ) { return false; }
	virtual bool shiftReleased( void ) { return false; }
	virtual bool isCapsLocked( void ) { return false; }
	virtual bool capsUnLocked( void ) { return false; }
	virtual bool isUpArrow( void ) { return false; }
	virtual bool isDownArrow( void ) { return false; }
	virtual bool isLeftArrow( void ) { return false; }
	virtual bool isRightArrow( void ) { return false; }

	// Activate the vibration/forcefeedback function of the controller
	// Intensity is specified VIBRATEINTENSITY
	// pan is specified using VIBRATEPAN.
	// Returns true/false based on controller's ability to vibrate
	enum VIBRATEINTENSITY
	{
		// Rumble goes from 0 - 200
		NOVIBRATION = 0,
		RUMBLELOW =	80,
		RUMBLEMEDIUM = 150,
		RUMBLEHIGH = 200,

		// Buzz can be or'd in
		BUZZ = 256,

		// Everything!
		FULLVIBRATION = (RUMBLEHIGH | BUZZ)
	};
	enum VIBRATEPAN
	{
		CENTER,
		LEFT,
		RIGHT,
	};

	enum HeadSetStatus
	{
		REMOVED = 0,
		INSERTED,
		UNINITIALIZED
	};

	virtual bool SetVibration(uint intensity, VIBRATEPAN pan = CENTER);

	// Do the vibration for duration milliseconds, with the above intensity/pan
	// values.
	virtual bool SetVibrationDuration(uint duration, uint intensity, VIBRATEPAN pan = CENTER);

	// Make it so there is no vibration (force forces it to end even timed vibrations)
	virtual bool ClearVibration(bool force = false);

	// Get the current vibration values. Returns false (and doesn't touch
	// intensity if there is no vibration
	virtual bool GetVibration(uint &intensity);

	// The input system calls this to "refresh" input at frame start
	virtual void BeginInput();

	// The input system calls this to "flush" input at frame end
	virtual void DoneInput() {}

	// Called when the controller is gaining/losing focus
	virtual void Activate(bool active) {}

	// Should we observe focus changes & call Activate on focus change?
	virtual bool ObserveFocus() { return false; }

#ifdef DIRECTX_PC
	// boolean function to test for first person mode
	bool InFirstPerson() { return inFirstPerson; }
	bool IsCenteringView() { return centerView; }
#endif

	// --- return whether headset is REMOVED or INSERTED
	inline HeadSetStatus GetHeadSetStatus( ) { return m_headSetStatus; }
	inline int GetHeadSetPort( ) { return m_headSetPort; }

protected:
	// Track the button states
	struct BUTTONSTATE
	{
		bool		last;			// state at last sample
		bool		now;			// state now
	};
	BUTTONSTATE	state[CB_BUTTONS];

	// Set the state of a button
	inline void SetState(ControllerButton button, bool on)
		{ state[button].now = on; }

	// Joystick stuff
	float 	stick[AA_AXES];

	// Utility to generate a transfer function
	// deadZonePercent indicates the percentage throw of the stick that returns 0
	// clampZonePercent indicates percentage at the limits fof throw that reutrn 1.0
	bool GenerateTransferFunction(float *transferLookup,
											uint entries,
											AxesTransferFunction transferFunction = ATF_LINEAR,
											float factor = 1.0f,
											float deadZonePercent = 5.0f,
											float clampZonePercent = 2.0f);

	// Are we allowing vibration?
	bool allowVibration;

#ifdef DIRECTX_PC
	// Are we in first person?
	bool inFirstPerson;
	// are we centering our view?
	bool centerView;
#endif

	// --- Headset support
	inline void SetHeadSetStatus( HeadSetStatus status ) { m_headSetStatus = status; }
	inline void SetHeadSetPort( int port ) { m_headSetPort = port; }

	HeadSetStatus m_headSetStatus;
	int m_headSetPort;
};

#endif
