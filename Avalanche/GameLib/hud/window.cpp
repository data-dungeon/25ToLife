/******************* includes ****************************/

#include "hud/HudPCH.h"
#include "hud/window.h"

Window::Window( void )
{
	AddChild( &m_Interior );
	AddChild( &m_BorderTL );
	AddChild( &m_BorderTR );
	AddChild( &m_BorderBR );
	AddChild( &m_BorderBL );
	for ( int i = 0; i < WINDOW_NUM_BORDER_PIECES; i++ ) {
		AddChild( &m_BorderT[ i ] );
		AddChild( &m_BorderR[ i ] );
		AddChild( &m_BorderB[ i ] );
		AddChild( &m_BorderL[ i ] );
	}
	AddChild( &m_Caption );
	AddChild( &m_CaptionBack );
	SetCaption( "" );
}

Window::~Window()
{
}

void Window::ShowInterior( void )
{
	m_Interior.Show();
}

void Window::HideInterior( void )
{
	m_Interior.Hide();
}

void Window::SetCaption( const char *pCaption, float HorzAlign, float VertAlign )
{
	m_Caption.SetText( pCaption );
	m_HorzAlign = HorzAlign;
	m_VertAlign = VertAlign;
}

void Window::ShowCaptionBack( void )
{
	m_CaptionBack.Show();
}

void Window::HideCaptionBack( void )
{
	m_CaptionBack.Hide();
}

void Window::Reset( void )
{
	Sprite::Reset();

	if( m_state.textureset.texture == TS_NO_TEXTURE )
	{
		m_Interior.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
		m_CaptionBack.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
		m_BorderTL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
		m_BorderTR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
		m_BorderBR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
		m_BorderBL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );

		m_BorderTL.SetSize( 4.0f, 4.0f );
		m_BorderTR.SetSize( 4.0f, 0.0f );
		m_BorderBR.SetSize( 4.0f, 4.0f );
		m_BorderBL.SetSize( 4.0f, 4.0f );

		for ( int i = 0; i < WINDOW_NUM_BORDER_PIECES; i++ ) {
			m_BorderT[ i ].SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
			m_BorderR[ i ].SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
			m_BorderB[ i ].SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
			m_BorderL[ i ].SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );

			m_BorderT[ i ].SetSize( 4.0f, 4.0f );
			m_BorderR[ i ].SetSize( 4.0f, 0.0f );
			m_BorderB[ i ].SetSize( 4.0f, 4.0f );
			m_BorderL[ i ].SetSize( 4.0f, 4.0f );
		}
	}
	else
	{
		//  TGS - all these rotations and scaling are done because we only have one corner piece
		//  The corner piece in this case was the upper right hand corner.  The side pieces are horizontal
		//  The addition at the end of SetTextureSet is the order in which the piece we need was ripped
		m_Interior.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 0 );
		m_CaptionBack.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 0 );
		m_BorderTR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 1 );
		m_BorderBR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 1 );
		m_BorderBR.SetRotation( 90.0f );
		m_BorderTL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 1 );
		m_BorderTL.SetScale( -1.0f, 1.0f );
		m_BorderBL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 1 );
		m_BorderBL.SetScale( -1.0f, 1.0f );
		m_BorderBL.SetRotation( -90.0f );

		for ( int i = 0; i < WINDOW_NUM_BORDER_PIECES; i++ ) {
			m_BorderT[ i ].SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 2 );
			m_BorderR[ i ].SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 2 );
			m_BorderR[ i ].SetRotation( 90.0f );
			m_BorderR[ i ].SetScale( 1.0f, -1.0f );
			m_BorderL[ i ].SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 2 );
			m_BorderL[ i ].SetRotation( 90.0f );
			m_BorderL[ i ].SetScale( 1.0f, -1.0f );
			m_BorderB[ i ].SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 2 );
		}
/*  TGS - old stuff ( works if each corner and side have their own art - so I'm keeping it in )
		m_BorderTL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 1 );
		m_BorderT.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 2 );
		m_BorderTR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 3 );
		m_BorderR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 4 );
		m_BorderBR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 5 );
		m_BorderB.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 6 );
		m_BorderBL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 7 );
		m_BorderL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 8 );
*/
		m_BorderTL.UseTextureSize();
		m_BorderTR.UseTextureSize();
		m_BorderBR.UseTextureSize();
		m_BorderBL.UseTextureSize();

		for ( int j = 0; j < WINDOW_NUM_BORDER_PIECES; j++ ) {
			m_BorderT[ j ].UseTextureSize();
			m_BorderR[ j ].UseTextureSize();
			m_BorderB[ j ].UseTextureSize();
			m_BorderL[ j ].UseTextureSize();
		}
	}

	SetCaption( "" );

	m_Interior.SetPriority( -50 );
	m_BorderTL.SetPriority( -50 );
	m_BorderTR.SetPriority( -50 );
	m_BorderBR.SetPriority( -50 );
	m_BorderBL.SetPriority( -50 );

	for ( int k = 0; k < WINDOW_NUM_BORDER_PIECES; k++ ) {
		m_BorderT[ k ].SetPriority( -52 );
		m_BorderR[ k ].SetPriority( -52 );
		m_BorderB[ k ].SetPriority( -52 );
		m_BorderL[ k ].SetPriority( -52 );
	}

	m_CaptionBack.SetPriority( -51 );

	float sizex = 0.0f, sizey = 0.0f;

	sizex += m_BorderTL.GetWidth();
	sizex += m_BorderT[ 0 ].GetWidth();
	sizex += m_BorderTR.GetWidth();

	sizey += m_BorderTL.GetHeight();
	sizey += m_BorderL[ 0 ].GetHeight();
	sizey += m_BorderBL.GetHeight();

	SetSize( sizex, sizey );
}

void Window::ComputeSprite( void )
{
	float xpos = 20.0f;
	float ypos = 30.0f;
	int l;

	if( !m_state.sprite.visible || FLOATTOINT(m_state.familycolor.a) == 0 )
		return;
	
	if( m_state.textureset.handle != INVALID_HANDLE )
	{
		if( m_state.textureset.texture == TS_NO_TEXTURE )
		{
			m_Interior.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
			m_CaptionBack.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
			m_BorderTL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
			m_BorderTR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
			m_BorderBR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
			m_BorderBL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );

			m_BorderTL.SetSize( 1.0f, 1.0f );
			m_BorderTR.SetSize( 1.0f, 1.0f );
			m_BorderBR.SetSize( 1.0f, 1.0f );
			m_BorderBL.SetSize( 1.0f, 1.0f );

			for ( int i = 0; i < WINDOW_NUM_BORDER_PIECES; i++ )
			{
				m_BorderT[ i ].SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
				m_BorderR[ i ].SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
				m_BorderB[ i ].SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
				m_BorderL[ i ].SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );

				m_BorderT[ i ].SetSize( 1.0f, 1.0f );
				m_BorderR[ i ].SetSize( 1.0f, 1.0f );
				m_BorderB[ i ].SetSize( 1.0f, 1.0f );
				m_BorderL[ i ].SetSize( 1.0f, 1.0f );
			}

			xpos = 0.0f;
			ypos = 0.0f;
		}
		else
		{
			//  TGS - all these rotations and scaling are done because we only have one corner piece
			//  The corner piece in this case was the upper right hand corner
			//  The side pieces are horizontal
			m_Interior.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 0 );
			m_CaptionBack.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 0 );
			m_BorderTR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 1 );
			m_BorderBR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 1 );
			m_BorderBR.SetRotation( 90.0f );
			m_BorderTL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 1 );
			m_BorderTL.SetScale( -1.0f, 1.0f );
			m_BorderBL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 1 );
			m_BorderBL.SetScale( -1.0f, 1.0f );
			m_BorderBL.SetRotation( -90.0f );

			for ( int j = 0; j < WINDOW_NUM_BORDER_PIECES; j++ ) {
				m_BorderT[ j ].SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 2 );
				m_BorderR[ j ].SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 2 );
				m_BorderR[ j ].SetRotation( 90.0f );
				m_BorderR[ j ].SetScale( 1.0f, -1.0f );
				m_BorderL[ j ].SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 2 );
				m_BorderL[ j ].SetRotation( 90.0f );
				m_BorderL[ j ].SetScale( 1.0f, -1.0f );
				m_BorderB[ j ].SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 2 );
			}
/*  TGS - old stuff ( works if each corner and side have their own art - so I'm keeping it in )
			m_Interior.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 0 );
			m_BorderTL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 1 );
			m_BorderT.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 2 );
			m_BorderTR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 3 );
			m_BorderR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 4 );
			m_BorderBR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 5 );
			m_BorderB.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 6 );
			m_BorderBL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 7 );
			m_BorderL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 8 );
*/
			m_BorderTL.UseTextureSize();
			m_BorderTR.UseTextureSize();
			m_BorderBR.UseTextureSize();
			m_BorderBL.UseTextureSize();
			for ( int k = 0; k < WINDOW_NUM_BORDER_PIECES; k++ ) {
				m_BorderT[ k ].UseTextureSize();			//  TGS - added to keep the texture the right size
				m_BorderB[ k ].UseTextureSize();			//  TGS - added to keep the texture the right size
				m_BorderR[ k ].UseTextureSize();			//  TGS - added to keep the texture the right size
				m_BorderL[ k ].UseTextureSize();			//  TGS - added to keep the texture the right size
			}
		}

		float x1 = -xpos;
		float y1 = -ypos;
		float x2 = x1 + m_BorderTL.m_state.size.x+xpos;
		float y2 = y1 + m_BorderTL.m_state.size.y+ypos;
		float x3 = x1 + m_state.size.x - m_BorderBR.m_state.size.x+2.0f*xpos;
		float y3 = y1 + m_state.size.y - m_BorderBR.m_state.size.y+2.0f*ypos;

/*  TGS - old stuff ( works if each corner and side have their own art - so I'm keeping it in )
		m_BorderTL.SetPosition( x1, y1 );
		m_BorderT.SetPosition( x2, y1 );
		m_BorderTR.SetPosition( x3, y1 );
		m_BorderR.SetPosition( x3, y2 );
		m_BorderBR.SetPosition( x3, y3 );
		m_BorderB.SetPosition( x2, y3 );
		m_BorderBL.SetPosition( x1, y3 );
		m_BorderL.SetPosition( x1, y2 );
*/
		float InteriorWidth = x3 - x2;
		float InteriorHeight = y3 - y2;

		int howmanyx = (int)( ( InteriorWidth + 2.0f*xpos ) / 128.0f );
		int howmanyy = (int)( ( InteriorHeight + 2.0f*ypos ) / 128.0f );

		if( m_state.textureset.texture == TS_NO_TEXTURE )
		{
			m_BorderTL.SetPosition( x1, y1 );
			m_BorderT[0].SetPosition( x2, y1 );
			m_BorderTR.SetPosition( x3, y1 );
			m_BorderR[0].SetPosition( x3, y2 );
			m_BorderBR.SetPosition( x3, y3 );
			m_BorderB[0].SetPosition( x2, y3 );
			m_BorderBL.SetPosition( x1, y3 );
			m_BorderL[0].SetPosition( x1, y2 );

			m_BorderT[0].SetSize( InteriorWidth, m_BorderT[0].GetHeight() );
			m_BorderB[0].SetSize( InteriorWidth, m_BorderB[0].GetHeight() );
			m_BorderL[0].SetSize( InteriorHeight, m_BorderL[0].GetHeight() );
			m_BorderR[0].SetSize( InteriorHeight, m_BorderR[0].GetHeight() );
			
			howmanyx = 1;
			howmanyy = 1;
		}
		else
		{
			//  TGS - I know this looks ugly, but because we only have art from the upper right hand corner
			//  and the top/bottom edge, we have to do this kind of math to get everything centered in the right place
			m_BorderTL.SetPosition( x1 + m_BorderTL.GetWidth() , y1 );
			m_BorderTR.SetPosition( x3 , y1 );
			m_BorderBR.SetPosition( x3 + m_BorderBR.GetWidth(), y3 );
			m_BorderBL.SetPosition( x1 , y3 );
			for ( l = 0; l < WINDOW_NUM_BORDER_PIECES; l++ ) {
				m_BorderT[ l ].SetPosition( x1 + m_BorderT[ 0 ].GetWidth() / 2.0f - 2.0f * WINDOW_EDGEOFFSET + m_BorderT[ 0 ].GetWidth() * (float)l, y1 + m_BorderT[ 0 ].GetHeight() / 2.0f - WINDOW_EDGEOFFSET);
				m_BorderB[ l ].SetPosition( x1 + m_BorderB[ 0 ].GetWidth() / 2.0f - 2.0f * WINDOW_EDGEOFFSET + m_BorderT[ 0 ].GetWidth() * (float)l, y3 + m_BorderB[ 0 ].GetHeight() / 2.0f + WINDOW_EDGEOFFSET);
				m_BorderL[ l ].SetPosition( x1 + m_BorderL[ 0 ].GetHeight() / 2.0f - WINDOW_EDGEOFFSET, y1 + m_BorderL[ 0 ].GetWidth() / 2.0f - 2.0f * WINDOW_EDGEOFFSET + m_BorderT[ 0 ].GetWidth() * (float)l);
				m_BorderR[ l ].SetPosition( x3 + m_BorderR[ 0 ].GetHeight() / 2.0f + WINDOW_EDGEOFFSET, y1 + m_BorderR[ 0 ].GetWidth() / 2.0f - 2.0f * WINDOW_EDGEOFFSET + m_BorderT[ 0 ].GetWidth() * (float)l);
			}

			if ( Math::Mod( InteriorWidth + 2.0f*xpos, m_BorderT[ 0 ].GetWidth() ) > m_BorderTL.GetWidth() / 2.0f) {
				howmanyx++;
			}

			if ( Math::Mod( InteriorHeight + 2.0f*ypos, m_BorderT[ 0 ].GetWidth() ) > m_BorderTL.GetWidth() / 2.0f ) {
				howmanyy++;
			}
		}

		float UnitHeight = 1.0f + InteriorHeight / m_BorderTL.GetHeight() / 2.0f;

		if( UnitHeight < 0.0f ) UnitHeight = 0.0f;
		if( UnitHeight > 1.0f ) UnitHeight = 1.0f;

		float UnitWidth = 1.0f + InteriorWidth / m_BorderTL.GetWidth() / 2.0f;

		if( UnitWidth < 0.0f ) UnitWidth = 0.0f;
		if( UnitWidth > 1.0f ) UnitWidth = 1.0f;

		m_BorderTL.CropRect( 0.0f, 0.0f, UnitWidth, UnitHeight );
		m_BorderTR.CropRect( 1.0f - UnitWidth, 0.0f, 1.0f, UnitHeight );
		m_BorderBR.CropRect( 1.0f - UnitWidth, 1.0f - UnitHeight, 1.0f, 1.0f );
		m_BorderBL.CropRect( 0.0f, 1.0f - UnitHeight, UnitWidth, 1.0f );

		for ( l = 0; l < WINDOW_NUM_BORDER_PIECES; l++ ) {
			m_BorderT[ l ].CropRect( 0.0f, 0.0f, 1.0f, UnitHeight );
			m_BorderR[ l ].CropRect( 1.0f - UnitWidth, 0.0f, 1.0f, 1.0f );
			m_BorderB[ l ].CropRect( 0.0f, 1.0f - UnitHeight, 1.0f, 1.0f );
			m_BorderL[ l ].CropRect( 0.0f, 0.0f, UnitWidth, 1.0f );
		}

		if( InteriorWidth < 0.0f ) InteriorWidth = 0.0f;
		if( InteriorHeight < 0.0f ) InteriorHeight = 0.0f;

		for ( l = 0; l < WINDOW_NUM_BORDER_PIECES; l++ ) {
			m_BorderT[ l ].Hide();
			m_BorderB[ l ].Hide();
			m_BorderL[ l ].Hide();
			m_BorderR[ l ].Hide();
		}

		for ( l = 0; l < howmanyx; l++ ) {
			m_BorderT[ l ].Show();
			m_BorderB[ l ].Show();
		}
		for ( l = 0; l < howmanyy; l++ ) {
			m_BorderL[ l ].Show();
			m_BorderR[ l ].Show();
		}
//		m_BorderT.SetSize( InteriorWidth + 2.0f*xpos, m_BorderT.GetHeight() );
//		m_BorderB.SetSize( InteriorWidth + 2.0f*xpos, m_BorderB.GetHeight() );
//		m_BorderL.SetSize( InteriorHeight + 2.0f*ypos, m_BorderL.GetHeight() );
//		m_BorderR.SetSize( InteriorHeight + 2.0f*ypos, m_BorderR.GetHeight() );

		float tx = (float) m_BorderT[ 0 ].m_state.textureset.sizex;
		float ty = (float) m_BorderL[ 0 ].m_state.textureset.sizey;

		if( tx < InteriorWidth ) tx = InteriorWidth;
		if( ty < InteriorHeight ) ty = InteriorHeight;

		for ( l = 0; l < WINDOW_NUM_BORDER_PIECES; l++ ) {
			m_BorderL[ l ].SetTextureRect( 0.0f, 0.0f, 1.0f, InteriorHeight / ty);
			m_BorderT[ l ].SetTextureRect( 0.0f, 0.0f, InteriorWidth / tx, 1.0f );
			m_BorderR[ l ].SetTextureRect( 0.0f, 0.0f, 1.0f, InteriorHeight / ty);
			m_BorderB[ l ].SetTextureRect( 0.0f, 0.0f, InteriorWidth / tx, 1.0f );
		}

		m_Interior.SetPosition( x2-xpos, y2-ypos );
		m_Interior.SetSize( InteriorWidth+xpos, InteriorHeight+ypos );

		m_CaptionBack.SetPosition( x2-xpos, y2-ypos );
		m_CaptionBack.SetSize( InteriorWidth+xpos, GetCaptionHeight() );

		m_BorderTL.SetColor( m_state.color.r, m_state.color.g, m_state.color.b, m_state.color.a );
		m_BorderTR.SetColor( m_state.color.r, m_state.color.g, m_state.color.b, m_state.color.a );
		m_BorderBR.SetColor( m_state.color.r, m_state.color.g, m_state.color.b, m_state.color.a );
		m_BorderBL.SetColor( m_state.color.r, m_state.color.g, m_state.color.b, m_state.color.a );

		for ( l = 0; l < WINDOW_NUM_BORDER_PIECES; l++ ) {
			m_BorderT[ l ].SetColor( m_state.color.r, m_state.color.g, m_state.color.b, m_state.color.a );
			m_BorderR[ l ].SetColor( m_state.color.r, m_state.color.g, m_state.color.b, m_state.color.a );
			m_BorderB[ l ].SetColor( m_state.color.r, m_state.color.g, m_state.color.b, m_state.color.a );
			m_BorderL[ l ].SetColor( m_state.color.r, m_state.color.g, m_state.color.b, m_state.color.a );
		}

		m_Interior.SetVertexColor( 0, 0.322f, 0.616f, 0.733f, 0.7f );
		m_Interior.SetVertexColor( 1, 0.322f, 0.616f, 0.733f, 0.7f );
		m_Interior.SetVertexColor( 2, 0.0f, 0.031f, 0.259f, 0.7f );
		m_Interior.SetVertexColor( 3, 0.0f, 0.031f, 0.259f, 0.7f );

		m_CaptionBack.SetVertexColor( 0, 0.118f, 0.216f, 0.529f, 0.65f );
		m_CaptionBack.SetVertexColor( 1, 0.118f, 0.216f, 0.529f, 0.65f );
		m_CaptionBack.SetVertexColor( 2, 0.047f, 0.118f, 0.286f, 0.65f );
		m_CaptionBack.SetVertexColor( 3, 0.047f, 0.118f, 0.286f, 0.65f );

/*		TGS - this makes for 1 solid background, be we don't want that this time
		for( int i = 0; i < 4; i++ )
			m_Interior.SetVertexColor( i, m_state.vertex[i].r, m_state.vertex[i].g, m_state.vertex[i].b, m_state.vertex[i].a ); 
*/
		m_Interior.m_state.shadow = m_state.shadow;
		m_CaptionBack.m_state.shadow = m_state.shadow;
		m_BorderTL.m_state.shadow = m_state.shadow;
		m_BorderTR.m_state.shadow = m_state.shadow;
		m_BorderBR.m_state.shadow = m_state.shadow;
		m_BorderBL.m_state.shadow = m_state.shadow;

		for ( l = 0; l < WINDOW_NUM_BORDER_PIECES; l++ )
		{
			m_BorderT[ l ].m_state.shadow = m_state.shadow;
			m_BorderR[ l ].m_state.shadow = m_state.shadow;
			m_BorderB[ l ].m_state.shadow = m_state.shadow;
			m_BorderL[ l ].m_state.shadow = m_state.shadow;
		}
	}

	m_Caption.SetOrigin( m_HorzAlign, m_VertAlign );
	m_Caption.SetPosition( m_state.size.x * m_HorzAlign, m_state.size.y * m_VertAlign );
	m_Caption.SetSize( m_Interior.m_state.size.x, m_Interior.m_state.size.y );

	if( m_state.textureset.handle == INVALID_HANDLE )
		m_Caption.SetSize( m_state.size.x, m_state.size.y );
}

float Window::GetBorderWidth( void )
{
	m_BorderTL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 1 );

	return (float) m_BorderTL.m_state.textureset.sizex;
}

float Window::GetBorderHeight( void )
{
	m_BorderTL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 1 );

	return (float) m_BorderTL.m_state.textureset.sizey;
}

float Window::GetCaptionHeight( void )
{
	return m_Caption.GetTextHeight();
}

//////////////////////////////////////////////////////////////////////////

FrameWindow::FrameWindow( void )
{
	AddChild( &m_Interior );
	AddChild( &m_BorderTL );
	AddChild( &m_BorderTR );
	AddChild( &m_BorderBR );
	AddChild( &m_BorderBL );
	AddChild( &m_BorderT );
	AddChild( &m_BorderR );
	AddChild( &m_BorderB );
	AddChild( &m_BorderL );
	AddChild( &m_Caption );
	SetCaption( "" );
}

FrameWindow::~FrameWindow()
{
}

void FrameWindow::ShowInterior( void )
{
	m_Interior.Show();
}

void FrameWindow::HideInterior( void )
{
	m_Interior.Hide();
}

void FrameWindow::SetCaption( const char *pCaption, float HorzAlign, float VertAlign )
{
	m_Caption.SetText( pCaption );
	m_HorzAlign = HorzAlign;
	m_VertAlign = VertAlign;
}

void FrameWindow::Reset( void )
{
	Sprite::Reset();

	if( m_state.textureset.texture == TS_NO_TEXTURE )
	{
		m_Interior.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );

		m_BorderTL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
		m_BorderTR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
		m_BorderBR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
		m_BorderBL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );

		m_BorderT.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
		m_BorderR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
		m_BorderB.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
		m_BorderL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );

		m_BorderTL.SetSize( 1.0f, 1.0f );
		m_BorderTR.SetSize( 1.0f, 1.0f );
		m_BorderBR.SetSize( 1.0f, 1.0f );
		m_BorderBL.SetSize( 1.0f, 1.0f );

		m_BorderT.SetSize( 1.0f, 1.0f );
		m_BorderR.SetSize( 1.0f, 1.0f );
		m_BorderB.SetSize( 1.0f, 1.0f );
		m_BorderL.SetSize( 1.0f, 1.0f );
	}
	else
	{
		m_BorderTL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 1 );
		m_BorderT.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 2 );
		m_BorderTR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 3 );
		m_BorderR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 4 );
		m_BorderBR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 5 );
		m_BorderB.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 6 );
		m_BorderBL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 7 );
		m_BorderL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 8 );

		m_BorderTL.UseTextureSize();
		m_BorderTR.UseTextureSize();
		m_BorderBR.UseTextureSize();
		m_BorderBL.UseTextureSize();
	}

	SetCaption( "" );

	m_Interior.SetPriority( -50 );
	m_BorderTL.SetPriority( -50 );
	m_BorderTR.SetPriority( -50 );
	m_BorderBR.SetPriority( -50 );
	m_BorderBL.SetPriority( -50 );

	m_BorderT.SetPriority( -52 );
	m_BorderR.SetPriority( -52 );
	m_BorderB.SetPriority( -52 );
	m_BorderL.SetPriority( -52 );

	float sizex = 0.0f, sizey = 0.0f;

	sizex += m_BorderTL.GetWidth();
	sizex += m_BorderT.GetWidth();
	sizex += m_BorderTR.GetWidth();

	sizey += m_BorderTL.GetHeight();
	sizey += m_BorderL.GetHeight();
	sizey += m_BorderBL.GetHeight();

	SetSize( sizex, sizey );
}

void FrameWindow::ComputeSprite( void )
{
	if( !m_state.sprite.visible )
	{
		m_Interior.Release();
		m_BorderTL.Release();
		m_BorderT.Release();
		m_BorderTR.Release();
		m_BorderR.Release();
		m_BorderBR.Release();
		m_BorderB.Release();
		m_BorderBL.Release();
		m_BorderL.Release();

		return;
	}
	
	if( FLOATTOINT(m_state.familycolor.a) == 0 )
		return;
	
	if( m_state.textureset.handle != INVALID_HANDLE )
	{
		if( m_state.textureset.texture == TS_NO_TEXTURE )
		{
			m_Interior.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
			m_BorderTL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
			m_BorderT.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
			m_BorderTR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
			m_BorderR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
			m_BorderBR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
			m_BorderB.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
			m_BorderBL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
			m_BorderL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
	
			m_BorderTL.SetSize( 4.0f, 4.0f );
			m_BorderT.SetSize( 4.0f, 4.0f );
			m_BorderTR.SetSize( 4.0f, 4.0f );
			m_BorderR.SetSize( 4.0f, 4.0f );
			m_BorderBR.SetSize( 4.0f, 4.0f );
			m_BorderB.SetSize( 4.0f, 4.0f );
			m_BorderBL.SetSize( 4.0f, 4.0f );
			m_BorderL.SetSize( 4.0f, 4.0f );
		}
		else
		{
			m_BorderTL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 1 );
			m_BorderT.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 2 );
			m_BorderTR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 3 );
			m_BorderR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 4 );
			m_BorderBR.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 5 );
			m_BorderB.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 6 );
			m_BorderBL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 7 );
			m_BorderL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 8 );

			m_BorderTL.UseTextureSize();
			m_BorderT.UseTextureSize();
			m_BorderTR.UseTextureSize();
			m_BorderR.UseTextureSize();
			m_BorderBR.UseTextureSize();
			m_BorderB.UseTextureSize();
			m_BorderBL.UseTextureSize();
			m_BorderL.UseTextureSize();
		}

		float x1 = 0.0f;
		float y1 = 0.0f;
		float x2 = x1 + m_BorderTL.m_state.size.x;
		float y2 = y1 + m_BorderTL.m_state.size.y;
		float x3 = x1 + m_state.size.x - m_BorderBR.m_state.size.x;
		float y3 = y1 + m_state.size.y - m_BorderBR.m_state.size.y;

		m_BorderTL.SetPosition( x1, y1 );
		m_BorderT.SetPosition( x2, y1 );
		m_BorderTR.SetPosition( x3, y1 );
		m_BorderR.SetPosition( x3, y2 );
		m_BorderBR.SetPosition( x3, y3 );
		m_BorderB.SetPosition( x2, y3 );
		m_BorderBL.SetPosition( x1, y3 );
		m_BorderL.SetPosition( x1, y2 );

		//////////////////////////////////////////////////////////////////////////
		
		float InteriorWidth = x3 - x2;
		float InteriorHeight = y3 - y2;

		//////////////////////////////////////////////////////////////////////////
		
		float UnitHeight = 1.0f + InteriorHeight / m_BorderTL.GetHeight() / 2.0f;

		if( UnitHeight < 0.0f ) UnitHeight = 0.0f;
		if( UnitHeight > 1.0f ) UnitHeight = 1.0f;

		//////////////////////////////////////////////////////////////////////////
		
		float UnitWidth = 1.0f + InteriorWidth / m_BorderTL.GetWidth() / 2.0f;

		if( UnitWidth < 0.0f ) UnitWidth = 0.0f;
		if( UnitWidth > 1.0f ) UnitWidth = 1.0f;
		
		//////////////////////////////////////////////////////////////////////////
		
		m_BorderTL.CropRect( 0.0f, 0.0f, UnitWidth, UnitHeight );
		m_BorderTR.CropRect( 1.0f - UnitWidth, 0.0f, 1.0f, UnitHeight );
		m_BorderBR.CropRect( 1.0f - UnitWidth, 1.0f - UnitHeight, 1.0f, 1.0f );
		m_BorderBL.CropRect( 0.0f, 1.0f - UnitHeight, UnitWidth, 1.0f );

		//////////////////////////////////////////////////////////////////////////

		m_BorderT.CropRect( 0.0f, 0.0f, 1.0f, UnitHeight );
		m_BorderR.CropRect( 1.0f - UnitWidth, 0.0f, 1.0f, 1.0f );
		m_BorderB.CropRect( 0.0f, 1.0f - UnitHeight, 1.0f, 1.0f );
		m_BorderL.CropRect( 0.0f, 0.0f, UnitWidth, 1.0f );

		if( InteriorWidth < 0.0f ) InteriorWidth = 0.0f;
		if( InteriorHeight < 0.0f ) InteriorHeight = 0.0f;

		m_BorderT.SetSize( InteriorWidth, m_BorderT.GetHeight() );
		m_BorderB.SetSize( InteriorWidth, m_BorderB.GetHeight() );
		m_BorderL.SetSize( m_BorderL.GetWidth(), InteriorHeight );
		m_BorderR.SetSize( m_BorderR.GetWidth(), InteriorHeight );

		float tx = (float) m_BorderT.m_state.textureset.sizex;
		float ty = (float) m_BorderL.m_state.textureset.sizey;

		if( tx < InteriorWidth ) tx = InteriorWidth;
		if( ty < InteriorHeight ) ty = InteriorHeight;

		m_BorderL.SetTextureRect( 0.0f, 0.0f, 1.0f, InteriorHeight / ty);
		m_BorderT.SetTextureRect( 0.0f, 0.0f, InteriorWidth / tx, 1.0f );
		m_BorderR.SetTextureRect( 0.0f, 0.0f, 1.0f, InteriorHeight / ty);
		m_BorderB.SetTextureRect( 0.0f, 0.0f, InteriorWidth / tx, 1.0f );

		m_Interior.SetPosition( x2, y2 );
		m_Interior.SetSize( InteriorWidth, InteriorHeight );

		m_BorderTL.SetColor( m_state.color.r, m_state.color.g, m_state.color.b, m_state.color.a );
		m_BorderTR.SetColor( m_state.color.r, m_state.color.g, m_state.color.b, m_state.color.a );
		m_BorderBR.SetColor( m_state.color.r, m_state.color.g, m_state.color.b, m_state.color.a );
		m_BorderBL.SetColor( m_state.color.r, m_state.color.g, m_state.color.b, m_state.color.a );

		m_BorderT.SetColor( m_state.color.r, m_state.color.g, m_state.color.b, m_state.color.a );
		m_BorderR.SetColor( m_state.color.r, m_state.color.g, m_state.color.b, m_state.color.a );
		m_BorderB.SetColor( m_state.color.r, m_state.color.g, m_state.color.b, m_state.color.a );
		m_BorderL.SetColor( m_state.color.r, m_state.color.g, m_state.color.b, m_state.color.a );

		for( int i = 0; i < 4; i++ )
			m_Interior.SetVertexColor( i, m_state.vertexcolor[i].r, m_state.vertexcolor[i].g, m_state.vertexcolor[i].b, m_state.vertexcolor[i].a ); 

		m_Interior.m_state.shadow = m_state.shadow;
		m_BorderTL.m_state.shadow = m_state.shadow;
		m_BorderTR.m_state.shadow = m_state.shadow;
		m_BorderBR.m_state.shadow = m_state.shadow;
		m_BorderBL.m_state.shadow = m_state.shadow;

		m_BorderT.m_state.shadow = m_state.shadow;
		m_BorderR.m_state.shadow = m_state.shadow;
		m_BorderB.m_state.shadow = m_state.shadow;
		m_BorderL.m_state.shadow = m_state.shadow;
	}

	m_Caption.SetOrigin( m_HorzAlign, m_VertAlign );
	m_Caption.SetPosition( m_state.size.x * m_HorzAlign, m_state.size.y * m_VertAlign );
	m_Caption.SetSize( m_Interior.m_state.size.x, m_Interior.m_state.size.y );

	if( m_state.textureset.handle == INVALID_HANDLE )
		m_Caption.SetSize( m_state.size.x, m_state.size.y );
}

float FrameWindow::GetBorderWidth( void )
{
	m_BorderTL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 1 );

	return (float) m_BorderTL.m_state.textureset.sizex;
}

float FrameWindow::GetBorderHeight( void )
{
	m_BorderTL.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture + 1 );

	return (float) m_BorderTL.m_state.textureset.sizey;
}

float FrameWindow::GetCaptionHeight( void )
{
	return m_Caption.GetTextHeight();
}