/***************************************************************************/
// GCN controller input
/***************************************************************************/
#ifndef GCNCTROL_H
#define GCNCTROL_H

#include <dolphin/pad.h>

#include "platform/ctroller.h"

class GCNSystemInputDriver;

// Resolution of our transfer functions
#define TRANSFER_LOOKUPS 		256

// An award for the longest class name perhaps?
class GCNControllerInputDriver : public ControllerInputDriver
{
public:
	GCNControllerInputDriver( int iPort, GCNSystemInputDriver *pGCNSID);
	~GCNControllerInputDriver( );

	// What is the device name of this controller, you have to worry about
	// foriegn versions of this	
	const char *DeviceName(void);

	// Setup a stick axis
	bool SetupAxis(AnalogAxis axis,
						AxesTransferFunction transferFunction,
						float factor = 1.0f,
						float deadZonePercent = 20.0f,
						float clampZonePercent = 2.0f);

	// Are we connected?
	bool Connected(void);

	// Turns vibration on/off globally.
	void Vibration( bool allow);

	// Activate the vibration/forcefeedback function of the controller
	// Intensity is specified VIBRATEINTENSITY
	// pan is specified using VIBRATEPAN.
	// Returns true/false based on controller's ability to vibrate
	bool SetVibration( uint intensity, VIBRATEPAN pan = CENTER);

	// Do the vibration for duration milliseconds, with the above intensity/pan
	// values.
	bool SetVibrationDuration( uint duration, uint intensity, VIBRATEPAN pan = CENTER);

	// Make it so there is no vibration
	bool ClearVibration( bool force = false);

	// The input system calls this to "refresh" input at frame start
	void BeginInput( );

	// The input system calls this to "flush" input at frame end
	void DoneInput( );

	bool ResetPressed( );

private:
	// Our transfer functions
	float transferLookup[AA_AXES][TRANSFER_LOOKUPS];

	inline void TranslateAxis( AnalogAxis axis, s8 s8Value)
	{
		stick[axis] = transferLookup[axis][( (int)s8Value) + 128];
	}

	// GCN stuff
	int							iPort;
	GCNSystemInputDriver		*pGCNSID;

	// Vibration suppot
	bool							motorsReady;
	u32							motorCutoff;
	u32							motorValue;
	u32							motorInc;
	u32							motorCommand;
};

#endif
