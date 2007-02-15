/***************************************************************************/
// Controller driver
/***************************************************************************/
#include "platform/PlatformPCH.h"

/***************************************************************************/
// Initialize eveything
/***************************************************************************/
ControllerInputDriver::ControllerInputDriver()
{
	uint i;

	// Init the sticks
	for (i = 0; i < AA_AXES; i++)
		stick[i] = 0.0f;

	// Init the button states
	for (i = 0; i < CB_BUTTONS; i++)
	{
		state[i].last = false;
		state[i].now = false;
	}

	// Vibration!
	allowVibration = true;

	// Headset
	m_headSetStatus = REMOVED;
	m_headSetPort = -1;
}

/***************************************************************************/
// Reset all button pressed states
/***************************************************************************/
void ControllerInputDriver::ClearPressed()
{
	for (int i = 0; i < CB_BUTTONS; i++)
		state[i].now = state[i].last = false;
}

/***************************************************************************/
// Force an update of the current state into the previous state
/***************************************************************************/
void ControllerInputDriver::UpdatePressed()
{
	for (int i = 0; i < CB_BUTTONS; i++)
		state[i].last = state[i].now;
}

/***************************************************************************/
// Queries the current state of the button hardware. Does not affect
// ButtonPressed, ButtonReleased, etc.
/***************************************************************************/
bool ControllerInputDriver::ButtonPeek(
ControllerButton button)
{
	return false;
}

/***************************************************************************/
// Was any button pressed?
/***************************************************************************/
bool ControllerInputDriver::AnyButtonPressed()
{
	return (ButtonPressed(CB_X) ||
			 	ButtonPressed(CB_SQUARE) ||
			 	ButtonPressed(CB_X) ||
			 	ButtonPressed(CB_O) ||
			 	ButtonPressed(CB_TRI) ||
			 	ButtonPressed(CB_START) ||
			 	ButtonPressed(CB_SELECT) ||
			 	ButtonPressed(CB_R1) ||
			 	ButtonPressed(CB_R2) ||
			 	ButtonPressed(CB_L1) ||
			 	ButtonPressed(CB_L2) ||
			 	ButtonPressed(CB_LEFT) ||
			 	ButtonPressed(CB_DOWN) ||
			 	ButtonPressed(CB_RIGHT) ||
			 	ButtonPressed(CB_UP) ||
			 	ButtonPressed(CB_STICK1) ||
			 	ButtonPressed(CB_STICK2));
}

/***************************************************************************/
// Was reset pressed?
/***************************************************************************/
bool ControllerInputDriver::SoftResetPoll()
{
	return(ButtonDown(CB_L2) &&
				ButtonDown(CB_R2) &&
				ButtonDown(CB_START) &&
				ButtonDown(CB_SELECT));
}

/***************************************************************************/
// Determine if player is moving stick or pressing buttons
/***************************************************************************/
bool ControllerInputDriver::ControlActivity(
float analogThreshold)
{
	int i;

	for (i = 0; i < CB_BUTTONS; i++)
	{
		if (state[i].now != 0)
			return true;
	}

	for (i = 0; i < AA_AXES; i++)
	{
		if (Math::Abs(stick[i]) > analogThreshold)
			return true;
	}

	return false;
}

/***************************************************************************/
// Turns vibration on/off globally.
/***************************************************************************/
void ControllerInputDriver::Vibration(bool allow)
{
	allowVibration = allow;
}

/***************************************************************************/
/***************************************************************************/
bool ControllerInputDriver::SetVibration(
uint intensity,
VIBRATEPAN pan)
{
	return false;
}

/***************************************************************************/
// Do the vibration for duration milliseconds, with the above intensity/pan
// values.
/***************************************************************************/
bool ControllerInputDriver::SetVibrationDuration(
uint duration,
uint intensity,
VIBRATEPAN pan)
{
	return false;
}

/***************************************************************************/
// Make it so there is no vibration (force forces it to end even timed vibrations)
/***************************************************************************/
bool ControllerInputDriver::ClearVibration(
bool force)
{
	return false;
}

/***************************************************************************/
// Get the current vibration values. Returns false (and doesn't touch
// intensity if there is no vibration
/***************************************************************************/
bool ControllerInputDriver::GetVibration(
uint &intensity)
{ 
	return false; 
}	

/***************************************************************************/
/***************************************************************************/
void ControllerInputDriver::BeginInput()
{
	// Save the last frame's button statess
	for (uint i = 0; i < CB_BUTTONS; i++)
		state[i].last = state[i].now;
}

/***************************************************************************/
// Utility to generate a transfer function
/***************************************************************************/
bool ControllerInputDriver::GenerateTransferFunction(
float *transferLookup,
uint entries,
AxesTransferFunction transferFunction,
float factor,
float deadZonePercent,
float clampZonePercent)
{
	float val = 0.0f;
	int half = entries / 2;
	int dead = (uint)((float)half * deadZonePercent / 100.0f);
	int clamp = half - (uint)((float)half * clampZonePercent / 100.0f);
	float inc = 1.0f / ((half - 1.0f)*
								(100.0f-deadZonePercent-clampZonePercent)*0.01f);

	// Make sure this makes sense
	if (factor == 0.0f)
		factor = 1.0f;

	for (int i = 0; i < half + 1; i++)
	{
		float out = 0.0f;

		// In the dead zone?
		if (i <= dead)
			out = 0.0f;
		else if (i >= clamp && clampZonePercent != 0.0f)
			out = 1.0f;
		else
		{
			switch (transferFunction)
			{
			case ATF_LINEAR:
				out = val * factor;
				break;

			case ATF_PARABOLA:
				out = val * val * factor;
				break;

			case ATF_EXPONENTIAL:
				out = (Math::Exp(val) - 0.5f) / Math::Exp(1.0f) * factor;
				break;

			case ATF_POWER:
				out = Math::Abs(Math::Pow(val, factor));
				break;
			}
			val += inc;
		}

		// Out of range
		if (out > 1.0f)
			out = 1.0f;
		else if (out < 0.0f)
			out = 0.0f;

		// set it
		if (half - i >= 0)
			transferLookup[half - i] = -1.0f * out;
		if (half + i < (int)entries)
			transferLookup[half + i] = out;
	}

#ifdef GRAPHTRANSFER
	// Print it out
	char buff[256];
	static int file = 0;
	sprintf(buff, "trans%d.txt", file++);
	FILE *fh = fopen(buff, "wt");
	static char *function[] = { "Linear", "Parabola", "Exponential", "Power" };
	fprintf(fh, "Function: %s, factor: %.1f, deadZone: %.1f, clampZone: %.1f\n\n",
				function[transferFunction],	factor, deadZonePercent, clampZonePercent);
	for (uint p = 0; p < entries; p++)
	{
		fprintf(fh, "%3d:", p);
		int spaces = (int)(Math::Abs(transferLookup[p]) * 70.0f);
		int s = 0;
		for (; s < spaces - 1; s++)
			buff[s] = ' ';
		buff[s++] = '*';
		buff[s++] = '\n';
		buff[s] = '\0';

		fprintf(fh, buff);
	}
	fclose(fh);
#endif

	return true;
}

