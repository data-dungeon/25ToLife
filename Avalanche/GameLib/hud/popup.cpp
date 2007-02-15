/******************* includes ****************************/

#include "hud/HudPCH.h"
#include "hud/popup.h"

PopupItem::PopupItem( int Id )
{
	Enable(), SetId( Id );
}

PopupMenu::PopupMenu( void )
{
	m_Window.SetParent( &m_Shuttle );
	m_Window.AddChild( &m_Border );
	m_Window.AddChild( &m_ArrowUp );
	m_Window.AddChild( &m_Select );
	m_ArrowUp.AddChild( &m_Move );

	m_Shuttle.InitAnim();

	m_Mode = POPUPMODE_VERTICAL;
	m_Active = false;
	m_CallBack = 0;
}

PopupMenu::~PopupMenu( void )
{
}

bool PopupMenu::Create( int Mode, t_Handle TextureSet, int Texture )
{
	m_Border.Create( TextureSet, Texture+0 );
	m_Window.Create( TextureSet, Texture+9 );

	m_Mode = Mode;
	
	return true;
}

void PopupMenu::Create2( const char *pCaption, t_Handle TextureSet, int Texture, POPUPSTYLE Style, float xPos, float yPos, float xScale, float yScale )
{
	m_Border.Create( TextureSet, Texture+0 );
	m_Window.Create( TextureSet, Texture+9 );
}

void PopupMenu::Draw( void )
{
	m_Border.SetSize( m_Window.GetWidth(), m_Window.GetHeight() );
	m_Border.HideInterior();

	float dx = m_Window.GetBorderWidth();
	float dy = m_Window.GetBorderHeight();
	float th = m_Window.GetCaptionHeight();

	float x1 = dx;
	float y1 = th;
	float x2 = m_Window.GetWidth() - dx;
	float y2 = m_Window.GetHeight() - th * 0.66f;

	float InteriorWidth = x2 - x1;
	float InteriorHeight = y2 - y1;

	int Count = GetNumChildren();
	float Countf = (float) Count;

	float ItemHeight = InteriorHeight / Countf;
	float ItemSpacing = ItemHeight;
	float TextHeight = ItemHeight * 1.2f;
	float Top = y1 + ItemSpacing * 0.5f;

	if( TextHeight < 35.0f )
		TextHeight = 35.0f;

	if( TextHeight > m_Window.m_Caption.GetTextHeight() * 0.75f )
		TextHeight = m_Window.m_Caption.GetTextHeight() * 0.75f;


	PopupItem *p = (PopupItem *) Child();

	for( int i = 0; i < Count; i++ )
	{
		p->m_Text.SetTextHeight( TextHeight );
		p->m_Text.SetOrigin( 0.5f, 0.5f );
		p->m_Text.SetPosition( m_Window.GetWidth() * 0.5f, Top + ItemSpacing * i );
		
		p = (PopupItem *) p->Next();
	}

	m_Shuttle.Update();
}

void PopupMenu::SetPosition( float x, float y )
{
	m_Shuttle.SetPosition( x, y );
}

void PopupMenu::SetOrigin( float x, float y )
{
	m_Window.SetOrigin( x, y );
}

void PopupMenu::SetScale( float x, float y )
{
	m_Window.SetScale( x, y );
}

void PopupMenu::SetSize( float x, float y )
{
	m_Window.SetSize( x, y );
}

void PopupMenu::OnCursorUp( void )
{
	if( m_Mode == POPUPMODE_VERTICAL )
		SelectPrev();

	return;
}

void PopupMenu::OnCursorDown( void )
{
	if( m_Mode == POPUPMODE_VERTICAL )
		SelectNext();

	return;
}

void PopupMenu::OnCursorLeft( void )
{
	if( m_Mode == POPUPMODE_HORIZONTAL )
		SelectPrev();

	return;
}

void PopupMenu::OnCursorRight( void )
{
	if( m_Mode == POPUPMODE_HORIZONTAL )
		SelectNext();

	return;
}

void PopupMenu::OnButtonOK( void )
{
	if( m_CallBack )
		StaticCallBack( m_CallBack, (PARAM) m_MenuChoice );

	return;
}

void PopupMenu::OnButtonCancel( void )
{
	if( m_CallBack )
		StaticCallBack( m_CallBack, (PARAM) IDM_CANCEL );

	return;
}

void PopupMenu::OnSelectionChange( void )
{
}

void PopupMenu::Animate( bool TrueOrFalse )
{
	m_Window.InitAnim();

	if( TrueOrFalse )
	{	//  TGS - ugly for now, but it works ( for 04/15/03 deadline )
		m_Window.m_anim->rotation.z.TimeCurve( true );
		m_Window.m_anim->rotation.z.SetFrame( 0.0f, -3.0f );
		m_Window.m_anim->rotation.z.NextFrame( 2.0f, -3.0f );
		m_Window.m_anim->rotation.z.NextFrame( 4.0f, -3.0f );
		m_Window.m_anim->rotation.z.Play( true );
/*
		m_Window.m_anim->scale.x.TimeCurve( true );
		m_Window.m_anim->scale.x.SetFrame( 0.0f, 1.0f );
		m_Window.m_anim->scale.x.NextFrame( 1.0f, 1.1f );
		m_Window.m_anim->scale.x.NextFrame( 1.0f, 1.0f );
		m_Window.m_anim->scale.x.Play( true );

		m_Window.m_anim->scale.y.TimeCurve( true );
		m_Window.m_anim->scale.y.SetFrame( 0.0f, 1.0f );
		m_Window.m_anim->scale.y.NextFrame( 0.6f, 1.0f );
		m_Window.m_anim->scale.y.NextFrame( 1.0f, 1.1f );
		m_Window.m_anim->scale.y.NextFrame( 1.0f, 1.0f );
		m_Window.m_anim->scale.y.SetLoopPoint( 0.6f );
		m_Window.m_anim->scale.y.Play( true );*/
	}
	else
		m_Window.m_anim->Clear();

	return;
}


void PopupMenu::Activate( float Time, int PopupAction )
{
	m_Active = true;

	m_Shuttle.m_anim->Clear();

	switch( PopupAction )
	{
	case POPUPACTION_DEFAULT:
		{
			// The default animation takes 0.5 seconds; scale it to the user's preference

			m_Shuttle.m_anim->scale.y.TimeCurve( true );
			m_Shuttle.m_anim->scale.y.SetFrame( 0.000f * Time, 0.00f );
			m_Shuttle.m_anim->scale.y.SetFrame( 0.400f * Time, 1.30f );
			m_Shuttle.m_anim->scale.y.SetFrame( 0.600f * Time, 0.90f );
			m_Shuttle.m_anim->scale.y.SetFrame( 0.800f * Time, 1.05f );
			m_Shuttle.m_anim->scale.y.SetFrame( 1.000f * Time, 1.00f );
			m_Shuttle.m_anim->scale.y.Play( false );

			m_Shuttle.m_anim->scale.x.TimeCurve( true );
			m_Shuttle.m_anim->scale.x.SetFrame( 0.000f * Time, 0.00f );
			m_Shuttle.m_anim->scale.x.SetFrame( 0.400f * Time, 0.80f );
			m_Shuttle.m_anim->scale.x.SetFrame( 0.600f * Time, 1.10f );
			m_Shuttle.m_anim->scale.x.SetFrame( 0.800f * Time, 0.95f );
			m_Shuttle.m_anim->scale.x.SetFrame( 1.000f * Time, 1.00f );
			m_Shuttle.m_anim->scale.x.Play( false );

			m_Shuttle.m_anim->familycolor.a.SetFrame( 0.0f * Time, 0.0f );
			m_Shuttle.m_anim->familycolor.a.SetFrame( 0.2f * Time, 1.0f );
			m_Shuttle.m_anim->familycolor.a.Play( false );

			m_Shuttle.m_anim->rotation.z.TimeCurve( true );
			m_Shuttle.m_anim->rotation.z.SetFrame( 0.000f * Time, 180.0f );
			m_Shuttle.m_anim->rotation.z.SetFrame( 0.500f * Time, 0.0f );
			m_Shuttle.m_anim->rotation.z.Play( false );
		}
		break;
	}

	OnActivate();

	return;
}

void PopupMenu::Deactivate( float Time, int PopupAction )
{
	m_Active = false;

	m_Shuttle.m_anim->Clear();

	switch( PopupAction )
	{
	case POPUPACTION_DEFAULT:
		{
			m_Shuttle.m_anim->scale.x.TimeCurve( true );
			m_Shuttle.m_anim->scale.x.SetFrame( 0.000f * Time, 1.0f );
			m_Shuttle.m_anim->scale.x.SetFrame( 1.000f * Time, 0.0f );
			m_Shuttle.m_anim->scale.x.Play( false );

			m_Shuttle.m_anim->scale.y.TimeCurve( true );
			m_Shuttle.m_anim->scale.y.SetFrame( 0.000f * Time, 1.0f );
			m_Shuttle.m_anim->scale.y.SetFrame( 1.000f * Time, 0.0f );
			m_Shuttle.m_anim->scale.y.Play( false );

			m_Shuttle.m_anim->familycolor.a.TimeCurve( true );
			m_Shuttle.m_anim->familycolor.a.SetFrame( 0.000f * Time, 1.0f );
			m_Shuttle.m_anim->familycolor.a.SetFrame( 0.700f * Time, 1.0f );
			m_Shuttle.m_anim->familycolor.a.SetFrame( 1.000f * Time, 0.0f );
			m_Shuttle.m_anim->familycolor.a.Play( false );
		}
		break;
	}

	OnDeactivate();

	return;
}

void PopupMenu::OnActivate( void )
{
}

void PopupMenu::OnDeactivate( void )
{
}

int PopupMenu::GetMenuChoice( void )
{
	return m_MenuChoice;
}

void PopupMenu::SelectPrev( void )
{
	PopupItem *p = GetPopupItem( m_MenuChoice );

	if( p )
	{
		p = (PopupItem *) p->Prev();

		while( !p->m_Enabled )
		{
			if( p->Prev() )
				p = (PopupItem *) p->Prev();
			else
			{
				while( p->Next() )
					p = (PopupItem *) p->Next();
			}
		}

		Select( p->m_Id );
	}

	return;
}

void PopupMenu::SelectNext( void )
{
	PopupItem *p = GetPopupItem( m_MenuChoice );

	if( p )
	{
		p = (PopupItem *) p->Next();

		while( !p->m_Enabled )
		{
			if( p->Prev() )
				p = (PopupItem *) p->Next();
			else
			{
				while( p->Next() )
					p = (PopupItem *) p->Prev();
			}
		}

		Select( p->m_Id );
	}

	return;
}

void PopupMenu::Select( int Id )
{
	m_MenuChoice = Id;

	OnSelectionChange();
}

void PopupMenu::Add( char *pName, int Id, Font *pFont, t_Handle TextureSet, int Texture )
{
	PopupItem *p = new PopupItem( Id );

	if( p )
	{
		AddChild( p );

		p->m_Text.SetParent( &m_Window );
		p->m_Text.SetFont( pFont );
		p->m_Text.SetText( pName );
		p->m_Text.ShadowSettings( true );
		p->m_Text.SetTextSpacing( -5.0f );

		p->m_Picture.Create( TextureSet, Texture );
	}

	return;
}

void PopupMenu::Remove( int Id )
{
	PopupItem *p = GetPopupItem( Id );

	if( p )
		delete p;

	return;
}

PopupItem *PopupMenu::GetPopupItem( int Id )
{
	PopupItem *p = (PopupItem *) Child();

	while( p )
	{
		if( Id == p->GetId() )
			return p;

		p = (PopupItem *) p->Next();
	}

	return 0;
}

//===================

PopupWindow::PopupWindow( void )
{
	AddChild( &m_Window );
	m_Window.AddChild( &m_Border );

	InitAnim();
	m_Window.InitAnim();

	m_Active = false;
}

PopupWindow::~PopupWindow()
{
}

void PopupWindow::Reset( void )
{
	Sprite::Reset();

	m_Window.Reset();
	m_Border.Reset();

	SetCaptionColor( 1.0f, 1.0f, 1.0f, 1.0f );
	SetCaptionAppearance( 0.0f, 0.0f );
	SetCaptionShadow( false );
}

void PopupWindow::Animate( bool TrueOrFalse )
{
	if( TrueOrFalse )
	{	//  TGS - ugly for now, but it works ( for 04/15/03 deadline )
		m_Window.m_anim->rotation.z.Clear();
		m_Window.m_anim->rotation.z.SetFrame( 0.0f, -3.0f );
		m_Window.m_anim->rotation.z.NextFrame( 2.0f, -3.0f );
		m_Window.m_anim->rotation.z.NextFrame( 4.0f, -3.0f );
		m_Window.m_anim->rotation.z.TimeCurve( true );
		m_Window.m_anim->rotation.z.Play( true );
/*
		m_Window.m_anim->scale.x.Clear();
		m_Window.m_anim->scale.x.SetFrame( 0.0f, 1.0f );
		m_Window.m_anim->scale.x.NextFrame( 1.0f, 1.1f );
		m_Window.m_anim->scale.x.NextFrame( 1.0f, 1.0f );
		m_Window.m_anim->scale.x.TimeCurve( true );
		m_Window.m_anim->scale.x.Play( true );

		m_Window.m_anim->scale.y.Clear();
		m_Window.m_anim->scale.y.SetFrame( 0.0f, 1.0f );
		m_Window.m_anim->scale.y.NextFrame( 0.6f, 1.0f );
		m_Window.m_anim->scale.y.NextFrame( 1.0f, 1.1f );
		m_Window.m_anim->scale.y.NextFrame( 1.0f, 1.0f );
		m_Window.m_anim->scale.y.SetLoopPoint( 0.6f );
		m_Window.m_anim->scale.y.TimeCurve( true );
		m_Window.m_anim->scale.y.Play( true );*/
	}
	else
		m_Window.m_anim->Clear();

	return;

}

void PopupWindow::Activate( float Time, int Mode )
{
	int j;
	if( !m_Active )
	{
		m_Active = true;

		if( !m_anim )
			InitAnim();

		switch( Mode )
		{
		case 0:
			m_anim->offset.y.Clear();
			m_anim->offset.y.TimeCurve( true );
			for ( j = 0; j < 14; j++ ) {
				m_anim->offset.y.SetFrame( 0.035f * (float)j, -70.0f + 5.0f * (float)j );
			}
 			m_anim->offset.y.SetFrame( 0.80f , 0.0f );
			m_anim->offset.y.Play( false );

			m_anim->scale.y.Clear();
			m_anim->scale.y.TimeCurve( true );
			for ( j = 0; j < 11; j++ ) {
				m_anim->scale.y.SetFrame( 0.035f * (float)j, 1.30f - 0.05f * (float)j );
			}
			for ( j = 11; j < 14; j++ ) {
				m_anim->scale.y.SetFrame( 0.035f * (float)j, 0.80f + 0.05f * (float)( j - 11 ) );
			}
			m_anim->scale.y.SetFrame( 0.80f , 1.00f );
			m_anim->scale.y.Play( false );

			m_anim->scale.x.Clear();
			m_anim->scale.x.TimeCurve( true );
			for ( j = 0; j < 11; j++ ) {
				m_anim->scale.x.SetFrame( 0.035f * (float)j, 1.30f - 0.05f * (float)j );
			}
			for ( j = 11; j < 14; j++ ) {
				m_anim->scale.x.SetFrame( 0.035f * (float)j, 0.80f + 0.05f * (float)( j - 11 ) );
			}
			m_anim->scale.x.SetFrame( 0.80f , 1.00f );
			m_anim->scale.x.Play( false );
			
			m_anim->familycolor.a.Clear();
			m_anim->familycolor.a.TimeCurve( true );
			m_anim->familycolor.a.SetFrame( 0.00f , 0.15f );
			m_anim->familycolor.a.SetFrame( 0.035f , 0.18f );
			m_anim->familycolor.a.SetFrame( 0.07f , 0.21f );
			m_anim->familycolor.a.SetFrame( 0.105f , 0.25f );
			m_anim->familycolor.a.SetFrame( 0.14f , 0.29f );
			m_anim->familycolor.a.SetFrame( 0.175f , 0.34f );
			m_anim->familycolor.a.SetFrame( 0.21f , 0.39f );
			m_anim->familycolor.a.SetFrame( 0.245f , 0.45f );
			m_anim->familycolor.a.SetFrame( 0.28f , 0.51f );
			m_anim->familycolor.a.SetFrame( 0.315f , 0.58f );
			m_anim->familycolor.a.SetFrame( 0.35f , 0.65f );
			m_anim->familycolor.a.SetFrame( 0.385f , 0.73f );
			m_anim->familycolor.a.SetFrame( 0.42f , 0.81f );
			m_anim->familycolor.a.SetFrame( 0.455f , 0.90f );
			m_anim->familycolor.a.SetFrame( 0.80f , 1.00f );
			m_anim->familycolor.a.Play( false );
/*
			m_anim->scale.y.Clear();
			m_anim->scale.y.TimeCurve( true );
			m_anim->scale.y.SetFrame( 0.000f * Time, 0.00f );
			m_anim->scale.y.SetFrame( 0.400f * Time, 0.80f );
			m_anim->scale.y.SetFrame( 0.600f * Time, 1.10f );
			m_anim->scale.y.SetFrame( 0.800f * Time, 0.95f );
			m_anim->scale.y.SetFrame( 1.000f * Time, 1.00f );
			m_anim->scale.y.Play( false );

			m_anim->scale.x.Clear();
			m_anim->scale.x.TimeCurve( true );
			m_anim->scale.x.SetFrame( 0.000f * Time, 0.00f );
			m_anim->scale.x.SetFrame( 0.400f * Time, 0.80f );
			m_anim->scale.x.SetFrame( 0.600f * Time, 1.10f );
			m_anim->scale.x.SetFrame( 0.800f * Time, 0.95f );
			m_anim->scale.x.SetFrame( 1.000f * Time, 1.00f );
			m_anim->scale.x.Play( false );

			m_anim->familycolor.a.Clear();
//			m_anim->familycolor.a.SetFrame( 0.0f * Time, 0.0f );
			m_anim->familycolor.a.SetFrame( 0.2f , 1.0f );
			m_anim->familycolor.a.Play( false );

			m_anim->rotation.z.Clear();
			m_anim->rotation.z.TimeCurve( true );
//			m_anim->rotation.z.SetFrame( 0.000f * Time, 180.0f );
			m_anim->rotation.z.SetFrame( 0.500f , 0.0f );
			m_anim->rotation.z.Play( false );*/
			break;
		case 1:
			m_anim->offset.x.Clear();
			m_anim->offset.x.SetFrame( 0.0f, -Sprite::SimulatedScreenWidth() * 2.0f );
			m_anim->offset.x.SetFrame( Time, 0.0f );
			m_anim->offset.x.Deccelerate( true );
			m_anim->offset.x.Play( false );

			m_anim->familycolor.a.Clear();
			m_anim->familycolor.a.SetFrame( 0.0f, m_state.familycolor.a );
			m_anim->familycolor.a.SetFrame( Time, 1.0f );
			m_anim->familycolor.a.Deccelerate( true );
			m_anim->familycolor.a.Play( false );

			break;
		}
	}

	return;
}

void PopupWindow::Deactivate( float Time, int Mode )
{
	if( m_Active )
	{
		m_Active = false;

		if( !m_anim )
			InitAnim();

		switch( Mode )
		{
		case 0:
			m_anim->scale.x.Clear();
			m_anim->scale.x.TimeCurve( true );
			m_anim->scale.x.SetFrame( 0.000f * Time, 0.0f );
//			m_anim->scale.x.SetFrame( 1.000f * Time, 0.0f );
			m_anim->scale.x.Play( false );

			m_anim->scale.y.Clear();
			m_anim->scale.y.TimeCurve( true );
			m_anim->scale.y.SetFrame( 0.000f * Time, 0.0f );
//			m_anim->scale.y.SetFrame( 1.000f * Time, 0.0f );
			m_anim->scale.y.Play( false );

			m_anim->familycolor.a.Clear();
			m_anim->familycolor.a.TimeCurve( true );
			m_anim->familycolor.a.SetFrame( 0.000f * Time, 0.0f );
//			m_anim->familycolor.a.SetFrame( 0.700f * Time, 1.0f );
			m_anim->familycolor.a.SetFrame( 1.000f * Time, 0.0f );
			m_anim->familycolor.a.Play( false );
			break;
		case 1:
			m_anim->offset.x.Clear();
			m_anim->offset.x.SetFrame( 0.0f, m_state.offset.x );
			m_anim->offset.x.SetFrame( Time, Sprite::SimulatedScreenWidth() * 2.0f );
			m_anim->offset.x.Accelerate( true );
			m_anim->offset.x.Play( false );

			m_anim->familycolor.a.Clear();
			m_anim->familycolor.a.SetFrame( 0.0f, m_state.familycolor.a );
			m_anim->familycolor.a.SetFrame( Time, 0.0f );
			m_anim->familycolor.a.Deccelerate( true );
			m_anim->familycolor.a.Play( false );
			break;
		}
	}

	return;
}

void PopupWindow::SetCaption( const char *pCaption )
{
	m_Window.SetCaption( pCaption, 0.5f, 0.0f );
}

void PopupWindow::SetCaptionColor( float r, float g, float b, float a )
{
	m_Window.m_Caption.SetColor( r, g, b, a );
}

void PopupWindow::SetCaptionAppearance( float TextHeight, float TextSpacing, float TextAlignment )
{
	m_Window.m_Caption.SetTextHeight( TextHeight );
	m_Window.m_Caption.SetTextSpacing( TextSpacing );
	m_Window.m_Caption.SetTextAlignment( TextAlignment );
}

void PopupWindow::SetCaptionShadow( bool Enabled, u8 Intensity /*= 255*/, s8 x /*= 2*/, s8 y /*= 2*/ )
{
	m_Window.m_Caption.ShadowSettings( Enabled, Intensity, x, y );
}

bool PopupWindow::Active( void )
{
	return m_Active;
}

void PopupWindow::ComputeSprite( void )
{
	m_Window.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
	m_Window.SetPosition( m_state.size.x * m_state.origin.x, m_state.size.y * m_state.origin.y );
	m_Window.SetOrigin( m_state.origin.x, m_state.origin.y );
	m_Window.SetSize( m_state.size.x, m_state.size.y );
	m_Window.SetColor( m_state.color.r, m_state.color.g, m_state.color.b, m_state.color.a );

	for( int i = 0; i < 4; i++ )
		m_Window.SetVertexColor( i, m_state.vertexcolor[i].r, m_state.vertexcolor[i].g, m_state.vertexcolor[i].b, m_state.vertexcolor[i].a );

	if( m_state.textureset.texture == TS_NO_TEXTURE )
		m_Border.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture );
	else
	{
		m_Border.SetTextureSet( m_state.textureset.handle, m_state.textureset.texture-9 );
	}

	m_Border.SetSize( m_state.size.x, m_state.size.y );
	m_Border.HideInterior();
}

//=============================================================================

PopupItem2::PopupItem2( int Id )
{
	SetId( Id );

	m_Text.AddChild( &m_Sprite );
}

void PopupItem2::SetId( int Id )
{
	m_Id = Id;
}

int PopupItem2::GetId( void )
{
	return m_Id;
}

PopupMenu2::PopupMenu2( void )
{
	m_pSelection = 0;
	m_pPrevSelection = 0;
	m_CallBack = 0;
	m_CallBackParam = 0;
	m_Command = IDM_INVALID;
	m_Active = false;
	f_InitalYOffset = 0.0f;
	m_ScrollTop = 0;
	m_ScrollBottom = 0;

	m_CancelCommand = IDM_CANCEL;

	EnableCursor( true );
	EnableCenterHighlight( false );

	m_Window.AddChild( &m_Message );
	m_Window.AddChild( &m_MessageBackground );
	m_Window.AddChild( &m_ScrollBar );
	m_Window.AddChild( &m_Buttons );

	Activate( 0.0f );
	Deactivate( 0.0f );
}

PopupMenu2::~PopupMenu2()
{
	Clear();
}

void PopupMenu2::LoadTextureSet( const char *pFileName )
{
	FreeTextureSet();

	m_TextureSet = dblLoad( pFileName );

	return;
}

void PopupMenu2::FreeTextureSet( void )
{
	m_TextureSet = INVALID_HANDLE;
}

t_Handle PopupMenu2::TextureSet( void )
{
	return m_TextureSet;
}

void PopupMenu2::Reset( void )
{
	PopupWindow::Reset();

	//===========================
	// clear the menu
	//===========================

	Clear();
	
	//===========================
	// default size
	//===========================

	SetSize( 320.0f, 256.0f );
	
	//===========================
	// default origin
	//===========================

	SetOrigin( 0.5f, 0.5f );
	
	//===========================
	// default screen position
	//===========================

	Sprite *pParent = Parent();

	if( !pParent )
		ScreenPosition( 0.5f, 0.5f );
	else
		SetPosition( pParent->m_state.size.x * 0.5f, pParent->m_state.size.y * 0.5f );

	//===========================
	// default window color
	//===========================
	
	SetColor( 0.0f, 0.0f, 0.7f, 0.4f );
	
	//===========================
	// default border color
	//===========================

	m_Border.SetColor( 1.0f, 1.0f, 0.6f, 1.0f );

	//===========================
	// default caption settings
	//===========================

	SetCaptionColor( 1.0f, 1.0f, 0.6f, 1.0f );
	SetCaptionAppearance( 75.0f, 0.0f );
	SetCaptionShadow( true );

	//===========================
	// default menu color
	//===========================

	SetMenuColor( 1.0f, 0.75f, 0.30f, 1.0f );

	//===========================
	// default highlight color
	//===========================

	SetHighlightColor( 1.0f, 1.0f, 1.0f, 1.0f );

	//===========================
	// default highlight scale
	//===========================

	SetHighlightScale( 1.0f, 1.0f );

	//===========================
	// default shadow settings
	//===========================

	SetMenuShadow( true );

	//===========================
	// default menu appearance
	//===========================

	SetMenuAppearance( 28.0f, -4.0f, 0.0f );

	//===========================
	// default alignment
	//===========================

	SetMenuAlignment( 0.5f );

	//===========================
	// default page size
	//===========================

	SetMenuPageSize( 2 );

	//===========================
	// default Margins
	//===========================

	SetMargins( 0.0f, 0.0f, 0.0f, 0.0f );

	//===========================
	// default selection
	//===========================
	
	SelectFirst();

	//===========================
	//  default number of columns
	//===========================
	SetNumberColumns( 1 );

	//===========================
	//  default extra spacing for rows
	//===========================
	SetRowSpacer( 0.0f );
}

void PopupMenu2::Add( const char *pName, int Id )
{
	PopupItem2 *p = new PopupItem2( Id );

	if( p )
	{
		p->m_Text.InitAnim();
		p->m_Text.SetText( pName );
		p->m_Text.SetOrigin( 0.5f, 0.5f );

		m_Window.AddChild( &p->m_Text );

//		if( !m_pSelection )
//			m_pSelection = p;
	}

	m_MenuRoot.AddChild( p );
}

void PopupMenu2::Delete( int Id )
{
	PopupItem2 *p = new PopupItem2( Id );

	while( p && p->GetId() != Id )
		p = (PopupItem2 *) p->Next();

	if( p )
		delete p;

	return;
}

void PopupMenu2::Clear( void )
{
	SetCaption( "" );

	m_Highlight.Remove();
	m_MessageBackground.Remove();

	while( m_MenuRoot.Child() )
		delete m_MenuRoot.Child();

	m_pSelection = 0;
	m_pPrevSelection = 0;
}

void PopupMenu2::SetCancelCommand( int Id )
{
	m_CancelCommand = Id;
}

void PopupMenu2::OnCursorUp( void )
{
	SelectPrev();
}

void PopupMenu2::OnCursorDown( void )
{
	SelectNext();
}

void PopupMenu2::OnCursorLeft( void )
{
}

void PopupMenu2::OnCursorRight( void )
{
}

void PopupMenu2::OnButtonOK( void )
{
	if( m_pSelection )
		IssueCommand( m_pSelection->GetId()  );

	return;
}

void PopupMenu2::OnButtonCancel( void )
{
	if( m_CallBack )
		CallMenuHandler( m_CallBack, m_CancelCommand, m_CallBackParam );

	return;
}

void PopupMenu2::SetItemText( int Id, const char *pText )
{
	PopupItem2 *pItem = GetItem( Id );

	if( pItem )
		pItem->m_Text.SetText( pText );

	return;
}

void PopupMenu2::Printf( int Id, const char *fmt, ... )
{
	static char szBuffer[1024];

	va_list args;
	va_start( args, fmt );
	vsprintf( szBuffer, fmt, args );
	va_end( args );

	SetItemText( Id, szBuffer );
}

void PopupMenu2::SetMenuColor( float r, float g, float b, float a )
{
	m_MenuColor.r = r;
	m_MenuColor.g = g;
	m_MenuColor.b = b;
	m_MenuColor.a = a;
}

void PopupMenu2::SetHighlightColor( float r, float g, float b, float a )
{
	m_HighlightColor.r = r;
	m_HighlightColor.g = g;
	m_HighlightColor.b = b;
	m_HighlightColor.a = a;
}

void PopupMenu2::SetHighlightScale( float x, float y )
{
	m_HighlightScale.Set( x, y );
}

void PopupMenu2::SetMenuShadow( bool TrueOrFalse, u8 Intensity /*= 255*/, s8 x /*= 2*/, s8 y /*= 2*/ )
{
	m_Shadow.Enabled = TrueOrFalse;
	m_Shadow.Intensity = Intensity;
	m_Shadow.x = x;
	m_Shadow.y = y;
}

void PopupMenu2::SetMenuAppearance( float TextHeight, float TextSpacing, float WordWrap )
{
	m_Appearance.TextHeight = TextHeight;
	m_Appearance.TextSpacing = TextSpacing;
	m_Appearance.WordWrap = WordWrap;
}

void PopupMenu2::SetMenuAlignment( float Alignment )
{
	m_Appearance.TextAlignment = Alignment;
}

void PopupMenu2::SetMenuPageSize( int MinItems )
{
	m_Appearance.PageSize = MinItems;
}

void PopupMenu2::SetMessage( const char *pMessage )
{
	m_Message.SetText( pMessage );
}

void PopupMenu2::SetMessageColor( float r, float g, float b, float a )
{
	m_Message.SetColor( r, g, b, a );
}

void PopupMenu2::SetMessageAppearance( float TextHeight, float TextSpacing, float TextAlignment )
{
	m_Message.SetTextHeight( TextHeight );
	m_Message.SetTextSpacing( TextSpacing );
	m_Message.SetTextAlignment( TextAlignment );
}

void PopupMenu2::SetMessageShadow( bool Enabled, u8 Intensity /*= 255*/, s8 x /*= 2*/, s8 y /*= 2*/ )
{
	m_Message.ShadowSettings( Enabled, Intensity, x, y );
}

void PopupMenu2::SetMargins( float Left, float Right, float Top, float Bottom )
{
	m_Appearance.Margins.Left			= Left;
	m_Appearance.Margins.Right		= Right;
	m_Appearance.Margins.Top			= Top;
	m_Appearance.Margins.Bottom		= Bottom;
}

void PopupMenu2::SelectFirst( void )
{
	if( m_MenuRoot.Child() )
	{
		m_pPrevSelection = m_pSelection;

		m_pSelection = (PopupItem2 *) m_MenuRoot.Child();

		Synchronize();
	}

	return;
}

void PopupMenu2::SelectNext( void )
{
	m_pPrevSelection = m_pSelection;

	if( m_pSelection )
		m_pSelection = (PopupItem2 *) m_pSelection->Next();

	if( !m_pSelection )
		m_pSelection = (PopupItem2 *) m_MenuRoot.Child();

	Synchronize();
}

void PopupMenu2::SelectPrev( void )
{
	m_pPrevSelection = m_pSelection;

	if( m_pSelection )
		m_pSelection = (PopupItem2 *) m_pSelection->Prev();

	if( !m_pSelection )
	{
		m_pSelection = (PopupItem2 *) m_MenuRoot.Child();

		while( m_pSelection && m_pSelection->Next() )
			m_pSelection = (PopupItem2 *) m_pSelection->Next();
	}

	Synchronize();
}

void PopupMenu2::SelectLast( void )
{
	m_pPrevSelection = m_pSelection;

	if( !m_pSelection )
		m_pSelection = (PopupItem2 *) m_MenuRoot.Child();

	while( m_pSelection && m_pSelection->Next() )
		m_pSelection = (PopupItem2 *) m_pSelection->Next();

	Synchronize();
}

void PopupMenu2::SetMenuHandler( MENUHANDLER CallBack, PARAM Param )
{
	m_CallBack = CallBack;
	m_CallBackParam = Param;
}

void PopupMenu2::Select( int Id )
{
	m_pPrevSelection = m_pSelection;

	m_pSelection = (PopupItem2 *) m_MenuRoot.Child();

	while( m_pSelection && m_pSelection->GetId() != Id )
		m_pSelection = (PopupItem2 *) m_pSelection->Next();

	Synchronize();
}

int PopupMenu2::GetSelection( void )
{
	if( m_pSelection )
		return m_pSelection->m_Id;

	return 0;
}

void PopupMenu2::SetMenuIdent( int Ident )
{
	m_Ident = Ident;
}

int PopupMenu2::GetMenuIdent( void )
{
	return m_Ident;
}

int PopupMenu2::Command( void )
{
	int Command = m_Command;

	m_Command = IDM_INVALID;

	return Command;
}

void PopupMenu2::IssueCommand( int Command )
{
	m_Command = Command;

	if( m_CallBack )
		CallMenuHandler( m_CallBack, m_Command, m_CallBackParam );

	return;
}

PopupItem2 *PopupMenu2::GetItem( int ItemId )
{
	PopupItem2 *p = (PopupItem2 *) m_MenuRoot.Child();

	while( p )
	{
		if( p->GetId() == ItemId )
			return p;

		p = (PopupItem2 *) p->Next();
	}

	return 0;
}

void PopupMenu2::SetNumberColumns( int numberColumns )
{
	m_NumberOfColumns = numberColumns;
}

void PopupMenu2::SetRowSpacer( float Spacer )
{
	m_RowSpacer = Spacer;
}

void PopupMenu2::SetPicture( int ItemId, t_Handle TextureSet, int Texture )
{
	PopupItem2 *p = GetItem( ItemId );

	if( p )
		p->m_Sprite.Create( TextureSet, Texture );

	return;
}

void PopupMenu2::SetPictureScale( int ItemId, float x, float y )
{
	PopupItem2 *p = GetItem( ItemId );

	if( p )
		p->m_Sprite.SetScale( x, y );

	return;
}

void PopupMenu2::SetPictureOffset( int ItemId, float x, float y )
{
	PopupItem2 *p = GetItem( ItemId );

	if( p )
		p->m_Sprite.SetOffset( x, y );

	return;
}

void PopupMenu2::EnableCursor( bool TrueOrFalse )
{
	m_CursorEnabled = TrueOrFalse;

	if( m_CursorEnabled )
	{
		m_Highlight.Create( Font::m_FontList.m_texture.handle, TS_NO_TEXTURE );
		m_Highlight.SetOrigin( 0.5f, 0.5f );
		m_Highlight.SetSize( 16.0f, 16.0f );
		m_Highlight.SetPosition( 0.0f, 0.0f );
		m_Highlight.SetPriority( -140 );
		m_Highlight.SetVertexPosition( 1, 0.0f, m_Highlight.m_state.size.y * 0.5f );
		m_Highlight.SetVertexPosition( 2, 0.0f, m_Highlight.m_state.size.y *-0.5f );
/*
		m_Highlight.InitAnim();
		m_Highlight.m_anim->offset.x().Clear();
		m_Highlight.m_anim->offset.x().SetFrame( 0.0f, -10.0f );
		m_Highlight.m_anim->offset.x().SetFrame( 0.5f, 0.0f );
		m_Highlight.m_anim->offset.x().SetFrame( 1.0f, -10.0f );
		m_Highlight.m_anim->offset.x().TimeCurve( true );
		m_Highlight.m_anim->offset.x().Play( true );
*/
		m_Highlight.SetVertexColor( 0, 0.306f, 0.486f, 0.627f, 0.8f );
		m_Highlight.SetVertexColor( 1, 0.306f, 0.486f, 0.627f, 0.8f );
		m_Highlight.SetVertexColor( 2, 0.129f, 0.231f, 0.424f, 0.8f );
		m_Highlight.SetVertexColor( 3, 0.129f, 0.231f, 0.424f, 0.8f );
	}
	else
	{
		m_Highlight.Release();
	}

	return;
}

void PopupMenu2::EnableCenterHighlight( bool TrueOrFalse )
{
	m_CenterHighlight = TrueOrFalse;
}

void PopupMenu2::SetFirstRowVerticalOffset( float yoffset ) {
	f_InitalYOffset = yoffset;
}

void PopupMenu2::ComputeSprite( void )
{
	PopupWindow::ComputeSprite();

	if( !m_pSelection )
		SelectFirst();

	float ch = m_Window.m_Caption.ComputeTextHeight();
	float ml = m_Appearance.Margins.Left;
	float mr = m_Appearance.Margins.Right;
	float mt = m_Appearance.Margins.Top;
	float mb = m_Appearance.Margins.Bottom;

	if( FLOATTOINT(ml) == 0 ) ml = m_Window.GetBorderWidth();
	if( FLOATTOINT(mr) == 0 ) mr = m_Window.GetBorderWidth();

	// A special case for top and bottom margin
	if( FLOATTOINT(mt) == 0 )
		mt = m_Window.GetBorderHeight();

	if( FLOATTOINT(mt) < FLOATTOINT(ch) )
		mt = ch;

	if( FLOATTOINT(mb) == 0 )
	{
		mb = m_Window.GetBorderHeight();

		if( FLOATTOINT(mb) < FLOATTOINT(m_Buttons.m_state.size.y) )
			mb = m_Buttons.m_state.size.y;
	}

	float w = m_state.size.x - ml - mr;
	float h = m_state.size.y - mt - mb;

	m_Message.SetOrigin( 0.5f, 0.5f );
	m_Message.SetWordWrap( true );
	m_Message.SetSize( w, h );

	float ih = m_Appearance.TextHeight;
	float th = m_Message.ComputeTextHeight();

	int c = m_MenuRoot.GetNumChildren();

	float x = ml;
	float mh = ih * c;

	m_ScrollBar.m_state.familycolor.a = 0;

	if( FLOATTOINT(th) > 0 )
	{
		if( th > h )
			th = h;
		
		if( th < h - mh )
			th = h - mh;
	}

	mh = h - th;

	if( c > 0 )
	{
		int PageSize = m_Appearance.PageSize;

		if( c < PageSize )
			PageSize = c;

		if( mh < ih * PageSize )
			mh = ih * PageSize, th = h - mh;
	}
	
//	m_Message.SetSize( 0.0f, th );	
	m_Message.SetOrigin( m_Message.GetTextAlignment(), 0.5f );
	m_Message.SetPosition( m_state.size.x * 0.5f, mt + th * 0.5f );

	if( c > 0 )
	{
		if( FLOATTOINT(th) == 0 )
		{
			mh = ih * c;
			
			if( mh > h )
				mh = h;	
		}

		int e = (int) (mh / ih);

		mh = e * ih;

		int sp = m_pSelection->GetTreePosition();
		int st = m_ScrollTop;
		int sb = m_ScrollBottom;

		if( sp < st )
			st = sp, sb = st + e - 1;
		else if( sp > sb )
			sb = sp, st = sb - e + 1;

		if( m_CenterHighlight )
			st = sp - e / 2;
		else if( st < 0 )
			st = 0;

		sb = st + e - 1;

		m_ScrollTop = st;
		m_ScrollBottom = sb;

		float y = m_state.size.y - mb - mh + ih * 0.5f;

		if( FLOATTOINT(th) == 0 )
			y = mt + h * 0.5f - mh * 0.5f + ih * 0.5f;

		if( st != 0 || sb != c - 1 )
		{
		//	m_ScrollBar.SetSize( 16.0f, mh );
		//	m_ScrollBar.SetPosition( x - m_ScrollBar.m_state.size.x() * 1.5f, y - ih * 0.5f );
			m_ScrollBar.m_state.position.y = y - ih * 0.5f;
			m_ScrollBar.m_state.size.y = mh;
			m_ScrollBar.SetRange( 0, c - 1 );
			m_ScrollBar.SetThumbPos( sp );
			m_ScrollBar.SetGroupColor( 1.0f, 1.0f, 1.0f, 0.4f );
			m_ScrollBar.SetPageSize( sb - st );
		}

		PopupItem2 *p = (PopupItem2 *) m_MenuRoot.Child();

		if( st < 0 )
			y = y - ih * st;

		y += f_InitalYOffset;
		//  TGS - just in case someone is a putz and wants more columns than items
		if ( c < m_NumberOfColumns )
			m_NumberOfColumns = c;

		//  TGS - must now distribute the items within the columns as evenly as possible
		//  This is how I'm going to do it, the extraNonEvenDistributed will keep the number of extra
		//  I'm going to distribute one per column from left to right until we run out
		//  personally, only maximum 2 columns look good
		int itemsPerColumn = c / m_NumberOfColumns;
		int extraNonEvenDistributed = c - itemsPerColumn * m_NumberOfColumns;
		int rowAdjust = 0;

		int columnItemFill = 0;
		int rowItemFill = -1;

		float topYPosition = y;

		float xSpacing = w / ( ( float )m_NumberOfColumns ) + 20.0f;

		if ( m_NumberOfColumns > 1 )
			x = 0;

		for( int i = 0; i < c; i++ )
		{
			if( i < st || i > sb )
			{
				p->m_Text.SetGroupColor( 1.0f, 1.0f, 1.0f, 0.0f );
				p->m_Sprite.SetGroupColor( 1.0f, 1.0f, 1.0f, 0.0f );
			}
			else
			{
				if ( m_NumberOfColumns > 1 ) {
					m_Appearance.TextAlignment = 0.0f;
					rowItemFill++;
					//  calculate if we need to change the column
					if ( extraNonEvenDistributed > 0 )
						rowAdjust = 1;
					else
						rowAdjust = 0;
					if ( rowItemFill >= itemsPerColumn + rowAdjust ) {
						rowItemFill = 0;
						columnItemFill++;
						extraNonEvenDistributed--;
						y = topYPosition;
						x+= xSpacing;
					}
				}

				p->m_Text.SetGroupColor( 1.0f, 1.0f, 1.0f, 1.0f );
				p->m_Sprite.SetGroupColor( 1.0f, 1.0f, 1.0f, 1.0f );

				p->m_Text.SetOrigin( m_Appearance.TextAlignment, 0.5f );
				p->m_Text.SetTextHeight( ih );
				p->m_Text.SetTextSpacing( m_Appearance.TextSpacing );
				p->m_Text.ShadowSettings( m_Shadow.Enabled, m_Shadow.Intensity, m_Shadow.x, m_Shadow.y );
				p->m_Text.SetColor( m_MenuColor.r, m_MenuColor.g, m_MenuColor.b, m_MenuColor.a );
				p->m_Text.SetWordWrap( true );
				p->m_Text.SetSize( w, 0.0f );
				p->m_Text.SetPosition( x + w * m_Appearance.TextAlignment, y );

				if( p->m_Sprite.m_state.textureset.handle != INVALID_HANDLE )
				{
					TextOutput to;

					p->m_Text.ComputeTextOutput( &to );

					p->m_Sprite.SetOrigin( 0.5f, 0.5f );
					p->m_Sprite.SetSize( to.Height, to.Height );
					p->m_Sprite.ShadowSettings( true );
					p->m_Sprite.SetPosition( to.x1 - to.Height * 0.5f, to.y1 + to.Height * 0.5f );
					p->m_Sprite.Show();
				}

				//  TGS - Highlight has not been completed for multiple columns
				//  There is some offset factor added when a picture is associated to the text line
				if( p == m_pSelection )
				{
					TextOutput to;

					p->m_Text.ComputeTextOutput( &to );

					if( FLOATTOINT(m_HighlightColor.a) != 0 )
						p->m_Text.SetColor( m_HighlightColor.r, m_HighlightColor.g, m_HighlightColor.b, m_HighlightColor.a );

					p->m_Text.AddChild( &m_Highlight );

					if( m_CursorEnabled )
					{
						float w2 = w - 12.0f;	// TGS - mintjulip - the width is bigger than the interior width, have to figure out why
						if ( m_NumberOfColumns > 1 ) {
							m_Highlight.SetPosition( x, 0.0f );
							m_Highlight.SetSize( xSpacing, to.Height * 0.55f );
						} else {
							m_Highlight.SetPosition( x, 0.0f );
							m_Highlight.SetSize( w2, to.Height * 0.55f );
						}
						m_Highlight.SetVertexPosition( 0, -( x + 12.0f + ml + mr)/ 2.0f, m_Highlight.m_state.size.y *-0.28f );
						m_Highlight.SetVertexPosition( 1, 0.0f, m_Highlight.m_state.size.y *-0.28f );
						m_Highlight.SetVertexPosition( 2, 0.0f, m_Highlight.m_state.size.y *0.28f );
						m_Highlight.SetVertexPosition( 3, -( x + 12.0f + ml + mr)/ 2.0f, m_Highlight.m_state.size.y *0.28f );

						if( p->m_Sprite.m_state.textureset.handle != INVALID_HANDLE )
							m_Highlight.Move( -p->m_Sprite.m_state.size.x, 0.0f );
					}
				}

				y = y + ih + m_RowSpacer;
			}
			p = (PopupItem2 *) p->Next();
		}
	}

	m_Border.HideInterior();
	m_Buttons.SetSize( w, mb );
	m_Buttons.SetOrigin( 0.5f, 0.5f );
	m_Buttons.SetPosition( m_state.size.x * 0.5f, m_state.size.y - mb * 0.5f );

	return;
}

void PopupMenu2::Synchronize( void )
{
	if( m_pPrevSelection != m_pSelection )
	{
		if( m_pPrevSelection )
			OnUnselection(), OnSelectionChange();

		if( m_pSelection )
			OnSelection();
	}

	return;
}

void PopupMenu2::OnSelection( void )
{
	PopupItem2 *p = m_pSelection;

	if( p )
	{
		p->m_Text.m_anim->scale.x.SetFrame( 0.0f, p->m_Text.m_state.scale.x );
		p->m_Text.m_anim->scale.x.SetFrame( 0.0f, m_HighlightScale.x() );
		p->m_Text.m_anim->scale.x.Accelerate( true );
		p->m_Text.m_anim->scale.x.Play( false );

		p->m_Text.m_anim->scale.y.SetFrame( 0.0f, p->m_Text.m_state.scale.y );
		p->m_Text.m_anim->scale.y.SetFrame( 0.0f, m_HighlightScale.y() );
		p->m_Text.m_anim->scale.y.Accelerate( true );
		p->m_Text.m_anim->scale.y.Play( false );
	}

	return;
}

void PopupMenu2::OnUnselection( void )
{
	PopupItem2 *p = m_pPrevSelection;

	if( p )
	{
		p->m_Text.m_anim->scale.x.SetFrame( 0.0f, p->m_Text.m_state.scale.x );
		p->m_Text.m_anim->scale.x.SetFrame( 0.0f, 1.0f );
		p->m_Text.m_anim->scale.x.Accelerate( true );
		p->m_Text.m_anim->scale.x.Play( false );

		p->m_Text.m_anim->scale.y.SetFrame( 0.0f, p->m_Text.m_state.scale.y );
		p->m_Text.m_anim->scale.y.SetFrame( 0.0f, 1.0f );
		p->m_Text.m_anim->scale.y.Accelerate( true );
		p->m_Text.m_anim->scale.y.Play( false );
	}

	return;
}

void PopupMenu2::OnSelectionChange( void )
{
	OnSelectionSound();
}

void PopupMenu2::OnSelectionSound( void )
{
}

PopupMessage::PopupMessage( void )
{
	m_Window.AddChild( &m_Message );
	m_Window.AddChild( &m_Button );
}

void PopupMessage::Reset( void )
{
	PopupWindow::Reset();

	m_Message.Reset();

	//===========================
	// default size
	//===========================

	SetSize( 320.0f, 256.0f );
	
	//===========================
	// default origin
	//===========================

	SetOrigin( 0.5f, 0.5f );
	
	//===========================
	// default screen position
	//===========================

	ScreenPosition( 0.5f, 0.5f );

	//===========================
	// default window color
	//===========================
	
	SetColor( 0.0f, 0.0f, 0.7f, 0.4f );
	
	//===========================
	// default border color
	//===========================

	m_Border.SetColor( 1.0f, 1.0f, 0.6f, 1.0f );

	//===========================
	// default caption settings
	//===========================

	SetCaptionColor( 1.0f, 1.0f, 0.6f, 1.0f );
	SetCaptionAppearance( 75.0f, 0.0f );
	SetCaptionShadow( true );

	//===========================
	// default message settings
	//===========================

	SetMessageColor( 1.0f, 1.0f, 0.6f, 1.0f );
	SetMessageAppearance( 40.0f, 0.0f );
	SetShadowAppearance( false );
}

void PopupMessage::SetMessage( const char *pMessage )
{
	m_Message.SetText( pMessage );
}

void PopupMessage::SetMessageAppearance( float TextHeight, float TextSpacing )
{
	m_Message.SetTextHeight( TextHeight );
	m_Message.SetTextSpacing( TextSpacing );
}

void PopupMessage::SetMessageColor( float r, float g, float b, float a )
{
	m_Message.SetColor( r, g, b, a );
}

void PopupMessage::SetShadowAppearance( bool Enabled, u8 Intensity /* = 255*/, s8 x /* = 2*/, s8 y /* = 2*/ )
{
	m_Message.ShadowSettings( Enabled, Intensity, x, y );
	m_Button.m_Picture.ShadowSettings( Enabled, Intensity, x, y );
	m_Button.m_Caption.ShadowSettings( Enabled, Intensity, x, y );
}

void PopupMessage::CreateOKButton( const char *pText, t_Handle TextureSet, int Texture )
{
	m_Button.Create( pText, TextureSet, Texture, Texture );
	m_Button.SetOrigin( 0.5f, 0.5f );
}

void PopupMessage::SetButtonAppearance( float TextHeight, float TextSpacing )
{
	m_Button.m_Caption.SetTextHeight( TextHeight );
	m_Button.m_Caption.SetTextSpacing( TextSpacing );
}

void PopupMessage::ComputeSprite( void )
{
	PopupWindow::ComputeSprite();

	float BorderWidth = m_Window.GetBorderWidth();
	float BorderHeight = m_Window.GetBorderHeight();
	float TextHeight = m_Window.m_Caption.GetTextHeight();

	float MessageWidth = m_state.size.x - BorderWidth * 2.0f;
	float MessageHeight = m_state.size.y - BorderHeight * 2.0f - TextHeight * 0.8f;
	float x1 = BorderWidth;
	float y1 = TextHeight;
	float x2 = x1 + MessageWidth;
	float y2 = y1 + MessageHeight;

	m_Message.SetOrigin( 0.5f, 0.5f );
	m_Message.SetPosition( x1 + MessageWidth * 0.5f, y1 + MessageHeight * 0.5f );
	m_Message.SetSize( MessageWidth, MessageHeight );
	m_Message.SetWordWrap( true );
	m_Message.SetTextAlignment( 0.5f );

	m_Button.SetOrigin( 0.5f, 1.0f );
	m_Button.SetPosition( x1 + MessageWidth * 0.25f, m_state.size.y );
}

// POPUPMESSAGE 2
PopupMessage2::PopupMessage2( void )
{
	int i;
	for ( i = 0; i < MAX_NUMBER_SHORT_MESSAGES; i++ ) {
		m_Window.AddChild( &m_Message[ i ] );
	}
	for ( i = 0; i < MAX_NUMBER_OF_BUTTONS; i++ ) {
		m_Window.AddChild( &m_Button[ i ] );
	}
}

void PopupMessage2::Reset( void )
{
	int i;
	PopupWindow::Reset();

	for ( i = 0; i < MAX_NUMBER_SHORT_MESSAGES; i++ ) {
		m_Message[ i ].Reset();
	}

	//===========================
	// default size
	//===========================

	SetSize( 320.0f, 256.0f );
	
	//===========================
	// default origin
	//===========================

	SetOrigin( 0.5f, 0.5f );
	
	//===========================
	// default screen position
	//===========================

	ScreenPosition( 0.5f, 0.5f );

	//===========================
	// default window color
	//===========================
	
	SetColor( 0.0f, 0.0f, 0.7f, 0.4f );
	
	//===========================
	// default border color
	//===========================

	m_Border.SetColor( 1.0f, 1.0f, 0.6f, 1.0f );

	//===========================
	// default caption settings
	//===========================

	SetCaptionColor( 1.0f, 1.0f, 0.6f, 1.0f );
	SetCaptionAppearance( 75.0f, 0.0f );
	SetCaptionShadow( true );

	//===========================
	// default message settings
	//===========================

	for ( i = 0; i < MAX_NUMBER_SHORT_MESSAGES; i++ ) {
		SetMessageColor( i, 1.0f, 1.0f, 0.6f, 1.0f );
		SetMessageAppearance( i, 40.0f, 0.0f );
		SetShadowAppearance( i, false );
	}

	//===========================
	// default button settings
	//===========================

	for ( i = 0; i < MAX_NUMBER_OF_BUTTONS; i++ ) {
		SetButtonPosition( i, 0.0f, 0.0f );
		SetButtonOrigin( i, 0.5f, 1.0f );
	}
}

void PopupMessage2::SetMessage( int MessageNumber, const char *pMessage )
{
	m_Message[ MessageNumber ].SetText( pMessage );
}

void PopupMessage2::SetMessageAppearance( int MessageNumber, float TextHeight, float TextSpacing )
{
	m_Message[ MessageNumber ].SetTextHeight( TextHeight );
	m_Message[ MessageNumber ].SetTextSpacing( TextSpacing );
}

void PopupMessage2::SetMessageColor( int MessageNumber, float r, float g, float b, float a )
{
	m_Message[ MessageNumber ].SetColor( r, g, b, a );
}

void PopupMessage2::SetShadowAppearance( int MessageNumber, bool Enabled, u8 Intensity /*= 255*/, s8 x /*= 2*/, s8 y /*= 2*/ )
{
	m_Message[ MessageNumber ].ShadowSettings( Enabled, Intensity, x, y );
}

void PopupMessage2::SetButtonShadowAppearance( int ButtonNumber, bool Enabled, u8 Intensity /*= 255*/, s8 x /*= 2*/, s8 y /*= 2*/ )
{
	m_Button[ ButtonNumber ].m_Picture.ShadowSettings( Enabled, Intensity, x, y );
	m_Button[ ButtonNumber ].m_Caption.ShadowSettings( Enabled, Intensity, x, y );
}

void PopupMessage2::CreateButton( int ButtonNumber, const char *pText, t_Handle TextureSet, int Texture )
{
	m_Button[ ButtonNumber ].Create( pText, TextureSet, Texture, Texture );
	m_Button[ ButtonNumber ].SetOrigin( 0.5f, 0.5f );
}

void PopupMessage2::SetButtonAppearance( float TextHeight, float TextSpacing )
{
	int i;
	for ( i = 0; i < MAX_NUMBER_OF_BUTTONS; i++ ) {
		m_Button[ i ].m_Caption.SetTextHeight( TextHeight );
		m_Button[ i ].m_Caption.SetTextSpacing( TextSpacing );
	}
}

void PopupMessage2::SetButtonPosition( int ButtonNumber, float x, float y )
{
	m_Button[ ButtonNumber ].SetPosition( x, y );
}

void PopupMessage2::SetButtonOrigin( int ButtonNumber, float x, float y )
{
	m_Button[ ButtonNumber ].SetOrigin( x, y );
}

void PopupMessage2::ComputeSprite( void )
{
	int i;
	PopupWindow::ComputeSprite();

	float BorderWidth = m_Window.GetBorderWidth();
	float BorderHeight = m_Window.GetBorderHeight();
	float TextHeight = m_Window.m_Caption.GetTextHeight();

	float MessageWidth = m_state.size.x - BorderWidth * 2.0f;
	float MessageHeight = m_state.size.y - BorderHeight * 2.0f - TextHeight * 0.8f;
	float x1 = BorderWidth;
	float y1 = TextHeight;
	float x2 = x1 + MessageWidth;
	float y2 = y1 + MessageHeight;

	for ( i = 0; i < MAX_NUMBER_SHORT_MESSAGES; i++ ) {
		m_Message[ i ].SetOrigin( 0.5f, 0.5f );
		m_Message[ i ].SetPosition( x1 + MessageWidth * 0.5f, y1 + MessageHeight * 0.5f );
		m_Message[ i ].SetSize( MessageWidth, MessageHeight );
		m_Message[ i ].SetWordWrap( true );
		m_Message[ i ].SetTextAlignment( 0.5f );
	}
}
