//***************************************************************************
// Stats dialog
//***************************************************************************
#ifndef STATSDLG_H
#define STATSDLG_H

// Include the windows crap
#include <windows.h>

// our base class
#include "stats.h"

class StatsDisplayDialog
{
public:
	// The init/shutdown
	StatsDisplayDialog(HINSTANCE hInst, HWND hParent, Stats &useStats);
	~StatsDisplayDialog(void);

	// Is this a dialog message?
	bool HandleMessage(MSG &msg)
		{ return(hDlg && IsDialogMessage(hDlg, &msg) != 0); }

	// Force an update
	void Update(void);

	// Has a quit been signaled?
	bool Quit(void)
		{ return(signalQuit); }

private:
	HWND		hDlg;		// Our dialog box
	Stats		&stats;
	bool		signalQuit;
	bool		setWait;

	// Our dialog proc
	static BOOL CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

	// Sizing stuff
	static bool LoadSize(HWND hDlg);
	static bool SaveSize(HWND hDlg);

	static bool WriteProfileInt(char *section, char *var, long value);
	static bool ReadProfileInt(char *section, char *var, long *value);

	static void InsertTextAtEnd(HWND hOut,	const char *text,	int len, bool scroll);
};

#endif

