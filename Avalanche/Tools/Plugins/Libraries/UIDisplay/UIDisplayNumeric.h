//-----------------------------------------------------------------------------
//	File:		UIDisplayNumeric.h
//	Created:	Dec. 5, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The UIDisplayNumeric class contains all the information needed to
//			display the proper ui control for a numeric in max.  It does not
//			handle the actual drawing of the control or the message handling.
//
//	History:	12/5/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __UIDISPLAYNUMERIC__
#define __UIDISPLAYNUMERIC__



//-----------------------------------------------------------------------------
// Includes:

#ifndef __UIDISPLAYBASE__
#include "UIDisplayBase.h"
#endif

#ifndef __CONFIGNUMERIC__
#include "ConfigNumeric.h"
#endif



//-----------------------------------------------------------------------------
// Pre-declarations, etc.:



//-----------------------------------------------------------------------------
// Class Declaration:

class UIDisplayNumeric : public UIDisplayBase
{
public:
	UIDisplayNumeric();
	~UIDisplayNumeric();

	// Accessors:
	ConfigNumeric::NumericType GetType();
	int GetCurrValueInt();
	float GetCurrValueFloat();
	int GetMinInt();
	float GetMinFloat();
	int GetMaxInt();
	float GetMaxFloat();
	int GetTickStep();
	int GetDefaultInt();
	float GetDefaultFloat();
	TupString GetFormat();
	bool HasEdit();
	bool HasSpinner();
	bool HasSlider();
	int GetLabelID();
	int GetEditID();
	int GetSpinnerID();
	int GetSliderID();

	// Mutators:
	void SetType(ConfigNumeric::NumericType newType);
	void SetCurrValue(int nNewValue);
	void SetCurrValue(float fNewValue);
	void SetMin(int nMin);
	void SetMin(float fMin);
	void SetMax(int nMax);
	void SetMax(float fMax);
	void SetTickStep(int nTickStep);
	void SetSpinnerStep(float fSpinnerStep);
	void SetDefault(int nDefault);
	void SetDefault(float fDefault);
	void SetFormat(TupString strNewFormat);
	void SetHasEdit(bool bHasEdit);
	void SetHasSpinner(bool bHasSpinner);
	void SetHasSlider(bool bHasSlider);
	void SetLabelID(int nNewID);
	void SetEditID(int nNewID);
	void SetSpinnerID(int nNewID);
	void SetSliderID(int nNewID);
	void SetShowIndeterminate(bool bShow);

	// Misc. methods:
	bool BuildActualUI(HWND hParentDlg, HINSTANCE hInstance,	HWND hToolTip,
					   int nStartx, int nStarty, int nLabelWidth, int nLabelHeight,
					   int& nControlSizeY);
	bool HandledWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam,
						   TupString& strKeyname, TupString& strValue,
						   TupArray<TupString>& vectKeysToPurge, int& nFlags);
	void RemoveToolTips(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip);

protected:
	ConfigNumeric::NumericType m_type;
	int m_nCurrValue;
	float m_fCurrValue;
	int m_nMin;
	float m_fMin;
	int m_nMax;
	float m_fMax;
	int m_nTickStep;
	float m_fSpinnerStep;
	int m_nDefault;
	float m_fDefault;
	TupString m_strFormat;
	bool m_bHasEdit;
	bool m_bHasSpinner;
	bool m_bHasSlider;
	int m_nLabelID;
	int	m_nEditID;
	int	m_nSpinnerID;
	int	m_nSliderID;
	HWND m_hLabel;
	HWND m_hEditCtrl;
	HWND m_hSpinnerCtrl;
	HWND m_hSliderCtrl;
	HWND m_hChildEditCtrl;
	bool m_bShowIndeterminate;

private:
};

//-----------------------------------------------------------------------------


#endif
