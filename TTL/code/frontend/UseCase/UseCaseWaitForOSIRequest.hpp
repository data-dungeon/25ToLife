///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseWaitForOSIRequest.hpp
///
///  \brief Defines the WaitForOSIRequest class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_WAIT_FOR_OSI_REQUEST_
#define _INCLUDED_WAIT_FOR_OSI_REQUEST_

#include "UseCase.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"

#include "OSI/OSI.hpp"

///////////////////////////////////////////////////////////////////////////////////////
/// The WaitForOSIRequest class.  This class handles waiting on an OSI request.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class WaitForOSIRequest : public UseCase, public Singleton<WaitForOSIRequest>
    {
	    public:
		    enum State
            {
				UNINITIALIZED,

				OSI_REQUEST_PUMP,	// Pump current OSI request until it finishes
				FATAL_ERROR,		// Fatal error occurred, wait for external shutdown

                NUM_STATES
            };
            
            /** \name UseCase public methods */
            //\{
			void start( bool requestStatus, const char *pTitle, const char *pText, bool displayError = true );
            virtual bool update() { return _stateMachine.update(this); }
            //\}

        protected:
	        friend Singleton<WaitForOSIRequest>;
        
            /** \name Constructor */
            //\{
            WaitForOSIRequest();
            //\}

            /** \name Destructor */
            //\{
            virtual ~WaitForOSIRequest() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool _init();
            virtual bool _deinit();
            //\}

            /** \name State Machine methods */
            //\{
            bool _updateOSIRequestPump();
			bool _updateFatalError();
			//\}
	    private:
            StateMachine<State, NUM_STATES, WaitForOSIRequest> _stateMachine;  /// < State machine

			bool _displayWait;  /// < Display wait screen?
			bool _displayError; /// < Display screens on errors?
    };
}

#endif //_INCLUDED_WAIT_FOR_OSI_REQUEST_