// ----------------------------------------------------------------------------
// File			-> NetMonitor.h
// Created		-> Jan 24, 2005
// Author		-> Dwain Skinner
// Description	-> Network Status Monitoring and error handling
// ----------------------------------------------------------------------------
#if defined(NETWORKING)
#ifndef _CNETMONITOR_H
#define _CNETMONITOR_H

#include "GameHelper/ErrorStack.h"
#include "GameHelper/Message.h"

class CNetMonitor
{
public:
	CNetMonitor										( void );
	~CNetMonitor									( );
	void initialize								( void );
	void terminate									( void );
	void update										( void );
	void showNetErrors							( void );
	inline unsigned int GetConnectionSilenceTime	( void );		// how long have we been silent is MS (not receiving)
	inline bool Enable							( bool i_enable );

	enum eErrorCodes
	{
		ERROR_NONE = 0,
		ERROR_LINK_DOWN,
		ERROR_TRANSMISSION_PROBLEMS,
		ERROR_DISCONNECTING,
		ERROR_DISCONNECTING_FATAL,
		ERROR_ABORTING,
	};

private:
	unsigned int									m_lostPacketsStartTime;
	unsigned int									m_updateTimer;
	unsigned int									m_lastErrorTime;
	int												m_errorCode;
	int												m_lastErrorCode;
	ErrorHandle										m_errorHandle;
	unsigned int									m_connectionSilence;
	bool												m_enabled;

	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER( CableErrorRetry, CNetMonitor ) d_cableErrorRetry;
	REF_SEND_MESSAGE_HANDLER( CableErrorCancel, CNetMonitor ) d_cableErrorCancel;
};

inline unsigned int CNetMonitor::GetConnectionSilenceTime( void )
{
	return( m_connectionSilence );
}

inline bool CNetMonitor::Enable( bool i_enable )
{
	// --- cannot turn off if we currently have an error, return failure
	if( !i_enable &&  m_errorCode != ERROR_NONE ) return false;

	m_enabled = i_enable;
	return( true );
}

extern CNetMonitor g_CNetMonitor;

#endif // _CNETMONITOR_H
#endif