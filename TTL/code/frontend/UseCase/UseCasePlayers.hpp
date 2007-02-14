///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCasePlayers.hpp
///
///  \brief Defines the Players class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_PLAYERS_
#define _INCLUDED_PLAYERS_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"


///////////////////////////////////////////////////////////////////////////////////////
/// The Players class.  This class handles the presentation
/// of the multiplayer choices to the user, and starting the appropriate
/// game choice.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class Players : public UseCase, public Singleton<Players>
    {
	    public:
		    enum State
            {
				MENU_PLAYERS_LIST,
				MENU_PLAYERS_LIST_POPUP,

				OSI_REQUEST_CLAN_INFO,
				OSI_REQUEST_FRIENDSHIP,
				OSI_REQUEST_SEND_CLAN_INVITE,
				OSI_REQUEST_SEND_MATCH_INVITE,
                NUM_STATES
            };
            
            /** \name UseCase public methods */
            //\{
            virtual bool				update()								{ return _stateMachine.update(this); }
			virtual void				start( void );
			virtual void				start( bool useCurrentPlayers );
			OSIPlayer*					getSelectedPlayer( void )				{ return _selectedPlayer ; }
            //\}

        protected:
            friend class Singleton<Players>;

            /** \name Constructor */
            //\{
            Players();
            //\}

            /** \name Destructor */
            //\{
            virtual ~Players() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool		_init();
            virtual bool		_deinit();
			virtual bool		_updateDisplayPlayersMenu();
			void				_convertSelectedPlayerIndexIntoSelectedPlayer( int selectedPlayerIndex );


			void _enterPlayerPopup();
			bool _updatePlayerPopup();

			void _enterOSIClanInfo();
			bool _updateOSIClanInfo();

			void _enterOSIInviteFriend();
			bool _updateOSIInviteFriend();

			void _enterRequestInviteToMatch( void );
			bool _updateRequestInviteToMatch( void );

			void _enterOSIInviteClan();
			bool _updateOSIInviteClan();
            //\}
    	
	    private:
            StateMachine<State, NUM_STATES, Players> _stateMachine;  /// < State machine
			OSIPlayer*					_selectedPlayer ;
			bool						_useCurrentPlayers;
			OSIPlayerList				_playerList;
    };
}

#endif //_INCLUDED_PLAYERS_