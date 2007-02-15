//-----------------------------------------------------------------------------
//	File:		UIDisplayBase.h
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplayBase class as a base class to other display classes.
//			These classes will contain all the information needed to
//			display the proper ui controls for attribute data.  It does not
//			handle the actual drawing of the controls or the message handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __UIDISPLAYBASE__
#define __UIDISPLAYBASE__



//-----------------------------------------------------------------------------
// Includes:

#ifndef __CURRATTRIBBASE__
#include "CurrAttribBase.h"
#endif

//-----------------------------------------------------------------------------
// Defines, etc.:

#define INDENT_SCALER	20
#define IDC_ALL_LABELS	7000


//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

//-----------------------------------------------------------------------------
// Class Declaration:

class UIDisplayBase
{
public:
	UIDisplayBase();
	virtual ~UIDisplayBase();

	// Accessors:
	const TupString &GetLabel() const;
	const TupString &GetKeyname() const;
	const TupString &GetToolTip() const;
	int GetIndentLevel() const;
	int GetAddedIndent() const;
	bool GetExclusive() const;
	bool GetDisabled() const;
	int GetCenterY() const;
	int GetTopY() const;
	int GetBottomY() const;

	// Mutators:
	void SetLabel(const TupString &strLabel);
	void SetKeyname(const TupString &strKeyname);
	void SetToolTip(const TupString &strLabelname,const TupString &strKeyname,const TupString &strToolTip);
	void SetIndentLevel(int nIndentLevel);
	void SetAddedIndent(int nIndent);
	void SetExclusive(bool bExclusive);
	void SetDisabled(bool bDisabled);

	void SetUniqueValues(const UniqueValues &uniqueValues);
	void SetUniqueValuesID(int uniqueValuesID);

	// Misc. methods:
	virtual bool BuildActualUI(HWND hParentDlg, HINSTANCE hInstance, HWND hToolTip,
							   int nStartx, int nStarty, int nLabelWidth, int nLabelHeight,
							   int& nControlSizeY) = 0;

	enum WinMessageFlags
	{
		WIN_MESSAGE_FLAG_SET_KEY_VALUE			= 0x0001,
		WIN_MESSAGE_FLAG_FORM_CHECK_LIST			= 0x0002,
		WIN_MESSAGE_FLAG_CHECK_LIST_CHECKED		= 0x0004,
		WIN_MESSAGE_FLAG_PURGE_KEYS				= 0x0008,
		WIN_MESSAGE_FLAG_EXCLUSIVE					= 0x0010,
		WIN_MESSAGE_FLAG_FIND_DIALOG				= 0x0020,
	};

	virtual bool HandledWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
								   TupString& strKeyname, TupString& strValue,
								   TupArray<TupString>& vectKeysToPurge, int& nFlags) = 0;

	virtual void RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip) = 0;

protected:
	TupString m_strLabel;
	TupString m_strKeyname;
	TupString m_strToolTip;
	int m_nIndentLevel;
	int m_nAddedIndent;
	bool m_bExclusive;
	bool m_bDisabled;
	int m_nCenterY;
	int m_nTopY;
	int m_nBottomY;

	UniqueValues m_uniqueValues;
	int m_uniqueValuesID;
	HWND m_hUniqueValuesButton;
	HICON m_hUniqueValuesIcon;

	void AddToolTip(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip,HWND hControl);
	void RemoveToolTip(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip,HWND hControl);

private:
};

//-----------------------------------------------------------------------------


#endif
