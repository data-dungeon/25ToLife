#include "TTLPCH.h"
#include "frontend/frontend.h"
#include "frontend/inputdriver.h"
#include "frontend/screens/screen_Dialog.h"
#include "data/screens/MultiplayerPopups.h"

/*virtual*/ void ScreenDialog::OnInitialize(void)
{
    Screen* pScreen = m_pScreen;

    UseHighlighting(true);

	if(pScreen)
	{
		pScreen->SetInitialButton(ID_DIALOG_OK);
		pScreen->SetCursor(ID_DIALOG_CURSOR);

        pScreen->SelectButton(ID_DIALOG_OK);
	}

	ScreenBaseClass::OnInitialize();
}

void ScreenDialog::OnScreenIntro( void )
{
    _status = RUNNING;

    Screen* pScreen = m_pScreen;

	if(pScreen)
	{
        pScreen->GetTextSprite(ID_DIALOG_TITLE)->SetText(_title);
        pScreen->GetTextSprite(ID_DIALOG_MSG)->SetText(_text);
    }

    ScreenBaseClass::OnScreenIntro();
}

void ScreenDialog::OnCommand( int Command )
{
    _status = DONE;

    ScreenBaseClass::OnCommand(Command);
}

void ScreenDialog::SetTitleText(const char *pText)
{
	strncpy(_title, pText, 250);
	if(m_pScreen)
	{
		m_pScreen->GetTextSprite(ID_DIALOG_TITLE)->SetText(_title);
	}
}

void ScreenDialog::SetDialogText(const char *pText)
{
	strncpy(_text, pText, 500);
	if(m_pScreen)
	{
		m_pScreen->GetTextSprite(ID_DIALOG_MSG)->SetText(_text);
	}
}
