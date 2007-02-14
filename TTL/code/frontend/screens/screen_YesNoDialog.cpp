#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_YesNoDialog.h"
#include "data/screens/MultiplayerPopups.h"
#include "frontend/frontend.h"

/*virtual*/ void ScreenYesNoDialog::OnInitialize(void)
{
    Screen* pScreen = m_pScreen;

    UseHighlighting(true);

	if(pScreen)
	{
		pScreen->SetInitialButton(ID_YESNO_YES);
		pScreen->SetNextButton(ID_YESNO_NO);
		pScreen->SetCursor(ID_YESNO_CURSOR);

        pScreen->SelectButton(ID_YESNO_YES);
	}

    _title[0] = 0;
    _text[0] =0;

	ScreenBaseClass::OnInitialize();
}

void ScreenYesNoDialog::OnScreenIntro( void )
{
    _status = RUNNING;

    if (m_pScreen)
    {
        m_pScreen->GetTextSprite(ID_YESNO_TITLE)->SetText(_title);
        m_pScreen->GetTextSprite(ID_YESNO_TEXT)->SetText(_text);
		  // alignment is specified in the tool!
        //m_pScreen->GetTextSprite(ID_YESNO_TEXT)->SetTextAlignment(0.5f);
        m_pScreen->GetTextSprite(ID_YESNO_TEXT)->SetOrigin(0.5f, 0.5f);
    }

    ScreenBaseClass::OnScreenIntro();
}

void ScreenYesNoDialog::OnCommand( int Command )
{
    // Set the status flag
    switch (Command)
    {
        case ID_YESNO_YES:
            _status = YES;
            break;

        case ID_YESNO_NO:
            _status = NO;
            break;
    }

    ScreenBaseClass::OnCommand(Command);
}

void ScreenYesNoDialog::SetTitleText(const char *pText)
{
	strncpy(_title, pText, 250);
	SetText( ID_YESNO_TITLE, _title );
}

void ScreenYesNoDialog::SetDialogText(const char *pText)
{
	strncpy(_text, pText, 1000);
	SetText( ID_YESNO_TEXT, _text );
}

void ScreenYesNoDialog::SetDefaultButtonToYes(bool yesButton)
{
	if(yesButton)
	{
		m_pScreen->SetInitialButton(ID_YESNO_YES);
		m_pScreen->SetNextButton(ID_YESNO_NO);
	}
	else
	{
		m_pScreen->SetInitialButton(ID_YESNO_NO);
		m_pScreen->SetNextButton(ID_YESNO_YES);
	}
	m_pScreen->SelectInitialButton();
}