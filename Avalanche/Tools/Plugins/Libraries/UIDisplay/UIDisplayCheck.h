//-----------------------------------------------------------------------------
//	File:		UIDisplayCheck.h
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplayCheck class contains all the information needed to
//			display the proper ui control for a checkbox in max.  It does not
//			handle the actual drawing of the control or the message handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __UIDISPLAYCHECK__
#define __UIDISPLAYCHECK__



//-----------------------------------------------------------------------------
// Includes:

#ifndef __UIDISPLAYBASE__
#include "UIDisplayBase.h"
#endif


//-----------------------------------------------------------------------------
// Pre-declarations, etc.:



//-----------------------------------------------------------------------------
// Class Declaration:

class UIDisplayCheck : public UIDisplayBase
{
public:
	UIDisplayCheck();
	~UIDisplayCheck();

	// Accessors:
	int GetState();
	int GetControlID();

	// Mutators:
	void SetState(int nNewState);
	void SetControlID(int nNewControlID);
	void AddPurgeKey(TupString strKeyToPurge);

	// Misc. methods:
	bool BuildActualUI(HWND hParentDlg, HINSTANCE hInstance,	HWND hToolTip,
					   int nStartx, int nStarty, int nLabelWidth, int nLabelHeight,
					   int& nControlSizeY);
	bool HandledWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
						   TupString& strKeyname, TupString& strValue,
						   TupArray<TupString>& vectKeysToPurge, int& nFlags);
	void RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip);

protected:
	int m_nState;
	int m_nControlID;
	TupArray<TupString> m_vectKeysToPurge;
	HWND m_hCheckBox;

private:
};

//-----------------------------------------------------------------------------


#endif
