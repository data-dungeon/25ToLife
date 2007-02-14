/***************************************************************************/
// Avatar Control Mapper
//
// This class maps the joystick input into "game" controls.  This layer of
// inderection has two purposes:
//
// - To make the remapping of the controller trivial
// - To provide both analog and digital directional control transparently
/***************************************************************************/
#ifndef AVCONTROLMAP_H
#define AVCONTROLMAP_H

#include "platform/ctrolmap.h"

class AvatarControlMapper : public ControlMapper
{
public:
	AvatarControlMapper();
	~AvatarControlMapper() {}

	// Set default configuration
	void Initialize();

	// Setup this up from the profile
	void SetupFromProfile();

	// Reset the stuff
	void Reset();

	// Update controller states (call once per frame)
	void Update();

	// The analog axes
	enum AXIS
	{
		FORWARDBACK,
		STRAFE,
		PITCH,
		YAW,
		AXES
	};

	// Get the [-1..1] value of a stick
	float GetAxis(AXIS which);

	// The digital input buttons
	enum DIGITALBUTTON
	{
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

		DIGITALBUTTONS
	};
	static const char *GetActionName(DIGITALBUTTON Button);

	// Queries on the digital controls

	// Was this button just pressed last frame?
	bool ButtonPressed(DIGITALBUTTON button);

	// Was this button just released last frame?
	bool ButtonReleased(DIGITALBUTTON button);

	// Is this button currenly down?
	bool ButtonDown(DIGITALBUTTON button);

	// Use some defaults
	typedef enum
	{
		STANDARD,
		ALT1,					// southpaw
		ASSBACKWARDS,
		ASSBACKWARDSALT1,
		ALT2,					// greenthumb
		ALT3,					// spartan
//pab		INTROVERT,			// introvert
	}
	MAPPINGSCHEME;

	const char* GetSchemeName( MAPPINGSCHEME Scheme );
	void SetButtonLayout( MAPPINGSCHEME Scheme );
	void SetThumbStickMode( MAPPINGSCHEME Scheme );
	void SetScheme(MAPPINGSCHEME Scheme);

	// Set/get axis invert stuff
	bool AxisInverted(AXIS axis)
		{ return ControlMapper::AxisInverted(axis); }
	void InvertAxis(AXIS axis, bool invert)
		{ ControlMapper::InvertAxis(axis, invert); }

	float GetControllerSensitivity()
		{ return m_sensitivity; }
	float GetControllerSpeed()
		{ return m_speed; }
	void SetControllerSensitivity(float sensitivity)
		{ m_sensitivity = sensitivity; }
	void SetControllerSpeed(float speed)
		{ m_speed = speed; }

private:
	float m_sensitivity;
	float m_speed;

	enum ButtonCombo
	{
		GENERICCOMBO1,
		GENERICCOMBO2,

		COMBOS
	};
	struct ButtonComboState
	{
		bool		last, now;
	};
	ButtonComboState m_combo[COMBOS];
	void UpdateCombo(ButtonCombo combo, bool newState)
		{ m_combo[combo].last = m_combo[combo].now; m_combo[combo].now = newState; }
	void ResetCombo(ButtonCombo combo)
		{ m_combo[combo].last = m_combo[combo].now = false; }
	bool ComboPressed(ButtonCombo combo)
		{ return (!m_combo[combo].last && m_combo[combo].now);  }
	bool ComboDown(ButtonCombo combo)
		{ return (m_combo[combo].last && m_combo[combo].now);  }
	bool ComboReleased(ButtonCombo combo)
		{ return (m_combo[combo].last && !m_combo[combo].now);  }

	bool		m_crouchToggle;
	bool		m_crouchLast, m_crouchNow;
};

#endif
