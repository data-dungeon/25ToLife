// ----------------------------------------------------------------------------
// File			-> NetMonitor.cpp
// Created		-> Jan 24, 2005
// Author		-> Dwain Skinner
// Description	-> Network Status Monitoring and error handling
// ----------------------------------------------------------------------------

// Precompiled header.
#include "TTLPCH.h"

#include "NetMonitor.h"
#include "Network/NetZ/NetZSession.h"
#include "Network/NetZ/NetZConnection.h"
#include "frontend/UseCase/UseCase.hpp"
#include "platform/MemoryCard/MemoryCard.h"

CNetMonitor g_CNetMonitor;

// ----------------------------------------------------------------------------
CNetMonitor::CNetMonitor( void )
{
	m_lastErrorCode = ERROR_NONE;
	m_errorCode = ERROR_NONE;
	m_errorHandle = ERROR_HANDLE_INVALID;
	m_updateTimer = 0;
	m_lostPacketsStartTime = 0;
	m_lastErrorTime = 0;
	m_connectionSilence = 0;
	m_enabled = true;
}

// ----------------------------------------------------------------------------
CNetMonitor::~CNetMonitor( void )
{
	if( m_errorHandle != ERROR_HANDLE_INVALID )
		g_errorStack.ClearError( m_errorHandle );
}

// ----------------------------------------------------------------------------
void CNetMonitor::initialize( void )
{
	m_enabled = true;
	d_cableErrorCancel.Initialize("CableErrorCancel", MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this );
	d_cableErrorRetry.Initialize("CableErrorRetry", MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this );
}

// ----------------------------------------------------------------------------
void CNetMonitor::terminate( void )
{
	m_enabled = false;
	d_messageOwner.UnregisterAll();
}

// ----------------------------------------------------------------------------
void CNetMonitor::update( void )
{
	#if defined(PS2) || defined(_XBOX)
	if ( OnlineUI::UseCase::pendingNetworkErrorClose() && OnlineUI::UseCase::allowUpdate() )
		OnlineUI::UseCase::setPendingNetworkErrorClose(false);
	
	if ( !OnlineUI::UseCase::allowUpdate() )
		OnlineUI::UseCase::setPendingNetworkErrorClose(true);

	OnlineUI::UseCase::setAllowUpdate(true);
	#endif


	// --- start fresh each time
	m_errorCode = ERROR_NONE;

	// --- if network error handling is disabled get out.
	if( !m_enabled ) return;

	// --- check the health of our connection last so we can override any other errors
	if( CNetZConnection::ConnectionUp() && g_MemCardManager->GetState() == CMemoryCard::CMEMORYCARD_IDLE )
	{
		// --- update the cable status
//		bool playing = false;
//		if( ValidSingleton(Referee) && g_referee.GetPhase() == Referee::PLAYING )
//			playing = true;

		uint32 curTime = g_timer.GetOSTime();

		// --- look for connection problems
		if( ValidSingleton( CSession ) )
		{
			// --- our we not receiving?
			if( ValidSingleton( CConnection ) && 
					g_session.GetNextRemoteHost( NULL ) &&
					g_connection.GetBPSReceivedCur() == 0 )
			{
				// --- lets keep track of how long since we received
				if( m_updateTimer == 0 )
				{
					m_updateTimer = curTime;
				}
				m_connectionSilence = curTime - m_updateTimer;

				bool linkStatus = CNetZConnection::GetLinkStatus();
				if( ( m_connectionSilence > 5000 && g_session.GetNumRemoteHosts() > 1 ) || !linkStatus )
				{
					m_lastErrorTime = curTime;
					m_errorCode = ERROR_TRANSMISSION_PROBLEMS;

					#if defined(PS2) || defined(_XBOX)
					OnlineUI::UseCase::setAllowUpdate(false);
					OnlineUI::UseCase::setPendingNetworkErrorClose(false);
					#endif
				}
				else
					m_connectionSilence = 0;
			}
			else
			{
				if( ( curTime - m_lastErrorTime ) > ( 1000 * 1 ) )
				{
					m_updateTimer = 0;
					m_lastErrorTime = 0;
					m_connectionSilence = 0;
				}
			}

			// --- try and determine if we are having connection problems due to a slow connection
			// --- or poor connection
			if( ValidSingleton( CConnection ) && m_errorCode == ERROR_NONE &&
				CNetZConnection::s_netSettings.d_connectionStabilityEnabled &&
				g_session.GetNumRemoteHosts() >= CNetZConnection::s_netSettings.d_connectionStabilityMinPlayers  )
			{
				uint32 totalRetransmit = g_connection.GetReliableRetransmittedTotal();
				uint32 totalReliable = g_connection.GetReliableTransmittedTotal() + totalRetransmit; 

				totalReliable = ( totalReliable > 0 ) ? totalReliable : 1;

				// --- are we exceeding our lost packets threshold?
				if( ( ( float )totalRetransmit / ( float )totalReliable ) > 
					CNetZConnection::s_netSettings.d_connectionStabilityResentThreshold )
				{
					if( m_lostPacketsStartTime == 0 )
						m_lostPacketsStartTime = curTime;

					uint32 unstableTime = curTime - m_lostPacketsStartTime;

					// --- have we met our minimum amount of time that we must maintain this threshold?
					if( unstableTime > ( CNetZConnection::s_netSettings.d_connectionStabilityMinThreshold * 1000 ) )
					{
						m_errorCode = ERROR_TRANSMISSION_PROBLEMS;
						#if defined(PS2) || defined(_XBOX)
						OnlineUI::UseCase::setAllowUpdate(false);
						OnlineUI::UseCase::setPendingNetworkErrorClose(false);
						#endif
					}

					// --- have we met our maximum amount of time we can sustain losing packets before we kick the player?
					if( unstableTime > ( CNetZConnection::s_netSettings.d_connectionStabilityMaxThreshold * 1000 ) )
					{
						m_errorCode = ERROR_DISCONNECTING_FATAL;
						#if defined(PS2) || defined(_XBOX)
						OnlineUI::UseCase::setAllowUpdate(false);
						OnlineUI::UseCase::setPendingNetworkErrorClose(false);
						#endif
					}
				}
				else
				{
					m_lostPacketsStartTime = 0;
				}
			}

			// --- did a fault occur?
			if( !g_session.GetNextRemoteHost( NULL ) && g_session.GetFaultStatus() && m_lastErrorTime  )
			{
				m_errorCode = ERROR_DISCONNECTING;

				#if defined(PS2) || defined(_XBOX)
				OnlineUI::UseCase::setAllowUpdate(false);
				OnlineUI::UseCase::setPendingNetworkErrorClose(false);
				#endif
			}
		}

		// --- DAS, only check link status if we are not in the game or transmission errors are happening
		if( m_errorCode != ERROR_TRANSMISSION_PROBLEMS && !CNetZConnection::GetLinkStatus() )
		{
			if( m_lastErrorCode != ERROR_ABORTING )
			{
				m_errorCode = ERROR_LINK_DOWN;

#if defined(PS2) || defined(_XBOX)
				OnlineUI::UseCase::setAllowUpdate(false);
				OnlineUI::UseCase::setPendingNetworkErrorClose(false);
#endif
			}
			else m_errorCode = ERROR_ABORTING;
		}
	}

	// --- Show any errors
	showNetErrors( );
}

// ----------------------------------------------------------------------------
void CNetMonitor::showNetErrors( )
{
	if( m_lastErrorCode == m_errorCode )
		return;

	if( m_errorHandle != ERROR_HANDLE_INVALID )
		g_errorStack.ClearError( m_errorHandle );

	switch( m_errorCode )
	{
		case ERROR_NONE:
			m_errorHandle = ERROR_HANDLE_INVALID;
			break;

		case ERROR_LINK_DOWN:
			if( g_regionUSA )
			{
				m_errorHandle = g_errorStack.SetError(
						"Network",
						"Network cable unplugged. . .",
						CErrorStack::ERROR_WARNING,
						ERROR_INFINITE_LIFETIME );
				g_errorStack.SetActionMessagePair( m_errorHandle, "Retry", "CableErrorRetry");
				g_errorStack.SetActionMessagePair( m_errorHandle, "Abort", "CableErrorCancel");
			}
			else
			{
				m_errorHandle = g_errorStack.SetError(
					"Network",
					"The Network cable is not connected.  Please check your network cable and try again.",
					CErrorStack::ERROR_WARNING,
					ERROR_INFINITE_LIFETIME );
				g_errorStack.SetActionMessagePair( m_errorHandle, "Retry", "CableErrorRetry");
				g_errorStack.SetActionMessagePair( m_errorHandle, "Abort", "CableErrorCancel");
			}
			break;

		case ERROR_TRANSMISSION_PROBLEMS:
			m_errorHandle = g_errorStack.SetError(
					"Network",
					"Connection Problems Detected. . .",
					CErrorStack::ERROR_WARNING,
					ERROR_INFINITE_LIFETIME );
			break;

		case ERROR_DISCONNECTING:
			m_errorHandle = g_errorStack.SetError(
					"Network",
					"Disconnecting from game. . .",
					CErrorStack::ERROR_WARNING,
					ERROR_INFINITE_LIFETIME );
			break;

		case ERROR_DISCONNECTING_FATAL:
			m_errorHandle = g_errorStack.SetError(
				"Network",
				"Disconnecting from game. . .",
				CErrorStack::ERROR_FATAL,
				ERROR_INFINITE_LIFETIME );
			break;

		case ERROR_ABORTING:
			m_errorHandle = g_errorStack.SetError(
					"Network",
					"Shutting network down. . . ",
					CErrorStack::ERROR_FATAL,
					ERROR_INFINITE_LIFETIME );
			break;
	}
	m_lastErrorCode = m_errorCode;
}

//============================================================================
// --- message handlers
void CNetMonitor::CableErrorCancel::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	// --- cable disconnect becomes ERROR_FATAL if user wants to abort
	//g_errorStack.SetErrorSeverity( d_ref->m_errorHandle, CErrorStack::ERROR_FATAL );

	ScreenMemCard* pMemCard = g_FrontEnd.GetMemCardScreen();
	if( pMemCard )
	{
			pMemCard->SetResult( MC_QUIT );
	}
	g_MemCardManager->SetPopupExitResult( PR_QUIT );
	g_MemCardManager->SetState( CMemoryCard::CMEMORYCARD_IDLE );
	d_ref->m_errorCode = ERROR_ABORTING;
	d_ref->showNetErrors();
}

void CNetMonitor::CableErrorRetry::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	g_errorStack.ClearError( d_ref->m_errorHandle );
	d_ref->m_lastErrorCode = ERROR_NONE;
}

