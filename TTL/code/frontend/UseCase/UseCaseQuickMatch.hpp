///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseQuickMatch.hpp
///
///  \brief Defines the QuickMatch class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_QUICKMATCH_
#define _INCLUDED_QUICKMATCH_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"
#include "frontend/UseCase/GameSettingConfig.hpp"
#include "UseCaseDisplayDialog.hpp"
#include "Network/OnlineSessionSearch.h"

///////////////////////////////////////////////////////////////////////////////////////
/// The QuickMatch class.  This class handles the presentation
/// of the multiplayer choices to the user, and starting the appropriate
/// game choice.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class QuickMatch : public UseCase, public Singleton<QuickMatch>
    {
	    public:
		    enum State
            {
				GO_TO_EXECUTE_FIND,
				EXECUTE_FIND,
                SELECT_MATCH,
				NO_GAMES_FOUND,
                NUM_STATES
            };
            
            /** \name UseCase public methods */
            //\{
            virtual bool				update() { return _stateMachine.update(this); }
            //\}

        protected:
	        friend Singleton<QuickMatch>;
        
            /** \name Constructor */
            //\{
            QuickMatch();
            //\}

            /** \name Destructor */
            //\{
            virtual ~QuickMatch() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
            //\}

            /** \name State machine methods */
            //\{
			bool _updateGoToExecuteFind();

            void _enterNoGamesFound();
            bool _updateNoGamesFound();

			void _enterExecuteFind();
			bool _updateExecuteFind();

            bool _updateWaitForFind();

            void _enterSelectMatch();
            bool _updateSelectMatch();
            //\}
    	
	    private:
            StateMachine<State, NUM_STATES, QuickMatch> _stateMachine;  /// < State machine

			OSIMatchOptions		_matchOptions;
			OSIMatchList*		_matchList;
            int                 _currentMatchNum;
            // Game settings values
            StringType*			_mapNamePtr;
            EnumType*			_gameTypePtr;

			bool				_cleanUpScreens;
    };
}

#endif //_INCLUDED_QUICKMATCH_
