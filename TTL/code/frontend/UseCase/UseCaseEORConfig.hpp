///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseEORConfig.hpp
///
///  \brief Defines the EORConfig class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_EOR_CONFIG_
#define _INCLUDED_EOR_CONFIG_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"
#include "Network/NetZ/NetZConnection.h"

#ifndef DIRECTX_PC
#include <platform/stack/ps2common/ps2networking.h>
#include <platform/stack/ps2common/ps2profilelist.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////
/// The EORConfig class.  This class handles the presentation
/// of the multiplayer choices to the user, and starting the appropriate
/// game choice.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class EORConfig : public UseCase, public Singleton<EORConfig>
    {
	    public:
		    enum State
            {
                NET_CONFIG,
                SANDBOX,
                CONNECT,
                DNAS,
                TERMINATE,
                NUM_STATES
            };
            
            /** \name UseCase public methods */
            //\{
            virtual bool				update() { return _stateMachine.update(this); }
            //\}

        protected:
	        friend Singleton<EORConfig>;
        
            /** \name Constructor */
            //\{
            EORConfig();
            //\}

            /** \name Destructor */
            //\{
            virtual ~EORConfig() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
            //\}

            /** \name State machine methods */
            //\{
            bool _updateNetConfig();
            bool _updateSandbox();
            bool _updateConnect();
            bool _updateTerminate();
            bool _updateDnas();
            void _dnasEntry();
            //\}
    	
	    private:
            StateMachine<State, NUM_STATES, EORConfig> _stateMachine;  /// < State machine
			bool _dnasRunning;

//            static PS2ProfileList _profileList; /// < List of network profiles
    };
}

#endif //_INCLUDED_EOR_CONFIG_
