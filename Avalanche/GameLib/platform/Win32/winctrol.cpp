/***************************************************************************/
// Windows controller input
/***************************************************************************/
#include "platform/PlatformPCH.h"

// Where the button map is stored
#ifdef DIRECTX
char *WindowsControllerInputDriver::buttonMapFile[2] =
{
	".\\data_dx\\buttmap\\buttmap.ini",
	".\\data_dx\\buttmap\\buttmap2.ini"
};
#else
char *WindowsControllerInputDriver::buttonMapFile[2] =
{
	".\\data_pc\\buttmap\\buttmap.ini",
	".\\data_pc\\buttmap\\buttmap2.ini"
};
#endif

/***************************************************************************/
// setup everthing
/***************************************************************************/
WindowsControllerInputDriver::WindowsControllerInputDriver(
HWND hWnd,
LPDIRECTINPUTDEVICE joystickDevice,
int deviceNumber)
{
	// save the device
	device = joystickDevice;

	// Create the secondary device for the joystick
	device2 = NULL;
	device->QueryInterface(IID_IDirectInputDevice2, (LPVOID *)&device2);
	ASSERT(device2); // assume this

	// Call the setup (can't use the return value here)
	bool ret = SetupDevice(hWnd);
	ASSERT(ret);

	// setup the button map & transfer functions
	LoadProfile(deviceNumber);

	// setup a default transfer function
	for (int a = 0; a < AA_AXES; a++)
		GenerateTransferFunction(transferLookup[a], TRANSFER_LOOKUPS,
											initTransferFunction[a],
											initFactor[a],
											initDeadZone[a],
											initClampZone[a]);
}

/***************************************************************************/
/***************************************************************************/
WindowsControllerInputDriver::~WindowsControllerInputDriver()
{
	// Release our devices
	if (device)
	{
		device->Unacquire();
		device->Release();
	}

	if (device2)
		device2->Release();
}

/***************************************************************************/
// What is the device name of this controller, you have to worry about
// foriegn versions of this	
/***************************************************************************/
const char *WindowsControllerInputDriver::DeviceName()
{
	return deviceName;
}

/***************************************************************************/
// Setup a stick axis
/***************************************************************************/
bool WindowsControllerInputDriver::SetupAxis(
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
// Are we connected
/***************************************************************************/
bool WindowsControllerInputDriver::Connected()
{
	return(true);
}

/***************************************************************************/
// The input system calls this to "refresh" input at frame start
/***************************************************************************/
void WindowsControllerInputDriver::BeginInput()
{
	// Can't work without a device
	ASSERT(device2);

	// Get the state
	DIJOYSTATE joyState;
	memset(&joyState, '\0', sizeof(joyState));
	HRESULT ret;
	do {
		// Poll the device (ignore return)
		ret = device2->Poll();

		// Get the state
		ret = device->GetDeviceState(sizeof(joyState), &joyState);
		if (ret == DIERR_INPUTLOST)
			device->Acquire();
		}
	while (ret == DIERR_INPUTLOST);

	// This is bad
	if (ret != DI_OK)
		return;

	// Call the parent
	ControllerInputDriver::BeginInput();

	// set the axes
	int axis[8];
	axis[0] = joyState.lX;
	axis[1] = joyState.lY;
	axis[2] = joyState.lZ;
	axis[3] = joyState.lRx;
	axis[4] = joyState.lRy;
	axis[5] = joyState.lRz;
	axis[6] = joyState.rglSlider[0];
	axis[7] = joyState.rglSlider[1];

	// Now map them into the controller
	for (int a = 0; a < AA_AXES; a++)
	{
		if (axisMap[a] >= 0)
		{
			int value;
			if (axisValid[axisMap[a]])
				value = axis[axisMap[a]];
			else
				value = 0;

			ASSERT(value >= 0 && value < TRANSFER_LOOKUPS);

			if (invert[a])
				stick[a] = transferLookup[a][TRANSFER_LOOKUPS - 1 - value];
			else
				stick[a] = transferLookup[a][value];
		}
	}

	// fake buttons with the pov hat
	bool povHat[4];
	if (LOWORD(joyState.rgdwPOV[0]) == 0xffff)
	{
		povHat[0] = povHat[1] = povHat[2] = povHat[3] = false;
	}
	else
	{
		int pos = joyState.rgdwPOV[0] / 100;
		povHat[0] = (pos > 300 || pos < 60);
		povHat[1] = (pos > 30 && pos < 150);
		povHat[2] = (pos > 120 && pos < 240);
		povHat[3] = (pos > 210 && pos < 330);
	}

	// Interpret the buttons
	for (int b = 0; b < CB_BUTTONS; b++)
	{
		// Button down?
		if (buttonMap[b] >= 0)
		{
			if (buttonMap[b] < 100)
				SetState((ControllerButton)b, (joyState.rgbButtons[buttonMap[b]] & 0x80) != 0);
			else
				SetState((ControllerButton)b, povHat[buttonMap[b] - 100]);
		}
	}

//#define STICKDEBUG
#if defined(STICKDEBUG) && defined(_DEBUG)
	static int debug = 0;
	if (debug++ % 10 != 0)
		return;

	char temp[256];

	// Dump the axes
	strcpy(temp, "Axis: ");
	for (a = 0; a < 8; a++)
	{
		if (!axisValid[a])
			strcat(temp, "X");
		else
			itoa(axis[a], temp + strlen(temp), 10);

		if (a != 7)
			strcat(temp, ", ");
	}
	g_console.Echo(temp);

	// Dump the buttons
	strcpy(temp, "Butt: ");
	for (b = 0; b < 32; b++)
	{
		if ((joyState.rgbButtons[b] & 0x80) != 0)
			strcat(temp, "X");
		else
			strcat(temp, "_");
		if (((b + 1) % 4) == 0)
			strcat(temp, " ");
	}
	strcat(temp, " POV: ");
	strcat(temp, povHat[0] ? "X " : "_ ");
	strcat(temp, povHat[1] ? "X " : "_ ");
	strcat(temp, povHat[2] ? "X " : "_ ");
	strcat(temp, povHat[3] ? "X " : "_ ");
	g_console.Echo(temp);
#endif
}

/***************************************************************************/
// The input system calls this to "flush" input at frame end
/***************************************************************************/
void WindowsControllerInputDriver::DoneInput()
{
	// Nothing to see here, move along....
}

/***************************************************************************/
// observe focus changes?
/***************************************************************************/
bool WindowsControllerInputDriver::ObserveFocus()
{
	return observeFocus;
}

/***************************************************************************/
// Internal for focus changes
/***************************************************************************/
void WindowsControllerInputDriver::Activate(
bool active)
{
	// Do we have a device?
	ASSERT(device);

	// Grab
	if (active)
		device->Acquire();
	else
		device->Unacquire();
}

/***************************************************************************/
// Setup all the direct input stuff
/***************************************************************************/
bool WindowsControllerInputDriver::SetupDevice(
HWND hWnd)
{
	// Duh
	ASSERT(device);

	// Set the data format to "simple joystick" - a predefined data format
	HRESULT ret = device->SetDataFormat(&c_dfDIJoystick);
	if (ret != DI_OK)
		return(false);

	// Set the cooperativity level to let DirectInput know how
	// this device should interact with the system and with other
	// DirectInput applications.
//#ifdef _DEBUG
#if 1
	ret = device->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
#else
	ret = device->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
#endif
	if (ret != DI_OK)
		return(false);

    // get the caps of this device and make sure it has all we need.
	DIDEVCAPS caps;

	ZeroMemory(&caps, sizeof(DIDEVCAPS));
	caps.dwSize = sizeof(DIDEVCAPS);

	device->GetCapabilities(&caps);

	ASSERT(caps.dwAxes >= 3);

	// set the range of the joystick axis
	axisValid[0] = SetupAxis(DIJOFS_X);
	axisValid[1] = SetupAxis(DIJOFS_Y);
	axisValid[2] = SetupAxis(DIJOFS_Z);
	axisValid[3] = SetupAxis(DIJOFS_RX);
	axisValid[4] = SetupAxis(DIJOFS_RY);
	axisValid[5] = SetupAxis(DIJOFS_RZ);
	axisValid[6] = SetupAxis(DIJOFS_SLIDER(0));
	axisValid[7] = SetupAxis(DIJOFS_SLIDER(1));

	// Aquire it for now
	device->Acquire();

	return(true);
}

/***************************************************************************/
// Setup a direct input axis
/***************************************************************************/
bool WindowsControllerInputDriver::SetupAxis(
DWORD diAxis)
{
	DIDEVICEOBJECTINSTANCE oi;
	oi.dwSize = sizeof (DIDEVICEOBJECTINSTANCE);

	if (SUCCEEDED(device->GetObjectInfo(&oi, diAxis, DIPH_BYOFFSET)))
	{
		// set the range of the joystick axis
		DIPROPRANGE prop;
		memset(&prop, 0, sizeof(prop));
		prop.diph.dwSize = sizeof(prop);
		prop.diph.dwHeaderSize = sizeof(prop.diph);
		prop.diph.dwHow = DIPH_BYOFFSET;
		prop.lMin = 0;
		prop.lMax = TRANSFER_LOOKUPS - 1;
		prop.diph.dwObj = diAxis;
		HRESULT ret = device->SetProperty(DIPROP_RANGE, &prop.diph);
		return(ret == DI_OK);
	}

	return(false);
}

/***************************************************************************/
// Our setup
/***************************************************************************/
void WindowsControllerInputDriver::LoadProfile(
int deviceNumber)
{
	// Make sure these are all init'd
	for (uint b = 0; b < CB_BUTTONS; b++)
		buttonMap[b] = -1;

	// Get the name
	GetPrivateProfileString("Device", "name", "Unknown",
									deviceName, sizeof(deviceName),
									buttonMapFile[deviceNumber]);

	if(!GetProfileBool(deviceNumber, "ObserveFocus", observeFocus))
		observeFocus = false;

	// Load in a ini for true mappings
	GetProfileButton(deviceNumber, "CB_UP", buttonMap[CB_UP]);
	GetProfileButton(deviceNumber, "CB_DOWN", buttonMap[CB_DOWN]);
	GetProfileButton(deviceNumber, "CB_LEFT", buttonMap[CB_LEFT]);
	GetProfileButton(deviceNumber, "CB_RIGHT", buttonMap[CB_RIGHT]);
	GetProfileButton(deviceNumber, "CB_X", buttonMap[CB_X]);
	GetProfileButton(deviceNumber, "CB_O", buttonMap[CB_O]);
	GetProfileButton(deviceNumber, "CB_TRI", buttonMap[CB_TRI]);
	GetProfileButton(deviceNumber, "CB_SQUARE", buttonMap[CB_SQUARE]);
	GetProfileButton(deviceNumber, "CB_L1", buttonMap[CB_L1]);
	GetProfileButton(deviceNumber, "CB_R1", buttonMap[CB_R1]);
	GetProfileButton(deviceNumber, "CB_L2", buttonMap[CB_L2]);
	GetProfileButton(deviceNumber, "CB_R2", buttonMap[CB_R2]);
	GetProfileButton(deviceNumber, "CB_START", buttonMap[CB_START]);
	GetProfileButton(deviceNumber, "CB_SELECT", buttonMap[CB_SELECT]);
	GetProfileButton(deviceNumber, "CB_STICK1", buttonMap[CB_STICK1]);
	GetProfileButton(deviceNumber, "CB_STICK2", buttonMap[CB_STICK2]);

	// Setup the axes
	for (uint a = 0; a < AA_AXES; a++)
		axisMap[a] = -1;

	// Get the stick setups
	for (a = 0; a < AA_AXES; a++)
		GetProfileStick(deviceNumber, a);
}

/***************************************************************************/
// Get a bool from the mapping file
/***************************************************************************/
bool WindowsControllerInputDriver::GetProfileBool(int deviceNumber, char *boolName, bool &value)
{
	char strValue[64];

	DWORD ret = GetPrivateProfileString("Device", boolName, "",
														strValue, sizeof(strValue),
														buttonMapFile[deviceNumber]);

	// Did we get it?
	if (ret == 0 || strValue[0] == '\0')
		return(false);

	// convert it
	if(stricmp(strValue, "Yes")==0 || stricmp(strValue, "True")==0)
		value = true;
	else
		value = false;

	return(true);
}

/***************************************************************************/
// Get a button from the mapping file
/***************************************************************************/
bool WindowsControllerInputDriver::GetProfileButton(
int deviceNumber,
char *buttonName,
int &value)
{
	char strValue[64];

	DWORD ret = GetPrivateProfileString("Button Map", buttonName, "",
														strValue, sizeof(strValue),
														buttonMapFile[deviceNumber]);

	// Did we get it?
	if (ret == 0 || strValue[0] == '\0')
		return(false);

	// covnert it
	value = atoi(strValue);
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool WindowsControllerInputDriver::GetProfileStick(
int deviceNumber,
int stick)
{
	// Make the defaults
	initTransferFunction[stick] = ATF_LINEAR;
	initDeadZone[stick] = 5.0f;
	initClampZone[stick] = 2.0f;
	invert[stick] = false;

	// setup the section
	char section[64];
	switch (stick)
	{
		case AA_X1:
			strcpy(section, "X1");
			break;
		case AA_Y1:
			strcpy(section, "Y1");
			break;
		case AA_X2:
			strcpy(section, "X2");
			break;
		case AA_Y2:
			strcpy(section, "Y2");
			break;
		case AA_THROTTLE:
			strcpy(section, "THROTTLE");
			break;
		case AA_RUDDER:
			strcpy(section, "RUDDER");
			break;
	}

	// Get the transfer function
	char value[64];
	DWORD ret = GetPrivateProfileString(section, "Map", "",
														value, sizeof(value),
														buttonMapFile[deviceNumber]);

	// Did we get it?
	if (ret != 0 && value[0] != '\0')
	{
		// covnert it
		if (strcmpi(value, "parabola") == 0)
			initTransferFunction[stick] = ATF_PARABOLA;
		else if (strcmpi(value, "exp") == 0)
			initTransferFunction[stick] = ATF_EXPONENTIAL;
		else if (strcmpi(value, "power") == 0)
			initTransferFunction[stick] = ATF_POWER;
	}

	// Get the factor
	ret = GetPrivateProfileString(section, "factor", "",
											value, sizeof(value),
											buttonMapFile[deviceNumber]);

	// Did we get it?
	if (ret != 0 && value[0] != '\0')
		initFactor[stick] = (float)atof(value);
	else
		initFactor[stick] = 1.0f;

	// Get the dead zone
	ret = GetPrivateProfileString(section, "DeadZone", "",
											value, sizeof(value),
											buttonMapFile[deviceNumber]);

	// Did we get it?
	if (ret != 0 && value[0] != '\0')
		initDeadZone[stick] = (float)atof(value);

	// Get the clamp zone
	ret = GetPrivateProfileString(section, "ClampZone", "",
											value, sizeof(value),
											buttonMapFile[deviceNumber]);

	// Did we get it?
	if (ret != 0 && value[0] != '\0')
		initClampZone[stick] = (float)atof(value);

	// Get the invert
	ret = GetPrivateProfileString(section, "Invert", "",
											value, sizeof(value),
											buttonMapFile[deviceNumber]);

	// Did we get it?
	if (ret != 0 && value[0] != '\0')
		invert[stick] = (atoi(value) != 0);

	// Get the mapped stick
	ret = GetPrivateProfileString(section, "axis", "",
											value, sizeof(value),
											buttonMapFile[deviceNumber]);

	// Did we get it?
	if (ret != 0 && value[0] != '\0')
	{
		if (strcmpi(value, "x") == 0)
			axisMap[stick] = 0;
		else if (strcmpi(value, "y") == 0)
			axisMap[stick] = 1;
		else if (strcmpi(value, "z") == 0)
			axisMap[stick] = 2;
		else if (strcmpi(value, "rx") == 0)
			axisMap[stick] = 3;
		else if (strcmpi(value, "ry") == 0)
			axisMap[stick] = 4;
		else if (strcmpi(value, "rz") == 0)
			axisMap[stick] = 5;
		else if (strcmpi(value, "slider0") == 0)
			axisMap[stick] = 6;
		else if (strcmpi(value, "slider1") == 0)
			axisMap[stick] = 7;
		else
			axisMap[stick] = atoi(value);
	}

	return(true);
}

