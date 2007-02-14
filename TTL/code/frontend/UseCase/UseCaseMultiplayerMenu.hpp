///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseMultiplayerMenu.hpp
///
///  \brief Defines the MultiplayerMenu class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_MULTIPLAYER_MENU_
#define _INCLUDED_MULTIPLAYER_MENU_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"

///////////////////////////////////////////////////////////////////////////////////////
/// The MultiplayerMenu class.  This class handles the presentation
/// of the multiplayer choices to the user, and starting the appropriate
/// game choice.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class MultiplayerMenu : public UseCase, public Singleton<MultiplayerMenu>
    {
	    public:
		    enum State
            {
				GET_MESSAGE_OF_THE_DAY,
                MULTIPLAYER,
				ASK_TO_LOGOUT,
                LOGOUT,
#ifdef _XBOX
				UPDATE_MY_CLAN_INFO,
				UPDATE_MY_RANK_AND_RATING,
#endif // _XBOX
                NUM_STATES
            };
            
            /** \name UseCase public methods */
            //\{
            virtual bool update() { return _stateMachine.update(this); }
            //\}

			void SetAutomatedUseCaseLaunch( UseCase* useCase )	{ _automatedUseCaseLaunch = useCase; }
			UseCase* GetAutomatedUseCaseLaunch() const			{ return _automatedUseCaseLaunch; }

        protected:
            friend Singleton<MultiplayerMenu>;

            /** \name Constructor */
            //\{
            MultiplayerMenu();
            //\}

            /** \name Destructor */
            //\{
            virtual ~MultiplayerMenu() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
			virtual void				_onOpen();
            //\}

            /** \name State Machine update methods */
            //\{
#ifdef _XBOX
			void _enterUpdateMyClanInfo();
			bool _updateUpdateMyClanInfo();
			void _enterUpdateMyRankAndRating();
			bool _updateUpdateMyRankAndRating();
#endif // _XBOX

			void _enterMessageOfTheDay();
            bool _updateMessageOfTheDay();
            bool _updateMultiplayer();
			void _enterLogout();
            bool _updateLogout();
			void _enterAskToLogout();
            bool _updateAskToLogout();
            //\}

			void _ensureScreenIntegrity();
    	
	    private:
            /** \name Utility methods */
            //\{
            void _updateDescription();
            //\}

			UseCase* _automatedUseCaseLaunch;

            StateMachine<State, NUM_STATES, MultiplayerMenu> _stateMachine;  /// < State machine

			bool _onOpenCalled;
    };
}



#endif //_INCLUDED_MULTIPLAYER_MENU_