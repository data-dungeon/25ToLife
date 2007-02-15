//------------------------------------------------
// LobbyOnline.cpp -- Implementation of the LobbyOnline
// class.

#include "LobbyOnline.hpp"
#include "exampleui/usecasemultiplayermenu.hpp"


LobbyOnline	theLobbyOnline ;

//------------------------------------------------
// This is a placeholder function for accessing the
// online lobbying functionality.  It simulates a
// main loop that displays a series of UI screens
// for handling online lobby services.  It returns
// once a match has been set up.
//-------------------------------------------------
bool lobby_online_main()
{
	theLobbyOnline.start();
	theLobbyOnline.mainloop();
	theLobbyOnline.shutdown();

	return true ;
}


///////////////////////////////////////////////////////////////////////////////////////
/// Start up function
///////////////////////////////////////////////////////////////////////////////////////
bool LobbyOnline::start()
{
	_inputPtr = new Input();
    _inputPtr->start();

//	MainMenu::getSingletonPtr()->start();
	MultiplayerMenu::getSingletonPtr()->start();
	return true ;
}


///////////////////////////////////////////////////////////////////////////////////////
/// Main Loop
///////////////////////////////////////////////////////////////////////////////////////
bool LobbyOnline::mainloop()
{
	bool stopRunning = false ;

		while ( !stopRunning )
		{
			getInput();
			stopRunning = update();
			draw();
		}

	return false ;
}


///////////////////////////////////////////////////////////////////////////////////////
/// Gets Input
///////////////////////////////////////////////////////////////////////////////////////
void LobbyOnline::getInput( void )
{
    _inputPtr->update();
}


///////////////////////////////////////////////////////////////////////////////////////
/// Updates the game
///////////////////////////////////////////////////////////////////////////////////////
bool LobbyOnline::update( void )
{
	UseCase* useCasePtr	= UseCase::getActiveUseCase();
	if ( !useCasePtr )	return true ;

	return useCasePtr->update( _inputPtr );
}


///////////////////////////////////////////////////////////////////////////////////////
/// Draws the world.  This handles start/end frame
/// duties, and calls the current use case's update
/// in between.
///////////////////////////////////////////////////////////////////////////////////////
void LobbyOnline::draw( void )
{
	UseCase* useCasePtr	= UseCase::getActiveUseCase();
	if ( !useCasePtr )	return ;

	// Render the scene
	useCasePtr->draw( );
}


///////////////////////////////////////////////////////////////////////////////////////
/// Shut down the application and free the instance pointer.
///////////////////////////////////////////////////////////////////////////////////////
void LobbyOnline::shutdown()
{

	// Shut down the input system
	if (_inputPtr)
	{
		_inputPtr->shutdown();
		delete _inputPtr;
	}
}


