/***************************************************************************/
// Default control mapper
//
// Just maps all the controller buttons to the expected buttons
/***************************************************************************/
#include "platform/PlatformPCH.h"

/***************************************************************************/
// Initlaize the controller
/***************************************************************************/
DefaultControlMapper::DefaultControlMapper(
const char *name) :
ControlMapper(name)
{
	// Setup a direct mapping
	SetButton(BUTTON_LEFT, CB_LEFT);
	SetButton(BUTTON_DOWN, CB_DOWN);
	SetButton(BUTTON_RIGHT, CB_RIGHT);
	SetButton(BUTTON_UP, CB_UP);

	SetButton(BUTTON_SQUARE, CB_SQUARE);
	SetButton(BUTTON_X, CB_X);
	SetButton(BUTTON_O, CB_O);
	SetButton(BUTTON_TRI, CB_TRI);

	SetButton(BUTTON_L1, CB_L1);
	SetButton(BUTTON_R1, CB_R1);
	SetButton(BUTTON_L2, CB_L2);
	SetButton(BUTTON_R2, CB_R2);

	SetButton(BUTTON_START, CB_START);
	SetButton(BUTTON_SELECT, CB_SELECT);
	SetButton(BUTTON_STICK1, CB_STICK1);
	SetButton(BUTTON_STICK2, CB_STICK2);

	SetAxis(AXIS_X1, AA_X1);
	SetAxis(AXIS_Y1, AA_Y1);
	SetAxis(AXIS_X2, AA_X2);
	SetAxis(AXIS_Y2, AA_Y2);
}

/***************************************************************************/
// Was this button just pressed last frame?
/***************************************************************************/
bool DefaultControlMapper::ButtonPressed(
DIGITALBUTTON button)
{
	return ControlMapper::ButtonPressed(button);
}

/***************************************************************************/
// Was this button just released last frame?
/***************************************************************************/
bool DefaultControlMapper::ButtonReleased(
DIGITALBUTTON button)
{
	return ControlMapper::ButtonReleased(button);
}

/***************************************************************************/
// Is this button down?
/***************************************************************************/
bool DefaultControlMapper::ButtonDown(
DIGITALBUTTON button)
{
	return ControlMapper::ButtonDown(button);
}

/***************************************************************************/
// Axis access
/***************************************************************************/
float DefaultControlMapper::GetAxis(
ANALOGAXIS axis)
{
	return ControlMapper::GetAxis(axis);
}

/***************************************************************************/
/***************************************************************************/
void DefaultControlMapper::InvertAxis(
ANALOGAXIS axis,
bool invertOn)
{
	ControlMapper::InvertAxis(axis, invertOn);
}

/***************************************************************************/
/***************************************************************************/
bool DefaultControlMapper::AxisInverted(
ANALOGAXIS axis)
{
	return ControlMapper::AxisInverted(axis);
}


