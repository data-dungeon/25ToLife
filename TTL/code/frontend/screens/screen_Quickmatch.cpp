#include "TTLPCH.h"
#include "frontend/frontend.h"
#include "frontend/inputdriver.h"
#include "frontend/screens/screen_Quickmatch.h"
#include "data/screens/Multiplayer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first created.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenQuickmatch::OnInitialize(void)
{
	// Hide PS2 help bar on PC
#ifdef DIRECTX_PC
	GetSprite(ID_QM_HELPICON1)->HideAll();
	GetSprite(ID_QM_HELPICON2)->HideAll();
	GetSprite(ID_QM_HELPICON3)->HideAll();

	// Setup buttons on PC
	SetInitialButton(ID_QM_JOIN);
	SetNextButton(ID_QM_NEXT);
	SetNextButton(ID_QM_BACK);

	// change the text to represent the bound keyboard controls
	GetTextSprite(ID_QM_BACK)->SetText("ESC - Back");
	GetTextSprite(ID_QM_JOIN)->SetText("Enter - Join");

	GetTextSprite(ID_QM_NEXT)->SetText("Right Arrow - Next");
#endif

	ScreenBaseClass::OnInitialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is displayed.  Reset the status.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenQuickmatch::OnScreenIntro(void)
{
    _status = RUNNING;

    ScreenBaseClass::OnScreenIntro();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when a button is pressed.  Trap the square button and set status accordingly.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenQuickmatch::OnButtonPressed(int Button)
{
#ifdef DIRECTX_PC
	if (FluffControlMapper::RIGHT == Button)
		_status = NEXT;
#else
    if (FluffControlMapper::SQUARE == Button)
    {
        _status = NEXT;
    }
#endif

    ScreenBaseClass::OnButtonPressed(Button);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the text field with the match number on the screen
///
/// \param Match The offset of the current match in the total list of matches
///
/// \param NumMatches The total number of matches in existance.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenQuickmatch::SetMatchNum(int Match, int NumMatches)
{
    ASSERT(m_pScreen);
    ASSERT(m_pScreen->GetTextSprite(ID_QUICK_MATCH));
    ASSERT(Match <= NumMatches);
    if (!m_pScreen) return;
    if (!m_pScreen->GetTextSprite(ID_QUICK_MATCH)) return;

    char Output[50];

    sprintf(Output, "%d / %d", Match, NumMatches);

    m_pScreen->GetTextSprite(ID_QUICK_MATCH)->SetText(Output);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the string for the game type of the current match
///
/// \param pGameType The string for the game type of the current match
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenQuickmatch::SetGameType(const char *pGameType)
{
    ASSERT(m_pScreen);
    ASSERT(m_pScreen->GetTextSprite(ID_QUICK_TYPE));
    ASSERT(pGameType);
    if (!m_pScreen) return;
    if (!m_pScreen->GetTextSprite(ID_QUICK_TYPE)) return;
    if (!pGameType) return;

    m_pScreen->GetTextSprite(ID_QUICK_TYPE)->SetText(pGameType);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the player string on the current screen
///
/// \param NumPlayers Number of players in this match
///
/// \param MaxPlayers Max players allowed in this game
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenQuickmatch::SetNumPlayers(int NumPlayers, int MaxPlayers)
{
    ASSERT(m_pScreen);
    ASSERT(m_pScreen->GetTextSprite(ID_QUICK_PLAYERS));
    ASSERT(NumPlayers <= MaxPlayers);
    if (!m_pScreen) return;
    if (!m_pScreen->GetTextSprite(ID_QUICK_PLAYERS)) return;

    char Output[50];

    sprintf(Output, "%d / %d", NumPlayers, MaxPlayers);

    m_pScreen->GetTextSprite(ID_QUICK_PLAYERS)->SetText(Output);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the name of the player hosting the current match
///
/// \param pHostName The name of the player hosting the current match
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenQuickmatch::SetHostName(const char *pHostName)
{
    ASSERT(m_pScreen);
    ASSERT(m_pScreen->GetTextSprite(ID_QUICK_HOST));
    ASSERT(pHostName);
    if (!m_pScreen) return;
    if (!m_pScreen->GetTextSprite(ID_QUICK_HOST)) return;
    if (!pHostName) return;

    m_pScreen->GetTextSprite(ID_QUICK_HOST)->SetText(pHostName);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the name of the map being played in the current match
///
/// \param pHostName The name of the map being played in the current match
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenQuickmatch::SetMapName(const char *pMapName)
{
    ASSERT(m_pScreen);
    ASSERT(m_pScreen->GetTextSprite(ID_QUICK_MAP));
    ASSERT(pMapName);
    if (!m_pScreen) return;
    if (!m_pScreen->GetTextSprite(ID_QUICK_MAP)) return;
    if (!pMapName) return;

    m_pScreen->GetTextSprite(ID_QUICK_MAP)->SetText(pMapName);
}

#ifdef DIRECTX_PC
void ScreenQuickmatch::OnCommand(int commandId)
{
	switch (commandId)
	{
		case ID_QM_JOIN:
			_status = JOIN;
			break;

		case ID_QM_NEXT:
			_status = NEXT;
			break;

		case ID_QM_BACK:
			_status = BACK;
			break;
	}

}
#endif