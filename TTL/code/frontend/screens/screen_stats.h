#ifndef SCREEN_STATS_H
#define SCREEN_STATS_H

#include "frontend/screens/screen_BaseClass.h"
#include "OSI/OSI.hpp"

//=============================================================================
// ScreenStats
//=============================================================================

class ScreenStats : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenStats );

	public:
        enum ScreenStatus
        {
            RUNNING,
            NEXT_STAT,
            PREV_STAT,
			CHANGED_FILTER,
			CHANGED_SOURCE,
			PAGE_PLAYER_LIST_UP,
			PAGE_PLAYER_LIST_DOWN,
            BACK
        };

        enum StatFilter
        {
            STAT_FILTER_SHOW_ALL,
            STAT_FILTER_SHOW_COP_ONLY,
            STAT_FILTER_SHOW_GANGSTA_ONLY,
            NUM_STAT_FILTER
        };

		enum StatSource
		{
			STAT_SOURCE_PLAYERS,
			STAT_SOURCE_TOP_PLAYERS,
			STAT_SOURCE_CLAN,
			STAT_SOURCE_TOP_CLANS,
			STAT_SOURCE_FRIENDS,
			NUM_STAT_SOURCE
		};

        /** \name Accessors */
        //\{
        ScreenStatus	GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
		StatFilter		GetStatFilter() { return static_cast<StatFilter>(_currentStatFilter); }
		StatSource		GetStatSource() { return static_cast<StatSource>(_currentStatSource); }
		bool			IsWaitingForRows() { return _needsMoreData; }
		unsigned int	GetStatTableDisplayRows();
        //\}

        /** \name Mutators */
        //\{
        void SetStatTitle(const char *pTitle);
		void SetCurrentStatName(const char *pCurrentStatName);
        void SetUserName(const char *pName);
		void AddRow(const char *pUserName, float value);
        void AddRow(const char *pUserName, int value);
        void AddRow(const char *pUserName, const char *pValue);
        void FinishAddingRows(void);
        //\}

		ScreenStats();			

    private:
        /** \name Base class overrides */
        //\{
        virtual void OnInitialize(void);
        virtual void OnScreenIntro(void);
        virtual void OnCommand( int Command );
        virtual void OnButtonCancel() { _status = BACK; }
        virtual void OnCursorLeft(void);
		virtual void OnCursorRight(void);
        virtual void OnButtonPressed( int Button );
        virtual void OnScreenUpdate(void);
        //\}

        /** \name Utility methods */
        //\{
        void UpdateStatFilter(void);
        GuiTable* GetStatTable(void);
        void Page(int Amount);
        void ClearStats(void);
        void ShowWait(void);
        void HideWait(void);
        //\}

        ScreenStatus _status;       ///< Current screen status
        int _currentStatFilter;     ///< See enum StatFilter
		int _currentStatSource;		///< See enum StatSource
        bool _needsMoreData;		///< Do we need more data from the use case?
        bool _moreStats;            ///< Can we retrieve more stats from the use case?
        int _newStats;              ///< Number of new stats added by the use case
        bool _waitingForUpdate;     ///< We're waiting for a stats update
};

#endif // SCREEN_STATS_H
