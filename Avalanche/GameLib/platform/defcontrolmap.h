/***************************************************************************/
// Default control mapper
//
// Just maps all the controller buttons to the expected buttons
/***************************************************************************/
#ifndef DEFAULTCONTROLMAP_H
#define DEFAULTCONTROLMAP_H

#include "platform/ctrolmap.h"

class DefaultControlMapper : public ControlMapper
{
public:
	DefaultControlMapper(const char *name);
	~DefaultControlMapper() {}

	// The digital input buttons
	enum DIGITALBUTTON
	{
		BUTTON_LEFT, BUTTON_DOWN,	BUTTON_RIGHT, BUTTON_UP,
		BUTTON_SQUARE, BUTTON_X, BUTTON_O, BUTTON_TRI,
		BUTTON_L1,               	// This is the white XBox button
		BUTTON_R1,						// This is the black XBox button
		BUTTON_L2,
		BUTTON_R2,
		BUTTON_START, BUTTON_SELECT,
		BUTTON_STICK1, BUTTON_STICK2
	};

	// Was this button just pressed last frame?
	bool ButtonPressed(DIGITALBUTTON button);

	// Was this button just released last frame?
	bool ButtonReleased(DIGITALBUTTON button);

	// Is this button down?
	bool ButtonDown(DIGITALBUTTON button);

	// Axis support
	enum ANALOGAXIS
	{
		AXIS_X1, AXIS_Y1,
		AXIS_X2, AXIS_Y2
	};

	// Axis access
	float GetAxis(ANALOGAXIS axis);
	void InvertAxis(ANALOGAXIS axis, bool invertOn);
	bool AxisInverted(ANALOGAXIS axis);
};

#endif
