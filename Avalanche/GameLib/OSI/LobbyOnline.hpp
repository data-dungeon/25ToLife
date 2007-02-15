//----------------------------------------------------
// LobbyOnline.cpp -- Online lobby front-end.
//----------------------------------------------------

#ifndef _INCLUDED_LOBBY_ONLINE_
#define _INCLUDED_LOBBY_ONLINE_

#include "exampleui/input.hpp"
#include "defines.h"
#include <cstdio>

bool lobby_online_main();

// This class provides a front-end to the online lobby
// functionality.  It serves as a test bed for Ritual's
// LobbyServiceProvider Interface functions.
class LobbyOnline
{
    public:
		LobbyOnline() : _inputPtr(NULL)			{ }
        virtual			~LobbyOnline()			{ }

		bool			start( void );
        bool			mainloop( void );
        void			shutdown( void );

    protected:
		void			draw( void );
		void			getInput( void );
		bool			update( void );

		Input*			_inputPtr;
};

#endif //_INCLUDED_LOBBY_ONLINE_