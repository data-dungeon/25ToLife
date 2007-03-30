#include "ttlpch.h"
#include "GameHelper/Translator.h"
#include "frontend/frontend.h"
#include "frontend/screens/screen_Briefing.h"
#include "data/screens/briefing.h"
#include "avatar/sobj.h"

bool	g_ObjectivesOnScreen = false;

void ScreenBriefing::SetPrimaryObjective( int n, const char* pObjective )
{
	SetText( GetPrimaryId(n), pObjective );
}

void ScreenBriefing::SetSecondaryObjective( int n, const char* pObjective )
{
	SetText( GetSecondaryId(n), pObjective );
}

int ScreenBriefing::GetPrimaryId( int n )
{
	switch(n)
	{
	case 0:
		return ID_BRIEFING_PRIMARY1;
	}

	return 0;
}

int ScreenBriefing::GetSecondaryId( int n )
{
	switch(n)
	{
	case 0:
		return ID_BRIEFING_SECONDARY1;
	case 1:
		return ID_BRIEFING_SECONDARY2;
	case 2:
		return ID_BRIEFING_SECONDARY3;
	}

	return 0;
}

void ScreenBriefing::InitLevel( void )
{
	SetText( ID_BRIEFING_TITLE, g_gameEnvironment.GetWorldDisplayName(g_gameEnvironment.Get()) );
	SetText( ID_BRIEFING_MISSIONTEXT, g_gameEnvironment.GetMissionText(g_gameEnvironment.Get()) );
}

void ScreenBriefing::InitThumbnail( void )
{
	const char* pFileName = g_gameEnvironment.GetBriefingScreen();
	m_BriefingImage = INVALID_HANDLE;
	if( pFileName )
	{
		m_BriefingImage = dblLoad( pFileName );

		Sprite* pImage = GetSprite( ID_BRIEFING_THUMBNAIL );

		if( pImage )
			pImage->SetTextureSet( m_BriefingImage, 0 );
	}

	return;
}

void ScreenBriefing::InitObjectives( void )
{
	char	s[ 255 ];
	bool	done = false;
	int	i , o;

	ScreenBaseClass::OnScreenIntro();

	// primary objectives
	g_secondaryObjective.GetFirstPrimaryObjective( o );
	done = false;
	i = 0;
	while( !done )
	{
		g_secondaryObjective.GetObjectiveString( o , s );
		if (s && *s)
			SetPrimaryObjective( i , s );
		else
			SetPrimaryObjective( i , "None." );
		done = g_secondaryObjective.GetNextPrimaryObjective( o );
		i++;
	}

	// bonus objectives
	g_secondaryObjective.GetFirstBonusObjective( o );
	i = 0;
	if( o!=-1 )
	{
		done = false;
		while( !done )
		{
			g_secondaryObjective.GetObjectiveString( o , s );
			SetSecondaryObjective( i , s );	
			done = g_secondaryObjective.GetNextBonusObjective( o );
			i++;
		}
	}
	else
	{
		SetSecondaryObjective( i , 0/*"None."*/ );
		i++;
	}

	// stat objectives
	g_secondaryObjective.GetFirstStatObjective( o );
	if( o != -1 )
	{
		done = false;
		while( !done )
		{
			g_secondaryObjective.GetObjectiveString( o , s );
			SetSecondaryObjective( i , s );
			done = g_secondaryObjective.GetNextStatObjective( o );
			i++;
		}
	}
	else
	{
		SetSecondaryObjective( i , 0 );
	}

	return;

}

void ScreenBriefing::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	// set to active
	g_ObjectivesOnScreen = true;

	// Initialize the level and playing as character name...
	InitLevel();

	// Initialize the objectives
	InitObjectives();

	g_messageDispatcher.SendMessageToAll("TimeSpeed", CMessageDispatcher::IntToData(CMasterTimer::PAUSE), INVALID_OBJECT_HANDLE);
#ifdef DIRECTX_PC
	g_MenuCursor.SetHidden(false);
	QuitButton.Show(false);
	BackButton.Show(false);
	QuitButton.Update();
	BackButton.Update();
#endif
}

void ScreenBriefing::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	SetHelpRect( ID_BRIEFING_CONTINUE );

	// clear it
	g_ObjectivesOnScreen = false;

	InitThumbnail();

	#ifdef DIRECTX_PC
		GetSprite(ID_BRIEFING_HELPICON)->Hide();
		//SetInitialButton(ID_BRIEFING_CONTINUE);
	#endif

	SetPrimaryObjective( 0, 0 );
	SetPrimaryObjective( 1, 0 );
	SetPrimaryObjective( 2, 0 );

	SetSecondaryObjective( 0, 0 );
	SetSecondaryObjective( 1, 0 );
	SetSecondaryObjective( 2, 0 );

	SetText( ID_BRIEFING_TITLE, 0 );
}

void ScreenBriefing::OnScreenExit( void )
{
	g_messageDispatcher.SendMessageToAll("TimeSpeed", CMessageDispatcher::IntToData(CMasterTimer::ONE_X), INVALID_OBJECT_HANDLE);

	ScreenBaseClass::OnScreenExit();

	if( m_BriefingImage != INVALID_HANDLE )
	{
		Sprite* pImage = GetSprite( ID_BRIEFING_THUMBNAIL );

		if( pImage )
		{
			pImage->ClearTextureSet();
			pImage->Reset();
		}

		dblRemove( m_BriefingImage );

		// DBL files will log into the texture cache, so we need to reset it
		TextureMgr::Cache().Reset();

		m_BriefingImage = INVALID_HANDLE;
	}

	g_ObjectivesOnScreen = false;
#ifdef DIRECTX_PC
	g_MenuCursor.SetHidden(true);
	QuitButton.Show(false);
	BackButton.Show(false);
#endif
}

void ScreenBriefing::OnButtonOK( void )
{
	SPause pauseMessage;
	pauseMessage.d_type = SPause::UNPAUSE;
	pauseMessage.d_menu = NULL;
	g_messageDispatcher.PostMessageToAll("Pause", (void*)&pauseMessage,
		INVALID_OBJECT_HANDLE);

	GotoScreen(0);
}

#ifdef DIRECTX_PC
void ScreenBriefing::OnScreenUpdate()
{
	if (g_MenuCursor.CursorButtonPressed(0))
	{
		OnButtonOK();
	}
}
#endif

//=============================================================================
// ScreenStatsSummary::SetStatistic
//=============================================================================

void ScreenStatsSummary::SetStatistic(int n, const char* pStat, const char* pValue )
{
	Text* pStatText = GetStatistic(n);

	if( pStatText )
		pStatText->SetText( pStat );

	Text* pValueText = GetValue(n);

	if( pValueText )
		pValueText->SetText( pValue );

	return;
}

//=============================================================================
// ScreenStatsSummary::GetStatistic
//=============================================================================

Text* ScreenStatsSummary::GetStatistic( int n )
{
	return GetTextSprite( GetStatId(n) );
}

//=============================================================================
// ScreenStatsSummary::GetValue
//=============================================================================

Text* ScreenStatsSummary::GetValue( int n )
{
	return GetTextSprite( ID_STATISTICS_VALUE, GetStatistic(n) );
}

//=============================================================================
// ScreenStatsSummary::GetStatId
//=============================================================================

int ScreenStatsSummary::GetStatId( int n )
{
	switch( n )
	{
	case 0:
		return ID_STATISTICS_STAT1;
	case 1:
		return ID_STATISTICS_STAT2;
	case 2:
		return ID_STATISTICS_STAT3;
	case 3:
		return ID_STATISTICS_STAT4;
	case 4:
		return ID_STATISTICS_STAT5;
	}

	return 0;
}


//=============================================================================
// ScreenStatsSummary::OnInitialize
//=============================================================================

void ScreenStatsSummary::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	SetHelpRect( ID_STATISTICS_HELPTEXT1 );
#ifdef DIRECTX_PC
		GetSprite(ID_STATISTICS_HELPTEXT1)->Hide();
		//SetInitialButton(ID_BRIEFING_CONTINUE);
#endif

	SetStatistic( 0, 0, 0 );
	SetStatistic( 1, 0, 0 );
	SetStatistic( 2, 0, 0 );
	SetStatistic( 3, 0, 0 );
	SetStatistic( 4, 0, 0 );
}

//=============================================================================
// ScreenStatsSummary::OnScreenIntro
//=============================================================================

void ScreenStatsSummary::OnScreenIntro( void )
{
	char s[ 256 ];
	char v[ 20 ];

	g_secondaryObjective.End();

	ScreenBaseClass::OnScreenIntro();

	// TODO: Show the statistics...
	char *role = NULL;
	if (g_players.GetPlayer(0) != NULL)
		role = g_players.GetPlayer( 0 )->Parm().GetString( "role" );

	if( !strcmp( role , "cop" ) )
	{
		g_secondaryObjective.GetStatString( O_HEADSHOTS , s );
		g_secondaryObjective.GetStatValue( O_HEADSHOTS , v );
		SetStatistic( 0 , s , v );

		g_secondaryObjective.GetStatString( O_CIVILIANS , s );
		g_secondaryObjective.GetStatValue( O_CIVILIANS , v );
		SetStatistic( 1 , s , v );

		g_secondaryObjective.GetStatString( O_KILLS , s );
		g_secondaryObjective.GetStatValue( O_KILLS , v );
		SetStatistic( 2 , s , v );

		g_secondaryObjective.GetStatString( O_ACCURACY , s );
		g_secondaryObjective.GetStatValue( O_ACCURACY , v );
		SetStatistic( 3 , s , v );

		g_secondaryObjective.GetStatString( O_APPREHEND , s );
		g_secondaryObjective.GetStatValue( O_APPREHEND , v );
		SetStatistic( 4 , s , v );
	}
	else
	{
		g_secondaryObjective.GetStatString( O_HEADSHOTS , s );
		g_secondaryObjective.GetStatValue( O_HEADSHOTS , v );
		SetStatistic( 0 , s , v );

		g_secondaryObjective.GetStatString( O_CIVILIANS , s );
		g_secondaryObjective.GetStatValue( O_CIVILIANS , v );
		SetStatistic( 1 , s , v );

		g_secondaryObjective.GetStatString( O_KILLS , s );
		g_secondaryObjective.GetStatValue( O_KILLS , v );
		SetStatistic( 2 , s , v );

		g_secondaryObjective.GetStatString( O_ACCURACY , s );
		g_secondaryObjective.GetStatValue( O_ACCURACY , v );
		SetStatistic( 3 , s , v );

		g_secondaryObjective.GetStatString( O_HOSTAGE , s );
		g_secondaryObjective.GetStatValue( O_HOSTAGE , v );
		SetStatistic( 4 , s , v );
	}

#ifdef DIRECTX_PC
	StartButton.SetText("Press Enter to Continue");
	StartButton.Show(true);
	QuitButton.Show(false);
	BackButton.Show(false);
	QuitButton.Update();
	BackButton.Update();
#endif
	return;
}

//=============================================================================
// ScreenStatsSummary::OnButtonOK
//=============================================================================

void ScreenStatsSummary::OnButtonOK( void )
{
#ifdef DIRECTX_PC
	StartButton.Show(false);
#endif

	GotoScreen( "SCREEN_ID_SUMMARY" );
}

//=============================================================================
// ScreenObjectiveSummary::SetObjective
//=============================================================================

void ScreenObjectiveSummary::SetObjective( int n, const char* pText )
{
	Text* pObjective = GetObjectiveText( n );

	if( pObjective )
	{
		pObjective->SetText( pText );

		if( xstrlen(pText) > 0 )
			pObjective->Show();
		else
			pObjective->Hide();
	}

	return;
}

//=============================================================================
// ScreenObjectiveSummary::SetObjectiveCompleted
//=============================================================================

void ScreenObjectiveSummary::SetObjectiveCompleted( int n, bool Completed )
{
	Sprite* pCrossOut = GetSprite( ID_SUMMARY_CROSSOUT, GetObjectiveText(n) );

	if( pCrossOut )
		pCrossOut->Show( !Completed );

	Text* pStatus = GetTextSprite( ID_SUMMARY_STATUS, GetObjectiveText(n) );

	if( pStatus )
	{
		Text* pSourceText = 0;

		if( Completed )
			pSourceText = GetTextSprite( ID_SUMMARY_COMPLETED );
		else
			pSourceText = GetTextSprite( ID_SUMMARY_FAILED );

		if( !pSourceText )
			pStatus->Hide();
		else
		{
			float x, y;
			pStatus->GetPosition( x, y );
			pStatus->Copy( pSourceText );
			pStatus->SetPosition( x, y );
			pStatus->Show();
		}
	}

	return;
}

//=============================================================================
// ScreenObjectiveSummary::SetReward
//=============================================================================

void ScreenObjectiveSummary::SetReward( int n, const char* pText )
{
	Text* pReward = GetRewardText( n );

	if( pReward )
	{
		pReward->SetText( pText );

		if( xstrlen( pText ) > 0 )
			pReward->Show();
		else
			pReward->Hide();
	}

	return;
}

//=============================================================================
// ScreenObjectiveSummary::GetObjectiveId
//=============================================================================

int ScreenObjectiveSummary::GetObjectiveId( int n )
{
	switch( n )
	{
	case 0:
		return ID_SUMMARY_OBJECTIVE1;
	case 1:
		return ID_SUMMARY_OBJECTIVE2;
	case 2:
		return ID_SUMMARY_OBJECTIVE3;
	case 3:
		return ID_SUMMARY_OBJECTIVE4;
	}

	return 0;
}

//=============================================================================
// ScreenObjectiveSummary::GetObjectiveText
//=============================================================================

Text* ScreenObjectiveSummary::GetObjectiveText(	int n )
{
	return GetTextSprite( GetObjectiveId(n) );
}

//=============================================================================
// ScreenObjectiveSummary::GetRewardText
//=============================================================================

Text* ScreenObjectiveSummary::GetRewardText(	int n )
{
	return GetTextSprite( ID_SUMMARY_REWARD, GetObjectiveText(n) );
}

//=============================================================================
// ScreenObjectiveSummary::OnInitialize
//=============================================================================

void ScreenObjectiveSummary::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	SetHelpRect( ID_SUMMARY_HELPRECT );
	#ifdef DIRECTX_PC
		GetSprite(ID_SUMMARY_HELPRECT)->Hide();
		//SetInitialButton(ID_BRIEFING_CONTINUE);
	#endif

	SetObjective( 0, 0 );
	SetObjective( 1, 0 );
	SetObjective( 2, 0 );
	SetObjective( 3, 0 );

	SetObjectiveCompleted( 0, false );
	SetObjectiveCompleted( 1, false );
	SetObjectiveCompleted( 2, false );
	SetObjectiveCompleted( 3, false );

	SetReward( 0, 0 );
	SetReward( 1, 0 );
	SetReward( 2, 0 );
	SetReward( 3, 0 );
}

//=============================================================================
// ScreenObjectiveSummary::OnScreenIntro
//=============================================================================

void ScreenObjectiveSummary::OnScreenIntro( void )
{
	char	s[ 255 ];
	bool	done = false;
	int	i , o;

	ScreenBaseClass::OnScreenIntro();

	// primary objectives
	g_secondaryObjective.GetFirstPrimaryObjective( o );
	done = false;
	i = 0;
	while( !done )
	{
		g_secondaryObjective.GetUnlockableString( o , s );
		if( *s && strcmp( GameLocks::UnlockDescription( s ) , "." ) )
		{
			g_secondaryObjective.GetObjectiveString( o , s );
			SetObjective( i , s );
			SetObjectiveCompleted( i , g_secondaryObjective.IsObjectiveMet( o ) );
			g_secondaryObjective.GetRewardString( o , s );
			SetReward( i , ""/*s*/ );
			i++;
		}
		done = g_secondaryObjective.GetNextPrimaryObjective( o );
	}
	// bonus objectives
	g_secondaryObjective.GetFirstBonusObjective( o );
	done = false;
//	i = 0;
	if( o!=-1 )
	{
		while( !done )
		{
			g_secondaryObjective.GetObjectiveString( o , s );
			SetObjective( i , s );
			SetObjectiveCompleted( i , g_secondaryObjective.IsObjectiveMet( o ) );
			g_secondaryObjective.GetRewardString( o , s );
			SetReward( i , ""/*s*/ );
			done = g_secondaryObjective.GetNextBonusObjective( o );
			i++;
		}
	}
	else
	{
		SetObjective( i , 0 );
		SetReward( i , 0 );
		i++;
	}

	// stat objectives
	g_secondaryObjective.GetFirstStatObjective( o );
	if( o != -1 )
	{
		done = false;
		while( !done )
		{
			g_secondaryObjective.GetObjectiveString( o , s );
			SetObjective( i , s );
			SetObjectiveCompleted( i , g_secondaryObjective.IsObjectiveMet( o ) );
			g_secondaryObjective.GetRewardString( o , s );
			SetReward( i , ""/*s*/ );
			done = g_secondaryObjective.GetNextStatObjective( o );
			i++;
		}
	}
	else
	{
		SetObjective( i , 0 );
		SetReward( i , 0 );
	}

#ifdef DIRECTX_PC
	StartButton.SetText("Press Enter to Continue");
	StartButton.Show(true);
	StartButton.Show(true);
	QuitButton.Show(false);
	BackButton.Show(false);
	QuitButton.Update();
	BackButton.Update();
#endif
	return;
}

//=============================================================================
// ScreenObjectiveSummary::OnButtonOK
//=============================================================================

void ScreenObjectiveSummary::OnButtonOK( void )
{
#ifdef DIRECTX_PC
	StartButton.Show(false);
#endif

	GotoScreen(0);
}
