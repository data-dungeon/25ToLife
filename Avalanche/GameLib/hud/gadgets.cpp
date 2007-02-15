/******************* includes ****************************/

#include "hud/HudPCH.h"
#include "hud/gadgets.h"

//=============================================================================
// PictureText::PictureText()
//=============================================================================

PictureText::PictureText( void )
{
	AddChild( &m_Picture );
	AddChild( &m_Caption );

	m_OnTexture = 0;
	m_OffTexture = 0;
	scalex = 0.0f;
	scaley = 0.0f;

	textPos = TEXTPOS_RIGHT;
}

//=============================================================================
// PictureText::Create()
//=============================================================================

void PictureText::Create( const char *pCaption, t_Handle TextureSet, int OnTexture, int OffTexture )
{
	m_Picture.Create( TextureSet, OnTexture );
	m_Caption.SetText( pCaption );

	m_OnTexture = OnTexture;
	m_OffTexture = OffTexture;
}

//=============================================================================
// PictureText::Draw()
//=============================================================================

void PictureText::Draw( void )
{
	Sprite::Draw();
	
	int Texture = m_OnTexture;

	if( !m_Checked )
		Texture = m_OffTexture;

	m_Picture.SetTexture( Texture );
}

//=============================================================================
// PictureText::Check()
//=============================================================================

void PictureText::Check( void )
{
	m_Checked = true;
}

//=============================================================================
// PictureText::Uncheck()
//=============================================================================

void PictureText::Uncheck( void )
{
	m_Checked = false;
}

//=============================================================================
// PictureText::Toggle()
//=============================================================================

void PictureText::Toggle( void )
{
	m_Checked = !m_Checked;
}

//=============================================================================
// PictureText::Checked()
//=============================================================================

bool PictureText::Checked( void )
{
	return m_Checked;
}

//=============================================================================
// PictureText::GetWidth()
//=============================================================================

float PictureText::GetWidth( void )
{
	float width = 0.0f;

	if( m_Picture.m_state.textureset.handle != INVALID_HANDLE )
		width = width + m_Picture.m_state.size.x;

	if( m_Caption.m_Text.length() > 0 )
		width = width + m_Caption.ComputeTextLength();

	return width;
}

//=============================================================================
// PictureText::GetHeight()
//=============================================================================

float PictureText::GetHeight( void )
{
	float height = 0.0f;
	float textheight = 0.0f;

	if( m_Picture.m_state.textureset.handle != INVALID_HANDLE )
		height = m_Picture.m_state.size.y;

	if( m_Caption.m_Text.length() > 0 )
		textheight = m_Caption.ComputeTextHeight();

	if( FLOATTOINT(height) < FLOATTOINT(textheight) )
		height = textheight;

	return height;
}

//=============================================================================
// PictureText::ComputeSprite()
//=============================================================================

void PictureText::ComputeSprite( void )
{
	TextOutput to;

	m_Caption.ComputeTextOutput( &to );

	float tw = to.Width;
	float th = to.Height;

	float sw = m_Picture.m_state.size.x * m_Picture.m_state.scale.x;
	float sh = m_Picture.m_state.size.y * m_Picture.m_state.scale.y;

	float w = sw + tw;
	float h = sh;

	if( FLOATTOINT(h) < FLOATTOINT(th) )
		h = th;
	
	if( FLOATTOINT(m_state.size.x) == 0 )
		m_state.size.x = w;

	if( FLOATTOINT(m_state.size.y) == 0 )
		m_state.size.y = h;

	float cx = m_state.size.x * 0.5f - w * 0.5f;
	float cy = m_state.size.y * 0.5f;
	
	float textX = 0.0f, textY = 0.0f;
	switch( textPos )
	{
		case TEXTPOS_RIGHT:
			textX = cx + sw;
			textY = cy - (th * 0.5f);
			break;
		case TEXTPOS_LEFT:
			textX = cx - tw;
			textY = cy - (th * 0.5f);
			break;
		case TEXTPOS_BOTTOMRIGHT:
			textX = cx + sw;
			textY = cy + (sh/2.0f) - th;
			break;
		case TEXTPOS_BOTTOMLEFT:
			textX = cx - tw;
			textY = cy + (sh/2.0f) - th;
			break;
	}
	
	m_Picture.SetPosition( cx, cy - sh * 0.5f );
	m_Caption.SetPosition( textX, textY );

	Sprite::ComputeSprite();
}

//=============================================================================
// ScrollBar::ScrollBar()
//=============================================================================

ScrollBar::ScrollBar( void )
{
	AddChild( &m_Shaft );
	AddChild( &m_ArrowUp );
	AddChild( &m_ArrowDown );

	m_Shaft.AddChild( &m_ScrollBar );

	m_RangeHi = 0;
	m_RangeLow = 0;
	m_Position = 0;
	m_LinesPerPage = 1;
}

//=============================================================================
// ScrollBar::TextureSetup()
//=============================================================================

void ScrollBar::Create( float x, float y, float Width, float Height, t_Handle TextureSet, int BarTexture, int ArrowUpTexture, int ArrowDownTexture, int ShaftTexture )
{
	m_Shaft.Create( TextureSet, ShaftTexture );
	m_ArrowUp.Create( TextureSet, ArrowUpTexture );
	m_ArrowDown.Create( TextureSet, ArrowDownTexture );
	m_ScrollBar.Create( TextureSet, BarTexture );

	SetPosition( x, y );
	SetSize( Width, Height );

	SyncComponents();
}

//=============================================================================
// ScrollBar::SetRange()
//=============================================================================

void ScrollBar::SetRange( int low, int hi )
{
	m_RangeLow = low;
	m_RangeHi = hi;
}

//=============================================================================
// ScrollBar::SetPageSize()
//=============================================================================

void ScrollBar::SetPageSize( int LinesPerPage )
{
	m_LinesPerPage = LinesPerPage;
}

//=============================================================================
// ScrollBar::SetThumbPos()
//=============================================================================

void ScrollBar::SetThumbPos( int pos )
{
	m_Position = pos;

	if( m_Position < m_RangeLow )
		m_Position = m_RangeLow;

	if( m_Position > m_RangeHi )
		m_Position = m_RangeHi;

	return;
}

//=============================================================================
// ScrollBar::GetThumbPos
//=============================================================================

int ScrollBar::GetThumbPos( void )
{
	return m_Position;
}

//=============================================================================
// ScrollBar::MoveThumbPos()
//=============================================================================

void ScrollBar::MoveThumbPos( int Change )
{
	SetThumbPos( GetThumbPos() + Change );
}

//=============================================================================
// ScrollBar::Draw()
//=============================================================================

void ScrollBar::Draw( void )
{
	Sprite::Draw();

	SyncComponents();
}

//=============================================================================
// ScrollBar::SyncComponents()
//=============================================================================

void ScrollBar::SyncComponents( void )
{
	float ArrowSize = m_state.size.x;

	float ShaftSize = m_state.size.y - ArrowSize * 2.0f;
	float Range = (float) (m_RangeHi - m_RangeLow + m_LinesPerPage);
	float UnitSize = (ShaftSize - m_state.size.x * 0.25f) / Range;
	float BarHeight = UnitSize * m_LinesPerPage;

	m_Shaft.SetSize( m_state.size.x, ShaftSize );
	m_Shaft.SetPosition( 0.0f, ArrowSize );
	m_ArrowUp.SetSize( m_state.size.x, ArrowSize );
	m_ArrowUp.SetPosition( 0.0f, 0.0f );
	m_ArrowDown.SetSize( m_state.size.x, ArrowSize );
	m_ArrowDown.SetPosition( 0.0f, m_state.size.y - ArrowSize );
	m_ScrollBar.SetSize( m_state.size.x * 0.75f, BarHeight );
	m_ScrollBar.SetPosition( m_state.size.x * 0.125f, m_state.size.x * 0.125f + UnitSize * (float) m_Position );

	return;
}

//=============================================================================
// GamePadButtons::GamePadButtons()
//=============================================================================

GamePadButtons::GamePadButtons( void )
{
	m_FixedPositionLock = false;
	for( int i = 0; i < ARRAYOF(m_Buttons); i++ )
		AddChild( &m_Buttons[i] );

	return;
}

//=============================================================================
// GamePadButtons::SetFixedPositionLock()
//=============================================================================

void GamePadButtons::SetFixedPositionLock( bool Enabled )
{
	m_FixedPositionLock = Enabled;
	return;
}

//=============================================================================
// GamePadButtons::CreateButton()
//=============================================================================

void GamePadButtons::CreateButton( int Button, const char *pText, t_Handle TextureSet, int Texture )
{
	if( Button >= 0 && Button < ARRAYOF(m_Buttons) )
		m_Buttons[Button].Create( pText, TextureSet, Texture, Texture );

	return;
}

//=============================================================================
// GamePadButtons::RemoveButton()
//=============================================================================

void GamePadButtons::RemoveButton( int Button )
{
	if( Button >= 0 && Button < ARRAYOF(m_Buttons) )
		m_Buttons[Button].Release();

	return;
}

//=============================================================================
// GamePadButtons::RemoveAllButtons()
//=============================================================================

void GamePadButtons::RemoveAllButtons( void )
{
	for( int i = 0; i < ARRAYOF(m_Buttons); i++ )
		m_Buttons[i].Release();

	return;
}

//=============================================================================
// GamePadButtons::SetButtonText()
//=============================================================================

void GamePadButtons::SetButtonText( int Button, const char *pText )
{
	m_Buttons[Button].m_Caption.SetText( pText );
}

//=============================================================================
// GamePadButtons::SetButtonPicture()
//=============================================================================

void GamePadButtons::SetButtonPicture( int Button, t_Handle TextureSet, int Texture )
{
	if( Button >= 0 && Button < ARRAYOF(m_Buttons) )
		m_Buttons[Button].m_Picture.SetTextureSet( TextureSet, Texture );

	return;
}

//=============================================================================
// GamePadButtons::SetPictureSize()
//=============================================================================

void GamePadButtons::SetPictureSize( int Button, float Width, float Height )
{
	if( Button >= 0 && Button < ARRAYOF(m_Buttons) )
		m_Buttons[Button].m_Picture.SetSize( Width, Height );

	return;
}

//=============================================================================
// GamePadButtons::SetTextAppearance()
//=============================================================================

void GamePadButtons::SetTextAppearance( float TextHeight, float TextSpacing, float TextAlignment )
{
	for( int i = 0; i < ARRAYOF(m_Buttons); i++ )
	{
		m_Buttons[i].m_Caption.SetTextHeight( TextHeight );
		m_Buttons[i].m_Caption.SetTextSpacing( TextSpacing );
		m_Buttons[i].m_Caption.SetTextAlignment( TextAlignment );
	}

	return;
}

//=============================================================================
// GamePadButtons::SetTextAppearanceButton()
//=============================================================================

void GamePadButtons::SetTextAppearanceButton( int Button, float TextHeight, float TextSpacing, float TextAlignment )
{
	m_Buttons[Button].m_Caption.SetTextHeight( TextHeight );
	m_Buttons[Button].m_Caption.SetTextSpacing( TextSpacing );
	m_Buttons[Button].m_Caption.SetTextAlignment( TextAlignment );

	return;
}

//=============================================================================
// GamePadButtons::SetTextColor()
//=============================================================================

void GamePadButtons::SetTextColor( float r, float g, float b, float a )
{
	for( int i = 0; i < ARRAYOF(m_Buttons); i++ )
		m_Buttons[i].m_Caption.SetColor( r, g, b, a );

	return;
}

//=============================================================================
// GamePadButtons::SetTextColorButton()
//=============================================================================

void GamePadButtons::SetTextColorButton( int Button, float r, float g, float b, float a )
{
	m_Buttons[ Button ].m_Caption.SetColor( r, g, b, a );
}


//=============================================================================
// GamePadButtons::SetButtonTextOffset()
//=============================================================================

void GamePadButtons::SetButtonTextOffset( int Button, float x, float y )
{
	m_Buttons[ Button ].m_Caption.SetOffset( x, y );
	return;
}


//=============================================================================
// GamePadButtons::SetButtonTextAlpha()
//=============================================================================

void GamePadButtons::SetButtonTextAlpha( int Button, float a )
{
	m_Buttons[ Button ].m_Caption.SetAlpha( a );
	return;
}

//=============================================================================
// GamePadButtons::SetButtonAlpha()
//=============================================================================

void GamePadButtons::SetButtonAlpha( int Button, float a )
{
	m_Buttons[ Button ].m_Picture.SetAlpha( a );
	return;
}

//=============================================================================
// GamePadButtons::SetButtonColor()
//=============================================================================

void GamePadButtons::SetButtonColor( int Button, float r, float g, float b, float a )
{
	m_Buttons[ Button ].m_Picture.SetColor( r, g, b, a );
	return;
}

//=============================================================================
// GamePadButtons::SetButtonOffset()
//=============================================================================

void GamePadButtons::SetButtonOffset( int Button, float x, float y )
{
	m_Buttons[ Button ].SetOffset( x, y );
	return;
}


//=============================================================================
// GamePadButtons::SetButtonScale()
//=============================================================================

void GamePadButtons::SetButtonScale( int Button, float scalex, float scaley )
{
	m_Buttons[ Button ].scalex = scalex;
	m_Buttons[ Button ].scaley = scaley;
	return;
}


//=============================================================================
// GamePadButtons::SetTextShadow()
//=============================================================================

void GamePadButtons::SetTextShadow( bool Enabled, u8 Intensity /*= 255*/, s8 x /*= 2*/, s8 y /*= 2*/ )
{
	for( int i = 0; i < ARRAYOF(m_Buttons); i++ )
		m_Buttons[i].m_Caption.ShadowSettings( Enabled, Intensity, x, y );

	return;
}

//=============================================================================
// GamePadButtons::ComputeSprite()
//=============================================================================

void GamePadButtons::ComputeSprite( void )
{
	int i;

	struct {
		float width;
		float height;
	} buttons[ARRAYOF(m_Buttons)];

	float total_w = 0.0f;
	float total_h = 0.0f;
	float sx = 1.0f;
	float sy = 1.0f;
	int count = 0;

	for( i = 0; i < ARRAYOF(m_Buttons); i++ )
	{
		buttons[i].width = m_Buttons[i].GetWidth() + 6.0f;
		buttons[i].height = m_Buttons[i].GetHeight();

		if( FLOATTOINT(buttons[i].width) > 0 && FLOATTOINT(buttons[i].height) > 0 )
		{
			count = count + 1;
			
			if( total_h < buttons[i].height )
				total_h = buttons[i].height;

			total_w = total_w + buttons[i].width;
		}
	}

	if( FLOATTOINT(m_state.size.x) == 0 )
		m_state.size.x = total_w;

	if( FLOATTOINT(m_state.size.y) == 0 )
		m_state.size.y = total_h;

	if( FLOATTOINT(m_state.size.x) < FLOATTOINT(total_w) )
		sx = m_state.size.x / total_w;

	if( FLOATTOINT(m_state.size.y) < FLOATTOINT(total_h) )
		sy = m_state.size.y / total_h;


	float extra = m_state.size.x - total_w;
	float space = 0.0f;

	if( count > 2 && FLOATTOINT(extra) > 0 )
		space = extra / (count-1);

	float x = 0.0f;
	float y = m_state.size.y * 0.5f;

	if( count == 2 )
		x = x + extra * 0.33f, space = extra * 0.33f;
	else if( count == 1 )
		x = x + extra * 0.5f;

	for( i = 0; i < ARRAYOF(m_Buttons); i++ )
	{
		float scalex, scaley;

		scalex = m_Buttons[ i ].scalex;
		scaley = m_Buttons[ i ].scaley;
		if ( scalex == 0.0f )
			scalex = 1.0f;
		if ( scaley == 0.0f )
			scaley = 1.0f;
		if ( !m_FixedPositionLock ) {
			if( FLOATTOINT(buttons[i].width) > 0 && FLOATTOINT(buttons[i].height) > 0 )
			{
				x = x + buttons[i].width * 0.5f;

				m_Buttons[i].SetSize( buttons[i].width, total_h );
				m_Buttons[i].SetPosition( x * sx, y * sy );
				m_Buttons[i].SetScale( sx*scalex, sy*scaley );
				m_Buttons[i].SetOrigin( 0.5f, 0.5f );

				x = x + buttons[i].width * 0.5f + space;
			}
		} else {
				m_Buttons[i].SetSize( buttons[i].width, total_h );
				m_Buttons[i].SetPosition( m_Buttons[ i ].GetWidth() / -2.0f * m_Buttons[ i ].m_Caption.GetTextAlignment() , 0.0f );
				m_Buttons[i].SetScale( 1.0f*scalex, 1.0f*scaley );
				m_Buttons[i].SetOrigin( 0.0f, 0.0f );
		}
	}

	Sprite::ComputeSprite();
}
