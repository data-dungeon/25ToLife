// precomp'ed header files
#include "StdAfx.h"

#include "ObjectAttributes.h"
#include "ObjAttrController.h"
#include "ConfigFilesManager.h"
#include "CurrAttribController.h"

using namespace AttrControls;

//-----------------------------------------------------------------------------
// Global objects and functions:

ObjectAttributes theObjectAttributes;

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
			if (theObjectAttributes.HandleWinMessage(hwnd, uMsg, wParam, lParam) == true)
			{
				// We don't need to rebuild the UIDisplay for this message!
				// But we do need to load the key/val panel:
				theObjectAttributes.LoadKeyValPanelTree();
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
			theObjectAttributes.m_hAttributesToolTip = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
				CustomControls::GetWindowClassName(CustomControls::TOOLTIP_WINDOW_CLASS), NULL,
				WS_POPUP, 
				0,0,0,0,
				hDlg, NULL, hInstance,
				NULL);

			ICustomToolTip *pToolTip = ICustomToolTip::GetIToolTip(theObjectAttributes.m_hAttributesToolTip);

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
			if (theObjectAttributes.LoadCurrAttribData() == true)
			{
				// hide the "No objects" label
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC_SELOBJECTS), SW_HIDE);

				// Enable the "Copy Attributes" buttons:
				theObjectAttributes.UpdateCopyPasteButtons();
				
				// Enable the apply buttons in the key/val panel:
				theObjectAttributes.SetApplyBttnStates(true, true);
				
				// Load the key/val panel:
				theObjectAttributes.LoadKeyValPanelTree();

				// Set up the UI Display:
				theObjectAttributes.SetUpUIDisplay(hDlg, hInstance,
												   START_X, START_Y,
												   LABEL_WD, LABEL_HT,
												   SPACE_BET_CTRLS, colorSwatchSubclassWndProc,false);

				// place # of selected objects in the stats
				int numSelectedObjects = theObjectAttributes.NumSelectedNodes();
				TupString stats;
				stats.Format("%d Selected",numSelectedObjects);
				SetWindowText(theObjectAttributes.m_hStatusStaticText,(const char *)stats);
			}
			else
			{
				// show the "No objects" label
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC_SELOBJECTS), SW_SHOW);

				// Enable the "Copy Attributes" buttons:
				theObjectAttributes.UpdateCopyPasteButtons();
				
				// Disable the apply buttons in the key/val panel:
				theObjectAttributes.SetApplyBttnStates(false, false);

				SetWindowText(theObjectAttributes.m_hStatusStaticText,"0 Selected");
			}

			return TRUE;
		}
		case WM_PAINT:
			{
				// draw the tree lines
				PAINTSTRUCT ps;
				HDC hdc;			
				hdc = BeginPaint( hDlg, &ps );
				theObjectAttributes.DrawUIDisplayLines(hdc);
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

						if (theObjectAttributes.HandleWinMessage(hDlg, message, wParam, lParam) == true)
						{
							// Set up the UI Display:
							theObjectAttributes.SetUpUIDisplay(hDlg, hInstance,
								START_X, START_Y,
								LABEL_WD, LABEL_HT,
								SPACE_BET_CTRLS, colorSwatchSubclassWndProc);
							
							// Load the key/val panel:
							theObjectAttributes.LoadKeyValPanelTree();
							
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
						if (theObjectAttributes.HandleWinMessage(hDlg, message, wParam, lParam) == true)
						{
							// We don't need to rebuild the UIDisplay for this message!
							// But we do need to load the key/val panel:
							theObjectAttributes.LoadKeyValPanelTree();
						}
					}
					break;
					// if this is a toolbar message
					case ICustomToolTip::UDM_TOOLTIP_DISPLAY:
					{
						// send to control that deals with it
						theObjectAttributes.HandleWinMessage(hDlg, message, wParam, lParam);
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
			if (theObjectAttributes.HandleWinMessage(hDlg, message, wParam, lParam) == true)
			{
				// Set up the UI Display:
				theObjectAttributes.SetUpUIDisplay(hDlg, hInstance,
					START_X, START_Y,
					LABEL_WD, LABEL_HT,
					SPACE_BET_CTRLS, colorSwatchSubclassWndProc);

				// We don't need to rebuild the UIDisplay for this message!
				// But we do need to load the key/val panel:
				theObjectAttributes.LoadKeyValPanelTree();
				
			}
			return FALSE;
		}
		case WM_SETCURSOR:
		{
			if (NULL != theObjectAttributes.m_hAttributesToolTip)
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
						ICustomToolTip *pToolTip = ICustomToolTip::GetIToolTip(theObjectAttributes.m_hAttributesToolTip);
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
			theObjectAttributes.RefreshConfig();
			int request = wParam; // 0 - no selection 1-check selection
			// there are any selected and we request it
			if (request && theObjectAttributes.LoadCurrAttribData() == true)
			{
				// hide the "No objects" label
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC_SELOBJECTS), SW_HIDE);

				// Enable the "Copy Attributes" buttons:
				theObjectAttributes.UpdateCopyPasteButtons();

				// Enable the apply buttons in the key/val panel:
				theObjectAttributes.SetApplyBttnStates(true, true);

				// Set up the UI Display:
				theObjectAttributes.SetUpUIDisplay(hDlg, hInstance,
					START_X, START_Y,
					LABEL_WD, LABEL_HT,
					SPACE_BET_CTRLS, colorSwatchSubclassWndProc);

				// place # of selected objects in the stats
				int numSelectedObjects = theObjectAttributes.NumSelectedNodes();
				TupString stats;
				stats.Format("%d Selected",numSelectedObjects);
				SetWindowText(theObjectAttributes.m_hStatusStaticText,(const char *)stats);
			}
			else
			{
				// show the "No objects" label
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC_SELOBJECTS), SW_SHOW);

				// Enable the "Copy Attributes" buttons:
				theObjectAttributes.UpdateCopyPasteButtons();

				// Disable the apply buttons in the key/val panel:
				theObjectAttributes.SetApplyBttnStates(false, false);

				// Destroy any CurrAttribData and UIDisplay:
				theObjectAttributes.DestroyCurrAttribData();
				theObjectAttributes.DestroyUIDisplay();

				// adjust the panel heights
				theObjectAttributes.AdjustPanelHeights(true);

				SetWindowText(theObjectAttributes.m_hStatusStaticText,"0 Selected");
			}

			// Load the key/val panel:
			theObjectAttributes.LoadKeyValPanelTree();
			return FALSE;
		}
	}
	return FALSE;
}

void ObjectAttributes::NotifySelSetChanged( void )
{
	if (bDialogUp == true)
	{
		// request for selection
		SendMessage(hDlg,WM_SELCHANGE,1,0);
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
	if (!theObjectAttributes.hDlg) // all ready done?
		return;

	// save off the position of the dialog
	RECT dialogRect;
	GetWindowRect(theObjectAttributes.hDlg, &dialogRect);
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
	ICustomRollupWindow* pRollupWindow = ICustomRollupWindow::GetICustomRollupWindow(GetDlgItem(theObjectAttributes.hDlg, IDC_CUSTOM_ROLLUP));
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
	}

	// save off the nonexclusive keys in the registry
	theObjectAttributes.SaveNonexclusiveKeys();

	// Now clean up:
	// Release any controls:
	if (theObjectAttributes.m_hIconOneToMany)
		::DestroyIcon(theObjectAttributes.m_hIconOneToMany);
	if (theObjectAttributes.m_hIconOneToManyDisabled)
		::DestroyIcon(theObjectAttributes.m_hIconOneToManyDisabled);
	if (theObjectAttributes.m_hIconReplace)
		::DestroyIcon(theObjectAttributes.m_hIconReplace);
	if (theObjectAttributes.m_hIconReplaceDisabled)
		::DestroyIcon(theObjectAttributes.m_hIconReplaceDisabled);
	if (theObjectAttributes.m_hIconBrowse)
		::DestroyIcon(theObjectAttributes.m_hIconBrowse);

	// Clean up things dealing with the keyval panel:
	theObjectAttributes.IdCancelMessage();

	// Unregister the rollup callback:
	ICustomRollupWindow::ReleaseICustomRollupWindow(pRollupWindow);

	theObjectAttributes.bDialogUp = false;

	EndDialog(theObjectAttributes.hDlg, FALSE);
	theObjectAttributes.DestroyUIDisplay();
	theObjectAttributes.DestroyCurrAttribData();
	theObjectAttributes.hDlg = NULL;
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
			theObjectAttributes.hDlg = hWnd;

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
				MoveWindow(theObjectAttributes.hDlg,
					dialogRect.left, dialogRect.top,
					dialogRect.right - dialogRect.left, dialogRect.bottom - dialogRect.top,
					TRUE);
			}
			else
			{
				WinUtil::CenterWindow(theObjectAttributes.hDlg, GetParent(theObjectAttributes.hDlg));
			}
			
			theObjectAttributes.m_hOneToManyButton = GetDlgItem(hWnd, IDC_ONETOMANY);
			theObjectAttributes.m_hReplaceButton = GetDlgItem(hWnd, IDC_REPLACE);
			theObjectAttributes.m_hCopyButton = GetDlgItem(hWnd, IDC_COPY_BUTTON);
			theObjectAttributes.m_hConfigList = GetDlgItem(hWnd, IDC_CFG_LIST);
			theObjectAttributes.m_hBrowseButton = GetDlgItem(hWnd, IDC_BROWSE);
			theObjectAttributes.m_hPurgeUnusedButton = GetDlgItem(hWnd, IDC_PURGE_UNUSED);
			theObjectAttributes.m_hFindButton = GetDlgItem(hWnd, IDC_FIND);

			theObjectAttributes.m_hStatusStaticText = GetDlgItem(hWnd, IDC_STATS);

			theObjectAttributes.m_oldScrollPos = -1;

			
			// subclass the buttons to handle the killfocus problem
			SetWindowLongPtr(theObjectAttributes.m_hOneToManyButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(theObjectAttributes.m_hOneToManyButton,GWL_WNDPROC,(LONG)PushButtonProc));
			SetWindowLongPtr(theObjectAttributes.m_hReplaceButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(theObjectAttributes.m_hReplaceButton,GWL_WNDPROC,(LONG)PushButtonProc));
			SetWindowLongPtr(theObjectAttributes.m_hCopyButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(theObjectAttributes.m_hCopyButton,GWL_WNDPROC,(LONG)PushButtonProc));
			SetWindowLongPtr(theObjectAttributes.m_hBrowseButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(theObjectAttributes.m_hBrowseButton,GWL_WNDPROC,(LONG)PushButtonProc));
			SetWindowLongPtr(theObjectAttributes.m_hPurgeUnusedButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(theObjectAttributes.m_hPurgeUnusedButton,GWL_WNDPROC,(LONG)PushButtonProc));
			SetWindowLongPtr(theObjectAttributes.m_hFindButton,GWLP_USERDATA,(LONG)SetWindowLongPtr(theObjectAttributes.m_hFindButton,GWL_WNDPROC,(LONG)PushButtonProc));
			
			theObjectAttributes.m_hIconOneToMany = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_ONETOMANY), IMAGE_ICON, 0, 0, 0);
			theObjectAttributes.m_hIconOneToManyDisabled = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_ONETOMANY_DISABLED), IMAGE_ICON, 0, 0, 0);
			theObjectAttributes.m_hIconReplace = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_REPLACE), IMAGE_ICON, 0, 0, 0);
			theObjectAttributes.m_hIconReplaceDisabled = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_REPLACE_DISABLED), IMAGE_ICON, 0, 0, 0);
			theObjectAttributes.m_hIconCopy = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_COPY), IMAGE_ICON, 0, 0, 0);
			theObjectAttributes.m_hIconCopyDisabled = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_COPY_DISABLED), IMAGE_ICON, 0, 0, 0);
			theObjectAttributes.m_hIconBrowse = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_BROWSE), IMAGE_ICON, 0, 0, 0);
			theObjectAttributes.m_hIconPurgeUnused = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_PURGE_UNUSED), IMAGE_ICON, 0, 0, 0);
			theObjectAttributes.m_hIconFind = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(IDI_FIND), IMAGE_ICON, 0, 0, 0);
			
			::SendMessage(theObjectAttributes.m_hOneToManyButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)theObjectAttributes.m_hIconOneToManyDisabled);
			::SendMessage(theObjectAttributes.m_hReplaceButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)theObjectAttributes.m_hIconReplaceDisabled);
			::SendMessage(theObjectAttributes.m_hCopyButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)theObjectAttributes.m_hIconCopyDisabled);
			::SendMessage(theObjectAttributes.m_hBrowseButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)theObjectAttributes.m_hIconBrowse);
			::SendMessage(theObjectAttributes.m_hPurgeUnusedButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)theObjectAttributes.m_hIconPurgeUnused);
			::SendMessage(theObjectAttributes.m_hFindButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)theObjectAttributes.m_hIconFind);
			
			// set up tooltips
			theObjectAttributes.m_hToolTip = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
															CustomControls::GetWindowClassName(CustomControls::TOOLTIP_WINDOW_CLASS), NULL,
															WS_POPUP, 
															0,0,0,0,
															hWnd, NULL, hInstance,
															NULL);

			ICustomToolTip *pToolTip = ICustomToolTip::GetIToolTip(theObjectAttributes.m_hToolTip);
			assert(pToolTip);

			int toolTipBehavior = ICustomToolTip::TOOLTIP_MULTIPLE_SHOW | ICustomToolTip::TOOLTIP_TRACKING_MOUSE | ICustomToolTip::TOOLTIP_CLOSE_LEAVEWND | ICustomToolTip::TOOLTIP_NOCLOSE_OVER;
			int toolTipStyle = ICustomToolTip::TOOLTIP_SHADOW;
			pToolTip->SetBehaviour(toolTipBehavior);
			pToolTip->SetStyles(toolTipStyle);
			pToolTip->SetDelayTime(TTDT_AUTOPOP,0);
			pToolTip->SetDelayTime(TTDT_INITIAL,500);
			pToolTip->SetDelayTime(TTDT_RESHOW,100);

			pToolTip->AddTool(theObjectAttributes.m_hBrowseButton,0);
			pToolTip->AddTool(theObjectAttributes.m_hOneToManyButton,0);
			pToolTip->AddTool(theObjectAttributes.m_hReplaceButton,0);
			pToolTip->AddTool(theObjectAttributes.m_hCopyButton,0);
			pToolTip->AddTool(theObjectAttributes.m_hConfigList,0);
			pToolTip->AddTool(theObjectAttributes.m_hPurgeUnusedButton,0);
			pToolTip->AddTool(theObjectAttributes.m_hFindButton,0);
			pToolTip->SetNotify(TRUE);
			pToolTip->SetNotify(theObjectAttributes.hDlg);
			pToolTip->Activate(TRUE);

			ICustomToolTip::ReleaseIToolTip(pToolTip);

			::SendMessage(theObjectAttributes.m_hConfigList,CB_RESETCONTENT,0,0L);
			ConfigFilesManager &configFilesManager = theObjectAttributes.GetConfigFilesManager();
			int numConfigFiles = configFilesManager.GetNumConfigFiles();
			for (int configFileIndex=0;configFileIndex<numConfigFiles;configFileIndex++)
			{
				int comboIndex = ::SendMessage(theObjectAttributes.m_hConfigList,CB_ADDSTRING,0,(LPARAM)((const char *)configFilesManager.GetName(configFileIndex)));
				::SendMessage(theObjectAttributes.m_hConfigList,CB_SETITEMDATA,comboIndex,configFileIndex);
				if (configFileIndex==configFilesManager.GetCurrentConfigIndex())
				{
					::SendMessage(theObjectAttributes.m_hConfigList,CB_SETCURSEL,comboIndex,0L);
				}
			}
			
			// Add the rollup panels to the rollup window:
			// Add the Key/Val panel:
			
			ICustomRollupWindow* pRollupWindow = ICustomRollupWindow::GetICustomRollupWindow(GetDlgItem(theObjectAttributes.hDlg, IDC_CUSTOM_ROLLUP));
			int bufLen = 80;
			static TCHAR keyValBuf[80];
			LoadString(hInstance, IDS_KEYS_AND_VALUES, keyValBuf, bufLen);
			theObjectAttributes.InitKeyValPanel(pRollupWindow, hInstance, (LPARAM)theObjectAttributes.hDlg);
			// Add the Attributes panel:
			static TCHAR attribBuf[80];
			LoadString(hInstance, IDS_ATTRIBUTES, attribBuf, bufLen);
			pRollupWindow->AppendRollup(hInstance, MAKEINTRESOURCE(IDD_ATTRIBUTES),
				AttributesRollupPageProc, attribBuf, (LPARAM)theObjectAttributes.hDlg,
				0);
			
			// set up the state of the panels
			pRollupWindow->Show();
			TupString strAttribTitle = attribBuf;
			TupString strKeyValTitle = keyValBuf;
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
			}
			theObjectAttributes.AdjustPanelHeights();

			// release the rollup window:
			ICustomRollupWindow::ReleaseICustomRollupWindow(pRollupWindow);

			HMENU hSysMenu = ::GetSystemMenu(hWnd,FALSE);
			if (hSysMenu != NULL)
			{
				::AppendMenu(hSysMenu,MF_SEPARATOR,NULL,NULL);
				::AppendMenu(hSysMenu,MF_STRING,IDM_ABOUTBOX,"&About ObjAttrib...");
			}

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
				if (pNotify->ti->m_hWnd == theObjectAttributes.m_hOneToManyButton)
				{
					LoadString(hInstance, IDS_ONETOMANY_TOOLTIP, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
				else if (pNotify->ti->m_hWnd == theObjectAttributes.m_hReplaceButton)
				{
					LoadString(hInstance, IDS_ONETOMANY__REPLACE_TOOLTIP, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
				else if (pNotify->ti->m_hWnd == theObjectAttributes.m_hCopyButton)
				{
					LoadString(hInstance, IDS_COPY_TOOLTIP, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
				else if (pNotify->ti->m_hWnd == theObjectAttributes.m_hConfigList)
				{
					LoadString(hInstance, IDS_CFG_LIST_TOOLTIP, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
				else if (pNotify->ti->m_hWnd == theObjectAttributes.m_hBrowseButton)
				{
					LoadString(hInstance, IDS_BROWSE_TOOLTIP, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
				else if (pNotify->ti->m_hWnd == theObjectAttributes.m_hPurgeUnusedButton)
				{
					LoadString(hInstance, IDS_PURGE_UNUSED_TOOLTIP, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
				else if (pNotify->ti->m_hWnd == theObjectAttributes.m_hFindButton)
				{
					LoadString(hInstance, IDS_FIND, charBuff, sizeof(charBuff));
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
			RECT dialogRect;
			GetClientRect(hWnd, &dialogRect);
			int nDialogWidth = dialogRect.right - dialogRect.left;
			int nDialogHeight = dialogRect.bottom - dialogRect.top;

			// Move the rollup window:
			// Find the new top:
			HWND rollupHWND = GetDlgItem(hWnd, IDC_CUSTOM_ROLLUP);
			int nRollupTop = ROLLUP_TOP;
			int nRollupLeft = ROLLUP_LEFT;
			int nRollupRight = nDialogWidth - ROLLUP_RIGHT;
			int nRollupBottom = nDialogHeight - ROLLUP_BOTTOM;
			// Move the rollup:
			MoveWindow(rollupHWND, nRollupLeft, nRollupTop,
					   nRollupRight - nRollupLeft, nRollupBottom - nRollupTop, TRUE);

			// Move the status line:
			HWND statsHWND = GetDlgItem(hWnd, IDC_STATS);
			int nStatsTop = nDialogHeight - STATS_TOP;
			int nStatsLeft = STATS_LEFT;
			int nStatsRight = nDialogWidth - STATS_RIGHT;
			int nStatsBottom = nDialogHeight - STATS_BOTTOM;
			// Move the rollup:
			MoveWindow(statsHWND, nStatsLeft, nStatsTop,
				nStatsRight - nStatsLeft, nStatsBottom - nStatsTop, TRUE);

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
						theObjectAttributes.PasteAttribsToSelSet(false);
						return TRUE;
					}
					case IDC_REPLACE:
					{
						theObjectAttributes.PasteAttribsToSelSet(true);
						return TRUE;
					}
					case IDC_COPY_BUTTON:
					{
						theObjectAttributes.CopySelection();
						theObjectAttributes.UpdateCopyPasteButtons();
						return TRUE;
					}
					case IDC_BROWSE:
					{
						if (theObjectAttributes.GetConfigFilesManager().EditConfigFileList(theObjectAttributes.hDlg))
						{
							::SendMessage(theObjectAttributes.m_hConfigList,CB_RESETCONTENT,0,0L);
							ConfigFilesManager &configFilesManager = theObjectAttributes.GetConfigFilesManager();
							int numConfigFiles = configFilesManager.GetNumConfigFiles();
							for (int configFileIndex=0;configFileIndex<numConfigFiles;configFileIndex++)
							{
								int comboIndex = ::SendMessage(theObjectAttributes.m_hConfigList,CB_ADDSTRING,0,(LPARAM)((const char *)configFilesManager.GetName(configFileIndex)));
								::SendMessage(theObjectAttributes.m_hConfigList,CB_SETITEMDATA,comboIndex,configFileIndex);
								if (configFileIndex==configFilesManager.GetCurrentConfigIndex())
								{
									::SendMessage(theObjectAttributes.m_hConfigList,CB_SETCURSEL,comboIndex,0L);
								}
							}
							theObjectAttributes.ChangeConfigFile(configFilesManager.GetCurrentConfigIndex());
						}
						return TRUE;
					}
					case IDC_PURGE_UNUSED:
					{
						theObjectAttributes.PurgeUnusedKeys();
						return TRUE;
					}
					case IDC_FIND:
					{
						theObjectAttributes.HandleFindButton();
						return TRUE;
					}
				}
			}
			else if (HIWORD(wParam)==CBN_SELCHANGE)
			{
				if (LOWORD(wParam)==IDC_CFG_LIST)
				{
					int comboBoxSelection = ::SendMessage(theObjectAttributes.m_hConfigList,CB_GETCURSEL,0,0L);
					int configSelection = ::SendMessage(theObjectAttributes.m_hConfigList,CB_GETITEMDATA,comboBoxSelection,0L);
					theObjectAttributes.ChangeConfigFile(configSelection);
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
			if (NULL != theObjectAttributes.m_hToolTip)
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
						ICustomToolTip *pToolTip = ICustomToolTip::GetIToolTip(theObjectAttributes.m_hToolTip);
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
		case WM_SYSCOMMAND:
		{
			int nID = wParam;
			if ((nID & 0xFFF0) == IDM_ABOUTBOX)
			{
				TupString aboutText;
				aboutText.Format("ObjAttrib plugin for Maya\nVersion %s\nCopyright 2001-2005 Avalanche Software\nBuild %s %s",
					g_pBuildVersion,g_pBuildDate,g_pBuildTime);
				MessageBox(hWnd,aboutText,"About ObjAttrib",MB_OK);
				return TRUE;
			}
			return FALSE;
		}
	}
	return FALSE;
}



//------------------------------------------------------------------------
// --               ObjectAttributes::Member Functions                 --
//------------------------------------------------------------------------
ObjectAttributes::ObjectAttributes()
// default constructor
{
	hDlg		= NULL;
	m_pController	= NULL;
	m_hToolTip = NULL;
	m_hAttributesToolTip = NULL;

	bDialogUp		= false;
	bRefreshWindowEnable = true;

	m_pConfigFilesManager = new ConfigFilesManager;
	m_pConfigFilesManager->LoadFromRegistry();
}

//------------------------------------------------------------------------
ObjectAttributes::~ObjectAttributes()
// default destructor
{
	if (m_pController)
	{
		delete m_pController;
		m_pController = NULL;
	}
	m_pConfigFilesManager->SaveToRegistry();
	delete m_pConfigFilesManager;
}

// execute from a snipe command
// see command args.h for command details
bool ObjectAttributes::ExecuteCommand(const CommandArgs &commandArgs, TupArray<TupString>& resultStrings, UndoRedoState &undoRedoState)
{
	RefreshAttributes(); // this should be done most of the time anyway

	// query mode
	if (commandArgs.m_bQueryMode)
	{
		if (commandArgs.m_bQueryVersion)
		{
			resultStrings.Add(TupString(g_pBuildVersion));
			return true;
		}
		else if (commandArgs.m_bQueryAttributeConfigPath)
		{
			int configIndex = m_pConfigFilesManager->GetCurrentConfigIndex();
			if (configIndex != -1)
			{
				TupString fileString = m_pConfigFilesManager->GetFile(configIndex);
				resultStrings.Add(fileString);
				return true;
			}
			else
			{
				THROW_FATAL_ERROR("Config file not set");
			}
		}
		else if (commandArgs.m_bQueryAttributeConfigName)
		{
			int configIndex = m_pConfigFilesManager->GetCurrentConfigIndex();
			if (configIndex != -1)
			{
				TupString nameString = m_pConfigFilesManager->GetName(configIndex);
				resultStrings.Add(nameString);
				return true;
			}
			else
			{
				THROW_FATAL_ERROR("Config file not set");
			}
		}
		else
		{
			m_pController->HandleQueryCommand(commandArgs,resultStrings,undoRedoState);
		}
	}
	// set mode
	else if (commandArgs.m_bSetMode)
	{
		m_pController->HandleSetCommand(commandArgs,resultStrings,undoRedoState);
	}
	// purge mode
	else if (commandArgs.m_bPurgeMode)
	{
		m_pController->HandlePurgeCommand(commandArgs,resultStrings,undoRedoState);
	}
	// find mode
	else if (commandArgs.m_bFindMode)
	{
		m_pController->HandleFindCommand(commandArgs,resultStrings,undoRedoState);
	}
	// remap mode
	else if (commandArgs.m_bRemapMode)
	{
		m_pController->HandleRemapCommand(commandArgs,resultStrings,undoRedoState);
	}
	// apply defaults mode
	else if (commandArgs.m_bApplyDefaultsMode)
	{
		m_pController->HandleApplyDefaultValuesCommand(commandArgs,resultStrings,undoRedoState);
	}
	// remove unused
	else if (commandArgs.m_bPurgeUnusedMode)
	{
		m_pController->HandlePurgeUnusedCommand(commandArgs,resultStrings,undoRedoState);
	}
	// no mode (just put up ui)
	else
	{
		ShowAttributeDialog();
	}
	return true;
}

// called when we need to refresh the window outside of having to do a selection set change
void ObjectAttributes::RefreshWindow()
{
	if (bDialogUp == true && bRefreshWindowEnable == true)
	{
		// request for selection
		SendMessage(hDlg,WM_SELCHANGE,1,0);
	}
}

//------------------------------------------------------------------------
void ObjectAttributes::RefreshAttributes() 
// this method initializes the necessary data in order to support object
// attributes
{
	if (!m_pController)
	{
		m_pController = new ObjAttrController();
	}

	if (m_pConfigFilesManager->NeedToLoadData())
	{
		// save off the nonexclusive keys in the registry
		m_pController->LoadConfigFile(*m_pConfigFilesManager);
	}
}

//------------------------------------------------------------------------
void ObjectAttributes::ShowAttributeDialog() 
{
	// make sure refresh attributes is called first
	// set up the attributes panel dialog
	if (!hDlg)
	{
		this->InitDialog();

		HWND hSnipeWindow = AfxGetMainWnd()->GetSafeHwnd();

		HINSTANCE hInstance = AfxGetInstanceHandle();

		hDlg = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_OBJECTATTRIB), 
										hSnipeWindow, ObjectAttributesDlgProc,
										(LPARAM) this);
		bDialogUp = true;
	}
	else
	{
        SetActiveWindow(hDlg);
	}
}

//------------------------------------------------------------------------
void ObjectAttributes::InitDialog()
// This method will initialize the object attribute dialog.  Really, the only
// thing done here is the registering of the callback.
//
{
}

//------------------------------------------------------------------------
void ObjectAttributes::EndDialog()
// This method will be called when the dialog box is closed.  It unregisters
// the callback for the dialog.
//
{
}

//------------------------------------------------------------------------
bool ObjectAttributes::AdjustPanelHeights(bool bUseDefaultSize)
// This method can be called to adjust the panel heights of all panels in the
// rollup window.  Care should be taken to call this method only after the
// m_pUIController has been created in the ObjAttrController object, and after
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
								m_oldScrollPos = pRollupWindow->GetScrollPos();
								pRollupWindow->SetPageDlgHeight(nIndex, nDefaultResourceHeight);
								bResult = true;						
							}
							else
							{
								int nTotalY = m_pController->GetTotalY();
								if (nTotalY > -1)
								{
									pRollupWindow->SetPageDlgHeight(nIndex, nTotalY);
									if (m_oldScrollPos != -1)
									{
										pRollupWindow->SetScrollPos(m_oldScrollPos);
										m_oldScrollPos = -1;
									}
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
bool ObjectAttributes::SetUpUIDisplay(HWND hParentDlg, HINSTANCE hInstance,
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
bool ObjectAttributes::HandleWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam)

// This method will take the incoming message and pass it onto the
// ObjAttrController to see if any of its controls want to handle the message.
// The return value from ObjAttrController is then passed back as the return
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
		bRefreshWindowEnable = false;
		bResult = m_pController->HandleWinMessage(hParent, message, wParam, lParam);
		bRefreshWindowEnable = true;
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
bool ObjectAttributes::DrawUIDisplayLines(HDC hdc)
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
bool ObjectAttributes::LoadCurrAttribData()
// ObjectAttributes::LoadCurrAttribData
// This method will tell the m_pController to build the CurrAttribData based on
// the selection set passed in through the Interface pointer.
//
// Output: Passes back the return value from the m_pController.  In this case
//		   true if there are objects in the selection set, false otherwise.
//
{
	return m_pController->LoadCurrAttribData();
}

bool ObjectAttributes::OkToCopy(void) const
{
	if (m_pController != NULL)
	{
		return m_pController->OkToCopy();
	}
	return false;
}

bool ObjectAttributes::OkToPaste(void) const
{
	if (m_pController != NULL)
	{
		return m_pController->OkToPaste();
	}
	return false;
}

bool ObjectAttributes::CopySelection(void)
{
	if (m_pController != NULL)
	{
		return m_pController->CopySelection();
	}
	return false;
}

//-----------------------------------------------------------------------------
// ObjectAttributes::PasteAttribsToSelSet
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
bool ObjectAttributes::PasteAttribsToSelSet(bool bReplace)
{
	bool bResult = false;
	
	if (m_pController != NULL)
	{
		bResult =  m_pController->PasteAttribsToSelSet(bReplace);
	}
	return bResult;
}

//-----------------------------------------------------------------------------
// ObjectAttributes::PurgeUnusedKeys
// This method will tell the m_pController to purge the unused keys and values
//
// Input:
//
// Output:
//-----------------------------------------------------------------------------
void ObjectAttributes::PurgeUnusedKeys(void)
{
	if (m_pController != NULL)
	{
		int totalAffected = m_pController->HandlePurgeUnused();
		TupString stats;
		stats.Format("%d Changed",totalAffected);
		SetWindowText(m_hStatusStaticText,(const char *)stats);
	}
}

//-----------------------------------------------------------------------------
// ObjectAttributes::HandlFindButton
// This method will tell the m_pController to put up the find dialog
//
// Input:
//
// Output:
//-----------------------------------------------------------------------------
void ObjectAttributes::HandleFindButton(void)
{
	if (m_pController != NULL)
	{
		m_pController->HandleFindButton();
	}
}


//------------------------------------------------------------------------
void ObjectAttributes::DestroyCurrAttribData()

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
void ObjectAttributes::DestroyUIDisplay()
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
void ObjectAttributes::SaveNonexclusiveKeys()
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
// ObjectAttributes::InitKeyValPanel
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
void ObjectAttributes::InitKeyValPanel(ICustomRollupWindow* pCustomRollupWindow, HINSTANCE hInstance, LPARAM lParam)
{
	m_pController->InitKeyValPanel(pCustomRollupWindow, hInstance, lParam);
}


//-----------------------------------------------------------------------------
// ObjectAttributes::IdCancelMessage
// This method will notify the m_pController that the IDCANCEL message was
// received.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ObjectAttributes::IdCancelMessage()
{
	m_pController->IdCancelMessage();
}

void ObjectAttributes::UpdateCopyPasteButtons(void)
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
// ObjectAttributes::SetApplyBttnStates
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
void ObjectAttributes::SetApplyBttnStates(bool bApplySelEnabled, bool bApplySceneEnabled)
{
	m_pController->SetApplyBttnStates(bApplySelEnabled, bApplySceneEnabled);
}


//-----------------------------------------------------------------------------
// ObjectAttributes::LoadKeyValPanelTree
// This method will tell the m_pController to load the key/vals in the key/val
// panel.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ObjectAttributes::LoadKeyValPanelTree()
{
	m_pController->LoadKeyValPanelTree();
}

ConfigFilesManager &ObjectAttributes::GetConfigFilesManager(void)
{
	return *m_pConfigFilesManager;
}

void ObjectAttributes::ChangeConfigFile(int selectionIndex)
{
	m_pConfigFilesManager->SetCurrentConfigIndex(selectionIndex);
	NotifySelSetChanged();
}

void ObjectAttributes::RefreshConfig(void)
{
	if (m_pConfigFilesManager->NeedToLoadData())
	{
		m_pController->SaveNonexclusiveKeys();
		RefreshAttributes();
		ShowAttributeDialog();
	}
}

int ObjectAttributes::NumSelectedNodes(void) const
{
	int total = 0;
	if (m_pController)
	{
		CurrAttribController *pCurrAttribController = m_pController->GetCurrAttribController();
		if (pCurrAttribController)
		{
			total = pCurrAttribController->GetSize();
		}
	}
	return total;

}
