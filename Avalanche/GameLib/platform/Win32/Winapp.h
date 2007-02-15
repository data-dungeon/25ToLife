/*************************************************************************/
// Windows application framework.  aka: Poor man's MFC. (or Portable man's
// mfc...)
/*************************************************************************/
#ifndef WINAPP_H
#define WINAPP_H

#define WIN32_LEAN_AND_MEAN
#ifdef _XBOX
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#else
#include <windows.h>
#endif

#ifndef _XBOX
// A very traditional single window based application class
class WindowsApp
{
public:
	// The constructor and destructors
	WindowsApp(void)
		{ hWnd = NULL; hInst = NULL; bMinimized = false; bMaximized = false; }
	virtual ~WindowsApp(void) {}

	// Who am I?
	static WindowsApp *IAm(void)
		{ return(me); }

	// The main methods
	virtual bool InitInstance(HINSTANCE hPrevInst, LPSTR cmdLine, int nCmdShow) = 0;

	virtual void TerminateInstance(void) = 0;

	// Here's were you put your accelerators
	virtual bool HandleMessage(MSG &msg) { return(false); }

	// Called every processing "frame"
	virtual void Process(void) {}

	// Message watcher
	virtual void MessageWatch(UINT msg, WPARAM wParam, LPARAM lParam) {}

	// Create method (return 0 for okay, -1 for error)
	virtual int OnCreate(LPCREATESTRUCT createStuff) { return(0); }
	virtual void OnDestroy(void) {}
	virtual int OnPaint(HDC hdc);
	virtual int HandleSizeChange( void ) { return(0); }
	virtual int OnSize(int sizeType, int width, int height) { return(DefWindowProc()); }
	virtual int OnMove(void) { return(DefWindowProc()); }
	virtual int OnMinMaxInfo(LPMINMAXINFO minMax) { return(-1); }
	virtual int OnActivate(WORD active, bool minimized, HWND hWnd) { return(DefWindowProc()); }
	virtual int OnClose(void) { return(DefWindowProc()); }

	// Palette stuff
	virtual int OnPaletteIsChanging(HWND hWndPalChg) { return(DefWindowProc()); }
	virtual int OnPaletteChanged(HWND hWndPalChg) { return(DefWindowProc()); }
	virtual int OnQueryNewPalette(void) { return(DefWindowProc()); }

	// User stuff
	virtual int OnUser(WPARAM wParam, LPARAM lParam) { return(DefWindowProc()); }

	// Timer message
	virtual int OnTimer(int timerID) { return(DefWindowProc()); }

	// Command messages
	virtual int OnCommand(WPARAM wParam, LPARAM lParam) { return(DefWindowProc()); }
	virtual int OnInitMenuPopup(HMENU hMenu) { return(DefWindowProc()); }

	// Mouse stuff
	virtual int OnLButtonDown(int x, int y, int keys) { return(DefWindowProc()); }
	virtual int OnLButtonUp(int x, int y, int keys) { return(DefWindowProc()); }
	virtual int OnRButtonDown(int x, int y, int keys) { return(DefWindowProc()); }
	virtual int OnRButtonUp(int x, int y, int keys) { return(DefWindowProc()); }
	virtual int OnMouseMove(int x, int y, int keys) { return(DefWindowProc()); }
#ifdef DIRECTX_PC
	virtual void FixMouseCursor() {}
	virtual int OnSetMaximize();
	bool setSoundLevels;
#endif

	// "catch-all" private message handler (return true if message handled,
	// with actual return value in &retval
	virtual bool OnOtherMessage(UINT msg, WPARAM wParam, LPARAM lParam, LONG &retval)
		{ return(false); }

	// Return the DefWindowProc result for this call
	int DefWindowProc(void)
		{ return(::DefWindowProc(hWnd, saveMsg, savewParam, savelParam)); }

	// Return the function pointer to the window proc
	WNDPROC WindowProc(void)
		{ return(windowProc); }

	// Accessors
	HWND GethWnd(void)
		{ return(hWnd); }
	HINSTANCE GethInst(void)
		{ return(hInst); }
	void SethInst(HINSTANCE hInstance)
		{ hInst = hInstance; }
	
	static float CpuSpeed( void );

	// We are quitting!
	void Quit(void);

protected:
	// Window stuff
	bool			bMinimized;
	bool			bMaximized;
	HINSTANCE   hInst;
	HWND        hWnd;

	// Save these
	UINT        saveMsg;
	WPARAM      savewParam;
	LPARAM      savelParam;

#ifdef DIRECTX_PC
	// create an exclusive application
	HANDLE applicationMutex;
#endif

	// Here is the window function pointer
	static WNDPROC windowProc;

	// Subclass must set the protected me
	static WindowsApp *me;

private:
	// Our true message processor
	static LONG PASCAL WinAppWindowProc(HWND win, UINT msg, WPARAM wParam, LPARAM lParam);
};
#endif

#endif
