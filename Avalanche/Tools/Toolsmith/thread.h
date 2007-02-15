//=============================================================================//
//  Copyright ©2001 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: thread.h                                                         //
//    Author: Daniel Silver                                                    //
//   Created: 11/19/2001                                                       //
//=============================================================================//

#ifndef __THREAD_H__
#define __THREAD_H__

#include <windows.h>

class CThread
{
	public:
		CThread();
		virtual ~CThread();

		static CThread *CreateNewThread( const char *pThreadName, LPTHREAD_START_ROUTINE ThreadFunction, LPVOID pParam, BOOL Suspended );
		static CThread *GetThread( HANDLE ThreadHandle );
		static CThread *GetThread( DWORD dwThreadId );
		
		void SetThreadProperties( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd );
		void SetThreadArguments( int argc, char **argv );
		void SetThreadName( const char *pThreadName );
		HANDLE GetThreadHandle( void );
		DWORD GetThreadId( void );
		
		char		 m_szThreadName[32];
		HINSTANCE m_hInstance;
		HINSTANCE m_hPrevInstance;
		LPSTR     m_lpCmdLine;
		int       m_nShowCmd;
		int		 m_argc;
		char		 **m_argv;
		HANDLE	 m_ThreadHandle;
		DWORD		 m_dwThreadId;

	private:
		static CThread *m_pThreads;
		CThread *m_pNext;
		BOOL    m_bAutoDelete;

		LPTHREAD_START_ROUTINE m_pfnThreadFunction;

		static DWORD WINAPI ThreadFunction( LPVOID pParam );
};

extern CThread StartupThread;

inline HINSTANCE AppInstanceHandle( void ) { return StartupThread.m_hInstance; }
inline LPSTR AppCommandLine( void ) { return StartupThread.m_lpCmdLine; }
inline int AppShowCommand( void ) { return StartupThread.m_nShowCmd; }

CThread * CreateNewThread( const char *pThreadName, LPTHREAD_START_ROUTINE ThreadFunction, LPVOID pParam, BOOL Suspended );

#ifdef _AFX
#ifndef _TOOLMAIN
#define _TOOLMAIN

extern int AFXAPI AfxWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow);

int ToolMain( void )
{
	return AfxWinMain( StartupThread.m_hInstance, StartupThread.m_hPrevInstance, StartupThread.m_lpCmdLine, StartupThread.m_nShowCmd );
}

#endif // _TOOLMAIN
#endif // _AFX

#endif // __THREAD_H__