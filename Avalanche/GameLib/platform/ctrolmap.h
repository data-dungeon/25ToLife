/***************************************************************************/
// Control Mapper
//
// This class maps the joystick input into "game" controls.  This layer of
// inderection has two purposes:
//
// - To make the remapping of the controller trivial
// - To provide both analog and digital directional control transparently
/***************************************************************************/
#ifndef CTROLMAP_H
#define CTROLMAP_H

#include "platform/ctroller.h"
#include "platform/systemi.h"
#include "container/dllist.h"

class ControllerFocusManager;

class ControlMapper
{
public:
	// Initlaize the controller
	ControlMapper(const char *name);
	virtual ~ControlMapper() {}

	// Give back the name
	const char *Name()
		{ return name; }

	// Enable/disable control reading
	void Enable(bool enable);
	bool Enabled()
		{ return enabled; }

	// Get the device
	ControllerInputDriver *GetDevice()
		{ return device; }

	// Update controller states (call once per frame)
	virtual void Update();

	// Eat the current button presses
	void EatButtonPresses();

	// Enable/disable vibration
	void EnableVibration( bool enabled = true );

	// Activate the vibration/forcefeedback function of the controller
	// Intensity is specified VIBRATEINTENSITY
	// pan is specified using VIBRATEPAN.
	// Returns true/false based on controller's ability to vibrate
	bool SetVibration(uint intensity, ControllerInputDriver::VIBRATEPAN pan = ControllerInputDriver::CENTER);

	// Do the vibration for duration milliseconds, with the above intensity/pan
	// values.
	bool SetVibrationDuration(uint duration, uint intensity, ControllerInputDriver::VIBRATEPAN pan = ControllerInputDriver::CENTER);

	// Make it so there is no vibration (force forces it to end even timed vibrations)
	bool ClearVibration(bool force = false);

	// Get the current vibration values. Returns false (and doesn't touch
	// intensity if there is no vibration
	bool GetVibration(uint &intensity);

	// Do we have focus
	bool HasFocus()
		{ return focus; }

	// Get the mapping
	ControllerButton GetButton( int button );

protected:
	// the device
	ControllerInputDriver 	*device;
	bool							enabled;

	// Setup the mapping
	void SetAxis(int axis, AnalogAxis controllerAxis, bool invert = false);
	void InvertAxis(int axis, bool invertoOn);
	bool AxisInverted(int axis);
	void SetButton(int button, ControllerButton controllerButton);

	// Get the [-1..1] value of a stick
	float GetAxis(int axis);

	// Was this button just pressed last frame?
	bool ButtonPressed(int button);

	// Was this button just released last frame?
	bool ButtonReleased(int button);

	// Is this button currenly down?
	bool ButtonDown(int button);

	// The mapping scheme
	AnalogAxis					axisMap[AA_AXES];
	float							invert[AA_AXES];

	// The array of mapped buttons
	ControllerButton			buttonMap[CB_BUTTONS];

	// Simulated buttons
	struct SimButtonState
	{
		bool last, now;
	};
	SimButtonState				simState[CB_BUTTONS];
	void ClearSimState();
	void AdvanceSimState();

	inline bool SimButtonPressed(ControllerButton button)
		{ return (simState[button].last == false && simState[button].now == true); }
	inline bool SimButtonReleased(ControllerButton button)
		{ return (simState[button].last == true && simState[button].now == false); }
	inline bool SimButtonDown(ControllerButton button)
		{ return (simState[button].now); }

	// Some useful translation stuff for analog to digital
	void Stick1ToDigital(bool enable, float threshold = 0.4f, int repeatMS = 350);
	bool Stick1ToDigital();

	// Use the analog stick to simulate digital presses
	bool							stick1ToDigital;
	float							stick1ToDigitalThreshold;
	uint							strobeRate, nextStrobe[4];
	void							UpdateStick1ToDigital();
	void							HandleStrobe(ControllerButton button, float stick);

private:
	// This stuff is intentionally private
	char							name[32];			// The name
	bool							focus;				// Focus management

	// Set a device into this mapper
	void BindDevice(ControllerInputDriver *inputDriver)
		{ device = inputDriver; }
	bool HasDevice()
		{ return (device != NULL); }
	
	// Allow focus
	void SetFocus();
	void ClearFocus();

	friend class ControllerFocusManager;

public:
	// We appear in a list
	ControlMapper *prev, *next;
};

#endif
