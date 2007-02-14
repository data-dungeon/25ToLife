///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseHostMatch.hpp
///
///  \brief Defines the HostMatch class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_HOST_MATCH_
#define _INCLUDED_HOST_MATCH_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"


///////////////////////////////////////////////////////////////////////////////////////
/// The HostMatch class.  This class handles the presentation
/// of the multiplayer choices to the user, and starting the appropriate
/// game choice.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class HostMatch : public UseCase, public Singleton<HostMatch>
    {
	    public:
		    enum State
            {
                CONFIGURING,
				VALIDATE_NAME,
				HOSTING,
				PLAYING,
				CANCELLING,
                NUM_STATES
            };
            
            /** \name UseCase public methods */
            //\{
            virtual bool				update()						{ return _stateMachine.update(this); }
			void						prepareForPlaying( void );
            //\}

            CMessageOwner d_messageOwner;  /// < Loop can receive messages

        protected:
            friend class Singleton<HostMatch>;

            /** \name Constructor */
            //\{
            HostMatch();
            //\}

            /** \name Destructor */
            //\{
            virtual ~HostMatch() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
            //\}

            /** \name State machine methods */
            //\{
			void _enterValidateMatchName();
            bool _updateValidateMatchName();

            bool _updateConfiguring( void );
			bool _updateHosting( void );
            bool _updatePlaying( void );
			bool _updateCancelling( void );
            //\}
    	
	    private:
            StateMachine<State, NUM_STATES, HostMatch>             _stateMachine;             /// < State machine
			OSIMatchOptions                                        _matchOptions;             /// < Match options
            REF_SEND_MESSAGE_HANDLER(CreatingMatch, HostMatch)     _creatingMatchHandler;     /// < Match create message handler
            REF_SEND_MESSAGE_HANDLER(ValidateMatchName, HostMatch) _validateMatchNameHandler; /// < Validate match name message handler
            REF_SEND_MESSAGE_HANDLER(GameRules, HostMatch)         _gameRulesHandler;         /// < Game rules change message handler
            REF_SEND_MESSAGE_HANDLER(CancelConfig, HostMatch)      _cancelConfigHandler;      /// < Configuration screen cancel message handler
    };
}

#endif //_INCLUDED_HOST_MATCH_