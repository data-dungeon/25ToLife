//-----------------------------------------------------------------------------
//	File:		UIDisplayCombo.h
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplayCombo class contains all the information needed to
//			display the proper ui control for a combo box in max.  It does not
//			handle the actual drawing of the control or the message handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __UIDISPLAYCOMBO__
#define __UIDISPLAYCOMBO__



//-----------------------------------------------------------------------------
// Includes:

#ifndef __UIDISPLAYBASE__
#include "UIDisplayBase.h"
#endif

//-----------------------------------------------------------------------------
// Helper Structures:

//-----------------------------------------------------------------------------
// Pre-declarations, etc.:



//-----------------------------------------------------------------------------
// Class Declaration:

class UIDisplayCombo : public UIDisplayBase
{
public:
	UIDisplayCombo();
	~UIDisplayCombo();

	// Accessors:
	int GetControlID();
	int GetSelectionIndex();
	TupString GetComboLabel(int nIndex);
	TupString GetComboKey(int nIndex);

	// Mutators:
	void SetLabelID(int nNewLabelID);
	void SetControlID(int nNewControlID);
	void SetSelectionIndex(int nNewSelIndex);
	void AddComboLabel(const TupString &strLabel);
	void AddComboKey(const TupString &strKey);
	void AddComboToolTip(const TupString &strLabel,const TupString &strKey,const TupString &strToolTip);
	void SetShowIndeterminate(bool bShow);
	void AddPurgeKey(const TupString &strKeyToPurge);

	// Misc. methods:
	bool BuildActualUI(HWND hParentDlg, HINSTANCE hInstance,	HWND hToolTip,
					   int nStartx, int nStarty, int nLabelWidth, int nLabelHeight,
					   int& nControlSizeY);
	bool HandledWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
						   TupString& strKeyname, TupString& strValue,
						   TupArray<TupString>& vectKeysToPurge, int& nFlags);
	void RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip);

protected:
	int m_nControlID;
	int m_nLabelID;
	int m_nCurrSelIndex;
	TupArray<TupString> m_vectComboLabels;
	TupArray<TupString> m_vectComboKeys;
	TupArray<TupString> m_vectComboToolTips;
	TupArray<TupString> m_vectKeysToPurge;
	bool m_bShowIndeterminate;
	HWND m_hLabel;
	HWND m_hComboBox;

private:
};

//-----------------------------------------------------------------------------


#endif
