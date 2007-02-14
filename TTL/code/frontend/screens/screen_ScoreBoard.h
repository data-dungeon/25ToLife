#ifndef _SCREEN_SCOREBOARD_H_
#define _SCREEN_SCOREBOARD_H_

#include "frontend/screens/screen_BaseClass.h"

class ScreenScoreBoard : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenScoreBoard );

	static const int SlotCount = 8;
	static const int TeamCount = 2;

	typedef struct
	{
		Sprite* pSlotRect;
		Sprite* pFriendIcon;
		Sprite* pRatingIcon;
		Text* pNameText;
		Text* pKillsText;
		Text* pDeathsText;
		Text* pPointsText;
	}
	ScoreBoardSlot;

	typedef struct
	{
		ScoreBoardSlot Slot[SlotCount];
		Sprite* pTeamRect;
		Sprite* pTeamTotals;
		Text* pTeamName;
		Text* pTeamKills;
		Text* pTeamDeaths;
		Text* pTeamPoints;
		Text* pWinning;
	}
	ScoreBoardTeam;

	public:
		ScreenScoreBoard() : m_MessageOwner( "ScreenScoreBoard" ) {}

		void ShowBk( bool Show );	
		bool Opaque( void );
		bool ValidTeam( int t );
		bool ValidSlot( int s );
		bool ValidTeamSlot( int t, int s );
		int GetTeamId( int t );
		Sprite* GetTeamRect( int t );
		int GetSlotId( int s );
		Sprite* GetSlotRect( int t, int s );
		Sprite* GetFriendIcon( int t, int s );
		Sprite* GetRatingIcon( int t, int s );
		Text* GetNameText( int t, int s );
		Text* GetKillsText( int t, int s );
		Text* GetDeathsText( int t, int s );
		Text* GetPointsText( int t, int s );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenRefresh( void );
		virtual void OnScreenExit( void );

		void Compose( void );

		CMessageOwner m_MessageOwner;
		REF_SEND_MESSAGE_HANDLER( ScoreUpdateHandler, ScreenScoreBoard )
			m_ScoreUpdateHandler;

		ScoreBoardTeam m_ScoreBoard[TeamCount];
		Sprite* m_pRankIcons;
		Sprite* m_pRatingIcons;
		Sprite* m_pFriendIcons;
		Sprite* m_pSpeakerIcons;
		bool m_Tabulated;
		bool m_Opaque;
};

#endif // _SCREEN_SCOREBOARD_H_
