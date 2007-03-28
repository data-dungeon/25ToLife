 /***************************************************************************/
// PS2 controller input
/***************************************************************************/
#include "platform/PlatformPCH.h"
#include "platform/PS2/PS2Mouse.h"
#include "platform/PS2/PS2Keyboard.h"

// This HAS to be aligned on a 64 byte boundary,
// Assume that scePadDmaBufferMax * sizeof(u_long128) yields an aligned address
typedef struct
{
	u_long128		buff[scePadDmaBufferMax];
	unsigned char 	actDirect[6];
	unsigned char 	actAlign[6];
} padDMA;

#define MAX_PORTS		2
#define MAX_SLOTS		4
padDMA padBuffer[MAX_PORTS][MAX_SLOTS]	__attribute__((aligned(64)));

/***************************************************************************/
// setup everthing
/***************************************************************************/
PS2ControllerInputDriver::PS2ControllerInputDriver(
int port,
int slot)
{
	// setup a default transfer function
	for (int a = 0; a < AA_AXES; a++)
		SetupAxis((AnalogAxis)a, ATF_LINEAR, 1, 32, 4);

  	// pad open
	this->port = port;
	this->slot = slot;

	// Setup the motor stuff
	motorsReady = false;
	motorCutoff = 0;
	currentIntensity = 0;
	for (uint i = 0; i < 6; i++)
	{
		padBuffer[port][slot].actDirect[i] = 0;
		padBuffer[port][slot].actAlign[i] = 0xff;
	}
	padBuffer[port][slot].actAlign[0] = 0;
	padBuffer[port][slot].actAlign[1] = 1;

	// Open the pad with the dma buffer
	u_long128 *dmaBuff = padBuffer[port][slot].buff;
 	scePadPortOpen(port, slot, dmaBuff);

	// Phase of initialization
	RedetectController();

	// --- DAS, new mouse support
#if 1
#if defined(NETWORKING)
	mouse		= new CPlatformMouse( );
	mouse->initialize( );
	keyboard = new CPlatformKeyboard( );
	keyboard->initialize( );
#endif
#else
	mouse = NULL;
	keyboard = NULL;
#endif

}

/***************************************************************************/
/***************************************************************************/
PS2ControllerInputDriver::~PS2ControllerInputDriver()
{
	scePadPortClose(port, slot);
#if defined(NETWORKING)
	if( keyboard )
	{
		delete keyboard;
		keyboard = NULL;
	}
	if( mouse )
	{
		delete mouse;
		mouse = NULL;
	}
#endif
}

/***************************************************************************/
// What is the device name of this controller, you have to worry about
// foriegn versions of this
/***************************************************************************/
const char *PS2ControllerInputDriver::DeviceName()
{
	// Allow this to update
	DetectController();

	// Which type to use
	CONTROLLER realType;
	if (readType >= 0)
		realType = readType;
	else
		realType = type;

	switch (realType)
	{
		case CONTROLLER_DIGITAL:
			return "Controller";
			break;

		case CONTROLLER_DUALSHOCK:
			return "Dualshock";
			break;

		case CONTROLLER_DUALSHOCK2:
			return "Dualshock 2";
			break;

		case CONTROLLER_ANALOG:
			return "Analog";
			break;

		case CONTROLLER_NEGCON:
			return "NeGcon";
			break;

		case CONTROLLER_NAMCOGUN:
			return "Namco Gun";
			break;

		case CONTROLLER_DISCONNECTED:
			return NULL;
			break;

		default:
		case CONTROLLER_UNKNOWN:
			return "Unknown";
			break;
	}
}

/***************************************************************************/
// Setup a stick axis
/***************************************************************************/
bool PS2ControllerInputDriver::SetupAxis(
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
	return true;
}

/***************************************************************************/
// Are we currently connected?
/***************************************************************************/
bool PS2ControllerInputDriver::Connected()
{
	DetectController();
	return (type != CONTROLLER_DISCONNECTED);
}

/***************************************************************************/
// Activate the vibration/forcefeedback function of the controller
// Returns true/false based on controller's ability to vibrate
/***************************************************************************/
bool PS2ControllerInputDriver::SetVibration(
uint intensity,
VIBRATEPAN pan)
{
#ifdef _DEBUG
	// I've had it with vibrating controls in debug builds...(food)
	return true;
#else
	// If we're globally preventing vibration, bail.
	if (!allowVibration)
		return false;

	// If we don't have motors, or they aren't yet initialized
	// return false
	if (!motorsReady)
		return false;

	// Are we already in a timed vibration?
	if (motorCutoff != 0)
		return false;

	// Save the data
	currentIntensity = intensity;

	// Alias the direct buffer
	unsigned char *actDirect = padBuffer[port][slot].actDirect;
	unsigned char oldActDirect[2];
	oldActDirect[0] = actDirect[0];
	oldActDirect[1] = actDirect[1];

	// Set the high frequency bastard
	actDirect[0] = (intensity & BUZZ) ? 1 : 0;

	// Setup the low frequency one
	actDirect[1] = (unsigned char)(intensity & 0xff);

	// Make sure we aren't flooding the DMA
	if (actDirect[0] == oldActDirect[0] && actDirect[1] == oldActDirect[1])
		return true;

	// Do it!
	scePadSetActDirect(port, slot, actDirect);

	// This overrides the time based function
	motorCutoff = 0;

	return true;
#endif
}

/***************************************************************************/
// Do the vibration for duration milliseconds, with the above intensity/pan
// values.
/***************************************************************************/
bool PS2ControllerInputDriver::SetVibrationDuration(
uint duration,
uint intensity,
VIBRATEPAN pan)
{
	// set the vibration going
	if (!SetVibration(intensity, pan))
		return false;

	// Setup the cutoff time (duration is approximate, the actuators have spindown time)
	motorCutoff = Now() + (duration >> 1);
	return true;
}

/***************************************************************************/
// Make it so there is no vibration
/***************************************************************************/
bool PS2ControllerInputDriver::ClearVibration(
bool force)
{
	// Force it off
	if (force)
		motorCutoff = 0;

	// Cut it off
	return(SetVibration(NOVIBRATION));
}

/***************************************************************************/
// Get the current vibration values. Returns false (and doesn't touch
// intensity if there is no vibration
/***************************************************************************/
bool PS2ControllerInputDriver::GetVibration(
uint &intensity)
{
	if (currentIntensity == 0)
		return false;

	intensity = currentIntensity;
	return true;
}

/***************************************************************************/
// The input system calls this to "refresh" input at frame start
/***************************************************************************/
void PS2ControllerInputDriver::BeginInput()
{
	// Process the pad's state
	if (!DetectController())
	{
		ClearButtons();
		return;
	}

	ASSERT(phase == READY);

	// Requires 32 bytes for biggest controller
	static unsigned char data[32] __attribute__((aligned(64)));

	static int padStatus = scePadGetState( port, slot );
	ASSERTS(padStatus == scePadStateFindCTP1 || padStatus == scePadStateStable, "pad communication error");

	// read joypad
	scePadRead(port, slot, data);

	// Did the read work? (retains last state)
	if (data[0] != 0)
		return;

	// what type are we reading from?
	lastReadType = readType;
	readType = (CONTROLLER)(data[1] >> 4);

	// Special "tester" case, they spazed
	if (type == CONTROLLER_DIGITAL && readType == CONTROLLER_DUALSHOCK)
	{
		TRACE("Cut the testers off from caffiene!!!!!!\n");
		RedetectController();
		return;
	}

	// Weird promotion
	if (readType == CONTROLLER_DUALSHOCK && type == CONTROLLER_DUALSHOCK2)
		readType = type;

	// Whoa this would be weird, but might be happening
	if ((int)lastReadType >= 0 && lastReadType != readType)
	{
		TRACE("Controller type changed without disconnect state!!!!!!\n");
		RedetectController();
		return;
	}

	// Call the parent
	ControllerInputDriver::BeginInput();

	// This is special, PS2 CDROM builds CAN ONLY HAVE DualShock2 support
	bool validController = true;
#ifdef CDROM
	// Which type to use
	CONTROLLER realType = (readType >= 0) ? readType : realType = type;
	validController = (realType == CONTROLLER_DUALSHOCK2);
#endif

	// If we don't have a valid controller, clear the state
	if (!validController)
		ClearButtons();
	else
	{
		// Decode the button state
		DecodeButtons(data[2], data[3]);

		// Decifer the analog sticks
		switch (readType)
		{
			case CONTROLLER_DIGITAL:
				// nohthing to do here...
				break;

			case CONTROLLER_DUALSHOCK:
				TranslateAxis(AA_X1, data[6]);
				TranslateAxis(AA_Y1, data[7], true);
				TranslateAxis(AA_X2, data[4]);
				TranslateAxis(AA_Y2, data[5], true);
				break;

			case CONTROLLER_DUALSHOCK2:
				TranslateAxis(AA_X1, data[6]);
				TranslateAxis(AA_Y1, data[7], true);
				TranslateAxis(AA_X2, data[4]);
				TranslateAxis(AA_Y2, data[5], true);
				TranslatePressure(AA_RIGHT, data[8]);
				TranslatePressure(AA_LEFT, data[9]);
				TranslatePressure(AA_UP, data[10]);
				TranslatePressure(AA_DOWN, data[11]);
				TranslatePressure(AA_TRI, data[12]);
				TranslatePressure(AA_O, data[13]);
				TranslatePressure(AA_X, data[14]);
				TranslatePressure(AA_SQUARE, data[15]);
				TranslatePressure(AA_L1, data[16]);
				TranslatePressure(AA_R1, data[17]);
				TranslatePressure(AA_L2, data[18]);
				TranslatePressure(AA_R2, data[19]);
				break;

			case CONTROLLER_ANALOG:
				TranslateAxis(AA_X1, data[6]);
				TranslateAxis(AA_Y1, data[7]);
				TranslateAxis(AA_X2, data[4]);
				TranslateAxis(AA_Y2, data[5]);
				break;

			case CONTROLLER_NEGCON:
	  			TranslateAxis(AA_X1, data[4]);
				TranslateAxis(AA_Y1, data[7]);
				TranslateAxis(AA_X2, data[4]);
				TranslateAxis(AA_Y2, data[5]);
				break;
		}
	}
#if defined(NETWORKING)
	// --- mouse input?
	if (false && mouse && mouse->isMouseLoaded( ))
	{
		if( mouse->beginInput( ) )
		{
			int x = mouse->getMouseX( );
			int y = mouse->getMouseY( );
			int button = mouse->getMouseButton( );
			if( x != 0 )
			{
				int sx = Math::Sign( x );
				int absx = Math::Abs( x );
				if( absx > 52 )
				{
					x *= ( sx * x - 52 );
				}
				x += ( sx * 42 );
				x = Math::Clamp( x, -127, 127 );
				x += 128;
			}
			if( y != 0 )
			{
				int sy = Math::Sign( y );
				int absy = Math::Abs( y );
				if( absy > 52 )
				{
					y *= ( sy * y - 52 );
				}
				y += ( sy * 42 );
				y = Math::Clamp( y, -127, 127 );
				y += 128;
			}

			// --- look to replace joystick controller data
			if( x != 0 )
				TranslateAxis( AA_X2, x );
			if( y != 0 )
				TranslateAxis( AA_Y2, -y );
			if( button & 0x01 )
			{
				TranslatePressure( AA_R1, 0xff );
				SetState( CB_R1, BUTTON_R1 );
			}
			if( button & 0x02 )
			{
				TranslatePressure( AA_R2, 0xff );
				SetState( CB_R2, BUTTON_R2 );
			}
		}
	}

	// --- keyboard input?
	if( keyboard && keyboard->isKeyboardActive( ) )
	{
		keyboard->beginInput( );

		// --- look for keys
#if 0
		if( keyboard->isKeyPressed( 'a' ) )
		{
			TranslateAxis( AA_X1, 0x00 );
		}
		if( keyboard->isKeyPressed( 'd' ) )
		{
			TranslateAxis( AA_X1, 0xff );
		}
		if( keyboard->isKeyPressed( 'w' ) )
		{
			TranslateAxis( AA_Y1, 0xff );
		}
		if( keyboard->isKeyPressed( 's' ) )
		{
			TranslateAxis( AA_Y1, 0x00 );
		}
#endif
	}
#endif
}

/***************************************************************************/
// Retrieve the last key pressed on the keyboard
/***************************************************************************/
u8 PS2ControllerInputDriver::getKey( void )
{
#if defined(NETWORKING)
	if( keyboard && keyboard->isKeyboardActive( ) )
	{
		return( keyboard->getChar( ) );
	}
#endif
	return( 0 );
}

/***************************************************************************/
// Retrieve the status of the shift key on the keyboard
/***************************************************************************/
bool PS2ControllerInputDriver::isShiftPressed( void )
{
#if defined(NETWORKING)
	if( keyboard && keyboard->isKeyboardActive() )
	{
		return( keyboard->isShiftPressed( ) );
	}
#endif
	return false;
}

/***************************************************************************/
// Retrieve the status of the shift key on the keyboard
/***************************************************************************/
bool PS2ControllerInputDriver::shiftReleased( void )
{
#if defined(NETWORKING)
	if( keyboard && keyboard->isKeyboardActive() )
	{
		return( keyboard->shiftReleased( ) );
	}
#endif
	return false;
}

/***************************************************************************/
// Retrieve the status of the caps key on the keyboard
/***************************************************************************/
bool PS2ControllerInputDriver::isCapsLocked( void )
{
#if defined(NETWORKING)
	if( keyboard && keyboard->isKeyboardActive() )
	{
		return( keyboard->isCapsLocked( ) );
	}
#endif
	return false;
}

/***************************************************************************/
// Retrieve the status of the caps key on the keyboard
/***************************************************************************/
bool PS2ControllerInputDriver::capsUnLocked( void )
{
#if defined(NETWORKING)
	if( keyboard && keyboard->isKeyboardActive() )
	{
		return( keyboard->capsUnLocked( ) );
	}
#endif
	return false;
}

/***************************************************************************/
// Retrieve the status of the arrow keys on the keyboard
/***************************************************************************/
bool PS2ControllerInputDriver::isUpArrow( void )
{
#if defined(NETWORKING)
	if( keyboard && keyboard->isKeyboardActive() )
	{
		return( keyboard->isUpArrow() );
	}
#endif
	return false;
}

bool PS2ControllerInputDriver::isDownArrow( void )
{
#if defined(NETWORKING)
	if( keyboard && keyboard->isKeyboardActive() )
	{
		return( keyboard->isDownArrow() );
	}
#endif
	return false;
}

bool PS2ControllerInputDriver::isLeftArrow( void )
{
#if defined(NETWORKING)
	if( keyboard && keyboard->isKeyboardActive() )
	{
		return( keyboard->isLeftArrow() );
	}
#endif
	return false;
}

bool PS2ControllerInputDriver::isRightArrow( void )
{
#if defined(NETWORKING)
	if( keyboard && keyboard->isKeyboardActive() )
	{
		return( keyboard->isRightArrow() );
	}
#endif
	return false;
}

/***************************************************************************/
// The input system calls this to "flush" input at frame end
/***************************************************************************/
void PS2ControllerInputDriver::DoneInput()
{
	// cutoff the motor?
	if (motorCutoff != 0 && motorCutoff <= Now())
	{
		ClearVibration(true);
		motorCutoff = 0;
	}
}

/***************************************************************************/
// clear the button state array.  Necessary when the controller is removed,
// so that we aren't still reporting that there are buttons down when there
// is no controller
/***************************************************************************/
void PS2ControllerInputDriver::ClearButtons()
{
	// set the states
	SetState(CB_LEFT, false);
	SetState(CB_DOWN, false);
	SetState(CB_RIGHT, false);
	SetState(CB_UP, false);
	SetState(CB_START, false);
	SetState(CB_SELECT, false);
	SetState(CB_STICK1, false);
	SetState(CB_STICK2, false);

	// Do something about NegCon?
	SetState(CB_SQUARE, false);
	SetState(CB_X, false);
	SetState(CB_O, false);
	SetState(CB_TRI, false);
	SetState(CB_R1, false);
	SetState(CB_L1, false);
	SetState(CB_R2, false);
	SetState(CB_L2, false);
}

/***************************************************************************/
// Decode the buttons into the state array
/***************************************************************************/
void PS2ControllerInputDriver::DecodeButtons(
unsigned char data2,
unsigned char data3)
{
	// Flip them
	data2 ^= 0xff;
	data3 ^= 0xff;

	// set the states
	SetState(CB_LEFT, data2 & BUTTON_LEFT);
	SetState(CB_DOWN, data2 & BUTTON_DOWN);
	SetState(CB_RIGHT, data2 & BUTTON_RIGHT);
	SetState(CB_UP, data2 & BUTTON_UP);
	SetState(CB_START, data2 & BUTTON_START);
	SetState(CB_SELECT, data2 & BUTTON_SEL);
	SetState(CB_STICK1, data2 & BUTTON_L3);
	SetState(CB_STICK2, data2 & BUTTON_R3);

	// Do something about NegCon?
	SetState(CB_SQUARE, data3 & BUTTON_SQUARE);
	SetState(CB_X, data3 & BUTTON_X);
	SetState(CB_O, data3 & BUTTON_O);
	SetState(CB_TRI, data3 & BUTTON_TRIANGLE);
	SetState(CB_R1, data3 & BUTTON_R1);
	SetState(CB_L1, data3 & BUTTON_L1);
	SetState(CB_R2, data3 & BUTTON_R2);
	SetState(CB_L2, data3 & BUTTON_L2);
	//printf("data2,3 %x,%x\n", data2, data3);
}

/***************************************************************************/
// Force a redetect
/***************************************************************************/
void PS2ControllerInputDriver::RedetectController(
bool disconnected)
{
	phase = START;

	if (disconnected)
		type = CONTROLLER_DISCONNECTED;
	else
		type = CONTROLLER_UNKNOWN;

	readType = type;
}

/***************************************************************************/
// Process the initialize
/***************************************************************************/
bool PS2ControllerInputDriver::DetectController()
{
	// get the state
 	int state = scePadGetState(port, slot);

	// No pad?
	if (state == scePadStateDiscon || state == scePadStateError)
	{
		// reset the state machine
		RedetectController(true);
		return false;
	}

	int id, extID;
	switch (phase)
	{
		case START:
			type = CONTROLLER_UNKNOWN;

			if (state != scePadStateStable && state != scePadStateFindCTP1)
				break;

			id = scePadInfoMode(port, slot, InfoModeCurID, 0);
			if (id == 0)
				break;

			extID = scePadInfoMode(port, slot, InfoModeCurExID, 0);
			if (extID > 0)
				id = extID;

			switch(id)
			{
				case 4: // Digital controller
					phase = 40; // Try for analog(dualshock)
					break;

				case 7: // Dualshock2 controller
					phase = 70;
					break;

				default:
					phase = READY;
					break;
			}
			break;

		// Analog Controller (old dualshock)
		case 40: // Analog Contoller check valid (otherwise fail phase)
			if (scePadInfoMode(port, slot,InfoModeCurExID, 0) == 0)
			{
				type = CONTROLLER_DIGITAL; // Digital pad
				phase = READY;
				break;
			}
			phase++;

		case 41: // Analog controller: Request Lock analog mode (asynchronous)
			if (scePadSetMainMode(port, slot, 1, 3) == 1)
				phase++;
			break;

		case 42: // Analog controller: Check state of previous request
			if (scePadGetReqState(port, slot)==scePadReqStateFaild)
				phase--;
			if (scePadGetReqState(port, slot)==scePadReqStateComplete)
			{
				// Lock mode complete
				phase = START; // Accept normal dualshock
			}
			break;

		// DualShock 2 Controller
	 	case 70: // Set motors
			if (scePadInfoAct(port, slot, -1, 0) == 0)
			{
				phase = READY;
				break;
			}

			// Asynchronous function
			if (scePadSetActAlign(port, slot, padBuffer[port][slot].actAlign) == 0)
			{
				ASSERTS(0, "shellPad: Set actAlign failed!!!!!!!!!!!!\n");
				break;
			}

			phase++;
			if (scePadGetReqState(port, slot) != scePadReqStateBusy)
			{
				ASSERTS(0, "shellPad: Set actAlign warning!\n");
			}
			break;

		case 71: // Check motors have been set
			if (scePadGetReqState(port, slot) == scePadReqStateFaild)
				phase--;

			if (scePadGetReqState(port, slot) == scePadReqStateComplete)
			{
				motorsReady = true;
				phase++;
			}
			break;

	 	case 72: // DualShock 2 check pressure sensitive possible
			if (scePadInfoPressMode(port, slot) == 1)
			{
				phase = 76;
				break;
			}
			type = CONTROLLER_DUALSHOCK;
			phase = READY;
			break;

 		case 76: // DualShock2 enable pressure sensitive mode (asynchronous function)
			if (scePadEnterPressMode(port, slot) == 1)
				phase++;
			break;

 		case 77: // Dualshock2 check status of request pressure sensitive mode
			if (scePadGetReqState(port, slot) == scePadReqStateFaild)
				phase--;
			if (scePadGetReqState(port, slot) == scePadReqStateComplete)
			{
				phase = READY;
				type = CONTROLLER_DUALSHOCK2;
			}
			break;
	}

	// Can they read?
	return (phase == READY && (state == scePadStateStable || state == scePadStateFindCTP1));
}

/***************************************************************************/
// Handy timer
/***************************************************************************/
extern "C" u32 u32Milliseconds;
u32 PS2ControllerInputDriver::Now()
{
	return u32Milliseconds;
}
