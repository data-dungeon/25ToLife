//	----------------------------------------------------------------------------
// File			-> NetError.h
// Created		-> Jan 31, 2005
// Author		-> Dwain Skinner
// Description	-> platform independent way to let the system know about network errors
// ----------------------------------------------------------------------------


#ifndef _NETERROR_H
#define _NETERROR_H

class CNetError
{
public:
	enum eNetworkError
	{
		ERROR_NONE = 0,
		ERROR_CONNECTION_CREATE_FAILED,
		ERROR_SESSION_CREATE_FAILED,
		ERROR_SESSION_JOIN_FAILED,
		ERROR_SESSION_NAME_FAILED,
		ERROR_GATHERING_CREATE_FAILED,
		ERROR_REGISTER_GATHERING_FAILED,
		ERROR_WAIT_REGISTER_GATHERING_FAILED,
		ERROR_JOIN_GATHERING_FAILED,
		ERROR_WAIT_JOIN_GATHERING_FAILED,
		ERROR_LAUNCH_SESSION_FAILED,
		ERROR_WAIT_LAUNCH_SESSION_FAILED,
		ERROR_PLAYER_AGENT_CREATE_FAILED,


		// --- must be last
		MAX_ERRORS,
	};

	static void SetNetworkErrorReal( eNetworkError error, int displayTimeSecs, const char *file, int line );
	static eNetworkError GetLastError( void ) { return s_lastError; }
	static const char *GetErrorMessage( void );
	static int GetDisplayTime( void ) { return s_displayTime; }

protected:

private:
	static eNetworkError		s_lastError;
	static int					s_displayTime;			// in seconds
	static unsigned int		s_startDisplayTime;	// in millisecs
	static eNetworkError		s_lastDisplayedError;
	static const char *		s_file;
	static int					s_line;
};

#define SetNetworkError( a, b ) SetNetworkErrorReal( a, b, __FILE__, __LINE__ )

#endif // _NETERROR_H