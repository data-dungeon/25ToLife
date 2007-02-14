#include "TTLPCH.h"
#include "frontend/inputdriver.h"

//=============================================================================
// Fluff Control Mapper
//
// This class maps the joystick device into "game" controls.  This layer of
// inderection has two purposes:
//
// - To make the remapping of the controller trivial
// - To provide both analog and digital directional control transparently
//=============================================================================

#include "TTLPCH.h"

#include "frontend/frontend.h"

//=============================================================================
// FullControlMapper::FluffControlMapper
//
//		Initialize the control mapper and set the stick to digital.
//=============================================================================

FluffControlMapper::FluffControlMapper() : ControlMapper("Fluff")
{
#if defined (_XBOX)
	SetButton(UP,			CB_UP		);
	SetButton(DOWN,			CB_DOWN		);
	SetButton(LEFT,			CB_LEFT		);
	SetButton(RIGHT,		CB_RIGHT	);
	SetButton(SELECT,		CB_SELECT	);
	SetButton(START,		CB_START	);
	SetButton(X,			CB_X		);
	SetButton(TRIANGLE,		CB_O		);
	SetButton(CIRCLE,		CB_TRI		);
	SetButton(SQUARE,		CB_SQUARE	);
	SetButton(L1,			CB_L1 		);
	SetButton(R1,			CB_R1 		);
	SetButton(L2,			CB_L2 		);
	SetButton(R2,			CB_R2 		);
#else
	SetButton(UP,			CB_UP		);
	SetButton(DOWN,			CB_DOWN		);
	SetButton(LEFT,			CB_LEFT		);
	SetButton(RIGHT,		CB_RIGHT	);
	SetButton(SELECT,		CB_SELECT	);
	SetButton(START,		CB_START	);
	SetButton(X,			CB_X		);
	SetButton(TRIANGLE,		CB_TRI		);
	SetButton(CIRCLE,		CB_O		);
	SetButton(SQUARE,		CB_SQUARE	);
	SetButton(L1,			CB_L1 		);
	SetButton(R1,			CB_R1 		);
	SetButton(L2,			CB_L2 		);
	SetButton(R2,			CB_R2 		);
#endif
	Stick1ToDigital( true );
}

//=============================================================================
// FluffControlMapper::ButtonPressed
//
//		Call this function to determine when a button is pressed.
//=============================================================================

bool FluffControlMapper::ButtonPressed( DIGITALBUTTON button )
{
	bool down = ControlMapper::ButtonPressed(button);

	if(g_ControllerRemoved == 1)
		return (false);

	switch( button )
	{
	case UP:
		down |= g_input.ButtonPressed(BC_UP);
		break;
	case DOWN:
		down |= g_input.ButtonPressed(BC_DOWN);
		break;
	case LEFT:
		down |= g_input.ButtonPressed(BC_LEFT);
		break;
	case RIGHT:
		down |= g_input.ButtonPressed(BC_RIGHT);
		break;
	case X:
		down |= g_input.ButtonPressed(BC_ENTER);
		break;
	case TRIANGLE:
		down |= g_input.ButtonPressed(BC_ESCAPE);
		break;
	case SELECT:
		down |= g_input.ButtonPressed(BC_TILDE);
		break;
	case START:
		down |= g_input.ButtonPressed(BC_ENTER);
		break;
#ifdef DIRECTX_PC
	case ESCAPE:
		down |= g_input.ButtonPressed(BC_ESCAPE);
#endif
	}
	return down;
}

//=============================================================================
// FluffControlMapper::ButtonReleased
//
//		Call this function to determine when a button is released.
//=============================================================================

bool FluffControlMapper::ButtonReleased( DIGITALBUTTON button )
{
	bool down = ControlMapper::ButtonReleased(button);

	if(g_ControllerRemoved == 1)
		return (false);

	switch( button )
	{
	case UP:
		down |= g_input.ButtonReleased(BC_UP);
		break;
	case DOWN:
		down |= g_input.ButtonReleased(BC_DOWN);
		break;
	case LEFT:
		down |= g_input.ButtonReleased(BC_LEFT);
		break;
	case RIGHT:
		down |= g_input.ButtonReleased(BC_RIGHT);
		break;
	case X:
		down |= g_input.ButtonReleased(BC_ENTER);
		break;
	case TRIANGLE:
		down |= g_input.ButtonReleased(BC_ESCAPE);
		break;
	case SELECT:
		down |= g_input.ButtonReleased(BC_TILDE);
		break;
	case START:
		down |= g_input.ButtonReleased(BC_ENTER);
		break;
	}
	return down;
}


//=============================================================================
// FluffControlMapper::ButtonDown
//
//		Call this function to determine when a button is being held down.
//=============================================================================

bool FluffControlMapper::ButtonDown( DIGITALBUTTON button )
{
	bool down = ControlMapper::ButtonDown(button);

	switch( button )
	{
	case UP:
		down |= g_input.ButtonDown(BC_UP);
		break;
	case DOWN:
		down |= g_input.ButtonDown(BC_DOWN);
		break;
	case LEFT:
		down |= g_input.ButtonDown(BC_LEFT);
		break;
	case RIGHT:
		down |= g_input.ButtonDown(BC_RIGHT);
		break;
	case X:
		down |= g_input.ButtonDown(BC_ENTER);
		break;
	case TRIANGLE:
		down |= g_input.ButtonDown(BC_ESCAPE);
		break;
	case SELECT:
		down |= g_input.ButtonDown(BC_TILDE);
		break;
	case START:
		down |= g_input.ButtonDown(BC_ENTER);
		break;
	}
	return down;
}

//=============================================================================
// ActionMap::ActionMap
//=============================================================================

const char* ActionMap::GetActionText( ActionType Action )
{
	switch( Action )
	{
	case Action_Move:
		return "Move";
	case Action_Aim:
		return "Aim";
	case Action_Fire:
		return "Attack";
	case Action_Reload:
		return "Reload";
	case Action_Use:
		return "Use";
	case Action_Jump:
		return "Jump";
	case Action_Crouch:
		return "Crouch";
#ifdef DIRECTX_PC
	case Action_NextWeapon:
		return "Next Weapon";
	case Action_PreviousWeapon:
		return "Prev Weapon";
#endif
	case Action_SwitchWeapon:
		return "Switch Weapon";
	case Action_VoiceChat:
		return "Voice Chat";
	case Action_ShowScores:
		return "Scoreboard";
	case Action_PeekLeft:
		return "Peek Left";
	case Action_PeekRight:
		return "Peek Right";
	case Action_MeleeSelect:
		return "Select Melee";
	case Action_Taunt:
		return "Taunt";
	case Action_Tag:
		return "Tag";
	case Action_Huck:
		return "Grenade";
	case Action_PauseOptions:
		return "Pause";
	}

	return 0;
}

ActionMap::ActionMap( void )
{
	ClearActionMap();
	DefaultActionMap(0);
}

int ActionMap::MaxActions( void )
{
	return sizeof(m_Actions)/sizeof(m_Actions[0]);
}

void ActionMap::SetAction( ActionType Action, DefaultControlMapper::DIGITALBUTTON Button )
{
	if( Action < Action_Max )
		m_Actions[Action] = Button;

	return;
}

DefaultControlMapper::DIGITALBUTTON ActionMap::GetAction( ActionType Action )
{
	if( Action < Action_Max )
		return m_Actions[Action];

	return DefaultControlMapper::DIGITALBUTTON(~0);
}


void ActionMap::DefaultActionMap( int i )
{
	ClearActionMap();

	switch( i )
	{
	case 0:
#if defined (_XBOX)
	// dpad
		SetAction( Action_PeekLeft,		DefaultControlMapper::BUTTON_LEFT );
		SetAction( Action_PeekRight,		DefaultControlMapper::BUTTON_RIGHT );
		SetAction( Action_Tag,				DefaultControlMapper::BUTTON_DOWN );
		SetAction( Action_Taunt,			DefaultControlMapper::BUTTON_UP );

	// face buttons
		SetAction( Action_MeleeSelect,	DefaultControlMapper::BUTTON_TRI );
		SetAction( Action_Use,				DefaultControlMapper::BUTTON_O );
		SetAction( Action_Reload,			DefaultControlMapper::BUTTON_SQUARE );
		SetAction( Action_Jump,				DefaultControlMapper::BUTTON_X );

	// shoulder buttons
		SetAction( Action_Huck,				DefaultControlMapper::BUTTON_L1 );
		SetAction( Action_Fire,				DefaultControlMapper::BUTTON_R1 );

	// stick buttons
		SetAction( Action_Crouch,			DefaultControlMapper::BUTTON_STICK1 );
		SetAction( Action_Aim,				DefaultControlMapper::BUTTON_STICK2 );

	// white / black
		SetAction (Action_VoiceChat,		DefaultControlMapper::BUTTON_L2 );
		SetAction (Action_SwitchWeapon,	DefaultControlMapper::BUTTON_R2 );

	// start, select
		SetAction( Action_ShowScores,		DefaultControlMapper::BUTTON_SELECT );
		SetAction( Action_PauseOptions,	DefaultControlMapper::BUTTON_START );
#else
			// dpad
		SetAction( Action_PeekLeft,		DefaultControlMapper::BUTTON_LEFT );
		SetAction( Action_PeekRight,		DefaultControlMapper::BUTTON_RIGHT );
		SetAction( Action_Tag,				DefaultControlMapper::BUTTON_DOWN );
		SetAction( Action_Taunt,			DefaultControlMapper::BUTTON_UP );

	// face buttons
		SetAction( Action_MeleeSelect,	DefaultControlMapper::BUTTON_TRI );
		SetAction( Action_Use,				DefaultControlMapper::BUTTON_O );
		SetAction( Action_Reload,			DefaultControlMapper::BUTTON_SQUARE );
		SetAction( Action_Jump,				DefaultControlMapper::BUTTON_X );

	// shoulder buttons
		SetAction( Action_Huck,				DefaultControlMapper::BUTTON_L1 );
		SetAction( Action_Fire,				DefaultControlMapper::BUTTON_R1 );
		SetAction (Action_Crouch,			DefaultControlMapper::BUTTON_L2 );
		SetAction (Action_SwitchWeapon,	DefaultControlMapper::BUTTON_R2 );

	// stick buttons
		SetAction( Action_VoiceChat,		DefaultControlMapper::BUTTON_STICK1 );
		SetAction( Action_Aim,				DefaultControlMapper::BUTTON_STICK2 );

	// start, select
		SetAction( Action_ShowScores,		DefaultControlMapper::BUTTON_SELECT );
		SetAction( Action_PauseOptions,	DefaultControlMapper::BUTTON_START );
#endif
		break;

	case 1:
#if defined (_XBOX)
	// dpad
		SetAction( Action_PeekLeft,		DefaultControlMapper::BUTTON_LEFT );
		SetAction( Action_PeekRight,		DefaultControlMapper::BUTTON_RIGHT );
		SetAction( Action_Tag,				DefaultControlMapper::BUTTON_DOWN );
		SetAction( Action_Taunt,			DefaultControlMapper::BUTTON_UP );

	// face buttons
		SetAction( Action_MeleeSelect,	DefaultControlMapper::BUTTON_TRI );
		SetAction( Action_Use,				DefaultControlMapper::BUTTON_O );
		SetAction( Action_Reload,			DefaultControlMapper::BUTTON_SQUARE );
		SetAction( Action_Jump,				DefaultControlMapper::BUTTON_X );

	// shoulder buttons
		SetAction( Action_Huck,				DefaultControlMapper::BUTTON_L1 );
		SetAction( Action_Fire,				DefaultControlMapper::BUTTON_R1 );

	// stick buttons
		SetAction( Action_Aim,				DefaultControlMapper::BUTTON_STICK1 );
		SetAction( Action_Crouch,			DefaultControlMapper::BUTTON_STICK2 );

	// white / black
		SetAction (Action_VoiceChat,		DefaultControlMapper::BUTTON_L2 );
		SetAction (Action_SwitchWeapon,	DefaultControlMapper::BUTTON_R2 );

	// start, select
		SetAction( Action_ShowScores,		DefaultControlMapper::BUTTON_SELECT );
		SetAction( Action_PauseOptions,	DefaultControlMapper::BUTTON_START );
#else
	// dpad
		SetAction( Action_PeekLeft,		DefaultControlMapper::BUTTON_LEFT );
		SetAction( Action_PeekRight,		DefaultControlMapper::BUTTON_RIGHT );
		SetAction( Action_MeleeSelect,	DefaultControlMapper::BUTTON_DOWN );
		SetAction( Action_Taunt,			DefaultControlMapper::BUTTON_UP );

	// face buttons
		SetAction( Action_SwitchWeapon,	DefaultControlMapper::BUTTON_TRI );
		SetAction( Action_Reload,			DefaultControlMapper::BUTTON_O );
		SetAction( Action_Use,				DefaultControlMapper::BUTTON_SQUARE );
		SetAction( Action_Jump,				DefaultControlMapper::BUTTON_X );

	// shoulder buttons
		SetAction( Action_Huck,				DefaultControlMapper::BUTTON_L1 );
		SetAction( Action_Fire,				DefaultControlMapper::BUTTON_R1 );
		SetAction (Action_Crouch,			DefaultControlMapper::BUTTON_L2 );
		SetAction (Action_VoiceChat,		DefaultControlMapper::BUTTON_R2 );

	// stick buttons
		SetAction( Action_Tag,		DefaultControlMapper::BUTTON_STICK1 );
		SetAction( Action_Aim,				DefaultControlMapper::BUTTON_STICK2 );

	// start, select
		SetAction( Action_ShowScores,		DefaultControlMapper::BUTTON_SELECT );
		SetAction( Action_PauseOptions,	DefaultControlMapper::BUTTON_START );
#endif
		break;

	case 2:
#if defined (_XBOX)
	// dpad
		SetAction( Action_PeekLeft,		DefaultControlMapper::BUTTON_LEFT );
		SetAction( Action_PeekRight,		DefaultControlMapper::BUTTON_RIGHT );
		SetAction( Action_Tag,				DefaultControlMapper::BUTTON_DOWN );
		SetAction( Action_Taunt,			DefaultControlMapper::BUTTON_UP );

	// face buttons
		SetAction( Action_Huck,				DefaultControlMapper::BUTTON_TRI );
		SetAction( Action_Use,				DefaultControlMapper::BUTTON_O );
		SetAction( Action_Reload,			DefaultControlMapper::BUTTON_SQUARE );
		SetAction( Action_Jump,				DefaultControlMapper::BUTTON_X );

	// shoulder buttons
		SetAction( Action_Crouch,			DefaultControlMapper::BUTTON_L1 );
		SetAction( Action_Fire,				DefaultControlMapper::BUTTON_R1 );

	// stick buttons
		SetAction( Action_VoiceChat,		DefaultControlMapper::BUTTON_STICK1 );
		SetAction( Action_Aim,				DefaultControlMapper::BUTTON_STICK2 );

	// white / black
		SetAction (Action_MeleeSelect,	DefaultControlMapper::BUTTON_L2 );
		SetAction (Action_SwitchWeapon,	DefaultControlMapper::BUTTON_R2 );

	// start, select
		SetAction( Action_ShowScores,		DefaultControlMapper::BUTTON_SELECT );
		SetAction( Action_PauseOptions,	DefaultControlMapper::BUTTON_START );
#else
	// dpad
		SetAction( Action_PeekLeft,		DefaultControlMapper::BUTTON_LEFT );
		SetAction( Action_PeekRight,		DefaultControlMapper::BUTTON_RIGHT );
		SetAction( Action_MeleeSelect,	DefaultControlMapper::BUTTON_DOWN );
		SetAction( Action_Taunt,			DefaultControlMapper::BUTTON_UP );

	// face buttons
		SetAction( Action_Reload,			DefaultControlMapper::BUTTON_TRI );
		SetAction( Action_VoiceChat,		DefaultControlMapper::BUTTON_O );
		SetAction( Action_Use,				DefaultControlMapper::BUTTON_SQUARE );
		SetAction( Action_Jump,				DefaultControlMapper::BUTTON_X );

	// shoulder buttons
		SetAction( Action_Huck,				DefaultControlMapper::BUTTON_L1 );
		SetAction( Action_Fire,				DefaultControlMapper::BUTTON_R1 );
		SetAction (Action_Crouch,			DefaultControlMapper::BUTTON_L2 );
		SetAction (Action_SwitchWeapon,	DefaultControlMapper::BUTTON_R2 );

	// stick buttons
		SetAction( Action_Tag,				DefaultControlMapper::BUTTON_STICK1 );
		SetAction( Action_Aim,				DefaultControlMapper::BUTTON_STICK2 );

	// start, select
		SetAction( Action_ShowScores,		DefaultControlMapper::BUTTON_SELECT );
		SetAction( Action_PauseOptions,	DefaultControlMapper::BUTTON_START );
#endif
		break;

	case 3:
#if defined (_XBOX)
	// dpad
		SetAction( Action_PeekLeft,		DefaultControlMapper::BUTTON_LEFT );
		SetAction( Action_PeekRight,		DefaultControlMapper::BUTTON_RIGHT );
		SetAction( Action_Tag,				DefaultControlMapper::BUTTON_DOWN );
		SetAction( Action_Taunt,			DefaultControlMapper::BUTTON_UP );

	// face buttons
		SetAction( Action_SwitchWeapon,	DefaultControlMapper::BUTTON_TRI );
		SetAction( Action_Use,				DefaultControlMapper::BUTTON_O );
		SetAction( Action_Reload,			DefaultControlMapper::BUTTON_SQUARE );
		SetAction( Action_Jump,				DefaultControlMapper::BUTTON_X );

	// shoulder buttons
		SetAction( Action_Crouch,			DefaultControlMapper::BUTTON_L1 );
		SetAction( Action_Fire,				DefaultControlMapper::BUTTON_R1 );

	// stick buttons
		SetAction( Action_VoiceChat,		DefaultControlMapper::BUTTON_STICK1 );
		SetAction( Action_Aim,				DefaultControlMapper::BUTTON_STICK2 );

	// white / black
		SetAction (Action_MeleeSelect,	DefaultControlMapper::BUTTON_L2 );
		SetAction (Action_Huck,				DefaultControlMapper::BUTTON_R2 );

	// start, select
		SetAction( Action_ShowScores,		DefaultControlMapper::BUTTON_SELECT );
		SetAction( Action_PauseOptions,	DefaultControlMapper::BUTTON_START );
#else
			// dpad
		SetAction( Action_PeekLeft,		DefaultControlMapper::BUTTON_LEFT );
		SetAction( Action_PeekRight,		DefaultControlMapper::BUTTON_RIGHT );
		SetAction( Action_Tag,				DefaultControlMapper::BUTTON_DOWN );
		SetAction( Action_Taunt,			DefaultControlMapper::BUTTON_UP );

	// face buttons
		SetAction( Action_MeleeSelect,	DefaultControlMapper::BUTTON_TRI );
		SetAction( Action_VoiceChat,		DefaultControlMapper::BUTTON_O );
		SetAction( Action_Jump,				DefaultControlMapper::BUTTON_SQUARE );
		SetAction( Action_Use,				DefaultControlMapper::BUTTON_X );

	// shoulder buttons
		SetAction( Action_SwitchWeapon,	DefaultControlMapper::BUTTON_L1 );
		SetAction( Action_Fire,				DefaultControlMapper::BUTTON_R1 );
		SetAction (Action_Crouch,			DefaultControlMapper::BUTTON_L2 );
		SetAction (Action_Huck,				DefaultControlMapper::BUTTON_R2 );

	// stick buttons
		SetAction( Action_Aim,				DefaultControlMapper::BUTTON_STICK1 );
		SetAction( Action_Reload,			DefaultControlMapper::BUTTON_STICK2 );

	// start, select
		SetAction( Action_ShowScores,		DefaultControlMapper::BUTTON_SELECT );
		SetAction( Action_PauseOptions,	DefaultControlMapper::BUTTON_START );
#endif
		break;
	}

	return;
}

void ActionMap::ClearActionMap( void )
{
	int n = MaxActions();

	for( int i = 0; i < n; i++ )
		m_Actions[i] = DefaultControlMapper::DIGITALBUTTON(~0);

	return;
}