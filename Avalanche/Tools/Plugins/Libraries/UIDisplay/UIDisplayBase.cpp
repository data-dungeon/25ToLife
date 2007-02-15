//-----------------------------------------------------------------------------
//	File:		UIDisplayBase.cpp
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



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "UIDisplayBase.h"

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// UIDisplayBase::UIDisplayBase
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayBase::UIDisplayBase()
{
	m_strLabel = "";
	m_nIndentLevel = 0;
	m_bExclusive = true;
	m_uniqueValuesID = -1;
	m_hUniqueValuesButton = NULL;
	m_hUniqueValuesIcon = NULL;
}


//-----------------------------------------------------------------------------
// UIDisplayBase::~UIDisplayBase
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
UIDisplayBase::~UIDisplayBase()
{
	if (m_hUniqueValuesButton != NULL)
		DestroyWindow(m_hUniqueValuesButton);
	if (m_hUniqueValuesIcon != NULL)
		DestroyIcon(m_hUniqueValuesIcon);
}


//-----------------------------------------------------------------------------
// UIDisplayBase::SetLabel
// This method will set the label for this UIDisplay object.
//
// Input:
//  TupString strLabel -- Label to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayBase::SetLabel(const TupString &strLabel)
{
	m_strLabel = strLabel;
}


//-----------------------------------------------------------------------------
// UIDisplayBase::SetKeyname
// This method will set the keyname for this UIDisplay object.
//
// Input:
//  TupString strKeyname -- Keyname to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayBase::SetKeyname(const TupString &strKeyname)
{
	m_strKeyname = strKeyname;
}

//-----------------------------------------------------------------------------
// UIDisplayBase::SetToolTip
// This method will set the tooltip for this UIDisplay object.
//
// Input:
//  TupString strToolTip -- ToolTip to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayBase::SetToolTip(const TupString &strLabelname,const TupString &strKeyname,const TupString &strToolTip)
{
	if (strKeyname.GetLength())
	{
		TupString colorString;
		if (m_bExclusive)
		{
			colorString = "<ct=0xff0000>"; // blue
		}
		else
		{
			colorString = "<ct=0x00c000>"; // green
		}
		if (strToolTip.GetLength())
		{
			m_strToolTip.Format("%s<al_c><b>Key:</b> %s (%s)</al_c></ct>\n\n%s",(const char *)colorString,(const char *)strLabelname,(const char *)strKeyname,(const char *)strToolTip);
		}
		else
		{
			m_strToolTip.Format("%s<al_c><b>Key:</b> %s (%s)</al_c></ct>",(const char *)colorString,(const char *)strLabelname,(const char *)strKeyname);
		}
	}
	else
	{
		m_strToolTip = strToolTip;
	}
}

//-----------------------------------------------------------------------------
// UIDisplayBase::SetIndentLevel
// This method will set the indent level for this UIDisplay object.
//
// Input:
//  int nIndentLevel -- The indent level.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayBase::SetIndentLevel(int nIndentLevel)
{
	m_nIndentLevel = nIndentLevel;
}

//-----------------------------------------------------------------------------
// UIDisplayBase::SetAddedIndent
// This method will set the indent level for this UIDisplay object.
//
// Input:
//  int nIndentLevel -- The indent level.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayBase::SetAddedIndent(int nIndent)
{
	m_nAddedIndent = nIndent;
}

//-----------------------------------------------------------------------------
// UIDisplayBase::SetExclusive
// This method will set the exclusive flag for this UIDisplay object.
//
// Input:
//  bool bExclusive -- Pass in true for exclusive, false for nonexclusive.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayBase::SetExclusive(bool bExclusive)
{
	m_bExclusive = bExclusive;
}


//-----------------------------------------------------------------------------
// UIDisplayBase::SetDisabled
// This method will set the disabled flag for this UIDisplay object.
//
// Input:
//  bool bDisabled -- Pass in true for disabled, false for not disabled.
//
// Output: None.
//-----------------------------------------------------------------------------
void UIDisplayBase::SetDisabled(bool bDisabled)
{
	m_bDisabled = bDisabled;
}

//-----------------------------------------------------------------------------
// UIDisplayBase::GetLabel
// This method will return the label for this UIDisplay object.
//
// Input: None.
//
// Output: TupString representing the label.
//-----------------------------------------------------------------------------
const TupString &UIDisplayBase::GetLabel() const
{
	return m_strLabel;
}


//-----------------------------------------------------------------------------
// UIDisplayBase::GetKeyname
// This method will return the keyname for this UIDisplay object.
//
// Input: None.
//
// Output: TupString representing the label.
//-----------------------------------------------------------------------------
const TupString &UIDisplayBase::GetKeyname() const
{
	return m_strKeyname;
}

//-----------------------------------------------------------------------------
// UIDisplayBase::GetToolTip
// This method will return the tooltip for this UIDisplay object.
//
// Input: None.
//
// Output: TupString representing the label.
//-----------------------------------------------------------------------------
const TupString &UIDisplayBase::GetToolTip() const
{
	return m_strToolTip;
}

//-----------------------------------------------------------------------------
// UIDisplayBase::GetIndentLevel
// This method will return the indent level for this UIDisplay object.
//
// Input: None.
//
// Output: int representing the indent level.
//-----------------------------------------------------------------------------
int UIDisplayBase::GetIndentLevel() const
{
	return m_nIndentLevel;
}

//-----------------------------------------------------------------------------
// UIDisplayBase::GetIndentLevel
// This method will return the indent level for this UIDisplay object.
//
// Input: None.
//
// Output: int representing the indent level.
//-----------------------------------------------------------------------------
int UIDisplayBase::GetAddedIndent() const
{
	return m_nAddedIndent;
}

//-----------------------------------------------------------------------------
// UIDisplayBase::GetExclusive
// This method will return the exclusive flag for this UIDisplay object.
//
// Input: None.
//
// Output: True if this object is exclusive, false otherwise. 
//-----------------------------------------------------------------------------
bool UIDisplayBase::GetExclusive() const
{
	return m_bExclusive;
}


//-----------------------------------------------------------------------------
// UIDisplayBase::GetDisabled
// This method will return the disabled flag for this UIDisplay object.
//
// Input: None.
//
// Output: True if this object is disabled, false otherwise. 
//-----------------------------------------------------------------------------
bool UIDisplayBase::GetDisabled() const
{
	return m_bDisabled;
}

//-----------------------------------------------------------------------------
// UIDisplayBase::GetCenterY
// This method will return the center client coordinate for the control
//
// Input: None.
//
// Output: int representing the value in client coordinates
//-----------------------------------------------------------------------------
int UIDisplayBase::GetCenterY() const
{
	return m_nCenterY;
}

//-----------------------------------------------------------------------------
// UIDisplayBase::GetTopY
// This method will return the top client coordinate for the control
//
// Input: None.
//
// Output: int representing the value in client coordinates
//-----------------------------------------------------------------------------
int UIDisplayBase::GetTopY() const
{
	return m_nTopY;
}

//-----------------------------------------------------------------------------
// UIDisplayBase::GetBottomY
// This method will return the bottom client coordinate for the control
//
// Input: None.
//
// Output: int representing the value in client coordinates
//-----------------------------------------------------------------------------
int UIDisplayBase::GetBottomY() const
{
	return m_nBottomY;
}

//-----------------------------------------------------------------------------
// UIDisplayBase::AddToolTip
// This method will add the window to the tooltip control
//
// Input: HWND hParentWnd -- handle to parent window
//  HWND hToolTip -- handle to tooltip window
//  HINSTANCE hInstance -- handle to application instance
//  HWND hControl -- control to add to the tooltip
//
// Output: None
//-----------------------------------------------------------------------------
void UIDisplayBase::AddToolTip(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip,HWND hControl)
{
	AttrControls::ICustomToolTip *pToolTip = AttrControls::ICustomToolTip::GetIToolTip(hToolTip);
	if (pToolTip)
	{
		pToolTip->AddTool(hControl,0);
		AttrControls::ICustomToolTip::ReleaseIToolTip(pToolTip);
	}
}

//-----------------------------------------------------------------------------
// UIDisplayBase::RemoveToolTip
// This method will add the window to the tooltip control
//
// Input: HWND hParentWnd -- handle to parent window
//  HWND hToolTip -- handle to tooltip window
//  HINSTANCE hInstance -- handle to application instance
//  HWND hControl -- control to add to the tooltip
//
// Output: None
//-----------------------------------------------------------------------------
void UIDisplayBase::RemoveToolTip(HWND hParentWnd,HINSTANCE hInstance,HWND hToolTip,HWND hControl)
{
	AttrControls::ICustomToolTip *pToolTip = AttrControls::ICustomToolTip::GetIToolTip(hToolTip);
	if (pToolTip)
	{
		int index = pToolTip->FindTool(hControl);
		if (pToolTip->IsExistTool(index))
		{
			pToolTip->RemoveTool(index);
		}
		AttrControls::ICustomToolTip::ReleaseIToolTip(pToolTip);
	}
}

//-----------------------------------------------------------------------------
// UIDisplayBase::SetUniqueValues
// This method will set the unique values
//
// Input: UniqueValues &uniqueValues -- source data
//
// Output: None
//-----------------------------------------------------------------------------
void UIDisplayBase::SetUniqueValues(const UniqueValues &uniqueValues)
{
	m_uniqueValues = uniqueValues; // copy the data
}

//-----------------------------------------------------------------------------
// UIDisplayBase::SetUniqueValuesID
// This method will set the unique values id for the button
//
// Input: int uniqueValuesID -- id for the unique values button
//
// Output: None
//-----------------------------------------------------------------------------
void UIDisplayBase::SetUniqueValuesID(int uniqueValuesID)
{
	m_uniqueValuesID = uniqueValuesID;
}


