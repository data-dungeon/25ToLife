#ifndef SCREEN_KEYBOARD_H
#define SCREEN_KEYBOARD_H

#include "frontend/screens/screen_BaseClass.h"

#define KEYBOARD_OK		(1)
#define KEYBOARD_CANCEL	(0)

#define KEYBOARD_MAX_ROWS		(8)
#define KEYBOARD_MAX_COLS		(32)

class ScreenKeyboard : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenKeyboard );

	typedef struct
	{
		int id;
		Sprite* sprite;
		u8 current;
		union
		{
			struct
			{
				u8 normal;
				u8 accent;
				u8 symbol;
			};
			u8 ch[3];
		};
	}
	KeyDef;

	typedef enum
	{
		None,
		Normal,
		Accents,
		Symbols,
	}
	KeyMap;

	enum eAscii
	{
		KEY_NULL = 0,
		KEY_SOH, KEY_STX, KEY_ETX, KEY_EOT, KEY_ENQ, KEY_ACK, KEY_BELL, KEY_BS,
		KEY_HT, KEY_LF, KEY_VT, KEY_FF, KEY_CR, KEY_SO, KEY_SI, KEY_DLE,
		KEY_DC1, KEY_DC2, KEY_DC3, KEY_DC4, KEY_NAK, KEY_SYN, KEY_ETB, KEY_CAN,
		KEY_EM, KEY_SUB, KEY_ESC, KEY_FS, KEY_GS, KEY_RS, KEY_US, KEY_SP,
		KEY_EXCLAM, KEY_DBLQUOTE, KEY_POUND, KEY_DOLLAR, KEY_PERCENT, KEY_AMP, KEY_SNGQUOTE, KEY_LPAREN,
		KEY_RPAREN, KEY_ASTERISK, KEY_PLUS, KEY_COMMA, KEY_MINUS, KEY_PERIOD, KEY_FWDSLASH, KEY_ZERO,
		KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE,
		KEY_COLON, KEY_SEMI, KEY_LESS, KEY_EQUAL, KEY_GREATER, KEY_QUESTION, KEY_AT, KEY_A,
		KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N,
		KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z, KEY_LBRACKET,
		KEY_BACKSLASH, KEY_RBRACKET, KEY_CARET, KEY_UNDERSCORE, KEY_ACCENT, KEY_a, KEY_b, KEY_c, KEY_d,
		KEY_e, KEY_f, KEY_g, KEY_h, KEY_i, KEY_j, KEY_k, KEY_l, KEY_m, KEY_n, KEY_o, KEY_p, KEY_q, KEY_r,
		KEY_s, KEY_t, KEY_u, KEY_v, KEY_w, KEY_x, KEY_y, KEY_z, KEY_LBRACE, KEY_BAR, KEY_RBRACE, KEY_TILDE,
		KEY_DEL,
		KEY_MAX_KEYS,
	};

	enum DefaultKey
	{
		ENTER_KEY,
		CLEAR_KEY
	};


	public:
		ScreenKeyboard( void );
		virtual ~ScreenKeyboard( void );

		bool InitKeys( int rows, int cols );
		void KillKeys( void );
		void SetKey( int row, int col, int id, u8 normal, u8 accent, u8 symbol );
		void Clear( void );
		const char* GetString( void );
		void SetString( const char* pString );
		void InsertChar( u8 ch );
		void BackSpace( int Count = 1 );
		int MaxLength( void );
		int MaxRows( void );
		int MaxCols( void );
		void MoveCursor( int x, int y );
		void SelectKey( int Key );
		int GetKey( void );
		int GetKey( int row, int col );
		KeyDef* GetKeyDef( int row, int col );
		u8 GetChar( void );
		void RestrictKey( bool Restrict, int ch );
		void RestrictKeys( bool Restrict, int min_ch, int max_ch );
		void RestrictChars( bool Restrict, const char* pString );
		bool Restricted( u8 ch );
		void UpdateKeys( void );

		void SetDefaultKey( DefaultKey key );
		void HighlightKey( int KeyId );
		void UnhighlightKey( int KeyId );
		void HighlightKey( Sprite* pKey );
		void UnhighlightKey( Sprite* pKey );
		void ShowLed( bool Show, int Key );
		void SetAutoHide( bool AutoHide );

		void SetKeyMap( KeyMap km, bool Shifted );

		void SetAccents( bool TrueOrFalse ) { m_Accents  = TrueOrFalse; }
		void SetCapsLock( bool TrueOrFalse ){ m_CapsLock = TrueOrFalse; }
		void SetShift( bool TrueOrFalse )   { m_Shift    = TrueOrFalse; }
		void SetSymbols( bool TrueOrFalse ) { m_Symbols  = TrueOrFalse; }

		bool GetAccents( void );//  { return m_Accents; }
		bool GetCapsLock( void );// { return m_CapsLock;}
		bool GetShift( void )    { return m_Shift;   }
		bool GetSymbols( void )  { return m_Symbols; }

		void ToggleAccents( void );
		void ToggleCapsLock( void );
		void ToggleShift( void );
		void ToggleSymbols( void );

		bool EnterString( void );
		bool ValidateString( void );
		void AcceptString( void );

		void SetPasswordMode( bool PasswordMode );

		void SetMaxString( int n ) { m_MaxString = n; }
		int GetMaxString( int n )  { return m_MaxString; }

		void ShowDimKeyboard( bool Dim );

	protected:
		Text* GetDisplayText( void );
		void UpdateDisplayText( void );

		virtual void OnInitialize( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
		virtual void OnButtonPressed( int Button );
		virtual void HandleInput( UIHandler* pUIHandler );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual void OnScreenUpdate( void );
		virtual void OnPopupShow( void );
		virtual void OnPopupHide( void );

		void StepUp( void );
		void StepDown( void );
		void StepLeft( void );
		void StepRight( void );

		void GetKeyboardKey( void );
		void MoveCursorTo( int row, int col );

		Sprite m_OldKey;
		Sprite m_NewKey;

	protected:
		Text m_KeyEffect;
		Text* m_pDisplayText;
		KeyDef* m_pKeyLayout;
		Sprite** m_pKeySprites;
		char m_String[1024];
		char m_RestrictedKeys[256];
		int m_Rows;
		int m_Cols;
		int m_StringPos;
		int m_CursorX;
		int m_CursorY;
		int m_MaxString;

		bool m_Accents;
		bool m_ShiftAccents;
		bool m_CapsLock;
		bool m_ShiftCapsLock;
		bool m_Shift;
		bool m_Symbols;
		bool m_AutoHide;
		bool m_PasswordMode;
		bool m_ResetPasswordMode;
#ifdef DIRECTX_PC
		bool m_IgnoreKeyShift;
#endif

		KeyMap m_KeyMap;
		bool m_Shifted;
		int m_DefaultKey;


};

#endif // SCREEN_KEYBOARD_H
