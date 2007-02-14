#ifndef _FRONTEND_INPUTDRIVER_H_
#define _FRONTEND_INPUTDRIVER_H_

//=============================================================================
// Fluff Control Mapper
//=============================================================================

#include "platform/ctrolmap.h"
#include "platform/defcontrolmap.h"

class FluffControlMapper : public ControlMapper
{
public:
	FluffControlMapper();
	~FluffControlMapper() {}

	// The digital input buttons
	enum DIGITALBUTTON
	{
		UP = 0,
		DOWN,
		LEFT,
		RIGHT,
		START,
		SELECT,
		X,
		TRIANGLE,
		CIRCLE,
		SQUARE,
		L1,
		R1,
		L2,
		R2,
#ifdef DIRECTX_PC
		ESCAPE,
#endif
		MAXBUTTONS,
	};

	// Was the button just pressed?
	bool ButtonPressed( DIGITALBUTTON Button );

	// Was the button just released?
	bool ButtonReleased( DIGITALBUTTON Button );

	// Is the button being held down?
	bool ButtonDown( DIGITALBUTTON Button );
};

class ActionMap
{
	public:
		typedef enum
		{
			Action_Move,
			Action_Aim,
			Action_Fire,
			Action_Reload,
			Action_Use,
			Action_Jump,
			Action_Crouch,
#ifdef DIRECTX_PC
			Action_NextWeapon,
			Action_PreviousWeapon,
#endif
			Action_SwitchWeapon,
			Action_VoiceChat,
			Action_ShowScores,
			Action_PeekLeft,
			Action_PeekRight,
			Action_MeleeSelect,
			Action_Taunt,
			Action_Tag,
			Action_Huck,
			Action_PauseOptions,
			Action_Max,
		}
		ActionType;

		static const char* GetActionText( ActionType Action );

	public:
		ActionMap( void );

		int MaxActions( void );
		void DefaultActionMap( int i );
		void SetAction( ActionType Action, DefaultControlMapper::DIGITALBUTTON Button );
		DefaultControlMapper::DIGITALBUTTON GetAction( ActionType Action );
		void ClearActionMap( void );

	protected:
		DefaultControlMapper::DIGITALBUTTON m_Actions[Action_Max];
};

#endif // _FRONTEND_INPUTDRIVER_H_