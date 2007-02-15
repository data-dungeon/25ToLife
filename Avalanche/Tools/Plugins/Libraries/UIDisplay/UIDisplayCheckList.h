//-----------------------------------------------------------------------------
//	File:		UIDisplayCheckList.h
//	Created:	July 30, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplayCheckList class contains all the information needed to
//			display the proper ui control for a checklist in Max.  It does not
//			handle the actual drawing of the control or the message handling.
//
//	History:	7/30/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __UIDISPLAYCHECKLIST__
#define __UIDISPLAYCHECKLIST__



//-----------------------------------------------------------------------------
// Includes:

#ifndef __UIDISPLAYBASE__
#include "UIDisplayBase.h"
#endif

//-----------------------------------------------------------------------------
// Pre-declarations, etc.:



//-----------------------------------------------------------------------------
// Class Declaration:

class UIDisplayCheckList : public UIDisplayBase
{
public:
	UIDisplayCheckList();
	~UIDisplayCheckList();

	// Public accessors & mutators:
	void SetLabelID(int nNewLabelID);
	void SetControlID(int nNewControlID);
	void SetPixelHeight(int nNewPixelHeight);
	void SetDefaultState(int nNewDefaultState);
	void SetEntityNames(const Keeper<TupString>& vectEntityNames);
	void SetCheckedStates(const TupArray<int>& vectCheckedStates);

	// Misc. methods:
	bool BuildActualUI(HWND hParentDlg, HINSTANCE hInstance,	HWND hToolTip,
					   int nStartx, int nStarty, int nLabelWidth, int nLabelHeight,
					   int& nControlSizeY);
	bool HandledWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
						   TupString& strKeyname, TupString& strValue,
						   TupArray<TupString>& vectKeysToPurge, int& nFlags);
	void RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip);

protected:
	// Protected methods:
	void LoadListView();

	// Protected data members:
	int m_nDefaultState;
	int m_nLabelID;
	int m_nControlID;
	int m_nPixelHeight;
	HWND m_hLabel;
	HWND m_hCheckList;
	TupArray<TupString> m_vectEntityNames;
	TupArray<int> m_vectCheckedStates;

private:
};

//-----------------------------------------------------------------------------


#endif
