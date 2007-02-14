#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_wait.h"
#include "data/screens/MultiplayerPopups.h"
#include "frontend/frontend.h"

static int idList[] =
{
    ID_WAIT_0, 
    ID_WAIT_1,
    ID_WAIT_2,
    ID_WAIT_3,
    ID_WAIT_4,
    ID_WAIT_5,
    ID_WAIT_6,
    ID_WAIT_7,
    ID_WAIT_8,
    ID_WAIT_9,
    ID_WAIT_10,
    ID_WAIT_11,
    ID_WAIT_12,
    ID_WAIT_13,
    ID_WAIT_14,
    ID_WAIT_15,
    ID_WAIT_16
};

static int numIds = sizeof(idList) / sizeof(int);

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenWait::OnInitialize(void)
{
	UseHighlighting(true);

	ScreenBaseClass::OnInitialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenWait::OnScreenIntro( void )
{
	if(m_pScreen)
	{
		for( int i = 0; i < numIds; ++i )
		{
			m_pScreen->GetSprite(idList[i])->m_state.color.a = 100;
		}
    }

    _currentPos = 0;

    ScreenBaseClass::OnScreenIntro();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the screen.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenWait::OnScreenUpdate( void )
{
    int last = _currentPos;

    ++_currentPos;
    if (_currentPos >= numIds)
    {
        _currentPos = 0;
    }

    m_pScreen->GetSprite(idList[last])->m_state.color.a = 100;
    m_pScreen->GetSprite(idList[_currentPos])->m_state.color.a = 255;
}
