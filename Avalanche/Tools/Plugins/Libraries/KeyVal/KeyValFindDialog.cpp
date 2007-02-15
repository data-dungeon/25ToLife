//-----------------------------------------------------------------------------
//	File:		KeyValFindDialog.cpp
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes:

#include "KeyValPCH.h"
#include "KeyValPanelController.h"
#include "KeyValCommunicatorBase.h"
#include "KeyValRegistry.h"
#include "KeyValFindDialog.h"
#include "resource.h"

KeyValFindDialog* g_pTheKeyValFindDialog = NULL;

// registryKeys
#define KEYVAL_FIND_DIALOG_TOP_VAL_NAME				(TEXT("KeyValFindDialogRectTop"))
#define KEYVAL_FIND_DIALOG_BOTTOM_VAL_NAME			(TEXT("KeyValFindDialogRectBottom"))
#define KEYVAL_FIND_DIALOG_LEFT_VAL_NAME				(TEXT("KeyValFindDialogRectLeft"))
#define KEYVAL_FIND_DIALOG_RIGHT_VAL_NAME				(TEXT("KeyValFindDialogRectRight"))
#define KEYVAL_FIND_DIALOG_MATCH_CASE_NAME			(TEXT("KeyValFindDialogMatchCase"))
#define KEYVAL_FIND_DIALOG_MATCH_WHOLE_WORD_NAME	(TEXT("KeyValFindDialogMatchWholeWord"))
#define KEYVAL_FIND_DIALOG_SEARCH_DEFAULTS			(TEXT("KeyValFindDialogSearchDefaults"))
#define KEYVAL_FIND_DIALOG_SEARCH_UNUSED				(TEXT("KeyValFindDialogSearchUnused"))
#define KEYVAL_FIND_DIALOG_APPEND_SELECTION_NAME	(TEXT("KeyValFindDialogAppendSelection"))
#define KEYVAL_FIND_DIALOG_CURRENT_SELECTION			(TEXT("KeyValFindDialogCurrentSelection"))
#define KEYVAL_FIND_DIALOG_USE_KEY						(TEXT("KeyValFindDialogUseKey"))
#define KEYVAL_FIND_DIALOG_USE_VALUE					(TEXT("KeyValFindDialogUseValue"))

#define NUM_SLOTS 15

//-----------------------------------------------------------------------------
// FindDialogProc
// This function acts as the callback proc for the find dialog
// Messages sent to the panel dialog are processed here.
//
// Input:
//  HWND hWnd -- Handle to the dialog.
//  UINT msg -- Specifies the message sent.
//  WPARAM wParam -- Specifies additional message-specific information.
//  LPARAM lParam -- Specifies additional message-specific information.
//
// Output: TRUE if this callback processed the message, FALSE otherwise.
//-----------------------------------------------------------------------------
static BOOL CALLBACK FindDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
		case WM_INITDIALOG:
			{
				g_pTheKeyValFindDialog->m_hWnd = hWnd;
				KeyValRegistry myRegistry(g_pTheKeyValFindDialog->m_pKeyValPanelController->GetRegKeyName());

				DWORD dwTop;
				DWORD dwLeft;
				DWORD dwBottom;
				DWORD dwRight;
				if ((myRegistry.Get(KEYVAL_FIND_DIALOG_TOP_VAL_NAME, &dwTop) == TRUE) &&
					(myRegistry.Get(KEYVAL_FIND_DIALOG_LEFT_VAL_NAME, &dwLeft) == TRUE) &&
					(myRegistry.Get(KEYVAL_FIND_DIALOG_BOTTOM_VAL_NAME, &dwBottom) == TRUE) &&
					(myRegistry.Get(KEYVAL_FIND_DIALOG_RIGHT_VAL_NAME, &dwRight) == TRUE))
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
					MoveWindow(g_pTheKeyValFindDialog->m_hWnd,
						dialogRect.left, dialogRect.top,
						dialogRect.right - dialogRect.left, dialogRect.bottom - dialogRect.top,
						TRUE);
				}
				else
				{
					WinUtil::CenterWindow(g_pTheKeyValFindDialog->m_hWnd, g_pTheKeyValFindDialog->m_hParentWnd);
				}

				g_pTheKeyValFindDialog->m_hFindButton = GetDlgItem(hWnd, IDC_BTN_FIND);
				g_pTheKeyValFindDialog->m_hKeyCombo = GetDlgItem(hWnd, IDC_COMBO_KEY);
				g_pTheKeyValFindDialog->m_hValueCombo = GetDlgItem(hWnd, IDC_COMBO_VALUE);
				g_pTheKeyValFindDialog->m_hKeyCheck = GetDlgItem(hWnd, IDC_CHECK_KEY);
				g_pTheKeyValFindDialog->m_hValueCheck = GetDlgItem(hWnd, IDC_CHECK_VALUE);
				g_pTheKeyValFindDialog->m_hMatchCaseCheck = GetDlgItem(hWnd, IDC_CHECK_MATCH_CASE);
				g_pTheKeyValFindDialog->m_hMatchWholeWordCheck = GetDlgItem(hWnd, IDC_CHECK_MATCH_WHOLE_WORD);
				g_pTheKeyValFindDialog->m_hAppendSelectionCheck = GetDlgItem(hWnd, IDC_CHECK_APPEND_SELECTION);
				g_pTheKeyValFindDialog->m_hSearchDefaultsCheck = GetDlgItem(hWnd, IDC_CHECK_SEARCH_DEFAULTS);
				g_pTheKeyValFindDialog->m_hSearchUnusedCheck = GetDlgItem(hWnd, IDC_CHECK_SEARCH_UNUSED);
				g_pTheKeyValFindDialog->m_hSearchCurrentSelectionCheck = GetDlgItem(hWnd, IDC_RDO_SEARCH_CURRENT_SELECTION);
				g_pTheKeyValFindDialog->m_hSearchEntireSceneCheck = GetDlgItem(hWnd, IDC_RDO_SEARCH_ENTIRE_SCENE);

				g_pTheKeyValFindDialog->m_keyHistory.RemoveAll();
				g_pTheKeyValFindDialog->m_valueHistory.RemoveAll();

				// First check the installed flag
				char registryName[80];
				char registryData[_MAX_PATH];

				for (int slotNum=0;slotNum<NUM_SLOTS;slotNum++)
				{
					sprintf(registryName,"FindDialogKey%02d",slotNum);
					if (myRegistry.Get(registryName,registryData, sizeof(registryData)-1))
					{
						TupString registryDataString(registryData);
						g_pTheKeyValFindDialog->m_keyHistory.Add(registryDataString);
					}

					sprintf(registryName,"FindDialogValue%02d",slotNum);
					if (myRegistry.Get(registryName,registryData, sizeof(registryData)-1))
					{
						TupString registryDataString(registryData);
						g_pTheKeyValFindDialog->m_valueHistory.Add(registryDataString);
					}
				}

				DWORD regValue;
				if (myRegistry.Get(KEYVAL_FIND_DIALOG_MATCH_CASE_NAME, &regValue) == TRUE)
				{
					g_pTheKeyValFindDialog->m_bMatchCase = !(regValue == 0);
				}
				if (myRegistry.Get(KEYVAL_FIND_DIALOG_MATCH_WHOLE_WORD_NAME, &regValue) == TRUE)
				{
					g_pTheKeyValFindDialog->m_bMatchWholeWord = !(regValue == 0);
				}
				if (myRegistry.Get(KEYVAL_FIND_DIALOG_APPEND_SELECTION_NAME, &regValue) == TRUE)
				{
					g_pTheKeyValFindDialog->m_bAppendSelection = !(regValue == 0);
				}
				if (myRegistry.Get(KEYVAL_FIND_DIALOG_SEARCH_DEFAULTS, &regValue) == TRUE)
				{
					g_pTheKeyValFindDialog->m_bSearchDefaults = !(regValue == 0);
				}
				if (myRegistry.Get(KEYVAL_FIND_DIALOG_SEARCH_UNUSED, &regValue) == TRUE)
				{
					g_pTheKeyValFindDialog->m_bSearchUnused = !(regValue == 0);
				}
				if (myRegistry.Get(KEYVAL_FIND_DIALOG_CURRENT_SELECTION, &regValue) == TRUE)
				{
					g_pTheKeyValFindDialog->m_bCurrentSelection = !(regValue == 0);
				}
				if (myRegistry.Get(KEYVAL_FIND_DIALOG_USE_KEY, &regValue) == TRUE)
				{
					g_pTheKeyValFindDialog->m_bUseKey = !(regValue == 0);
				}
				if (myRegistry.Get(KEYVAL_FIND_DIALOG_USE_VALUE, &regValue) == TRUE)
				{
					g_pTheKeyValFindDialog->m_bUseValue = !(regValue == 0);
				}

				// set up tooltips
				g_pTheKeyValFindDialog->m_hToolTip = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
					CustomControls::GetWindowClassName(CustomControls::TOOLTIP_WINDOW_CLASS), NULL,
					WS_POPUP, 
					0,0,0,0,
					g_pTheKeyValFindDialog->m_hWnd, NULL, g_pTheKeyValFindDialog->m_hInstance,
					NULL);

				ICustomToolTip *pToolTip = ICustomToolTip::GetIToolTip(g_pTheKeyValFindDialog->m_hToolTip);

				int toolTipBehavior = ICustomToolTip::TOOLTIP_MULTIPLE_SHOW | ICustomToolTip::TOOLTIP_TRACKING_MOUSE | ICustomToolTip::TOOLTIP_CLOSE_LEAVEWND | ICustomToolTip::TOOLTIP_NOCLOSE_OVER;
				int toolTipStyle = ICustomToolTip::TOOLTIP_SHADOW;
				pToolTip->SetBehaviour(toolTipBehavior);
				pToolTip->SetStyles(toolTipStyle);
				pToolTip->SetDelayTime(TTDT_AUTOPOP,0);
				pToolTip->SetDelayTime(TTDT_INITIAL,500);
				pToolTip->SetDelayTime(TTDT_RESHOW,100);

				pToolTip->AddTool(g_pTheKeyValFindDialog->m_hFindButton,0);
				pToolTip->AddTool(g_pTheKeyValFindDialog->m_hKeyCombo,0);
				pToolTip->AddTool(g_pTheKeyValFindDialog->m_hValueCombo,0);
				pToolTip->AddTool(g_pTheKeyValFindDialog->m_hKeyCheck,0);
				pToolTip->AddTool(g_pTheKeyValFindDialog->m_hValueCheck,0);
				pToolTip->AddTool(g_pTheKeyValFindDialog->m_hMatchCaseCheck,0);
				pToolTip->AddTool(g_pTheKeyValFindDialog->m_hMatchWholeWordCheck,0);
				pToolTip->AddTool(g_pTheKeyValFindDialog->m_hAppendSelectionCheck,0);
				pToolTip->AddTool(g_pTheKeyValFindDialog->m_hSearchDefaultsCheck,0);
				pToolTip->AddTool(g_pTheKeyValFindDialog->m_hSearchUnusedCheck,0);
				pToolTip->AddTool(g_pTheKeyValFindDialog->m_hSearchCurrentSelectionCheck,0);
				pToolTip->AddTool(g_pTheKeyValFindDialog->m_hSearchEntireSceneCheck,0);

				pToolTip->SetNotify(TRUE);
				pToolTip->SetNotify(g_pTheKeyValFindDialog->m_hWnd);
				pToolTip->Activate(TRUE);

				pToolTip->SetParentWnd(g_pTheKeyValFindDialog->m_hWnd); // new parent since one gathered from constructor is wrong(since dialog is a child)

				ICustomToolTip::ReleaseIToolTip(pToolTip);

				g_pTheKeyValFindDialog->RefreshControls();

				return TRUE;
			}
		case WM_NOTIFY:
			{
				return g_pTheKeyValFindDialog->OnNotify(lParam);
			}
		case WM_COMMAND:
			{
				if (LOWORD(wParam) == IDCLOSE || 
					 LOWORD(wParam) == IDCANCEL)
				{
					g_pTheKeyValFindDialog->DialogDone();
					return TRUE;
				}
				else if (LOWORD(wParam) == IDOK)
				{
					g_pTheKeyValFindDialog->DoFind();
					return TRUE;
				}
				else if (HIWORD(wParam)==BN_CLICKED)
				{
					switch(LOWORD(wParam))
					{
					case IDC_BTN_FIND:
						{
							g_pTheKeyValFindDialog->DoFind();
							return TRUE;
						}
					case IDC_CHECK_MATCH_CASE:
						{
							int nCheckFlags = SendMessage(g_pTheKeyValFindDialog->m_hMatchCaseCheck, BM_GETSTATE, 0, 0);
							g_pTheKeyValFindDialog->m_bMatchCase = ((nCheckFlags & 0x0003) == BST_CHECKED);
							g_pTheKeyValFindDialog->RefreshControls();
							return TRUE;
						}
					case IDC_CHECK_MATCH_WHOLE_WORD:
						{
							int nCheckFlags = SendMessage(g_pTheKeyValFindDialog->m_hMatchWholeWordCheck, BM_GETSTATE, 0, 0);
							g_pTheKeyValFindDialog->m_bMatchWholeWord = ((nCheckFlags & 0x0003) == BST_CHECKED);
							g_pTheKeyValFindDialog->RefreshControls();
							return TRUE;
						}
					case IDC_CHECK_APPEND_SELECTION:
						{
							int nCheckFlags = SendMessage(g_pTheKeyValFindDialog->m_hAppendSelectionCheck, BM_GETSTATE, 0, 0);
							g_pTheKeyValFindDialog->m_bAppendSelection = ((nCheckFlags & 0x0003) == BST_CHECKED);
							g_pTheKeyValFindDialog->RefreshControls();
							return TRUE;
						}
					case IDC_CHECK_SEARCH_DEFAULTS:
						{
							int nCheckFlags = SendMessage(g_pTheKeyValFindDialog->m_hSearchDefaultsCheck, BM_GETSTATE, 0, 0);
							g_pTheKeyValFindDialog->m_bSearchDefaults = ((nCheckFlags & 0x0003) == BST_CHECKED);
							g_pTheKeyValFindDialog->RefreshControls();
							return TRUE;
						}
					case IDC_CHECK_SEARCH_UNUSED:
						{
							int nCheckFlags = SendMessage(g_pTheKeyValFindDialog->m_hSearchUnusedCheck, BM_GETSTATE, 0, 0);
							g_pTheKeyValFindDialog->m_bSearchUnused = ((nCheckFlags & 0x0003) == BST_CHECKED);
							g_pTheKeyValFindDialog->RefreshControls();
							return TRUE;
						}
					case IDC_CHECK_KEY:
						{
							int nCheckFlags = SendMessage(g_pTheKeyValFindDialog->m_hKeyCheck, BM_GETSTATE, 0, 0);
							g_pTheKeyValFindDialog->m_bUseKey = ((nCheckFlags & 0x0003) == BST_CHECKED);
							g_pTheKeyValFindDialog->RefreshControls();
							return TRUE;
						}
					case IDC_CHECK_VALUE:
						{
							int nCheckFlags = SendMessage(g_pTheKeyValFindDialog->m_hValueCheck, BM_GETSTATE, 0, 0);
							g_pTheKeyValFindDialog->m_bUseValue = ((nCheckFlags & 0x0003) == BST_CHECKED);
							g_pTheKeyValFindDialog->RefreshControls();
							return TRUE;
						}
					case IDC_RDO_SEARCH_CURRENT_SELECTION:
						{
							g_pTheKeyValFindDialog->m_bCurrentSelection = true;
							g_pTheKeyValFindDialog->RefreshControls();
							return TRUE;
						}
					case IDC_RDO_SEARCH_ENTIRE_SCENE:
						{
							g_pTheKeyValFindDialog->m_bCurrentSelection = false;
							g_pTheKeyValFindDialog->RefreshControls();
							return TRUE;
						}
					}
				}
				else if (HIWORD(wParam)==CBN_EDITCHANGE)
				{
					switch(LOWORD(wParam))
					{
						case IDC_COMBO_KEY:
							{
								char tempString[1024];
								GetWindowText(g_pTheKeyValFindDialog->m_hKeyCombo, tempString, 1024);
								g_pTheKeyValFindDialog->m_findKey = tempString;
								return TRUE;
							}
						case IDC_COMBO_VALUE:
							{
								char tempString[1024];
								GetWindowText(g_pTheKeyValFindDialog->m_hValueCombo, tempString, 1024);
								g_pTheKeyValFindDialog->m_findValue = tempString;
								return TRUE;
							}
					}
				}
				else if (HIWORD(wParam)==CBN_SELCHANGE)
				{
					switch(LOWORD(wParam))
					{
						case IDC_COMBO_KEY:
							{
								int selection = ::SendMessage(g_pTheKeyValFindDialog->m_hKeyCombo,CB_GETCURSEL,0,0);
								if (selection != -1)
								{
									g_pTheKeyValFindDialog->m_findKey = g_pTheKeyValFindDialog->m_keyHistory[selection];
								}
								return TRUE;
							}
						case IDC_COMBO_VALUE:
							{
								int selection = ::SendMessage(g_pTheKeyValFindDialog->m_hValueCombo,CB_GETCURSEL,0,0);
								if (selection != -1)
								{
									g_pTheKeyValFindDialog->m_findValue = g_pTheKeyValFindDialog->m_valueHistory[selection];
								}
								return TRUE;
							}
						}
				}
				break;
			}
		case WM_DESTROY:
			{
				g_pTheKeyValFindDialog->DialogDone();
				return TRUE;
			}
		case WM_SETCURSOR:
			{
				return g_pTheKeyValFindDialog->OnSetCursor((UINT)LOWORD(lParam),(UINT)HIWORD(lParam));
			}
		case WM_GETMINMAXINFO:
			{
				MINMAXINFO *lpMMI = (MINMAXINFO *)lParam;
				lpMMI->ptMinTrackSize.x = 372;
				lpMMI->ptMinTrackSize.y = 211;
				return TRUE;
			}
	}
	return FALSE;
}

KeyValFindDialog::KeyValFindDialog(HINSTANCE hInstance, HWND hParentWnd,KeyValPanelController *pKeyValPanelController) :
 m_hInstance(hInstance),m_hParentWnd(hParentWnd),m_pKeyValPanelController(pKeyValPanelController),m_hWnd(0),
 m_hFindButton(0),m_hKeyCombo(0),m_hValueCombo(0),m_hKeyCheck(0),m_hValueCheck(0),
 m_hMatchCaseCheck(0),m_hMatchWholeWordCheck(0),m_hAppendSelectionCheck(0),
 m_hToolTip(0),
 m_hSearchCurrentSelectionCheck(0),m_hSearchEntireSceneCheck(0),
 m_bCurrentSelection(true),m_bAppendSelection(false),m_bUseKey(true),m_bUseValue(true),m_bMatchCase(false),m_bMatchWholeWord(false),
 m_bSearchDefaults(true),m_bSearchUnused(false)
{
	assert(!g_pTheKeyValFindDialog);
	g_pTheKeyValFindDialog = this;
}

KeyValFindDialog::~KeyValFindDialog()
{
	DialogDone();
	g_pTheKeyValFindDialog = NULL;
}

void KeyValFindDialog::Show(void)
{
	if (!m_hWnd)
	{
		m_hWnd = CreateDialogParam(m_hInstance, MAKEINTRESOURCE(IDD_FIND), 
			m_hParentWnd, FindDialogProc, (LPARAM) this);
	}
	else
	{
		SetActiveWindow(m_hWnd);
	}
	RefreshControls();
}

void KeyValFindDialog::DialogDone(void)
{
	if (!m_hWnd) // all ready done?
		return;

	char registryName[80];

	// save off the position of the dialog
	RECT dialogRect;
	GetWindowRect(m_hWnd, &dialogRect);
	KeyValRegistry myRegistry(m_pKeyValPanelController->GetRegKeyName());
	myRegistry.Set(KEYVAL_FIND_DIALOG_TOP_VAL_NAME, (DWORD) dialogRect.top);
	myRegistry.Set(KEYVAL_FIND_DIALOG_BOTTOM_VAL_NAME, (DWORD) dialogRect.bottom);
	myRegistry.Set(KEYVAL_FIND_DIALOG_LEFT_VAL_NAME, (DWORD) dialogRect.left);
	myRegistry.Set(KEYVAL_FIND_DIALOG_RIGHT_VAL_NAME, (DWORD) dialogRect.right);

	int slotNum;
	for (slotNum=0;slotNum<NUM_SLOTS;slotNum++)
	{
		sprintf(registryName,"FindDialogKey%02d",slotNum);
		myRegistry.Delete(registryName);

		sprintf(registryName,"FindDialogValue%02d",slotNum);
		myRegistry.Delete(registryName);
	}

	int size = m_keyHistory.GetSize();
	if (size>NUM_SLOTS)
		size = NUM_SLOTS;

	for (slotNum=0;slotNum<size;slotNum++)
	{
		sprintf(registryName,"FindDialogKey%02d",slotNum);
		myRegistry.Set(registryName,LPCTSTR(m_keyHistory[slotNum]));
	}

	size = m_valueHistory.GetSize();
	if (size>NUM_SLOTS)
		size = NUM_SLOTS;

	for (slotNum=0;slotNum<size;slotNum++)
	{
		sprintf(registryName,"FindDialogValue%02d",slotNum);
		myRegistry.Set(registryName,LPCTSTR(m_valueHistory[slotNum]));
	}

	myRegistry.Set(KEYVAL_FIND_DIALOG_MATCH_CASE_NAME,(DWORD)m_bMatchCase);
	myRegistry.Set(KEYVAL_FIND_DIALOG_MATCH_WHOLE_WORD_NAME,(DWORD)m_bMatchWholeWord);
	myRegistry.Set(KEYVAL_FIND_DIALOG_SEARCH_DEFAULTS,(DWORD)m_bSearchDefaults);
	myRegistry.Set(KEYVAL_FIND_DIALOG_SEARCH_UNUSED,(DWORD)m_bSearchUnused);
	myRegistry.Set(KEYVAL_FIND_DIALOG_APPEND_SELECTION_NAME,(DWORD)m_bAppendSelection);
	myRegistry.Set(KEYVAL_FIND_DIALOG_CURRENT_SELECTION,(DWORD)m_bCurrentSelection);
	myRegistry.Set(KEYVAL_FIND_DIALOG_USE_KEY,(DWORD)m_bUseKey);
	myRegistry.Set(KEYVAL_FIND_DIALOG_USE_VALUE,(DWORD)m_bUseValue);

	EndDialog(m_hWnd, FALSE);
	m_hWnd = NULL;
}

void KeyValFindDialog::RefreshControls(void)
{
	::SendMessage(m_hKeyCombo,CB_RESETCONTENT,0,0L);
	::SendMessage(m_hValueCombo,CB_RESETCONTENT,0,0L);
	
	bool bFoundString = false;
	int slotNum;
	for (slotNum=0;slotNum<m_keyHistory.GetSize();slotNum++)
	{
		::SendMessage(m_hKeyCombo,CB_ADDSTRING,0,(LPARAM)((const char *)m_keyHistory[slotNum]));
		if (m_keyHistory[slotNum] == m_findKey)
		{
			::SendMessage(m_hKeyCombo,CB_SETCURSEL,0,(LPARAM)(slotNum));
			bFoundString = true;
		}
	}
	if (!bFoundString)
	{
		::SendMessage(m_hKeyCombo,CB_SETCURSEL,0,(LPARAM)(-1));
	}

	bFoundString = false;
	for (slotNum=0;slotNum<m_valueHistory.GetSize();slotNum++)
	{
		::SendMessage(m_hValueCombo,CB_ADDSTRING,0,(LPARAM)((const char *)m_valueHistory[slotNum]));
		if (m_valueHistory[slotNum] == m_findValue)
		{
			::SendMessage(m_hValueCombo,CB_SETCURSEL,0,(LPARAM)(slotNum));
			bFoundString = true;
		}
	}
	if (!bFoundString)
	{
		::SendMessage(m_hKeyCombo,CB_SETCURSEL,0,(LPARAM)(-1));
	}

	SetWindowText(m_hKeyCombo, m_findKey);
	SetWindowText(m_hValueCombo, m_findValue);

	SendMessage(m_hKeyCheck, BM_SETCHECK, m_bUseKey ? BST_CHECKED : BST_UNCHECKED , 0);
	SendMessage(m_hValueCheck, BM_SETCHECK, m_bUseValue ? BST_CHECKED : BST_UNCHECKED , 0);

	SendMessage(m_hMatchCaseCheck, BM_SETCHECK, m_bMatchCase ? BST_CHECKED : BST_UNCHECKED , 0);
	SendMessage(m_hMatchWholeWordCheck, BM_SETCHECK, m_bMatchWholeWord ? BST_CHECKED : BST_UNCHECKED , 0);

	SendMessage(m_hSearchDefaultsCheck, BM_SETCHECK, m_bSearchDefaults ? BST_CHECKED : BST_UNCHECKED , 0);
	SendMessage(m_hSearchUnusedCheck, BM_SETCHECK, m_bSearchUnused ? BST_CHECKED : BST_UNCHECKED , 0);

	CheckRadioButton(m_hWnd,IDC_RDO_SEARCH_CURRENT_SELECTION,IDC_RDO_SEARCH_ENTIRE_SCENE,
		m_bCurrentSelection ? IDC_RDO_SEARCH_CURRENT_SELECTION : IDC_RDO_SEARCH_ENTIRE_SCENE);

	if (m_bCurrentSelection)
	{
		EnableWindow(m_hAppendSelectionCheck,FALSE);
		SendMessage(m_hAppendSelectionCheck, BM_SETCHECK, BST_UNCHECKED, 0);
	}
	else
	{
		EnableWindow(m_hAppendSelectionCheck,TRUE);
		SendMessage(m_hAppendSelectionCheck, BM_SETCHECK, m_bAppendSelection ? BST_CHECKED : BST_UNCHECKED , 0);
	}

	EnableWindow(m_hKeyCombo,m_bUseKey ? TRUE : FALSE);
	EnableWindow(m_hValueCombo,m_bUseValue ? TRUE : FALSE);
	EnableWindow(m_hFindButton,(m_bUseKey || m_bUseValue) ? TRUE : FALSE);

}

BOOL KeyValFindDialog::OnNotify(LPARAM lParam)
{
	LPNMHDR hdr = (LPNMHDR) lParam;
	// This is where the tooltip text is provided for the toolbar buttons and the two
	// action buttons:
	switch (hdr->code)
	{
		case ICustomToolTip::UDM_TOOLTIP_DISPLAY:
			{
				ICustomToolTip::ToolTipDisplay *pNotify = (ICustomToolTip::ToolTipDisplay*) hdr;

				CHAR charBuff[200];
				if (pNotify->ti->m_hWnd == m_hFindButton)
				{
					LoadString(m_hInstance, IDS_TT_FIND_BUTTON, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
				else if (pNotify->ti->m_hWnd == m_hKeyCombo)
				{
					LoadString(m_hInstance, IDS_TT_KEY_COMBO, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
				else if (pNotify->ti->m_hWnd == m_hValueCombo)
				{
					LoadString(m_hInstance, IDS_TT_VALUE_COMBO, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
				else if (pNotify->ti->m_hWnd == m_hKeyCheck)
				{
					LoadString(m_hInstance, IDS_TT_KEY_CHECK, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
				else if (pNotify->ti->m_hWnd == m_hValueCheck)
				{
					LoadString(m_hInstance, IDS_TT_VALUE_CHECK, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
				else if (pNotify->ti->m_hWnd == m_hMatchCaseCheck)
				{
					LoadString(m_hInstance, IDS_TT_MATCH_CASE, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
				else if (pNotify->ti->m_hWnd == m_hMatchWholeWordCheck)
				{
					LoadString(m_hInstance, IDS_TT_MATCH_WHOLE_WORD, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
				else if (pNotify->ti->m_hWnd == m_hSearchDefaultsCheck)
				{
					LoadString(m_hInstance, IDS_TT_SEARCH_DEFAULTS, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
				else if (pNotify->ti->m_hWnd == m_hSearchUnusedCheck)
				{
					LoadString(m_hInstance, IDS_TT_SEARCH_UNUSED, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
				else if (pNotify->ti->m_hWnd == m_hAppendSelectionCheck)
				{
					LoadString(m_hInstance, IDS_TT_APPEND_CHECK, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
				else if (pNotify->ti->m_hWnd == m_hSearchCurrentSelectionCheck)
				{
					LoadString(m_hInstance, IDS_TT_CURRENT_SELECTION_CHECK, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
				else if (pNotify->ti->m_hWnd == m_hSearchEntireSceneCheck)
				{
					LoadString(m_hInstance, IDS_TT_ENTIRE_SCENE_CHECK, charBuff, sizeof(charBuff));
					pNotify->ti->m_sTooltip = charBuff;
				}
			}
			return TRUE;
	}
	return TRUE;
}

BOOL KeyValFindDialog::OnSetCursor(UINT nHitTest,UINT message)
{
	if (NULL != m_hToolTip)
	{
		MSG msg;
		memset(&msg,0,sizeof(MSG));
		POINT pt;
		::GetCursorPos(&pt);   
		msg.hwnd = m_hWnd;
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
				ICustomToolTip *pToolTip = ICustomToolTip::GetIToolTip(m_hToolTip);
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

void KeyValFindDialog::DoFind(void)
{
	if (m_findKey.GetLength()>0 && m_bUseKey)
	{
		m_keyHistory.InsertAt(0,m_findKey);
		for (int historyIndex = m_keyHistory.GetSize()-1; historyIndex>0; historyIndex--)
		{
			if (m_keyHistory[historyIndex] == m_findKey)
			{
				m_keyHistory.RemoveAt(historyIndex);
			}
		}
	}
	if (m_keyHistory.GetSize() > NUM_SLOTS)
	{
		m_keyHistory.SetSize(NUM_SLOTS);
	}

	if (m_findValue.GetLength()>0 && m_bUseValue)
	{
		m_valueHistory.InsertAt(0,m_findValue);
		for (int historyIndex = m_valueHistory.GetSize()-1; historyIndex>0; historyIndex--)
		{
			if (m_valueHistory[historyIndex] == m_findValue)
			{
				m_valueHistory.RemoveAt(historyIndex);
			}
		}
	}
	if (m_valueHistory.GetSize() > NUM_SLOTS)
	{
		m_valueHistory.SetSize(NUM_SLOTS);
	}
	RefreshControls();

	KeyValueFind findParams;
	findParams.m_bUseKey = m_bUseKey;
	findParams.m_keyName = m_findKey;
	findParams.m_bUseValue = m_bUseValue;
	findParams.m_valueName = m_findValue;
	findParams.m_bMatchCase = m_bMatchCase;
	findParams.m_bMatchWholeWord = m_bMatchWholeWord;
	findParams.m_bSearchDefaults = m_bSearchDefaults;
	findParams.m_bSearchUnused = m_bSearchUnused;

	int totalFound = m_pKeyValPanelController->HandleFind(findParams,m_bCurrentSelection,m_bAppendSelection);
}

void KeyValFindDialog::SetFindKey(const char *pKeyName)
{
	m_findKey = pKeyName;
}

void KeyValFindDialog::SetFindValue(const char *pValueName)
{
	m_findValue = pValueName;
}


