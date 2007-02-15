/***************************************************************************/
// System input driver
/***************************************************************************/
#ifndef SYSTEMI_H
#define SYSTEMI_H

#include "platform/BaseType.h"
#include "platform/ctroller.h"

// Maximum number of controllers we want to support
#define MAX_CONTROLLERS 	2

enum {
	CONTROLLER_NOT_LOCKED = -1,
};


typedef enum ButtonCode
{
	// letters
	BC_A,	BC_B,	BC_C,	BC_D,	BC_E,	BC_F,	BC_G,	BC_H,
	BC_I,	BC_J,	BC_K,	BC_L,	BC_M,	BC_N,	BC_O,	BC_P,
	BC_Q,	BC_R,	BC_S,	BC_T,	BC_U,	BC_V,	BC_W,	BC_X,
	BC_Y,	BC_Z,

	// Digits
	BC_0,	BC_1,	BC_2,	BC_3,	BC_4,	BC_5,	BC_6,	BC_7,
	BC_8,	BC_9,

	// symbols & stuff
	BC_SPACE, BC_TILDE, BC_MINUS,	BC_PLUS,	BC_LEFTBRACKET,
	BC_RIGHTBRACKET, BC_FORWARDSLASH, BC_SEMICOLON,
	BC_QUOTE, BC_PERIOD, BC_COMMA, BC_BACKSLASH,

	// Control
	BC_ESCAPE, BC_BACKSPACE, BC_TAB, BC_ENTER, BC_INSERT,
	BC_DELETE, BC_HOME, BC_END, BC_PAGEUP,	BC_PAGEDOWN,
	BC_UP, BC_DOWN, BC_LEFT, BC_CENTER,	BC_RIGHT,

	// Numpad
	BC_MULTIPLY, BC_ADD,	BC_SUBTRACT, BC_DIVIDE,

	// Function keys
	BC_F1, BC_F2, BC_F3,	BC_F4, BC_F5, BC_F6,
	BC_F7, BC_F8, BC_F9,

	// Mouse
	BC_MOUSELEFT, BC_MOUSEMIDDLE,	BC_MOUSERIGHT,
	BC_MOUSEWHEELUP, BC_MOUSEWHEELDOWN,

	// Modify keys
	BC_SHIFT, BC_CONTROL,
//#ifdef DIRECTX_PC
	BC_ALT,
//#endif

	// Always returns false
	BC_INVALID,

	// Total codes
	BC_BUTTONCODES
};

class SystemInputDriver
{
public:
	// Constructor
	SystemInputDriver() {}
	virtual ~SystemInputDriver() {}

	// Call this to beigin using input
	virtual void BeginInput() {}

	// Clear input counts, etc. (call at the end of processing)
	virtual void DoneInput() {}

	// Call this if you are just in a tight loop
	void UpdateInput()
		{ BeginInput(); DoneInput(); }

	// Has the hardware reset button been pressed?
	virtual bool HardwareResetPressed()
		{ return false; }

	// Have they touched the off button?
	virtual bool HardwarePowerDown()
		{ return false; }

	//------------------------
	// Work with the keybaord
	//------------------------

	// Is the keyboard connected?
	virtual bool KeyboardConnected()
		{ return false; }

	// Was this button pressed last frame?
	virtual bool ButtonPressed(ButtonCode button)
		{ return false; }

	virtual bool ButtonReleased(ButtonCode button)
		{ return false; }

	// Is this key currenly down?
	virtual bool ButtonDown(ButtonCode button)
		{ return false; }

	//------------------------
	// Work with the mouse
	//------------------------

	virtual bool MouseConnected()
		{ return false; }

	// Mouse position
	virtual int MouseX()
		{ return 0; }
	virtual int MouseY()
		{ return 0; }

#ifdef DIRECTX_PC
    virtual bool wasMouseMoved()
        { return false; }
#endif

	//------------------------------------------------------
	// Aquire/use a hardware controller
	//------------------------------------------------------

	// How many controlles are there?
	virtual uint Controllers() = 0;

	// Get a controller interface (these pointers are "static", you don't delete them)
	virtual ControllerInputDriver *GetController(uint player) = 0;

	virtual void UnlockAllControllers(void) {}
	virtual void LockPlayerToController(uint player, int controllerNum) {}
	virtual void UnlockPlayerFromController(uint playerNum) {}
	virtual bool ControllerLocked(uint player)
		{ return true;}
	virtual bool UnmappedControllerLocked(uint controllerNum)
		{ return true;}
	virtual int GetLockedControllerIndex(uint player)
		{ return (int)player; }
	virtual int GetFirstUnlockedControllerIndex(void)
		{ return -1; } 
	virtual ControllerInputDriver *GetUnlockedController(uint player)
		{ return GetController(player); }
	virtual ControllerInputDriver *GetAssertController(void)
		{ return GetController(0); }
	virtual ControllerInputDriver *GetFirstUnlockedController(void)
		{ return NULL;}
	virtual uint GetNumLockedControllers(void)
		{ return Controllers();}
};

// Our singleton
extern SystemInputDriver *g_inputPtr;
inline SystemInputDriver &SafePtr(SystemInputDriver *ptr)
{
	ASSERT(ptr);
	return(*ptr);
}
#define g_input SafePtr(g_inputPtr)

#endif
