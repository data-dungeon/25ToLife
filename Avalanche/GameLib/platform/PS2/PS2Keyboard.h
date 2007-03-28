// ----------------------------------------------------------------------------
// File			-> PS2Keyboard.h
// Created		-> April 25, 2004
// Author		-> Dwain Skinner
// Description	-> Platform dependent keyboard support
// ----------------------------------------------------------------------------
#if defined(NETWORKING)
#include "platform/CKeyboard.h"
#include <liblgkbm.h>

class CPlatformKeyboard : public CKeyboard
{

public:
	CPlatformKeyboard									( void );
	~CPlatformKeyboard								( void );

	enum eKeyStatus
	{
		KEY_NONE = 0,
		KEY_PRESSED,
		KEY_RELEASED,
		KEY_LOCKED,
		KEY_UNLOCKED,
	};

	bool initialize									( void );
	bool beginInput									( void );
	bool endInput										( void );
	void updateKeyboardDevice						( void );
	bool isKeyboardActive							( void ) { return( m_kLive ); }
	int getKeyboardData								( void );
	void displayKeyboardData						( void );
	bool isKeyPressed									( const char c );
	u8 getChar											( void );
	bool isShiftPressed								( void ) { return( m_shiftStatus == KEY_PRESSED ); }
	bool shiftReleased								( void ) { return( m_shiftStatus == KEY_RELEASED ); }
	bool isCapsLocked									( void ) { return( m_capsStatus == KEY_LOCKED ); }
	bool capsUnLocked									( void ) { return ( m_capsStatus == KEY_UNLOCKED ); }
	virtual bool isUpArrow							( void ) { return ( m_uArrow ); }
	virtual bool isDownArrow						( void ) { return ( m_dArrow ); }
	virtual bool isLeftArrow						( void ) { return ( m_lArrow ); }
	virtual bool isRightArrow						( void ) { return ( m_rArrow ); }

private:
	void updateSpecialKeys							( void );

	lgKbdStateInfo										m_keyboardState;
	lgKbdEventList										m_keyboardEventList;
	lgKbdXlatedList									m_keyboardXlatedList;					
	int													m_keyboardHint;
	int													m_keyboardHandle;
	eKeyStatus											m_shiftStatus;
	eKeyStatus											m_capsStatus;
	bool													m_kLive;
	bool													m_keyboardConnected;
	bool													m_uArrow;
	bool													m_dArrow;
	bool													m_lArrow;
	bool													m_rArrow;
};
#endif