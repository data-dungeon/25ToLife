//-----------------------------------------------------------------------------
//	File:		UIDisplayEdit.h
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplayEdit class contains all the information needed to
//			display the proper ui control for an editbox in max.  It does not
//			handle the actual drawing of the control or the message handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __UIDISPLAYEDIT__
#define __UIDISPLAYEDIT__



//-----------------------------------------------------------------------------
// Includes:

#ifndef __UIDISPLAYBASE__
#include "UIDisplayBase.h"
#endif

#ifndef __CONFIGEDITBOX__
#include "ConfigEditBox.h"
#endif


//-----------------------------------------------------------------------------
// Pre-declarations, etc.:



//-----------------------------------------------------------------------------
// Class Declaration:

class UIDisplayEdit : public UIDisplayBase
{
public:
	UIDisplayEdit();
	~UIDisplayEdit();

	// Accessors:
	TupString GetCurrText();
	ConfigEditBox::EditBoxType GetType();	
	int GetLabelID();
	int GetControlID();

	// Mutators:
	void SetCurrText(TupString strCurrText);
	void SetType(ConfigEditBox::EditBoxType newType);
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
	TupString m_strCurrText;
	ConfigEditBox::EditBoxType m_type;
	int m_nLabelID;
	int m_nControlID;
	HWND m_hLabel;
	HWND m_hCustEditCtrl;
	HWND m_hChildEditCtrl;

private:
};

//-----------------------------------------------------------------------------


#endif
