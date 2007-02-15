//-----------------------------------------------------------------------------
//	File:		UIDisplayFreeColor.h
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplayFreeColor class contains all the information needed to
//			display the proper ui control for a freecolorbox in max.  It does
//			not handle the actual drawing of the control or the message
//			handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __UIDISPLAYFREECOLOR__
#define __UIDISPLAYFREECOLOR__



//-----------------------------------------------------------------------------
// Includes:

#ifndef __UIDISPLAYBASE__
#include "UIDisplayBase.h"
#endif

#ifndef __CURRATTRIBBASE__
#include "CurrAttribBase.h"
#endif

//-----------------------------------------------------------------------------
// Pre-declarations, etc.:



//-----------------------------------------------------------------------------
// Class Declaration:

class UIDisplayFreeColor : public UIDisplayBase
{
public:
	UIDisplayFreeColor();
	~UIDisplayFreeColor();

	// Accessors:
	AttribColor GetCurrentColor();
	int GetControlID();

	// Mutators:
	void SetCurrentColor(AttribColor newColor);
	void SetCurrentColor(COLORREF newColor);
	void SetLabelID(int nNewLabelID);
	void SetControlID(int nNewControlID);

	// Misc. methods:
	bool BuildActualUI(HWND hParentDlg, HINSTANCE hInstance,	HWND hToolTip,
					   int nStartx, int nStarty, int nLabelWidth, int nLabelHeight,
					   int& nControlSizeY);
	bool HandledWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
						   TupString& strKeyname, TupString& strValue,
						   TupArray<TupString>& vectKeysToPurge, int& nFlags);
	void RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip);

protected:
	AttribColor m_colorCurrent;
	int m_nControlID;
	int m_nLabelID;
	HWND m_hLabel;
	HWND m_hFreeColorCtrl;

private:
};

//-----------------------------------------------------------------------------


#endif
