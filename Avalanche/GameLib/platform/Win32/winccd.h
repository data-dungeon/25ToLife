/***************************************************************************/
// Windows command console
/***************************************************************************/
#ifndef WINCCD_H
#define WINCCD_H

// Include the windows crap
#include <windows.h>

// our base class
#include "cmdcon/ccondisp.h"

class WindowsCommandConsoleDisplay : public CommandConsoleDisplay
{
public:
	// The init/shutdown
	WindowsCommandConsoleDisplay(CommandConsole &parent);
	~WindowsCommandConsoleDisplay(void);

	// Create the window
	bool Create(HINSTANCE hInst, HWND hParent);

	// Display this new line on the console
	bool NewLine(const char *display);

	// Is this a dialog message?
	bool HandleMessage(MSG &msg)
		{ return(hDlg && IsDialogMessage(hDlg, &msg) != 0); }

private:
	HWND		hDlg;		// Our dialog box
	HWND		hOut;		// The output window

	// Our dialog proc
	static BOOL CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

	// Utilities
	void ProcessCommand(HWND hIn);
	static void BlankText(HWND hIn);
	static void InsertTextAtEnd(HWND hOut, const char *text, int len = -1, bool scroll = false);

	// Sizing stuff
	static bool LoadSize(HWND hDlg);
	static bool SaveSize(HWND hDlg);
	static bool HandleResize(HWND hDlg, HWND hOut);

	static bool WriteProfileInt(char *section, char *var, long value);
	static bool ReadProfileInt(char *section, char *var, long *value);
};

#endif

