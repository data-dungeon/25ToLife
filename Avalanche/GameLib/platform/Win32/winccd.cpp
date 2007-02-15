/***************************************************************************/
// Windows command console
/***************************************************************************/

/******************* includes ****************************/

#include "platform/PlatformPCH.h"
#include "cmdcon/cmdcon.h"

#include <richedit.h>
#include <commctrl.h>

/***************************************************************************/
// The init/shutdown
/***************************************************************************/
WindowsCommandConsoleDisplay::WindowsCommandConsoleDisplay(
CommandConsole &parent) :
CommandConsoleDisplay(parent)
{
	hDlg = NULL;
	hOut = NULL;
}

/***************************************************************************/
// The init/shutdown
/***************************************************************************/
WindowsCommandConsoleDisplay::~WindowsCommandConsoleDisplay(void)
{
	if (hDlg)
		DestroyWindow(hDlg);
}

/***************************************************************************/
// Bind to windows
/***************************************************************************/
bool WindowsCommandConsoleDisplay::Create(
HINSTANCE hInst,
HWND hParent)
{
#ifdef DIRECTX_PC
#ifndef _DEBUG
	return false;
#endif //_DEBUG
#endif //DIRECTX_PC

	// We are supposed to call this I think?
	InitCommonControls();

	// Load the rich edit dll
	HMODULE hModule = LoadLibrary("Riched20.dll");
	ASSERT(hModule);

	// Create the dialog
	hDlg = CreateDialogParam(hInst,
										MAKEINTRESOURCE(IDD_CONSOLE),
									  	hParent,
										(DLGPROC)DialogProc,
										(LPARAM)this);

	// Get the handle to the existing edit control
	hOut = GetDlgItem(hDlg, IDC_CONSOLEOUT);

	// Did we get the rich edit module?
	if (hModule)
	{
		// Get the existing control's size
		RECT textRect;
		GetWindowRect(hOut, &textRect);
		int width = textRect.right - textRect.left;
		int height = textRect.bottom - textRect.top;

		// Build the coordinate system for the child
		POINT p;
		p.x = textRect.left; p.y = textRect.top;
		ScreenToClient(hDlg, &p);

		// Create the new rich edit in it's place!
		HWND hRich = CreateWindowEx(0,
			  									RICHEDIT_CLASS,
												"",
												ES_MULTILINE | ES_AUTOVSCROLL |
												ES_AUTOHSCROLL | ES_OEMCONVERT | ES_READONLY |
												WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP,
												p.x, p.y, width, height,
												hDlg,
												NULL,
												hInst,
												NULL);

		ASSERT(hRich);

		// Setup the control
		if (hRich)
		{
			// Now delete the old window
			DestroyWindow(hOut);
			hOut = hRich;

			// Setup the rich edit control
			BlankText(hOut);
			SendMessage(hOut, EM_SETTEXTMODE, TM_PLAINTEXT, 0);

			// Setup the font
			CHARFORMAT charFormat;
			memset(&charFormat, '\0', sizeof(charFormat));
			charFormat.cbSize = sizeof(charFormat);
			charFormat.dwMask = CFM_FACE | CFM_SIZE | CFM_BOLD | CFM_ITALIC;
			charFormat.yHeight = 11 * 20;
			strcpy(charFormat.szFaceName, "Lucida Console");

			SendMessage(hOut, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&charFormat);

			// We've kind of resized everything...
			HandleResize(hDlg, hOut);
		}
	}

	return(hDlg != NULL);
}

/***************************************************************************/
// Display this new line on the console
/***************************************************************************/
bool WindowsCommandConsoleDisplay::NewLine(
const char *display)
{
	// Add the text to the box
	if (!hOut)
		return(false);

	while (*display)
	{
		// scan for new line
		int len = 0;
		while (display[len] != '\n' && display[len] != '\0')
			++len;
		InsertTextAtEnd(hOut, display, len);

		if (display[len] == '\n')
			display += len + 1;
		else
			display += len;

		InsertTextAtEnd(hOut, "\r\n", -1, *display == '\0');
	}

	// Done
	return(true);
}

/***************************************************************************/
// Our dialog proc
/***************************************************************************/
BOOL CALLBACK WindowsCommandConsoleDisplay::DialogProc(
HWND hDlg,
UINT msg,
WPARAM wParam,
LPARAM lParam)
{
	//!!! One console per app up at a time
	static WindowsCommandConsoleDisplay *me = NULL;

	switch (msg)
	{
		case WM_INITDIALOG:
			// Who are we?
			me = (WindowsCommandConsoleDisplay *)lParam;

			// Load our size and set it
			LoadSize(hDlg);

			// Set focus to the dialog
			SetFocus(GetDlgItem(hDlg, IDC_CONSOLEIN));
			return(FALSE);

		case WM_COMMAND:
			{
				switch (LOWORD(wParam))
				{
					case IDOK:
						ASSERT(me);
						if (me)
							me->ProcessCommand(GetDlgItem(hDlg, IDC_CONSOLEIN));
						return(TRUE);

					case IDCANCEL:
						ASSERT(me);
						if (me)
							me->BlankText(GetDlgItem(hDlg, IDC_CONSOLEIN));
						return(TRUE);
				}
			}
			break;

		case WM_SIZE:
			HandleResize(hDlg, me->hOut);
			break;

		case WM_DESTROY:
			SaveSize(hDlg);
			me = NULL;
			break;
	}

	return(FALSE);
}

/***************************************************************************/
// Process the input command
/***************************************************************************/
void WindowsCommandConsoleDisplay::ProcessCommand(
HWND hIn)
{
	// How big is the text?
	int size = (int)SendMessage(hIn, WM_GETTEXTLENGTH, 0, 0) + 1;
	if (size <= 1)
		return;

	// Allocate space for the buffer
	char *temp;
	char buffer[256];
	if (size < sizeof(buffer))
		temp = buffer;
	else
		temp = new char[size];

	// This should never happen
	if (!temp)
	{
		BlankText(hIn);
		return;
	}

	// Get the text
	int ret = (int)SendMessage(hIn, WM_GETTEXT, (WPARAM)size, (LPARAM)temp);
	BlankText(hIn);

	// Process it
	if (ret > 0)
		cmdCon.Process(temp);

	// free it
	if (temp != buffer)
		delete [] temp;
}

/***************************************************************************/
// blank out the input text
/***************************************************************************/
void WindowsCommandConsoleDisplay::BlankText(
HWND hIn)
{
	static char *blank = "";
	SendMessage(hIn, WM_SETTEXT, 0, (LPARAM)blank);
}

/***************************************************************************/
// blank out the input text
/***************************************************************************/
void WindowsCommandConsoleDisplay::InsertTextAtEnd(
HWND hOut,
const char *text,
int len,
bool scroll)
{
	// Put the selection at the end of the text
	int length = (int)SendMessage(hOut, WM_GETTEXTLENGTH, 0, 0);
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
		SendMessage(hOut, EM_LINESCROLL, 0, 1);
}

/***************************************************************************/
// Sizing stuff
/***************************************************************************/
bool WindowsCommandConsoleDisplay::LoadSize(
HWND hDlg)
{
	RECT rect;

	// Load the rect
	if (!ReadProfileInt("AvalancheConsole", "top", &rect.top) ||
			!ReadProfileInt("AvalancheConsole", "left", &rect.left) ||
			!ReadProfileInt("AvalancheConsole", "right", &rect.right) ||
			!ReadProfileInt("AvalancheConsole", "bottom", &rect.bottom))
		return(false);

   int width = rect.right - rect.left;
   int height = rect.bottom - rect.top;
   SetWindowPos(hDlg, NULL,
						rect.left, rect.top,
						width, height,
						SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool WindowsCommandConsoleDisplay::SaveSize(
HWND hDlg)
{
	RECT rect;

	GetWindowRect(hDlg, &rect);

	// Save these (should use reg someday)
	WriteProfileInt("AvalancheConsole", "top", rect.top);
	WriteProfileInt("AvalancheConsole", "left", rect.left);
	WriteProfileInt("AvalancheConsole", "right", rect.right);
	WriteProfileInt("AvalancheConsole", "bottom", rect.bottom);

	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool WindowsCommandConsoleDisplay::HandleResize(
HWND hDlg,
HWND hOut)
{
	if (!hDlg)
		return(false);

	// Get the dialog rect
	RECT rect;
	GetClientRect(hDlg, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	// setup the input rect
	HWND hIn = GetDlgItem(hDlg, IDC_CONSOLEIN);
	RECT oldRect, inRect;
	GetWindowRect(hIn, &oldRect);
	int oldHeight = oldRect.bottom - oldRect.top;
	inRect.left = 5;
	inRect.right = width - 5;
	inRect.bottom = height - 5;
	inRect.top = inRect.bottom - oldHeight;
   SetWindowPos(hIn, NULL, inRect.left, inRect.top,
						inRect.right - inRect.left, inRect.bottom - inRect.top,
						SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);


	// setup the out rect
	RECT outRect;
	outRect.left = 5;
	outRect.top = 5;
	outRect.right = width - 5;
	outRect.bottom = inRect.top - 5;
   SetWindowPos(hOut, NULL, outRect.left, outRect.top,
						outRect.right - outRect.left, outRect.bottom - outRect.top,
						SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool WindowsCommandConsoleDisplay::WriteProfileInt(
char *section,
char *var,
long value)
{
	char temp[64];
	return(WriteProfileString(section, var, itoa(value, temp, 10)) != 0);
}

/***************************************************************************/
/***************************************************************************/
bool WindowsCommandConsoleDisplay::ReadProfileInt(
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

