#ifndef _SCREEN_BRIEFING_H_
#define _SCREEN_BRIEFING_H_

//=============================================================================
//		This file contains the implementation for the briefing screens displayed
//		at the beginning of each single player mission.
//=============================================================================

#include "frontend/screens/screen_BaseClass.h"
#include "weapons/ttlweapon.h"

class ScreenBriefing : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenBriefing );

	public:
		void SetPrimaryObjective( int n, const char* pObjective );
		void SetSecondaryObjective( int n, const char* pObjective );
		int GetPrimaryId( int n );
		int GetSecondaryId( int n );
		void InitThumbnail( void );
		void KillThumbnail( void );
		void InitLevel( void );
		void InitObjectives( void );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual void OnButtonOK( void );
#ifdef DIRECTX_PC
		virtual void OnScreenUpdate( void );
#endif

		t_Handle m_BriefingImage;
};

class ScreenStatsSummary : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenStatsSummary );

	public:
		void SetStatistic( int n, const char* pStat, const char* pValue );
		Text* GetStatistic( int n );
		Text* GetValue( int n );
		int GetStatId( int n );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnButtonOK( void );
};

class ScreenObjectiveSummary : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenObjectiveSummary );

	public:
		void SetObjective( int n, const char* pText );
		void SetObjectiveCompleted( int n, bool Completed );
		void SetReward( int n, const char* pText );

		int GetObjectiveId( int n );
		Text* GetObjectiveText( int n );
		Text* GetRewardText( int n );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnButtonOK( void );
};

#endif // _SCREEN_BRIEFING_H_
