//-----------------------------------------------------------------------------
//	File:		UIController.cpp
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIController class contains all the information needed to
//			display the proper ui controls for attribute data.  It does not
//			handle the actual drawing of the controls or the message handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "UIController.h"
#include "FuncPtrController.h"
#include "UIDisplayBase.h"

//-----------------------------------------------------------------------------
// Defines, Globals, etc.:



//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// UIController::UIController
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIController::UIController()
{
	m_nTotalY = -1;
}


//-----------------------------------------------------------------------------
// UIController::~UIController
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIController::~UIController()
{
	int numDisplayObjs = m_vectDisplayObjects.GetSize();
	for (int nIndex = numDisplayObjs - 1; nIndex >= 0; nIndex--)
	{
		UIDisplayBase* pCurrObj = m_vectDisplayObjects[nIndex];
		if (pCurrObj != NULL)
		{
			delete pCurrObj;
			pCurrObj = NULL;
		}
		m_vectDisplayObjects.RemoveAt(nIndex);			
	}

	m_vectDisplayObjects.RemoveAll();
}


//-----------------------------------------------------------------------------
// UIController::AddDisplayObject
// This method will add the object pointer passed in to the vector of UI
// objects.
//
// Input:
//  UIDisplayBase* pDisplayObject -- The pointer to add.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIController::AddDisplayObject(UIDisplayBase* pDisplayObject)
{
	m_vectDisplayObjects.Add(pDisplayObject);
}


//-----------------------------------------------------------------------------
// UIController::GetTotalY
// This method will return the total amount of y spacing that the current
// UI controls take up.
//
// Input: None.
//
// Output: int representing the total amount of y.
//-----------------------------------------------------------------------------
int UIController::GetTotalY()
{
	return m_nTotalY;
}


//-----------------------------------------------------------------------------
// UIController::BuildActualUI
// This method will go through each of the UIDisplay objects and set up and
// create the actual UI information necessary to display the controls.
//
// Input:
//  HWND hParentDlg -- The HWND of the parent dialog.
//  HINSTANCE hInstance -- The handle to the application instance.
//	 HWND hToolTip -- The handle to the tooltip for the parent dialog.
//  HFONT hUIFont -- The font to use (some controls and static texts need this).
//  int nStartx -- The x coord to start the labels and controls at.
//  int nStarty -- The y coord to start the first control at.
//  int nLabelWidth -- The width of the control labels.
//  int nLabelHeight -- The height of the control labels.
//  int nSpaceBetweenCtrls -- The spacing between controls.
//
// Output: True if this method is successful, false otherwise.
//-----------------------------------------------------------------------------
bool UIController::BuildActualUI(HWND hParentDlg, HINSTANCE hInstance,  HWND hToolTip,
								 int nStartx, int nStarty,
								 int nLabelWidth, int nLabelHeight,
								 int nSpaceBetweenCtrls)
{
	bool bResult = true;

	// Cycle through each of the UIDisplay objects and tell it to create its UI:
	int nControlSizeY;
	int nNumObjects = m_vectDisplayObjects.GetSize();
	for (int nIndex = 0; nIndex < nNumObjects; nIndex++)
	{
		bResult = (m_vectDisplayObjects[nIndex])->BuildActualUI(hParentDlg, hInstance, hToolTip,
																 nStartx, nStarty,
																 nLabelWidth, nLabelHeight,
																 nControlSizeY);		
		nStarty = nStarty + nControlSizeY + nSpaceBetweenCtrls;

		if (bResult == false)
			break;
	}

	if (bResult == false)
		m_nTotalY = -1;
	else
		m_nTotalY = nStarty;

	return bResult;
}


//-----------------------------------------------------------------------------
// UIController::SetSwatchFuncPtr
// This method will tell the FuncPtrController to set the new color swatch
// proc to the WNDPROC passed in.  
//
// Input:
//  WNDPROC newColorSwatchProc -- The new WNDPROC to set for the color swatches.
//
// Output: True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool UIController::SetNewSwatchProc(WNDPROC newColorSwatchProc)

{
	theFuncPtrController.SetNewSwatchProc(newColorSwatchProc);
	return true;
}


//-----------------------------------------------------------------------------
// UIController::HandleWinMessage
// This method will take the message passed in and pass it on to each of the
// UIDisplay objects so they can have a chance to handle the message.  If any
// of the objects actual handle the message, then the keyname and value will
// be passed back in the strKeyname and strValue parameters.
//
// Input:
//  HWND hParent -- The HWND of the parent of the control the message pertains to.
//  UINT message -- Specifies the message.
//  WPARAM wParam -- Specifies additional message information. The contents of
//					 this parameter depend on the value of message.
//  LPARAM lParam -- Specifies additional message information. The contents of
//					 this parameter depend on the value of message.
//  TupString& strKeyname -- If any object handled the message, then upon
//							   return, this parameter will hold the keyname in
//							   question, otherwise the contents will not be changed.
//  TupString& strValue -- If any object handled the message, then upon
//							 return, this parameter will hold the value in
//							 question, otherwise the contents will not be changed.
//	TupArray<TupString>& vectKeysToPurge -- Vector of keys that the UIDisplay
//												 object says should be purged.
//	int& nFlags -- Any flags that need to be returned to the calling routine.
//
// Output: True if any of the display objects handle the message, false
//		   otherwise.  Also, see above for the parameters that are also used as
//					   return values.
//-----------------------------------------------------------------------------
bool UIController::HandleWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
									TupString& strKeyname, TupString& strValue,
									TupArray<TupString>& vectKeysToPurge, int& nFlags)
{
	bool bResult = false;

	// Cycle through all the display objects and pass the message to each.
	int nNumObjects = m_vectDisplayObjects.GetSize();
	for (int nIndex = 0; nIndex < nNumObjects; nIndex++)
	{
		bResult = (m_vectDisplayObjects[nIndex])->HandledWinMessage(hParent, message,
																	 wParam, lParam,
																	 strKeyname, strValue,
																	 vectKeysToPurge, nFlags);
		if (bResult == true)
			break;
	}

	return bResult;
}

//-----------------------------------------------------------------------------
// UIController::RemoveToolTips
// This method will tell the FuncPtrController to set the new color swatch
// proc to the WNDPROC passed in.  
//
// Input:
//  HWND hParent -- The HWND of the parent of the control the message pertains to.
//  HINSTANCE hInstance -- Appliation instance handle.
//  HWND hToolTip -- the HWND of the tooltip control
//
// Output: none
//-----------------------------------------------------------------------------
void UIController::RemoveToolTips(HWND hParent,HINSTANCE hInstance,HWND hToolTip)
{
	int nNumObjects = m_vectDisplayObjects.GetSize();
	for (int nIndex = 0; nIndex < nNumObjects; nIndex++)
	{
		(m_vectDisplayObjects[nIndex])->RemoveToolTips(hParent, hInstance,hToolTip);
	}
}

//-----------------------------------------------------------------------------
// UIController::GetNumDisplayObjects
// This method will return the number of display objects
//
// Input: None
//
// Output: int representing the total number of display objects
//-----------------------------------------------------------------------------
int UIController::GetNumDisplayObjects(void) const
{
	return m_vectDisplayObjects.GetSize();
}

//-----------------------------------------------------------------------------
// UIController::GetDisplayObject
// This method will return the number of display objects
//
// Input: int representing the index in the array
//
// Output: UIDisplayBase * pointer to UIDisplay object
// if the index is not within bounds NULL will be returned
//-----------------------------------------------------------------------------
const UIDisplayBase* UIController::GetDisplayObject(int index) const
{
	if (index>=0 && index<m_vectDisplayObjects.GetSize())
		return m_vectDisplayObjects[index];
	else
		return NULL;
}
