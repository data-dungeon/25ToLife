#include "TTLPCH.h"
#include "frontend/screens/screen_ScoreBoard.h"
#include "data/screens/ScoreBoard.h"

#include "frontend/savegame.h"
#include "Voice/NetZ/CVoiceChat.h"
#include "EngineHelper/timer.h"
#include "main/PlayerDB.h"
#include "environ/ConfigMgr.h"

#include "frontend/frontend.h"
#if defined (_XBOX)
#include "platform/xbox/xboxlive.h"
#endif

//=============================================================================
// ScreenScoreBoard::ShowBk
//=============================================================================

void ScreenScoreBoard::ShowBk( bool Show )
{
	Sprite *pBackground = GetSprite( ID_SCOREBOARD_BK );

	if( pBackground )
	{
		if( Show )
			pBackground->m_state.familycolor.a = 255;
		else
			pBackground->m_state.familycolor.a = 0;
	}

	m_Opaque = Show;
}

bool ScreenScoreBoard::Opaque( void )
{
	return m_Opaque;
}

//=============================================================================
// ScreenScoreBoard::ValidTeam
//=============================================================================

bool ScreenScoreBoard::ValidTeam( int t )
{
	if( t >= 0 && t < TeamCount )
		return true;

	return false;
}

//=============================================================================
// ScreenScoreBoard::ValidSlot
//=============================================================================

bool ScreenScoreBoard::ValidSlot( int s )
{
	if( s >= 0 && s < SlotCount )
		return true;

	return false;
}

//=============================================================================
// ScreenScoreBoard::ValidTeamSlot
//=============================================================================

bool ScreenScoreBoard::ValidTeamSlot( int t, int s )
{
	if( !ValidTeam(t) )
		return false;

	if( !ValidSlot(s) )
		return false;

	return true;
}

//=============================================================================
// ScreenScoreBoard::GetTeamId
//=============================================================================

int ScreenScoreBoard::GetTeamId( int t )
{
	static const int ScoreBoardTeams[] =
	{
		ID_SCOREBOARD_TEAM2,
		ID_SCOREBOARD_TEAM1,
	};

	const int c = sizeof(ScoreBoardTeams)/sizeof(ScoreBoardTeams[0]);

	if( t >= 0 && t < c )
		return ScoreBoardTeams[t];

	return 0;
}

//=============================================================================
// ScreenScoreBoard::GetSlotId
//=============================================================================

int ScreenScoreBoard::GetSlotId( int s )
{
	static const int ScoreBoardSlots[] =
	{
		ID_SCOREBOARD_PLAYER1,
		ID_SCOREBOARD_PLAYER2,
		ID_SCOREBOARD_PLAYER3,
		ID_SCOREBOARD_PLAYER4,
		ID_SCOREBOARD_PLAYER5,
		ID_SCOREBOARD_PLAYER6,
		ID_SCOREBOARD_PLAYER7,
		ID_SCOREBOARD_PLAYER8,
	};

	const int c = sizeof(ScoreBoardSlots)/sizeof(ScoreBoardSlots[0]);

	if( s >= 0 && s < c )
		return ScoreBoardSlots[s];
	
	return 0;
}

//=============================================================================
// ScreenScoreBoard::GetSlotRect
//=============================================================================

Sprite* ScreenScoreBoard::GetSlotRect( int t, int s )
{
	Sprite* pSlotRect = 0;

	if( ValidTeamSlot( t, s ) )
		pSlotRect = m_ScoreBoard[t].Slot[s].pSlotRect;
	
	return pSlotRect;
}

//=============================================================================
// ScreenScoreBoard::GetFriendIcon
//=============================================================================

Sprite* ScreenScoreBoard::GetFriendIcon( int t, int s )
{
	Sprite* pFriendIcon = 0;

	if( ValidTeamSlot( t, s ) )
		pFriendIcon = m_ScoreBoard[t].Slot[s].pFriendIcon;

	return pFriendIcon;
}


//=============================================================================
// ScreenScoreBoard::GetRatingIcon
//=============================================================================

Sprite* ScreenScoreBoard::GetRatingIcon( int t, int s )
{
	Sprite* pRatingIcon = 0;

	if( ValidTeamSlot( t, s ) )
		pRatingIcon = m_ScoreBoard[t].Slot[s].pRatingIcon;

	return pRatingIcon;
}

//=============================================================================
// ScreenScoreBoard::GetNameText
//=============================================================================

Text* ScreenScoreBoard::GetNameText( int t, int s )
{
	Text* pNameText = 0;

	if( ValidTeamSlot( t, s ) )
		pNameText = m_ScoreBoard[t].Slot[s].pNameText;

	return pNameText;
}


//=============================================================================
// ScreenScoreBoard::GetKillsText
//=============================================================================

Text* ScreenScoreBoard::GetKillsText( int t, int s )
{
	Text* pKillsText = 0;

	if( ValidTeamSlot( t, s ) )
		pKillsText = m_ScoreBoard[t].Slot[s].pKillsText;

	return pKillsText;
}


//=============================================================================
// ScreenScoreBoard::GetDeathsText
//=============================================================================

Text* ScreenScoreBoard::GetDeathsText( int t, int s )
{
	Text* pDeathsText = 0;

	if( ValidTeamSlot( t, s ) )
		pDeathsText = m_ScoreBoard[t].Slot[s].pDeathsText;

	return pDeathsText;
}


//=============================================================================
// ScreenScoreBoard::GetPointsText
//=============================================================================

Text* ScreenScoreBoard::GetPointsText( int t, int s )
{
	Text* pPointsText = 0;

	if( ValidTeamSlot( t, s ) )
		pPointsText = m_ScoreBoard[t].Slot[s].pPointsText;

	return pPointsText;
}

//=============================================================================
// ScreenScoreBoard::OnInitialize
//=============================================================================

void ScreenScoreBoard::OnInitialize( void )
{
	g_DisableControllerRemoved = false;

	ScreenBaseClass::OnInitialize();
	Sprite* pTeamRect;
	Sprite* pTeamTotals;
	Sprite* pSlotRect;
	int t, s;

	for( t = 0; t < TeamCount; t++ )
	{
		pTeamRect = GetSprite( GetTeamId(t) );
		pTeamTotals = GetSprite( ID_SCOREBOARD_TEAMTOTALS, pTeamRect );

		m_ScoreBoard[t].pTeamRect   = pTeamRect;
		m_ScoreBoard[t].pTeamTotals = pTeamTotals;
		m_ScoreBoard[t].pTeamName   = GetTextSprite( ID_SCOREBOARD_TEAMNAME, pTeamRect );
		m_ScoreBoard[t].pTeamKills  = GetTextSprite( ID_SCOREBOARD_KILLS, pTeamTotals );
		m_ScoreBoard[t].pTeamDeaths = GetTextSprite( ID_SCOREBOARD_DEATHS, pTeamTotals );
		m_ScoreBoard[t].pTeamPoints = GetTextSprite( ID_SCOREBOARD_POINTS, pTeamTotals );
		m_ScoreBoard[t].pWinning    = GetTextSprite( ID_SCOREBOARD_WINNER, pTeamTotals );

		for( s = 0; s < SlotCount; s++ )
		{
			pSlotRect = GetSprite( GetSlotId(s), pTeamRect );
			m_ScoreBoard[t].Slot[s].pSlotRect = pSlotRect;
			m_ScoreBoard[t].Slot[s].pFriendIcon = GetSprite( ID_SCOREBOARD_FRIENDICON, pSlotRect );
			m_ScoreBoard[t].Slot[s].pRatingIcon = GetSprite( ID_SCOREBOARD_RATINGICON, pSlotRect );
			m_ScoreBoard[t].Slot[s].pNameText   = GetTextSprite( ID_SCOREBOARD_NAME, pSlotRect );
			m_ScoreBoard[t].Slot[s].pKillsText  = GetTextSprite( ID_SCOREBOARD_KILLS, pSlotRect );
			m_ScoreBoard[t].Slot[s].pDeathsText = GetTextSprite( ID_SCOREBOARD_DEATHS, pSlotRect );
			m_ScoreBoard[t].Slot[s].pPointsText = GetTextSprite( ID_SCOREBOARD_POINTS, pSlotRect );
		}
	}

	m_pRankIcons    = GetSprite( ID_SCOREBOARD_RANKICONS );
	m_pRatingIcons  = GetSprite( ID_SCOREBOARD_RATINGICONS );
	m_pFriendIcons  = GetSprite( ID_SCOREBOARD_FRIENDICONS );
	m_pSpeakerIcons = GetSprite( ID_SCOREBOARD_SPEAKERICONS );

	ShowBk( true );

	m_ScoreUpdateHandler.Initialize( "ManDown", MESSAGE_HANDLE_IF_MASTER|MESSAGE_HANDLE_IF_DUPLICA, m_MessageOwner, this );
}

//=============================================================================
// ScreenScoreBoard::OnScreenIntro
//=============================================================================

void ScreenScoreBoard::OnScreenIntro( void )
{
	// clear any removed controller messages first
	g_ControllerRemoved = 0;
	g_FrontEnd.ShowErrorMessage( false );
	g_FrontEnd.m_ControllerDisconnected = false;

	g_DisableControllerRemoved = true;

	#if defined (_XBOX)
	CXboxLive::EndUIXFeature();
	#endif

	// kill any memcard actions
	g_MemCardManager->SetCancelOperation(true);

	ScreenBaseClass::OnScreenIntro();

	m_Tabulated = false;
	Compose();
}

//=============================================================================
// ScreenScoreBoard::OnScreenRefresh
//=============================================================================

void ScreenScoreBoard::OnScreenRefresh( void )
{
	ScreenBaseClass::OnScreenRefresh();

	if (!m_Tabulated)
		Compose();
}

//=============================================================================
// ScreenScoreBoard::Compose
//=============================================================================

void ScreenScoreBoard::Compose( void )
{
	Text* pTeamName;
	Text* pTeamKills;
	Text* pTeamDeaths;
	Text* pTeamPoints;
	Text* pWinning;
	Sprite* pFriendIcon;
	Sprite* pRatingIcon;
	Text* pNameText;
	Text* pKillsText;
	Text* pDeathsText;
	Text* pPointsText;
	Sprite* pSourceIcon;

	int nTeamKills;
	int nTeamDeaths;
	int nTeamScore;
	int nTeamStatus;
	int nKills;
	int nDeaths;
	int nScore;

	int Rank;
	int Rating;
	int Team;
	int Slot;
	int Player;
	int PlayerCount;
	int FriendIcon;
	int SpeakerIcon;
	
	// 5/7/05 we've crashed here on invalid ref... this is BAD, but avoidable.
	if (!ValidSingleton(Referee))
		return;

	Referee &ref = g_referee;
	CScoringModel &score = ref.GetScoringModel();
	CTeamingModel &teaming = ref.GetTeamingModel();

	nKills = 0;
	nDeaths = 0;
	nScore = 0;

	CPlayerAgent* pAgent;
	int PlayerTeam;
	int WinningTeam = -1;
	int WinningPlayer;

	int ScoreBoard;

	for( Team = 0; Team < TeamCount; Team++ )
	{
		ScoreBoard = score.IsTeamBlue( Team ) ? 1 : 0;

		if( ValidTeam(ScoreBoard) )
		{
			pTeamName   = m_ScoreBoard[ScoreBoard].pTeamName;
			pTeamKills  = m_ScoreBoard[ScoreBoard].pTeamKills;
			pTeamDeaths = m_ScoreBoard[ScoreBoard].pTeamDeaths;
			pTeamPoints = m_ScoreBoard[ScoreBoard].pTeamPoints;
			pWinning    = m_ScoreBoard[ScoreBoard].pWinning;

			PlayerCount = score.GetPlayerInfoCount();

			Slot = 0;

			// now populate it
			int total;
			Player = score.GetBestPlayerOnTeam( score.TOTAL_SCORE, Team, total );
			//while( (Player != score.INVALID_PLAYER) && (Slot < SlotCount) )
			while( Slot < SlotCount )
			{
				if( ValidSlot(Slot) )
				{
					pNameText   = m_ScoreBoard[ScoreBoard].Slot[Slot].pNameText;
					pKillsText  = m_ScoreBoard[ScoreBoard].Slot[Slot].pKillsText;
					pDeathsText = m_ScoreBoard[ScoreBoard].Slot[Slot].pDeathsText;
					pPointsText = m_ScoreBoard[ScoreBoard].Slot[Slot].pPointsText;
					pFriendIcon = m_ScoreBoard[ScoreBoard].Slot[Slot].pFriendIcon;
					pRatingIcon = m_ScoreBoard[ScoreBoard].Slot[Slot].pRatingIcon;

					pAgent = 0;
					PlayerTeam = 0;
					Rank = 0;
					Rating = 0; 

					if( Slot < PlayerCount )
					{
						pAgent = ref.GetPlayerAgent( Player );
						PlayerTeam = score.GetPlayerTeam( Player );
					}

					if( Player != score.INVALID_PLAYER && PlayerTeam == Team )
					{
						nKills  = score.GetPlayerCount(score.ENEMY_KILLS, Player);
						if (score.GetPlayerProfileVariant( Player ) == CPlayerProfile::COP)
							nKills += score.GetPlayerCount(score.APPREHENDS, Player);
						nDeaths = score.GetPlayerCount(score.ALL_DEATHS, Player);
						if (score.GetPlayerProfileVariant( Player ) == CPlayerProfile::GANG)
							nDeaths += score.GetPlayerCount(score.APPREHENDS, Player);
						nScore  = score.GetPlayerCount(score.TOTAL_SCORE, Player);

						// Distinguish dead/dropped players from living ones
						const char* name = score.GetPlayerName( Player );
						pNameText->SetText( name );
						if ( ((ref.GetPhase() == ref.PLAYING) && !ref.IsAlive( Player )) ||
								(pAgent == NULL) || strcmpi(name, pAgent->GetDisplayName()) != 0 )
							pNameText->SetAlpha( 0.6f );
						else
							pNameText->SetAlpha( 1.0f );

						pKillsText->Printf( "%d", nKills );
						pDeathsText->Printf( "%d", nDeaths );
						pPointsText->Printf( "%d", nScore );

						if( pAgent )
						{
							Rank = pAgent->GetRank();
							Rating = pAgent->GetStarRating();
						}
					}
					else
					{
						pNameText->SetText( 0 );
						pKillsText->SetText( 0 );
						pDeathsText->SetText( 0 );
						pPointsText->SetText( 0 );
					}

					if( pFriendIcon )
					{
						pSourceIcon = 0;

						// try for a speaker icon
						if( !pSourceIcon && m_pSpeakerIcons )
						{
							if( ref.IsParticipantVoiceReDirected(Player) )
								SpeakerIcon = ID_SCOREBOARD_ICONTVSPEAKER;
							else
							if( ref.IsParticipantHeadsetConnected(Player) )
								SpeakerIcon = ID_SCOREBOARD_ICONSPEAKER;
							else
								SpeakerIcon = 0;

							if( ref.IsParticipantTalking(Player) )
								pSourceIcon = GetSprite( SpeakerIcon, m_pSpeakerIcons );
						}

						// then try for a friend icon
						if( !pSourceIcon && m_pFriendIcons )
						{
							FriendIcon = ID_SCOREBOARD_ICONPLAYER;

							if( pAgent && pAgent->IsSessionHost() )
								FriendIcon = ID_SCOREBOARD_ICONHOST;
							else
							if( pAgent == ref.GetLocalPlayerAgent() )
								FriendIcon = ID_SCOREBOARD_ICONSELF;
							else
							{							
								// TODO: DETERMINE TYPE OF FRIEND ICON
								//       ID_SCOREBOARD_ICONFRIEND (confirmed friend)
								//       ID_SCOREBOARD_ICONFRIEND_SENT (sent a friend request to this guy)
								//       ID_SCOREBOARD_ICONFRIEND_REQUESTED (received a friend request from this guy)
							}

							pSourceIcon = GetSprite( FriendIcon, m_pFriendIcons );
						}

						// if we got a source icon, copy it and show it
						if( pSourceIcon )
						{
							float x, y;
							pFriendIcon->GetPosition( x, y );
							pFriendIcon->Copy( pSourceIcon );
							pFriendIcon->SetPosition( x, y );
							pFriendIcon->Show();
						}
						else
						{
							// otherwise hide the shit out of it
							pFriendIcon->SetTextureSet( INVALID_HANDLE, 0 );
							pFriendIcon->Hide();
						}
					}

					if( pRatingIcon )
					{
						pSourceIcon = 0;

						// try for a rank icon
						if( !pSourceIcon && m_pRankIcons )
							pSourceIcon = m_pRankIcons->GetNthChild(Rank-1);

						// then try for a rating icon
						if( !pSourceIcon && m_pRatingIcons )
							pSourceIcon = m_pRatingIcons->GetNthChild(Rating-1);

						// if we got a source icon, copy it and show it
						if( pSourceIcon )
						{
							float x, y;
							pRatingIcon->GetPosition( x, y );
							pRatingIcon->Copy( pSourceIcon );
							pRatingIcon->SetPosition( x, y );
							pRatingIcon->Show();
						}
						else
						{
							// otherwise hide the shit out of it
							pRatingIcon->SetTextureSet( INVALID_HANDLE, 0 );
							pRatingIcon->Hide();
						}
					}
				}

				Slot++;

				Player = score.GetNextBestPlayerOnTeam( score.TOTAL_SCORE, Player, total );
			}

			// get the team stats
			nTeamKills  = score.GetTeamCount( score.ENEMY_KILLS, Team );
			if (score.GetTeamProfileVariant( Team ) == CPlayerProfile::COP)
				nTeamKills += score.GetTeamCount(score.APPREHENDS, Team);
			nTeamDeaths = score.GetTeamCount( score.ALL_DEATHS, Team );
			if (score.GetTeamProfileVariant( Team ) == CPlayerProfile::GANG)
				nTeamDeaths += score.GetTeamCount(score.APPREHENDS, Team);
			nTeamScore  = score.GetTeamCount( score.TOTAL_SCORE, Team );
			nTeamStatus = score.GetTeamStatus( Team );

			// show the team name
			if( pTeamName )
				pTeamName->SetText( score.IsTeamBlue( Team ) ? teaming.GetBlueTeamName() : teaming.GetRedTeamName() );

			// show the team kills
			if( pTeamKills )
				pTeamKills->Printf( "%d", nTeamKills );

			// show the team deaths
			if( pTeamDeaths )
				pTeamDeaths->Printf( "%d", nTeamDeaths );

			// show the team score
			if( pTeamPoints )
				pTeamPoints->Printf( "%d", nTeamScore );

			// show who's winning and who's losing
			if( pWinning )
			{
				if( nTeamStatus == CScoringModel::WINNING )
					pWinning->SetText( "Winning" );
				else if( nTeamStatus == CScoringModel::LOSING )
					pWinning->SetText( "Losing" );
				else if( nTeamStatus == CScoringModel::TIED )
					pWinning->SetText( "Tied" );
				else if( nTeamStatus == CScoringModel::LOST )
					pWinning->SetText( "Lost" );
				else if( nTeamStatus == CScoringModel::WON )
					pWinning->SetText( "Won" );
			}

			if( nTeamStatus == score.WINNING || nTeamStatus == score.WON )
			{
				WinningTeam = Team;
				WinningPlayer = score.GetTeamMVP( Team );
			}
		}
	}

	int CurrentRound = g_referee.GetRoundCount();
	int TotalRounds = g_referee.GetRoundsInMatch();
	Printf( ID_SCOREBOARD_ROUND, "%d/%d", CurrentRound, TotalRounds );

	const char* pWinningTeam = ( WinningTeam != -1 ? (score.IsTeamBlue( WinningTeam ) ? 
		teaming.GetBlueTeamName() : teaming.GetRedTeamName()) : "None" );
	SetText( ID_SCOREBOARD_WINNINGTEAM, pWinningTeam );

	CPlayerAgent* mvpAgent = ref.GetPlayerAgent( score.GetOverallMVP() );
	const char* pWinningPlayer = mvpAgent ? mvpAgent->GetDisplayName() : "None";
	SetText( ID_SCOREBOARD_WINNINGPLAYER, pWinningPlayer );

	int RoundsWon = score.IsAlphaBlue() ? score.GetAlphaWins() : score.GetBetaWins();
	Printf( ID_SCOREBOARD_TEAMSCORE1, "%d/%d", RoundsWon, TotalRounds );

	RoundsWon = score.IsAlphaBlue() ? score.GetBetaWins() : score.GetAlphaWins();
	Printf( ID_SCOREBOARD_TEAMSCORE2, "%d/%d", RoundsWon, TotalRounds );

	if( score.IsTabulated() )
	{
		m_Tabulated = true;
		g_messageDispatcher.SendMessageToAll("ScoresTabulated", NULL,
				INVALID_OBJECT_HANDLE);
	}

	return;
}

//=============================================================================
// ScreenScoreBoard::OnScreenExit
//=============================================================================

void ScreenScoreBoard::OnScreenExit( void )
{
	/*
	GuiTable* pTable1 = (GuiTable*) GetInterface( ID_SCOREBOARD_TEAM1, GuiTable::ClassnameStatic() );
	GuiTable* pTable2 = (GuiTable*) GetInterface( ID_SCOREBOARD_TEAM2, GuiTable::ClassnameStatic() );

	for (int Slot = 0; Slot < 8; Slot++)
	{
		pTable1->SetItem( Slot, 0, "" );
		pTable1->SetItem( Slot, 1, "" );
		pTable1->SetItem( Slot, 2, "" );
		pTable1->SetItem( Slot, 3, "" );
		pTable1->SetItem( Slot, 4, "" );

		pTable2->SetItem( Slot, 0, "" );
		pTable2->SetItem( Slot, 1, "" );
		pTable2->SetItem( Slot, 2, "" );
		pTable2->SetItem( Slot, 3, "" );
		pTable2->SetItem( Slot, 4, "" );
	}
	*/

	g_DisableControllerRemoved = false;

	ScreenBaseClass::OnScreenExit();
}

//=============================================================================
// ScreenScoreBoard::ScoreUpdateHandler
//=============================================================================

void ScreenScoreBoard::ScoreUpdateHandler::HandleMessage( void* pData, ActorHandle pSender, bool Posted )
{
}
