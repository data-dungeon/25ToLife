#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "UseCaseEORPassword.hpp"
#include "UseCaseDisplayDialog.hpp"
#include "../screens/screen_wait.h"
#include "../screens/screen_Dialog.h"
#include "frontend/frontend.h"
#include "UseCaseUtility.hpp"

//XBox use cases
#ifdef _XBOX
#include "frontend/UseCase/Xbox/UseCaseUIXLogin.hpp"
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::EORPassword::EORPassword() : 
    _stateMachine(MAKE_REQUEST)
{
    _stateMachine.setStateUpdate(MAKE_REQUEST, &OnlineUI::EORPassword::_updateMakeRequest);
    _stateMachine.setStateEnter(SUCCESS, &OnlineUI::EORPassword::_enterSuccess);
    _stateMachine.setStateEnter(FAILURE, &OnlineUI::EORPassword::_enterFailure);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EORPassword::_init( void )
{
    //OUITODO: Add OSI call to recover EORPassword here...

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Do any needed clean up.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EORPassword::_deinit( void )
{
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when updating the MAKE_REQUEST state
///
/// \return true to continue processing, false to exit the use case stack
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORPassword::_updateMakeRequest()
{
    //OUITODO: Remove clock stuff when adding OSI call
    static float Clock = 0.0f;

    Clock += g_timer.GetOSFrameSec();

    if (Clock > 2.0f)
    {
        //OUITODO: Check OSI call return status.  If successful:
        _stateMachine.setCurrentState(this, SUCCESS);
        //else
        //_stateMachine.setCurrentStat(this, FAILURE);
        Clock = 0.0f;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the SUCCESS state
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORPassword::_enterSuccess()
{
    g_FrontEnd.EndScreen(0);
    stop();
    _stateMachine.setCurrentState(this, MAKE_REQUEST);
    DisplayDialog::getSingletonPtr()->start("Eidos Online Account Login", "An email has been sent to the email \
                                                                           address used when creating your \
                                                                           online name.");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the FAILURE state
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORPassword::_enterFailure()
{
    g_FrontEnd.EndScreen(0);
    _stateMachine.setCurrentState(this, MAKE_REQUEST);
    stop();
    DisplayDialog::getSingletonPtr()->start("Eidos Online Account Login", OSI::getSingletonPtr()->getErrorString());
}
