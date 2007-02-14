#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "frontend/screens/screen_Keyboard.h"
#include "data/screens/Keyboard2.h"

ScreenKeyboard::ScreenKeyboard( void )
{
	m_pDisplayText = 0;
	m_pKeyLayout = 0;
	m_Rows = 0;
	m_Cols = 0;
	m_CursorX = 0;
	m_CursorY = 0;

	m_MaxString = 0;

	m_Accents = false;
	m_CapsLock = false;
	m_Shift = false;
	m_Symbols = false;
	m_AutoHide = true;
	m_PasswordMode = false;
	m_ResetPasswordMode = false;

	m_KeyMap = None;
	m_Shifted = false;

	m_DefaultKey = ID_KEYBOARD_CLEAR;

	Clear();

}

ScreenKeyboard::~ScreenKeyboard( void )
{
	m_KeyEffect.Remove();

	KillKeys();
}

bool ScreenKeyboard::InitKeys( int rows, int cols )
{
	KillKeys();

	m_pKeyLayout = new KeyDef[rows*cols];

	if( !m_pKeyLayout )
		return false;

	m_Rows = rows;
	m_Cols = cols;

	memset( m_pKeyLayout, 0, sizeof(KeyDef) * rows * cols );

	return true;
}

void ScreenKeyboard::KillKeys( void )
{
	if( m_pKeyLayout )
	{
		delete [] m_pKeyLayout;
		m_pKeyLayout = 0;
		m_Rows = 0;
		m_Cols = 0;
	}

	return;
}

void ScreenKeyboard::SetKey( int row, int col, int id, u8 normal, u8 accent, u8 symbol )
{
	int n = col + row * m_Cols;

	m_pKeyLayout[n].id = id;
	m_pKeyLayout[n].current = normal;
	m_pKeyLayout[n].normal = normal;
	m_pKeyLayout[n].accent = accent;
	m_pKeyLayout[n].symbol = symbol;

	return;
}

void ScreenKeyboard::Clear( void )
{
	memset( m_String, 0, sizeof(m_String) );
}

const char* ScreenKeyboard::GetString( void )
{
	return m_String;
}

void ScreenKeyboard::SetString( const char* pString )
{
	int n = MaxLength();

	if( pString )
		strncpy( m_String, pString, n );
	else
		m_String[0] = 0;

	m_String[n] = 0;
}

void ScreenKeyboard::InsertChar( u8 ch )
{
	int len = xstrlen( m_String );
	if( len < MaxLength() )
	{
		if( !Restricted(ch) )
		{
			m_String[len+0] = ch;
			m_String[len+1] = 0;

			// the key was allowed
			PlaySound( "keyboard_input" );

			// disable shift
			SetShift( false );

			// send the event message
			g_messageDispatcher.SendMessageToAll( "KeyboardKeyPressed", (void*) (int) ch, INVALID_OBJECT_HANDLE );
		}
	}
	else
	{
		// the key was not allowed
		PlaySoundEvent( Screen::SOUNDEVENT_INVALID );
	}

	return;
}

void ScreenKeyboard::BackSpace( int Count )
{
	int n = xstrlen( m_String );
	int i;

	if( Count > n )
		Count = n;

	for( i = n - Count; i < n; i++ )
		m_String[i] = 0;

	if( n > 0 )
		PlaySound( "keyboard_backspace" );

	return;
}

int ScreenKeyboard::MaxLength( void )
{
	int n = sizeof(m_String)/sizeof(m_String[0])-1;

	if( m_MaxString > 0 && m_MaxString < n )
		return m_MaxString;

	return n;
}

int ScreenKeyboard::MaxRows( void )
{
	return m_Rows;
}

int ScreenKeyboard::MaxCols( void )
{
	return m_Cols;
}

void ScreenKeyboard::MoveCursorTo( int row, int col )
{
	int oldKey = GetKey();
	Sprite* pOldKey = GetSprite( oldKey );
	m_CursorX = col;
	m_CursorY = row;
	int newKey = GetKey();
	Sprite* pNewKey = GetSprite( GetKey() );

	if( pNewKey != pOldKey )
	{
		HighlightKey(pNewKey);
		UnhighlightKey( pOldKey );
		PlaySound( "keyboard_move" );
	}
}

void ScreenKeyboard::MoveCursor( int x, int y )
{
	if( !m_pKeyLayout )
		return;

	int StepX = 0;
	int StepY = 0;

	if( x > 0 )
		StepX =  1;
	else
	if( x < 0 )
		StepX = -1;

	if( y > 0 )
		StepY =  1;
	else
	if( y < 0 )
		StepY = -1;

	if( x < 0 ) x = 0 - x;
	if( y < 0 ) y = 0 - y;

	int i;
	int Rows = MaxRows();
	int Cols = MaxCols();

	int OldKey = GetKey();
	int NewKey = OldKey;

	int cx = m_CursorX;
	int cy = m_CursorY;

	Sprite* pOldKey = GetSprite( OldKey );
	
	for( i = 0; i < y; i++ )
	{
		while( OldKey == NewKey || NewKey == 0 )
		{
			m_CursorY += StepY;

			if(m_CursorY < 0 )
				m_CursorY += Rows;
			else
			if(m_CursorY >= Rows )
				m_CursorY -= Rows;

			NewKey = GetKey( m_CursorY, m_CursorX );

			if( cy == m_CursorY )
				break;
		}
	}

	for( i = 0; i < x; i++ )
	{
		while( OldKey == NewKey || NewKey == 0 )
		{
			m_CursorX += StepX;

			if(m_CursorX < 0 )
				m_CursorX += Cols;
			else
			if(m_CursorX >= Cols )
				m_CursorX -= Cols;

			NewKey = GetKey( m_CursorY, m_CursorX );

			if( cx == m_CursorX )
				break;
		}
	}

	Sprite* pNewKey = GetSprite( GetKey() );

	if( pNewKey != pOldKey )
	{
		if( StepY != 0 || StepX != 0 )
			PlaySound( "keyboard_move" );

		HighlightKey(pNewKey);
		UnhighlightKey( pOldKey );
	}

	return;
}

void ScreenKeyboard::SetDefaultKey( DefaultKey key )
{
	switch( key )
	{
	case ENTER_KEY:
		m_DefaultKey = ID_KEYBOARD_ENTER;
		break;
	case CLEAR_KEY:
		m_DefaultKey = ID_KEYBOARD_CLEAR;
		break;
	}

	SelectKey( m_DefaultKey );
}

void ScreenKeyboard::SelectKey( int Key )
{
	int rows = MaxRows();
	int cols = MaxCols();
	int x, y;

	int OldKey = GetKey();

	if( Key == OldKey )
		return;

	UnhighlightKey( OldKey );

	for( y = 0; y < rows; y++ )
	{
		for( x = 0; x < cols; x++ )
		{
			if( GetKey( y, x ) == Key )
			{
				m_CursorX = x;
				m_CursorY = y;

				HighlightKey( Key );

				return;
			}
		}
	}

	return;
}

Text* ScreenKeyboard::GetDisplayText( void )
{
	return m_pDisplayText;
}

void ScreenKeyboard::UpdateDisplayText( void )
{
	Text* pDisplayText = GetDisplayText();

	if( pDisplayText )
	{
		const char* pString = m_String;
		char* pPasswordString = 0;

		if( m_PasswordMode )
		{
			pPasswordString = new char[128];

			if( pPasswordString )
				xstrncpyz( pPasswordString, m_String, 128 );

			for( int i = 0; i < 128; i++ )
			{
				if( pPasswordString[i] == 0 )
					break;

				pPasswordString[i] = '*';
			}

			pString = pPasswordString;
		}

		TextOutput to;
		float w, h;

		pDisplayText->GetSize(w, h);
		pDisplayText->SetSize(0.0f, h);
		pDisplayText->SetTextNoTranslate( pString );
		pDisplayText->ComputeTextOutput(&to);
		pDisplayText->SetSize(w, h);

		if( to.Width > w )
		{
			// this is when the string is large enough that it has exceeded its display rectangle
			// we're going to chop off as little as possile from the beginning and display the rest

			char* smaller_string;
			int smaller_count;
			int smaller_pos;
			int count = xstrlen(pString)+4;

			if( count > 0 )
			{
				smaller_string = new char[count];

				if( smaller_string )
				{
					smaller_count = count;
					smaller_pos = count - smaller_count;

					while( to.Width > w )
					{
						smaller_count = smaller_count / 2;
						smaller_pos = count - smaller_count;

						xstrncpyz( smaller_string, "...", count );
						xstrncatz( smaller_string, &pString[smaller_pos], count );

						if( smaller_count < 4 ) // panic!
							break;

						pDisplayText->SetSize( 0.0f, h );
						pDisplayText->SetTextNoTranslate( smaller_string );
						pDisplayText->ComputeTextOutput( &to );
						pDisplayText->SetSize( w, h );
					}

					while( to.Width < w )
					{
						smaller_count = smaller_count + (count - smaller_count + 1) / 2;
						smaller_pos = count - smaller_count;

						if( smaller_count > count )
							smaller_count = count;

						xstrncpyz( smaller_string, "...", count );
						xstrncatz( smaller_string, &pString[smaller_pos], count );

						pDisplayText->SetSize( 0.0f, h );
						pDisplayText->SetTextNoTranslate( smaller_string );
						pDisplayText->ComputeTextOutput( &to );
						pDisplayText->SetSize( w, h );

						if( smaller_count == count )
							break;
					}

					while( to.Width > w )
					{
						smaller_count--;
						smaller_pos = count - smaller_count;

						xstrncpyz( smaller_string, "...", count );
						xstrncatz( smaller_string, &pString[smaller_pos], count );

						if( smaller_count < 4 ) // panic!
							break;

						pDisplayText->SetSize( 0.0f, h );
						pDisplayText->SetTextNoTranslate( smaller_string );
						pDisplayText->ComputeTextOutput( &to );
						pDisplayText->SetSize( w, h );
					}

					delete [] smaller_string;
				}
			}
		}

		if( pPasswordString )
			delete [] pPasswordString;
	}

	return;
}

void ScreenKeyboard::GetKeyboardKey( )
{
	ControllerInputDriver *c = g_input.GetController( 0 );
	u8 key;

	// sorry to hack up the code like this, but this
	// was the best way I could see to have both
	// the shift key functionality and still keep
	// the shift button on the onscreen keyboard
	// working like you'd expect

	// --- get shift status
	if( c->isShiftPressed( ) )
	{
		SetShift( true );
#ifdef DIRECTX_PC
		m_IgnoreKeyShift = false;
#endif
	}
	else if( c->shiftReleased( ) 
#ifdef DIRECTX_PC
		&& !m_IgnoreKeyShift
#endif
		)
	{
		SetShift( false );
	}

	// --- get the caps lock status
	if( c->isCapsLocked( ) )
	{
		SetCapsLock( true );
#ifdef DIRECTX_PC
		m_IgnoreKeyShift = false;
#endif
	}
	else if( c->capsUnLocked( ) 
#ifdef DIRECTX_PC
		&& !m_IgnoreKeyShift 
#endif
		)
	{
		SetCapsLock( false );
	}


	// --- arrow keys
#if 0
	// --- testing wants these disabled for now
	if( c->isUpArrow() ) OnCursorUp();
	if( c->isDownArrow() ) OnCursorDown();
	if( c->isLeftArrow() ) OnCursorLeft();
	if( c->isRightArrow() ) OnCursorRight();
#endif

	if( ( key = c->getKey() ) )
	{
		switch( key )
		{
			// --- enter
			case KEY_LF:
				AcceptString();
				break;

			// --- space
			case KEY_SP:
				InsertChar(' ');
				break;

			// --- backspace
			case KEY_BS:
				BackSpace(1);
				break;

			default:
// "shift" before we check for restricted keys
#ifdef DIRECTX_PC
				if (m_Shifted)
				{
					switch( key )
					{
					case('1'):
						key = '!';
						break;
					case('2'):
						key = '@';
						break;
					case('3'):
						key = '#';
						break;
					case('4'):
						key = '$';
						break;
					case('5'):
						key = '%';
						break;
					case('6'):
						key = '^';
						break;
					case('7'):
						key = '&';
						break;
					case('8'):
						key = '*';
						break;
					case('9'):
						key = '(';
						break;
					case('0'):
						key = ')';
						break;
					case('.'):
						key = '>';
						break;
					case(','):
						key = '<';
					default:
						key -= 32;
					}
				}
#endif
				InsertChar( key );
				break;
		}
	}
}

int ScreenKeyboard::GetKey( void )
{
	return GetKey( m_CursorY, m_CursorX );
}

int ScreenKeyboard::GetKey( int row, int col )
{
	KeyDef* pKeyDef = GetKeyDef( row, col );

	if( pKeyDef )
		return pKeyDef->id;
	
	return 0;
}

ScreenKeyboard::KeyDef* ScreenKeyboard::GetKeyDef( int row, int col )
{
	KeyDef* pKeyDef = 0;

	if( m_pKeyLayout )
	{
		if(row >= 0 && row < MaxRows() && col >= 0 && col < MaxCols() )
			pKeyDef = &m_pKeyLayout[col+row*m_Cols];
	}

	return pKeyDef;
}

u8 ScreenKeyboard::GetChar( void )
{
	Sprite* pKey = GetSprite( GetKey() );

	if( !pKey )
		return 0;

	Text* pKeyText = (Text*) GetSprite( ID_KEYBOARD_KEY, pKey );

	if( !pKeyText )
		return 0;

	return pKeyText->m_Text.getch(0);
}

void ScreenKeyboard::RestrictKey( bool Restrict, int ch )
{
	int n = sizeof(m_RestrictedKeys) / sizeof(m_RestrictedKeys[0]);

	if( ch >= 0 && ch < n )
	{
		if( Restrict )
			m_RestrictedKeys[ch] = ~0;
		else
			m_RestrictedKeys[ch] = 0;
	}

	return;
}

void ScreenKeyboard::RestrictKeys( bool Restrict, int min_ch, int max_ch )
{
	int n = sizeof(m_RestrictedKeys) / sizeof(m_RestrictedKeys[0]);
	int i;
	char c;

	if( min_ch > max_ch )
		RestrictKeys( Restrict, max_ch, min_ch );
	else
	{
		min_ch = Math::Clamp( min_ch, 0, n-1 );
		max_ch = Math::Clamp( max_ch, 0, n-1 );

		if( Restrict )
			c =~0;
		else
			c = 0;

		for( i = min_ch; i <= max_ch; i++ )
			m_RestrictedKeys[i] = c;
	}

	return;
}

void ScreenKeyboard::RestrictChars( bool Restrict, const char* pString )
{
	if( pString )
	{
		int len = xstrlen(pString);
		int i;

		for( i = 0; i < len; i++ )
			RestrictKey( Restrict, (unsigned char)pString[i] );
	}

	return;
}

bool ScreenKeyboard::Restricted( u8 ch )
{
	const int n = sizeof(m_RestrictedKeys)/sizeof(m_RestrictedKeys[0]);

	if( ch >= n )
		return true;

	if( m_RestrictedKeys[ch] != 0 )
		return true;

	// don't allow spaces as the first character
	if( ch == ' ' && xstrlen(m_String) < 1 )
		return true;

	return false;
}

void ScreenKeyboard::UpdateKeys( void )
{
	KeyDef* pKeyDef;
	Sprite* pSprite;
	Text* pText;

	int rows = MaxRows();
	int cols = MaxCols();
	int x, y;
	int ch;

	for( y = 0; y < rows; y++ )
	{
		for( x = 0; x < cols; x++ )
		{
			pKeyDef = GetKeyDef( y, x );

			if( pKeyDef )
			{
				pSprite = GetSprite( pKeyDef->id );
				pText = GetTextSprite( ID_KEYBOARD_KEY, pSprite );

				ch = pKeyDef->current;

				if( pSprite )
				{
					if( !Restricted(ch) || ch == 0 )
					{
						pSprite->m_state.familycolor.a = 255;
					}
					else
					{
						pSprite->m_state.familycolor.a = 64;
					}
				}
			}
		}
	}

	ShowLed( GetShift(), ID_KEYBOARD_SHIFT );
	ShowLed( GetAccents(), ID_KEYBOARD_ACCENTS );
	ShowLed( GetCapsLock(), ID_KEYBOARD_CAPS );
	ShowLed( false, ID_KEYBOARD_SYMBOLS );

	bool Valid = ValidateString();

	if( !Valid && 0 == xstrlen(m_String) )
		Valid = true;

	ShowSprite( ID_KEYBOARD_INVALIDKEY, !Valid );

	Sprite* pEnterKey = GetSprite( ID_KEYBOARD_ENTER );

	if( pEnterKey )
	{
		if( Valid )
			pEnterKey->m_state.familycolor.a = 255;
		else
			pEnterKey->m_state.familycolor.a = 64;
	}

	Sprite* pSpaceBar = GetSprite( ID_KEYBOARD_SPACE );

	if( pSpaceBar )
	{
		bool AllowSpaces = !Restricted(' ');

		if( xstrlen(m_String) < 1 )
			AllowSpaces = false;

		if( AllowSpaces )
			pSpaceBar->m_state.familycolor.a = 255;
		else
			pSpaceBar->m_state.familycolor.a = 64;
	}

	Sprite* pDeleteKey = GetSprite( ID_KEYBOARD_BACKSPACE );

	if( pDeleteKey )
	{
		if( xstrlen(m_String) > 0)
			pDeleteKey->m_state.familycolor.a = 255;
		else
			pDeleteKey->m_state.familycolor.a = 64;
	}

	Sprite* pAccents = GetSprite( ID_KEYBOARD_ACCENTS );
	Sprite* pCapsLock = GetSprite( ID_KEYBOARD_CAPS );
	Sprite* pShift = GetSprite( ID_KEYBOARD_SHIFT );
	Text* pSymbols = GetTextSprite( ID_KEYBOARD_KEY, GetSprite( ID_KEYBOARD_SYMBOLS ) );

	if( pSymbols)
	{
		if( GetSymbols() )
			pSymbols->SetText( "Alphabet" );
		else
			pSymbols->SetText( "Symbols" );
	}

	int KeyAlpha = 255;

	if( GetSymbols() )
		KeyAlpha = 64;

	if( pAccents )
		pAccents->m_state.familycolor.a = KeyAlpha;

	if( pCapsLock )
		pCapsLock->m_state.familycolor.a = KeyAlpha;

	if( pShift )
		pShift->m_state.familycolor.a = KeyAlpha;

	return;
}

void ScreenKeyboard::HighlightKey( int KeyId )
{
	HighlightKey( GetSprite( KeyId ) );
}

void ScreenKeyboard::UnhighlightKey( int KeyId )
{
	UnhighlightKey( GetSprite( KeyId ) );
}

void ScreenKeyboard::HighlightKey( Sprite* pKey )
{
	if( pKey )
	{
		pKey->SetAlpha( 0.400f );

		Sprite* pChild = pKey->Child();

		while( pChild )
		{
			pChild->SetBaseColor( 255, 255, 255, 255 );
			pChild->SetScale( 1.3f, 1.3f );
			pChild = pChild->Next();
		}
	}

	return;
}

void ScreenKeyboard::UnhighlightKey( Sprite* pKey )
{
	if( pKey )
	{
		pKey->SetAlpha( 0.1875f );

		Sprite* pChild = pKey->Child();

		while( pChild )
		{
			pChild->SetBaseColor( 192, 192, 192, 255 );
			pChild->SetScale( 1.0f, 1.0f );
			pChild = pChild->Next();
		}
	}

	return;
}


void ScreenKeyboard::ShowLed(	bool Show, int Key )
{
	Sprite* pKey = GetSprite( Key );
	Sprite* pLed;

	if( pKey )
	{
		pLed = GetSprite( ID_KEYBOARD_LED, pKey );

		if( pLed )
		{
			if( Show )
				pLed->m_state.color.a = 255;
			else
				pLed->m_state.color.a = 0;
		}
	}

	return;
}

void ScreenKeyboard::SetAutoHide( bool AutoHide )
{
	m_AutoHide = AutoHide;
}

void ScreenKeyboard::SetKeyMap( KeyMap cm, bool Shifted )
{
	if( m_KeyMap != cm || m_Shifted != Shifted )
	{
		m_KeyMap = cm;
		m_Shifted = Shifted;

		Sprite* pKey;
		Text* pKeyText;
		KeyDef* pKeyDef;
		int rows = MaxRows();
		int cols = MaxCols();
		int x, y;
		int c;
		u8 ch;
		char keytext[2];

		c = int(cm) - 1;

		for( y = 0; y < rows; y++ )
		{
			for( x = 0; x < cols; x++ )
			{
				pKeyDef = GetKeyDef( y, x );

				if( pKeyDef )
				{
					pKey = GetSprite( pKeyDef->id );
					pKeyText = GetTextSprite( ID_KEYBOARD_KEY, pKey );
				
					ch = pKeyDef->ch[c];

					if( ch != 0 )
					{
						if( Shifted )
							ch = xupper(ch);
						else
							ch = xlower(ch);

						keytext[0] = ch;
						keytext[1] = 0;

						pKeyDef->current = ch;
						pKeyText->SetText( keytext );
					}
				}
			}
		}
	}

	return;
}

bool ScreenKeyboard::GetAccents( void )
{
	if( m_Accents && m_ShiftAccents )
		return false;

	if( m_Accents || m_ShiftAccents )
		return true;

	return false;
}


bool ScreenKeyboard::GetCapsLock( void )
{
	if( m_CapsLock && m_ShiftCapsLock )
		return false;

	if( m_CapsLock || m_ShiftCapsLock )
		return true;

	return false;
}

void ScreenKeyboard::ToggleAccents( void )
{
	if( !m_Accents )
		m_Accents = true;
	else
		m_Accents = false;

	PlaySound( "keyboard_accents" );
}

void ScreenKeyboard::ToggleCapsLock( void )
{
	if( !m_CapsLock )
		m_CapsLock = true;
	else
		m_CapsLock = false;

	PlaySound( "keyboard_shift" );
}

void ScreenKeyboard::ToggleShift( void )
{
	if( !m_Shift )
		m_Shift = true;
	else
		m_Shift = false;

	PlaySound( "keyboard_shift" );
}

void ScreenKeyboard::ToggleSymbols( void )
{
	if( !m_Symbols )
		m_Symbols = true;
	else
		m_Symbols = false;

	PlaySound( "keyboard_symbols" );
}

bool ScreenKeyboard::EnterString( void )
{
	if( !ValidateString() )
	{
		PlaySoundEvent( Screen::SOUNDEVENT_INVALID );
		return false;
	}

	PlaySound( "keyboard_enter" );
	AcceptString();

	return true;
}

bool ScreenKeyboard::ValidateString( void )
{
	int n = xstrlen(m_String);
	int i;

	if( n < 1 )
		return false;

	for( i = 0; i < n; i++ )
	{
		if( Restricted(m_String[i]) )
			return false;
	}

	return true;
}

void ScreenKeyboard::AcceptString( void )
{
	if( m_AutoHide )
		EndScreen( KEYBOARD_OK );

	g_messageDispatcher.SendMessageToAll( "KeyboardInput", (void*) GetString(), INVALID_OBJECT_HANDLE );
}

void ScreenKeyboard::SetPasswordMode( bool PasswordMode )
{
	m_PasswordMode = PasswordMode;
	m_ResetPasswordMode = false;
}

void ScreenKeyboard::ShowDimKeyboard( bool Dim )
{
	int Alpha = 255;

	if( Dim )
		Alpha = 64;

	Sprite* pScreenSprite = GetScreenSprite();

	if( pScreenSprite )
		pScreenSprite->m_state.familycolor.a = Alpha;

	return;
}

void ScreenKeyboard::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	m_pDisplayText = GetTextSprite( ID_KEYBOARD_STRING );

	InitKeys( 5, 11 );

	// row[0]
	SetKey( 0, 0, ID_KEYBOARD_CLEAR, 0, 0, 0 );
	SetKey( 0, 1, ID_KEYBOARD_1, '1', '1', '(' );	
	SetKey( 0, 2, ID_KEYBOARD_2, '2', '2', ')' );	
	SetKey( 0, 3, ID_KEYBOARD_3, '3', '3', '&' );	
	SetKey( 0, 4, ID_KEYBOARD_4, '4', '4', '_' );	
	SetKey( 0, 5, ID_KEYBOARD_5, '5', '5', '^' );	
	SetKey( 0, 6, ID_KEYBOARD_6, '6', '6', '%' );	
	SetKey( 0, 7, ID_KEYBOARD_7, '7', '7', '\\' );	
	SetKey( 0, 8, ID_KEYBOARD_8, '8', '8', '/' );	
	SetKey( 0, 9, ID_KEYBOARD_9, '9', '9', '@' );	
	SetKey( 0,10, ID_KEYBOARD_0, '0', '0', '#' );	

	// row[1]
	SetKey( 1, 0, ID_KEYBOARD_ACCENTS, 0, 0, 0 );
	SetKey( 1, 1, ID_KEYBOARD_A, 'a', 'à', '[' );	
	SetKey( 1, 2, ID_KEYBOARD_B, 'b', 'á', ']' );	
	SetKey( 1, 3, ID_KEYBOARD_C, 'c', 'â', '$' );	
	SetKey( 1, 4, ID_KEYBOARD_D, 'd', 'ã', '£' );	
	SetKey( 1, 5, ID_KEYBOARD_E, 'e', 'ä', '¤' );	
	SetKey( 1, 6, ID_KEYBOARD_F, 'f', 'å', '¥' );	
	SetKey( 1, 7, ID_KEYBOARD_G, 'g', 'è', ';' );	
	SetKey( 1, 8, ID_KEYBOARD_H, 'h', 'é', ':' );	
	SetKey( 1, 9, ID_KEYBOARD_I, 'i', 'ê', '\'' );	
	SetKey( 1,10, ID_KEYBOARD_J, 'j', 'ë', '"' );	

	// row[2]
	SetKey( 2, 0, ID_KEYBOARD_CAPS, 0, 0, 0 );
	SetKey( 2, 1, ID_KEYBOARD_K, 'k', 'ì', '<' );	
	SetKey( 2, 2, ID_KEYBOARD_L, 'l', 'í', '>' );	
	SetKey( 2, 3, ID_KEYBOARD_M, 'm', 'î', '?' );	
	SetKey( 2, 4, ID_KEYBOARD_N, 'n', 'ï', '!' );	
	SetKey( 2, 5, ID_KEYBOARD_O, 'o', 'ò', '¿' );	
	SetKey( 2, 6, ID_KEYBOARD_P, 'p', 'ó', '¡' );	
	SetKey( 2, 7, ID_KEYBOARD_Q, 'q', 'ô', '-' );	
	SetKey( 2, 8, ID_KEYBOARD_R, 'r', 'õ', '*' );	
	SetKey( 2, 9, ID_KEYBOARD_S, 's', 'ö', '+' );	
	SetKey( 2,10, ID_KEYBOARD_T, 't', 'ø', '=' );	
	
	// row[3]
	SetKey( 3, 0, ID_KEYBOARD_SHIFT, 0, 0, 0 );
	SetKey( 3, 1, ID_KEYBOARD_U, 'u', 'ù', '{' );				
	SetKey( 3, 2, ID_KEYBOARD_V, 'v', 'ú', '}' );				
	SetKey( 3, 3, ID_KEYBOARD_W, 'w', 'û', '«' );				
	SetKey( 3, 4, ID_KEYBOARD_X, 'x', 'ü', '»' );				
	SetKey( 3, 5, ID_KEYBOARD_Y, 'y', 'ý', '~' );				
	SetKey( 3, 6, ID_KEYBOARD_Z, 'z', 'ç', '`' );				
	SetKey( 3, 7, ID_KEYBOARD_AT,'@', 'ß', ',' );				
	SetKey( 3, 8, ID_KEYBOARD_PERIOD, '.', 'ñ', '.' );			
	SetKey( 3, 9, ID_KEYBOARD_UNDERSCORE, '_', 'æ', '_' );	
	SetKey( 3,10, ID_KEYBOARD_DASH, '-', '-', '-' );			

	// row[4]
	SetKey( 4, 0, ID_KEYBOARD_SYMBOLS, 0, 0, 0 );
	SetKey( 4, 1, ID_KEYBOARD_BACKSPACE, 0, 0, 0 );
	SetKey( 4, 2, ID_KEYBOARD_BACKSPACE, 0, 0, 0 );
	SetKey( 4, 3, ID_KEYBOARD_SPACE, 0, 0, 0 );
	SetKey( 4, 4, ID_KEYBOARD_SPACE, 0, 0, 0 );
	SetKey( 4, 5, ID_KEYBOARD_SPACE, 0, 0, 0 );
	SetKey( 4, 6, ID_KEYBOARD_SPACE, 0, 0, 0 );
	SetKey( 4, 7, ID_KEYBOARD_SPACE, 0, 0, 0 );
	SetKey( 4, 8, ID_KEYBOARD_SPACE, 0, 0, 0 );
	SetKey( 4, 9, ID_KEYBOARD_ENTER, 0, 0, 0 );
	SetKey( 4,10, ID_KEYBOARD_ENTER, 0, 0, 0 );

	int n = sizeof(m_RestrictedKeys)/sizeof(m_RestrictedKeys[0]);

	RestrictKeys( false, 0, n );
	SetMaxString( 0 );

	HighlightKey( ID_KEYBOARD_CLEAR );

	// Hide PS2-specific keys
#ifdef DIRECTX_PC
	GetSprite(ID_KEYBOARD_HELPTEXTICON1)->HideAll();
	GetSprite(ID_KEYBOARD_HELPTEXTICON2)->HideAll();
	GetSprite(ID_KEYBOARD_HELPTEXTICON3)->HideAll();
	GetSprite(ID_KEYBOARD_HELPTEXTICON4)->HideAll();
	GetSprite(ID_KEYBOARD_HELPTEXT3)->HideAll();

	// Make cancel a working button
	SetNextButton(ID_KEYBOARD_HELPTEXT4);
#endif
}

void ScreenKeyboard::OnCursorUp( void )
{
	MoveCursor( 0,-1 );
}

void ScreenKeyboard::OnCursorDown( void )
{
	MoveCursor( 0, 1 );
}

void ScreenKeyboard::OnCursorLeft( void )
{
	MoveCursor(-1, 0 );
}

void ScreenKeyboard::OnCursorRight( void )
{
	MoveCursor( 1, 0 );
}

void ScreenKeyboard::OnButtonOK( void )
{
#ifdef DIRECTX_PC
	if (GetButton(ID_KEYBOARD_HELPTEXT4)->m_Selected)
	{
		GetButton(ID_KEYBOARD_HELPTEXT4)->m_Selected = false;
		OnButtonCancel();
		return;
	}
	// stop the mysterious double character thing when hitting enter
	FluffControlMapper* fluffMapper = (FluffControlMapper*)g_controlFocus.Find("Fluff");
	if (fluffMapper->ButtonPressed(FluffControlMapper::START))
		return;
#endif

	int Key = GetKey();
	
	Sprite* pKey = GetSprite( Key );

	if( pKey )
	{
		Text* pKeyText = GetTextSprite( ID_KEYBOARD_KEY, pKey );

		if( pKeyText )
		{
			m_KeyEffect.Copy( pKeyText );
			m_KeyEffect.InitAnim();
			
			SpriteAnim& anim = *m_KeyEffect.m_anim;

			anim.scale.x.SetFrame( 0.000f, 3.0f );
			anim.scale.x.SetFrame( 0.200f, 3.2f );
			anim.scale.x.SetFrame( 0.400f, 1.0f );
			anim.scale.x.Deccelerate( true );
			anim.scale.x.Play( false );

			anim.scale.y.SetFrame( 0.000f, 3.0f );
			anim.scale.y.SetFrame( 0.200f, 3.2f );
			anim.scale.y.SetFrame( 0.400f, 1.0f );
			anim.scale.y.Deccelerate( true );
			anim.scale.y.Play( false );

			anim.color.a.SetFrame( 0.000f, 0.3f );
			anim.color.a.SetFrame( 0.200f, 0.3f );
			anim.color.a.SetFrame( 0.400f, 0.0f );
			anim.color.a.Deccelerate( true );
			anim.color.a.Play( false );

			float x, y;
			pKeyText->GetScreenPosition( x, y );
			m_KeyEffect.SetScreenPosition( x, y );
			m_KeyEffect.SetPriority( 5000 );
		}
	}

	switch( Key )
	{
	case ID_KEYBOARD_CLEAR:
		PlaySound( "keyboard_clear" );
		Clear();
		break;

	case ID_KEYBOARD_ACCENTS:
		if( !GetSymbols() )
			ToggleAccents();
		break;

	case ID_KEYBOARD_CAPS:
#ifdef DIRECTX_PC
		m_IgnoreKeyShift = true;
#endif
		if( !GetSymbols() )
			ToggleCapsLock();
		break;

	case ID_KEYBOARD_SHIFT:
#ifdef DIRECTX_PC
		m_IgnoreKeyShift = true;
#endif
		if( !GetSymbols() )
			ToggleShift();
		break;

	case ID_KEYBOARD_SYMBOLS:
		ToggleSymbols();
		break;
		
	case ID_KEYBOARD_ENTER:
		EnterString();
		break;

	case ID_KEYBOARD_SPACE:
		InsertChar(' ');
		break;

	case ID_KEYBOARD_BACKSPACE:
		BackSpace(1);
		break;

	default:
		InsertChar( GetChar() );
		break;
	}

	return;
}

void ScreenKeyboard::OnButtonCancel( void )
{
	EndScreen(KEYBOARD_CANCEL);

	g_messageDispatcher.SendMessageToAll( "KeyboardCancel", NULL, INVALID_OBJECT_HANDLE );
}

void ScreenKeyboard::OnButtonPressed( int Button )
{
	switch( Button )
	{
	case FluffControlMapper::CIRCLE:
		InsertChar(' ');
		break;
	case FluffControlMapper::SQUARE:
		BackSpace(1);
		break;
#ifndef _XBOX
	case FluffControlMapper::SELECT:
#else
	case FluffControlMapper::L2:
#endif
		ToggleSymbols();
		break;

	case FluffControlMapper::START:
		EnterString();
		break;

	default:
		ScreenBaseClass::OnButtonPressed( Button );
	}

	return;
}

void ScreenKeyboard::HandleInput( UIHandler* ph )
{
#ifndef DIRECTX_PC
#ifdef _XBOX
	if(ph->DeviceButtonDown( FluffControlMapper::L1 ) )
#else
	if(ph->DeviceButtonDown( FluffControlMapper::L1 ) ||
		ph->DeviceButtonDown( FluffControlMapper::L2 ) )
#endif
	{
		m_ShiftAccents = true;
	}
	else
		m_ShiftAccents = false;

#ifdef _XBOX
	if(ph->DeviceButtonDown( FluffControlMapper::R1 ) )
#else
	if(ph->DeviceButtonDown( FluffControlMapper::R1 ) ||
		ph->DeviceButtonDown( FluffControlMapper::R2 ) )
#endif
	{
		m_ShiftCapsLock = true;
	}
	else
		m_ShiftCapsLock = false;
#endif

	ScreenBaseClass::HandleInput( ph );
}

void ScreenKeyboard::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();
	SelectKey( m_DefaultKey );

	if( m_ResetPasswordMode )
		SetPasswordMode( false );

	return;
}

void ScreenKeyboard::OnScreenExit( void )
{
	Sprite* pSprite = GetScreenSprite();

	if( pSprite )
	{
		pSprite->InitAnim();

		SpriteAnim& anim = *pSprite->m_anim;

		anim.familycolor.a.SetFrame( 0.000f, 1.0f );
		anim.familycolor.a.SetFrame( 0.100f, 0.0f );
		anim.familycolor.a.Deccelerate( true );
		anim.familycolor.a.Play( false );
	}

	m_KeyEffect.Remove();
	m_KeyEffect.ReleaseAll();

	int n = sizeof(m_RestrictedKeys)/sizeof(m_RestrictedKeys[0]);

	RestrictKeys( false, 0, n );
	SetMaxString( 0 );

	m_ResetPasswordMode = true;

#ifdef DIRECTX_PC
	ScreenBaseClass::OnScreenExit();
#endif
}

void ScreenKeyboard::OnScreenUpdate( void )
{
#ifdef DIRECTX_PC
	// mouse cursor stuff
	g_inputPtr->GetController(0)->EatButtonPresses();
	if (g_MenuCursor.IsActive()) {
		float x1, y1, x2, y2;	
		// check for back navigation
		if (g_MenuCursor.CursorButtonPressed(1))
		{
			g_MenuCursor.ForceButtonUp(1);
			OnButtonCancel();
		}
		float x = g_MenuCursor.GetX();
		float y = g_MenuCursor.GetY();
		Sprite* pSprite;
		Sprite* pOldKey = GetSprite( GetKey() );

		for (int rows =0; rows < MaxRows(); ++rows)
		{
			for (int cols =0; cols < MaxCols(); ++cols)
			{
				pSprite = GetSprite( GetKey(rows, cols) );
				if( pSprite )
				{
					pSprite->GetFamilyRect(x1, y1, x2, y2);
					if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
					{
						// set our current button
						m_CursorX = cols;
						m_CursorY = rows;
						// do coloring stuff here
						pSprite->SetAlpha( 0.400f );
						Sprite* pChild = pSprite->Child();
						while( pChild )
						{
							pChild->SetBaseColor( 255, 255, 255, 255 );
							pChild->SetScale( 1.3f, 1.3f );
							pChild = pChild->Next();
						}
						if( pOldKey && pOldKey != pSprite )
						{
							pOldKey->SetAlpha( 0.1875f );
							pChild = pOldKey->Child();
							while( pChild )
							{
								pChild->SetBaseColor( 160, 160, 160, 255 );
								pChild->SetScale( 1.0f, 1.0f );
								pChild = pChild->Next();
							}
						}
						if ( g_MenuCursor.CursorButtonPressed(0) )
						{
								g_MenuCursor.ForceButtonUp(0);
								OnButtonOK(); 
						}
						break;
					}
				}
			}
		}
		ScreenBaseClass::OnScreenUpdate();
	}
	// since we're gonna be doing keyboard input, 
	// disable the keyboard controller
#endif

	UpdateDisplayText();

	KeyMap km = Normal;
	bool Shifted = false;

	// --- DAS added support for real keyboards on PS2, can work on any platform
	GetKeyboardKey( );

	if( GetAccents() )
		km = Accents;

	if( GetSymbols() )
		km = Symbols;

	if( GetShift() || GetCapsLock() )
		Shifted = true;

	SetKeyMap( km, Shifted );

	m_KeyEffect.Update();

	UpdateKeys();
}

void ScreenKeyboard::OnPopupShow( void )
{
	ShowDimKeyboard( true );
}

void ScreenKeyboard::OnPopupHide( void )
{
	ShowDimKeyboard( false );
}

void ScreenKeyboard::StepUp( void )
{
	MoveCursor( 0,-1 );
}

void ScreenKeyboard::StepDown( void )
{
	MoveCursor( 0, 1 );
}

void ScreenKeyboard::StepLeft( void )
{
	MoveCursor(-1, 0 );
}

void ScreenKeyboard::StepRight( void )
{
	MoveCursor( 1, 0 );
}