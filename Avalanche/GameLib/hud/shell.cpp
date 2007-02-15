/******************* includes ****************************/

#include "hud/HudPCH.h"



Screen *Screen::m_pScreenList = 0;

//=============================================================================
// Screen::Screen()
//=============================================================================

Screen::Screen( void )
{
	if( !m_pScreenList )
		m_pScreenList = this;
	else
	{
		Screen *pScreen = m_pScreenList;

		while( pScreen->m_pNextScreen )
			pScreen = pScreen->m_pNextScreen;

		pScreen->m_pNextScreen = this;
	}

	return;
}

//=============================================================================
// Screen::~Screen()
//=============================================================================

Screen::~Screen()
{
	if( m_pScreenList == this )
		m_pScreenList = m_pNextScreen;
	else
	{
		Screen *pScreen = m_pScreenList;

		while( pScreen && pScreen->m_pNextScreen != this )
			pScreen = pScreen->m_pNextScreen;

		if( pScreen )
			pScreen->m_pNextScreen = m_pNextScreen;
	}

	return;
}
