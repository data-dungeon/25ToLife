#include "TTLPCH.h"
#include "GameHelper/Translator.h"
#include "frontend/frontend.h"
#include "frontend/savegame.h"
#include "frontend/screens/screen_TagEditor.h"
#include "data/screens/TagEditor.h"
#include "main/Lobby.h"

enum
{
	COMMAND_TAG_CANCELYES = 0x100,
	COMMAND_TAG_CANCELNO,
	COMMAND_TAG_ERROROK,
	COMMAND_TAG_FILTEROK,
};

enum
{
	DIALOGID_TAG_CANCEL = 0x100,
	DIALOGID_TAG_ERROR,
	DIALOGID_TAG_FILTER,
};

//=============================================================================
// ScreenTagEditor::ScreenTagEditor
//=============================================================================

ScreenTagEditor::ScreenTagEditor( void )
{
	m_pRedBar = 0;
	m_pGreenBar = 0;
	m_pBlueBar = 0;
	m_ColorIndex = 0;
}

//=============================================================================
// ScreenTagEditor::~ScreenTagEditor
//=============================================================================

ScreenTagEditor::~ScreenTagEditor( void )
{
}

//=============================================================================
// ScreenTagEditor::ShowMainMenu
//=============================================================================

void ScreenTagEditor::ShowMainMenu( bool Show )
{
	Sprite* pSprite = GetSprite( ID_TAGEDITOR_MAINMENU );

	if( Show )
	{
		m_MenuMode = MENUMODE_MAIN;

		SetInitialButton( ID_TAGEDITOR_TEXT );
		SetNextButton( ID_TAGEDITOR_COLORS );
		SetNextButton( ID_TAGEDITOR_DONE );
		SetCursor( ID_TAGEDITOR_CURSOR );
	}

	if( pSprite )
	{
		if( Show )
			pSprite->m_state.familycolor.a = 255;
		else
			pSprite->m_state.familycolor.a = 0;
	}

	return;
}

//=============================================================================
// ScreenTagEditor::ShowColorsMenu
//=============================================================================

void ScreenTagEditor::ShowColorsMenu( bool Show )
{
	Sprite* pSprite = GetSprite( ID_TAGEDITOR_COLORMENU );

	if( Show )
	{
		m_MenuMode = MENUMODE_COLORS;
		SetInitialButton( ID_TAGEDITOR_COLOR1 );
		SetNextButton( ID_TAGEDITOR_COLOR2 );
		SetNextButton( ID_TAGEDITOR_COLOR3 );
		SetCursor( ID_TAGEDITOR_COLORCURSOR );
	}

	if( pSprite )
	{
		if( Show )
			pSprite->m_state.familycolor.a = 255;
		else
			pSprite->m_state.familycolor.a = 0;
	}

	return;
}

//=============================================================================
// ScreenTagEditor::ShowColorPicker
//=============================================================================

void ScreenTagEditor::ShowColorPicker( bool Show )
{
	Sprite* pSprite = GetSprite( ID_TAGEDITOR_COLORPICKER );

	if( Show )
	{
		m_MenuMode = MENUMODE_PICKER;

		SetInitialButton( ID_TAGEDITOR_REDSLIDER );
		SetNextButton( ID_TAGEDITOR_GREENSLIDER );
		SetNextButton( ID_TAGEDITOR_BLUESLIDER );
		SetCursor( ID_TAGEDITOR_PICKERCURSOR );
	}

	if( pSprite )
	{
		if( Show )
			pSprite->m_state.familycolor.a = 255;
		else
			pSprite->m_state.familycolor.a = 0;
	}

	GuiProgressBar* pRedBar   = (GuiProgressBar*) GetInterface( ID_TAGEDITOR_REDBAR,   GuiProgressBar::ClassnameStatic() );
	GuiProgressBar* pGreenBar = (GuiProgressBar*) GetInterface( ID_TAGEDITOR_GREENBAR, GuiProgressBar::ClassnameStatic() );
	GuiProgressBar* pBlueBar  = (GuiProgressBar*) GetInterface( ID_TAGEDITOR_BLUEBAR,  GuiProgressBar::ClassnameStatic() );

	if( pRedBar )
		pRedBar->SetProgress( m_TagRed[m_ColorIndex] );

	if( pGreenBar )
		pGreenBar->SetProgress( m_TagGreen[m_ColorIndex] );

	if( pBlueBar )
		pBlueBar->SetProgress( m_TagBlue[m_ColorIndex] );

	return;
}

//=============================================================================
// ScreenTagEditor::ShowKeyboard
//=============================================================================

void ScreenTagEditor::ShowKeyboard( bool Show )
{
	if( Show )
	{
		ScreenKeyboard* pKeyboard = GetKeyboard();

		if( pKeyboard )
		{
			pKeyboard->SetMaxString( g_GlobalProfile.GetMaxTag() );
			pKeyboard->SetString( m_TagString.get() );
			pKeyboard->RestrictKeys( true, 0, 255 );
			pKeyboard->RestrictChars( false, "0123456789AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz" );
			pKeyboard->SetDefaultKey( ScreenKeyboard::ENTER_KEY );
		}

		g_FrontEnd.ShowKeyboard();
	}
	else
	{
		ScreenManager* pScreenManager = GetScreenManager();

		if( pScreenManager )
			pScreenManager->ClosePopup( "SCREEN_ID_KEYBOARD" );
	}

	return;
}

//=============================================================================
// ScreenTagEditor::HasChanged
//=============================================================================

bool ScreenTagEditor::HasChanged( void )
{
	xstring TagString;
	float r, g, b, a;
	int i, c;
	bool Result;
	
	Result = false;
	
	c = g_GlobalProfile.GetMaxTagColors();

	for( i = 0; i < c; i++ )
	{
		g_GlobalProfile.GetTagColor( i, r, g, b, a );

		if( m_TagRed[i] != r || m_TagGreen[i] != g || m_TagBlue[i] != b )
			Result = true;
	}

	if( !m_TagString.compare( g_GlobalProfile.GetTagString() ) )
		Result = true;

	return Result;
}

//=============================================================================
// ScreenTagEditor::CancelChanges
//=============================================================================

void ScreenTagEditor::CancelChanges( void )
{
	if( !HasChanged() )
		GotoPrevScreen();
	else
	{
		ScreenPopupDialog* pDialog = GetPopupDialog();

		if( pDialog )
		{
			pDialog->Clear();
			pDialog->SetTitle( "Changes will be lost!" );
			pDialog->SetMessage( "Do you want to cancel your changes and return to the previous menu?" );
			pDialog->SetCancelCommand( COMMAND_TAG_CANCELNO );
			pDialog->AddMenuItem( "Yes", COMMAND_TAG_CANCELYES, 0 );
			pDialog->AddMenuItem( "No", COMMAND_TAG_CANCELNO, 0 );
			pDialog->SelectMenuItem( COMMAND_TAG_CANCELNO );
			ShowPopupDialog();
		}
	}

	return;
}

void ScreenTagEditor::UpdateTagString( void )
{
	const char* pTagString = m_TagString.get();

	Text* pTagString1 = GetTextSprite( ID_TAGEDITOR_STRING1 );
	Text* pTagString2 = GetTextSprite( ID_TAGEDITOR_STRING2 );
	Text* pTagString3 = GetTextSprite( ID_TAGEDITOR_STRING3 );

	if( pTagString1 )
	{
		pTagString1->SetText( pTagString );
		pTagString1->SetBaseColor( m_TagRed[0], m_TagGreen[0], m_TagBlue[0], pTagString1->GetAlpha() );
	}

	if( pTagString2 )
	{
		pTagString2->SetText( pTagString );
		pTagString2->SetBaseColor( m_TagRed[1], m_TagGreen[1], m_TagBlue[1], pTagString2->GetAlpha() );
	}

	if( pTagString3 )
	{
		pTagString3->SetText( pTagString );
		pTagString3->SetBaseColor( m_TagRed[2], m_TagGreen[2], m_TagBlue[2], pTagString3->GetAlpha() );
	}

	return;
}

//=============================================================================
// ScreenTagEditor::OnInitialize
//=============================================================================

void ScreenTagEditor::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	UseHighlighting( true );

	SetHelpRect( ID_TAGEDITOR_HELPRECT );
	SetInitialButton( ID_TAGEDITOR_TEXT );
	SetNextButton( ID_TAGEDITOR_COLORS );
	SetNextButton( ID_TAGEDITOR_DONE );
	SetCursor( ID_TAGEDITOR_CURSOR );

	m_pRedBar   = CreateProgressBar( ID_TAGEDITOR_REDBAR );
	m_pGreenBar = CreateProgressBar( ID_TAGEDITOR_GREENBAR );
	m_pBlueBar  = CreateProgressBar( ID_TAGEDITOR_BLUEBAR );

	if( m_pRedBar )
	{
		m_pRedBar->SetPosition( 1.0f );
		m_pRedBar->SetRange( 0.0f, 1.0f );
		m_pRedBar->SetSteps( 20 );
	}

	if( m_pGreenBar )
	{
		m_pGreenBar->SetPosition( 1.0f );
		m_pGreenBar->SetRange( 0.0f, 1.0f );
		m_pGreenBar->SetSteps( 20 );
	}

	if( m_pBlueBar )
	{
		m_pBlueBar->SetPosition( 1.0f );
		m_pBlueBar->SetRange( 0.0f, 1.0f );
		m_pBlueBar->SetSteps( 20 );
	}

	ShowMainMenu( true );
	ShowColorsMenu( false );
	ShowColorPicker( false );

	Text* pTagString1 = GetTextSprite( ID_TAGEDITOR_STRING1 );
	Text* pTagString2 = GetTextSprite( ID_TAGEDITOR_STRING2 );
	Text* pTagString3 = GetTextSprite( ID_TAGEDITOR_STRING3 );

	if( pTagString1 )
		pTagString1->SetTextEffect( new SortTagEffect );

	if( pTagString2 )
		pTagString2->SetTextEffect( new SortTagEffect );

	if( pTagString3 )
		pTagString3->SetTextEffect( new SortTagEffect );

	m_ColorIndex = 0;

	// Hide the PS2 help bar
#ifdef DIRECTX_PC
	GetSprite(ID_TAGEDITOR_HELPRECT)->HideAll();
#endif

	//==========================================================================
	// This message handler will respond to text that was filtered because of
	// offensive content.
	//==========================================================================

	m_TextFilteredHandler.Initialize( "TextFiltered", MESSAGE_HANDLE_IF_MASTER, m_MessageOwner, this );
}

//=============================================================================
// ScreenTagEditor::OnScreenIntro
//=============================================================================

void ScreenTagEditor::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	float Alpha;
	g_GlobalProfile.GetTagColor( 0, m_TagRed[0], m_TagGreen[0], m_TagBlue[0], Alpha );
	g_GlobalProfile.GetTagColor( 1, m_TagRed[1], m_TagGreen[1], m_TagBlue[1], Alpha );
	g_GlobalProfile.GetTagColor( 2, m_TagRed[2], m_TagGreen[2], m_TagBlue[2], Alpha );
	m_TagString.set( g_GlobalProfile.GetTagString() );

	ShowMainMenu( true );
	ShowColorsMenu( false );

	return;
}

//=============================================================================
// ScreenTagEditor::OnScreenUpdate
//=============================================================================

void ScreenTagEditor::OnScreenUpdate( void )
{
//#ifdef DIRECTX_PC
//	if (g_MenuCursor.IsActive()) {
//		float x1, y1, x2, y2;	
//		// check for back navigation
//		if (g_MenuCursor.CursorButtonPressed(1))
//		{
//			g_MenuCursor.ForceButtonUp(1);
//			OnButtonCancel();
//		}
//		float x = g_MenuCursor.GetX();
//		float y = g_MenuCursor.GetY();
//		// functionality for menu sliders
//		if (GetButton(ID_TAGEDITOR_REDSLIDER)->Enabled())
//		{
//			GuiProgressBar* sliders[3] = {m_pRedBar,m_pGreenBar,m_pBlueBar};
//			for (int c=0; c < 3; ++c)
//			{
//				switch (c) {
//					case 0:
//						SelectButton(ID_TAGEDITOR_REDSLIDER);
//						break;
//					case 1:
//						SelectButton(ID_TAGEDITOR_GREENSLIDER);
//						break;
//					case 2:
//						SelectButton(ID_TAGEDITOR_BLUESLIDER);
//						break;
//						}
//				if (sliders[c])
//				{
//					//sliders[c]->m_p->GetFamilyRect(x1,y1,x2,y2);
//					//if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
//					//{
//					//	// see which side of the slider we're on, and move the
//					//	// slider accordingly
//					//	sliders[c]->m_pBarThumb->GetFamilyRect(x1,y1,x2,y2);
//					//	float sliderXPos = x1 + ((x2-x1) * 0.5f);
//					//	if ( g_MenuCursor.CursorButtonPressed(0) )
//					//	{
//					//		if ( x < sliderXPos ) OnCursorMovement( GetCurrentButtonId(),-1 );
//					//		else if ( x > sliderXPos ) OnCursorMovement( GetCurrentButtonId(),1 );
//					//	}
//					//	break;
//					//}
//				}
//			}
//		}
//		// functionality for button based menus
//		Button* button = GetDefaultButton();
//		if (!button)
//		{
//			if (g_MenuCursor.CursorButtonPressed(0))
//				OnButtonStart();
//		}
//		// search through the buttons to see if we're over one
//		// if so, check to see if we're clicking the mouse 
//		while(button) 
//		{
//			button->SpriteHost()->GetFamilyRect(x1, y1, x2, y2);
//			if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
//			{
//				SelectButton(button);
//				if ( g_MenuCursor.CursorButtonPressed(0) )
//				{
//						g_MenuCursor.ForceButtonUp(0);
//						OnButtonOK(); 
//				}
//				break;
//			}
//			button = button->GetNextButton();
//		}
//	}
//#else
		ScreenBaseClass::OnScreenUpdate();
//#endif

	if( m_MenuMode == MENUMODE_PICKER )
	{
		if( m_pRedBar )
			m_TagRed[m_ColorIndex] = m_pRedBar->GetProgress();

		if( m_pGreenBar )
			m_TagGreen[m_ColorIndex] = m_pGreenBar->GetProgress();

		if( m_pBlueBar )
			m_TagBlue[m_ColorIndex] = m_pBlueBar->GetProgress();

		GuiProgressBar* pBars[3] = {m_pRedBar, m_pGreenBar, m_pBlueBar};
		Sprite* pSprite;
		Text* pValue;

		for( int i = 0; i < 3; i++ )
		{
			if( pBars[i] )
			{
				pSprite = pBars[i]->SpriteHost();

				if( pSprite )
				{
					pValue = GetTextSprite( ID_TAGEDITOR_VALUE, pSprite );

					if( pValue )
						pValue->Printf( "%d", int(pBars[i]->GetProgress() * 100.0f + 0.5f) );
				}
			}
		}
	}

	const int ColorRects[] =
	{
		ID_TAGEDITOR_COLOR1RECT,
		ID_TAGEDITOR_COLOR2RECT,
		ID_TAGEDITOR_COLOR3RECT,
	};

	Sprite* pColorRect;

	for( int i = 0; i < 3; i++ )
	{
		pColorRect = GetSprite( ColorRects[i] );

		if( pColorRect )
			pColorRect->SetBaseColor( m_TagRed[i], m_TagGreen[i], m_TagBlue[i], 1.0f );
	}

	return;
}

//=============================================================================
// ScreenTagEditor::OnScreenRefresh
//=============================================================================

void ScreenTagEditor::OnScreenRefresh( void )
{
	ScreenBaseClass::OnScreenRefresh();

	UpdateTagString();

	ScreenManager* pScreenManager = GetScreenManager();
	ScreenKeyboard* pKeyboard = GetKeyboard();

	if( pScreenManager && pKeyboard )
	{
		if( pScreenManager->GetActiveScreen() == pScreenManager->GetScreen( "SCREEN_ID_KEYBOARD") )
		{
			SetText( ID_TAGEDITOR_STRING1, pKeyboard->GetString() );
			SetText( ID_TAGEDITOR_STRING2, pKeyboard->GetString() );
			SetText( ID_TAGEDITOR_STRING3, pKeyboard->GetString() );
		}
	}

	return;
}

//=============================================================================
// ScreenTagEditor::OnPopupHide
//=============================================================================

void ScreenTagEditor::OnPopupHide( void )
{
	ScreenBaseClass::OnPopupHide();

	ShowMainMenu( true );
}

//=============================================================================
// ScreenTagEditor::OnCommand
//=============================================================================

void ScreenTagEditor::OnCommand( int Command )
{
	switch( Command )
	{
	case ID_TAGEDITOR_TEXT:
		OnCommandText();
		break;

	case ID_TAGEDITOR_COLORS:
		OnCommandColors();
		break;

	case ID_TAGEDITOR_DONE:
		OnCommandDone();
		break;

	case ID_TAGEDITOR_COLOR1:
		OnCommandColorPicker(0);
		break;

	case ID_TAGEDITOR_COLOR2:
		OnCommandColorPicker(1);
		break;

	case ID_TAGEDITOR_COLOR3:
		OnCommandColorPicker(2);
	}

	return;
}

//=============================================================================
// ScreenTagEditor::OnCursorUp
//=============================================================================

void ScreenTagEditor::OnCursorUp( void )
{
	ScreenBaseClass::OnCursorUp();
}

//=============================================================================
// ScreenTagEditor::OnCursorDown
//=============================================================================

void ScreenTagEditor::OnCursorDown( void )
{
	ScreenBaseClass::OnCursorDown();
}

//=============================================================================
// ScreenTagEditor::OnCursorLeft
//=============================================================================

void ScreenTagEditor::OnCursorLeft( void )
{
	OnCursorMovement( GetCurrentButtonId(),-1 );
}

//=============================================================================
// ScreenTagEditor::OnCursorRight
//=============================================================================

void ScreenTagEditor::OnCursorRight( void )
{
	OnCursorMovement( GetCurrentButtonId(),1 );
}


//=============================================================================
// ScreenTagEditor::OnCursorMovement
//=============================================================================

void ScreenTagEditor::OnCursorMovement( int ButtonId, int N )
{
	GuiProgressBar* pBar = 0;

	switch( ButtonId )
	{
	case ID_TAGEDITOR_REDSLIDER:
		pBar = m_pRedBar;
		break;

	case ID_TAGEDITOR_GREENSLIDER:
		pBar = m_pGreenBar;
		break;

	case ID_TAGEDITOR_BLUESLIDER:
		pBar = m_pBlueBar;
		break;
	}

	if( pBar )
		pBar->Step(N);

	return;
}

//=============================================================================
// ScreenTagEditor::OnButtonOK
//=============================================================================

void ScreenTagEditor::OnButtonOK( void )
{
	ScreenBaseClass::OnButtonOK();
}


//=============================================================================
// ScreenTagEditor::OnButtonCancel
//=============================================================================

void ScreenTagEditor::OnButtonCancel( void )
{
	switch( m_MenuMode )
	{
	case MENUMODE_MAIN:
		CancelChanges();
		break;

	case MENUMODE_COLORS:
		ShowColorsMenu( false );
		ShowMainMenu( true );
		break;

	case MENUMODE_PICKER:
		ShowColorPicker( false );
		ShowColorsMenu( true );
		break;
	}

	return;
}

//=============================================================================
// ScreenTagEditor::OnButtonStart
//=============================================================================

void ScreenTagEditor::OnButtonStart( void )
{
	ScreenBaseClass::OnButtonOK();
}

//=============================================================================
// ScreenTagEditor::OnKeyboardInput
//=============================================================================

const char* NaughtyWords[] =
{
	"2ob", "3cst4", "3ncvl", "3ng1n", "3xt4s", "4n4l", "anal", "anale", "anus", 
	"arsch", "arse", "ass", "b1co", "b1ko", "b1t3", "b1t3s", "b3vh", "b41s3", "baise", 
	"balle", "bicha", "bico", "biga", "biko", "bitch", "bite", "bites", "boch3", 
	"boche", "bolpo", "brvt3", "b÷lsi", "b°sse", "c*nt", "c.unt", "c4c4", "cabra", 
	"cacca", "cagna", "cagon", "cazzi", "cazzo", "cesso", "ch13r", "ch4vd", "chier", 
	"cl1to", "clit", "clito", "cock", "cocks", "cojon", "cona", "conn3", "conne", 
	"corno", "covp", "cu.nt", "cul", "culo", "cum", "cun.t", "cunnt", "cunt", "cuunt", 
	"cuzpo", "cvl", "cvnt", "cynt", "c·", "dildo", "dio", "dirne", "dritt", "droga", 
	"duce", "ducon", "dvcon", "dwaas", "dwiep", "ebreo", "eikel", "f*ck", "f.ick", 
	"f.u.t", "f0tze", "f1on", "faen", "fag", "fcik", "fcuk", "feier", "felch", 
	"fette", "fica", "fick", "figg", "figge", "fion", "fisse", "fitte", "foder", 
	"foll4", "folla", "fotz3", "fotze", "fotzl", "foy4", "foy4r", "fu.t", "fuck", 
	"fuk", "fut", "fvck", "fvt", "fyck", "fyt", "f·ck", "f·ck", "fvck", "fnck", 
	"g3rb3", "g4rc3", "gay", "geil", "geile", "gerbe", "gesu", "ghey", "gl4nd", 
	"gland", "gnak", "god3", "gode", "gozo", "hoden", "hoer", "homo", "hore", "huera", 
	"huora", "hure", "huren", "jaco", "japse", "jod4n", "jodan", "jode", "joder", 
	"jude", "juter", "jvt3r", "jµvel", "jµvla", "kike", "kkk", "klojo", "kloot", 
	"klux", "kl÷te", "knep", "kojon", "krunk", "kuk", "kulli", "kusi", "kusse", "kut", 
	"kyrpS", "k°dd", "k°lle", "lamer", "lappi", "lesbe", "lesbo", "loffa", "looli", 
	"luder", "lul", "m3lon", "m3rd", "m3rd3", "mela", "merda", "merde", "moese", 
	"monga", "morv3", "mose", "muna", "m÷pse", "m÷se", "m°ss", "n1gg4", "n1gga", 
	"n421", "nagle", "naida", "nazi", "neger", "negro", "nigaz", "nigga", "no3vd", 
	"noeud", "ntm", "nute", "nutte", "nnsse", "ostia", "p1n3", "p1p1", "p1p3", 
	"p1ss3", "p1sse", "p3d3", "p3t3v", "p41n", "paki", "palle", "paska", "pd", 
	"peado", "pede", "pedo", "pela", "pene", "penis", "perse", "peteu", "phuk", 
	"phux", "pica", "picio", "pik", "pikk", "pila", "pillu", "pine", "pipe", "pirla", 
	"piru", "pissa", "plouc", "plovc", "pol4c", "pol4k", "polac", "polak", "poll4", 
	"polla", "pompa", "poot", "porco", "porn", "porno", "porra", "poya", "puff", 
	"pule", "pussy", "put4", "puta", "pute", "puto", "puton", "pvt3", "pvt4", "pvta", 
	"pvto", "pTdT", "p·ssy", "p·ssy", "pvssy", "pnssy", "queer", "queue", "qv3v3", 
	"r1t4l", "r4ton", "rape", "reet", "revne", "rotop", "r°v", "s4lop", "salop", 
	"santi", "sesso", "sh1t", "shit", "simon", "skrid", "slet", "snade", "soper", 
	"sorca", "spunk", "suoro", "svans", "t4rt3", "tetta", "tette", "tispe", "tit", 
	"tits", "topa", "tr1so", "triso", "troia", "trou", "trov", "trut", "tucke", 
	"tunte", "tw@t", "twat", "t°s", "v1bro", "v1ol", "v4g1n", "vadia", "vagin", 
	"viado", "vibro", "viol", "vittu", "vitun", "vogle", "vulve", "vvlv3", "w4nk", 
	"wank", "whore", "wichs", "xota", "zinne", "zob", "zorra", "SpSrS", 
};

bool IsNaughtyWord(const char* word)
{
	if (word && word[0])
	{
		for (int i = 0; i < (sizeof(NaughtyWords) / sizeof(NaughtyWords[0])); i++)
			if (stricmp(NaughtyWords[i], word) == 0)
				return true;
	}

	return false;
}

//=============================================================================

void ScreenTagEditor::OnKeyboardInput( const char* pString )
{
	m_OriginalTag.set( m_TagString.get() );
	m_TagString.set( pString );

	bool Online = false;

	if( ValidSingleton( CLobby ) && !g_lobby.IsLAN() )
		Online = true;

	if( Online )
	{
		g_messageDispatcher.SendMessageToLocalHost( "FilterText", (void*) pString, INVALID_OBJECT_HANDLE );
		ShowPopup( "SCREEN_ID_PLEASEWAIT" );
	}
	else
	{
		if( IsNaughtyWord( pString ) )
			OnIllegalTag();
	}

	return;
}

//=============================================================================
// ScreenTagEditor::OnDialogCommand
//=============================================================================

void ScreenTagEditor::OnDialogCommand( int Command )
{
	switch( Command )
	{
	case COMMAND_TAG_CANCELYES:
		GotoPrevScreen();
		break;
	
	case COMMAND_TAG_CANCELNO:
		g_FrontEnd.ClosePopup( "SCREEN_ID_POPUPDIALOG" );
		break;
	
	case COMMAND_TAG_ERROROK:
		g_FrontEnd.ClosePopup( "SCREEN_ID_POPUPDIALOG" );
		break;

	case COMMAND_TAG_FILTEROK:
		g_FrontEnd.ClosePopup( "SCREEN_ID_POPUPDIALOG" );
		m_TagString.set( m_OriginalTag.get() );
		break;
	}

	return;
}

//=============================================================================
// ScreenTagEditor::OnCommandText
//=============================================================================

void ScreenTagEditor::OnCommandText( void )
{
	ShowMainMenu( false );
	ShowKeyboard( true );
}

//=============================================================================
// ScreenTagEditor::OnCommandColors
//=============================================================================

void ScreenTagEditor::OnCommandColors( void )
{
	ShowMainMenu( false );
	ShowColorsMenu( true );
}

//=============================================================================
// ScreenTagEditor::OnCommandDone
//=============================================================================

void ScreenTagEditor::OnCommandDone( void )
{
	g_GlobalProfile.SetTagColor( 0, m_TagRed[0], m_TagGreen[0], m_TagBlue[0], 1.0f );
	g_GlobalProfile.SetTagColor( 1, m_TagRed[1], m_TagGreen[1], m_TagBlue[1], 1.0f );
	g_GlobalProfile.SetTagColor( 2, m_TagRed[2], m_TagGreen[2], m_TagBlue[2], 1.0f );
	g_GlobalProfile.SetTagString( m_TagString.get() );

	m_TagString.release();
	m_OriginalTag.release();

	EndScreen(0);
}

//=============================================================================
// ScreenTagEditor::OnCommandColorPicker
//=============================================================================

void ScreenTagEditor::OnCommandColorPicker( int n )
{
	m_ColorIndex = n;

	int TextId = 0;

	switch( n )
	{
	case 0:
		TextId = ID_TAGEDITOR_COLOR1;
		break;

	case 1:
		TextId = ID_TAGEDITOR_COLOR2;
		break;

	case 2:
		TextId = ID_TAGEDITOR_COLOR3;
		break;
	}

	Text* pSource = GetTextSprite( TextId );
	const char* pColorType = 0;

	if( pSource )
		pColorType = pSource->GetText();

	Printf( ID_TAGEDITOR_ZONE, "%s: %s", g_translator.Translate( "Modify Color" ), pColorType );
	
	ShowColorsMenu( false );
	ShowColorPicker( true );
}

//=============================================================================
// ScreenTagEditor::OnIllegalTag
//=============================================================================

void ScreenTagEditor::OnIllegalTag( void )
{
	ScreenPopupDialog* pDialog = GetPopupDialog();

	if( pDialog )
	{
		pDialog->Clear();
		pDialog->SetDialogId( DIALOGID_TAG_FILTER );
		pDialog->SetMessage( "The text entered contains offensive content." );
		pDialog->SetCancelCommand( COMMAND_TAG_FILTEROK );
		pDialog->AddMenuItem( "OK", COMMAND_TAG_FILTEROK, 0 );
		ShowPopupDialog();
	}

	return;
}

//==========================================================================
// ScreenTagEditor::TextFiltered::HandleMessage
//
//		This message handler responds to the text being filtered or not
//==========================================================================

void ScreenTagEditor::TextFiltered::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	bool filtered = i_data == 0 ? false : true;

	if( d_ref->GetActive() )
	{
		g_FrontEnd.ClosePopup( "SCREEN_ID_PLEASEWAIT" );

		if( filtered )
			d_ref->OnIllegalTag();
	}

	return;
}
