//-----------------------------------------------------------------------------
//	File:		ImageAttributes.cpp
//	Created:	June 13, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Dave Astle, based on code by Brad Worthen
//
//	Desc:	The ImageAttributes class acts as the main class for this utility
//			plugin.  It allows the user to set custom attributes at the image
//			level in 3ds Max.  This file contains the ImageAttributes class
//			and some helper classes and functions.
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "ImageAttributesPCH.h"
#include "ImgAttrController.h"
#include "ImageAttributes.h"
#include "../UIDisplay/FuncPtrController.h"
#include "../UIDisplay/Registry.h"
#include "notify.h"
#include "irollupsettings.h"
#include "resource.h"

//-----------------------------------------------------------------------------
// Defines, etc.:


#define IMAGEATTRIBUTES_CLASS_ID Class_ID(0x17b511de, 0x685458ce)

#define IMAGE_ATTRIB_KEY (TEXT("Software\\Avalanche Software\\ImageAttributes"))

// Control position defines:
#define START_X			10
#define START_Y			10
#define LABEL_WD		110
#define LABEL_HT		15
#define SPACE_BET_CTRLS	8

// Registry defines:
#define DIALOG_TOP_VAL_NAME		"DialogRectTop"
#define DIALOG_BOTTOM_VAL_NAME	"DialogRectBottom"
#define DIALOG_LEFT_VAL_NAME	"DialogRectLeft"
#define DIALOG_RIGHT_VAL_NAME	"DialogRectRight"
#define KEYVAL_PANEL_OPEN_NAME	"KeyValPanelOpen"
#define ATTRIB_PANEL_OPEN_NAME	"AttribPanelOpen"
#define SHOW_ALL_IMAGES_NAME	"ShowAll"
#define SHOW_MATERIALS_NAME		"ShowMaterials"

// Min/max window size defines:
#define INSIDE_FRAME_SPACE_ROLLUP 2
#define SPACE_ABOVE_ROLLUP        31
#define SLIDER_SIZE       2
#define MIN_LIST_WIDTH    180
#define ROLLUP_WIDTH      (350 + INSIDE_FRAME_SPACE_ROLLUP * 2)
#define MIN_DLG_WIDTH     (ROLLUP_WIDTH + MIN_LIST_WIDTH + SLIDER_SIZE)
#define MIN_DLG_HEIGHT    290


// Custom toolbar button id defines:
#define ID_BUTTON_ONETOMANY         7501
#define ID_BUTTON_ONETOMANY_REPLACE 7502
#define ID_BUTTON_IMAGES		7503
#define ID_BUTTON_MATERIALS		7504


//-----------------------------------------------------------------------------
// ImageAttributesClassDesc Class Declaration:

class ImageAttributesClassDesc : public ClassDesc2
{
public:
	int				IsPublic() { return TRUE; }
	void*				Create(BOOL loading = FALSE) { return &theImageAttributes; }
	const TCHAR*	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return UTILITY_CLASS_ID; }
	Class_ID			ClassID() { return IMAGEATTRIBUTES_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	// Returns fixed parsable name (scripter-visible name):
	const TCHAR*	InternalName() { return _T("ImageAttributes"); }
	// Returns owning module handle:
	HINSTANCE		HInstance() { return hInstance; }
};



//-----------------------------------------------------------------------------
// Global objects and functions:

ImageAttributes theImageAttributes;
ImgAttrRollupCallBack theImgAttrRollupCallBack;
static ImageAttributesClassDesc ImageAttributesDesc;
ClassDesc2* GetImageAttributesDesc()
{
	return &ImageAttributesDesc;
}


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
			if (theImageAttributes.HandleWinMessage(hwnd, uMsg, wParam, lParam) == true)
			{
				// We don't need to rebuild the UIDisplay for this message!
				// But we do need to load the key/val panel:
				theImageAttributes.LoadKeyValPanelTree();
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



//-----------------------------------------------------------------------------
// AttributesRollupPageProc
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
//-----------------------------------------------------------------------------
BOOL CALLBACK AttributesRollupPageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	theImageAttributes.m_hAttribsRollup = hDlg;
	switch (message)
	{
	case WM_INITDIALOG:
		{
			// Load in the current attrib data (if any) and see if we need to show the controls:
			Interface* ip = GetCOREInterface();
			if (theImageAttributes.GetImageList(ip, theImageAttributes.m_showAllImages))
			{
				theImageAttributes.DisplayImageList();
				theImageAttributes.UpdateAttribDisplay(false);
			}
			else
			{
				theImageAttributes.DisplayImageList();
			}

			return TRUE;
		}
	case WM_PAINT:
		{
			if (theImageAttributes.m_bShowControls == true)
			{
				// Load in the current attrib data (if any) and see if we need to show the controls:
				if (theImageAttributes.m_bObjsInSel == true)
				{
					// Hide the "No objects" label:
					ShowWindow(GetDlgItem(hDlg, IDC_STATIC_SELOBJECTS), SW_HIDE);

					// Set up the UI Display:
					theImageAttributes.SetUpUIDisplay(hDlg, hInstance, theImageAttributes.m_hAppFont,
																 START_X, START_Y, LABEL_WD, LABEL_HT,
																 SPACE_BET_CTRLS, colorSwatchSubclassWndProc);
				}
				else
				{
					// Adjust the panel heights:
					theImageAttributes.AdjustPanelHeights(true);

					// Show the "No objects" label:
					ShowWindow(GetDlgItem(hDlg, IDC_STATIC_SELOBJECTS), SW_SHOW);
				}

				theImageAttributes.m_bShowControls = false;
				theImageAttributes.m_bObjsInSel = false;
			}

			return FALSE;
		}
	case WM_COMMAND:
		{
			switch(HIWORD(wParam))
			{
				// Check for a selection change in a combobox:
			case CBN_CLOSEUP:
				{
					// Fall through:
				}
				// Check for a checkbox button pressed:
			case BN_CLICKED:
				{
					// Pass the message on to see if it gets handled:
					if (theImageAttributes.HandleWinMessage(hDlg, message, wParam, lParam) == true)
					{
						// Set up the UI Display:
						theImageAttributes.SetUpUIDisplay(hDlg, hInstance, theImageAttributes.m_hAppFont,
																	 START_X, START_Y, LABEL_WD, LABEL_HT,
																	 SPACE_BET_CTRLS, colorSwatchSubclassWndProc);

						// Load the key/val panel:
						theImageAttributes.LoadKeyValPanelTree();
						
						return FALSE;
					}
					else
						return TRUE;
				}
			}
			return TRUE;			
		}
		// Check to see if a custom edit box is changing:
		case WM_CUSTEDIT_ENTER:
		{
			// Fall through:
		}
		// Check to see if a spinner button is let up:
		case CC_SPINNER_BUTTONUP:
		{
			// Fall through:
		}
		// Check to see if a slider thumb is let up:
		case CC_SLIDER_BUTTONUP:
		{
			// Fall through:
		}
		// Check to see if a free color swatch is changing color:
		case CC_COLOR_CHANGE:
		{
			// Pass the message on to let any of the controls handle the message:
			if (theImageAttributes.HandleWinMessage(hDlg, message, wParam, lParam) == true)
			{
				// We don't need to rebuild the UIDisplay for this message!
				// But we do need to load the key/val panel:
				theImageAttributes.LoadKeyValPanelTree();
			}

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
static void NotifySelSetChanged(void* param, NotifyInfo* info)
{
	// only care about the selection set changing when not showing all images
	if (theImageAttributes.m_showAllImages == true)
		return;

	if (theImageAttributes.bDialogUp == true)
	{
		theImageAttributes.m_bShowControls = true;
		Interface* ip = GetCOREInterface();
		if (theImageAttributes.GetImageList(ip, theImageAttributes.m_showAllImages))
		{
			theImageAttributes.DisplayImageList();
			if (theImageAttributes.LoadCurrAttribData(ip))
			{
				// Set the "objects in selection" to true:
				theImageAttributes.m_bObjsInSel = true;

				theImageAttributes.m_pOneToManyButton->Disable();
				theImageAttributes.m_pOneToManyButtonReplace->Disable();

				// Enable the apply buttons in the key/val panel:
				theImageAttributes.SetApplyBttnStates(true, true);
			}
			else
			{
				// Set the "objects in selection" to false:
				theImageAttributes.m_bObjsInSel = false;

				// Disable the "Copy Attributes" buttons:
				theImageAttributes.m_pOneToManyButton->Disable();
				theImageAttributes.m_pOneToManyButtonReplace->Disable();

				// Disable the apply buttons in the key/val panel:
				theImageAttributes.SetApplyBttnStates(false, false);

				// Destroy any CurrAttribData and UIDisplay:
				theImageAttributes.DestroyCurrAttribData();
				theImageAttributes.DestroyUIDisplay();
			}
		}
		else
		{
			// make sure the image list is displayed as empty
			theImageAttributes.DisplayImageList();
		}

		// Fire off the WM_PAINT message to the attribute panel:
		InvalidateRgn(theImageAttributes.hDlg, NULL, FALSE);
		UpdateWindow(theImageAttributes.hDlg);

		// Load the key/val panel:
		theImageAttributes.LoadKeyValPanelTree();
	}
}


//-----------------------------------------------------------------------------
// ImageAttributesDlgProc
// This function acts as the callback function for the image attributes
// dialog.  Messages sent to the dialog are processed here.
//
// Input:
//  HWND hWnd -- Handle to the dialog.
//  UINT msg -- Specifies the message sent.
//  WPARAM wParam -- Specifies additional message-specific information.
//  LPARAM lParam -- Specifies additional message-specific information.
//
// Output: TRUE if this callback processed the message, FALSE otherwise.
//-----------------------------------------------------------------------------
static BOOL CALLBACK ImageAttributesDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		{
			theImageAttributes.hDlg = hWnd;
			
			// Move dialog to the last saved position and size:
			Registry myRegistry(IMAGE_ATTRIB_KEY);
			DWORD dwTop;
			DWORD dwLeft;
			DWORD dwBottom;
			DWORD dwRight;
			DWORD showAllImages;
			DWORD showMaterials;

			if (myRegistry.Get(SHOW_ALL_IMAGES_NAME, &showAllImages) == TRUE)
			{
				if (0 == showAllImages)
				{
					theImageAttributes.m_showAllImages = false;
					CheckRadioButton(hWnd, IDC_RADIO_SHOWSEL, IDC_RADIO_SHOWALL, IDC_RADIO_SHOWSEL);
				}
				else
				{
					theImageAttributes.m_showAllImages = true;
					CheckRadioButton(hWnd, IDC_RADIO_SHOWSEL, IDC_RADIO_SHOWALL, IDC_RADIO_SHOWALL);
				}
			}
			else
			{
				theImageAttributes.m_showAllImages = true;
				CheckRadioButton(hWnd, IDC_RADIO_SHOWSEL, IDC_RADIO_SHOWALL, IDC_RADIO_SHOWALL);
			}

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
				MoveWindow(theImageAttributes.hDlg,
						   dialogRect.left, dialogRect.top,
						   dialogRect.right - dialogRect.left, dialogRect.bottom - dialogRect.top,
						   TRUE);
			}
			else
			{
				CenterWindow(theImageAttributes.hDlg, GetParent(theImageAttributes.hDlg));
			}

			// Set up the main toolbar:
			theImageAttributes.m_pMainToolbar = GetICustToolbar(GetDlgItem(hWnd, IDC_TOOLBAR_MAIN));
			theImageAttributes.m_pMainToolbar->SetBottomBorder(FALSE);
			theImageAttributes.m_pMainToolbar->SetTopBorder(FALSE);
			theImageAttributes.m_hImageMainToolbar = ImageList_Create(16, 15, TRUE, 6, 0);
			HBITMAP hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TOOLBAR_MAIN));
			HBITMAP	hMask = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TOOLBAR_MAIN_MASK));
			ImageList_Add(theImageAttributes.m_hImageMainToolbar, hBitmap, hMask);
			DeleteObject(hBitmap);
			DeleteObject(hMask);
			theImageAttributes.m_pMainToolbar->SetImage(theImageAttributes.m_hImageMainToolbar);
			// Now add the tools:
			// Add a seperator:
			theImageAttributes.m_pMainToolbar->AddTool(ToolSeparatorItem(3));
			// Add the one-to-many button:
			theImageAttributes.m_pMainToolbar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,
																	   0, 1, 2, 1, 16, 15, 
																	   22, 20, ID_BUTTON_ONETOMANY));
			// Add the one-to-many replace button:
			theImageAttributes.m_pMainToolbar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,
																	   3, 4, 5, 4, 16, 15, 
																	   22, 20, ID_BUTTON_ONETOMANY_REPLACE));
			// Add another seperator:
			theImageAttributes.m_pMainToolbar->AddTool(ToolSeparatorItem(3));
			// Allow the buttons to be highlighted in a specific color.
			// GREEN_WASH is the standard color for MAX buttons:
			theImageAttributes.m_pOneToManyButton = theImageAttributes.m_pMainToolbar->GetICustButton(ID_BUTTON_ONETOMANY);
			theImageAttributes.m_pOneToManyButton->SetCheckHighlight(TRUE);
			theImageAttributes.m_pOneToManyButton->SetHighlightColor(GREEN_WASH);
			theImageAttributes.m_pOneToManyButtonReplace = theImageAttributes.m_pMainToolbar->GetICustButton(ID_BUTTON_ONETOMANY_REPLACE);
			theImageAttributes.m_pOneToManyButtonReplace->SetCheckHighlight(TRUE);
			theImageAttributes.m_pOneToManyButtonReplace->SetHighlightColor(GREEN_WASH);

			// Set up the image/material toolbar:
			theImageAttributes.m_pSelToolbar = GetICustToolbar(GetDlgItem(hWnd, IDC_TOOLBAR_SEL));
			theImageAttributes.m_pSelToolbar->SetBottomBorder(FALSE);
			theImageAttributes.m_pSelToolbar->SetTopBorder(FALSE);
			theImageAttributes.m_hImageSelToolbar = ImageList_Create(16, 15, TRUE, 2, 0);
			hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_SELECTION));
			hMask = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_SELECTION_MASK));
			ImageList_Add(theImageAttributes.m_hImageSelToolbar, hBitmap, hMask);
			DeleteObject(hBitmap);
			DeleteObject(hMask);
			theImageAttributes.m_pSelToolbar->SetImage(theImageAttributes.m_hImageSelToolbar);
			theImageAttributes.m_pSelToolbar->AddTool(ToolSeparatorItem(3));
			// Now add the buttons:
			theImageAttributes.m_pSelToolbar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,
																	   0, 0, 0, 0, 16, 15, 
																	   22, 20, ID_BUTTON_IMAGES));
			theImageAttributes.m_pSelToolbar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,
																	   1, 1, 1, 1, 16, 15, 
																	   22, 20, ID_BUTTON_MATERIALS));
			theImageAttributes.m_pSelToolbar->AddTool(ToolSeparatorItem(3));

			theImageAttributes.m_pImagesButton = theImageAttributes.m_pSelToolbar->GetICustButton(ID_BUTTON_IMAGES);
			theImageAttributes.m_pImagesButton->SetCheckHighlight(FALSE);
			theImageAttributes.m_pMaterialsButton = theImageAttributes.m_pSelToolbar->GetICustButton(ID_BUTTON_MATERIALS);
			theImageAttributes.m_pMaterialsButton->SetCheckHighlight(FALSE);

			if (myRegistry.Get(SHOW_MATERIALS_NAME, &showMaterials) == TRUE)
			{
				if (0 == showMaterials)
				{
					theImageAttributes.m_showMaterials = false;
					theImageAttributes.m_pMaterialsButton->SetCheck(FALSE);
					theImageAttributes.m_pImagesButton->SetCheck(TRUE);
				}
				else
				{
					theImageAttributes.m_showMaterials = true;
					theImageAttributes.m_pMaterialsButton->SetCheck(TRUE);
					theImageAttributes.m_pImagesButton->SetCheck(FALSE);
				}
			}
			else
			{
				theImageAttributes.m_showMaterials = false;
				theImageAttributes.m_pMaterialsButton->SetCheck(FALSE);
				theImageAttributes.m_pImagesButton->SetCheck(TRUE);
			}

			// Add the image list
			theImageAttributes.InitImageList(hWnd);

			// Add the rollup panels to the rollup window:
			// Add the Key/Val panel:
			IRollupWindow* pRollupWindow = GetIRollup(GetDlgItem(theImageAttributes.hDlg, IDC_CUSTOM_ROLLUP));
			ICatRegistry* pCatReg = GetIRollupSettings()->GetCatReg();
			pCatReg->Load();
			static TCHAR keyValBuf[80];
			int bufLen = 80;
			LoadString(hInstance, IDS_KEYS_AND_VALUES, keyValBuf, bufLen);
			int nErrorCat = 0;
			int nCat = pCatReg->GetCat(ImageAttributesDesc.SuperClassID(), ImageAttributesDesc.ClassID(), keyValBuf, nErrorCat);
			theImageAttributes.InitKeyValPanel(pRollupWindow, hInstance, (LPARAM)theImageAttributes.hDlg, nCat);
			// Add the Attributes panel:
			static TCHAR attribBuf[80];
			LoadString(hInstance, IDS_ATTRIBUTES, attribBuf, bufLen);
			nCat = pCatReg->GetCat(ImageAttributesDesc.SuperClassID(), ImageAttributesDesc.ClassID(), attribBuf, nErrorCat);
			pRollupWindow->AppendRollup(hInstance, MAKEINTRESOURCE(IDD_ATTRIBUTES), AttributesRollupPageProc,
												 attribBuf, (LPARAM)theImageAttributes.hDlg, 0, nCat);
			// Set up the state of the panels:
			pRollupWindow->Show();
			std::string strAttribTitle = attribBuf;
			std::string strKeyValTitle = keyValBuf;
			int nNumPanels = pRollupWindow->GetNumPanels();
			for (int nIndex = 0; nIndex < nNumPanels; nIndex++)
			{
				HWND hwndCurrPanel = pRollupWindow->GetPanelDlg(nIndex);
				IRollupPanel* pCurrRollupPanel = pRollupWindow->GetPanel(hwndCurrPanel);
				HWND hwndTitle = pCurrRollupPanel->GetTitleWnd();
				int nBufLength = 80;
				TCHAR titleBuf[80];
				GetWindowText(hwndTitle, titleBuf, nBufLength);
				std::string strCurrTitle = titleBuf;
				// Find which panel we are at:
				if (strCurrTitle == strAttribTitle)
				{
					// We are at the attrib panel:
					DWORD dwAttribPanelOpen;
					if (myRegistry.Get(ATTRIB_PANEL_OPEN_NAME, &dwAttribPanelOpen) == TRUE)
					{
						// The registry entry was found so set state acccordingly:
						if (dwAttribPanelOpen == 0)
							pRollupWindow->SetPanelOpen(nIndex, FALSE);
						else
							pRollupWindow->SetPanelOpen(nIndex, TRUE);
					}
					else
					{
						// Set the default open/closed state:
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

			theImageAttributes.AdjustPanelHeights();
			// Register the rollup callback - This allows for the panels to be
			// repositioned by the user:
			pRollupWindow->RegisterRollupCallback(&theImgAttrRollupCallBack);
			// Release the rollup window:
			ReleaseIRollup(pRollupWindow);

			return TRUE;
		}
	case WM_NOTIFY:
		{
			// This is where I provide the tooltip text for the toolbar buttons:
			LPNMHDR hdr = (LPNMHDR)lParam;
			if (hdr->code == TTN_NEEDTEXT)
			{
				CHAR charBuff[175];
				LPTOOLTIPTEXT lpttt;
				lpttt = (LPTOOLTIPTEXT)hdr;				
				switch (lpttt->hdr.idFrom)
				{
				case ID_BUTTON_ONETOMANY:
					{
						LoadString(hInstance, IDS_ONETOMANY_TOOLTIP, charBuff, sizeof(charBuff)); 
						lpttt->lpszText = charBuff;
						break;
					}
				case ID_BUTTON_ONETOMANY_REPLACE:
					{
						LoadString(hInstance, IDS_ONETOMANY__REPLACE_TOOLTIP, charBuff, sizeof(charBuff)); 
						lpttt->lpszText = charBuff;
						break;
					}
				case ID_BUTTON_IMAGES:
					{
						LoadString(hInstance, IDS_IMAGES_TOOLTIP, charBuff, sizeof(charBuff)); 
						lpttt->lpszText = charBuff;
						break;
					}

				case ID_BUTTON_MATERIALS:
					{
						LoadString(hInstance, IDS_MATERIALS_TOOLTIP, charBuff, sizeof(charBuff)); 
						lpttt->lpszText = charBuff;
						break;
					}
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
			GetClientRect(rollupHWND, &dialogRect);

			int nRollupTop = SPACE_ABOVE_ROLLUP;

			// Find the new left:
			int rollupWidth = dialogRect.right - dialogRect.left;
			int nRollupLeft = nDialogWidth - rollupWidth - INSIDE_FRAME_SPACE_ROLLUP * 2;

			// Find the new right:
			int nRollupRight = nDialogWidth - INSIDE_FRAME_SPACE_ROLLUP;

			// Find the new bottom:
			int nRollupBottom = nDialogHeight - INSIDE_FRAME_SPACE_ROLLUP;

			// Move the rollup:
			MoveWindow(rollupHWND, nDialogWidth - ROLLUP_WIDTH, nRollupTop, ROLLUP_WIDTH, nRollupBottom - nRollupTop, TRUE);

			// position the main toolbar
			HWND hToolbar = GetDlgItem(hWnd, IDC_TOOLBAR_MAIN);
			RECT toolbarRect;
			GetClientRect(hToolbar, &toolbarRect);
			MoveWindow(hToolbar, nDialogWidth - ROLLUP_WIDTH, 0, ROLLUP_WIDTH, toolbarRect.bottom - toolbarRect.top, TRUE);

			// position the image list
			HWND hListBox = GetDlgItem(hWnd, IDC_IMAGELIST);
			int imageListTop = nRollupTop;
			int imageListLeft = 0;
			int imageListRight = nRollupLeft - SLIDER_SIZE;
			int imageListBottom = nRollupBottom;
			MoveWindow(hListBox, 0, imageListTop, nDialogWidth - ROLLUP_WIDTH - SLIDER_SIZE, imageListBottom - imageListTop, TRUE);

#if 0
			// The following is just a test to see if I could get the panel to
			// adjust in width dynamically.  I am waiting to hear back from
			// Discreet on this.
			// Move the rollup panel:
			IRollupWindow* pRollupWindow = GetIRollup(rollupHWND);
			HWND panelHWND = pRollupWindow->GetPanelDlg(0);
			HWND rollupPanelHWND = GetParent(panelHWND);
			MoveWindow(rollupPanelHWND, 0, 3, 200, 200, TRUE);
			ReleaseIRollup(pRollupWindow);
#endif
			return FALSE;
		}

	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDCANCEL:
				{
					// Save off the position of the dialog:
					RECT dialogRect;
					GetWindowRect(theImageAttributes.hDlg, &dialogRect);
					Registry myRegistry(IMAGE_ATTRIB_KEY);
					myRegistry.Set(DIALOG_TOP_VAL_NAME, (DWORD) dialogRect.top);
					myRegistry.Set(DIALOG_BOTTOM_VAL_NAME, (DWORD) dialogRect.bottom);
					myRegistry.Set(DIALOG_LEFT_VAL_NAME, (DWORD) dialogRect.left);
					myRegistry.Set(DIALOG_RIGHT_VAL_NAME, (DWORD) dialogRect.right);

					myRegistry.Set(SHOW_ALL_IMAGES_NAME, (DWORD) theImageAttributes.m_showAllImages);
					myRegistry.Set(SHOW_MATERIALS_NAME, (DWORD) theImageAttributes.m_showMaterials);

					// Save off the order of the rollup panels:
					TCHAR titleBuf[80];
					int bufLen = 80;
					LoadString(hInstance, IDS_KEYS_AND_VALUES, titleBuf, bufLen);
					std::string strKeyValTitle = titleBuf;
					LoadString(hInstance, IDS_ATTRIBUTES, titleBuf, bufLen);
					std::string strAttribTitle = titleBuf;
					IRollupWindow* pRollupWindow = GetIRollup(GetDlgItem(theImageAttributes.hDlg, IDC_CUSTOM_ROLLUP));
					int nScrollPos = pRollupWindow->GetScrollPos();
					int nNumPanels = pRollupWindow->GetNumPanels();
					for (int nIndex = 0; nIndex < nNumPanels; nIndex++)
					{
						HWND hwndCurrPanel = pRollupWindow->GetPanelDlg(nIndex);
						IRollupPanel* pCurrRollupPanel = pRollupWindow->GetPanel(hwndCurrPanel);
						HWND hwndTitle = pCurrRollupPanel->GetTitleWnd();
						int nBufLength = 80;
						TCHAR titleBuf[80];
						GetWindowText(hwndTitle, titleBuf, nBufLength);
						std::string strCurrTitle = titleBuf;
						// Find which panel we are at:
						if (strCurrTitle == strAttribTitle)
						{
							// We are at the attrib panel:
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

					// Save off the nonexclusive keys in the registry:
					theImageAttributes.SaveNonexclusiveKeys();

					// Now clean up:
					// Release any controls:
					ReleaseICustToolbar(theImageAttributes.m_pMainToolbar);	
					ImageList_Destroy(theImageAttributes.m_hImageMainToolbar);
					ReleaseICustButton(theImageAttributes.m_pOneToManyButton);
					ReleaseICustButton(theImageAttributes.m_pOneToManyButtonReplace);

					// Clean up things dealing with the keyval panel:
					theImageAttributes.IdCancelMessage();

					// Unregister the rollup callback:
					pRollupWindow->UnRegisterRollupCallback(&theImgAttrRollupCallBack);
					ReleaseIRollup(pRollupWindow);

					theImageAttributes.bDialogUp = false;

					EndDialog(hWnd, FALSE);
					theImageAttributes.DestroyUIDisplay();
					theImageAttributes.DestroyCurrAttribData();
					if (theImageAttributes.iu)
						theImageAttributes.iu->CloseUtility();
					theImageAttributes.hDlg = NULL;
					theImageAttributes.End();
					return TRUE;
				}

			case ID_BUTTON_ONETOMANY:
				{
					// Uncheck the other one-to-many button:
					if (theImageAttributes.m_pOneToManyButtonReplace->IsChecked() == TRUE)
						theImageAttributes.m_pOneToManyButtonReplace->SetCheck(FALSE);

					theImageAttributes.m_pickState = ImageAttributes::PS_ONETOMANY;

					return TRUE;
				}

			case ID_BUTTON_ONETOMANY_REPLACE:
				{
					// Uncheck the other one-to-many button:
					if (theImageAttributes.m_pOneToManyButton->IsChecked() == TRUE)
						theImageAttributes.m_pOneToManyButton->SetCheck(FALSE);

					theImageAttributes.m_pickState = ImageAttributes::PS_ONETOMANY_REPLACE;

					return TRUE;
				}

			case ID_BUTTON_IMAGES:
				{
					theImageAttributes.m_showMaterials = false;
					theImageAttributes.m_pImagesButton->SetCheck(TRUE);
					theImageAttributes.m_pMaterialsButton->SetCheck(FALSE);
					Interface* ip = GetCOREInterface();
					theImageAttributes.GetImageList(ip, theImageAttributes.m_showAllImages);
					theImageAttributes.DisplayImageList();
					theImageAttributes.UpdateAttribDisplay();

					return TRUE;
				}

			case ID_BUTTON_MATERIALS:
				{
					theImageAttributes.m_showMaterials = true;
					theImageAttributes.m_pImagesButton->SetCheck(FALSE);
					theImageAttributes.m_pMaterialsButton->SetCheck(TRUE);
					Interface* ip = GetCOREInterface();
					theImageAttributes.GetImageList(ip, theImageAttributes.m_showAllImages);
					theImageAttributes.DisplayImageList();
					theImageAttributes.UpdateAttribDisplay();

					return TRUE;
				}

			case IDC_IMAGELIST:
				{
					if (HIWORD(wParam) == LBN_SELCHANGE)
					{
						switch (theImageAttributes.m_pickState)
						{
						case ImageAttributes::PS_ONETOMANY:
							{
								theImageAttributes.CopyAttribsToSelSet();
								theImageAttributes.m_pickState = ImageAttributes::PS_NONE;
								theImageAttributes.m_pOneToManyButton->SetCheck(FALSE);
							} break;

						case ImageAttributes::PS_ONETOMANY_REPLACE:
							{
								theImageAttributes.CopyAttribsToSelSet(true);
								theImageAttributes.m_pickState = ImageAttributes::PS_NONE;
								theImageAttributes.m_pOneToManyButtonReplace->SetCheck(FALSE);
							} break;
						}
						theImageAttributes.UpdateAttribDisplay();
					}

					return TRUE;
				}

			case IDC_RADIO_SHOWALL:
				{
					theImageAttributes.m_showAllImages = true;
					Interface* ip = GetCOREInterface();
					theImageAttributes.GetImageList(ip, theImageAttributes.m_showAllImages);
					theImageAttributes.DisplayImageList();
					theImageAttributes.UpdateAttribDisplay();

					return TRUE;
				}

			case IDC_RADIO_SHOWSEL:
				{
					theImageAttributes.m_showAllImages = false;
					Interface* ip = GetCOREInterface();
					theImageAttributes.GetImageList(ip, theImageAttributes.m_showAllImages);
					theImageAttributes.DisplayImageList();
					theImageAttributes.UpdateAttribDisplay();

					return TRUE;
				}
			}
		}
	}
	return FALSE;
}



//-----------------------------------------------------------------------------
// ImageAttributesCallBack::proc
// Put here by the Appwizard
//
// Input:
//  Interface* ip --
//
// Output: None.
//-----------------------------------------------------------------------------
void ImageAttributesCallBack::proc(Interface* ip)
{
}



//-----------------------------------------------------------------------------
// ImgAttrRollupCallBack Class Definition:

//-----------------------------------------------------------------------------
// ImgAttrRollupCallBack::ImgAttrRollupCallBack
// This method is the default constructor for this class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
ImgAttrRollupCallBack::ImgAttrRollupCallBack()
{
}


//-----------------------------------------------------------------------------
// ImgAttrRollupCallBack::~ImgAttrRollupCallBack
// This method is the destructor for this class.
//
// Input:
//
// Output: 
//-----------------------------------------------------------------------------
ImgAttrRollupCallBack::~ImgAttrRollupCallBack()
{
}


//-----------------------------------------------------------------------------
// ImgAttrRollupCallBack::GetEditObjClassID
// This method is used by Max to get the superclass id and class id.  In this
// case, it is needed during the saving out of the rollup panel order in the
// RollupOrder.cfg file.
//
// Input:
//  SClass_ID& sid -- On output, this will contain the super class id.
//  Class_ID& cid -- On output, this will contain the class id.
//
// Output: TRUE to indicate to the system that drag-n-drop is supported.
//-----------------------------------------------------------------------------
BOOL ImgAttrRollupCallBack::GetEditObjClassID(SClass_ID& sid, Class_ID& cid)
{
	sid = ImageAttributesDesc.SuperClassID();
	cid = ImageAttributesDesc.ClassID();

	return TRUE;
}



//-----------------------------------------------------------------------------
// ImageAttributes Class Definition:

//-----------------------------------------------------------------------------
// ImageAttributes::ImageAttributes
// This method is the default constructor for this class:
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
ImageAttributes::ImageAttributes()
{
	iu = NULL;
	ip = NULL;	
	hDlg = NULL;
	bConfigReadIn = false;
	bDialogUp = false;
	m_bShowControls = false;
	m_bObjsInSel = false;
	controller = NULL;
	m_hAppFont = NULL;
	m_pMainToolbar = NULL;
	m_hImageMainToolbar = NULL;
	m_pOneToManyButton = NULL;
	m_pOneToManyButtonReplace = NULL;
	m_showAllImages = true;
	m_pickState = PS_NONE;
	
	RegisterNotification(NotifySelSetChanged, NULL, NOTIFY_SELECTIONSET_CHANGED);
}


//-----------------------------------------------------------------------------
// ImageAttributes::~ImageAttributes
// This method is the destructor for this class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
ImageAttributes::~ImageAttributes()
{
	if (controller != NULL)
	{
		delete controller;
		controller = NULL;
	}

	UnRegisterNotification(NotifySelSetChanged, NULL, NOTIFY_SELECTIONSET_CHANGED);
}


//-----------------------------------------------------------------------------
// ImageAttributes::BeginEditParams
// This method is called when the user clicks on the Image Attributes button
// in the Utility panel of 3ds Max.
//
// Input:
//  Interface *ip -- An interface pointer that may be used to call methods of
//					 the Interface class.
//  IUtil *iu -- An interface pointer that may be used to close the current
//				 utility.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImageAttributes::BeginEditParams(Interface* ip,IUtil* iu) 
{
	// Set up the font we will use:
	m_hAppFont = GetCOREInterface()->GetAppHFont();

	if (bConfigReadIn == false)
	{
		// Create the controller and tell it to load in the config file:
		controller = new ImgAttrController(this);
		controller->LoadConfigFile(ip);

		bConfigReadIn = true;
	}

	// Set up the dialog:
	this->iu = iu;
	this->ip = ip;
	if (!hDlg)
	{
		this->Init();
		hDlg = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_IMAGEATTRIB),
										 GetActiveWindow(), ImageAttributesDlgProc,
										 (LPARAM) this);
		bDialogUp = true;
	}
	else
	{
		SetActiveWindow(hDlg);
	}

	iu->CloseUtility();
}

	
//-----------------------------------------------------------------------------
// ImageAttributes::EndEditParams
// This method is called when this plug-in is to be closed.
//
// Input:
//  Interface *ip -- An interface pointer that may be used to call methods of
//					 the Interface class.
//  IUtil *iu -- An interface pointer that may be used to close the current
//				 utility.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImageAttributes::EndEditParams(Interface *ip,IUtil *iu)
{
	this->iu = iu;
}


//-----------------------------------------------------------------------------
// ImageAttributes::Init
// This method will initialize the image attribute dialog.  Really, the only
// thing done here is the registering of the callback.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImageAttributes::Init()
{
	ip->RegisterRedrawViewsCallback(&objAttribCallBack);
}


//-----------------------------------------------------------------------------
// ImageAttributes::End
// This method will be called when the dialog box is closed.  It unregisters
// the callback for the dialog.
//
// Input:
//
// Output: None.
//-----------------------------------------------------------------------------
void ImageAttributes::End()
{
	ip->UnRegisterRedrawViewsCallback(&objAttribCallBack);
}


//-----------------------------------------------------------------------------
// ImageAttributes::SetUpUIDisplay
// This method will tell the controller to set up the UIDisplay structure prior
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
//-----------------------------------------------------------------------------
bool ImageAttributes::SetUpUIDisplay(HWND hParentDlg, HINSTANCE hInstance, HFONT hUIFont,
									  int nStartx, int nStarty,
									  int nLabelWidth, int nLabelHeight,
									  int nSpaceBetweenCtrls, WNDPROC newColorSwatchProc,
									  bool bAdjustPanelHeights)
{
	bool bResult = true;

	bResult = controller->SetUpUIDisplay(hParentDlg, hInstance, hUIFont,
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


//-----------------------------------------------------------------------------
// ImageAttributes::AdjustPanelHeights
// This method can be called to adjust the panel heights of all panels in the
// rollup window.  Care should be taken to call this method only after the
// m_pUIController has been created in the ImgAttrController object, and after
// all the panels have been added to the rollup window in the dialog.
//
// Input:
//	bool bUseDefaultSize -- Default value is false, pass in true if you want
//							the panel to revert back to the size of the dialog
//							resource in the resource file.
//
// Output: True if succesful, false otherwise. 
//-----------------------------------------------------------------------------
bool ImageAttributes::AdjustPanelHeights(bool bUseDefaultSize)
{
	bool bResult = false;

	if ((theImageAttributes.hDlg != NULL) && (controller != NULL))
	{
		HWND rollupWindowHWND = GetDlgItem(theImageAttributes.hDlg, IDC_CUSTOM_ROLLUP);
		if (rollupWindowHWND != NULL)
		{
			IRollupWindow* pRollupWindow = GetIRollup(rollupWindowHWND);
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
						IRollupPanel* pCurrRollupPanel = pRollupWindow->GetPanel(hwndCurrPanel);
						HWND hwndTitle = pCurrRollupPanel->GetTitleWnd();
						int nBufLength = 80;
						TCHAR titleBuf[80];
						GetWindowText(hwndTitle, titleBuf, nBufLength);
						std::string strCurrTitle = titleBuf;
						// See if the current panel is the attrib panel:
						if (strCurrTitle == strAttribTitle)
						{
							if (bUseDefaultSize == true)
							{
								int nDefaultResourceHeight = 78;
								pRollupWindow->SetPageDlgHeight(nIndex, nDefaultResourceHeight);

								// I have to repaint the window, because for some
								// reason, Max leaves some artifacting when resizing
								// the dialog to its original size.  I know, it's
								// pretty lame!:
								theImageAttributes.m_bShowControls = false;
								theImageAttributes.m_bObjsInSel = false;
								InvalidateRgn(rollupWindowHWND, NULL, FALSE);
								UpdateWindow(rollupWindowHWND);

								bResult = true;						
							}
							else
							{
								int nTotalY = controller->GetTotalY();
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
			ReleaseIRollup(pRollupWindow);
		}
	}

	return bResult;
}

//-----------------------------------------------------------------------------
// ImageAttributes::HandleWinMessage
// This method will take the incoming message and pass it onto the
// ImgAttrController to see if any of its controls want to handle the message.
// The return value from ImgAttrController is then passed back as the return
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
//-----------------------------------------------------------------------------
bool ImageAttributes::HandleWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam)
{
	bool bResult = false;

	if (controller != NULL)
	{
		// Pass the message on to the Controller:
		bResult = controller->HandleWinMessage(hParent, message, wParam, lParam);
	}
	else
	{
		bResult = false;
	}

	return bResult;
}


//-----------------------------------------------------------------------------
// ImageAttributes::LoadCurrAttribData
// This method will tell the controller to build the CurrAttribData based on
// the selection set passed in through the Interface pointer.
//
// Input:
//  Interface *ip -- The Interface pointer to pass on to the controller.
//
// Output: Passes back the return value from the controller.  In this case
//		   true if there are objects in the selection set, false otherwise.
//-----------------------------------------------------------------------------
bool ImageAttributes::LoadCurrAttribData(Interface *ip)
{
	return controller->LoadCurrAttribData(ip, m_showMaterials);
}


//-----------------------------------------------------------------------------
// ImageAttributes::CopyAttribsToSelSet
// This method will tell the controller to copy the attributes from the node
// passed in to the nodes in the current selection set (which should be
// currently stored in the CurrAttribController).
//
// Input:
//  INode* pNode -- The node to get the attributes from.
//	bool bReplace -- True if the destination nodes should have all of their
//					 attributes thrown away and replaced with the source node's
//					 attributes.  Default value is false.
//
// Output: Passes back the return value from the controller, or false if it is
//		   NULL.
//-----------------------------------------------------------------------------
bool ImageAttributes::CopyAttribsToSelSet(bool bReplace)
{
	bool bResult = false;

	if (controller != NULL)
	{
		bResult =  controller->CopyAttribsToSelSet(bReplace);
		if (bResult == true)
		{
			// Set the appropriate flags:
			theImageAttributes.m_bShowControls = true;
			theImageAttributes.m_bObjsInSel = true;

			// Fire off the WM_PAINT message for the attribute panel:
			InvalidateRgn(theImageAttributes.hDlg, NULL, FALSE);
			UpdateWindow(theImageAttributes.hDlg);

			// Load the key/val panel:
			theImageAttributes.LoadKeyValPanelTree();
		}
		else
		{
			// No attribs to copy:
			CHAR messageBuff[175];
			CHAR captionBuff[175];
			LoadString(hInstance, IDS_NO_ATTRIBUTES, messageBuff, sizeof(messageBuff));
			LoadString(hInstance, IDS_IMAGE_ATTRIBUTES, captionBuff, sizeof(captionBuff));
			MessageBox(theImageAttributes.ip->GetMAXHWnd(), messageBuff, captionBuff,
					   MB_OK | MB_ICONEXCLAMATION);
		}
	}

	return bResult;
}


//-----------------------------------------------------------------------------
// ImageAttributes::DestroyCurrAttribData
// This method will tell the controller to destroy any current attribute data.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImageAttributes::DestroyCurrAttribData()
{
	controller->DestroyCurrAttribData();
}


//-----------------------------------------------------------------------------
// ImageAttributes::DestroyUIDisplay
// This method will tell the controller to destroy any UIDisplay.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImageAttributes::DestroyUIDisplay()
{
	controller->DestroyUIDisplay();
}


//-----------------------------------------------------------------------------
// ImageAttributes::SaveNonexclusiveKeys
// This method will the controller to save the nonexclusive keys to the
// registry.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImageAttributes::SaveNonexclusiveKeys()
{
	controller->SaveNonexclusiveKeys();
}


//-----------------------------------------------------------------------------
// ImageAttributes::InitKeyValPanel
// This method will tell the controller to set up the KeyVal panel.
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
void ImageAttributes::InitKeyValPanel(IRollupWindow* pRollupWindow, HINSTANCE hInstance, LPARAM lParam, int nCategory)
{
	controller->InitKeyValPanel(pRollupWindow, hInstance, lParam, nCategory);
}


//-----------------------------------------------------------------------------
// ImageAttributes::IdCancelMessage
// This method will notify the controller that the IDCANCEL message was
// received.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImageAttributes::IdCancelMessage()
{
	controller->IdCancelMessage();
}


//-----------------------------------------------------------------------------
// ImageAttributes::SetApplyBttnStates
// This method will tell the controller to set the apply button states for the
// key/val panel.
//
// Input:
//  bool bApplySelEnabled -- True if the Apply To Sel button is to be enabled.
//  bool bApplySceneEnabled -- True if the Apply To Scene button is to be
//							   enabled.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImageAttributes::SetApplyBttnStates(bool bApplySelEnabled, bool bApplySceneEnabled)
{
	controller->SetApplyBttnStates(bApplySelEnabled, bApplySceneEnabled);
}


//-----------------------------------------------------------------------------
// ImageAttributes::LoadKeyValPanelTree
// This method will tell the controller to load the key/vals in the key/val
// panel.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImageAttributes::LoadKeyValPanelTree()
{
	controller->LoadKeyValPanelTree();
}


/****************************************************************************
 ImageAttributes::InitImageList()
****************************************************************************/
void ImageAttributes::InitImageList(HWND hParent)
{
	controller->InitImageList(hParent);
}


/****************************************************************************
 ImageAttributes::GetImageList()
****************************************************************************/
bool ImageAttributes::GetImageList(Interface *pInterface, bool viewAll)
{
	return controller->GetImageList(pInterface, viewAll, m_showMaterials);
}


/****************************************************************************
 ImageAttributes::DisplayImageList()
****************************************************************************/
void ImageAttributes::DisplayImageList()
{
	controller->DisplayImageList();
}


/****************************************************************************
 ImageAttributes::UpdateAttribDisplay()

 Updates the current attributes display based on the selection set, whether
 or not images/materials are selected in the list, etc.
****************************************************************************/
void ImageAttributes::UpdateAttribDisplay(bool resize)
{
	// see if any images/materials are selected, and load their attributes
	if (LoadCurrAttribData(ip))
	{
		// Hide the "No objects" label:
		ShowWindow(GetDlgItem(m_hAttribsRollup, IDC_STATIC_SELOBJECTS), SW_HIDE);

		// Enable the "Copy Attributes" buttons:
		m_pOneToManyButton->Enable(TRUE);
		m_pOneToManyButtonReplace->Enable(TRUE);


		// Enable the apply buttons in the key/val panel:
		SetApplyBttnStates(true, true);

		// Load the key/val panel:
		LoadKeyValPanelTree();

		// Set up the UI Display:
		SetUpUIDisplay(m_hAttribsRollup, hInstance, theImageAttributes.m_hAppFont,
							START_X, START_Y, LABEL_WD, LABEL_HT,
							SPACE_BET_CTRLS, colorSwatchSubclassWndProc, resize);
	}
	else
	{
		// Show the "No objects" label:
		ShowWindow(GetDlgItem(m_hAttribsRollup, IDC_STATIC_SELOBJECTS), SW_SHOW);
		AdjustPanelHeights(true);

		// Disable the "Copy Attributes" buttons:
		m_pOneToManyButton->Disable();
		m_pOneToManyButtonReplace->Disable();

		// Disable the apply buttons in the key/val panel:
		SetApplyBttnStates(false, false);

		// Destroy any CurrAttribData and UIDisplay:
		theImageAttributes.DestroyCurrAttribData();
		theImageAttributes.DestroyUIDisplay();
	}

	// Fire off the WM_PAINT message to the attribute panel:
	InvalidateRgn(hDlg, NULL, FALSE);
	UpdateWindow(hDlg);
}