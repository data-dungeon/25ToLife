/***************************************************************************/
// Windows controller input
/***************************************************************************/

/******************* includes ****************************/

#include "platform/PlatformPCH.h"
#ifdef DIRECTX_PC
#include "EngineHelper/timer.h"
#include "cmdcon/cmdcon.h"
#endif


/***************************************************************************/
/***************************************************************************/
KeyControllerInputDriver::KeyControllerInputDriver(
SystemInputDriver &inputDriver,
const char *buttonMapName) :
input(inputDriver)
{
	LoadButtonMap(buttonMapName);
#ifdef DIRECTX_PC
    inFirstPerson = false;
#endif
}

/***************************************************************************/
// Setup a stick axis
/***************************************************************************/
bool KeyControllerInputDriver::SetupAxis(
AnalogAxis axis,
AxesTransferFunction transferFunction,
float factor,
float deadZonePercent,
float clampZonePercent)
{
	// We don't do anything here
	return(true);
}

/***************************************************************************/
// Are we connected
/***************************************************************************/
bool KeyControllerInputDriver::Connected(void)
{
	return(true);
}

/***************************************************************************/
// The input system calls this to "refresh" input at frame start
/***************************************************************************/
#ifdef DIRECTX_PC // Ritual PC function
#define DIAGONAL_COMPENSATION_FACTOR 0.70710f
void KeyControllerInputDriver::BeginInput(void)
{
	// if we're walking, move us slower
	float walkfactor = 1.0f;
    if (input.ButtonDown(Button(BUTTON_WALK)))
    {
		walkfactor = 0.49f;
	}
	SetAnalog(stick[AA_Y1], walkfactor, Button(STICK1_UP), Button(STICK1_DOWN));
	SetAnalog(stick[AA_X1], walkfactor, Button(STICK1_RIGHT), Button(STICK1_LEFT));
	// correct for fast movement in two directions bug
	if ( abs(stick[AA_Y1]) == walkfactor && abs(stick[AA_X1]) == walkfactor )
	{
		stick[AA_Y1] *= DIAGONAL_COMPENSATION_FACTOR;
		stick[AA_X1] *= DIAGONAL_COMPENSATION_FACTOR;
	}


#ifdef WIN32
	// These are for debugging purposes on PC
	stick[AA_THROTTLE] = stick[AA_RUDDER] = 0.0f;
#endif

	WindowsSystemInputDriver* inputDrv = (WindowsSystemInputDriver*)g_inputPtr;

	if (inputDrv)
	{
		if(inputDrv->wasMouseMoved() == true) 
		{
			WindowsSystemInputDriver* inputDrv = (WindowsSystemInputDriver*)g_inputPtr;
			float dt;
			if (g_timer.GetFrameMS() != 0)
			{
				dt = 1.0f / (float)g_timer.GetFrameMS();
				dt *= mouseSensitivity;
			}
			else
				dt = mouseSensitivity;

			stick[AA_Y2] = -(float)(inputDrv->MouseY()) * dt;
			stick[AA_X2] =  (float)(inputDrv->MouseX()) * dt;
		}
		SetState(CB_ESCAPE, BC_ESCAPE);
	}
    else
    {
       	SetAnalog(stick[AA_Y2], keyboardSensitivity, Button(STICK2_UP), Button(STICK2_DOWN));
        SetAnalog(stick[AA_X2], keyboardSensitivity, Button(STICK2_RIGHT), Button(STICK2_LEFT));
    }

	// Set all the states
    // since the up and down dpad buttons control
    // the first person modes in the game, we'll
    // make it so on the keyboard, the "first person"
    // key acts like a toggle
    SetState( CB_UP, Button( DPAD_UP ));
	if ( input.ButtonReleased(Button(BUTTON_FIRSTPERSON)) )
	{
		inFirstPerson = !inFirstPerson;
	}

	// check to see if we should be centering our view
	if ( input.ButtonDown(Button(BUTTON_CENTERVIEW)) ) centerView = true;
	else centerView = false;
	SetState(CB_DOWN, Button(DPAD_DOWN));
	SetState(CB_LEFT, Button(DPAD_LEFT));
	SetState(CB_RIGHT, Button(DPAD_RIGHT));

	SetState(CB_SQUARE, Button(BUTTON_SQUARE));
	SetState(CB_X, Button(BUTTON_X));
	SetState(CB_O, Button(BUTTON_O));
	SetState(CB_TRI, Button(BUTTON_TRI));

	SetState(CB_L1, Button(BUTTON_L1));
	SetState(CB_R1, Button(BUTTON_R1));
	SetState(CB_L2, Button(BUTTON_L2));
	SetState(CB_R2, Button(BUTTON_R2));

	SetState(CB_STICK1, Button(BUTTON_STICK1));
	SetState(CB_STICK2, Button(BUTTON_STICK2));

	SetState(CB_START, Button(BUTTON_START));
	SetState(CB_SELECT, Button(BUTTON_SELECT));

		
	
}
#else
void KeyControllerInputDriver::BeginInput(void)
{
	// Do the analog sticks
	SetAnalog(stick[AA_Y1], Button(STICK1_UP), Button(STICK1_DOWN));
	SetAnalog(stick[AA_X1], Button(STICK1_RIGHT), Button(STICK1_LEFT));
	SetAnalog(stick[AA_Y2], Button(STICK2_UP), Button(STICK2_DOWN));
	SetAnalog(stick[AA_X2], Button(STICK2_RIGHT), Button(STICK2_LEFT));

#ifdef WIN32
	// These are for debugging purposes on PC
	stick[AA_THROTTLE] = stick[AA_RUDDER] = 0.0f;
#endif

	// Set all the states
	SetState(CB_DOWN, Button(DPAD_DOWN));
	SetState(CB_UP, Button(DPAD_UP));
	SetState(CB_LEFT, Button(DPAD_LEFT));
	SetState(CB_RIGHT, Button(DPAD_RIGHT));

	SetState(CB_SQUARE, Button(BUTTON_SQUARE));
	SetState(CB_X, Button(BUTTON_X));
	SetState(CB_O, Button(BUTTON_O));
	SetState(CB_TRI, Button(BUTTON_TRI));

	SetState(CB_L1, Button(BUTTON_L1));
	SetState(CB_R1, Button(BUTTON_R1));
	SetState(CB_L2, Button(BUTTON_L2));
	SetState(CB_R2, Button(BUTTON_R2));

	SetState(CB_STICK1, Button(BUTTON_STICK1));
	SetState(CB_STICK2, Button(BUTTON_STICK2));

	SetState(CB_START, Button(BUTTON_START));
	SetState(CB_SELECT, Button(BUTTON_SELECT));
}
#endif


/***************************************************************************/
// The input system calls this to "flush" input at frame end
/***************************************************************************/
void KeyControllerInputDriver::DoneInput(void)
{
	// Nothing to see here, move along....
}

/***************************************************************************/
// Load in a button map
/***************************************************************************/
#ifdef DIRECTX_PC
bool KeyControllerInputDriver::LoadButtonMap(
const char *buttonMapName)
{

	if (DeviceName() != "Keyboard") return false;
	
//             ini FilePath   ini button name               button abstract enum				default key
//             ~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~             ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	~~~~~~~~~~~~~~
	MapButton( buttonMapName, "FIRST_PERSON",    buttonMap[ BUTTONABSTRACT_FIRST_PERSON ],		BC_V );
	MapButton( buttonMapName, "DPAD_DOWN",       buttonMap[ BUTTONABSTRACT_TAG ],				BC_T );
	MapButton( buttonMapName, "LEAN_LEFT",       buttonMap[ BUTTONABSTRACT_LEAN_LEFT ],			BC_Q );
	MapButton( buttonMapName, "LEAN_RIGHT",      buttonMap[ BUTTONABSTRACT_LEAN_RIGHT ],		BC_E );
	MapButton( buttonMapName, "MOVE_FORWARD",    buttonMap[ BUTTONABSTRACT_MOVE_FORWARD ],		BC_W );
	MapButton( buttonMapName, "MOVE_BACKWARD",   buttonMap[ BUTTONABSTRACT_MOVE_BACKWARD ],		BC_S );
	MapButton( buttonMapName, "STRAFE_LEFT",     buttonMap[ BUTTONABSTRACT_STRAFE_LEFT ],		BC_A );
	MapButton( buttonMapName, "STRAFE_RIGHT",    buttonMap[ BUTTONABSTRACT_STRAFE_RIGHT ],		BC_D );
	MapButton( buttonMapName, "BUTTON_STICK1",   buttonMap[ BUTTONABSTRACT_UNKNOWN_STICK1 ],	BC_INVALID );
	MapButton( buttonMapName, "LOOK_UP",         buttonMap[ BUTTONABSTRACT_LOOK_UP ],			BC_INVALID );
	MapButton( buttonMapName, "LOOK_DOWN",       buttonMap[ BUTTONABSTRACT_LOOK_DOWN ],			BC_INVALID );
	MapButton( buttonMapName, "TURN_LEFT",       buttonMap[ BUTTONABSTRACT_TURN_LEFT ],			BC_INVALID );
	MapButton( buttonMapName, "TURN_RIGHT",      buttonMap[ BUTTONABSTRACT_TURN_RIGHT ],		BC_INVALID );
	MapButton( buttonMapName, "ZOOM",            buttonMap[ BUTTONABSTRACT_ZOOM ],				BC_Z );
	MapButton( buttonMapName, "JUMP",            buttonMap[ BUTTONABSTRACT_JUMP ],				BC_SPACE );
	MapButton( buttonMapName, "BUTTON_O",        buttonMap[ BUTTONABSTRACT_UNKNOWN_O ],			BC_ENTER );
	MapButton( buttonMapName, "RELOAD",          buttonMap[ BUTTONABSTRACT_RELOAD ],			BC_R );
	MapButton( buttonMapName, "USE",             buttonMap[ BUTTONABSTRACT_USE ],				BC_MOUSERIGHT );
    MapButton( buttonMapName, "CROUCH",          buttonMap[ BUTTONABSTRACT_CROUCH ],			BC_C );
	MapButton( buttonMapName, "FIRE",            buttonMap[ BUTTONABSTRACT_FIRE ],				BC_MOUSELEFT );
	MapButton( buttonMapName, "PREVIOUS_WEAPON", buttonMap[ BUTTONABSTRACT_PREVIOUS_WEAPON ],   BC_MOUSEWHEELUP );
    MapButton( buttonMapName, "NEXT_WEAPON",     buttonMap[ BUTTONABSTRACT_NEXT_WEAPON ],		BC_MOUSEWHEELDOWN );
	MapButton( buttonMapName, "BUTTON_START",    buttonMap[ BUTTONABSTRACT_START ],				BC_ENTER );
	MapButton( buttonMapName, "BUTTON_SELECT",   buttonMap[ BUTTONABSTRACT_SELECT ],			BC_BACKSLASH );
	MapButton( buttonMapName, "WALK",            buttonMap[ BUTTONABSTRACT_WALK ],				BC_SHIFT );
    MapButton( buttonMapName, "CENTER_VIEW",     buttonMap[ BUTTONABSTRACT_CENTER_VIEW ],		BC_X );
	MapButton( buttonMapName, "THROW_GRENADE",	 buttonMap[ BUTTONABSTRACT_THROW_GRENADE ],		BC_G );
	MapButton( buttonMapName, "TAUNT",			 buttonMap[ BUTTONABSTRACT_TAUNT ],				BC_F );

	// read in our keyboard sensitivity
	UINT ret = GetPrivateProfileInt("Keyboard Settings", "TURN_RATE", 0, buttonMapName);
	if ( ret ) 
		keyboardSensitivity = (float)ret / 255.0f;
	else // initialize to some default value
		keyboardSensitivity = 1.0f;
	
	// read in our mouse parameters
	ret = GetPrivateProfileInt("Mouse Settings", "SENSITIVITY", 0, buttonMapName);
	if ( ret ) 
		mouseSensitivity = (float)ret / 255.0f;
	else // initialize to some default value
		mouseSensitivity = 0.5f;

	return(true);
}

bool KeyControllerInputDriver::WriteButtonMap(char * buttonMapLocation, char * errorString)
{
	// test for duplicate button map entries
	for( int c=0; c < BUTTONABSTRACT_NUM_ENTRIES; ++c )
	{
		for ( int d=0; d < BUTTONABSTRACT_NUM_ENTRIES; ++d )
		{
			if ( c != d )
				if (( buttonMap[c] == buttonMap[d] ) && 
					( buttonMap[c] !=  BC_INVALID ) )
				{
					//build our error string
					if (errorString)
						sprintf(errorString, "The key  %s  is bound to two different actions", 
							ButtonCodeToString(buttonMap[c]));
					return false;
				}
					
		}
	}

	char bmLocation[MAX_PATH];
	if (buttonMapLocation)
		strcpy(bmLocation, buttonMapLocation);
	else
		strcpy(bmLocation, ".\\data_pc\\buttmap\\keymap.ini");
	// write out each button mapping to our 
	// ini file
#define WRITE_BUTTON_INI( name, code ) \
	WritePrivateProfileString("Button Map", name, code, bmLocation);\

	WRITE_BUTTON_INI("FIRST_PERSON",	ButtonCodeToString(buttonMap[ BUTTONABSTRACT_FIRST_PERSON ] ) )
	WRITE_BUTTON_INI("DPAD_DOWN",       ButtonCodeToString(buttonMap[ BUTTONABSTRACT_TAG ]))
	WRITE_BUTTON_INI("LEAN_LEFT",       ButtonCodeToString(buttonMap[ BUTTONABSTRACT_LEAN_LEFT ]))		
	WRITE_BUTTON_INI("LEAN_RIGHT",      ButtonCodeToString(buttonMap[ BUTTONABSTRACT_LEAN_RIGHT ]))	
	WRITE_BUTTON_INI("MOVE_FORWARD",    ButtonCodeToString(buttonMap[ BUTTONABSTRACT_MOVE_FORWARD ]))	
	WRITE_BUTTON_INI("MOVE_BACKWARD",   ButtonCodeToString(buttonMap[ BUTTONABSTRACT_MOVE_BACKWARD ]))	
	WRITE_BUTTON_INI("STRAFE_LEFT",     ButtonCodeToString(buttonMap[ BUTTONABSTRACT_STRAFE_LEFT ]))	
	WRITE_BUTTON_INI("STRAFE_RIGHT",    ButtonCodeToString(buttonMap[ BUTTONABSTRACT_STRAFE_RIGHT ]))	
	WRITE_BUTTON_INI("BUTTON_STICK1",   ButtonCodeToString(buttonMap[ BUTTONABSTRACT_UNKNOWN_STICK1 ]))	
	WRITE_BUTTON_INI("LOOK_UP",         ButtonCodeToString(buttonMap[ BUTTONABSTRACT_LOOK_UP ]))	
	WRITE_BUTTON_INI("LOOK_DOWN",       ButtonCodeToString(buttonMap[ BUTTONABSTRACT_LOOK_DOWN ]))	
	WRITE_BUTTON_INI("TURN_LEFT",       ButtonCodeToString(buttonMap[ BUTTONABSTRACT_TURN_LEFT ]))	
	WRITE_BUTTON_INI("TURN_RIGHT",      ButtonCodeToString(buttonMap[ BUTTONABSTRACT_TURN_RIGHT ]))	
	WRITE_BUTTON_INI("ZOOM",            ButtonCodeToString(buttonMap[ BUTTONABSTRACT_ZOOM ]))	
	WRITE_BUTTON_INI("JUMP",            ButtonCodeToString(buttonMap[ BUTTONABSTRACT_JUMP ]))	
	WRITE_BUTTON_INI("BUTTON_O",        ButtonCodeToString(buttonMap[ BUTTONABSTRACT_UNKNOWN_O ]))	
	WRITE_BUTTON_INI("RELOAD",          ButtonCodeToString(buttonMap[ BUTTONABSTRACT_RELOAD ]))	
	WRITE_BUTTON_INI("USE",             ButtonCodeToString(buttonMap[ BUTTONABSTRACT_USE ]))	
	WRITE_BUTTON_INI("CROUCH",          ButtonCodeToString(buttonMap[ BUTTONABSTRACT_CROUCH ]))	
	WRITE_BUTTON_INI("FIRE",            ButtonCodeToString(buttonMap[ BUTTONABSTRACT_FIRE ]))	
	WRITE_BUTTON_INI("PREVIOUS_WEAPON", ButtonCodeToString(buttonMap[ BUTTONABSTRACT_PREVIOUS_WEAPON ]))
	WRITE_BUTTON_INI("NEXT_WEAPON",     ButtonCodeToString(buttonMap[ BUTTONABSTRACT_NEXT_WEAPON ]))	
	WRITE_BUTTON_INI("BUTTON_START",    ButtonCodeToString(buttonMap[ BUTTONABSTRACT_START ]))	
	WRITE_BUTTON_INI("BUTTON_SELECT",   ButtonCodeToString(buttonMap[ BUTTONABSTRACT_SELECT]))	
	WRITE_BUTTON_INI("WALK",            ButtonCodeToString(buttonMap[ BUTTONABSTRACT_WALK ]))	
	WRITE_BUTTON_INI("CENTER_VIEW",     ButtonCodeToString(buttonMap[ BUTTONABSTRACT_CENTER_VIEW ]))	
	WRITE_BUTTON_INI("THROW_GRENADE",   ButtonCodeToString(buttonMap[ BUTTONABSTRACT_THROW_GRENADE ]))
	WRITE_BUTTON_INI("TAUNT",			ButtonCodeToString(buttonMap[ BUTTONABSTRACT_TAUNT ]))
	
	// put this new map into effect
	// this is only really, reeeaaally necessary when we're 
	// actually playing the game
	LoadButtonMap(bmLocation);
	
	return true;	

}

void KeyControllerInputDriver::WriteDefaultButtonMap(char * buttonMapLocation)
{

	char bmLocation[MAX_PATH];
	if (buttonMapLocation)
		strcpy(bmLocation, buttonMapLocation);
	else
		strcpy(bmLocation, ".\\data_pc\\buttmap\\keymap.ini");

#define WRITE_BUTTON_INI( name, code ) \
	WritePrivateProfileString("Button Map", name, code, bmLocation);\

	WRITE_BUTTON_INI("FIRST_PERSON",	ButtonCodeToString( BC_V ) );
	WRITE_BUTTON_INI("DPAD_DOWN",       ButtonCodeToString( BC_T ) );
	WRITE_BUTTON_INI("LEAN_LEFT",       ButtonCodeToString( BC_Q ) );
	WRITE_BUTTON_INI("LEAN_RIGHT",      ButtonCodeToString( BC_E ) );
	WRITE_BUTTON_INI("MOVE_FORWARD",    ButtonCodeToString( BC_W ) );	
	WRITE_BUTTON_INI("MOVE_BACKWARD",   ButtonCodeToString( BC_S ) );	
	WRITE_BUTTON_INI("STRAFE_LEFT",     ButtonCodeToString( BC_A ) );
	WRITE_BUTTON_INI("STRAFE_RIGHT",    ButtonCodeToString( BC_D ) );	
	WRITE_BUTTON_INI("BUTTON_STICK1",   ButtonCodeToString( BC_INVALID ) );	
	WRITE_BUTTON_INI("LOOK_UP",         ButtonCodeToString( BC_INVALID ) );
	WRITE_BUTTON_INI("LOOK_DOWN",       ButtonCodeToString( BC_INVALID ) );
	WRITE_BUTTON_INI("TURN_LEFT",       ButtonCodeToString( BC_INVALID ) );
	WRITE_BUTTON_INI("TURN_RIGHT",      ButtonCodeToString( BC_INVALID ) );
	WRITE_BUTTON_INI("ZOOM",            ButtonCodeToString( BC_Z ) );
	WRITE_BUTTON_INI("JUMP",            ButtonCodeToString( BC_SPACE ) );
	WRITE_BUTTON_INI("BUTTON_O",        ButtonCodeToString( BC_ENTER ) );
	WRITE_BUTTON_INI("RELOAD",          ButtonCodeToString( BC_R ) );
	WRITE_BUTTON_INI("USE",             ButtonCodeToString( BC_MOUSERIGHT ) );
	WRITE_BUTTON_INI("CROUCH",          ButtonCodeToString( BC_C ) );
	WRITE_BUTTON_INI("FIRE",            ButtonCodeToString( BC_MOUSELEFT ) );
	WRITE_BUTTON_INI("PREVIOUS_WEAPON", ButtonCodeToString( BC_MOUSEWHEELUP ) );
	WRITE_BUTTON_INI("NEXT_WEAPON",     ButtonCodeToString( BC_MOUSEWHEELDOWN ) );
	WRITE_BUTTON_INI("BUTTON_START",    ButtonCodeToString( BC_ENTER ) );
	WRITE_BUTTON_INI("BUTTON_SELECT",   ButtonCodeToString( BC_BACKSLASH ) );
	WRITE_BUTTON_INI("WALK",            ButtonCodeToString( BC_SHIFT ) );
	WRITE_BUTTON_INI("CENTER_VIEW",     ButtonCodeToString( BC_X ) );
	WRITE_BUTTON_INI("THROW_GRENADE",   ButtonCodeToString( BC_G ) );
	WRITE_BUTTON_INI("TAUNT",			ButtonCodeToString( BC_F ) );
	
	// put this new map into effect
	// this is only really, reeeaaally necessary when we're 
	// actually playing the game
	LoadButtonMap(bmLocation);
}

#else
bool KeyControllerInputDriver::LoadButtonMap(
const char *buttonMapName)
{
	MapButton(buttonMapName, "DPAD_UP", buttonMap[DPAD_UP], BC_INVALID);
	MapButton(buttonMapName, "DPAD_DOWN", buttonMap[DPAD_DOWN], BC_INVALID);

	MapButton(buttonMapName, "DPAD_LEFT", buttonMap[DPAD_LEFT], BC_LEFT);
	MapButton(buttonMapName, "DPAD_RIGHT", buttonMap[DPAD_RIGHT], BC_RIGHT);

	MapButton(buttonMapName, "STICK1_UP", buttonMap[STICK1_UP], BC_INVALID);
	MapButton(buttonMapName, "STICK1_DOWN", buttonMap[STICK1_DOWN], BC_INVALID);
	MapButton(buttonMapName, "STICK1_LEFT", buttonMap[STICK1_LEFT], BC_INVALID);
	MapButton(buttonMapName, "STICK1_RIGHT", buttonMap[STICK1_RIGHT], BC_INVALID);
	MapButton(buttonMapName, "BUTTON_STICK1", buttonMap[BUTTON_STICK1], BC_INVALID);

	MapButton(buttonMapName, "STICK2_UP", buttonMap[STICK2_UP], BC_INVALID);
	MapButton(buttonMapName, "STICK2_DOWN", buttonMap[STICK2_DOWN], BC_INVALID);
	MapButton(buttonMapName, "STICK2_LEFT", buttonMap[STICK2_LEFT], BC_INVALID);
	MapButton(buttonMapName, "STICK2_RIGHT", buttonMap[STICK2_RIGHT], BC_INVALID);
	MapButton(buttonMapName, "BUTTON_STICK2", buttonMap[BUTTON_STICK2], BC_INVALID);

	MapButton(buttonMapName, "BUTTON_X", buttonMap[BUTTON_X], BC_UP);
	MapButton(buttonMapName, "BUTTON_O", buttonMap[BUTTON_O], BC_ENTER);
	MapButton(buttonMapName, "BUTTON_TRI", buttonMap[BUTTON_TRI], BC_C);
	MapButton(buttonMapName, "BUTTON_SQUARE", buttonMap[BUTTON_SQUARE], BC_DOWN);

	MapButton(buttonMapName, "BUTTON_L1", buttonMap[BUTTON_L1], BC_LEFTBRACKET);
	MapButton(buttonMapName, "BUTTON_R1", buttonMap[BUTTON_R1], BC_RIGHTBRACKET);
	MapButton(buttonMapName, "BUTTON_L2", buttonMap[BUTTON_L2], BC_SEMICOLON);
	MapButton(buttonMapName, "BUTTON_R2", buttonMap[BUTTON_R2], BC_QUOTE);

	MapButton(buttonMapName, "BUTTON_START", buttonMap[BUTTON_START], BC_ENTER);
	MapButton(buttonMapName, "BUTTON_SELECT", buttonMap[BUTTON_SELECT], BC_BACKSLASH);

	return(true);
}
#endif

/***************************************************************************/
/***************************************************************************/
void KeyControllerInputDriver::MapButton(
const char *buttonMapName,
char *tag,
ButtonCode &button,
ButtonCode defaultCode)
{
#if defined ( WIN32 ) && ! defined ( _XBOX )
	char value[64];

	// Get it
	DWORD ret = GetPrivateProfileString("Button Map", tag, "",
														value, sizeof(value),
														buttonMapName);
	// Did we get it?
	if (ret == 0 || value[0] == '\0' || !StringToButtonCode(value, button))
		button = defaultCode;

#else
	button = defaultCode;
#endif
}

/***************************************************************************/
// Convert a string to a button code
/***************************************************************************/
bool KeyControllerInputDriver::StringToButtonCode(
const char *string,
ButtonCode &code)
{
	// Structure for mapping a string into a button code
	struct BUTTONMAPPER
	{
		char 			*name;
		ButtonCode  code;
	};
	BUTTONMAPPER buttonMapper[] =
		{
		{ "A", BC_A },
		{ "B", BC_B },
		{ "C", BC_C },
		{ "D", BC_D },
		{ "E", BC_E },
		{ "F", BC_F },
		{ "G", BC_G },
		{ "H", BC_H },
		{ "I", BC_I },
		{ "J", BC_J },
		{ "K", BC_K },
		{ "L", BC_L },
		{ "M", BC_M },
		{ "N", BC_N },
		{ "O", BC_O },
		{ "P", BC_P },
		{ "Q", BC_Q },
		{ "R", BC_R },
		{ "S", BC_S },
		{ "T", BC_T },
		{ "U", BC_U },
		{ "V", BC_V },
		{ "W", BC_W },
		{ "X", BC_X },
		{ "Y", BC_Y },
		{ "Z", BC_Z },
		{ "0", BC_0 },
		{ "1", BC_1 },
		{ "2", BC_2 },
		{ "3", BC_3 },
		{ "4", BC_4 },
		{ "5", BC_5 },
		{ "6", BC_6 },
		{ "7", BC_7 },
		{ "8", BC_8 },
		{ "9", BC_9 },
		{ "SPACE", BC_SPACE },
		{ "TILDE", BC_TILDE },
		{ "MINUS", BC_MINUS },
		{ "PLUS", BC_PLUS },
		{ "LEFTBRACKET", BC_LEFTBRACKET },
		{ "RIGHTBRACKET", BC_RIGHTBRACKET },
		{ "FORWARDSLASH", BC_FORWARDSLASH },
		{ "SEMICOLON", BC_SEMICOLON },
		{ "QUOTE", BC_QUOTE },
		{ "PERIOD", BC_PERIOD },
		{ "COMMA", BC_COMMA },
		{ "BACKSLASH", BC_BACKSLASH },
		{ "ESCAPE", BC_ESCAPE },
		{ "BACKSPACE", BC_BACKSPACE },
		{ "TAB", BC_TAB },
		{ "ENTER", BC_ENTER },
		{ "INSERT", BC_INSERT },
		{ "DELETE", BC_DELETE },
		{ "HOME", BC_HOME },
		{ "END", BC_END },
		{ "PAGEUP", BC_PAGEUP },
		{ "PAGEDOWN", BC_PAGEDOWN },
		{ "UP", BC_UP },
		{ "DOWN", BC_DOWN },
		{ "LEFT", BC_LEFT },
		{ "CENTER", BC_CENTER },
		{ "RIGHT", BC_RIGHT },
		{ "MULTIPLY", BC_MULTIPLY },
		{ "ADD", BC_ADD },
		{ "SUBTRACT", BC_SUBTRACT },
		{ "DIVIDE", BC_DIVIDE },
		{ "F1", BC_F1 },
		{ "F2", BC_F2 },
		{ "F3", BC_F3 },
		{ "F4", BC_F4 },
		{ "F5", BC_F5 },
		{ "F6", BC_F6 },
		{ "F7", BC_F7 },
		{ "F8", BC_F8 },
		{ "F9", BC_F9 },
		{ "LEFT MOUSE", BC_MOUSELEFT },
		{ "MIDDLE MOUSE", BC_MOUSEMIDDLE },
		{ "RIGHT MOUSE", BC_MOUSERIGHT },
		{ "MOUSEWHEEL UP", BC_MOUSEWHEELUP },
		{ "MOUSEWHEEL DOWN", BC_MOUSEWHEELDOWN },
		{ "SHIFT", BC_SHIFT },
		{ "CONTROL", BC_CONTROL },
		{ "ALT",	 BC_ALT }
		};
	int buttonMapperSize = sizeof(buttonMapper) / sizeof(BUTTONMAPPER);

	// Scan the list for the code
	for (int i = 0; i < buttonMapperSize; i++)
		if (strcmpi(buttonMapper[i].name, string) == 0)
		{
			code = buttonMapper[i].code;
			return(true);
		}

	return(false);
}

#ifdef DIRECTX_PC
/***************************************************************************/
// Convert a string to a button code
/***************************************************************************/
char* KeyControllerInputDriver::ButtonCodeToString( ButtonCode code ) const
{
//	ButtonCode code = static_cast<ButtonCode>( codeNumber );

	// Structure for mapping a string into a button code
	struct BUTTONMAPPER
	{
		char 			*name;
		ButtonCode		code;
	};
	BUTTONMAPPER buttonMapper[] =
		{
		{ "A", BC_A },
		{ "B", BC_B },
		{ "C", BC_C },
		{ "D", BC_D },
		{ "E", BC_E },
		{ "F", BC_F },
		{ "G", BC_G },
		{ "H", BC_H },
		{ "I", BC_I },
		{ "J", BC_J },
		{ "K", BC_K },
		{ "L", BC_L },
		{ "M", BC_M },
		{ "N", BC_N },
		{ "O", BC_O },
		{ "P", BC_P },
		{ "Q", BC_Q },
		{ "R", BC_R },
		{ "S", BC_S },
		{ "T", BC_T },
		{ "U", BC_U },
		{ "V", BC_V },
		{ "W", BC_W },
		{ "X", BC_X },
		{ "Y", BC_Y },
		{ "Z", BC_Z },
		{ "0", BC_0 },
		{ "1", BC_1 },
		{ "2", BC_2 },
		{ "3", BC_3 },
		{ "4", BC_4 },
		{ "5", BC_5 },
		{ "6", BC_6 },
		{ "7", BC_7 },
		{ "8", BC_8 },
		{ "9", BC_9 },
		{ "SPACE", BC_SPACE },
		{ "TILDE", BC_TILDE },
		{ "MINUS", BC_MINUS },
		{ "PLUS", BC_PLUS },
		{ "LEFTBRACKET", BC_LEFTBRACKET },
		{ "RIGHTBRACKET", BC_RIGHTBRACKET },
		{ "FORWARDSLASH", BC_FORWARDSLASH },
		{ "SEMICOLON", BC_SEMICOLON },
		{ "QUOTE", BC_QUOTE },
		{ "PERIOD", BC_PERIOD },
		{ "COMMA", BC_COMMA },
		{ "BACKSLASH", BC_BACKSLASH },
		{ "ESCAPE", BC_ESCAPE },
		{ "BACKSPACE", BC_BACKSPACE },
		{ "TAB", BC_TAB },
		{ "ENTER", BC_ENTER },
		{ "INSERT", BC_INSERT },
		{ "DELETE", BC_DELETE },
		{ "HOME", BC_HOME },
		{ "END", BC_END },
		{ "PAGEUP", BC_PAGEUP },
		{ "PAGEDOWN", BC_PAGEDOWN },
		{ "UP", BC_UP },
		{ "DOWN", BC_DOWN },
		{ "LEFT", BC_LEFT },
		{ "CENTER", BC_CENTER },
		{ "RIGHT", BC_RIGHT },
		{ "MULTIPLY", BC_MULTIPLY },
		{ "ADD", BC_ADD },
		{ "SUBTRACT", BC_SUBTRACT },
		{ "DIVIDE", BC_DIVIDE },
		{ "F1", BC_F1 },
		{ "F2", BC_F2 },
		{ "F3", BC_F3 },
		{ "F4", BC_F4 },
		{ "F5", BC_F5 },
		{ "F6", BC_F6 },
		{ "F7", BC_F7 },
		{ "F8", BC_F8 },
		{ "F9", BC_F9 },
		{ "LEFT MOUSE", BC_MOUSELEFT },
		{ "MIDDLE MOUSE", BC_MOUSEMIDDLE },
		{ "RIGHT MOUSE", BC_MOUSERIGHT },
		{ "MOUSEWHEEL UP", BC_MOUSEWHEELUP },
		{ "MOUSEWHEEL DOWN", BC_MOUSEWHEELDOWN },
		{ "SHIFT", BC_SHIFT },
		{ "ALT",   BC_ALT },
		{ "CONTROL", BC_CONTROL }
		};
	int buttonMapperSize = sizeof(buttonMapper) / sizeof(BUTTONMAPPER);

	// Scan the list for the code
	for (int i = 0; i < buttonMapperSize; i++)
	{
		if( buttonMapper[i].code == code )
		{
			return buttonMapper[i].name;
		}
	}

	return "UNKNOWN KEY";
}




/***************************************************************************/
// Get a string name for an abstract action enum
/***************************************************************************/
const char* KeyControllerInputDriver::ButtonMapToDescription( int abstractActionNumber ) const
{
	struct BUTTONMAPPER
	{
		char 			*name;
		int				abstractActionNumber;
	};
	BUTTONMAPPER buttonMapper[] =
	{
		{ "First / Third Person",			BUTTONABSTRACT_FIRST_PERSON },
		{ "Custom Tag",						BUTTONABSTRACT_TAG },
		{ "Lean Left",						BUTTONABSTRACT_LEAN_LEFT },
		{ "Lean Right",						BUTTONABSTRACT_LEAN_RIGHT },
		{ "Move Forward",					BUTTONABSTRACT_MOVE_FORWARD },
		{ "Move Backward",					BUTTONABSTRACT_MOVE_BACKWARD },
		{ "Strafe Left",					BUTTONABSTRACT_STRAFE_LEFT },
		{ "Strafe Right",					BUTTONABSTRACT_STRAFE_RIGHT },
		{ "BUTTONABSTRACT_UNKNOWN_STICK1",	BUTTONABSTRACT_UNKNOWN_STICK1 },
		{ "Look Up",						BUTTONABSTRACT_LOOK_UP },
		{ "Look Down",						BUTTONABSTRACT_LOOK_DOWN },
		{ "Turn Left",						BUTTONABSTRACT_TURN_LEFT },
		{ "Turn Right",						BUTTONABSTRACT_TURN_RIGHT },
		{ "Zoom",							BUTTONABSTRACT_ZOOM },
		{ "Jump",							BUTTONABSTRACT_JUMP },
		{ "Use",							BUTTONABSTRACT_UNKNOWN_O },
		{ "Reload",							BUTTONABSTRACT_RELOAD },
		{ "Use",							BUTTONABSTRACT_USE },
		{ "Crouch",							BUTTONABSTRACT_CROUCH },
		{ "Fire",							BUTTONABSTRACT_FIRE },
		{ "Melee Weapon",					BUTTONABSTRACT_PREVIOUS_WEAPON },
		{ "Next Weapon",					BUTTONABSTRACT_NEXT_WEAPON },
		{ "BUTTONABSTRACT_START",			BUTTONABSTRACT_START },
		{ "Show Scoreboard",				BUTTONABSTRACT_SELECT },
		{ "Walk",							BUTTONABSTRACT_WALK },
		{ "Center View",					BUTTONABSTRACT_CENTER_VIEW },
		{ "Throw Grenade",					BUTTONABSTRACT_THROW_GRENADE },

	};
	int buttonMapperSize = sizeof(buttonMapper) / sizeof(BUTTONMAPPER);

	// Scan the list for the code
	for (int i = 0; i < buttonMapperSize; i++)
	{
		if( buttonMapper[i].abstractActionNumber == abstractActionNumber )
		{
			return buttonMapper[i].name;
		}
	}

	return "UNKNOWN ABSTRACT ACTION NAME";
}


/***************************************************************************/
// Get a string name for an abstract action enum
/***************************************************************************/
bool KeyControllerInputDriver::ButtonMapToString( int abstractActionNumber, char* uiStringName ) const
{
	ButtonCode bc = buttonMap[ abstractActionNumber ];
	const char* bcStringName = ButtonCodeToString( bc );
	strcpy( uiStringName, bcStringName );
	return true;

/*
	struct BUTTONMAPPER
	{
		char 			*name;
		int				abstractActionNumber;
	};
	BUTTONMAPPER buttonMapper[] =
	{
		{ "First / Third Person",				BUTTONABSTRACT_FIRST_PERSON },
		{ "BUTTONABSTRACT_UNKNOWN_DPAD_DOWN",	BUTTONABSTRACT_UNKNOWN_DPAD_DOWN },
		{ "Lean Left",							BUTTONABSTRACT_LEAN_LEFT },
		{ "Lean Right",							BUTTONABSTRACT_LEAN_RIGHT },
		{ "Move Forward",						BUTTONABSTRACT_MOVE_FORWARD },
		{ "Move Backward",						BUTTONABSTRACT_MOVE_BACKWARD },
		{ "Strafe Left",						BUTTONABSTRACT_STRAFE_LEFT },
		{ "Strafe Right",						BUTTONABSTRACT_STRAFE_RIGHT },
		{ "BUTTONABSTRACT_UNKNOWN_STICK1",		BUTTONABSTRACT_UNKNOWN_STICK1 },
		{ "Look Up",							BUTTONABSTRACT_LOOK_UP },
		{ "Look Down",							BUTTONABSTRACT_LOOK_DOWN },
		{ "Turn Left",							BUTTONABSTRACT_TURN_LEFT },
		{ "Turn Right",							BUTTONABSTRACT_TURN_RIGHT },
		{ "Zoom",								BUTTONABSTRACT_ZOOM },
		{ "Jump",								BUTTONABSTRACT_JUMP },
		{ "BUTTONABSTRACT_UNKNOWN_O",			BUTTONABSTRACT_UNKNOWN_O },
		{ "Reload",								BUTTONABSTRACT_RELOAD },
		{ "Use",								BUTTONABSTRACT_USE },
		{ "Crouch",								BUTTONABSTRACT_CROUCH },
		{ "Fire",								BUTTONABSTRACT_FIRE },
		{ "Previous Weapon",					BUTTONABSTRACT_PREVIOUS_WEAPON },
		{ "Next Weapon",						BUTTONABSTRACT_NEXT_WEAPON },
		{ "BUTTONABSTRACT_START",				BUTTONABSTRACT_START },
		{ "BUTTONABSTRACT_SELECT",				BUTTONABSTRACT_SELECT },
		{ "Walk",								BUTTONABSTRACT_WALK },
		{ "Center View",						BUTTONABSTRACT_CENTER_VIEW },
	};
	int buttonMapperSize = sizeof(buttonMapper) / sizeof(BUTTONMAPPER);

	// Scan the list for the code
	for (int i = 0; i < buttonMapperSize; i++)
	{
		if( buttonMapper[i].abstractActionNumber == abstractActionNumber )
		{
			strcpy( uiStringName, buttonMapper[i].name );
			return true;
		}
	}

	strcpy( uiStringName, "UNKNOWN ABSTRACT ACTION NAME" );
	return false;
*/
}


/***************************************************************************/
// Get a string name for the key currently bound to an abstract action enum
/***************************************************************************/
/*
bool KeyControllerInputDriver::ButtonMapToString( int abstractActionNumber, char* uiStringName ) const
{
	ButtonCode bc = buttonMap[ abstractActionNumber ];
	const char* bcStringName = ButtonCodeToString( bc );
	strcpy( uiStringName, bcStringName );
	return true;
/*
	for( int i = 0; i < BUTTONABSTRACT_NUM_ENTRIES; ++ i )
	{
		ButtonCode bc = buttonMap[ i ];
	}


	struct BUTTONMAPPER
	{
		char 			*name;
		int				abstractActionNumber;
	};
	BUTTONMAPPER buttonMapper[] =
	{
		{ "First / Third Person",				BUTTONABSTRACT_FIRST_PERSON },
		{ "BUTTONABSTRACT_UNKNOWN_DPAD_DOWN",	BUTTONABSTRACT_UNKNOWN_DPAD_DOWN },
		{ "Lean Left",							BUTTONABSTRACT_LEAN_LEFT },
		{ "Lean Right",							BUTTONABSTRACT_LEAN_RIGHT },
		{ "Move Forward",						BUTTONABSTRACT_MOVE_FORWARD },
		{ "Move Backward",						BUTTONABSTRACT_MOVE_BACKWARD },
		{ "Strafe Left",						BUTTONABSTRACT_STRAFE_LEFT },
		{ "Strafe Right",						BUTTONABSTRACT_STRAFE_RIGHT },
		{ "BUTTONABSTRACT_UNKNOWN_STICK1",		BUTTONABSTRACT_UNKNOWN_STICK1 },
		{ "Look Up",							BUTTONABSTRACT_LOOK_UP },
		{ "Look Down",							BUTTONABSTRACT_LOOK_DOWN },
		{ "Turn Left",							BUTTONABSTRACT_TURN_LEFT },
		{ "Turn Right",							BUTTONABSTRACT_TURN_RIGHT },
		{ "Zoom",								BUTTONABSTRACT_ZOOM },
		{ "Jump",								BUTTONABSTRACT_JUMP },
		{ "BUTTONABSTRACT_UNKNOWN_O",			BUTTONABSTRACT_UNKNOWN_O },
		{ "Reload",								BUTTONABSTRACT_RELOAD },
		{ "Use",								BUTTONABSTRACT_USE },
		{ "Crouch",								BUTTONABSTRACT_CROUCH },
		{ "Fire",								BUTTONABSTRACT_FIRE },
		{ "Previous Weapon",					BUTTONABSTRACT_PREVIOUS_WEAPON },
		{ "Next Weapon",						BUTTONABSTRACT_NEXT_WEAPON },
		{ "BUTTONABSTRACT_START",				BUTTONABSTRACT_START },
		{ "BUTTONABSTRACT_SELECT",				BUTTONABSTRACT_SELECT },
		{ "Walk",								BUTTONABSTRACT_WALK },
		{ "Center View",						BUTTONABSTRACT_CENTER_VIEW },
	};
	int buttonMapperSize = sizeof(buttonMapper) / sizeof(BUTTONMAPPER);

	// Scan the list for the code
	for (int i = 0; i < buttonMapperSize; i++)
	{
		if( buttonMapper[i].abstractActionNumber == abstractActionNumber )
		{
			strcpy( uiStringName, buttonMapper[i].name );
			return true;
		}
	}

	strcpy( uiStringName, "UNKNOWN ABSTRACT ACTION NAME" );
	return false;
}
*/


/*
bool KeyControllerInputDriver::ButtonMapToString( int codeNumber, char* stringName ) const
{
	ButtonCode code = static_cast<ButtonCode>( codeNumber );

	// Structure for mapping a string into a button code
	struct BUTTONMAPPER
	{
		char 			*name;
		ButtonCode		code;
	};
	BUTTONMAPPER buttonMapper[] =
		{
		{ "A", BC_A },
		{ "B", BC_B },
		{ "C", BC_C },
		{ "D", BC_D },
		{ "E", BC_E },
		{ "F", BC_F },
		{ "G", BC_G },
		{ "H", BC_H },
		{ "I", BC_I },
		{ "J", BC_J },
		{ "K", BC_K },
		{ "L", BC_L },
		{ "M", BC_M },
		{ "N", BC_N },
		{ "O", BC_O },
		{ "P", BC_P },
		{ "Q", BC_Q },
		{ "R", BC_R },
		{ "S", BC_S },
		{ "T", BC_T },
		{ "U", BC_U },
		{ "V", BC_V },
		{ "W", BC_W },
		{ "X", BC_X },
		{ "Y", BC_Y },
		{ "Z", BC_Z },
		{ "0", BC_0 },
		{ "1", BC_1 },
		{ "2", BC_2 },
		{ "3", BC_3 },
		{ "4", BC_4 },
		{ "5", BC_5 },
		{ "6", BC_6 },
		{ "7", BC_7 },
		{ "8", BC_8 },
		{ "9", BC_9 },
		{ "SPACE", BC_SPACE },
		{ "TILDE", BC_TILDE },
		{ "MINUS", BC_MINUS },
		{ "PLUS", BC_PLUS },
		{ "LEFTBRACKET", BC_LEFTBRACKET },
		{ "RIGHTBRACKET", BC_RIGHTBRACKET },
		{ "FORWARDSLASH", BC_FORWARDSLASH },
		{ "SEMICOLON", BC_SEMICOLON },
		{ "QUOTE", BC_QUOTE },
		{ "PERIOD", BC_PERIOD },
		{ "COMMA", BC_COMMA },
		{ "BACKSLASH", BC_BACKSLASH },
		{ "ESCAPE", BC_ESCAPE },
		{ "BACKSPACE", BC_BACKSPACE },
		{ "TAB", BC_TAB },
		{ "ENTER", BC_ENTER },
		{ "INSERT", BC_INSERT },
		{ "DELETE", BC_DELETE },
		{ "HOME", BC_HOME },
		{ "END", BC_END },
		{ "PAGEUP", BC_PAGEUP },
		{ "PAGEDOWN", BC_PAGEDOWN },
		{ "UP", BC_UP },
		{ "DOWN", BC_DOWN },
		{ "LEFT", BC_LEFT },
		{ "CENTER", BC_CENTER },
		{ "RIGHT", BC_RIGHT },
		{ "MULTIPLY", BC_MULTIPLY },
		{ "ADD", BC_ADD },
		{ "SUBTRACT", BC_SUBTRACT },
		{ "DIVIDE", BC_DIVIDE },
		{ "F1", BC_F1 },
		{ "F2", BC_F2 },
		{ "F3", BC_F3 },
		{ "F4", BC_F4 },
		{ "F5", BC_F5 },
		{ "F6", BC_F6 },
		{ "F7", BC_F7 },
		{ "F8", BC_F8 },
		{ "F9", BC_F9 },
		{ "MOUSELEFT", BC_MOUSELEFT },
		{ "MOUSEMIDDLE", BC_MOUSEMIDDLE },
		{ "MOUSERIGHT", BC_MOUSERIGHT },
		{ "MOUSEWHEELUP", BC_MOUSEWHEELUP },
		{ "MOUSEWHEELDOWN", BC_MOUSEWHEELDOWN },
		{ "SHIFT", BC_SHIFT },
		{ "CONTROL", BC_CONTROL }
		};
	int buttonMapperSize = sizeof(buttonMapper) / sizeof(BUTTONMAPPER);

	// Scan the list for the code
	for (int i = 0; i < buttonMapperSize; i++)
	{
		if( buttonMapper[i].code == code )
		{
			strcpy( stringName, buttonMapper[i].name );
			return true;
		}
	}

	strcpy( stringName, "UNKNOWN KEY" );
	return false;
}
*/

// change an abstract button code from the avatar hud stuff to a 
// descriptive string
const char* KeyControllerInputDriver::DigitalButtonToKey( int button) const
{

/*	
		FIRE,
		RELOAD,
		JUMP,
		CROUCH,
		USE,
		CHANGEWEAPON,
#ifdef DIRECTX_PC
		CHANGEBACKWEAPON,
#endif
		SELECTMELEE,
		HUCK,
		PUSHTOTALK,
		AIM,
		TAUNT,
		TAG,
		PEEKLEFT,
		PEEKRIGHT,
		COMBO1,
		COMBO2,
		};
*/
	switch(button)
	{
	case 0:
		return ButtonCodeToString(buttonMap[ BUTTONABSTRACT_FIRE ] );
	case 1:
		return ButtonCodeToString(buttonMap[ BUTTONABSTRACT_RELOAD ] );
	case 2:
		return ButtonCodeToString(buttonMap[ BUTTONABSTRACT_JUMP ] );
	case 3:
		return ButtonCodeToString(buttonMap[ BUTTONABSTRACT_CROUCH ] );
	case 4:
		return ButtonCodeToString(buttonMap[ BUTTONABSTRACT_USE ] );
	case 5:
		return ButtonCodeToString(buttonMap[ BUTTONABSTRACT_NEXT_WEAPON ] );
	case 6:
	case 7:
		return ButtonCodeToString(buttonMap[ BUTTONABSTRACT_PREVIOUS_WEAPON ] );
	case 8:
		return ButtonCodeToString(buttonMap[ BUTTONABSTRACT_THROW_GRENADE ] );
	case 10: 
		return ButtonCodeToString(buttonMap[ BUTTONABSTRACT_ZOOM ] );
	case 12:
		return ButtonCodeToString(buttonMap[ BUTTONABSTRACT_TAG ] );
	case 13:
		return ButtonCodeToString(buttonMap[ BUTTONABSTRACT_LEAN_LEFT ] );
	case 14:
		return ButtonCodeToString(buttonMap[ BUTTONABSTRACT_LEAN_RIGHT ] );
	
	default:
		return "UNKNOWN KEY";
	}
}


/***************************************************************************/
// Bind a key manually
/***************************************************************************/
void KeyControllerInputDriver::BindBCToAbstractAction( ButtonCode bc, int abstractActionNumber )
{
	buttonMap[ abstractActionNumber ] = bc;
}


#endif // DIRECTX_PC


#ifdef DIRECTX_PC
/***************************************************************************/
// Set analog data
/***************************************************************************/
void KeyControllerInputDriver::SetAnalog(
float &stick,
float amt,
ButtonCode buttonPositive,
ButtonCode buttonNegative)
{
	stick = 0.0f;
	if (input.ButtonDown(buttonPositive))
		stick += amt;
	if (input.ButtonDown(buttonNegative))
		stick -= amt;
}
#else
void KeyControllerInputDriver::SetAnalog(
float &stick,
ButtonCode buttonPositive,
ButtonCode buttonNegative)
{
	stick = 0.0f;
	if (input.ButtonDown(buttonPositive))
		stick += 1.0f;
	if (input.ButtonDown(buttonNegative))
		stick += -1.0f;
}
#endif

/***************************************************************************/
// Set the button state
/***************************************************************************/
void KeyControllerInputDriver::SetState(
ControllerButton cb,
ButtonCode button)
{
	state[cb].last = state[cb].now;
	state[cb].now = input.ButtonDown(button);
}


#ifdef DIRECTX_PC
/***************************************************************************/
// Check if a controller button was pressed this frame
/***************************************************************************/
bool KeyControllerInputDriver::WasButtonPressedThisFrame( const ControllerButton& cb ) const
{
	return( state[cb].now && !state[cb].last );
}


/***************************************************************************/
// Convert a windows VK to a BC
/***************************************************************************/
int KeyControllerInputDriver::GetBCForVK( int vkCode ) const
{
	if( vkCode >= 'A' && vkCode <= 'Z' )
	{
		return BC_A + (vkCode - 'A');
	}

	if( vkCode >= '0' && vkCode <= '9' )
	{
		return BC_0 + (vkCode - '0');
	}

	struct BUTTONMAPPER
	{
		int			vk;
		ButtonCode	bc;
	};
	BUTTONMAPPER buttonMapper[] =
	{
		{ VK_SPACE,				BC_SPACE, },
		{ VK_OEM_3,				BC_TILDE, },
		{ VK_OEM_MINUS,			BC_MINUS, },
		{ VK_OEM_PLUS,			BC_PLUS, },
		{ VK_OEM_4,				BC_LEFTBRACKET, },
		{ VK_OEM_6,				BC_RIGHTBRACKET, },
		{ VK_OEM_2,				BC_FORWARDSLASH, },
		{ VK_OEM_1,				BC_SEMICOLON, },
		{ VK_OEM_7,				BC_QUOTE, },
		{ VK_OEM_PERIOD,			BC_PERIOD, },
		{ VK_OEM_COMMA,				BC_COMMA, },
		{ VK_OEM_5,			BC_BACKSLASH, },
		{ VK_ESCAPE,			BC_ESCAPE, },
		{ VK_BACK,			BC_BACKSPACE, },
		{ VK_TAB,				BC_TAB, },
		{ VK_RETURN,				BC_ENTER, },
		{ VK_INSERT,			BC_INSERT, },
		{ VK_DELETE,			BC_DELETE, },
		{ VK_HOME,				BC_HOME, },
		{ VK_END,				BC_END, },
//		{ VK_PAGEUP,			BC_PAGEUP, },
//		{ VK_PAGEDOWN,			BC_PAGEDOWN, },
		{ VK_UP,				BC_UP, },
		{ VK_DOWN,				BC_DOWN, },
		{ VK_LEFT,				BC_LEFT, },
//		{ VK_CENTER,			BC_CENTER, },
		{ VK_RIGHT,				BC_RIGHT, },
		{ VK_MULTIPLY,			BC_MULTIPLY, },
		{ VK_ADD,				BC_ADD, },
		{ VK_SUBTRACT,			BC_SUBTRACT, },
		{ VK_DIVIDE,			BC_DIVIDE, },
		{ VK_F1,				BC_F1, },
		{ VK_F2,				BC_F2, },
		{ VK_F3,				BC_F3, },
		{ VK_F4,				BC_F4, },
		{ VK_F5,				BC_F5, },
		{ VK_F6,				BC_F6, },
		{ VK_F7,				BC_F7, },
		{ VK_F8,				BC_F8, },
		{ VK_F9,				BC_F9, },
//		{ VK_MOUSELEFT,			BC_MOUSELEFT, },
//		{ VK_MOUSEMIDDLE,		BC_MOUSEMIDDLE, },
//		{ VK_MOUSERIGHT,		BC_MOUSERIGHT, },
//		{ VK_MOUSEWHEELUP,		BC_MOUSEWHEELUP, },
//		{ VK_MOUSEWHEELDOWN,	BC_MOUSEWHEELDOWN, },
		{ VK_SHIFT,				BC_SHIFT, },
		{ VK_MENU,				BC_ALT, },
		{ VK_CONTROL,			BC_CONTROL, },
	};
	int buttonMapperSize = sizeof(buttonMapper) / sizeof(BUTTONMAPPER);

	for( int i = 0; i < buttonMapperSize; ++ i )
	{
		if( buttonMapper[ i ].vk == vkCode )
		{
			return buttonMapper[ i ].bc;
		}
	}

	return BC_INVALID;
}


/***************************************************************************/
// Convert a ButtonCode to a Windows VirtualKey
/***************************************************************************/
int KeyControllerInputDriver::GetVKForBC( ButtonCode bc ) const
{
	if( bc >= BC_A && bc <= BC_Z )
	{
		return 'A' + (bc - BC_A);
	}

	if( bc >= BC_0 && bc <= BC_9 )
	{
		return '0' + (bc - BC_0);
	}

	struct BUTTONMAPPER
	{
		int			vk;
		ButtonCode	bc;
	};
	BUTTONMAPPER buttonMapper[] =
	{
		{ VK_SPACE,				BC_SPACE, },
		{ VK_OEM_3,				BC_TILDE, },
		{ VK_OEM_MINUS,			BC_MINUS, },
		{ VK_OEM_PLUS,			BC_PLUS, },
		{ VK_OEM_4,				BC_LEFTBRACKET, },
		{ VK_OEM_6,				BC_RIGHTBRACKET, },
		{ VK_OEM_2,				BC_FORWARDSLASH, },
		{ VK_OEM_1,				BC_SEMICOLON, },
		{ VK_OEM_7,				BC_QUOTE, },
		{ VK_OEM_PERIOD,			BC_PERIOD, },
		{ VK_OEM_COMMA,				BC_COMMA, },
		{ VK_OEM_5,			BC_BACKSLASH, },
		{ VK_ESCAPE,			BC_ESCAPE, },
		{ VK_BACK,			BC_BACKSPACE, },
		{ VK_TAB,				BC_TAB, },
		{ VK_RETURN,				BC_ENTER, },
		{ VK_INSERT,			BC_INSERT, },
		{ VK_DELETE,			BC_DELETE, },
		{ VK_HOME,				BC_HOME, },
		{ VK_END,				BC_END, },
		{ VK_PRIOR,				BC_PAGEUP, },
		{ VK_NEXT,			BC_PAGEDOWN, },
		{ VK_UP,				BC_UP, },
		{ VK_DOWN,				BC_DOWN, },
		{ VK_LEFT,				BC_LEFT, },
//		{ VK_CENTER,			BC_CENTER, },
		{ VK_RIGHT,				BC_RIGHT, },
		{ VK_MULTIPLY,			BC_MULTIPLY, },
		{ VK_ADD,				BC_ADD, },
		{ VK_SUBTRACT,			BC_SUBTRACT, },
		{ VK_DIVIDE,			BC_DIVIDE, },
		{ VK_F1,				BC_F1, },
		{ VK_F2,				BC_F2, },
		{ VK_F3,				BC_F3, },
		{ VK_F4,				BC_F4, },
		{ VK_F5,				BC_F5, },
		{ VK_F6,				BC_F6, },
		{ VK_F7,				BC_F7, },
		{ VK_F8,				BC_F8, },
		{ VK_F9,				BC_F9, },
		{ VK_LBUTTON,			BC_MOUSELEFT, },
		{ VK_MBUTTON,			BC_MOUSEMIDDLE, },
		{ VK_RBUTTON,			BC_MOUSERIGHT, },
		//{ VK_MOUSEWHEELUP,		BC_MOUSEWHEELUP, },
		//{ VK_MOUSEWHEELDOWN,	BC_MOUSEWHEELDOWN, },
		{ VK_SHIFT,				BC_SHIFT, },
		{ VK_MENU,				BC_ALT, },
		{ VK_CONTROL,			BC_CONTROL, },
	};
	int buttonMapperSize = sizeof(buttonMapper) / sizeof(BUTTONMAPPER);

	for( int i = 0; i < buttonMapperSize; ++ i )
	{
		if( buttonMapper[ i ].bc == bc )
		{
			return buttonMapper[ i ].vk;
		}
	}

	return 0;
}


/***************************************************************************/
// Convert an abstract key action enum to an abstract action ini string
/***************************************************************************/
bool KeyControllerInputDriver::GetIniStringForAbstractAction( int abstractActionNumber, char* iniStringName ) const
{
	struct BUTTONMAPPER
	{
		char 			*name;
		int				abstractActionNumber;
	};
	BUTTONMAPPER buttonMapper[] =
	{
		{ "FIRST_PERSON",						BUTTONABSTRACT_FIRST_PERSON },
		{ "BUTTONABSTRACT_TAG",					BUTTONABSTRACT_TAG },
		{ "LEAN_LEFT",							BUTTONABSTRACT_LEAN_LEFT },
		{ "LEAN_RIGHT",							BUTTONABSTRACT_LEAN_RIGHT },
		{ "MOVE_FORWARD",						BUTTONABSTRACT_MOVE_FORWARD },
		{ "MOVE_BACKWARD",						BUTTONABSTRACT_MOVE_BACKWARD },
		{ "STRAFE_LEFT",						BUTTONABSTRACT_STRAFE_LEFT },
		{ "STRAFE_RIGHT",						BUTTONABSTRACT_STRAFE_RIGHT },
		{ "BUTTONABSTRACT_UNKNOWN_STICK1",		BUTTONABSTRACT_UNKNOWN_STICK1 },
		{ "LOOK_UP",							BUTTONABSTRACT_LOOK_UP },
		{ "LOOK_DOWN",							BUTTONABSTRACT_LOOK_DOWN },
		{ "TURN_LEFT",							BUTTONABSTRACT_TURN_LEFT },
		{ "TURN_RIGHT",							BUTTONABSTRACT_TURN_RIGHT },
		{ "ZOOM",								BUTTONABSTRACT_ZOOM },
		{ "JUMP",								BUTTONABSTRACT_JUMP },
		{ "BUTTONABSTRACT_UNKNOWN_O",			BUTTONABSTRACT_UNKNOWN_O },
		{ "RELOAD",								BUTTONABSTRACT_RELOAD },
		{ "USE",								BUTTONABSTRACT_USE },
		{ "CROUCH",								BUTTONABSTRACT_CROUCH },
		{ "FIRE",								BUTTONABSTRACT_FIRE },
		{ "BUTTONABSTRACT_PREVIOUS_WEAPON",		BUTTONABSTRACT_PREVIOUS_WEAPON },
		{ "NEXT_WEAPON",						BUTTONABSTRACT_NEXT_WEAPON },
		{ "BUTTONABSTRACT_START",				BUTTONABSTRACT_START },
		{ "BUTTONABSTRACT_SELECT",				BUTTONABSTRACT_SELECT },
		{ "WALK",								BUTTONABSTRACT_WALK },
		{ "CENTER_VIEW",						BUTTONABSTRACT_CENTER_VIEW },
	};
	int buttonMapperSize = sizeof(buttonMapper) / sizeof(BUTTONMAPPER);

	// Scan the list for the code
	for (int i = 0; i < buttonMapperSize; i++)
	{
		if( buttonMapper[i].abstractActionNumber == abstractActionNumber )
		{
			strcpy( iniStringName, buttonMapper[i].name );
			return true;
		}
	}

	strcpy( iniStringName, "UNKNOWN ABSTRACT ACTION" );
	return false;
}

u8 KeyControllerInputDriver::getKey()
{
	// return if we're not the active window
	if (GetActiveWindow() != DisplayState.hWnd)
		return 0;
	static unsigned char keyboardState[256];
	static unsigned char lastKeyboardState[256];
	memcpy(lastKeyboardState, keyboardState, 256*sizeof(unsigned char));
	GetKeyboardState(keyboardState);
	for (int c=0; c < 255; ++c)
	{
		// check for special keys we want
		if ( ( c < 32  && c != 8   )    )  // delete
			 continue;
		if (   ( c > 106 ) &&
			   (
			       c != VK_OEM_PERIOD &&
				   c != VK_OEM_COMMA  
				)
			)
			continue;
		if (
			    ( keyboardState[c] & 0x80 ) == 0x80 
			 && ( lastKeyboardState[c] & 0x80 ) != 0x80 
		    )
		{
			switch(c)
			{
				case (VK_LEFT):
				case (VK_RIGHT):
				case (VK_DOWN):
				case (VK_UP):
					return 0;
					break;
				case (VK_OEM_PERIOD):
					return '.';
				case (VK_OEM_COMMA):
					return ',';
				default:
					// hack to fix all caps all the time
					// bug that crept in
					if ( c > 64 && c < 91 )
						c+=32;
					// correct value for the number pad
					else if ( c > 95 && c < 106 )
						c-= 48;
					return c;
			}
		}
	}
	return 0;
}

#endif // DIRECTX_PC


