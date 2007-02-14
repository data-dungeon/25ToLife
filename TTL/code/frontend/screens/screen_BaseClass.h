#ifndef SCREEN_BASECLASS_H
#define SCREEN_BASECLASS_H

#include "hud/hudlib.h"
#include "frontend/savegame.h"

#if defined(PS2) && !defined(_DEBUG)
#define _CLOSED_BETA
#endif

class ScreenKeyboard;
class ScreenPopupDialog;

class ScreenBaseClass : public ScreenHandler
{
	DECLARE_CLASSOBJECT( ScreenBaseClass );

	typedef struct
	{
		float m_Red;
		float m_Green;
		float m_Blue;
		float m_Alpha;
		int m_SpriteId;
		float m_ScaleX;
		float m_ScaleY;
	}
	HighlightInfo;

	class ScreenParams
	{
		public:
			ScreenParams( void );
			void Default( void );

			float IntroTime;
			float ExitTime;
			int HelpRect;
	};

	typedef enum
	{
		SAVEGAME_IDLE,
		SAVEGAME_SAVE,
		SAVEGAME_LOAD,
	}
	SAVEGAMESTATE;

	class InterfaceRecord : public TreeNode
	{
		DECLARE_TREENODE( InterfaceRecord );

		public:
			InterfaceRecord( void );
			InterfaceRecord( SpriteInterface* pInterface );
			void SetInterface( SpriteInterface* pInterface );
			SpriteInterface* GetInterface( void );
			SpriteInterface* GetInterface( const char* pInterfaceType );

		protected:
			SpriteInterface* m_pInterface;
	};

	public:
		ScreenBaseClass( void );
		virtual ~ScreenBaseClass( void );

		void UpdateInterfaces( void );
		bool RegisterInterface( SpriteInterface* pInterface );
		SpriteInterface* AddInterface( int SpriteId, const char* pInterfaceType );
		GuiListBox* CreateListBox( int SpriteId );
		GuiScrollBar* CreateScrollBar( int SpriteId );
		GuiSlider* CreateSlider( int SpriteId );
		GuiProgressBar* CreateProgressBar( int SpriteId );
		GuiTable* CreateTable( int SpriteId );
		GuiCycleButton* CreateCycleButton( int TextId );
		GuiScrollingText* CreateScrollingText( int SpriteId );

		InterfaceRecord* GetInterfaceRecord( const char* pInterfaceType );
		SpriteInterface* GetInterface( const char* pInterfaceType );
		SpriteInterface* GetInterface( int SpriteId, const char* pInterfaceType );
		GuiScrollBar* GetScrollBar( void );
		GuiListBox* GetListBox( void );
		GuiTable* GetTable( void );
		GuiScrollingText* GetScrollingText( void );

		void ShowScreenIntro( void );
		void ShowScreenExit( void );

		void UseHighlighting( bool TrueOrFalse );
		void SetHighlightPercentage( float r, float g, float b, float a );
		bool SetHighlightCount( int n );
		void InitTextEffects( void );
		void StartTextEffects( void );

		ScreenKeyboard* GetKeyboard( void );
		const char* GetKeyboardString( void );
		void ShowKeyboard( void );

		ScreenPopupDialog* GetPopupDialog( void );
		void ShowPopupDialog( void );

		void SaveProfile( void );
		void SaveProfile( const char* pProfileName );
		void SaveNewProfile( const char* pProfileName );

		void SetHelpRect( int HelpRectId );
		Sprite* GetHelpRect( void );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenUpdate( void );
		virtual void OnScreenExit( void );
		virtual float GetScreenIntroTime( void );
		virtual float GetScreenExitTime( void );
		virtual void OnButtonPressed( int Button );
		virtual void OnButtonReleased( int Button );
		virtual void OnButtonSelect( Button* pButton, Sprite* pSprite );
		virtual void OnButtonUnselect( Button* pButton, Sprite* pSprite );
		virtual void OnPopupShow( void );
		virtual void OnPopupHide( void );
		virtual void OnKeyboardKeyPressed( int Key );
		virtual void OnKeyboardInput( const char* pString );
		virtual void OnKeyboardCancel( void );
		virtual void OnDialogCommand( int Command );
		virtual void OnDialogCancel( void );
		virtual void OnSaveGameUpdate( void );
		virtual void OnSaveGameIdle( void );
	//	virtual void OnSaveGameSave( void );
	//	virtual void OnSaveGameLoad( void );
		virtual void OnSaveGameResult( int Result );
		virtual void OnScrollBarUpdate( GuiScrollBar* pScrollBar );
		virtual void HandleInput( UIHandler* pUIHandler );

		CMessageOwner m_MessageOwner;
		REF_BASIC_MESSAGE_HANDLER( KeyboardKeyPressed,  MESSAGE_STRING, ScreenBaseClass ) m_KeyboardKeyPressedHandler;
		REF_BASIC_MESSAGE_HANDLER( KeyboardInput,       MESSAGE_VALUE,  ScreenBaseClass ) m_KeyboardInputHandler;
		REF_BASIC_MESSAGE_HANDLER( KeyboardCancel,      MESSAGE_VALUE,  ScreenBaseClass ) m_KeyboardCancelHandler;
		REF_BASIC_MESSAGE_HANDLER( DialogCommand,       MESSAGE_VALUE,  ScreenBaseClass ) m_DialogCommandHandler;
		REF_BASIC_MESSAGE_HANDLER( DialogCancel,        MESSAGE_VALUE,  ScreenBaseClass ) m_DialogCancelHandler;

	protected:
		ScreenParams m_ScreenParams;
		InterfaceRecord m_InterfaceRecords;
		SPRITESTATESTRUCT m_InitialButtonState;
		HighlightInfo* m_pHighlightInfo;
		int m_HighlightCount;
		float m_hlRed;
		float m_hlGreen;
		float m_hlBlue;
		float m_hlAlpha;
		bool m_UseHighlighting;
		bool m_ScaleOnIntro;
#ifdef DIRECTX_PC
		Text StartButton;
		Text BackButton;
		Text QuitButton;
#endif

		static ts_SaveProfile m_LastProfileSaved;

		SAVEGAMESTATE m_SaveGameState;
};

#endif // SCREEN_BASECLASS_H