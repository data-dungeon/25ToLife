//-----------------------------------------------------------------------------
//	File:		UIDisplaySetColor.h
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplaySetColor class contains all the information needed to
//			display the proper ui control for a setcolorbox in max.  It does
//			not handle the actual drawing of the control or the message
//			handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __UIDISPLAYSETCOLOR__
#define __UIDISPLAYSETCOLOR__



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

class UIDisplaySetColor : public UIDisplayBase
{
public:
	UIDisplaySetColor();
	~UIDisplaySetColor();

	// Accessors:
	int GetSelectionIndex();
	AttribColor GetColor(int nIndex);
	int GetCurrColorID();
	int GetChoiceCtrlID(int nIndex);

	// Mutators:
	void SetSelectionIndex(int nNewSelIndex);
	void AddColor(AttribColor newColor);
	void SetCurrColorID(int nNewID);
	void AddChoiceCtrlID(int nNewID);
	void SetShowIndeterminate(bool bShow);

	void SetLabelID(int nNewLabelID);

	// Misc. methods:
	bool BuildActualUI(HWND hParentDlg, HINSTANCE hInstance,	HWND hToolTip,
					   int nStartx, int nStarty, int nLabelWidth, int nLabelHeight,
					   int& nControlSizeY);
	bool HandledWinMessage(HWND hColorSwatch, UINT message, WPARAM wParam, LPARAM lParam,
						   TupString& strKeyname, TupString& strValue,
						   TupArray<TupString>& vectKeysToPurge, int& nFlags);
	void RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip);

protected:
	int m_nCurrSelIndex;
	TupArray<AttribColor> m_vectColors;
	TupArray<int> m_vectControlIDs;
	int m_nCurrColorID;
	int m_nLabelID;
	HWND m_hLabel;
	HWND m_hCurrColorCtrl;
	TupArray<HWND> m_vectColorChoiceHWNDs;
	bool m_bShowIndeterminate;

private:
};

//-----------------------------------------------------------------------------


#endif
