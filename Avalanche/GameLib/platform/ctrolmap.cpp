/***************************************************************************/
// Control Mapper
//
// This class maps the joystick input into "game" controls.  This layer of
// inderection has two purposes:
//
// - To make the remapping of the controller trivial
// - To provide both analog and digital directional control transparently
/***************************************************************************/
#include "platform/PlatformPCH.h"
#include "EngineHelper/timer.h"

/***************************************************************************/
// Initlaize the controller
/***************************************************************************/
ControlMapper::ControlMapper(
const char *name)
{
	enabled = true;
	
	ASSERT(strlen(name) < sizeof(this->name) - 1);
	strcpy(this->name, name);

	// Weeeee
	device = NULL;
	ClearFocus();

	// Init all the mapping stuff
	for (int a = 0; a < AA_AXES; a++)
		SetAxis(a, AA_INVALID);
	for (int b = 0; b < CB_BUTTONS; b++)
		SetButton(b, CB_INVALID);

	// Nothing to see here
	ClearSimState();

	// This is off by default
	stick1ToDigital = false;
}

/***************************************************************************/
// Enable/disable control reading
/***************************************************************************/
void ControlMapper::Enable(
bool enable)
{
	if (enable == enabled)
		return;
	enabled = enable;
	
	if (!enabled)
		ClearVibration();
}

/***************************************************************************/
/***************************************************************************/
void ControlMapper::Update()
{
	// Allow this to update
	AdvanceSimState();
	UpdateStick1ToDigital();
}

/***************************************************************************/
// Eat the current button presses
/***************************************************************************/
void ControlMapper::EatButtonPresses()
{
	if (HasFocus() && enabled)
		device->EatButtonPresses();
	AdvanceSimState();
}

/***************************************************************************/
// Eat the current button presses
/***************************************************************************/
void ControlMapper::EnableVibration( bool enabled )
{
	// TODO: whatever is necessary to enable/disable vibration
}

/***************************************************************************/
// Activate the vibration/forcefeedback function of the controller
// Intensity is specified VIBRATEINTENSITY
// pan is specified using VIBRATEPAN.
// Returns true/false based on controller's ability to vibrate
/***************************************************************************/
bool ControlMapper::SetVibration(
uint intensity, 
ControllerInputDriver::VIBRATEPAN pan)
{
	if (!HasFocus() || !enabled)
		return false;
	return device->SetVibration(intensity, pan);
}

/***************************************************************************/
// Do the vibration for duration milliseconds, with the above intensity/pan
// values.
/***************************************************************************/
bool ControlMapper::SetVibrationDuration(
uint duration, 
uint intensity, 
ControllerInputDriver::VIBRATEPAN pan)
{
	if (!HasFocus() || !enabled)
		return false;
	return device->SetVibrationDuration(duration, intensity, pan);
}

/***************************************************************************/
// Make it so there is no vibration (force forces it to end even timed vibrations)
/***************************************************************************/
bool ControlMapper::ClearVibration(
bool force)
{
	if (!HasFocus() || !enabled)
		return false;
	return device->ClearVibration(force);
}

/***************************************************************************/
// Get the current vibration values. Returns false (and doesn't touch
// intensity if there is no vibration
/***************************************************************************/
bool ControlMapper::GetVibration(
uint &intensity)
{
	if (!HasFocus() || !enabled)
		return false;
	return device->GetVibration(intensity);
}

/***************************************************************************/
// Get button mapping
/***************************************************************************/
ControllerButton ControlMapper::GetButton( int button )
{
	ASSERT(button >= 0 && button < CB_BUTTONS);

	return buttonMap[button];
}

/***************************************************************************/
// Setup the mapping
/***************************************************************************/
void ControlMapper::SetAxis(
int axis,
AnalogAxis controllerAxis,
bool invert)
{
	ASSERT(axis >= 0 && axis < AA_AXES);

	axisMap[axis] = controllerAxis;
	InvertAxis(axis, invert);
}

/***************************************************************************/
/***************************************************************************/
void ControlMapper::InvertAxis(
int axis,
bool invertOn)
{
	ASSERT(axis >= 0 && axis < AA_AXES);

	invert[axis] = invertOn ? -1.0f : 1.0f;
}

/***************************************************************************/
/***************************************************************************/
bool ControlMapper::AxisInverted(
int axis)
{
	ASSERT(axis >= 0 && axis < AA_AXES);

	return (invert[axis] < 0.0f);
}

/***************************************************************************/
// Setup the mapping
/***************************************************************************/
void ControlMapper::SetButton(
int button,
ControllerButton controllerButton)
{
	ASSERT(button >= 0 && button < CB_BUTTONS);

	buttonMap[button] = controllerButton;
}

/***************************************************************************/
// Get the [-1..1] value of a stick
/***************************************************************************/
float ControlMapper::GetAxis(
int axis)
{
	ASSERT(axis >= 0 && axis < AA_AXES);

	// No device, no value
	if (!device || axisMap[axis] == AA_INVALID || !HasFocus() || !enabled)
		return 0.0f;

	// Give them back the raw data
	return device->StickPosition(axisMap[axis]) * invert[axis];
}

/***************************************************************************/
// Was this button just pressed last frame?
/***************************************************************************/
bool ControlMapper::ButtonPressed(
int button)
{
	ASSERT(button >= 0 && button < CB_BUTTONS);

	if (!device || buttonMap[button] == CB_INVALID || !HasFocus() || !enabled)
		return false;

	ControllerButton map = buttonMap[button];
	return (device->ButtonPressed(map) || SimButtonPressed(map));
}

/***************************************************************************/
// Was this button just released last frame?
/***************************************************************************/
bool ControlMapper::ButtonReleased(
int button)
{
	ASSERT(button >= 0 && button < CB_BUTTONS);

	if (!device || buttonMap[button] == CB_INVALID || !HasFocus() || !enabled)
		return false;

	ControllerButton map = buttonMap[button];
	return (device->ButtonReleased(map) || SimButtonReleased(map));
}

/***************************************************************************/
// Is this button currenly down?
/***************************************************************************/
bool ControlMapper::ButtonDown(
int button)
{
	ASSERT(button >= 0 && button < CB_BUTTONS);

	if (!device || buttonMap[button] == CB_INVALID || !HasFocus() || !enabled)
		return false;

	ControllerButton map = buttonMap[button];
	return (device->ButtonDown(map) || SimButtonDown(map));
}

/***************************************************************************/
/***************************************************************************/
void ControlMapper::ClearSimState()
{
	for (int i = 0; i < CB_BUTTONS; i++)
	{
		simState[i].last = false;
		simState[i].now = false;
	}
}

/***************************************************************************/
/***************************************************************************/
void ControlMapper::AdvanceSimState()
{
	for (int i = 0; i < CB_BUTTONS; i++)
		simState[i].last = simState[i].now;
}

/***************************************************************************/
/***************************************************************************/
void ControlMapper::Stick1ToDigital(
bool enable,
float threshold,
int repeatMS)
{
	stick1ToDigital = enable;
	stick1ToDigitalThreshold = threshold;
	strobeRate = repeatMS;
	nextStrobe[0] = nextStrobe[1] = nextStrobe[2] = nextStrobe[3] = 0;
}

/***************************************************************************/
/***************************************************************************/
bool ControlMapper::Stick1ToDigital()
{
	return stick1ToDigital;
}

/***************************************************************************/
/***************************************************************************/
void ControlMapper::UpdateStick1ToDigital()
{
	// Do we have to do any work?
	if (!stick1ToDigital || !HasFocus())
	{
		simState[CB_RIGHT].now = false;
		simState[CB_LEFT].now = false;
		simState[CB_UP].now = false;
		simState[CB_DOWN].now = false;
		return;
	}

	// What is up
	float stickX = 0.0f;
	float stickY = 0.0f;
	if ( device != NULL )
	{
		stickX = device->StickPosition(AA_X1);
		stickY = device->StickPosition(AA_Y1);
	}

	// Fill out the states
	simState[CB_RIGHT].now = (stickX > stick1ToDigitalThreshold);
	simState[CB_LEFT].now = (stickX< -stick1ToDigitalThreshold);
	simState[CB_UP].now = (stickY > stick1ToDigitalThreshold);
	simState[CB_DOWN].now = (stickY < -stick1ToDigitalThreshold);

	// Apply a "strobe" to that the analog stick repeats
	HandleStrobe(CB_LEFT, stickX);
	HandleStrobe(CB_RIGHT, stickX);
	HandleStrobe(CB_UP, stickY);
	HandleStrobe(CB_DOWN, stickY);
}

/***************************************************************************/
/***************************************************************************/
void ControlMapper::HandleStrobe(
ControllerButton button,
float stick)
{
	// what is the stobe index (bit of a hack)
	int strobeIndex = button - CB_LEFT;

	// now
	uint now = g_timer.GetOSTime();

	// Setup the strobe
	if (SimButtonReleased(button))
		nextStrobe[strobeIndex] = 0;
	else if (SimButtonPressed(button))
	{
		// Figure out the delay
		float factor = Math::LinearMap(Math::Abs(stick), stick1ToDigitalThreshold, 1.0f, 1.0f, 0.5f);

		// --- DAS, this lame casting is so PS2 does not convert to double precision before converting to a uint
		uint delay = (uint)((int)((float)strobeRate * factor));

		// Yo!
		nextStrobe[strobeIndex] = now + delay;
	}

	// Apply the strobe
	if (nextStrobe[strobeIndex] && now >= nextStrobe[strobeIndex])
	{
		simState[button].now = false;
		nextStrobe[strobeIndex] = 0;
	}
}

/***************************************************************************/
/***************************************************************************/
void ControlMapper::SetFocus()
{	
	focus = true; 
}

/***************************************************************************/
/***************************************************************************/
void ControlMapper::ClearFocus()
{ 
	focus = false; 
	if (device)
		device->ClearVibration(true);
}

