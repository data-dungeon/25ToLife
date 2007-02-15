/* assert.c */

/******************* includes ****************************/

#include "Layers/LayersPCH.h"

#ifndef DISABLE_ASSERTS

#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include "platform/Win32/resource.h"
#include "platform/Win32/Winapp.h"

#define ASSERT_SUPPORT_IGNORE
#define ASSERT_SUPPORT_IGNORE_ALWAYS
#define ASSERT_SUPPORT_ABORT
#define ASSERT_SUPPORT_QUIT

#define ASSERT_NOASSERT 0
#define ASSERT_ABORT 1
#define ASSERT_QUIT 2
#define ASSERT_IGNORE 3
#define ASSERT_IGNORE_ALWAYS 4

namespace Assert
{
	static bool disableAsserts = false;
	static const char* sExpr;
	static const char* sFile;
	static const char* sString;
	static int sLine;

	BOOL CALLBACK DlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		int id;

		switch(uMsg)
		{
		case WM_INITDIALOG:

			// turn on the appropriate buttons
#ifdef ASSERT_SUPPORT_IGNORE_ALWAYS
			EnableWindow(GetDlgItem(hwndDlg, IDC_IGNORE_ALWAYS), TRUE);
#endif
#ifdef ASSERT_SUPPORT_IGNORE
			EnableWindow(GetDlgItem(hwndDlg, IDC_IGNORE), TRUE);
#endif
#ifdef ASSERT_SUPPORT_ABORT
			EnableWindow(GetDlgItem(hwndDlg, IDC_DEBUG), TRUE);
#endif
#ifdef ASSERT_SUPPORT_QUIT
			EnableWindow(GetDlgItem(hwndDlg, IDC_QUIT), TRUE);
#endif

			// now set the strings up...
			SetWindowText(GetDlgItem(hwndDlg, IDC_FILE), sFile);
			SetWindowText(GetDlgItem(hwndDlg, IDC_EXPRESSION), sExpr);
			if(sString)
				SetWindowText(GetDlgItem(hwndDlg, IDC_STRING), sString);
			char buffer[100];
			sprintf(buffer, "%d", sLine);
			SetWindowText(GetDlgItem(hwndDlg, IDC_LINE), buffer);

			// show the window
			ShowWindow(hwndDlg, SW_SHOW);

			return TRUE;

		case WM_COMMAND:
			id = LOWORD(wParam);
			switch(id)
			{
			case IDC_IGNORE:
				EndDialog(hwndDlg, ASSERT_IGNORE);
				return TRUE;
			case IDC_IGNORE_ALWAYS:
				EndDialog(hwndDlg, ASSERT_IGNORE_ALWAYS);
				return TRUE;
			case IDC_DEBUG:
				EndDialog(hwndDlg, ASSERT_ABORT);
				return TRUE;
			case IDC_QUIT:
				EndDialog(hwndDlg, ASSERT_QUIT);
				return TRUE;
			}
		}

		return FALSE;
	}

	bool Dialog(const char *expr, const char *file, int line, const char* string)
	{
		dbgPrint("Assertion failure:\n");
		if(string) 
			dbgPrint("String: %s\n", string);
		dbgPrint("Expression: %s\n", expr);
		dbgPrint("File %s\n", file);
		dbgPrint("Line %d\n", line);

#ifdef ASSERT_SUPPORT_IGNORE_ALWAYS
		if(IgnoreAlways(expr, file, line, string))
			return true;
#endif

		WindowsApp *winApp = WindowsApp::IAm();

		LPCTSTR resource;
	
		if(string)
			resource = MAKEINTRESOURCE(IDD_ASSERTSDLG);
		else
			resource = MAKEINTRESOURCE(IDD_ASSERTDLG);
	
		// store this off for DlgProc
		sExpr = expr;
		sFile = file;
		sLine = line;
		sString = string;

		// beep
		MessageBeep(MB_ICONSTOP);

		// do our dialog box
		int result = DialogBox(winApp->GethInst(), resource, winApp->GethWnd(), DlgProc);

		switch(result)
		{
		case ASSERT_IGNORE:
			return true;
		case ASSERT_IGNORE_ALWAYS:
			SetIgnoreAlways(expr, file, line, string, true);
			return true;
		case ASSERT_QUIT:
			FatalExit(0);
			return true;
		case ASSERT_ABORT:
			return false;
		default:
			// unknown return from our dlg proc
			LOCKUP();
			return false;
		}
	}

	bool CrtReport(const char *expr, const char *file, int line, const char* string)
	{
		dbgPrint("Assertion failure:\n");
		if(string) 
			dbgPrint("String: %s\n", string);
		dbgPrint("Expression: %s\n", expr);
		dbgPrint("File %s\n", file);
		dbgPrint("Line %d\n", line);

#ifdef ASSERT_SUPPORT_IGNORE_ALWAYS
		if(IgnoreAlways(expr, file, line, string))
			return true;
#endif

#ifdef _DEBUG
		int result;

		if(string)
			result = _CrtDbgReport(_CRT_ASSERT, file, line, NULL, "%s\r\nString: %s", expr, string);
		else
			result = _CrtDbgReport(_CRT_ASSERT, file, line, NULL, "%s", expr);

		// there is no way to turn on the ignore always from this function!

		if(result == 0)
			return true;
#endif

		return false;
	}

	bool PigpenHandler(const char *expr, const char *file, int line, const char* string)
	{
#ifdef _DEBUG
		return(false);	// let debugger handle it
#else //_DEBUG
		return(true);	// ignore it
#endif //_DEBUG
	}

};

#endif //ASSERT_SUPPORT_IGNORE_ALWAYS
