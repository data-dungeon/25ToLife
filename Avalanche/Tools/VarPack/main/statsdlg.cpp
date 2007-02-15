//***************************************************************************
// Windows command console
//***************************************************************************
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "statsdlg.h"
#include "resource.h"

//***************************************************************************
// The init/shutdown
//***************************************************************************
StatsDisplayDialog::StatsDisplayDialog(
HINSTANCE hInst,
HWND hParent,
Stats &useStats) :
stats(useStats)
{
	INITCOMMONCONTROLSEX init;

	init.dwSize = sizeof(init);
	init.dwICC = ICC_PROGRESS_CLASS;
	InitCommonControlsEx(&init);

	// Create us
	hDlg = CreateDialogParam(hInst,
										MAKEINTRESOURCE(IDD_PROGRESS),
									  	hParent,
										(DLGPROC)DialogProc,
										(LPARAM)this);

	signalQuit = false;
	setWait = false;
}

//***************************************************************************
// The init/shutdown
//***************************************************************************
StatsDisplayDialog::~StatsDisplayDialog(void)
{
	if (hDlg)
		DestroyWindow(hDlg);
}

//***************************************************************************
// Force a visual update
//***************************************************************************
void StatsDisplayDialog::Update(void)
{
	if (hDlg == NULL)
		return;

	// set all the fields
	char temp[256];

	// Current instance
	SendDlgItemMessage(hDlg, ID_FILENAME, WM_SETTEXT, 0, (LPARAM)stats.currentFile.Data());

	sprintf(temp, "%d", stats.onFile);
	SendDlgItemMessage(hDlg, ID_FILENUMBER, WM_SETTEXT, 0, (LPARAM)temp);

	sprintf(temp, "%d", stats.totalSize);
	SendDlgItemMessage(hDlg, ID_TOTALSIZE, WM_SETTEXT, 0, (LPARAM)temp);

	// Status
	char *newText = stats.status.Data();
	if (newText[0] != '\0')
	{
		InsertTextAtEnd(GetDlgItem(hDlg, ID_STATUS), newText, strlen(newText), true);
		stats.ClearStatus();
	}

	// Progress
	SendDlgItemMessage(hDlg, ID_PROGRESS, PBM_SETPOS, (WPARAM)((int)stats.percentageDone), 0);

	// Setup the wait?
	if (!setWait && stats.done)
	{
		SendDlgItemMessage(hDlg, IDCANCEL, WM_SETTEXT, 0, (LPARAM)"Close");
		setWait = true;
	}
}

//***************************************************************************
// Our dialog proc
//***************************************************************************
BOOL CALLBACK StatsDisplayDialog::DialogProc(
HWND hDlg,
UINT msg,
WPARAM wParam,
LPARAM lParam)
{
	//!!! One console per app up at a time
	static StatsDisplayDialog *me = NULL;

	switch (msg)
	{
		case WM_INITDIALOG:
			{
			// Who are we?
			me = (StatsDisplayDialog *)lParam;

			// Load our size and set it
			LoadSize(hDlg);

			char temp[256];

			// Static stuff
			SendDlgItemMessage(hDlg, ID_INPUTFILE, WM_SETTEXT, 0, (LPARAM)me->stats.startDir);
			SendDlgItemMessage(hDlg, ID_OUTPUTFILE, WM_SETTEXT, 0, (LPARAM)me->stats.outputFile);
			sprintf(temp, "%d", me->stats.totalFiles);
			SendDlgItemMessage(hDlg, ID_TOTALINSTANCES, WM_SETTEXT, 0, (LPARAM)temp);

			// Set focus to the dialog
			SetFocus(GetDlgItem(hDlg, IDCANCEL));
			return(FALSE);
			}
		case WM_COMMAND:
			{
				switch (LOWORD(wParam))
				{
					case IDCANCEL:
						me->signalQuit = true;
						return(TRUE);
				}
			}
			break;

		case WM_DESTROY:
			SaveSize(hDlg);
			if (me)
				me->hDlg = NULL;
			me = NULL;
			break;
	}

	return(FALSE);
}

//***************************************************************************
// Sizing stuff
//***************************************************************************
bool StatsDisplayDialog::LoadSize(
HWND hDlg)
{
	RECT rect;

	// Load the rect
	if (!ReadProfileInt("OBBWorld", "top", &rect.top) ||
			!ReadProfileInt("OBBWorld", "left", &rect.left))
		return(false);

	SetWindowPos(hDlg, NULL,
						rect.left, rect.top,
						0, 0,
						SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);

	return(true);
}

//***************************************************************************
//***************************************************************************
bool StatsDisplayDialog::SaveSize(
HWND hDlg)
{
	RECT rect;

	GetWindowRect(hDlg, &rect);

	// Save these (should use reg someday)
	WriteProfileInt("OBBWorld", "top", rect.top);
	WriteProfileInt("OBBWorld", "left", rect.left);

	return(true);
}

//***************************************************************************
//***************************************************************************
bool StatsDisplayDialog::WriteProfileInt(
char *section,
char *var,
long value)
{
	char temp[64];
	return(WriteProfileString(section, var, itoa(value, temp, 10)) != 0);
}

//***************************************************************************
//***************************************************************************
bool StatsDisplayDialog::ReadProfileInt(
char *section,
char *var,
long *value)
{
	char temp[64];
	if (GetProfileString(section, var, "", temp, sizeof(temp)) <= 0)
		return(false);

	*value = atoi(temp);
	return(true);
}

/***************************************************************************/
// blank out the input text
/***************************************************************************/
void StatsDisplayDialog::InsertTextAtEnd(
HWND hOut,
const char *text,
int len,
bool scroll)
{
	// Put the selection at the end of the text
	int length = (int)SendMessage(hOut, WM_GETTEXTLENGTH, 0, 0);
	if (length > 32767)
		SendMessage(hOut, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
	else
		SendMessage(hOut, EM_SETSEL, (WPARAM)length, (LPARAM)-1);

	// Submit the text
	if (len < 0)
		SendMessage(hOut, EM_REPLACESEL, 0, (LPARAM)text);
	else
	{
		char buff[1024];
		char *temp;

		// get buffer space
		if (len < sizeof(buff) - 1)
			temp = buff;
		else
			temp = new char[len + 1];
		if (!temp)
			return;

		// Copy it in
		strncpy(temp, text, len);
		temp[len] = '\0';

		// submit it
		SendMessage(hOut, EM_REPLACESEL, 0, (LPARAM)temp);

		// Get rid of it?
		if (len >= sizeof(buff) - 1)
			delete [] temp;
	}

	// Scroll into view?
	if (scroll)
		SendMessage(hOut, EM_SCROLLCARET, 0, 0);
}

