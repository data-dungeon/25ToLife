// ----------------------------------------------------------------------------
// File			-> PS2Keyboard.cpp
// Created		-> April 25, 2004
// Author		-> Dwain Skinner
// Description	-> Platform dependent keyboard support
// ----------------------------------------------------------------------------
#if defined(NETWORKING)
#include "platform/PlatformPCH.h"
#include "platform/PS2/PS2Keyboard.h"
#include "platform/PS2/util.h"
#include "platform/PS2/Maptables.h"

#define MAX_KBDS                      1
static bool initialized = false;

#define KEY_SHIFT1_HID 0xe1
#define KEY_SHIFT2_HID 0xe5
#define KEY_CAPS_HID 0x39
#define KEY_UARROW_HID 0x52
#define KEY_DARROW_HID 0x51
#define KEY_LARROW_HID 0x50
#define KEY_RARROW_HID 0x4f

// ----------------------------------------------------------------------------
CPlatformKeyboard::CPlatformKeyboard( void )
{
	m_kLive					= false;
	m_keyboardConnected	= false;
	m_keyboardHint			= LGKBM_HINT_ENUMNEEDED;
	m_keyboardHandle		= LGKBM_INVALID_HANDLE;
	m_shiftStatus			= KEY_NONE;
	m_capsStatus			= KEY_NONE;
	m_uArrow					= false;
	m_dArrow					= false;
	m_lArrow					= false;
	m_rArrow					= false;
	memset( &m_keyboardState,      0, sizeof(m_keyboardState)      );
	memset( &m_keyboardEventList,  0, sizeof(m_keyboardEventList)  );
	memset( &m_keyboardXlatedList, 0, sizeof(m_keyboardXlatedList) );
}

// ----------------------------------------------------------------------------
CPlatformKeyboard::~CPlatformKeyboard( void )
{
}

// ----------------------------------------------------------------------------
bool CPlatformKeyboard::initialize( void )
{
	bool success = false;
	if( initialized ) return true;

//	if( !CPlatformMouse::isMouseActive( ) )
	if( false )
	{

		initialized = true;
		PS2App *app = PS2App::IAm();
		char usbd_param[] = "dev=11\0ed=32\0gtd=64\0itd=64\0ioreq=128\0hub=2\0";
		char  lgkbm_param[100];               // cmd line to be passed to lgKdmdrv
		int   size_param = 0;
		char  *thpri_param = "thpri=40";      // Initial Thread Priority is 40
		char  buf[10];

		// --- load the usb drivers
		if( ( success = app->LoadModule( "IRX\\USBD.IRX", sizeof( usbd_param ), usbd_param ) ) )
		{

			// Prepare command line parameter list for mouse+keyboard driver.
			myitoa(MAX_KBDS, buf);
			size_param += len_strcpy(lgkbm_param+size_param, LGKBM_PARAM_MAX_NUM_KBDS);
			size_param += len_strcpy(lgkbm_param+size_param,"=");
			size_param += len_strcpy(lgkbm_param+size_param, buf);
			size_param++;      // skip past the null terminator

			myitoa(MAX_MICE, buf);
			size_param += len_strcpy(lgkbm_param+size_param, LGKBM_PARAM_MAX_NUM_MICE);
			size_param += len_strcpy(lgkbm_param+size_param,"=");
			size_param += len_strcpy(lgkbm_param+size_param, buf);
			size_param++;      // skip past the null terminator

			size_param += len_strcpy(lgkbm_param+size_param, thpri_param);
			size_param++;      // increment the length to include null terminator

			// --- load the logitech keyboard and mouse driver
			success = app->LoadModule( "IRX\\LGKBM.IRX", size_param,  lgkbm_param );
			if( success )
			{
				success = ( lgKbMInit(MAX_KBDS, MAX_MICE, APPLICATION_THREAD_PRIORITY - 1, NULL) == LGKBM_SUCCESS );
			}
		}
	}
	else
	{
		success = true;
	}

	// --- set the correct language translation table
	switch ( sceScfGetLanguage() )
	{
		case SCE_FRENCH_LANGUAGE:
			setLanguage( CKeyboard::KEYBOARD_FRENCH );  
			break;

		case SCE_SPANISH_LANGUAGE:	
			setLanguage( CKeyboard::KEYBOARD_SPANISH ); 
			break;

		case SCE_ITALIAN_LANGUAGE:
			setLanguage( CKeyboard::KEYBOARD_ITALIAN ); 
			break;

		case SCE_GERMAN_LANGUAGE:
			setLanguage( CKeyboard::KEYBOARD_GERMAN );
			break;

		default:
			setLanguage( CKeyboard::KEYBOARD_ENGLISH ); 
			break;
	}

	switch( m_language )
	{
		case KEYBOARD_ENGLISH:
			lgKbdSetupTranslationTable( USEnglishKbdMapTable, szUSEnglishTable );
			break;

		case KEYBOARD_FRENCH:
			lgKbdSetupTranslationTable( FrenchKbdMapTable, szFrenchTable );
			break;

		case KEYBOARD_SPANISH:
			lgKbdSetupTranslationTable( SpanishKbdMapTable, szSpanishTable );
			break;

		case KEYBOARD_ITALIAN:
			lgKbdSetupTranslationTable( ItalianKbdMapTable, szItalianTable );
			break;

		case KEYBOARD_GERMAN:
			lgKbdSetupTranslationTable( GermanKbdMapTable, szGermanTable );
			break;

		default:
			lgKbdSetupTranslationTable( USEnglishKbdMapTable, szUSEnglishTable );
			break;
	}

	m_kLive = success;
	return( success );
}

// ----------------------------------------------------------------------------
bool CPlatformKeyboard::beginInput( void )
{
	// --- lets test for our devices first
	if( m_kLive )
	{
		// --- see if we need to find the mouse device
		lgKbdEnumHint( &m_keyboardHint );
		if( m_keyboardHint == LGKBM_HINT_ENUMNEEDED )
		{
			// --- we've either plugged or unplugged the mouse
			updateKeyboardDevice( );
		}

		// --- make sure we have a valid mouse
		if( m_keyboardHandle != LGKBM_INVALID_HANDLE )
		{
			// --- read the keyboard data
			getKeyboardData( );
		}
		return( true );
	}
	return( false );
}

// ----------------------------------------------------------------------------
bool CPlatformKeyboard::endInput( void )
{
	return( false );
}

// ----------------------------------------------------------------------------
void CPlatformKeyboard::updateKeyboardDevice( void ) 
{
	int                device_handle;
	int                rc;
	lgKbdDeviceDesc	desc;
	char              *pDeviceLocation;

	// --- enumerate the devices
	rc = lgKbdEnumerate( 0, &desc );
	if( rc != LGKBM_SUCCESS ) 
	{
		// --- we didn't find any devices
		m_keyboardHandle	= LGKBM_INVALID_HANDLE;
		m_shiftStatus		= KEY_NONE;
		m_capsStatus		= KEY_NONE;
		return ;
	}

	// --- Enumeration Succeeded. Lets open the device.
	rc = lgKbdOpen( 0, &m_keyboardHandle );
	if( rc != LGKBM_SUCCESS ) 
	{
		ASSERTS( false, "keyboard open failed" );
		return ;
	}

	// --- initial delay in ms, and repeat rate in ms
	lgKbdSetRepeatRate( 300, 75 );
	printf( "Keyboard #1 opened" );
	m_keyboardHint		= LGKBM_HINT_NOENUMNEEDED;
	return;
}

// ----------------------------------------------------------------------------
int CPlatformKeyboard::getKeyboardData( void )
{
	int rc = LGKBM_SUCCESS;

	// --- Read the keyboard state bitmap
	rc = lgKbdReadState( m_keyboardHandle, &m_keyboardState );
	if( rc == LGKBM_ERR_DEVICE_LOST ) 
	{
		return rc;
	}

	// -- other errors?
	if( rc ) 
	{
		return rc;
	}

	// --- Get the key press/release event list.
	rc = lgKbdRead( m_keyboardHandle, &m_keyboardEventList );
	if( rc == LGKBM_ERR_DEVICE_LOST ) 
	{
		return rc;
	}

	// --- other errors?
	if(rc != LGKBM_SUCCESS ) 
	{
		return rc;
	}

	// --- Translate the key press/release events into characters.
	rc = lgKbdTranslate(m_keyboardHandle, &m_keyboardEventList, &m_keyboardXlatedList );
	updateSpecialKeys( );

#ifndef CDROM
	displayKeyboardData( );
#endif

	return LGKBM_SUCCESS;
}

// ----------------------------------------------------------------------------
bool CPlatformKeyboard::isKeyPressed( const char c )
{
	unsigned char hid=0xff;

	if( c == 'a' ) hid = 0x04;
	if( c == 'd' ) hid = 0x07;
	if( c == 'w' ) hid = 0x1a;
	if( c == 's' ) hid = 0x16;

	return( lgIsKeyPressed( &m_keyboardState, hid ) );

	if( m_keyboardXlatedList.num_keys ) 
	{
		int i;
		for( i = 0; i < m_keyboardXlatedList.num_keys; ++i ) 
		{
			char key = m_keyboardXlatedList.xlated_char[ i ];
			if( c == key )
			{
				return( true );
			}
		}
	}
	return( false );
}

// ----------------------------------------------------------------------------
void CPlatformKeyboard::displayKeyboardData( void )
{
	lgKbdEvent *event;
	int i;


	for( i = 0; i < m_keyboardEventList.num_events; ++i ) 
	{
		event = &m_keyboardEventList.events[ i ];
		printf("Kbd#(%d):Event Time =%u, led = 0x%x, modkey = 0x%x, key 0x%x %s\n",
			1, event->key_info.time_stamp, event->led_state, event->modifier_state, 
			event->key_info.key_code, get_event_type(event->event_type));
	}

	if( m_keyboardXlatedList.num_keys ) 
	{
		printf("Kbd#(%d):Translated Char = ", 0 );
		for( i = 0; i < m_keyboardXlatedList.num_keys; ++i ) 
		{
			printf(" 0x%x", m_keyboardXlatedList.xlated_char[ i ] );
			if(!( m_keyboardXlatedList.xlated_char[ i ] & LGKBD_RAWHID_MASK ) ) 
			{
				printf(" (%c)", m_keyboardXlatedList.xlated_char[ i ] );
			}
		}
		printf("\n");
	}
}

// ----------------------------------------------------------------------------
u8 CPlatformKeyboard::getChar( void )
{
	if( m_keyboardXlatedList.num_keys )
	{
		if(!( m_keyboardXlatedList.xlated_char[ 0 ] & LGKBD_RAWHID_MASK ) ) 
			return( m_keyboardXlatedList.xlated_char[ 0 ] );
	}
	return 0;
}

// ----------------------------------------------------------------------------
void CPlatformKeyboard::updateSpecialKeys( void )
{
	// --- shift status
	if( lgIsKeyPressed( &m_keyboardState, KEY_SHIFT1_HID ) || lgIsKeyPressed( &m_keyboardState, KEY_SHIFT2_HID ) )
	{
		m_shiftStatus = KEY_PRESSED;
	}
	else if( m_shiftStatus == KEY_PRESSED )
	{
		m_shiftStatus = KEY_RELEASED;
	}
	else m_shiftStatus = KEY_NONE;

	// --- caps lock status
	for( int i = 0; i < m_keyboardEventList.num_events; ++i ) 
	{
		lgKbdEvent *event;
		event = &m_keyboardEventList.events[ i ];
		if( event->led_state & LGKBD_CAPSLOCK_MASK )
		{
			m_capsStatus = KEY_LOCKED;
		}
		else
		{
			m_capsStatus = KEY_UNLOCKED;
		}
	}

	// --- look for arrow keys
	m_uArrow = false;
	m_dArrow = false;
	m_lArrow = false;
	m_rArrow = false;
	if( m_keyboardXlatedList.num_keys ) 
	{
		for( int i = 0; i < m_keyboardXlatedList.num_keys; ++i ) 
		{
			if( m_keyboardXlatedList.xlated_char[ i ] & LGKBD_RAWHID_MASK )
			{
				s8 key = m_keyboardXlatedList.xlated_char[ i ] & 0xff;
				if( key == KEY_UARROW_HID ) m_uArrow = true;
				if( key == KEY_DARROW_HID ) m_dArrow = true;
				if( key == KEY_LARROW_HID ) m_lArrow = true;
				if( key == KEY_RARROW_HID ) m_rArrow = true;
			}
		}
	}
}
#endif
