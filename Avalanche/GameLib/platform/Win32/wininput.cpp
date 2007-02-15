/***************************************************************************/
// Windows input driver
/***************************************************************************/
#include "platform/PlatformPCH.h"
#include "cmdcon/cmdcon.h"

#ifdef DEBUGKEY
void DebugKey(unsigned char key, int remap);
#endif

// Statics
WindowsSystemInputDriver::DeviceMap WindowsSystemInputDriver::deviceMap[MAX_CONTROLLERS];
int WindowsSystemInputDriver::deviceMaps = 0;

// The device map file
#ifdef DIRECTX_PC
#define DEVICEMAP_INI 	".\\data_pc\\buttmap\\keymap.ini"
#else
#define DEVICEMAP_INI 	".\\data_dx\\buttmap\\devicemap.ini"
#endif

/***************************************************************************/
// Start create
/***************************************************************************/
WindowsSystemInputDriver::WindowsSystemInputDriver(void)
{
	// No direct input object yet
	dInput = NULL;

	// Init the map to nothing
	for (int i = 0; i < 256; i++)
		keyToButtonMap[i] = -1;

	// Setup the mappings (jesus, windows is so stupid)
	keyToButtonMap[VK_LBUTTON] = BC_MOUSELEFT;
	keyToButtonMap[VK_MBUTTON] = BC_MOUSEMIDDLE;
	keyToButtonMap[VK_RBUTTON] = BC_MOUSERIGHT;

	keyToButtonMap[VK_BACK] = BC_BACKSPACE;
	keyToButtonMap[VK_TAB] = BC_TAB;
	keyToButtonMap[VK_RETURN] = BC_ENTER;
	keyToButtonMap[VK_SHIFT] = BC_SHIFT;
	keyToButtonMap[VK_CONTROL] = BC_CONTROL;
	keyToButtonMap[VK_MENU]		= BC_ALT;
	keyToButtonMap[VK_ESCAPE] = BC_ESCAPE;
	keyToButtonMap[VK_SPACE] = BC_SPACE;
	keyToButtonMap[VK_END] = BC_END;
	keyToButtonMap[VK_HOME] = BC_HOME;
	keyToButtonMap[VK_LEFT] = BC_LEFT;
	keyToButtonMap[VK_CLEAR] = BC_CENTER;
	keyToButtonMap[VK_UP] = BC_UP;
	keyToButtonMap[VK_RIGHT] = BC_RIGHT;
	keyToButtonMap[VK_DOWN] = BC_DOWN;
	keyToButtonMap[VK_INSERT] = BC_INSERT;
	keyToButtonMap[VK_DELETE] = BC_DELETE;
	keyToButtonMap[VK_PRIOR] = BC_PAGEUP;
	keyToButtonMap[VK_NEXT] = BC_PAGEDOWN;

	keyToButtonMap['0'] = BC_0;
	keyToButtonMap['1'] = BC_1;
	keyToButtonMap['2'] = BC_2;
	keyToButtonMap['3'] = BC_3;
	keyToButtonMap['4'] = BC_4;
	keyToButtonMap['5'] = BC_5;
	keyToButtonMap['6'] = BC_6;
	keyToButtonMap['7'] = BC_7;
	keyToButtonMap['8'] = BC_8;
	keyToButtonMap['9'] = BC_9;

	keyToButtonMap['A'] = BC_A;
	keyToButtonMap['B'] = BC_B;
	keyToButtonMap['C'] = BC_C;
	keyToButtonMap['D'] = BC_D;
	keyToButtonMap['E'] = BC_E;
	keyToButtonMap['F'] = BC_F;
	keyToButtonMap['G'] = BC_G;
	keyToButtonMap['H'] = BC_H;
	keyToButtonMap['I'] = BC_I;
	keyToButtonMap['J'] = BC_J;
	keyToButtonMap['K'] = BC_K;
	keyToButtonMap['L'] = BC_L;
	keyToButtonMap['M'] = BC_M;
	keyToButtonMap['N'] = BC_N;
	keyToButtonMap['O'] = BC_O;
	keyToButtonMap['P'] = BC_P;
	keyToButtonMap['Q'] = BC_Q;
	keyToButtonMap['R'] = BC_R;
	keyToButtonMap['S'] = BC_S;
	keyToButtonMap['T'] = BC_T;
	keyToButtonMap['U'] = BC_U;
	keyToButtonMap['V'] = BC_V;
	keyToButtonMap['W'] = BC_W;
	keyToButtonMap['X'] = BC_X;
	keyToButtonMap['Y'] = BC_Y;
	keyToButtonMap['Z'] = BC_Z;

	keyToButtonMap[VK_MULTIPLY] = BC_MULTIPLY;
	keyToButtonMap[VK_ADD] = BC_ADD;
	keyToButtonMap[VK_SUBTRACT] = BC_SUBTRACT;
	keyToButtonMap[VK_DIVIDE] = BC_DIVIDE;

	keyToButtonMap[VK_F1] = BC_F1;
	keyToButtonMap[VK_F2] = BC_F2;
	keyToButtonMap[VK_F3] = BC_F3;
	keyToButtonMap[VK_F4] = BC_F4;
	keyToButtonMap[VK_F5] = BC_F5;
	keyToButtonMap[VK_F6] = BC_F6;
	keyToButtonMap[VK_F7] = BC_F7;
	keyToButtonMap[VK_F8] = BC_F8;
	keyToButtonMap[VK_F9] = BC_F9;

	// Hacking
	keyToButtonMap[192] = BC_TILDE;
	keyToButtonMap[189] = BC_MINUS;
	keyToButtonMap[187] = BC_PLUS;
	keyToButtonMap[219] = BC_LEFTBRACKET;
	keyToButtonMap[221] = BC_RIGHTBRACKET;
	keyToButtonMap[220] = BC_FORWARDSLASH;
	keyToButtonMap[186] = BC_SEMICOLON;
	keyToButtonMap[222] = BC_QUOTE;
	keyToButtonMap[188] = BC_COMMA;
	keyToButtonMap[190] = BC_PERIOD;
	keyToButtonMap[191] = BC_BACKSLASH;

	// How many entries are really in the map?
	mapEntries = 256;
	while (mapEntries && keyToButtonMap[mapEntries - 1] == -1)
		--mapEntries;

	// Clear the controllers
	controllers = 0;
	for (int j = 0; j < MAX_CONTROLLERS; j++)
		controller[j] = NULL;
}

/***************************************************************************/
// Clean up (remember to call Terminate!!)
/***************************************************************************/
WindowsSystemInputDriver::~WindowsSystemInputDriver(void)
{
	Terminate();
}

/***************************************************************************/
// Call this at the very beginning to really intialize stuff
/***************************************************************************/
bool WindowsSystemInputDriver::Initialize(
HINSTANCE hInst,
HWND hWnd)
{
	// Save this for later
	this->hWnd = hWnd;

	// Create are dInput device
	HRESULT ret = DirectInputCreate(hInst, DIRECTINPUT_VERSION, &dInput, NULL);

	// Create the device map
	LoadDeviceMap();

	// Enumerate our joystick controller interfaces (creates controllers!)
	if (ret == DI_OK)
		dInput->EnumDevices(DIDEVTYPE_JOYSTICK, EnumJoysticks, this, DIEDFL_ATTACHEDONLY);

	// Make a keyboard controller if we can
	if (controllers < MAX_CONTROLLERS && controller[controllers] == NULL)
	{
		MEM_SET_ONE_SHOT_NEW_NAME("KeyControllerInputDriver");
#ifdef DIRECTX_PC
		controller[controllers] = new KeyControllerInputDriver(*this, DEVICEMAP_INI);
#else
		controller[controllers] = new KeyControllerInputDriver(*this, "buttmap\\keymap.ini");
#endif
		ASSERT(controller[controllers]);
		++controllers;
	}

	return true;
}

/***************************************************************************/
// Call this at the end to release system resources
/***************************************************************************/
void WindowsSystemInputDriver::Terminate(void)
{
	// Clear the controllers
	for (int j = 0; j < MAX_CONTROLLERS; j++)
		if (controller[j])
		{
			delete controller[j];
			controller[j] = NULL;
		}

	// Hey those idiots should be calling terminate....
	if (dInput)
	{
		dInput->Release();
		dInput = NULL;
	}
}

/***************************************************************************/
// Call this to beigin using input
/***************************************************************************/
#ifdef DIRECTX_PC
#define DEAD_ZONE 0
#endif
void WindowsSystemInputDriver::BeginInput(void)
{
	int i;

	// Transfer all the states
	for (i = 0; i < BC_BUTTONCODES; i++)
		state[i].last = state[i].now;

	if (hWnd == GetFocus())
	{
		// Do the keyboard
		for (i = 0; i < mapEntries; i++)
		{
			int remap = keyToButtonMap[i];
			if (remap >= 0)
					state[remap].now = ((GetAsyncKeyState(i) & 0x8000) != 0);
		}

		// Copy in all the asynch states
		// we need better response, so make the l and r buttons true async
		state[BC_MOUSELEFT].now = asyncState[ASYNC_MOUSELEFT] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
		state[BC_MOUSEMIDDLE].now = asyncState[ASYNC_MOUSEMIDDLE];
		state[BC_MOUSERIGHT].now = asyncState[ASYNC_MOUSERIGHT] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
		state[BC_MOUSEWHEELUP].now = asyncState[ASYNC_MOUSEWHEELUP];
		state[BC_MOUSEWHEELDOWN].now = asyncState[ASYNC_MOUSEWHEELDOWN];

#ifdef DIRECTX_PC
        POINT tempPoint;
		RECT tempRect;
		GetCursorPos(&tempPoint);
		GetWindowRect(DisplayState.hWnd, &tempRect);
		int screenWidth = (int)((float)DisplayState.nScreenWidth * 0.5f) + tempRect.left;
		int screenHeight = (int)((float)DisplayState.nScreenHeight * 0.5f) + tempRect.top;
        tempPoint.x -= screenWidth;
		tempPoint.y -= screenHeight;
        mouseX = mouseY = 0;
        if(abs(tempPoint.x) > DEAD_ZONE) 
        { 
            mouseX = tempPoint.x;
        }
        if(abs(tempPoint.y) > DEAD_ZONE)
        {
            mouseY = tempPoint.y;
        }
		SetCursorPos(screenWidth,screenHeight);
#endif

	}

	// Do the controller stuff
	for (i = 0; i < MAX_CONTROLLERS; i++)
		if (controller[i])
			controller[i]->BeginInput();
}

/***************************************************************************/
// Clear the key input (call at the end of processing)
/***************************************************************************/
void WindowsSystemInputDriver::DoneInput(void)
{
	// Do the controller stuff
	for (int i = 0; i < MAX_CONTROLLERS; i++)
		if (controller[i])
			controller[i]->DoneInput();

	// Clear these special states
	asyncState[ASYNC_MOUSEWHEELUP] = false;
	asyncState[ASYNC_MOUSEWHEELDOWN] = false;

#ifdef DIRECTX_PC
    // the mouse gets polled every frame, so reset its
    // activity flag
    mouseInputLastFrame = false;
#endif
}

/***************************************************************************/
// Watch keyboard messages
/***************************************************************************/
#ifdef DIRECTX_PC
void WindowsSystemInputDriver::HandleMessage(
HWND hWnd,
UINT msg,
WPARAM wParam,
LPARAM lParam)
{
	switch (msg)
	{
		case WM_LBUTTONDOWN:
			mouseInputLastFrame = true;
			asyncState[ASYNC_MOUSELEFT] = true;
			SetCapture(hWnd);
			break;

		case WM_RBUTTONDOWN:
			mouseInputLastFrame = true;
			asyncState[ASYNC_MOUSERIGHT] = true;
			SetCapture(hWnd);
			break;

		case WM_MBUTTONDOWN:
			mouseInputLastFrame = true;
			asyncState[ASYNC_MOUSEMIDDLE] = true;
			SetCapture(hWnd);
			break;

		case WM_LBUTTONUP:
			mouseInputLastFrame = true;
			ReleaseCapture();
			asyncState[ASYNC_MOUSELEFT] = false;
			break;

		case WM_RBUTTONUP:
			mouseInputLastFrame = true;
			ReleaseCapture();
			asyncState[ASYNC_MOUSERIGHT] = false;
			break;

		case WM_MBUTTONUP:
			mouseInputLastFrame = true;
			ReleaseCapture();
			asyncState[ASYNC_MOUSEMIDDLE] = false;
			break;

		case WM_MOUSEMOVE:
			mouseInputLastFrame = true;
			break;

		case WM_MOUSEWHEEL:
			{
				mouseInputLastFrame = true;
				int delta = (short)HIWORD(wParam);
				if (delta > 0)
					asyncState[ASYNC_MOUSEWHEELUP] = true;
				else if (delta < 0)
					asyncState[ASYNC_MOUSEWHEELDOWN] = true;
			}
			break;
    }
}
#else
void WindowsSystemInputDriver::HandleMessage(
HWND hWnd,
UINT msg,
WPARAM wParam,
LPARAM lParam)
{
	switch (msg)
	{
   		case WM_LBUTTONDOWN:
  			mouseX = LOWORD(lParam);
  			mouseY = HIWORD(lParam);
   			asyncState[ASYNC_MOUSELEFT] = true;
   			SetCapture(hWnd);
   			break;

   		case WM_RBUTTONDOWN:
			mouseX = LOWORD(lParam);
			mouseY = HIWORD(lParam);
   			asyncState[ASYNC_MOUSERIGHT] = true;
   			SetCapture(hWnd);
   			break;

   		case WM_MBUTTONDOWN:
  			mouseX = LOWORD(lParam);
  			mouseY = HIWORD(lParam);
   			asyncState[ASYNC_MOUSEMIDDLE] = true;
   			SetCapture(hWnd);
   			break;

   		case WM_LBUTTONUP:
			mouseX = LOWORD(lParam);
			mouseY = HIWORD(lParam);
   			ReleaseCapture();
   			asyncState[ASYNC_MOUSELEFT] = false;
   			break;

   		case WM_RBUTTONUP:
			mouseX = LOWORD(lParam);
  			mouseY = HIWORD(lParam);
   			ReleaseCapture();
   			asyncState[ASYNC_MOUSERIGHT] = false;
   			break;

   		case WM_MBUTTONUP:
  			mouseX = LOWORD(lParam);
  			mouseY = HIWORD(lParam);
   			ReleaseCapture();
   			asyncState[ASYNC_MOUSEMIDDLE] = false;
   			break;

   		case WM_MOUSEMOVE:
  			mouseX = LOWORD(lParam);
  			mouseY = HIWORD(lParam);
   			break;

   		case WM_MOUSEWHEEL:
   			{
   				int delta = (short)HIWORD(wParam);
   				if (delta > 0)
   					asyncState[ASYNC_MOUSEWHEELUP] = true;
   				else if (delta < 0)
   					asyncState[ASYNC_MOUSEWHEELDOWN] = true;
   			}
   			break;

   		case WM_ACTIVATE:
   			{
    			// Do the controller stuff
   				for (int i = 0; i < MAX_CONTROLLERS; i++)
   					if (controller[i] && controller[i]->ObserveFocus())
   						controller[i]->Activate(wParam != WA_INACTIVE);
   			}
   			break;
	}
}
#endif
/***************************************************************************/
/***************************************************************************/
void WindowsSystemInputDriver::LoadDeviceMap()
{
	// No syntax checking here
	bool hasMapping = false;
	for (int i = 0; i < MAX_CONTROLLERS; i++)
	{
		// Controller
		char controller[32];

		// Make it
		sprintf(controller, "controller%d", i);

		// Get the name
		int len = GetPrivateProfileString("DeviceMap", controller, "",
						 								deviceMap[i].name, sizeof(deviceMap[i].name),
														DEVICEMAP_INI);
		if (len <= 0)
			deviceMap[i].controller = -1;
		else
		{
			deviceMap[i].controller = i;
			hasMapping = true;
		}			
		deviceMap[i].inUse = false;
	}

	// Did we get any data?
	deviceMaps = hasMapping ? MAX_CONTROLLERS : 0;
}

/***************************************************************************/
// Scan string for occurances of key
/***************************************************************************/
static const char *strstri(
const char *string,
const char *key)
{
	int len = strlen(string);
	int keylen = strlen(key);

	for (int i = 0; i < (len - keylen) + 1; i++)
	{
		for (int cmp = 0;
					cmp < keylen && tolower(string[i + cmp]) == tolower(key[cmp]);
					cmp++);

		// If we made the full scan, we have a match
		if (cmp == keylen)
			return &string[i];
	}

	// No match
	return NULL;
}

/***************************************************************************/
/***************************************************************************/
int WindowsSystemInputDriver::FindDevice(
const char *deviceName)
{
  	// scan the list
	for (int d = 0; d < deviceMaps; d++)
	{
		if (!deviceMap[d].inUse && strstri(deviceName, deviceMap[d].name) != NULL)
		{
			deviceMap[d].inUse = true;
			return deviceMap[d].controller;
		}
	}

	// Not found
	return -1;
}

/***************************************************************************/
// Enumerate and create the controller objects
/***************************************************************************/
BOOL CALLBACK WindowsSystemInputDriver::EnumJoysticks(
LPCDIDEVICEINSTANCE devInst,
LPVOID context)
{
	// Get the pointer to ourselves
	WindowsSystemInputDriver *me = (WindowsSystemInputDriver *)context;
	ASSERT(me);
	ASSERT(me->dInput);

	// What controller number are we binding to this device?
	int controller = FindDevice(devInst->tszInstanceName);
	if (controller < 0 && !HaveDeviceMap())
	{
		// Only intialize controllers in avaialable slots
		if (me->controllers < MAX_CONTROLLERS)
			controller = me->controllers++;
	}	

	// Special (since some controllers are named the same thing)
	if (me->controller[controller])
		controller = -1;

	// Informational
	char temp[300];
	if (controller >= 0)
		sprintf(temp, "Device \"%s\" mapped to slot %d.\n", devInst->tszInstanceName, controller);
	else
		sprintf(temp, "Device \"%s\" unmapped.\n", devInst->tszInstanceName);
	g_console.Echo(temp);

	// Ignore this controller?
	if (controller < 0)
		return DIENUM_CONTINUE;

	// duh
	ASSERT(controller >= 0);
	ASSERT(controller < MAX_CONTROLLERS);

	// obtain an interface to the enumerated joystick.
	LPDIRECTINPUTDEVICE device;
	HRESULT ret = me->dInput->CreateDevice(devInst->guidInstance, &device, NULL);

	// if it failed, then we can't use this joystick for some
	// bizarre reason.  (Maybe the user unplugged it while we
	// were in the middle of enumerating it.)  So continue enumerating
	if (ret != DI_OK)
		return DIENUM_CONTINUE;

	// Make the controller (it owns device now
	MEM_SET_ONE_SHOT_NEW_NAME("InputDriver");
	me->controller[controller] = new WindowsControllerInputDriver(me->hWnd, device, controller);
	ASSERT(me->controller[controller]);

	// Keep going?
	return DIENUM_CONTINUE;
}

/***************************************************************************/
// Debugging crap
/***************************************************************************/
#ifdef DEBUGKEY
#include <stdio.h>

/***************************************************************************/
// Fancy debug key, as Windows is documented so shittily
/***************************************************************************/
void DebugKey(
unsigned char key,
int remap)
{
	typedef struct
	{
		char 	  				*name;
		unsigned int      key;
	} NAMEKEY;

	static NAMEKEY windowsKey[] =
	{
		{ "VK_LBUTTON        ", 0x01 },
		{ "VK_RBUTTON        ", 0x02 },
		{ "VK_CANCEL         ", 0x03 },
		{ "VK_MBUTTON        ", 0x04 },
		{ "VK_BACK           ", 0x08 },
		{ "VK_TAB            ", 0x09 },
		{ "VK_CLEAR          ", 0x0C },
		{ "VK_RETURN         ", 0x0D },
		{ "VK_SHIFT          ", 0x10 },
		{ "VK_CONTROL        ", 0x11 },
		{ "VK_MENU           ", 0x12 },
		{ "VK_PAUSE          ", 0x13 },
		{ "VK_CAPITAL        ", 0x14 },
		{ "VK_KANA           ", 0x15 },
		{ "VK_HANGEUL        ", 0x15 },
		{ "VK_HANGUL         ", 0x15 },
		{ "VK_JUNJA          ", 0x17 },
		{ "VK_FINAL          ", 0x18 },
		{ "VK_HANJA          ", 0x19 },
		{ "VK_KANJI          ", 0x19 },
		{ "VK_ESCAPE         ", 0x1B },
		{ "VK_CONVERT        ", 0x1C },
		{ "VK_NONCONVERT     ", 0x1D },
		{ "VK_ACCEPT         ", 0x1E },
		{ "VK_MODECHANGE     ", 0x1F },
		{ "VK_SPACE          ", 0x20 },
		{ "VK_PRIOR          ", 0x21 },
		{ "VK_NEXT           ", 0x22 },
		{ "VK_END            ", 0x23 },
		{ "VK_HOME           ", 0x24 },
		{ "VK_LEFT           ", 0x25 },
		{ "VK_UP             ", 0x26 },
		{ "VK_RIGHT          ", 0x27 },
		{ "VK_DOWN           ", 0x28 },
		{ "VK_SELECT         ", 0x29 },
		{ "VK_PRINT          ", 0x2A },
		{ "VK_EXECUTE        ", 0x2B },
		{ "VK_SNAPSHOT       ", 0x2C },
		{ "VK_INSERT         ", 0x2D },
		{ "VK_DELETE         ", 0x2E },
		{ "VK_HELP           ", 0x2F },
		{ "VK_LWIN           ", 0x5B },
		{ "VK_RWIN           ", 0x5C },
		{ "VK_APPS           ", 0x5D },
		{ "VK_NUMPAD0        ", 0x60 },
		{ "VK_NUMPAD1        ", 0x61 },
		{ "VK_NUMPAD2        ", 0x62 },
		{ "VK_NUMPAD3        ", 0x63 },
		{ "VK_NUMPAD4        ", 0x64 },
		{ "VK_NUMPAD5        ", 0x65 },
		{ "VK_NUMPAD6        ", 0x66 },
		{ "VK_NUMPAD7        ", 0x67 },
		{ "VK_NUMPAD8        ", 0x68 },
		{ "VK_NUMPAD9        ", 0x69 },
		{ "VK_MULTIPLY       ", 0x6A },
		{ "VK_ADD            ", 0x6B },
		{ "VK_SEPARATOR      ", 0x6C },
		{ "VK_SUBTRACT       ", 0x6D },
		{ "VK_DECIMAL        ", 0x6E },
		{ "VK_DIVIDE         ", 0x6F },
		{ "VK_F1             ", 0x70 },
		{ "VK_F2             ", 0x71 },
		{ "VK_F3             ", 0x72 },
		{ "VK_F4             ", 0x73 },
		{ "VK_F5             ", 0x74 },
		{ "VK_F6             ", 0x75 },
		{ "VK_F7             ", 0x76 },
		{ "VK_F8             ", 0x77 },
		{ "VK_F9             ", 0x78 },
		{ "VK_F10            ", 0x79 },
		{ "VK_F11            ", 0x7A },
		{ "VK_F12            ", 0x7B },
		{ "VK_F13            ", 0x7C },
		{ "VK_F14            ", 0x7D },
		{ "VK_F15            ", 0x7E },
		{ "VK_F16            ", 0x7F },
		{ "VK_F17            ", 0x80 },
		{ "VK_F18            ", 0x81 },
		{ "VK_F19            ", 0x82 },
		{ "VK_F20            ", 0x83 },
		{ "VK_F21            ", 0x84 },
		{ "VK_F22            ", 0x85 },
		{ "VK_F23            ", 0x86 },
		{ "VK_F24            ", 0x87 },
		{ "VK_NUMLOCK        ", 0x90 },
		{ "VK_SCROLL         ", 0x91 }
	};
	static int windowsKeyEntries = sizeof(windowsKey) / sizeof(NAMEKEY);

	static NAMEKEY myKey[] =
	{
		{ "BC_A", 0x00 },
		{ "BC_B", 0x00 },
		{ "BC_C", 0x00 },
		{ "BC_D", 0x00 },
		{ "BC_E", 0x00 },
		{ "BC_F", 0x00 },
		{ "BC_G", 0x00 },
		{ "BC_H", 0x00 },
		{ "BC_I", 0x00 },
		{ "BC_J", 0x00 },
		{ "BC_K", 0x00 },
		{ "BC_L", 0x00 },
		{ "BC_M", 0x00 },
		{ "BC_N", 0x00 },
		{ "BC_O", 0x00 },
		{ "BC_P", 0x00 },
		{ "BC_Q", 0x00 },
		{ "BC_R", 0x00 },
		{ "BC_S", 0x00 },
		{ "BC_T", 0x00 },
		{ "BC_U", 0x00 },
		{ "BC_V", 0x00 },
		{ "BC_W", 0x00 },
		{ "BC_X", 0x00 },
		{ "BC_Y", 0x00 },
		{ "BC_Z", 0x00 },
		{ "BC_0", 0x00 },
		{ "BC_1", 0x00 },
		{ "BC_2", 0x00 },
		{ "BC_3", 0x00 },
		{ "BC_4", 0x00 },
		{ "BC_5", 0x00 },
		{ "BC_6", 0x00 },
		{ "BC_7", 0x00 },
		{ "BC_8", 0x00 },
		{ "BC_9", 0x00 },
		{ "BC_SPACE", 0x00 },
		{ "BC_TILDE", 0x00 },
		{ "BC_MINUS", 0x00 },
		{ "BC_PLUS", 0x00 },
		{ "BC_LEFTBRACKET", 0x00 },
		{ "BC_RIGHTBRACKET", 0x00 },
		{ "BC_FOWARDSLASH", 0x00 },
		{ "BC_SEMICOLON", 0x00 },
		{ "BC_QUOTE", 0x00 },
		{ "BC_PERIOD", 0x00 },
		{ "BC_COMMA", 0x00 },
		{ "BC_BACKSLASH", 0x00 },
		{ "BC_ESCAPE", 0x00 },
		{ "BC_BACKSPACE", 0x00 },
		{ "BC_TAB", 0x00 },
		{ "BC_ENTER", 0x00 },
		{ "BC_INSERT", 0x00 },
		{ "BC_DELETE", 0x00 },
		{ "BC_HOME", 0x00 },
		{ "BC_END", 0x00 },
		{ "BC_PAGEUP", 0x00 },
		{ "BC_PAGEDOWN", 0x00 },
		{ "BC_UP", 0x00 },
		{ "BC_DOWN", 0x00 },
		{ "BC_LEFT", 0x00 },
		{ "BC_CENTER", 0x00 },
		{ "BC_RIGHT", 0x00 },
		{ "BC_MULTIPLY", 0x00 },
		{ "BC_ADD", 0x00 },
		{ "BC_SUBTRACT", 0x00 },
		{ "BC_DIVIDE", 0x00 },
		{ "BC_F1", 0x00 },
		{ "BC_F2", 0x00 },
		{ "BC_F3", 0x00 },
		{ "BC_F4", 0x00 },
		{ "BC_F5", 0x00 },
		{ "BC_F6", 0x00 },
		{ "BC_F7", 0x00 },
		{ "BC_F8", 0x00 },
		{ "BC_F9", 0x00 },
		{ "BC_MOUSELEFT", 0x00 },
		{ "BC_MOUSEMIDDLE", 0x00 },
		{ "BC_MOUSERIGHT", 0x00 },
		{ "BC_MOUSEWHEELUP", 0x00 },
		{ "BC_MOUSEWHEELDOWN", 0x00 },
		{ "BC_SHIFT", 0x00 },
		{ "BC_CONTROL", 0x00 }
	};
	static int myKeyEntries = sizeof(myKey) / sizeof(NAMEKEY);

	static int firstTime = true;

	if (firstTime)
	{
		for (int i = 0; i < myKeyEntries; i++)
			myKey[i].key = i;
	}

	// Open the file
	FILE *fh;
	if (firstTime)
		fh = fopen("debugkey.txt", "wt");
	else
		fh = fopen("debugkey.txt", "at");
	firstTime = false;
	if (!fh)
		return;

	// Find my key name
	char *myKeyName = "Uknown";
	for (int i = 0; remap >= 0 && i < myKeyEntries; i++)
		if (myKey[i].key == (unsigned int)remap)
		{
		 	myKeyName = myKey[i].name;
			break;
		}

	// Find the windows key map name
	char *windowsKeyName = "Uknown";
	for (i = 0; i < windowsKeyEntries; i++)
		if (windowsKey[i].key == key)
		{
			windowsKeyName = windowsKey[i].name;
			break;
		}

	// Show the output
	fprintf(fh, "Remap: [%d] %s = [%u] %s (%c)\n", remap, myKeyName, (unsigned int)key, windowsKeyName, key);
	fclose(fh);
}

#endif
