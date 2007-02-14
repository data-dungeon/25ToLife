#ifndef SCREEN_CUSTOMPLAYER_H
#define SCREEN_CUSTOMPLAYER_H

#include "frontend/screens/screen_BaseClass.h"
#include "weapons/armoryview.h"

//=============================================================================
// ScreenCustomBase
//=============================================================================

class ScreenCustomBase : public ScreenBaseClass
{
	protected:
		virtual void HandleInput( UIHandler* pUIHandler );
};


//=============================================================================
// ScreenCharSelect2
//
//		This is the screen where you select which character you'd like to edit.
//=============================================================================

class ScreenCharSelect2 : public ScreenCustomBase
{
	DECLARE_CLASSOBJECT( ScreenCharSelect2 );

	public:
		ScreenCharSelect2( void );

		GuiListBox* GetListBox( void );
		void SetCharType( int CharType );
		void ChangeCharacter( int n );
		void OnMenuChange( void );
		Sprite* GetIcon( int n );
		int GetIconId( int n );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
		virtual void OnScreenUpdate( void );

	protected:
		char m_szProfileName[ 128 ];
		int m_iSaveGamePort;
};

//=============================================================================
// ScreenCharSelect3
//
//		This is where you select from a list of prebuilt characters.
//=============================================================================

class ScreenCharSelect3 : public ScreenCustomBase
{
	DECLARE_CLASSOBJECT( ScreenCharSelect3 );

	public:
		GuiListBox* GetListBox( void );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
		virtual void OnDialogCommand( int Command );
		virtual void OnPopupShow( void );
		virtual void OnPopupHide( void );
};

//=============================================================================
// ScreenCustomMenu
//=============================================================================

class ScreenCustomMenu : public ScreenCustomBase
{
	DECLARE_CLASSOBJECT( ScreenCustomMenu );

	public:
		void SetDefaultCharacter( void );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void	);
		virtual void OnScreenExit( void );
};

//=============================================================================
// ScreenCustomName
//=============================================================================

class ScreenCustomName : public ScreenCustomBase
{
	DECLARE_CLASSOBJECT( ScreenCustomName );

	protected:
		virtual void OnScreenIntro( void );
		virtual void OnScreenRefresh( void );
		virtual void OnKeyboardInput( const char* pString );
		virtual void OnKeyboardCancel( void );
};

//=============================================================================
// ScreenCustomEdit
//
//		This is the screen that shows and selects what pieces of clothing are
//		available.  The menus here are dynamic.
//=============================================================================

class ScreenCustomEdit : public ScreenCustomBase
{
	DECLARE_CLASSOBJECT( ScreenCustomEdit );

	public:
		typedef enum
		{
			EditMode_Head,
			EditMode_Body,
			EditMode_Arms,
			EditMode_Hands,
			EditMode_Legs,
			EditMode_Feet,
		}
		EditMode;

		ScreenCustomEdit( void );
		void SetEditMode( EditMode Mode );
		GuiListBox* GetListBox( void );
		void ShowIcon( int Item, bool Show );

	protected:
		void InitListboxSelections( void );

		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenUpdate( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
		virtual void OnButtonPressed( int Button );
		virtual void OnGuiMessage( GuiControl* pControl, int message, void* pParam );
};

//=============================================================================
// ScreenCustomPlayer
//
//		This is the screen where you select what part of the body you want to
//		edit.
//=============================================================================

class ScreenCustomPlayer : public ScreenCustomBase
{
	DECLARE_CLASSOBJECT( ScreenCustomPlayer );

	public:
		ScreenCustomPlayer( void );
		virtual ~ScreenCustomPlayer();

		void GotoCustomScreen( const char* pScreenName, ScreenCustomEdit::EditMode Mode );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual void OnScreenRefresh( void );
		virtual void OnCommand( int Command );
		virtual void OnCommandHead( void );
		virtual void OnCommandBody( void );
		virtual void OnCommandArms( void );
		virtual void OnCommandHands( void );
		virtual void OnCommandLegs( void );
		virtual void OnCommandFeet( void );
		virtual void OnCommandPrebuilt( void );
		virtual void OnCommandClear( void );
		virtual void OnDialogCommand( int Command );
		virtual void OnPopupShow( void );
		virtual void OnPopupHide( void );

		bool m_ShowCustomScreen;
};

//=============================================================================
// ScreenCustomWeapons
//
//		This is the screen where you edit the loadout.
//=============================================================================

class ScreenCustomWeapons : public ScreenCustomBase
{
	DECLARE_CLASSOBJECT( ScreenCustomWeapons );

	public:
		void UpdateWeaponInHand( void );
		void ChangeWeaponInHand( int n );
		void CommitWeaponLoadout( void );
		void RevertWeaponLoadout( void );
		int GetCharacterIndex( void );
		int GetVariant( void );
		ArmoryView::PlayerType GetPlayerType( void );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenUpdate( void );
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnButtonCancel( void );
		virtual void OnButtonSelect( Button* pButton, Sprite* pSprite );
};

#endif // SCREEN_CUSTOMPLAYER_H