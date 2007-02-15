//-----------------------------------------------------------------------------
//	File:		UIController.h
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


#ifndef __UICONTROLLER__
#define __UICONTROLLER__



//-----------------------------------------------------------------------------
// Includes:

//-----------------------------------------------------------------------------
// Pre-declarations, etc.:



//-----------------------------------------------------------------------------
// Class Declaration:

class UIDisplayBase;

class UIController
{
public:
	UIController();
	~UIController();

	// Mutators:
	void AddDisplayObject(UIDisplayBase* pDisplayObject);

	// Accessors:
	int GetTotalY();

	// Misc. Methods:
	bool BuildActualUI(HWND hParentDlg, HINSTANCE hInstance, HWND hToolTip,
					   int nStartx, int nStarty,
					   int nLabelWidth, int nLabelHeight,
					   int nSpaceBetweenCtrls);
	bool SetNewSwatchProc(WNDPROC newColorSwatchProc);
	bool HandleWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
						  TupString& strKeyname, TupString& strValue,
						  TupArray<TupString>& vectKeysToPurge, int& nFlags);

	void RemoveToolTips(HWND hParent,HINSTANCE hInstance,HWND hToolTip);

	int GetNumDisplayObjects(void) const;
	const UIDisplayBase* GetDisplayObject(int index) const;

protected:
	TupArray<UIDisplayBase*> m_vectDisplayObjects;
	int m_nTotalY;

private:
};

//-----------------------------------------------------------------------------


#endif
