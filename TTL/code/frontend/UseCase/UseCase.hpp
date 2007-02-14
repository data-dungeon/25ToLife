///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCase.hpp
///  \brief Defines the base UseCase class
///  which provides common interface for running use cases.
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_USE_CASE_
#define _INCLUDED_USE_CASE_

#include "container/sstack.h"
#include "frontend/frontend.h"

#if defined( _XBOX ) && defined( _DEBUG )
#define printf XBUtil_DebugPrint
#endif

///////////////////////////////////////////////////////////////////////////////////////
/// A UseCase defines a specific portion of a game, such
/// as finding a match, hosting a match, playing the game,
/// changing options, etc.  This is an abstract base class.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class UseCase;

    ///////////////////////////////////////////////////////////////////////////////////////
    /// Quick and dirty UseCase Stack class
    ///////////////////////////////////////////////////////////////////////////////////////
    typedef DynamicStack<OnlineUI::UseCase*> UseCaseStack;

    class UseCase
    {
        public:
            /** \name Destructor */
            //\{
            virtual ~UseCase( void )	{ }
            //\}

		    /** \name Accessors */
		    //\{
		    bool						isRunning( void )				{ return _running; }
            static bool                 allowUpdate()                   { return _allowUpdate; }
            static bool                 pendingNetworkErrorClose()      { return _pendingNetworkErrorClose; }
		    //\}

		    /** \name Mutator */
		    //\{
		    void						setRunning( bool running )		          { _running = running; }
            static bool                 setAllowUpdate(bool allow)                { _allowUpdate = allow; return true; }
            static bool                 setPendingNetworkErrorClose(bool pending) { _pendingNetworkErrorClose = pending; return true; }
			template < class POPUP >
			POPUP*						expectPopup();
			template < class SCREEN >
			SCREEN*						expectScreen();
		    //\}

            /** \name UseCase methods */
            //\{
		    virtual bool				update() = 0 ;

		    void				        start( void );
		    void						stop( void );
            //\}

		    static UseCase*				getActiveUseCase();
			static void					popThroughUseCase( UseCase* useCase );
			
#ifdef _DEBUG
			static const char*			getNameForUseCase( const OnlineUI::UseCase* useCase );
			static void					dumpUseCaseStack();
#endif // _DEBUG

            /** \name Utility methdos */
            //\{
            template <typename SCREEN>
            static SCREEN* GetActiveScreen();

            template <typename SCREEN>
            static bool IsScreenActive();

			static ScreenKeyboard* ActivateKeyboard( const char* displayText, int maxLength = 0, const char* restrictChars = "" );
			static ScreenKeyboard* ActivateKeyboardForNames( const char* displayText, int maxLength = 15 );
			static ScreenKeyboard* ActivateKeyboardForPasswords( const char* displayText, int maxLength = 15 );
			static ScreenKeyboard* ActivateKeyboardForClanNames( const char* displayText, int maxLength = 15 );
			static ScreenKeyboard* ActivateKeyboardForEmail( const char* displayText, int maxLength = 0 );
			static ScreenKeyboard* ActivateKeyboardForPostalCode( const char* displayText, int maxLength = 9 );
            //\}

        protected:
		    /** \name Constructor */
		    //\{
		    UseCase( void );
		    //\}

            virtual bool				_init() = 0;
            virtual bool				_deinit() = 0;
		    virtual void				_onOpen() { }
            virtual void                _onPause() { }
    		
	    private:
		    static UseCaseStack			_useCaseStack;
		    bool						_running;
            static bool                 _allowUpdate;
			static bool                 _pendingNetworkErrorClose;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the active screen on the stack and guarantee that the screen on the stack is the same type
    /// of screen that we requested.
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename SCREEN>
    /*static*/ SCREEN* UseCase::GetActiveScreen()
    {
		if (!g_FrontEnd.GetActiveScreen()) return NULL ;

        ScreenHandler* pHandler = g_FrontEnd.GetActiveScreenHandler();
        // Make sure we have a valid screen handler
        if( pHandler == 0 )
		{
			return 0;
		}

        // Make sure screen has right name
        if (strcmp(pHandler->Classname(), SCREEN::ClassnameStatic()) != 0)
        {
            printf("Expected screen: %s.  Found screen: %s.\n", SCREEN::ClassnameStatic(), pHandler->Classname());
        }
        if (strcmp(pHandler->Classname(), SCREEN::ClassnameStatic()) == 0)
        {
            // If we have the right screen, return it.
            return reinterpret_cast<SCREEN *>(pHandler);
        }
        else
        {
            // If we have the wrong screen and the user continued past the assert,
            // return NULL and force a crash. >:)
            return 0;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /// See if the screen on top of the stack is the type of screen in the template parameter.
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename SCREEN>
    /*static*/ bool UseCase::IsScreenActive()
    {
        ScreenHandler* pHandler = g_FrontEnd.GetActiveScreenHandler();

        // Make sure we have a valid screen handler
        if( pHandler == 0 )
		{
			return false;
		}

        if (strcmp(pHandler->Classname(), SCREEN::ClassnameStatic()) == 0)
        {
            return true;
        }
        return false;
    }

	///////////////////////////////////////////////////////////////////////////////////////////////////
    /// Expect POPUP as the active screen on the stack.  If POPUP is not active, stop the use case
	/// if there are no popups.
    ///////////////////////////////////////////////////////////////////////////////////////////////////
	template < class POPUP >
	POPUP* UseCase::expectPopup()
	{
		POPUP* pPopup = UseCase::GetActiveScreen< POPUP >();
		if( pPopup == NULL )
		{
			// Our expected popup is not active, has another popup stomped us?
			Screen* pScreen = g_FrontEnd.GetCurrentScreen();
			if( pScreen == NULL || pScreen->GetActivePopup() == NULL )
			{
				// No popup stomped us, we're in trouble, abort!
				stop();
			}
		}
		return pPopup;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
    /// Expect SCREEN as the active screen on the stack.  If SCREEN is not active, stop the use case
	/// if there are no popups.
    ///////////////////////////////////////////////////////////////////////////////////////////////////
	template < class SCREEN >
	SCREEN* UseCase::expectScreen()
	{
		SCREEN* pScreen = UseCase::GetActiveScreen< SCREEN >();
		if( pScreen == NULL )
		{
			// Our expected screen is not active, is there a popup on top of us?
			Screen* pCurrentScreen = g_FrontEnd.GetCurrentScreen();
			if( pCurrentScreen == NULL || pCurrentScreen->GetActivePopup() == NULL )
			{
				// There are no popups up and this is not our expected screen, abort!
				stop();
			}
		}
		return pScreen;
	}
}

#endif //INCLUDED_USE_CASE_