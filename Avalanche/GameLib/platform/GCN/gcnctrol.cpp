/***************************************************************************/
// GCN controller input
/***************************************************************************/
#include "platform/PlatformPCH.h"

#include "EngineHelper/timer.h"

extern "C" {
extern u32 u32Milliseconds;
}

#define MOTOR_TRIGGER_VAL ( 256 * 256)

/***************************************************************************/
// setup everthing
/***************************************************************************/
GCNControllerInputDriver::GCNControllerInputDriver( int iPort, GCNSystemInputDriver *pGCNSID)
{
	// setup a default transfer function
	for (int a = 0; a < AA_AXES; a++)
	{
		SetupAxis( (AnalogAxis)a, ATF_LINEAR, 1, 15, 39);
	}

  	// pad open
	this->iPort = iPort;
	this->pGCNSID = pGCNSID;

	// Setup the motor stuff
	motorsReady = true;
	motorCutoff = 0;
	motorValue = 0;
	motorInc = 0;

	motorCommand = PAD_MOTOR_STOP;

	pGCNSID->SetMotorCommand( iPort, motorCommand);

	// Allow vibration by default
	allowVibration = true;
}

/***************************************************************************/
/***************************************************************************/
GCNControllerInputDriver::~GCNControllerInputDriver( void)
{
}

/***************************************************************************/
// What is the device name of this controller, you have to worry about
// foriegn versions of this	
/***************************************************************************/
const char *GCNControllerInputDriver::DeviceName( void)
{
	if (!Connected())
		return NULL;
	else
		return "Controller";
}

/***************************************************************************/
// Setup a stick axis
/***************************************************************************/
bool GCNControllerInputDriver::SetupAxis(
	AnalogAxis axis,
	AxesTransferFunction transferFunction,
	float factor,
	float deadZonePercent,
	float clampZonePercent)
{
	// Call the utility
	GenerateTransferFunction(	transferLookup[axis],
										TRANSFER_LOOKUPS,
										transferFunction,
										factor,
										deadZonePercent,
										clampZonePercent);
	return( true);
}

/***************************************************************************/
// Are we connected?
/***************************************************************************/
bool GCNControllerInputDriver::Connected(void)
{
	PADStatus *pPADStatus = pGCNSID->GetControllerData( iPort);
	return( pPADStatus->err == PAD_ERR_NONE || pPADStatus->err == PAD_ERR_TRANSFER);
}

/***************************************************************************/
// Activate the vibration/forcefeedback function of the controller
// Returns true/false based on controller's ability to vibrate
/***************************************************************************/
void GCNControllerInputDriver::Vibration(
	bool allow)
{
	allowVibration = allow;
}

/***************************************************************************/
// Activate the vibration/forcefeedback function of the controller
// Returns true/false based on controller's ability to vibrate
/***************************************************************************/
bool GCNControllerInputDriver::SetVibration(
	uint intensity,
	VIBRATEPAN pan)
{
	// If we're globally preventing vibration, bail.
	if( !allowVibration)
		return( false);

	// If we don't have motors, or they aren't yet initialized
	// return false
	if( !motorsReady)
		return( false);

	// Are we already in a timed vibration?
	if( motorCutoff != 0)
		return( false);

	// This overrides the time based function
	motorValue = 0;
	motorInc = intensity * intensity;
	motorCutoff = 0;

	return(true);
}

/***************************************************************************/
// Do the vibration for duration milliseconds, with the above intensity/pan
// values.
/***************************************************************************/
bool GCNControllerInputDriver::SetVibrationDuration(
	uint duration,
	uint intensity,
	VIBRATEPAN pan)
{
	// set the vibration going
	if( !SetVibration( intensity, pan))
	{
		return( false);
	}

	// Setup the cutoff time (duration is approximate, the actuators have spindown time)
	motorCutoff = u32Milliseconds + duration;
	return(true);
}

/***************************************************************************/
// Make it so there is no vibration
/***************************************************************************/
bool GCNControllerInputDriver::ClearVibration(
	bool force)
{
	// Force it off
	if( force)
	{
		motorCutoff = 0;
	}

	// Cut it off
	return( SetVibration( NOVIBRATION));
}

/***************************************************************************/
// The input system calls this to "refresh" input at frame start
/***************************************************************************/
void GCNControllerInputDriver::BeginInput( void)
{

	ControllerInputDriver::BeginInput( );

	PADStatus		*pPADStatus;

	pPADStatus = pGCNSID->GetControllerData( iPort);

	if( pPADStatus->err != PAD_ERR_NONE)
	{
		if( pPADStatus->err == PAD_ERR_NO_CONTROLLER)
		{
			pGCNSID->SetResetMask( iPort);
		}
		pPADStatus->stickX = 0;
		pPADStatus->stickY = 0;
		pPADStatus->substickX = 0;
		pPADStatus->substickY = 0;
		pPADStatus->button = 0;
		pPADStatus->triggerLeft = 0;
		pPADStatus->triggerRight = 0;
		pPADStatus->analogA = 0;
		pPADStatus->analogB = 0;
	}

	u16	allDPad = ( PAD_BUTTON_LEFT | PAD_BUTTON_RIGHT | PAD_BUTTON_UP | PAD_BUTTON_DOWN);

	TranslateAxis( AA_X1, pPADStatus->stickX);
	TranslateAxis( AA_Y1, pPADStatus->stickY);
	TranslateAxis( AA_X2, pPADStatus->substickX);
	TranslateAxis( AA_Y2, pPADStatus->substickY);

	// Decode the buttons into the state array
	SetState( CB_LEFT, pPADStatus->button & PAD_BUTTON_LEFT);
	SetState( CB_RIGHT, pPADStatus->button & PAD_BUTTON_RIGHT);
	SetState( CB_UP, pPADStatus->button & PAD_BUTTON_UP);
	SetState( CB_DOWN, pPADStatus->button & PAD_BUTTON_DOWN);

	SetState( CB_START, pPADStatus->button & PAD_BUTTON_MENU);
	SetState( CB_SELECT,	( pPADStatus->button & PAD_TRIGGER_Z) &&
								( pPADStatus->button & allDPad) &&
								!( pPADStatus->button & PAD_TRIGGER_L) &&
								!( pPADStatus->button & PAD_TRIGGER_R));

	SetState( CB_X, pPADStatus->button & PAD_BUTTON_A);
	SetState( CB_SQUARE, pPADStatus->button & PAD_BUTTON_B);
	SetState( CB_O, pPADStatus->button & PAD_BUTTON_X);
	SetState( CB_TRI, pPADStatus->button & PAD_BUTTON_Y);

	SetState( CB_Z,   pPADStatus->button & PAD_TRIGGER_Z );

	SetState( CB_R1,	( pPADStatus->button & PAD_TRIGGER_R) &&
							!( pPADStatus->button & PAD_TRIGGER_Z) &&
							!( pPADStatus->button & allDPad));
	SetState( CB_R2,	( pPADStatus->button & PAD_TRIGGER_R) &&
							( pPADStatus->button & PAD_TRIGGER_Z) &&
							!( pPADStatus->button & allDPad));
	SetState( CB_STICK2,	( pPADStatus->button & PAD_TRIGGER_R) &&
								!( pPADStatus->button & PAD_TRIGGER_Z) &&
								( pPADStatus->button & allDPad));

	SetState( CB_L1,	( pPADStatus->button & PAD_TRIGGER_L) &&
							!( pPADStatus->button & PAD_TRIGGER_Z) &&
							!( pPADStatus->button & allDPad));
	SetState( CB_L2,	( pPADStatus->button & PAD_TRIGGER_L) &&
							( pPADStatus->button & PAD_TRIGGER_Z) &&
							!( pPADStatus->button & allDPad));
	SetState( CB_STICK1,	( pPADStatus->button & PAD_TRIGGER_L) &&
								!( pPADStatus->button & PAD_TRIGGER_Z) &&
								( pPADStatus->button & allDPad));


	SetState( CB_RESET, false);

	// Simulate D-Pad
	ControllerInputDriver::TranslatePrimaryAxisToDigital();
}

/***************************************************************************/
// Was reset pressed?
/***************************************************************************/
bool GCNControllerInputDriver::ResetPressed(void)
{
	return( false);
}

/***************************************************************************/
// The input system calls this to "flush" input at frame end
/***************************************************************************/
void GCNControllerInputDriver::DoneInput( void)
{
	// cutoff the motor?
	if( motorCutoff != 0 && motorCutoff <= u32Milliseconds)
	{
		ClearVibration( true);
		motorCutoff = 0;
	}

	if( motorInc == 0)
	{
		if( motorCommand != PAD_MOTOR_STOP)
		{
			motorCommand = PAD_MOTOR_STOP;
			pGCNSID->SetMotorCommand( iPort, motorCommand);
		}
	}
	else
	{
		motorValue += motorInc;
		if( motorValue >= MOTOR_TRIGGER_VAL)
		{
			motorValue = 0;
			if( motorCommand != PAD_MOTOR_RUMBLE)
			{
				motorCommand = PAD_MOTOR_RUMBLE;
				pGCNSID->SetMotorCommand( iPort, motorCommand);
			}
		}
	}
}

