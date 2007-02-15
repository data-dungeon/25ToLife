//-----------------------------------------------------------------------------
//	File:		UIDisplayLabel.h
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplayLabel class contains all the information needed to
//			display the proper ui control for an editbox in max.  It does not
//			handle the actual drawing of the control or the message handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __UIDISPLAYLABEL__
#define __UIDISPLAYLABEL__



//-----------------------------------------------------------------------------
// Includes:

#ifndef __UIDISPLAYBASE__
#include "UIDisplayBase.h"
#endif

#ifndef __CONFIGLABEL__
#include "ConfigLabel.h"
#endif


//-----------------------------------------------------------------------------
// Pre-declarations, etc.:



//-----------------------------------------------------------------------------
// Class Declaration:

class UIDisplayLabel : public UIDisplayBase
{
public:
	UIDisplayLabel();
	~UIDisplayLabel();

	// Mutators:
	void AddLabelString(const TupString &newLabel);
	void AddLabelID(int nNewLabelID);
	void SetLabelToolTip(const TupString &strToolTip);

	// Misc. methods:
	bool BuildActualUI(HWND hParentDlg, HINSTANCE hInstance,	HWND hToolTip,
		int nStartx, int nStarty, int nLabelWidth, int nLabelHeight,
		int& nControlSizeY);
	bool HandledWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
		TupString& strKeyname, TupString& strValue,
		TupArray<TupString>& vectKeysToPurge, int& nFlags);
	void RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip);

protected:
	TupArray<int> m_nLabelIDs;
	TupArray<TupString> m_labelStrings;
	TupArray<HWND> m_hLabels;

private:
};

//-----------------------------------------------------------------------------


#endif
