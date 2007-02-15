/***************************************************************************/
// Keyboard controller driver
/***************************************************************************/
#ifndef KEYCTRL_H
#define KEYCTRL_H

#include "platform/ctroller.h"
#include "platform/systemi.h"

#ifdef DIRECTX_PC
#define BINDING_CONFLICT_STRING_LENGTH 256
#endif

class KeyControllerInputDriver : public ControllerInputDriver
{
public:
	KeyControllerInputDriver(SystemInputDriver &inputDriver, const char *buttonMapName = 0);
	~KeyControllerInputDriver(void) {}

	// What is the device name of this controller, you have to worry about
	// foriegn versions of this	
	const char *DeviceName()
		{ return "Keyboard"; }

	// Setup a stick axis
	bool SetupAxis(AnalogAxis axis,
						AxesTransferFunction transferFunction,
						float factor,
						float deadZonePercent,
						float clampZonePercent);

	// Is the controller currently connected?
	bool Connected(void);

	// The input system calls this to "refresh" input at frame start
	void BeginInput(void);

	// The input system calls this to "flush" input at frame end
	void DoneInput(void);

private:
	SystemInputDriver	&input;

#ifdef DIRECTX_PC
	float				mouseSensitivity;
	float				keyboardSensitivity;
#endif	

	typedef enum
		{
		DPAD_UP = 0,
		DPAD_DOWN,
		DPAD_LEFT,
		DPAD_RIGHT,

		STICK1_UP,
		STICK1_DOWN,
		STICK1_LEFT,
		STICK1_RIGHT,
		BUTTON_STICK1,

		STICK2_UP,
		STICK2_DOWN,
		STICK2_LEFT,
		STICK2_RIGHT,
		BUTTON_STICK2,

		BUTTON_X,
		BUTTON_O,
		BUTTON_TRI,
		BUTTON_SQUARE,

		BUTTON_L1,
		BUTTON_R1,
		BUTTON_L2,
		BUTTON_R2,

		BUTTON_START,
		BUTTON_SELECT,

#ifdef DIRECTX_PC
        BUTTON_WALK,
        BUTTON_CENTERVIEW,
		BUTTON_FIRSTPERSON,
#endif

		BUTTONMAP_ENTRIES
		} ButtonMap;
	ButtonCode buttonMap[BUTTONMAP_ENTRIES];

#ifdef DIRECTX_PC
	// Enum equivalents of ButtonMap enums, named as action-abstractions rather than buttons
	enum
	{
		BUTTONABSTRACT_FIRST_PERSON			= BUTTON_FIRSTPERSON,
		BUTTONABSTRACT_TAG					= DPAD_DOWN,
		BUTTONABSTRACT_LEAN_LEFT			= DPAD_LEFT,
		BUTTONABSTRACT_LEAN_RIGHT			= DPAD_RIGHT,
		BUTTONABSTRACT_MOVE_FORWARD			= STICK1_UP,
		BUTTONABSTRACT_MOVE_BACKWARD		= STICK1_DOWN,
		BUTTONABSTRACT_STRAFE_LEFT			= STICK1_LEFT,
		BUTTONABSTRACT_STRAFE_RIGHT			= STICK1_RIGHT,
		BUTTONABSTRACT_UNKNOWN_STICK1		= BUTTON_STICK1,
		BUTTONABSTRACT_LOOK_UP				= STICK2_UP,
		BUTTONABSTRACT_LOOK_DOWN			= STICK2_DOWN,
		BUTTONABSTRACT_TURN_LEFT			= STICK2_LEFT,
		BUTTONABSTRACT_TURN_RIGHT			= STICK2_RIGHT,
		BUTTONABSTRACT_ZOOM					= BUTTON_STICK2,
		BUTTONABSTRACT_JUMP					= BUTTON_X,
		BUTTONABSTRACT_UNKNOWN_O			= BUTTON_O,
		BUTTONABSTRACT_RELOAD				= BUTTON_SQUARE,
		BUTTONABSTRACT_USE					= BUTTON_O,
		BUTTONABSTRACT_CROUCH				= BUTTON_L2,
		BUTTONABSTRACT_FIRE					= BUTTON_R1,
		BUTTONABSTRACT_PREVIOUS_WEAPON		= BUTTON_TRI,
		BUTTONABSTRACT_NEXT_WEAPON			= BUTTON_R2,
		BUTTONABSTRACT_START				= BUTTON_START,
		BUTTONABSTRACT_SELECT				= BUTTON_SELECT,
        BUTTONABSTRACT_WALK					= BUTTON_WALK,
        BUTTONABSTRACT_CENTER_VIEW			= BUTTON_CENTERVIEW,
		BUTTONABSTRACT_THROW_GRENADE		= BUTTON_L1,
		BUTTONABSTRACT_TAUNT				= DPAD_UP,
		BUTTONABSTRACT_NUM_ENTRIES			= BUTTONMAP_ENTRIES,
	};// AbstractButtonAction;

	/*virtual*/ u8 getKey( void );
	/*virtual*/ bool isShiftPressed( void ) { return (GetKeyState(VK_SHIFT) & 0x8000) != 0; }
	/*virtual*/ bool shiftReleased( void ) { return (GetKeyState(VK_SHIFT) & 0x8000) == 0; }
	/*virtual*/ bool isCapsLocked( void ) { return (GetKeyState(VK_CAPITAL) & 1) == 1; }
	/*virtual*/ bool capsUnLocked( void ) { return (GetKeyState(VK_CAPITAL) & 1) == 0; }

	// write a changed buttonmap to the HD
	// hard code which map to load right now
	bool WriteButtonMap(char * buttonMapLocation = NULL, char * errorString = NULL);

	// write a "default" keymap to the ini file
	void WriteDefaultButtonMap(char * buttonMapLocation = NULL);

#endif // DIRECTX_PC


	// Turn a button map into a key code
	inline ButtonCode Button(ButtonMap map)
		{ return(buttonMap[map]); }

	// Load in a button map
	bool LoadButtonMap(const char *buttonMap);

	// Map a button
	void MapButton(const char *buttonMap, char *tag, ButtonCode &button, ButtonCode defaultCode);

	// Convert a string to a button code
	bool StringToButtonCode(const char *string, ButtonCode &code);

#ifdef DIRECTX_PC
	// make the pc keybinding class a friend
	friend class ScreenPCKeyBindings;
	friend class MarqueeInstrument;
	friend class ScreenBaseClass;
	friend class WinMemCard;

public:
//	bool ButtonCodeToString( ButtonCode code, char* string ) const;
	char* ButtonCodeToString( ButtonCode code ) const;
	bool ButtonMapToString( int abstractActionNumber, char* uiStringName ) const;
	const char* ButtonMapToDescription( int abstractActionNumber ) const;
	bool GetIniStringForAbstractAction( int abstractActionNumber, char* iniStringName ) const;
	bool WasButtonPressedThisFrame( const ControllerButton& cb ) const;
	int GetBCForVK( int vkCode ) const;
	int GetVKForBC( ButtonCode bc ) const;
	void BindBCToAbstractAction( ButtonCode bc, int abstractActionNumber );
	const char* GetIniStringForAbstractAction( int abstractActionNumber ) const;
	const char* DigitalButtonToKey(int button) const;
private:

	// Set analog data
	void SetAnalog(float &stick, float amt, ButtonCode buttonPositive, ButtonCode buttonNegative);
#else
	void SetAnalog(float &stick, ButtonCode buttonPositive, ButtonCode buttonNegative);
#endif
	// Set the button state
	void SetState(ControllerButton cb, ButtonCode button);
};


#endif
