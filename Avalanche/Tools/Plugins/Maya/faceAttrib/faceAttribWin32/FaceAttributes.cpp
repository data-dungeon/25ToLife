// precomp'ed header files
#include "StdAfx.h"

// volitile local header files
#include "faceAttribWin32.h"
#include "FaceAttributes.h"
#include "FaceAttrController.h"
#include "FaceAttribContainerWnd.h"
#include "resource.h"
#include "ConfigFilesManager.h"
// #include "PickMode.h"

using namespace AttrControls;

//-----------------------------------------------------------------------------
// Global objects and functions:

FaceAttributes theFaceAttributes;

//------------------------------------------------------------------------
// --                             Callbacks                             --
//------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// colorSwatchSubclassWndProc
// This function acts as the "substitute" windows procdure for the color
// swatches.  It's needed to capture the clicking on of the color swatches and
// keeping the color picker from coming up.
//
// Input:
//  HWND hwnd -- Handle of the color swatch.
//  UINT uMsg -- Specifies the message sent.
//  WPARAM wParam -- Specifies any additional message-specific information.
//  LPARAM lParam -- Specifies any additional message-specific information.
//
// Output: Not exactly sure what the output should be, but I imagine it's like
// other window procedures which is: TRUE if this procedure processed the
// message and FALSE otherwise.
//-----------------------------------------------------------------------------
LRESULT APIENTRY colorSwatchSubclassWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		{
			// Pass the message on to let any set color swatches handle the message:
			if (theFaceAttributes.HandleWinMessage(hwnd, uMsg, wParam, lParam) == true)
			{
				// We don't need to rebuild the UIDisplay for this message!
				// But we do need to load the key/val panel:
				theFaceAttributes.LoadKeyValPanelTree();
			}
			
			break;
		}
	case WM_DESTROY:
		{
			// Restore the original window procedure:
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR) theFuncPtrController.GetOrigSwatchProc());
			// Fallthrough...
		}
	default:
		{
			// Call the original window procedure:
			return CallWindowProc(theFuncPtrController.GetOrigSwatchProc(), hwnd, uMsg, wParam, lParam); 
			break;
		}
	}
	
	return 0;
}

BOOL CALLBACK AttributesRollupPageProc(HWND hDlg, UINT message, WPARAM wParam,
									   LPARAM lParam)
// This function acts as the callback function for the attributes rollup
// dialog (page).  Messages sent to the dialog are processed here.
//
// Input:
//  HWND hWnd -- Handle to the dialog.
//  UINT message -- Specifies the message sent.
//  WPARAM wParam -- Specifies additional message-specific information.
//  LPARAM lParam -- Specifies additional message-specific information.
//
// Output: TRUE if this callback processed the message, FALSE otherwise.
//
{
	switch (message)
	{
		case WM_INITDIALOG:
		{
			// set up tooltip for the attributes
			theFaceAttributes.m_hAttributesToolTip = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
				CustomControls::GetWindowClassName(CustomControls::TOOLTIP_WINDOW_CLASS), NULL,
				WS_POPUP, 
				0,0,0,0,
				hDlg, NULL, hInstance,
				NULL);

			ICustomToolTip *pToolTip = ICustomToolTip::GetIToolTip(theFaceAttributes.m_hAttributesToolTip);

			int toolTipBehavior = ICustomToolTip::TOOLTIP_MULTIPLE_SHOW | ICustomToolTip::TOOLTIP_TRACKING_MOUSE | ICustomToolTip::TOOLTIP_CLOSE_LEAVEWND | ICustomToolTip::TOOLTIP_NOCLOSE_OVER;
			int toolTipStyle = ICustomToolTip::TOOLTIP_SHADOW;
			pToolTip->SetBehaviour(toolTipBehavior);
			pToolTip->SetStyles(toolTipStyle);
			pToolTip->SetDelayTime(TTDT_AUTOPOP,0);
			pToolTip->SetDelayTime(TTDT_INITIAL,500);
			pToolTip->SetDelayTime(TTDT_RESHOW,100);

			pToolTip->SetNotify(TRUE);
			pToolTip->SetNotify(hDlg);
			pToolTip->Activate(TRUE);

			pToolTip->SetParentWnd(hDlg); // new parent since one gathered from constructor is wrong(since dialog is a child)

			ICustomToolTip::ReleaseIToolTip(pToolTip);

			// load in the current attrib data (if any) and see if we need
			// to show the controls
			if (theFaceAttributes.LoadCurrAttribData() == true)
			{
				// hide the "No objects" label
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC_SELOBJECTS), SW_HIDE);

				// Update the "Copy Attributes" buttons:
				theFaceAttributes.UpdateCopyPasteButtons();
				
				// Enable the apply buttons in the key/val panel:
				theFaceAttributes.SetApplyBttnStates(true, true);
				
				// Load the key/val panel:
				theFaceAttributes.LoadKeyValPanelTree();

				// Set up the UI Display:
				theFaceAttributes.SetUpUIDisplay(hDlg, hInstance,
					START_X, START_Y,
					LABEL_WD, LABEL_HT,
					SPACE_BET_CTRLS, colorSwatchSubclassWndProc,false);
			}
			else
			{
				// show the "No objects" label
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC_SELOBJECTS), SW_SHOW);

				// Update the "Copy Attributes" buttons:
				theFaceAttributes.UpdateCopyPasteButtons();
				
				// Disable the apply buttons in the key/val panel:
				theFaceAttributes.SetApplyBttnStates(false, false);
			}

			return TRUE;
		}
		case WM_PAINT:
		{
			// draw the tree lines
			PAINTSTRUCT ps;
			HDC hdc;			
			hdc = BeginPaint( hDlg, &ps );
			theFaceAttributes.DrawUIDisplayLines(hdc);
			EndPaint( hDlg, &ps );

			return FALSE;
		}
	    case WM_COMMAND:
		{
			switch(HIWORD(wParam))
			{
				// check for a selection change in a combobox
				case CBN_CLOSEUP:
				{
					// fall through
				}
				// Check for a checkbox button pressed:
				case BN_CLICKED:
				{
					// pass the message on to see if it gets handled
					if (theFaceAttributes.HandleWinMessage(hDlg, message, wParam, lParam) == true)
					{
						// Set up the UI Display:
						theFaceAttributes.SetUpUIDisplay(hDlg, hInstance,
							START_X, START_Y,
							LABEL_WD, LABEL_HT,
							SPACE_BET_CTRLS, colorSwatchSubclassWndProc);
						
						// Load the key/val panel:
						theFaceAttributes.LoadKeyValPanelTree();
						
						return FALSE;
					}
					else
						return TRUE;
				}
			}
			return TRUE;			
		}
		case WM_NOTIFY:
		{
			LPNMHDR hdr = (LPNMHDR) lParam;
			switch (hdr->code)
			{
				// check to see if a listview was clicked on
				case NM_CLICK:
				{
					// pass the message on to see if it gets handled
					if (theFaceAttributes.HandleWinMessage(hDlg, message, wParam, lParam) == true)
					{
						// We don't need to rebuild the UIDisplay for this message!
						// But we do need to load the key/val panel:
						theFaceAttributes.LoadKeyValPanelTree();
					}
				}
				break;
				// if this is a toolbar message
				case ICustomToolTip::UDM_TOOLTIP_DISPLAY:
				{
					// send to control that deals with it
					theFaceAttributes.HandleWinMessage(hDlg, message, wParam, lParam);
				}
				break;
			}
			return TRUE;
		}
		// check to see if a custom edit box is changing
		case WM_ATCUSTOMEDIT_ENTER:
		{
			// fall through
		}
		// check to see if a spinner button is let up
		case CC_ATSPINNER_BUTTONUP:
		{
			// Fall through:
		}
		// Check to see if a slider thumb is let up:
		case CC_ATSLIDER_BUTTONUP:
		{
			// fall through
		}
		// check to see if a free color swatch is changing color
//		case CC_COLOR_CHANGE:
		{
			// pass the message on to let any of the controls handle the message
			if (theFaceAttributes.HandleWinMessage(hDlg, message, wParam, lParam) == true)
			{
				// Set up the UI Display:
				theFaceAttributes.SetUpUIDisplay(hDlg, hInstance,
					START_X, START_Y,
					LABEL_WD, LABEL_HT,
					SPACE_BET_CTRLS, colorSwatchSubclassWndProc);

				// We don't need to rebuild the UIDisplay for this message!
				// But we do need to load the key/val panel:
				theFaceAttributes.LoadKeyValPanelTree();
			}
			return FALSE;
		}
		case WM_SETCURSOR:
		{
			if (NULL != theFaceAttributes.m_hAttributesToolTip)
			{
				WORD message = HIWORD(lParam);

				MSG msg;
				memset(&msg,0,sizeof(MSG));
				POINT pt;
				::GetCursorPos(&pt);   
				msg.hwnd = hDlg;
				msg.message = message;
				msg.wParam = 0;
				msg.lParam = MAKELONG(pt.x, pt.y);//logical unit
				msg.time = ::GetTickCount();
				msg.pt = pt;         
				switch(message)
				{
				case WM_LBUTTONDOWN:
				case WM_LBUTTONDBLCLK:
				case WM_RBUTTONDOWN:
				case WM_RBUTTONDBLCLK:
				case WM_MBUTTONDOWN:
				case WM_MBUTTONDBLCLK:
				case WM_NCLBUTTONDOWN:
				case WM_NCLBUTTONDBLCLK:
				case WM_NCRBUTTONDOWN:
				case WM_NCRBUTTONDBLCLK:
				case WM_NCMBUTTONDOWN:
				case WM_NCMBUTTONDBLCLK:
				case WM_MOUSEMOVE:
					{
						ICustomToolTip *pToolTip = ICustomToolTip::GetIToolTip(theFaceAttributes.m_hAttributesToolTip);
						if (pToolTip)
						{
							pToolTip->RelayEvent(&msg);
							ICustomToolTip::ReleaseIToolTip(pToolTip);
						}
					}
					break;
				}
			}
			return FALSE;
		}
			// when there is a selection change
		case WM_SELCHANGE:
		{
			theFaceAttributes.RefreshConfig();
			int request = wParam; // 0 - no selection 1-check selection
			// there are any selected and we request it
			if (request && theFaceAttributes.LoadCurrAttribData() == true)
			{
				// hide the "No objects" label
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC_SELOBJECTS), SW_HIDE);

				// Enable the "Copy Attributes" buttons:
				theFaceAttributes.UpdateCopyPasteButtons();

				// Enable the apply buttons in the key/val panel:
				theFaceAttributes.SetApplyBttnStates(true, true);

				// Set up the UI Display:
				theFaceAttributes.SetUpUIDisplay(hDlg, hInstance,
					START_X, START_Y,
					LABEL_WD, LABEL_HT,
					SPACE_BET_CTRLS, colorSwatchSubclassWndProc);
			}
			else
			{
				// show the "No objects" label
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC_SELOBJECTS), SW_SHOW);

				// Enable the "Copy Attributes" buttons:
				theFaceAttributes.UpdateCopyPasteButtons();

				// Disable the apply buttons in the key/val panel:
				theFaceAttributes.SetApplyBttnStates(false, false);

				// Destroy any CurrAttribData and UIDisplay:
				theFaceAttributes.DestroyCurrAttribData();
				theFaceAttributes.DestroyUIDisplay();

				// adjust the panel heights
				theFaceAttributes.AdjustPanelHeights(true);
			}

			// Load the key/val panel:
			theFaceAttributes.LoadKeyValPanelTree();
			return FALSE;
		}
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// NotifySelSetChanged
// This method acts as a callback procedure that gets called when the
// NOTIFY_SELECTIONSET_CHANGED message is sent.
//
// Input:
//  void* param -- This will be NULL, since that is how it is registered.  In
//				   other words, this procedure doesn't need to know anything
//				   about it.
//  NotifyInfo* info -- The notification info structure.
//
// Output: None.
//-----------------------------------------------------------------------------
static void NotifySelSetChanged(void * data)
{
	if (theFaceAttributes.bDialogUp == true)
	{
		// request for selection
		SendMessage(theFaceAttributes.hDlg,WM_SELCHANGE,1,0);
	}
}

static void NotifyBeforeOpen(void * data)
{
	if (theFaceAttributes.bDialogUp == true)
	{
		// no request for selection
		SendMessage(theFaceAttributes.hDlg,WM_SELCHANGE,0,0);
	}
}

static LRESULT APIENTRY PushButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 
    if (uMsg == WM_KILLFOCUS || uMsg == WM_SETFOCUS)
		return TRUE;
	
	WNDPROC oldProc = (WNDPROC) GetWindowLong(hwnd, GWLP_USERDATA);
	
    return CallWindowProc(oldProc, hwnd, uMsg, wParam, lParam); 
} 

static void DialogDone(void)
{
	if (!theFaceAttributes.hDlg)
		return;

	// save off the position of the dialog
	RECT dialogRect;
	GetWindowRect(theFaceAttributes.hDlg, &dialogRect);
	Registry myRegistry(OBJECT_ATTRIB_KEY);
	myRegistry.Set(DIALOG_TOP_VAL_NAME, (DWORD) dialogRect.top);
	myRegistry.Set(DIALOG_BOTTOM_VAL_NAME, (DWORD) dialogRect.bottom);
	myRegistry.Set(DIALOG_LEFT_VAL_NAME, (DWORD) dialogRect.left);
	myRegistry.Set(DIALOG_RIGHT_VAL_NAME, (DWORD) dialogRect.right);

	// save off the open/closed state of the panels
	TCHAR titleBuf[80];
	int bufLen = 80;
	LoadString(hInstance, IDS_KEYS_AND_VALUES, titleBuf, bufLen);
	TupString strKeyValTitle = titleBuf;
	LoadString(hInstance, IDS_ATTRIBUTES, titleBuf, bufLen);
	TupString strAttribTitle = titleBuf;
	LoadString(hInstance, IDS_APPLY_COLORS, titleBuf, bufLen);
	TupString strApplyColorsTitle = titleBuf;

	ICustomRollupWindow* pRollupWindow = ICustomRollupWindow::GetICustomRollupWindow(GetDlgItem(theFaceAttributes.hDlg, IDC_CUSTOM_ROLLUP));
	int nScrollPos = pRollupWindow->GetScrollPos();
	int nNumPanels = pRollupWindow->GetNumPanels();
	for (int nIndex = 0; nIndex < nNumPanels; nIndex++)
	{
		HWND hwndCurrPanel = pRollupWindow->GetPanelDlg(nIndex);
		ICustomRollupPanel* pCurrRollupPanel = pRollupWindow->GetPanel(hwndCurrPanel);
		HWND hwndTitle = pCurrRollupPanel->GetTitleWnd();
		
		int nBufLength = 80;
		TCHAR titleBuf[80];
		
		GetWindowText(hwndTitle, titleBuf, nBufLength);
		TupString strCurrTitle = titleBuf;
		// find which panel we are at
		if (strCurrTitle == strAttribTitle)
		{
			// we are at the attrib panel
			if (pRollupWindow->IsPanelOpen(nIndex) == TRUE)
				myRegistry.Set(ATTRIB_PANEL_OPEN_NAME, (DWORD) 1);
			else
				myRegistry.Set(ATTRIB_PANEL_OPEN_NAME, (DWORD) 0);
		}
		else if (strCurrTitle == strKeyValTitle)
		{
			// We are at the key/val panel:
			if (pRollupWindow->IsPanelOpen(nIndex) == TRUE)
				myRegistry.Set(KEYVAL_PANEL_OPEN_NAME, (DWORD) 1);
			else
				myRegistry.Set(KEYVAL_PANEL_OPEN_NAME, (DWORD) 0);
		}
		else if (strCurrTitle == strApplyColorsTitle)
		{
			// We are at the key/val panel:
			if (pRollupWindow->IsPanelOpen(nIndex) == TRUE)
				myRegistry.Set(APPLYCOLORS_PANEL_OPEN_NAME, (DWORD) 1);
			else
				myRegistry.Set(APPLYCOLORS_PANEL_OPEN_NAME, (DWORD) 0);
		}
	}

	// save off the nonexclusive keys in the registry
	theFaceAttributes.SaveNonexclusiveKeys();

	// Now clean up:
	// Release any controls:
	if (theFaceAttributes.m_hIconOneToMany)
		::DestroyIcon(theFaceAttributes.m_hIconOneToMany);
	if (theFaceAttributes.m_hIconOneToManyDisabled)
		::DestroyIcon(theFaceAttributes.m_hIconOneToManyDisabled);
	if (theFaceAttributes.m_hIconReplace)
		::DestroyIcon(theFaceAttributes.m_hIconReplace);
	if (theFaceAttributes.m_hIconReplaceDisabled)
		::DestroyIcon(theFaceAttributes.m_hIconReplaceDisabled);
	if (theFaceAttributes.m_hIconBrowse)
		::DestroyIcon(theFaceAttributes.m_hIconBrowse);

	// Clean up things dealing with the keyval panel:
	theFaceAttributes.IdCancelMessage();

	// Unregister the rollup callback:
	ICustomRollupWindow::ReleaseICustomRollupWindow(pRollupWindow);

	theFaceAttributes.bDialogUp = false;

	EndDialog(theFaceAttributes.hDlg, FALSE);
	theFaceAttributes.DestroyUIDisplay();
	theFaceAttributes.DestroyCurrAttribData();
	theFaceAttributes.hDlg = NULL;
}


// This function acts as the callback function for the object attributes
// dialog.  Messages sent to the dialog are processed here.
// Input:
//  HWND hWnd -- Handle to the dialog.
//  UINT msg -- Specifies the message sent.
//  WPARAM wParam -- Specifies additional message-specific information.
//  LPARAM lParam -- Specifies additional message-specific information.
//
// Output: TRUE if this callback processed the message, FALSE otherwise.
//
static BOOL CALLBACK ObjectAttributesDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			Registry myRegistry(OBJECT_ATTRIB_KEY);
			theFaceAttributes.hDlg = hWnd;

			DWORD dwTop;
			DWORD dwLeft;
			DWORD dwBottom;
			DWORD dwRight;
			if ((myRegistry.Get(DIALOG_TOP_VAL_NAME, &dwTop) == TRUE) &&
				(myRegistry.Get(DIALOG_LEFT_VAL_NAME, &dwLeft) == TRUE) &&
				(myRegistry.Get(DIALOG_BOTTOM_VAL_NAME, &dwBottom) == TRUE) &&
				(myRegistry.Get(DIALOG_RIGHT_VAL_NAME, &dwRight) == TRUE))
			{
				RECT dialogRect;
				dialogRect.top = (LONG) dwTop;
				dialogRect.bottom = (LONG) dwBottom;
				dialogRect.left = (LONG) dwLeft;
				dialogRect.right = (LONG) dwRight;
				
				RECT desktopRect;
				desktopRect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
				desktopRect.bottom = desktopRect.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
				desktopRect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
				desktopRect.right = desktopRect.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
				
				if (dialogRect.top < desktopRect.top)
				{
					dialogRect.bottom = desktopRect.top + (dialogRect.bottom - dialogRect.top);
					dialogRect.top = desktopRect.top;
				}
				if (dialogRect.right < (desktopRect.left + 50))
				{
					dialogRect.right = desktopRect.left + (dialogRect.right - dialogRect.left);
					dialogRect.left = desktopRect.left;
				}
				if (dialogRect.top > (desktopRect.bottom - 50))
				{
					dialogRect.top = desktopRect.bottom - (dialogRect.bottom - dialogRect.top);
					dialogRect.bottom = desktopRect.bottom;
				}
				if (dialogRect.left > (desktopRect.right - 50))
				{
					dialogRect.left = desktopRect.right - (dialogRect.right - dialogRect.left);
					dialogRect.right = desktopRect.right;
				}
				MoveWindow(theFaceAttributes.hDlg,
					dialogRect.left, dialogRect.top,
					dialogRect.right - dialogRect.left, dialogRect.bottom - dialogRect.top,
					TRUE);
			}
			else
			{
				WinUtil::CenterWindow(theFaceAttributes.hDlg, GetParent(theFaceAttributes.hDlg));
			}
			
			theFaceAttributes.m_hOneToManyButton = GetDlgItem(hWnd, IDC_ONETOMANY);
			theFaceAttributes.m_hReplaceButton = GetDlgItem(hWnd, IDC_REPLACE);
			theFaceAttributes.m_hCopyButton = GetDlgItem(hWnd, IDC_COPY_BUTTON);
			theFaceAttributes.m_hConfigList = GetDlgItem(hWnd, IDC_CFG_LIST);
			theFaceAttributes.m_hBrowseButton = GetDlgItem(hWnd, IDC_BROWSE);
			
			// subclass the buttons to handle the killfocus problem
			SetWindowLongPtr(theFaceAttributes.m_hOneToManyButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(theFaceAttributes.m_hOneToManyButton,GWL_WNDPROC,(LONG)PushButtonProc));
			SetWindowLongPtr(theFaceAttributes.m_hReplaceButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(theFaceAttributes.m_hReplaceButton,GWL_WNDPROC,(LONG)PushButtonProc));
			SetWindowLongPtr(theFaceAttributes.m_hCopyButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(theFaceAttributes.m_hCopyButton,GWL_WNDPROC,(LONG)PushButtonProc));
			SetWindowLongPtr(theFaceAttributes.m_hBrowseButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(theFaceAttributes.m_hBrowseButton,GWL_WNDPROC,(LONG)PushButtonProc));
			
			theFaceAttributes.m_hIconOneToMany = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_ONETOMANY), IMAGE_ICON, 0, 0, 0);
			theFaceAttributes.m_hIconOneToManyDisabled = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_ONETOMANY_DISABLED), IMAGE_ICON, 0, 0, 0);
			theFaceAttributes.m_hIconReplace = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_REPLACE), IMAGE_ICON, 0, 0, 0);
			theFaceAttributes.m_hIconReplaceDisabled = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_REPLACE_DISABLED), IMAGE_ICON, 0, 0, 0);
			theFaceAttributes.m_hIconCopy = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_COPY), IMAGE_ICON, 0, 0, 0);
			theFaceAttributes.m_hIconCopyDisabled = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_COPY_DISABLED), IMAGE_ICON, 0, 0, 0);
			theFaceAttributes.m_hIconBrowse = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_BROWSE), IMAGE_ICON, 0, 0, 0);
			
			::SendMessage(theFaceAttributes.m_hOneToManyButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)theFaceAttributes.m_hIconOneToManyDisabled);
			::SendMessage(theFaceAttributes.m_hReplaceButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)theFaceAttributes.m_hIconReplaceDisabled);
			::SendMessage(theFaceAttributes.m_hCopyButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)theFaceAttributes.m_hIconCopyDisabled);
			::SendMessage(theFaceAttributes.m_hBrowseButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)theFaceAttributes.m_hIconBrowse);
			
			// set up tooltips
			theFaceAttributes.m_hToolTip = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
				CustomControls::GetWindowClassName(CustomControls::TOOLTIP_WINDOW_CLASS), NULL,
				WS_POPUP, 
				0,0,0,0,
				hWnd, NULL, hInstance,
				NULL);

			ICustomToolTip *pToolTip = ICustomToolTip::GetIToolTip(theFaceAttributes.m_hToolTip);
			assert(pToolTip);

			int toolTipBehavior = ICustomToolTip::TOOLTIP_MULTIPLE_SHOW | ICustomToolTip::TOOLTIP_TRACKING_MOUSE | ICustomToolTip::TOOLTIP_CLOSE_LEAVEWND | ICustomToolTip::TOOLTIP_NOCLOSE_OVER;
			int toolTipStyle = ICustomToolTip::TOOLTIP_SHADOW;
			pToolTip->SetBehaviour(toolTipBehavior);
			pToolTip->SetStyles(toolTipStyle);
			pToolTip->SetDelayTime(TTDT_AUTOPOP,0);
			pToolTip->SetDelayTime(TTDT_INITIAL,500);
			pToolTip->SetDelayTime(TTDT_RESHOW,100);

			pToolTip->AddTool(theFaceAttributes.m_hBrowseButton,0);
			pToolTip->AddTool(theFaceAttributes.m_hOneToManyButton,0);
			pToolTip->AddTool(theFaceAttributes.m_hReplaceButton,0);
			pToolTip->AddTool(theFaceAttributes.m_hCopyButton,0);
			pToolTip->AddTool(theFaceAttributes.m_hConfigList,0);
			pToolTip->SetNotify(TRUE);
			pToolTip->SetNotify(theFaceAttributes.hDlg);
			pToolTip->Activate(TRUE);

			ICustomToolTip::ReleaseIToolTip(pToolTip);

			::SendMessage(theFaceAttributes.m_hConfigList,CB_RESETCONTENT,0,0L);
			ConfigFilesManager &configFilesManager = theFaceAttributes.GetConfigFilesManager();
			int numConfigFiles = configFilesManager.GetNumConfigFiles();
			for (int configFileIndex=0;configFileIndex<numConfigFiles;configFileIndex++)
			{
				int comboIndex = ::SendMessage(theFaceAttributes.m_hConfigList,CB_ADDSTRING,0,(LPARAM)((const char *)configFilesManager.GetName(configFileIndex)));
				::SendMessage(theFaceAttributes.m_hConfigList,CB_SETITEMDATA,comboIndex,configFileIndex);
				if (configFileIndex==configFilesManager.GetCurrentConfigIndex())
				{
					::SendMessage(theFaceAttributes.m_hConfigList,CB_SETCURSEL,comboIndex,0L);
				}
			}
			
			// Add the rollup panels to the rollup window:
			// Add the Key/Val panel:
			
			ICustomRollupWindow* pRollupWindow = ICustomRollupWindow::GetICustomRollupWindow(GetDlgItem(theFaceAttributes.hDlg, IDC_CUSTOM_ROLLUP));
			int bufLen = 80;
			static TCHAR applyColorBuf[80];
			LoadString(hInstance, IDS_APPLY_COLORS, applyColorBuf, bufLen);
			theFaceAttributes.InitApplyColorsPanel(pRollupWindow, hInstance, (LPARAM)theFaceAttributes.hDlg);
			static TCHAR keyValBuf[80];
			LoadString(hInstance, IDS_KEYS_AND_VALUES, keyValBuf, bufLen);
			theFaceAttributes.InitKeyValPanel(pRollupWindow, hInstance, (LPARAM)theFaceAttributes.hDlg);
			// Add the Attributes panel:
			static TCHAR attribBuf[80];
			LoadString(hInstance, IDS_ATTRIBUTES, attribBuf, bufLen);
			pRollupWindow->AppendRollup(hInstance, MAKEINTRESOURCE(IDD_ATTRIBUTES),
				AttributesRollupPageProc, attribBuf, (LPARAM)theFaceAttributes.hDlg,
				0);
			
			// set up the state of the panels
			pRollupWindow->Show();
			TupString strAttribTitle = attribBuf;
			TupString strKeyValTitle = keyValBuf;
			TupString strApplyColorsTitle = applyColorBuf;
			int nNumPanels = pRollupWindow->GetNumPanels();
			for (int nIndex = 0; nIndex < nNumPanels; nIndex++)
			{
				HWND hwndCurrPanel = pRollupWindow->GetPanelDlg(nIndex);
				ICustomRollupPanel* pCurrRollupPanel = pRollupWindow->GetPanel(hwndCurrPanel);
				HWND hwndTitle = pCurrRollupPanel->GetTitleWnd();
				int nBufLength = 80;
				TCHAR titleBuf[80];
				GetWindowText(hwndTitle, titleBuf, nBufLength);
				TupString strCurrTitle = titleBuf;
				
				if (strCurrTitle == strAttribTitle)
				{
					// we are at the attrib panel
					DWORD dwAttribPanelOpen;
					if (myRegistry.Get(ATTRIB_PANEL_OPEN_NAME, &dwAttribPanelOpen) == TRUE)
					{
						// the registry entry was found so set state acccordingly:
						if (dwAttribPanelOpen == 0)
							pRollupWindow->SetPanelOpen(nIndex, FALSE);
						else
							pRollupWindow->SetPanelOpen(nIndex, TRUE);
					}
					else
					{
						// set the default open/closed state:
						pRollupWindow->SetPanelOpen(nIndex, TRUE);
					}
				}
				else if (strCurrTitle == strKeyValTitle)
				{
					// We are at the key/val panel:
					DWORD dwKeyValPanelOpen;
					if (myRegistry.Get(KEYVAL_PANEL_OPEN_NAME, &dwKeyValPanelOpen) == TRUE)
					{
						// The registry entry was found so set state acccordingly:
						if (dwKeyValPanelOpen == 0)
							pRollupWindow->SetPanelOpen(nIndex, FALSE);
						else
							pRollupWindow->SetPanelOpen(nIndex, TRUE);
					}
					else
					{
						// Set the default open/closed state:
						pRollupWindow->SetPanelOpen(nIndex, FALSE);
					}
				}
				else if (strCurrTitle == strApplyColorsTitle)
				{
					// We are at the key/val panel:
					DWORD dwApplyColorsPanelOpen;
					if (myRegistry.Get(APPLYCOLORS_PANEL_OPEN_NAME, &dwApplyColorsPanelOpen) == TRUE)
					{
						// The registry entry was found so set state acccordingly:
						if (dwApplyColorsPanelOpen == 0)
							pRollupWindow->SetPanelOpen(nIndex, FALSE);
						else
							pRollupWindow->SetPanelOpen(nIndex, TRUE);
					}
					else
					{
						// Set the default open/closed state:
						pRollupWindow->SetPanelOpen(nIndex, FALSE);
					}
				}
			}
			theFaceAttributes.AdjustPanelHeights();

			// release the rollup window:
			ICustomRollupWindow::ReleaseICustomRollupWindow(pRollupWindow);

			return TRUE;
		}
		case WM_NOTIFY:
			{
				// This is where I provide the tooltip text for the toolbar buttons:
				LPNMHDR hdr = (LPNMHDR)lParam;
				if (hdr->code == ICustomToolTip::UDM_TOOLTIP_DISPLAY)
				{
					ICustomToolTip::ToolTipDisplay *pNotify = (ICustomToolTip::ToolTipDisplay*) hdr;

					CHAR charBuff[200];
					if (pNotify->ti->m_hWnd == theFaceAttributes.m_hOneToManyButton)
					{
						LoadString(hInstance, IDS_ONETOMANY_TOOLTIP, charBuff, sizeof(charBuff));
						pNotify->ti->m_sTooltip = charBuff;
					}
					else if (pNotify->ti->m_hWnd == theFaceAttributes.m_hReplaceButton)
					{
						LoadString(hInstance, IDS_ONETOMANY__REPLACE_TOOLTIP, charBuff, sizeof(charBuff));
						pNotify->ti->m_sTooltip = charBuff;
					}
					else if (pNotify->ti->m_hWnd == theFaceAttributes.m_hCopyButton)
					{
						LoadString(hInstance, IDS_COPY_TOOLTIP, charBuff, sizeof(charBuff));
						pNotify->ti->m_sTooltip = charBuff;
					}
					else if (pNotify->ti->m_hWnd == theFaceAttributes.m_hConfigList)
					{
						LoadString(hInstance, IDS_CFG_LIST_TOOLTIP, charBuff, sizeof(charBuff));
						pNotify->ti->m_sTooltip = charBuff;
					}
					else if (pNotify->ti->m_hWnd == theFaceAttributes.m_hBrowseButton)
					{
						LoadString(hInstance, IDS_BROWSE_TOOLTIP, charBuff, sizeof(charBuff));
						pNotify->ti->m_sTooltip = charBuff;
					}
				}
				return TRUE;
		}
		case WM_GETMINMAXINFO:
		{
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = MIN_DLG_WIDTH;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = MIN_DLG_HEIGHT;
			return FALSE;
		}
		case WM_SIZE:
		{
			// Move the controls:
			// Move the rollup window:
			RECT dialogRect;
			GetClientRect(hWnd, &dialogRect);
			int nDialogWidth = dialogRect.right - dialogRect.left;
			int nDialogHeight = dialogRect.bottom - dialogRect.top;
			// Find the new top:
			HWND rollupHWND = GetDlgItem(hWnd, IDC_CUSTOM_ROLLUP);
			int nRollupTop = SPACE_ABOVE_ROLLUP;
			// Find the new left:
			int nRollupLeft = INSIDE_FRAME_SPACE_ROLLUP;
			// Find the new right:
			int nRollupRight = nDialogWidth - INSIDE_FRAME_SPACE_ROLLUP;
			// Find the new bottom:
			int nRollupBottom = nDialogHeight - INSIDE_FRAME_SPACE_ROLLUP;
			// Move the rollup:
			MoveWindow(rollupHWND, nRollupLeft, nRollupTop,
					   nRollupRight - nRollupLeft, nRollupBottom - nRollupTop, TRUE);
			return FALSE;
		}
	   case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDCANCEL)
			{
				DialogDone();
				return TRUE;
			}
			else if (HIWORD(wParam)==BN_CLICKED)
			{
				switch(LOWORD(wParam))
				{
					case IDC_ONETOMANY:
					{
						theFaceAttributes.PasteAttribsToSelSet(false);
						return TRUE;
					}
					case IDC_REPLACE:
					{
						theFaceAttributes.PasteAttribsToSelSet(true);
						return TRUE;
					}
					case IDC_COPY_BUTTON:
					{
						theFaceAttributes.CopySelection();
						theFaceAttributes.UpdateCopyPasteButtons();
						return TRUE;
					}
					case IDC_BROWSE:
					{
						if (theFaceAttributes.GetConfigFilesManager().EditConfigFileList(theFaceAttributes.hDlg))
						{
							::SendMessage(theFaceAttributes.m_hConfigList,CB_RESETCONTENT,0,0L);
							ConfigFilesManager &configFilesManager = theFaceAttributes.GetConfigFilesManager();
							int numConfigFiles = configFilesManager.GetNumConfigFiles();
							for (int configFileIndex=0;configFileIndex<numConfigFiles;configFileIndex++)
							{
								int comboIndex = ::SendMessage(theFaceAttributes.m_hConfigList,CB_ADDSTRING,0,(LPARAM)((const char *)configFilesManager.GetName(configFileIndex)));
								::SendMessage(theFaceAttributes.m_hConfigList,CB_SETITEMDATA,comboIndex,configFileIndex);
								if (configFileIndex==configFilesManager.GetCurrentConfigIndex())
								{
									::SendMessage(theFaceAttributes.m_hConfigList,CB_SETCURSEL,comboIndex,0L);
								}
							}
							theFaceAttributes.ChangeConfigFile(configFilesManager.GetCurrentConfigIndex());
						}
						return TRUE;
					}
				}
			}
			else if (HIWORD(wParam)==CBN_SELCHANGE)
			{
				if (LOWORD(wParam)==IDC_CFG_LIST)
				{
					int comboBoxSelection = ::SendMessage(theFaceAttributes.m_hConfigList,CB_GETCURSEL,0,0L);
					int configSelection = ::SendMessage(theFaceAttributes.m_hConfigList,CB_GETITEMDATA,comboBoxSelection,0L);
					theFaceAttributes.ChangeConfigFile(configSelection);
				}
			}
			break;
		}
		case WM_DESTROY:
		{
			DialogDone();
			return TRUE;
		}
		case WM_SETCURSOR:
		{
			if (NULL != theFaceAttributes.m_hToolTip)
			{
				WORD message = HIWORD(lParam);

				MSG msg;
				memset(&msg,0,sizeof(MSG));
				POINT pt;
				::GetCursorPos(&pt);   
				msg.hwnd = hWnd;
				msg.message = message;
				msg.wParam = 0;
				msg.lParam = MAKELONG(pt.x, pt.y);//logical unit
				msg.time = ::GetTickCount();
				msg.pt = pt;         
				switch(message)
				{
					case WM_LBUTTONDOWN:
					case WM_LBUTTONDBLCLK:
					case WM_RBUTTONDOWN:
					case WM_RBUTTONDBLCLK:
					case WM_MBUTTONDOWN:
					case WM_MBUTTONDBLCLK:
					case WM_NCLBUTTONDOWN:
					case WM_NCLBUTTONDBLCLK:
					case WM_NCRBUTTONDOWN:
					case WM_NCRBUTTONDBLCLK:
					case WM_NCMBUTTONDOWN:
					case WM_NCMBUTTONDBLCLK:
					case WM_MOUSEMOVE:
					{
						ICustomToolTip *pToolTip = ICustomToolTip::GetIToolTip(theFaceAttributes.m_hToolTip);
						if (pToolTip)
						{
							pToolTip->RelayEvent(&msg);
							ICustomToolTip::ReleaseIToolTip(pToolTip);
						}
					}
					break;
				}
			}
			return FALSE;
		}
		// pass the selchange message to the dialog in the attributes panel
		case WM_SELCHANGE:
		{
			HWND rollupWindowHWND = GetDlgItem(hWnd, IDC_CUSTOM_ROLLUP);
			if (rollupWindowHWND != NULL)
			{
				ICustomRollupWindow* pRollupWindow = ICustomRollupWindow::GetICustomRollupWindow(rollupWindowHWND);
				if (pRollupWindow != NULL)
				{
					int nNumPanels = pRollupWindow->GetNumPanels();
					if (nNumPanels > 0)
					{
						// For now, I only need to resize the attrib panel.  So
						// find the attrib panel in the rollup window:
						TCHAR titleBuf[80];
						int bufLen = 80;
						LoadString(hInstance, IDS_ATTRIBUTES, titleBuf, bufLen);
						std::string strAttribTitle = titleBuf;
						for (int nIndex = 0; nIndex < nNumPanels; nIndex++)
						{
							HWND hwndCurrPanel = pRollupWindow->GetPanelDlg(nIndex);
							ICustomRollupPanel* pCurrRollupPanel = pRollupWindow->GetPanel(hwndCurrPanel);
							HWND hwndTitle = pCurrRollupPanel->GetTitleWnd();
							int nBufLength = 80;
							TCHAR titleBuf[80];
							GetWindowText(hwndTitle, titleBuf, nBufLength);
							std::string strCurrTitle = titleBuf;
							// See if the current panel is the attrib panel:
							if (strCurrTitle == strAttribTitle)
							{
								HWND hDlg = pCurrRollupPanel->GetDlgWnd();
								SendMessage(hDlg,WM_SELCHANGE,wParam,lParam);
							}
						}
					}
				}
			}
			return FALSE;
		}
	}
	return FALSE;
}

//------------------------------------------------------------------------
// --               FaceAttributes::Member Functions                 --
//------------------------------------------------------------------------
FaceAttributes::FaceAttributes()
// default constructor
{
	hDlg		= NULL;
	m_pController	= NULL;
	m_hToolTip = NULL;
	m_hAttributesToolTip = NULL;

	bDialogUp		= false;

	MStatus status;
	m_updateActiveListCallbackID = MModelMessage::addCallback(MModelMessage::kActiveListModified,NotifySelSetChanged,this,&status);
	assert(status);
	m_beforeOpenCallbackID = MSceneMessage::addCallback(MSceneMessage::kBeforeOpen,NotifyBeforeOpen,this,&status);
	assert(status);

	m_pConfigFilesManager = new ConfigFilesManager;
	m_pConfigFilesManager->LoadFromRegistry();
}

//------------------------------------------------------------------------
FaceAttributes::~FaceAttributes()
// default destructor
{
	if (m_pController)
	{
		delete m_pController;
		m_pController = NULL;
	}
	m_pConfigFilesManager->SaveToRegistry();
	delete m_pConfigFilesManager;

	MModelMessage::removeCallback(m_updateActiveListCallbackID);
	MSceneMessage::removeCallback(m_beforeOpenCallbackID);
}

//------------------------------------------------------------------------
void FaceAttributes::SetupAttributes() 
// this method initializes the necessary data in order to support object
// attributes
{
	if (!m_pController)
	{
		m_pController = new FaceAttrController();
	}

	if (m_pConfigFilesManager->NeedToLoadData())
	{
		// save off the nonexclusive keys in the registry
		m_pController->LoadConfigFile(*m_pConfigFilesManager);
	}

	// set up the attributes panel dialog
	if (!hDlg)
	{
		this->InitDialog();
		CWnd *pAppWindow  = AfxGetMainWnd();

		MStatus status;
		HWND hMayaWindow = (HWND)M3dView::applicationShell(&status);

		hDlg = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_FACEATTRIB),
										 hMayaWindow, ObjectAttributesDlgProc,
										 (LPARAM) this);
		bDialogUp = true;
	}
	else
	{
        SetActiveWindow(hDlg);
	}
}

//------------------------------------------------------------------------
void FaceAttributes::InitDialog()
// This method will initialize the object attribute dialog.  Really, the only
// thing done here is the registering of the callback.
//
{
}

//------------------------------------------------------------------------
void FaceAttributes::EndDialog()
// This method will be called when the dialog box is closed.  It unregisters
// the callback for the dialog.
//
{
}

//------------------------------------------------------------------------
bool FaceAttributes::AdjustPanelHeights(bool bUseDefaultSize)
// This method can be called to adjust the panel heights of all panels in the
// rollup window.  Care should be taken to call this method only after the
// m_pUIController has been created in the FaceAttrController object, and after
// all the panels have been added to the rollup window in the dialog.
//
// Input:
//	bool bUseDefaultSize -- Default value is false, pass in true if you want
//							the panel to revert back to the size of the dialog
//							resource in the resource file.
//
// Output: True if succesful, false otherwise. 
//
{
	bool bResult = false;

	if ((hDlg != NULL) && (m_pController != NULL))
	{
		HWND rollupWindowHWND = GetDlgItem(hDlg, IDC_CUSTOM_ROLLUP);
		if (rollupWindowHWND != NULL)
		{
			ICustomRollupWindow* pRollupWindow = ICustomRollupWindow::GetICustomRollupWindow(rollupWindowHWND);
			if (pRollupWindow != NULL)
			{
				int nNumPanels = pRollupWindow->GetNumPanels();
				if (nNumPanels > 0)
				{
					// For now, I only need to resize the attrib panel.  So
					// find the attrib panel in the rollup window:
					TCHAR titleBuf[80];
					int bufLen = 80;
					LoadString(hInstance, IDS_ATTRIBUTES, titleBuf, bufLen);
					TupString strAttribTitle = titleBuf;
					for (int nIndex = 0; nIndex < nNumPanels; nIndex++)
					{
						HWND hwndCurrPanel = pRollupWindow->GetPanelDlg(nIndex);
						ICustomRollupPanel* pCurrRollupPanel = pRollupWindow->GetPanel(hwndCurrPanel);
						HWND hwndTitle = pCurrRollupPanel->GetTitleWnd();
						int nBufLength = 80;
						TCHAR titleBuf[80];
						GetWindowText(hwndTitle, titleBuf, nBufLength);
						TupString strCurrTitle = titleBuf;
						// See if the current panel is the attrib panel:
						if (strCurrTitle == strAttribTitle)
						{
							if (bUseDefaultSize == true)
							{
								int nDefaultResourceHeight = 78;
								pRollupWindow->SetPageDlgHeight(nIndex, nDefaultResourceHeight);
								bResult = true;						
							}
							else
							{
								int nTotalY = m_pController->GetTotalY();
								if (nTotalY > -1)
								{
									pRollupWindow->SetPageDlgHeight(nIndex, nTotalY);
									bResult = true;
								}
							}
						}
					}
				}
			}
			ICustomRollupWindow::ReleaseICustomRollupWindow(pRollupWindow);
		}
	}

	return bResult;
}

//------------------------------------------------------------------------
// This method will tell the m_pController to set up the UIDisplay structure prior
// to the dialog being displayed.
//
// Input:
//  HWND hParentDlg -- The HWND for the parent dialog.
//	HINSTANCE hInstance -- The handle to the application instance.
//  HFONT hUIFont -- Some labels and controls need this font.
//  int nStartx -- The x coord that labels and controls should start at.
//  int nStarty -- The y coord that the first label/control should start at.
//  int nLabelWidth -- The largest allowable width for labels.
//  int nLabelHeight -- The height used when creating labels.
//  int nSpaceBetweenCtrls -- The amount of spacing between controls.
//	WNDPROC newColorSwatchProc -- The new color swatch proc.
//	bool bAdjustPanelHeights -- Default value is true, pass in false if you do
//								not want to adjust the heights of the panels in
//								the rollup window.
//
// Output: True if the actual UI was successfully created, false otherwise.
//
bool FaceAttributes::SetUpUIDisplay(HWND hParentDlg, HINSTANCE hInstance,
									  int nStartx, int nStarty,
									  int nLabelWidth, int nLabelHeight,
									  int nSpaceBetweenCtrls, WNDPROC newColorSwatchProc,
									  bool bAdjustPanelHeights)
{
	bool bResult = true;

	bResult = m_pController->SetUpUIDisplay(hParentDlg, hInstance, m_hAttributesToolTip, 
										 nStartx, nStarty,
										 nLabelWidth, nLabelHeight,
										 nSpaceBetweenCtrls, newColorSwatchProc);
	if (bResult == true)
	{
		// Adjust the height of the rollup page:
		if (bAdjustPanelHeights == true)
		{
			this->AdjustPanelHeights();
		}
	}

	return bResult;
}

//------------------------------------------------------------------------
bool FaceAttributes::HandleWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam)

// This method will take the incoming message and pass it onto the
// FaceAttrController to see if any of its controls want to handle the message.
// The return value from FaceAttrController is then passed back as the return
// value for this method.
//
// Input:
//  HWND hParent -- The parent HWND of the control the message is regarding.
//  UINT message -- Specifies the message.
//  WPARAM wParam -- Specifies additional message information. The contents of
//					 this parameter depend on the value of message. 
//  LPARAM lParam -- Specifies additional message information. The contents of
//					 this parameter depend on the value of message.
//
// Output: True if the message was handled, false otherwise.
//
{
	bool bResult = false;

	if (m_pController != NULL)
	{
		// Pass the message on to the Controller:
		bResult = m_pController->HandleWinMessage(hParent, message, wParam, lParam);
	}
	else
	{
		bResult = false;
	}

	return bResult;
}

//
// This function will draw the lines to the left of the controls
//
bool FaceAttributes::DrawUIDisplayLines(HDC hdc)
{
	enum DrawStyles
	{
		STYLE_BLANK,
		STYLE_TOP_CAP,
		STYLE_VERT_LINE,
		STYLE_VERT_LINE_CONNECT_TALL,
		STYLE_VERT_LINE_CONNECT_SHORT,
		STYLE_BOTTOM_CAP_SHORT,
		STYLE_BOTTOM_CAP_TALL,
	};

	const UIController *pUIController = m_pController->GetUIController();
	if (!pUIController)
		return false;

	int topLeft = CustomControls::GetColor(CustomControls::COLOR_ROLLUP_TITLE_SHADOW);
	int bottomRight = CustomControls::GetColor(CustomControls::COLOR_ROLLUP_TITLE_HILIGHT);

	HPEN tl = CreatePen(PS_SOLID, 0, topLeft);
	HPEN br = CreatePen(PS_SOLID, 0, bottomRight);

	HPEN op = (HPEN)SelectObject(hdc, tl);

	int topTallY = START_Y;
	int topShortY;
	int centerY;
	int bottomY;
	int leftX;
	int rightX;

	int numDisplayObjects = pUIController->GetNumDisplayObjects();
	int displayIndex;
	// calculate the max indent level
	int maxIndentLevel = 0;
	for (displayIndex = 0;displayIndex<numDisplayObjects;displayIndex++)
	{
		const UIDisplayBase *pDisplayObject = pUIController->GetDisplayObject(displayIndex);
		if (pDisplayObject)
		{
			int indentLevel = pDisplayObject->GetIndentLevel();
			if (indentLevel>maxIndentLevel)
			{
				maxIndentLevel = indentLevel;
			}
		}
	}
	TupArray<int> parentObjectIndexes;
	parentObjectIndexes.SetSize(numDisplayObjects);
	TupArray<int> nextSiblingIndexes;
	nextSiblingIndexes.SetSize(numDisplayObjects);

	TupArray<int> lastParents;
	lastParents.SetSize(maxIndentLevel+1);

	for (displayIndex = 0;displayIndex<numDisplayObjects;displayIndex++)
	{
		parentObjectIndexes[displayIndex] = -1;
		nextSiblingIndexes[displayIndex] = -1;

		const UIDisplayBase *pDisplayObject = pUIController->GetDisplayObject(displayIndex);

		int indentLevel = pDisplayObject->GetIndentLevel();
		// not the first one
		if (displayIndex>0)
		{
			int prevIndentLevel = pUIController->GetDisplayObject(displayIndex-1)->GetIndentLevel();
			// first child
			if (prevIndentLevel<indentLevel)
			{
				lastParents[indentLevel] = displayIndex;
			}
			else // has a previous sibling
			{
				nextSiblingIndexes[lastParents[indentLevel]] = displayIndex;
			}
			parentObjectIndexes[displayIndex] = lastParents[indentLevel-1];
		}
		lastParents[indentLevel] = displayIndex;
	}

	for (displayIndex = 0;displayIndex<numDisplayObjects;displayIndex++)
	{
		const UIDisplayBase *pDisplayObject = pUIController->GetDisplayObject(displayIndex);
		topShortY = pDisplayObject->GetTopY()-4;
		centerY = pDisplayObject->GetCenterY();
		bottomY = pDisplayObject->GetBottomY();
		int indentLevel = pDisplayObject->GetIndentLevel();

		DrawStyles style;

		int parentIndex = displayIndex; // start with 

		for (int indentIndex=indentLevel;indentIndex>=0;indentIndex--)
		{
			leftX = (START_X-10)+(indentIndex * INDENT_SCALER);
			rightX = leftX+5;
			// columns left of control
			if (indentIndex<indentLevel)
			{
				if (nextSiblingIndexes[parentIndex]!=-1)
				{
					style = STYLE_VERT_LINE;
				}
				else
				{
					style = STYLE_BLANK;
				}
			}
			// column next to control
			else
			{
				// first one
				if (displayIndex==0)
				{
					// no more siblings?
					if (nextSiblingIndexes[parentIndex]==-1)
					{
						style = STYLE_BLANK;
					}
					else
					{
						style = STYLE_TOP_CAP;
					}
				}
				else
				{
					int prevIndentLevel = pUIController->GetDisplayObject(displayIndex-1)->GetIndentLevel();
					if (prevIndentLevel<indentLevel)
					{
						if (nextSiblingIndexes[parentIndex]!=-1)
						{
							style = STYLE_VERT_LINE_CONNECT_SHORT;
						}
						else
						{
							style = STYLE_BOTTOM_CAP_SHORT;
						}
					}
					else
					{
						if (nextSiblingIndexes[parentIndex]!=-1)
						{
							style = STYLE_VERT_LINE_CONNECT_TALL;
						}
						else
						{
							style = STYLE_BOTTOM_CAP_TALL;
						}
					}
				}
			}
			switch (style)
			{
				case STYLE_BLANK:
					break;
				case STYLE_TOP_CAP:
					MoveToEx(hdc, rightX, centerY, NULL);
					LineTo(hdc, leftX, centerY);
					LineTo(hdc, leftX, bottomY);
					SelectObject(hdc, br);
					MoveToEx(hdc, rightX, centerY+1, NULL);
					LineTo(hdc, leftX+1, centerY+1);
					LineTo(hdc, leftX+1, bottomY);
					SelectObject(hdc, tl);
					break;
				case STYLE_VERT_LINE:
					MoveToEx(hdc, leftX, topTallY, NULL);
					LineTo(hdc, leftX, bottomY);
					SelectObject(hdc, br);
					MoveToEx(hdc, leftX+1, topTallY, NULL);
					LineTo(hdc, leftX+1, bottomY);
					SelectObject(hdc, tl);
					break;
				case STYLE_VERT_LINE_CONNECT_TALL:
					MoveToEx(hdc, leftX, topTallY, NULL);
					LineTo(hdc, leftX, bottomY);
					MoveToEx(hdc, leftX+1, centerY, NULL);
					LineTo(hdc, rightX+1, centerY);
					SelectObject(hdc, br);
					MoveToEx(hdc, leftX+1, topTallY, NULL);
					LineTo(hdc, leftX+1, centerY);
					MoveToEx(hdc, rightX, centerY+1, NULL);
					LineTo(hdc, leftX+1, centerY+1);
					LineTo(hdc, leftX+1, bottomY);
					SelectObject(hdc, tl);
					break;
				case STYLE_VERT_LINE_CONNECT_SHORT:
					MoveToEx(hdc, leftX, topShortY, NULL);
					LineTo(hdc, leftX, bottomY);
					MoveToEx(hdc, leftX+1, centerY, NULL);
					LineTo(hdc, rightX+1, centerY);
					SelectObject(hdc, br);
					MoveToEx(hdc, leftX+1, topShortY, NULL);
					LineTo(hdc, leftX+1, centerY);
					MoveToEx(hdc, rightX, centerY+1, NULL);
					LineTo(hdc, leftX+1, centerY+1);
					LineTo(hdc, leftX+1, bottomY);
					SelectObject(hdc, tl);
					break;
				case STYLE_BOTTOM_CAP_SHORT:
					MoveToEx(hdc, leftX, topShortY, NULL);
					LineTo(hdc, leftX, centerY);
					LineTo(hdc, rightX+1, centerY);
					SelectObject(hdc, br);
					MoveToEx(hdc, leftX+1, topShortY, NULL);
					LineTo(hdc, leftX+1, centerY);
					MoveToEx(hdc, leftX+1, centerY+1, NULL);
					LineTo(hdc, rightX+1, centerY+1);
					SelectObject(hdc, tl);
					break;
				case STYLE_BOTTOM_CAP_TALL:
					MoveToEx(hdc, leftX, topTallY, NULL);
					LineTo(hdc, leftX, centerY);
					LineTo(hdc, rightX+1, centerY);
					SelectObject(hdc, br);
					MoveToEx(hdc, leftX+1, topTallY, NULL);
					LineTo(hdc, leftX+1, centerY);
					MoveToEx(hdc, leftX, centerY+1, NULL);
					LineTo(hdc, rightX+1, centerY+1);
					SelectObject(hdc, tl);
					break;
			}
			parentIndex = parentIndex!=-1 ? parentObjectIndexes[parentIndex] : -1;
		}
		topTallY = bottomY;
	}
	SelectObject(hdc, op);
	DeleteObject(tl);
	DeleteObject(br);
	return true;
}

//------------------------------------------------------------------------
bool FaceAttributes::LoadCurrAttribData()
// FaceAttributes::LoadCurrAttribData
// This method will tell the m_pController to build the CurrAttribData based on
// the selection set passed in through the Interface pointer.
//
// Output: Passes back the return value from the m_pController.  In this case
//		   true if there are objects in the selection set, false otherwise.
//
{
	return m_pController->LoadCurrAttribData();
}

bool FaceAttributes::OkToCopy(void) const
{
	if (m_pController != NULL)
	{
		return m_pController->OkToCopy();
	}
	return false;
}

bool FaceAttributes::OkToPaste(void) const
{
	if (m_pController != NULL)
	{
		return m_pController->OkToPaste();
	}
	return false;
}

bool FaceAttributes::CopySelection(void)
{
	if (m_pController != NULL)
	{
		return m_pController->CopySelection();
	}
	return false;
}

//-----------------------------------------------------------------------------
// FaceAttributes::PasteAttribsToSelSet
// This method will tell the m_pController to copy the attributes from the node
// passed in to the nodes in the current selection set (which should be
// currently stored in the CurrAttribController).
//
// Input:
//  INode* pNode -- The node to get the attributes from.
//	bool bReplace -- True if the destination nodes should have all of their
//					 attributes thrown away and replaced with the source node's
//					 attributes.  Default value is false.
//
// Output: Passes back the return value from the m_pController, or false if it is
//		   NULL.
//-----------------------------------------------------------------------------
bool FaceAttributes::PasteAttribsToSelSet(bool bReplace)
{
	bool bResult = false;
	
	if (m_pController != NULL)
	{
		bResult =  m_pController->PasteAttribsToSelSet(bReplace);
		if (bResult == true)
		{
			// request for selection
			SendMessage(theFaceAttributes.hDlg,WM_SELCHANGE,1,0);
		}
	}
	return bResult;
}


//------------------------------------------------------------------------
void FaceAttributes::DestroyCurrAttribData()

// This method will tell the m_pController to destroy any current attribute data.
//
// Input: None.
//
// Output: None.
//
{
	m_pController->DestroyCurrAttribData();
}

//------------------------------------------------------------------------
void FaceAttributes::DestroyUIDisplay()
// This method will tell the m_pController to destroy any UIDisplay.
//
// Input: None.
//
// Output: None.
//
{
	m_pController->DestroyUIDisplay();
}

//-----------------------------------------------------------------------------
void FaceAttributes::SaveNonexclusiveKeys()
// This method will the m_pController to save the nonexclusive keys to the
// registry.
//
// Input: None.
//
// Output: None.
//
{
	m_pController->SaveNonexclusiveKeys();
}

//-----------------------------------------------------------------------------
// FaceAttributes::InitKeyValPanel
// This method will tell the m_pController to set up the KeyVal panel.
//
// Input:
//	IRollupWindow* pRollupWindow -- The rollup window that the panel will be
//									appended to.
//  HINSTANCE hInstance -- The INSTANCE to use for the keyval panel creation.
//  LPARAM lParam -- The LPARAM to use for the keyval panel creation.
//	int nCategory -- The category to use for the keyval panel order.
//
// Output: None.
//-----------------------------------------------------------------------------
void FaceAttributes::InitKeyValPanel(ICustomRollupWindow* pCustomRollupWindow, HINSTANCE hInstance, LPARAM lParam)
{
	m_pController->InitKeyValPanel(pCustomRollupWindow, hInstance, lParam);
}

//-----------------------------------------------------------------------------
// FaceAttributes::InitApplyColorsPanel
// This method will tell the m_pController to set up the ApplyColors panel.
//
// Input:
//	IRollupWindow* pRollupWindow -- The rollup window that the panel will be
//									appended to.
//  HINSTANCE hInstance -- The INSTANCE to use for the keyval panel creation.
//  LPARAM lParam -- The LPARAM to use for the keyval panel creation.
//	int nCategory -- The category to use for the keyval panel order.
//
// Output: None.
//-----------------------------------------------------------------------------
void FaceAttributes::InitApplyColorsPanel(ICustomRollupWindow* pCustomRollupWindow, HINSTANCE hInstance, LPARAM lParam)
{
	m_pController->InitApplyColorsPanel(pCustomRollupWindow, hInstance, lParam);
}


//-----------------------------------------------------------------------------
// FaceAttributes::IdCancelMessage
// This method will notify the m_pController that the IDCANCEL message was
// received.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void FaceAttributes::IdCancelMessage()
{
	m_pController->IdCancelMessage();
}

void FaceAttributes::UpdateCopyPasteButtons(void)
{
	if (OkToCopy())
	{
		::EnableWindow(m_hCopyButton,TRUE);
		::SendMessage(m_hCopyButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hIconCopy);
	}
	else
	{
		::EnableWindow(m_hCopyButton,FALSE);
		::SendMessage(m_hCopyButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hIconCopyDisabled);
	}

	if (OkToPaste())
	{
		::EnableWindow(m_hOneToManyButton,TRUE);
		::EnableWindow(m_hReplaceButton,TRUE);
		::SendMessage(m_hOneToManyButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hIconOneToMany);
		::SendMessage(m_hReplaceButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hIconReplace);
	}
	else
	{
		::EnableWindow(m_hOneToManyButton,FALSE);
		::EnableWindow(m_hReplaceButton,FALSE);
		::SendMessage(m_hOneToManyButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hIconOneToManyDisabled);
		::SendMessage(m_hReplaceButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hIconReplaceDisabled);
	}
}

//-----------------------------------------------------------------------------
// FaceAttributes::SetApplyBttnStates
// This method will tell the m_pController to set the apply button states for the
// key/val panel.
//
// Input:
//  bool bApplySelEnabled -- True if the Apply To Sel button is to be enabled.
//  bool bApplySceneEnabled -- True if the Apply To Scene button is to be
//							   enabled.
//
// Output: None.
//-----------------------------------------------------------------------------
void FaceAttributes::SetApplyBttnStates(bool bApplySelEnabled, bool bApplySceneEnabled)
{
	m_pController->SetApplyBttnStates(bApplySelEnabled, bApplySceneEnabled);
}


//-----------------------------------------------------------------------------
// FaceAttributes::LoadKeyValPanelTree
// This method will tell the m_pController to load the key/vals in the key/val
// panel.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void FaceAttributes::LoadKeyValPanelTree()
{
	m_pController->LoadKeyValPanelTree();
}

ConfigFilesManager &FaceAttributes::GetConfigFilesManager(void)
{
	return *m_pConfigFilesManager;
}

void FaceAttributes::ChangeConfigFile(int selectionIndex)
{
	m_pConfigFilesManager->SetCurrentConfigIndex(selectionIndex);
	NotifySelSetChanged(NULL);
}

void FaceAttributes::RefreshConfig(void)
{
	if (m_pConfigFilesManager->NeedToLoadData())
	{
		m_pController->SaveNonexclusiveKeys();
		SetupAttributes();
	}
}