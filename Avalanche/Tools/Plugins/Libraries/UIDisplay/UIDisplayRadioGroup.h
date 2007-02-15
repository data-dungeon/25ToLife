//-----------------------------------------------------------------------------
//	File:		UIDisplayRadioGroup.h
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplayRadioGroup class contains all the information needed
//			to display the proper ui control for a radio group in max.  It does
//			not handle the actual drawing of the control or the message
//			handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __UIDISPLAYRADIOGROUP__
#define __UIDISPLAYRADIOGROUP__



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

class UIDisplayRadioGroup : public UIDisplayBase
{
public:
	UIDisplayRadioGroup();
	~UIDisplayRadioGroup();

	// Accessors:
	int GetSelectionIndex();
	const TupString &GetRadioLabel(int nIndex);
	const TupString &GetRadioKey(int nIndex);
	const TupString &GetRadioToolTip(int nIndex);
	int GetControlID(int nIndex);

	// Mutators:
	void SetSelectionIndex(int nNewSelIndex);
	void AddRadioLabel(const TupString &strLabel);
	void AddRadioKey(const TupString &strKey);
	void AddRadioToolTip(const TupString &strLabel,const TupString &strKey,const TupString &strToolTip);
	void AddControlID(int nNewID);
	void AddPurgeKey(TupString strKeyToPurge);

	void SetLabelID(int nNewLabelID);

	// Public methods:
	bool BuildActualUI(HWND hParentDlg, HINSTANCE hInstance,	HWND hToolTip,
					   int nStartx, int nStarty, int nLabelWidth, int nLabelHeight,
					   int& nControlSizeY);
	bool HandledWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
						   TupString& strKeyname, TupString& strValue,
						   TupArray<TupString>& vectKeysToPurge, int& nFlags);
	void RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip);

protected:
	// Protected methods:
	int IdInGroupAt(int nIdToCheck);

	// Protected data members:
	int m_nCurrSelIndex;
	TupArray<TupString> m_vectRadioLabels;
	TupArray<TupString> m_vectRadioKeys;
	TupArray<TupString> m_vectRadioToolTips;
	TupArray<int> m_vectControlIDs;
	TupArray<HWND> m_vectCtrlHWNDs;
	TupArray<TupString> m_vectKeysToPurge;
	int m_nLabelID;
	HWND m_hLabel;

private:
};

//-----------------------------------------------------------------------------


#endif
