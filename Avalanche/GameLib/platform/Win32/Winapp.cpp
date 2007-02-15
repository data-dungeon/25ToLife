//*************************************************************************
// Windows application framework.  aka: Poor man's MFC. (or Portable man's
// mfc...)
//
// (c) Copyright 1996, Jeffrey Gosztyla.
//*************************************************************************

/******************* includes ****************************/

#include "platform/PlatformPCH.h"

// Initialize our statics
WNDPROC WindowsApp::windowProc = (WNDPROC)WindowsApp::WinAppWindowProc;

//*************************************************************************
// Here it is, our very own WinMain
//*************************************************************************
int PASCAL WinMain(
HINSTANCE hInstance,
HINSTANCE hPrevInst,
LPSTR cmdLine,
int nCmdShow)
{
	SetThreadAffinityMask(GetCurrentThread(), 1<<0);

#ifndef DISABLE_ASSERTS
	// switch to the generic dialog asserts...
	Assert::SetHandler(Assert::CrtReport);
#endif

	MSG  msg;
	WindowsApp *winApp = WindowsApp::IAm();

	// Has something gone seriously wrong?
	if (winApp == NULL)
		return(0);

	// Init our instance
	winApp->SethInst(hInstance);

#ifndef DISABLE_ASSERTS
	// switch to our own dialog asserts...
	// it needs hInstance to work...
	Assert::SetHandler(Assert::Dialog);
#endif

	// initialize the winapp...
	if (!winApp->InitInstance(hPrevInst, cmdLine, nCmdShow))
		return(0);

	// returns TRUE if game is at end state
	for (;;)
	{
		// Allow messages to process
		if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			winApp->Process();  // Do stuff here!
		else
		{
			// If get message returns FALSE we are quitting
			if (!GetMessage(&msg, NULL, 0, 0))
				break;

			// Go, go, go!
			if (!winApp->HandleMessage(msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	// cleanup, prepare to exit
	winApp->TerminateInstance();
	winApp->OnDestroy();

	// Exit
	return(msg.wParam);
}

//*************************************************************************
// We are quitting!
//*************************************************************************
void WindowsApp::Quit(void)
{
	if (hWnd)
		DestroyWindow(hWnd);
	else
		PostQuitMessage(0);

	hWnd = NULL;
}

//*************************************************************************
// Our windows proc
//*************************************************************************
LONG PASCAL WindowsApp::WinAppWindowProc(
HWND hWnd,
UINT msg,
WPARAM wParam,
LPARAM lParam)
{
	WindowsApp *winApp = WindowsApp::IAm();

	// Save these
	winApp->hWnd = hWnd;
	winApp->saveMsg = msg;
	winApp->savewParam = wParam;
	winApp->savelParam = lParam;

	// Let the message watch have a peek
	winApp->MessageWatch(msg, wParam, lParam);

	switch (msg)
	{
		case WM_CREATE:
			// Call the create method
			return(winApp->OnCreate((LPCREATESTRUCT)lParam));

		case WM_DESTROY:
			// Shut down the program
//			winApp->OnDestroy();
			PostQuitMessage(0);
			break;

		case WM_CLOSE:
			return(winApp->OnClose());

		case WM_PAINT:
			// Call the paint method
			return(winApp->OnPaint((HDC)wParam));


#ifdef DIRECTX
		case WM_ENTERSIZEMOVE:
			gfxDisableDevice( );
			break;

		case WM_SIZE:
			if( wParam == SIZE_MINIMIZED )
			{
				gfxDisableDevice( );
				winApp->bMinimized = true;
				winApp->bMaximized = false;
			}
			else if( wParam == SIZE_MAXIMIZED )
			{
				if( winApp->bMinimized )
					gfxEnableDevice( );

				winApp->bMinimized = false;
				winApp->bMaximized = true;
				
				return winApp->HandleSizeChange( );
			}
			else if( wParam == SIZE_RESTORED )
			{
				if( winApp->bMaximized )
				{
					winApp->bMaximized = false;
					// if we're in directx_pc mode,
					// make sure to update size sensative stuff
					// regardless of whether or not you were maximized
					winApp->HandleSizeChange( );
				}
				else if ( winApp->bMinimized )
				{
					gfxEnableDevice( );
					winApp->bMinimized = false;
				}
//#ifdef DIRECTX_PC
//				else
//					winApp->FixMouseCursor();
//#endif
			}

			break;

		case WM_EXITSIZEMOVE:
			gfxEnableDevice( );
			winApp->HandleSizeChange( );

			break;
#else				
		case WM_SIZE:
			{
				// Window size message
				int sizeType = (int)wParam;
				int width = (int)LOWORD(lParam);
				int height = (int)HIWORD(lParam);
				return(winApp->OnSize(sizeType, width, height));
			}
#endif

		case WM_MOVE:
			// Window move message
			return(winApp->OnMove( ));

		case WM_GETMINMAXINFO:
			if (winApp->OnMinMaxInfo((LPMINMAXINFO)lParam) == 0)
				return(0);
			break;

		case WM_ACTIVATE:
#ifdef DIRECTX_PC
			if ( wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE )
			{
				ShowCursor(FALSE);
				// restore our sound settings
				winApp->setSoundLevels = true;
			}
            else if ( wParam == WA_INACTIVE ) 
                ShowCursor(TRUE);

#endif
			return(winApp->OnActivate(LOWORD(wParam), HIWORD(wParam) != 0, (HWND)lParam));

		case WM_TIMER:
			// Timer message
			return(winApp->OnTimer(wParam));

		case WM_COMMAND:
			// A command message
			return(winApp->OnCommand(wParam, lParam));

		case WM_INITMENUPOPUP:
			// Menu is about to popup
			return(winApp->OnInitMenuPopup((HMENU)wParam));

		// Mouse handling
		case WM_LBUTTONDOWN:
			return(winApp->OnLButtonDown(LOWORD(lParam), HIWORD(lParam), wParam));

		case WM_LBUTTONUP:
			return(winApp->OnLButtonUp(LOWORD(lParam), HIWORD(lParam), wParam));

		case WM_RBUTTONDOWN:
			return(winApp->OnRButtonDown(LOWORD(lParam), HIWORD(lParam), wParam));

		case WM_RBUTTONUP:
		return(winApp->OnRButtonUp(LOWORD(lParam), HIWORD(lParam), wParam));

		case WM_MOUSEMOVE:
			// Mouse is moving
			return(winApp->OnMouseMove(LOWORD(lParam), HIWORD(lParam), wParam));

		case WM_PALETTEISCHANGING:
			return(winApp->OnPaletteIsChanging((HWND)wParam));

		case WM_PALETTECHANGED:
			return(winApp->OnPaletteChanged((HWND)wParam));

		case WM_QUERYNEWPALETTE:
			return(winApp->OnQueryNewPalette());

		case WM_USER:
			return(winApp->OnUser(wParam, lParam));

#ifdef DIRECTX_PC
			//handle maximizing windows
		case WM_SYSKEYDOWN:
			{
				if ((GetAsyncKeyState(VK_RETURN) & 0x8000) != 0)
					return(winApp->OnSetMaximize());
				if ((GetKeyState(VK_F4) & 0x800) != 0)
					return(winApp->OnClose());
			}
		case WM_KEYDOWN:
			if ((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0 ||
				(GetAsyncKeyState(VK_RWIN) & 0x8000) != 0 ||
				(GetAsyncKeyState(VK_APPS) & 0x8000) != 0)
				return 0;
		case WM_SYSCOMMAND:
			if (wParam != SC_CLOSE &&
				wParam != SC_RESTORE && 
				wParam != SC_MAXIMIZE &&
				wParam != SC_MINIMIZE && 
				wParam != SC_ICON &&
				wParam != SC_ZOOM )
				return 0;
#endif
		
		default:
			{
				LONG retval;

				// Call the catch all
				if (winApp->OnOtherMessage(msg, wParam, lParam, retval))
					return(retval);
			}
		break;
	}

	// Always return this if the message wasn't handled
	return(::DefWindowProc(hWnd, msg, wParam, lParam));
}

//*************************************************************************
//*************************************************************************
int WindowsApp::OnPaint(
HDC hdc) 
{ 
	RECT rect;
#ifdef DIRECTX
	if( DirectX_PresentParams.Windowed )
	{
		if( GetUpdateRect( hWnd, &rect, FALSE ) )
			ValidateRect( hWnd, &rect );
	}
	else
		ValidateRect( hWnd, NULL );
#else
		if( GetUpdateRect( hWnd, &rect, FALSE ) )
			ValidateRect( hWnd, &rect );
#endif

	return 0; 
}

#ifdef DIRECTX_PC
//*************************************************************************
//*************************************************************************
int WindowsApp::OnSetMaximize()
{
	if (bMaximized)
	{
		//restore
		bMaximized = false;
		ShowWindow(hWnd,SW_RESTORE);
	} 
	else 
	{
		// maximize
		bMaximized = true;
		ShowWindow(hWnd,SW_MAXIMIZE);
	}
	return 0;
}

#endif

float GetWinTime( void )
{
	static __int64 base = 0;
	static __int64 counter = 0;
	static __int64 frequency = 0;

	QueryPerformanceCounter( (LARGE_INTEGER *) &counter );

	if( base == 0 )
		base = counter;

	if( frequency == 0 )
		QueryPerformanceFrequency( (LARGE_INTEGER *) &frequency );

	return (float) ((double) (counter - base) / (double) frequency);
}

float WindowsApp::CpuSpeed( void )
{
	static float time1 = 0.0f;
	static float time2 = 0.0f;
	static __int64 counter1 = 0;
	static __int64 counter2 = 0;

	if( *((int *)&time1) == 0 )
	{
		time1 = GetWinTime();

		__asm
		{
			rdtsc;
			mov dword ptr counter1+0, eax;
			mov dword ptr counter1+4, edx;
		}

		while( time2 - time1 < 0.500f )
			time2 = GetWinTime();

		__asm
		{
			rdtsc;
			mov dword ptr counter2+0, eax;
			mov dword ptr counter2+4, edx;
		}
	}

	return (float)((double)(counter2 - counter1) / (double)(time2 - time1) / 1000000.0f);
}