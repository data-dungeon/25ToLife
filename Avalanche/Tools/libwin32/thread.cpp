//=============================================================================//
//  Copyright ©2001 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: thread.cpp                                                       //
//    Author: Daniel Silver                                                    //
//   Created: 11/19/2001                                                       //
//=============================================================================//

#include "pch.h"
#include "thread.h"

CThread StartupThread;

CThread *CThread::m_pFirstThread = 0;

CThread::CThread()
{
	if( !m_pFirstThread )
		m_pFirstThread = this;
	else
	{
		CThread *pThread = m_pFirstThread;

		while( pThread->m_pNextThread )
			pThread = pThread->m_pNextThread;

		pThread->m_pNextThread = this;
	}

	m_pNextThread = 0;

	SetThreadProperties( NULL, NULL, NULL, SW_SHOW );
	SetThreadArguments( 0, 0 );
	SetThreadName( "<no name>" );

	m_pfnThreadFunction = 0;
	m_bAutoDelete = FALSE;
}

CThread::~CThread()
{
	if( m_pFirstThread == this )
		m_pFirstThread = m_pNextThread;
	else
	{
		CThread *pThread = m_pFirstThread;

		while( pThread->m_pNextThread != this )
			pThread = pThread->m_pNextThread;

		pThread->m_pNextThread = m_pNextThread;
	}

	return;
}

CThread *CThread::CreateNewThread( const char *pThreadName, LPTHREAD_START_ROUTINE ThreadFunction, LPVOID pParam, BOOL Suspended )
{
	CThread *pThread = new CThread;

	if( pThread )
	{
		DWORD dwThreadId = 0xcdcdcdcd;
		HANDLE ThreadHandle = CreateThread( NULL, 0, CThread::ThreadFunction, pParam, CREATE_SUSPENDED, &dwThreadId );

		if( !ThreadHandle )
		{
			delete pThread;
			pThread = 0;
		}
		else
		{
			pThread->m_ThreadHandle = ThreadHandle;
			pThread->m_dwThreadId = dwThreadId;
			pThread->m_bAutoDelete = TRUE;
			pThread->m_pfnThreadFunction = ThreadFunction;

			pThread->SetThreadProperties( AppInstanceHandle(), 0, 0, 0 );
			pThread->SetThreadArguments( 0, 0 );
			pThread->SetThreadName( pThreadName );

			if( !Suspended )
				ResumeThread( ThreadHandle );
		}
	}

	return pThread;
}

CThread *CThread::GetThread( HANDLE ThreadHandle )
{
	CThread *pThread = m_pFirstThread;

	while( pThread )
	{
		if( pThread->GetThreadHandle() == ThreadHandle )
			return pThread;
	}

	return 0;
}

CThread *CThread::GetThread( DWORD dwThreadId )
{
	CThread *pThread = m_pFirstThread;

	while( pThread )
	{
		if( pThread->GetThreadId() == dwThreadId )
			return pThread;

		pThread = pThread->m_pNextThread;
	}

	return 0;
}

void CThread::SetThreadProperties( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	m_hInstance			= hInstance;
	m_hPrevInstance	= hPrevInstance;
	m_lpCmdLine			= lpCmdLine;
	m_nShowCmd			= nShowCmd;

	return;
}

void CThread::SetThreadArguments( int argc, char **argv )
{
	m_argc = argc;
	m_argv = argv;
}

void CThread::SetThreadName( const char *pThreadName )
{
	strncpy( m_szThreadName, pThreadName, sizeof(m_szThreadName) );
}

HANDLE CThread::GetThreadHandle( void )
{
	return m_ThreadHandle;
}

DWORD CThread::GetThreadId( void )
{
	return m_dwThreadId;
}

CThread * CreateNewThread( const char *pThreadName, LPTHREAD_START_ROUTINE ThreadFunction, LPVOID pParam, BOOL Suspended )
{
	return CThread::CreateNewThread( pThreadName, ThreadFunction, pParam, Suspended );
}

DWORD WINAPI CThread::ThreadFunction( LPVOID pParam )
{
	DWORD dwResult = 0;
	CThread *pThread = GetThread( GetCurrentThreadId() );

	if( pThread )
	{
		dwResult = pThread->m_pfnThreadFunction( pParam );

		if( pThread->m_bAutoDelete )
			delete pThread;
	}

	return dwResult;
}

extern int ToolMain( void );

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	StartupThread.SetThreadProperties( hInstance, hPrevInstance, lpCmdLine, nShowCmd );
	StartupThread.SetThreadName( "Startup Thread" );

	return ToolMain();
}