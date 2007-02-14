#ifndef _SCREEN_WEAPONS_H_
#define _SCREEN_WEAPONS_H_

#include "frontend/screens/screen_BaseClass.h"
#include "data/screens/NetworkGames.h"
#include "data/screens/pause.h"
#include "weapons/armoryview.h"

class ScreenWeapons : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenWeapons );

	typedef enum
	{
		MENUMODE_NOTINITIALIZED,
		MENUMODE_TEAMSELECT,
		MENUMODE_WEAPONSELECT,
	}
	MENUMODE;

	public:
		ScreenWeapons();

	public:
		void Update();
		void RevertWeaponLoadout( int variant);
		void CommitWeaponLoadout( int variant );
		void UseDefaultLoadout( int variant );

		virtual int SlotToButtonId( int Slot );
		virtual int ButtonIdToSlot( int ButtonId );
		virtual int GetParamId() { return ID_WEAPONS_PARAM; }
		virtual int GetLabelId() { return ID_WEAPONS_LABEL; }
		virtual int TeamToButtonId( int Team );
		virtual int ButtonIdToTeam( int ButtonId );

		void SetMenuMode( MENUMODE MenuMode );
		MENUMODE GetMenuMode();

		GuiCycleButton* SetCycleButton( int ButtonId );
		GuiCycleButton* GetCycleButton( int ButtonId );
		void SetButtonLabel( int ButtonId, const char* pLabel );
		void ShowWeapons( bool Show );

	protected:
		virtual void OnInitialize();
		virtual void OnScreenIntro();
		virtual void OnCursorLeft();
		virtual void OnCursorRight();
		virtual void OnButtonCycle( int N );
		virtual void OnButtonCancel();
		virtual void OnButtonSelect( Button* pButton, Sprite* pSprite );
		virtual void OnButtonPressed( int Button );
		virtual void OnCommand( int Command );
		virtual void OnCommandDone();
#ifdef DIRECTX_PC
		virtual void OnScreenUpdate();
#endif

		MENUMODE m_MenuMode;
		int m_Variant;
		int m_PreviousButton;

		ArmoryView::PlayerType GetPlayerType(int variant);
		ArmoryView::PlayerType GetPlayerType()
			{ return GetPlayerType(m_Variant); }
};

class ScreenWeaponsInGame : public ScreenWeapons
{
	DECLARE_CLASSOBJECT( ScreenWeaponsInGame );

	public:
		virtual int SlotToButtonId( int Slot );
		virtual int GetParamId() { return ID_WEAPONS2_PARAM; }
		virtual int GetLabelId() { return ID_WEAPONS2_LABEL; }

	protected:
		virtual void OnInitialize();
		virtual void OnButtonOK();
		virtual void OnButtonCancel();
};

#endif // _SCREEN_WEAPONS_H_
