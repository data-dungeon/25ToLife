// ----------------------------------------------------------------------------
// File			-> CKeyboard.h
// Created		-> April 25, 2004
// Author		-> Dwain Skinner
// Description	-> Platform independent keyboard support
// ----------------------------------------------------------------------------


class CKeyboard
{

public:
	CKeyboard											( ) {}
	virtual ~CKeyboard								( ) {}

	enum eKeyboardLanguage
	{
		KEYBOARD_ENGLISH = 0,
		KEYBOARD_FRENCH,
		KEYBOARD_SPANISH,
		KEYBOARD_ITALIAN,
		KEYBOARD_GERMAN
	};

	virtual bool initialize							( void ){ return false; }
	virtual bool beginInput							( void ){ return false; }
	virtual bool endInput							( void ){ return false; }
	virtual bool isKeyboardActive					( void ){ return false; }
	virtual bool isKeyPressed						( const char c ) { return false; }
	virtual u8 getChar								( void ) { return 0; }
	virtual bool isShiftPressed					( void ) { return false; }
	virtual bool shiftReleased						( void ) { return false; }
	virtual bool isCapsLocked						( void ) { return false; }
	virtual bool capsUnLocked						( void ) { return false; }
	virtual bool isUpArrow							( void ) { return false; }
	virtual bool isDownArrow						( void ) { return false; }
	virtual bool isLeftArrow						( void ) { return false; }
	virtual bool isRightArrow						( void ) { return false; }
	static void setLanguage							( eKeyboardLanguage lang = KEYBOARD_ENGLISH );

	static eKeyboardLanguage						m_language;
private:

protected:
};
