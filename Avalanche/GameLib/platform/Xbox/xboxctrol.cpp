/***************************************************************************/
// Xbox controller input
/***************************************************************************/
#include "platform/PlatformPCH.h"

#include "Layers/Debug.h"
#include "platform/Xbox/xboxctrol.h"
#include "platform/Xbox/XboxLive.h"
#include <stdio.h>
#include "Math/MathClass.h"

/***************************************************************************/
// setup everthing
/***************************************************************************/
XboxControllerInputDriver::XboxControllerInputDriver(
DWORD port)
{
	// Initialize this thingie
	hDevice = NULL;
	this->port = port;
	CreateDevice();

	// setup a default transfer function
	for (int a = 0; a < AA_AXES; a++)
		SetupAxis((AnalogAxis)a, ATF_LINEAR, 1, 32, 4);

	// Setup the motor stuff
#ifdef _DEBUG
	allowVibration = false;
#else
	allowVibration = true;
#endif
	vibrationCutoff = Now() + 5000;

	// Dummy up the feedback stuff
	memset(&feedback, '\0', sizeof(XINPUT_FEEDBACK));

	m_headSetInserted = false;
	m_microPhoneInserted = false;
}

/***************************************************************************/
/***************************************************************************/
XboxControllerInputDriver::~XboxControllerInputDriver(void)
{
	DestroyDevice();
}

/***************************************************************************/
// Setup a stick axis
/***************************************************************************/
bool XboxControllerInputDriver::SetupAxis(
AnalogAxis axis,
AxesTransferFunction transferFunction,
float factor,
float deadZonePercent,
float clampZonePercent)
{
	// Call the utility
	GenerateTransferFunction(transferLookup[axis], TRANSFER_LOOKUPS,
										transferFunction, factor,
										deadZonePercent, clampZonePercent);
	return(true);
}


/***************************************************************************/
// Is the controller currently connected?
/***************************************************************************/
bool XboxControllerInputDriver::Connected(void)
{
	if (hDevice)
		return(true);	
	else
		return(false);
}

/***************************************************************************/
// Activate the vibration/forcefeedback function of the controller
// Returns true/false based on controller's ability to vibrate
/***************************************************************************/
void XboxControllerInputDriver::Vibration(
bool allow)
{
	allowVibration = allow;
}

/***************************************************************************/
// Activate the vibration/forcefeedback function of the controller
// Returns true/false based on controller's ability to vibrate
/***************************************************************************/
bool XboxControllerInputDriver::SetVibration(
uint intensity,
VIBRATEPAN pan)
{
	// This is bad
	if (!hDevice)
		return(false);

	// If we're globally preventing vibration, bail.
	if (!allowVibration)
		return(false);

	// Are we already in a timed vibration?
	if (vibrationCutoff != 0)
		return(false);

	// Setup the vibration structure
	XINPUT_RUMBLE &rumble = feedback.Rumble;

	// Clamp the input intensity
	int clampedIntensity = Math::Clamp(intensity & (BUZZ - 1), 0, RUMBLEHIGH);

	// Special threshold, just don't bother
	if (clampedIntensity < RUMBLELOW)
		clampedIntensity = 0;

	// Scale into th
	int i = (clampedIntensity * 65535) / 200;

	// Divide by two, since the Xbox controller throbs like a donkey
	i /= 2;

	switch (pan)
	{
		case LEFT:
			rumble.wLeftMotorSpeed = i;
			rumble.wRightMotorSpeed = 0;
			break;
		case CENTER:
			rumble.wLeftMotorSpeed = i;
			rumble.wRightMotorSpeed = i;
			break;
		case RIGHT:
			rumble.wLeftMotorSpeed = 0;
			rumble.wRightMotorSpeed = i;
			break;
	}

#if 1
	// Force the high intesity on!
	if (intensity & BUZZ)
		rumble.wRightMotorSpeed = 65535;
#endif

	// Tell the device
	XInputSetState(hDevice, &feedback);
	return(true);
}

/***************************************************************************/
// Do the vibration for duration milliseconds, with the above intensity/pan
// values.
/***************************************************************************/
bool XboxControllerInputDriver::SetVibrationDuration(
uint duration,
uint intensity,
VIBRATEPAN pan)
{
	// set the vibration going
	if (!SetVibration(intensity, pan))
		return(false);

	// Setup the cutoff time (duration is approximate, the actuators have spindown time)
	vibrationCutoff = Now() + (duration >> 1);
	return(true);
}

/***************************************************************************/
// Make it so there is no vibration
/***************************************************************************/
bool XboxControllerInputDriver::ClearVibration(
bool force)
{
	// Force it off
	if (force)
		vibrationCutoff = 0;

	// Cut it off
	return(SetVibration(NOVIBRATION));
}

/***************************************************************************/
// Notify of controller changes
/***************************************************************************/
void XboxControllerInputDriver::NotifyControlChanges(
bool inserted,
bool removed,
int newPort)
{
	// Were we removed?
	if (removed || port != newPort)
	{
		DestroyDevice();
		port = newPort;
	}

	// Were we inserted?
	if (inserted && hDevice == NULL)
	{
		CreateDevice();

		// --- look to initialize the headset port with a connected controller to start
		if( GetHeadSetPort() < 0 )
			SetHeadSetPort( port );
	}
}

/***************************************************************************/
/***************************************************************************/
void XboxControllerInputDriver::NotifyHeadSetChange( int curPort, bool micInserted, bool micRemoved, bool headInserted, bool headRemoved )
{
	if( micInserted || micRemoved || headInserted || headRemoved )
	{
		if( micInserted ) m_microPhoneInserted = true;
		if( micRemoved ) m_microPhoneInserted = false;
		if( headInserted ) m_headSetInserted = true;
		if( headRemoved ) m_headSetInserted = false;

		if( ( g_input.GetLockedControllerIndex(0) == CONTROLLER_NOT_LOCKED && g_input.GetFirstUnlockedControllerIndex() == curPort ) ||
			 ( g_input.GetLockedControllerIndex(0) == curPort ) )
		{
			if( m_headSetInserted && m_microPhoneInserted )
			{
				TRACE( "Communicator inserted" );
				SetHeadSetStatus( HeadSetStatus::INSERTED );
				SetHeadSetPort( curPort );
			}
			if( !m_headSetInserted && !m_microPhoneInserted )
			{
				TRACE( "Communicator removed" );
				SetHeadSetStatus( HeadSetStatus::REMOVED );
			}
		}
	}
}

#ifdef _DEBUG
/***************************************************************************/
/***************************************************************************/
void XboxControllerInputDriver::DebugButton(
char *name,
int index)
{
	TRACE(name);

	if (state[index].last && state[index].now)
		TRACE(" down");
	else if (!state[index].last && state[index].now)
		TRACE(" pressed");
	else if (state[index].last && !state[index].now)
		TRACE(" released");
	else if (!state[index].last && !state[index].now)
		TRACE(" up");

	TRACE("\n");
}
#endif

/***************************************************************************/
// The input system calls this to "refresh" input at frame start
/***************************************************************************/
void XboxControllerInputDriver::BeginInput(int i)
{
	// Do we have a device?
	if (!hDevice)
		return;

	// Poll the device
	XInputPoll(hDevice);

	// Get the state
	XINPUT_STATE inputState;
	if (XInputGetState(hDevice, &inputState) != ERROR_SUCCESS)
	{
		return;
	}
	

	// Translate the axes
	XINPUT_GAMEPAD &gamepad = inputState.Gamepad;

	// Setup the button states
	SetState(CB_LEFT, (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0);
	SetState(CB_DOWN, (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0);
	SetState(CB_RIGHT, (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0);
	SetState(CB_UP, (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0);

	SetState(CB_START, (gamepad.wButtons & XINPUT_GAMEPAD_START) != 0);
	SetState(CB_SELECT, (gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0);

	SetState(CB_STICK1, (gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0);
	SetState(CB_STICK2, (gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0);

	SetState(CB_SQUARE, ButtonDown(gamepad, XINPUT_GAMEPAD_X));
	SetState(CB_X, ButtonDown(gamepad, XINPUT_GAMEPAD_A));
	SetState(CB_O, ButtonDown(gamepad, XINPUT_GAMEPAD_B));
	SetState(CB_TRI, ButtonDown(gamepad, XINPUT_GAMEPAD_Y));

	SetState(CB_R1, ButtonDown(gamepad, XINPUT_GAMEPAD_RIGHT_TRIGGER));
	SetState(CB_L1, ButtonDown(gamepad, XINPUT_GAMEPAD_LEFT_TRIGGER));
	SetState(CB_R2, ButtonDown(gamepad, XINPUT_GAMEPAD_BLACK));
	SetState(CB_L2, ButtonDown(gamepad, XINPUT_GAMEPAD_WHITE));

	TranslateAxis(AA_X1, gamepad.sThumbLX);
	TranslateAxis(AA_Y1, gamepad.sThumbLY);
	TranslateAxis(AA_X2, gamepad.sThumbRX);
	TranslateAxis(AA_Y2, gamepad.sThumbRY);
}

/***************************************************************************/
// What is the device name of this controller, you have to worry about
// foriegn versions of this	
/***************************************************************************/
const char *XboxControllerInputDriver::DeviceName()
{
	return deviceName;
}

/***************************************************************************/
// The input system calls this to "flush" input at frame end
/***************************************************************************/
void XboxControllerInputDriver::DoneInput(void)
{
	// cutoff the motor?
	if (vibrationCutoff != 0 && vibrationCutoff <= Now())
	{
		ClearVibration(true);
		vibrationCutoff = 0;
	}
}

/***************************************************************************/
// Internal for focus changes
/***************************************************************************/
void XboxControllerInputDriver::Activate(
bool active)
{
	// This should go away, it's a DInput relic
}

/***************************************************************************/
// Create the device
/***************************************************************************/
bool XboxControllerInputDriver::CreateDevice(void)
{
	// Make it!
	ASSERT(hDevice == NULL);
	hDevice = XInputOpen(XDEVICE_TYPE_GAMEPAD, port, XDEVICE_NO_SLOT, NULL);
	if (!hDevice)
	{
		DWORD ohFuck = GetLastError();
	}

	// Did we get it?
	return(hDevice != NULL);
}

/***************************************************************************/
// destroy the device
/***************************************************************************/
void XboxControllerInputDriver::DestroyDevice(void)
{
	if (hDevice)
	{
		XInputClose(hDevice);
		hDevice = NULL;
	}
}

