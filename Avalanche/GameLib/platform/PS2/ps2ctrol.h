/***************************************************************************/
// PS2 controller input
/***************************************************************************/
#ifndef PS2CTROL_H
#define PS2CTROL_H

// PS2 stuff
#include <eetypes.h>
#include <libpad.h>
#include <libpc.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <stdlib.h>

#include "platform/ctroller.h"

// Resolution of our transfer functions
#define TRANSFER_LOOKUPS 		256

class CMouseDriver;
class CKeyboard;

// An award for the longest class name perhaps?
class PS2ControllerInputDriver : public ControllerInputDriver
{
public:
	PS2ControllerInputDriver(int port, int slot = 0);
	~PS2ControllerInputDriver();

	// What is the device name of this controller, you have to worry about
	// foriegn versions of this
	const char *DeviceName();

	// Setup a stick axis
	bool SetupAxis(AnalogAxis axis,
						AxesTransferFunction transferFunction,
						float factor = 1.0f,
						float deadZonePercent = 20.0f,
						float clampZonePercent = 2.0f);

	// Are we currently connected?
	bool Connected();

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

	// Get the current vibration values. Returns false (and doesn't touch
	// intensity if there is no vibration
	bool GetVibration(uint &intensity);

	// The input system calls this to "refresh" input at frame start
	void BeginInput();

	// The input system calls this to "flush" input at frame end
	void DoneInput();

	u8 getKey( void );
	bool isShiftPressed( void );
	bool shiftReleased( void );
	bool isCapsLocked( void );
	bool capsUnLocked( void );
	bool isUpArrow( void );
	bool isDownArrow( void );
	bool isLeftArrow( void );
	bool isRightArrow( void );

private:
	// Our transfer functions
	float transferLookup[AA_AXES][TRANSFER_LOOKUPS];
	inline void TranslateAxis(AnalogAxis axis, unsigned char data, bool flip = false);
	inline void TranslatePressure(AnalogAxis axis, unsigned char data);

	// PS2 device ID
	int				port, slot;

	// Vibration support
	u32				currentIntensity;
	bool				motorsReady;
	u32				motorCutoff;

	// Digital button mappings
	enum DATA2BUTTON
	{
		BUTTON_LEFT = 0x80,
		BUTTON_DOWN = 0x40,
		BUTTON_RIGHT = 0x20,
		BUTTON_UP = 0x10,
		BUTTON_START = 0x08,
		BUTTON_R3 = 0x04,
		BUTTON_L3 = 0x02,
		BUTTON_SEL = 0x01
	};
	enum DATA3BUTTON
	{
		BUTTON_SQUARE = 0x80,
		BUTTON_X = 0x40,
		BUTTON_O = 0x20,
		BUTTON_TRIANGLE = 0x10,
		BUTTON_R1 = 0x08,
		BUTTON_L1 = 0x04,
		BUTTON_R2 = 0x02,
		BUTTON_L2 = 0x01
	};
	enum DATA3NEGCONBUTTON
	{
		NCBUTTON_A = 0x20,
		NCBUTTON_B = 0x10,
		NCBUTTON_R = 0x08,
		NCBUTTON_L = 0x04		// This could be wrong
	};
	void DecodeButtons(unsigned char data2, unsigned char data3);
	void ClearButtons();

	// Controller type
	enum CONTROLLER
	{
		CONTROLLER_UNKNOWN = -2,
		CONTROLLER_DISCONNECTED = -1,
		CONTROLLER_DIGITAL = 0x04,
		CONTROLLER_DUALSHOCK = 0x07,
		CONTROLLER_DUALSHOCK2 = 0x99,
		CONTROLLER_ANALOG = 0x05,
		CONTROLLER_NEGCON = 0x02,
		CONTROLLER_NAMCOGUN = 0x06
	};

	// Type inferred from state machine
	CONTROLLER	type;

	// Types from the read (can be more accurate)
	CONTROLLER	readType;
	CONTROLLER  lastReadType;

	// Initialize state machine
	enum PHASE
	{
		START = 0,
		READY = 99
	};
	int 	phase;
	void RedetectController(bool disconnected = false);
	bool DetectController();

	// Timing
	u32 Now();

	CMouseDriver				*mouse;
	CKeyboard					*keyboard;

};

/***************************************************************************/
/***************************************************************************/
inline void PS2ControllerInputDriver::TranslateAxis(
AnalogAxis axis,
unsigned char data,
bool flip)
{
	int value;
	if (flip)
		value = 255 - (int)data;
	else
		value = (int)data;
	stick[axis] = transferLookup[axis][value];
}

/***************************************************************************/
/***************************************************************************/
inline void PS2ControllerInputDriver::TranslatePressure(
AnalogAxis axis,
unsigned char data)
{
	stick[axis] = (float)data * (1.0f / 255.0f);
}

#endif
