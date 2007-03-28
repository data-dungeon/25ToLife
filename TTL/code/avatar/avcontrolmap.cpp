/***************************************************************************/
// Avatar Control Mapper
//
// This class maps the joystick device into "game" controls.  This layer of
// inderection has two purposes:
//
// - To make the remapping of the controller trivial
// - To provide both analog and digital directional control transparently
/***************************************************************************/
#include "TTLPCH.h"

#include "frontend/savegame.h"
#include "voice/netz/cvoicechat.h"
#include "Network/Session.h"

/***************************************************************************/
// Initlaize the controller
/***************************************************************************/
AvatarControlMapper::AvatarControlMapper() :
ControlMapper("Avatar")
{
	SetScheme(STANDARD);
	m_sensitivity = 0.5f;
	m_speed = 0.5f;

	for (int combo = 0; combo < COMBOS; combo++)
		ResetCombo((ButtonCombo)combo);

#ifdef _XBOX
	m_crouchToggle = true;
#else
	m_crouchToggle = false;
#endif
	m_crouchLast = m_crouchNow = false;
}

/***************************************************************************/
// Set default configuration
/***************************************************************************/
void AvatarControlMapper::Initialize()
{
	device->SetupAxis(AA_X1, ATF_LINEAR, 1.0f, 32.0f, 4.0f);
	device->SetupAxis(AA_Y1, ATF_LINEAR, 1.0f, 32.0f, 4.0f);
	device->SetupAxis(AA_X2, ATF_LINEAR, 1.0f, 32.0f, 4.0f);
	device->SetupAxis(AA_Y2, ATF_LINEAR, 1.0f, 32.0f, 4.0f);
}

/***************************************************************************/
/***************************************************************************/
void AvatarControlMapper::SetupFromProfile()
{
	SetButtonLayout((MAPPINGSCHEME)g_GlobalProfile.GetButtonLayout());
	SetThumbStickMode((MAPPINGSCHEME)g_GlobalProfile.GetThumbStickMode());
	SetControllerSensitivity(g_GlobalProfile.GetLookSensitivity() / 10.0f);
	SetControllerSpeed(g_GlobalProfile.GetLookSpeed() / 10.0f);
	InvertAxis(PITCH, g_GlobalProfile.GetInvertedAiming());
	EnableVibration(g_GlobalProfile.GetVibration());
#if defined(NETWORKING)
	// --- tell voice about our push to talk button
	if( ValidSingleton( CSession ) && g_session.IsNetworked() )
	{
		g_CVoiceChat.SetPushToTalk( GetButton( PUSHTOTALK ) );
	}
#endif
	g_messageDispatcher.SendMessageToAll("UpdateControllerSettings", 0, (ActorHandle)INVALID_OBJECT_HANDLE);
}

/***************************************************************************/
// Reset the stuff
/***************************************************************************/
void AvatarControlMapper::Reset()
{
	for (int combo = 0; combo < COMBOS; combo++)
		ResetCombo((ButtonCombo)combo);

	m_crouchLast = m_crouchNow = false;
}

/***************************************************************************/
// Update controller states (call once per frame)
/***************************************************************************/
void AvatarControlMapper::Update()
{
	if (device && HasFocus() && enabled)
	{
		UpdateCombo(GENERICCOMBO1, device->ButtonDown(CB_STICK1) && device->ButtonDown(CB_TRI));
		UpdateCombo(GENERICCOMBO2, device->ButtonDown(CB_STICK1) && device->ButtonDown(CB_SQUARE));
	}

	if (m_crouchToggle)
	{
		if (!HasFocus() || !enabled)
		{
		 	m_crouchLast = false;
			m_crouchNow = false;
		}
		else
		{
			m_crouchLast = m_crouchNow;
			if (ControlMapper::ButtonPressed(CROUCH))
				m_crouchNow = !m_crouchNow;
		}
	}
}

/***************************************************************************/
// Get the [-1..1] value of a stick
/***************************************************************************/
float AvatarControlMapper::GetAxis(
AXIS which)
{
	return ControlMapper::GetAxis(which);
}

/***************************************************************************/
/***************************************************************************/
const char *AvatarControlMapper::GetActionName(
DIGITALBUTTON Button)
{
	switch( Button )
	{
		case FIRE:
			return "Attack";
		case RELOAD:
			return "Reload";
		case JUMP:
			return "Jump";
		case CROUCH:
			return "Crouch";
		case USE:
			return "Use";
		case TAUNT:
			return "Taunt";
		case CHANGEWEAPON:
			return "Change Weapon";
		case PUSHTOTALK:
			return "PushToTalk";
		case AIM:
			return "ZoomIn";
	 	case TAG:
			return "Tag";
		case PEEKLEFT:
			return "PeekLeft";
		case PEEKRIGHT:
			return "PeekRight";
	 	case HUCK:
			return "Throw";
		case SELECTMELEE:
			return "Melee";
	}

	return "Undefined";
}

/***************************************************************************/
// Was this button just pressed last frame?
/***************************************************************************/
bool AvatarControlMapper::ButtonPressed(
DIGITALBUTTON button)
{
	if (button == CROUCH)
	{
		if (m_crouchToggle)
			return (m_crouchNow && !m_crouchLast);
	}
	else if (button == COMBO1)
		return ComboPressed(GENERICCOMBO1);
	else if (button == COMBO2)
		return ComboPressed(GENERICCOMBO2);

	return ControlMapper::ButtonPressed(button);
}

/***************************************************************************/
// Was this button just released last frame?
/***************************************************************************/
bool AvatarControlMapper::ButtonReleased(
DIGITALBUTTON button)
{
	if (button == CROUCH)
	{
		if (m_crouchToggle)
			return (!m_crouchNow && m_crouchLast);
	}
	else if (button == COMBO1)
		return ComboReleased(GENERICCOMBO1);
	else if (button == COMBO2)
		return ComboReleased(GENERICCOMBO2);

	return ControlMapper::ButtonReleased(button);
}

/***************************************************************************/
// Is this button currenly down?
/***************************************************************************/
bool AvatarControlMapper::ButtonDown(
DIGITALBUTTON button)
{
	if (button == CROUCH)
	{
		if (m_crouchToggle)
			return m_crouchNow;
	}
	else if (button == COMBO1)
		return ComboDown(GENERICCOMBO1);
	else if (button == COMBO2)
		return ComboDown(GENERICCOMBO2);

	return ControlMapper::ButtonDown(button);
}

/***************************************************************************/
/***************************************************************************/
const char* AvatarControlMapper::GetSchemeName( MAPPINGSCHEME Scheme )
{
	switch( Scheme )
	{
	case STANDARD:
		return "Standard";

	case ALT1:
		return "Alt 1";

	case ASSBACKWARDS:
		return "Backwards";

	case ASSBACKWARDSALT1:
		return "Backwards Alt 1";

	case ALT2:
		return "Alt 2";

	case ALT3:
		return "Alt 3";

//	case INTROVERT:
//		return "Introvert";
	}

	return "Undefined";
}

/***************************************************************************/
/***************************************************************************/
void AvatarControlMapper::SetButtonLayout( MAPPINGSCHEME Scheme )
{
	switch( Scheme )
	{
	case STANDARD:
#if defined (_XBOX)
		SetButton(JUMP, CB_X);
		SetButton(USE, CB_O);
		SetButton(SELECTMELEE, CB_TRI);
		SetButton(RELOAD, CB_SQUARE);

		SetButton(TAUNT, CB_UP);
		SetButton(PEEKRIGHT, CB_RIGHT);
		SetButton(PEEKLEFT, CB_LEFT);
		SetButton(TAG, CB_DOWN);

		SetButton(FIRE, CB_R1);
		SetButton(HUCK, CB_L1);

		SetButton(AIM, CB_STICK2);

		SetButton(CHANGEWEAPON, CB_R2);
		SetButton(PUSHTOTALK, CB_L2);
		SetButton(CROUCH, CB_STICK1);
#else
		SetButton(JUMP, CB_X);
		SetButton(USE, CB_O);
		SetButton(SELECTMELEE, CB_TRI);
		SetButton(RELOAD, CB_SQUARE);

		SetButton(TAUNT, CB_UP);
		SetButton(PEEKRIGHT, CB_RIGHT);
		SetButton(PEEKLEFT, CB_LEFT);
		SetButton(TAG, CB_DOWN);

		SetButton(FIRE, CB_R1);
		SetButton(HUCK, CB_L1);

		SetButton(AIM, CB_STICK2);

		SetButton(CHANGEWEAPON, CB_R2);
		SetButton(PUSHTOTALK, CB_STICK1);
		SetButton(CROUCH, CB_L2);
#endif
		break;

	case ALT1:
		// Use the standard button layout
		SetButtonLayout(STANDARD);

		// But make these changes
#if defined (_XBOX)
		SetButton(AIM, CB_STICK1);
		SetButton(CROUCH, CB_STICK2);
#else
		SetButton(PUSHTOTALK, CB_R2);
		SetButton(SELECTMELEE, CB_DOWN);
		SetButton(USE, CB_SQUARE);
		SetButton(RELOAD, CB_O);
		SetButton(CHANGEWEAPON, CB_TRI);
		SetButton(TAG, CB_STICK1);
#endif
		break;

	case ASSBACKWARDS:
		// Use the standard button layout
		SetButtonLayout( STANDARD );
		break;

	case ASSBACKWARDSALT1:
		// Use the southpaw button layout
		SetButtonLayout( ALT1 );
		break;

//#ifdef DANREVIEWTHESE
// --- DAS, put these in and tell Dan about them, so the gamePadSettings screen at least works

	case ALT2:
		// Use the standard button layout
		SetButtonLayout(STANDARD);

		// But make these changes
#if defined (_XBOX)
		SetButton(CROUCH, CB_L1);
		SetButton(HUCK, CB_TRI);
		SetButton(PUSHTOTALK, CB_STICK1);
		SetButton(SELECTMELEE, CB_L2);
#else
		SetButton(SELECTMELEE, CB_DOWN);
		SetButton(TAG, CB_STICK1);
		SetButton(USE, CB_SQUARE);
		SetButton(PUSHTOTALK, CB_O);
		SetButton(RELOAD, CB_TRI);
#endif

		break;

	case ALT3:
		// Use the standard button layout
		SetButtonLayout(STANDARD);

		// But make these changes
#if defined (_XBOX)
		SetButton(CROUCH, CB_L1);
		SetButton(PUSHTOTALK, CB_STICK1);
		SetButton(HUCK, CB_R2);
		SetButton(CHANGEWEAPON, CB_TRI);
#else
		SetButton(CHANGEWEAPON, CB_L1);
		SetButton(HUCK, CB_R2);
		SetButton(AIM, CB_STICK1);
		SetButton(RELOAD, CB_STICK2);
		SetButton(JUMP, CB_SQUARE);
		SetButton(USE, CB_X);
		SetButton(PUSHTOTALK, CB_O);
#endif

		break;

//pab	case INTROVERT:
//pab		// Use the standard button layout
//pab		SetButtonLayout(STANDARD);
//pab
//pab		// But make these changes
//pab		SetButton(FIRE, CB_L2);
//pab		SetButton(CHANGEWEAPON, CB_L1);
//pab		SetButton(HUCK, CB_R2);
//pab		SetButton(CROUCH, CB_R1);
//pab		break;

//#endif

	default:
		SetButtonLayout( STANDARD );
		break;
	}

	return;
}

/***************************************************************************/
/***************************************************************************/
void AvatarControlMapper::SetThumbStickMode( MAPPINGSCHEME Scheme )
{
	switch( Scheme )
	{
	case STANDARD:
		// Setup the axes
		SetAxis(STRAFE, AA_X1);
		SetAxis(FORWARDBACK, AA_Y1);
		SetAxis(YAW, AA_X2);
		SetAxis(PITCH, AA_Y2);
		break;

	case ALT1:
		// Setup the axes
		SetAxis(STRAFE, AA_X2);
		SetAxis(FORWARDBACK, AA_Y2);
		SetAxis(YAW, AA_X1);
		SetAxis(PITCH, AA_Y1);
		break;

	case ASSBACKWARDS:
		// Setup the axes
		SetAxis(STRAFE, AA_X2);
		SetAxis(FORWARDBACK, AA_Y1);
		SetAxis(YAW, AA_X1);
		SetAxis(PITCH, AA_Y2);
		break;

	case ASSBACKWARDSALT1:
		// Setup the axes
		SetAxis(STRAFE, AA_X1);
		SetAxis(FORWARDBACK, AA_Y2);
		SetAxis(YAW, AA_X2);
		SetAxis(PITCH, AA_Y1);
		break;

	default:
		SetThumbStickMode(STANDARD);
		break;
	}

	return;
}

/***************************************************************************/
// Use some defaults
/***************************************************************************/
void AvatarControlMapper::SetScheme(
MAPPINGSCHEME scheme)
{
	SetButtonLayout( scheme );
	SetThumbStickMode( scheme );
}
