//-----------------------------------------------------------------------------
//	File:		ApplyColorsPanelController.h
//	Created:	November 17th, 2004
//				Copyright (c) 2002-2004, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Adam Clayton
//
//	Desc:	The ApplyColorsPanelController class controlls all functionality
//			regarding the apply colors panel
//-----------------------------------------------------------------------------

#ifndef __APPLYCOLORSPANELCONTROLLER__
#define __APPLYCOLORSPANELCONTROLLER__

using namespace AttrControls;

//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class ApplyColorsCommunicatorBase;


//-----------------------------------------------------------------------------
// ApplyColorsPanelController Class Declaration:

class ApplyColorsPanelController
{
	// private data members:
	ApplyColorsCommunicatorBase* m_pApplyColorsCommunicatorBase;
	HWND m_hPanel;
	HWND m_hTreeCtrl;
	HWND m_hComboBox;

	HWND m_hWnd;
	HWND m_hApplyButton;
	HWND m_hRemoveColorButton;

	HINSTANCE m_hInstance;

	void BuildTreeView(void);
	void BuildComboBox(void);

public:

	// Constructors and Destructors:
	ApplyColorsPanelController(ApplyColorsCommunicatorBase* pApplyColorsCommBase);
	virtual ~ApplyColorsPanelController();

	// Public methods:
	void SetHWnd(HWND hWnd);
	void OnInitDialog(void);
	BOOL OnNotify(LPARAM lParam);
	void OnSize(HWND hWnd,int cx,int cy);
		
	void InitApplyColorsPanel(ICustomRollupWindow* pCustomRollupWindow, HINSTANCE hInstance, LPARAM lParam);
	void IdCancelMessage();
	void SetApplyButtonState(bool bEnabled);
	void SetRemoveColorButtonState(bool bEnabled);

	void UpdateTreeViewAndCombo(void);

	void HandleApplyButton(void);
	void HandleRemoveColorButton(void);
	void HandleColorSetChange(void);

};


extern ApplyColorsPanelController* g_pTheApplyColorsPanelController;

//-----------------------------------------------------------------------------


#endif
