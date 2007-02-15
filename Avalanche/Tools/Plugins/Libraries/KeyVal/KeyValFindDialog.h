//-----------------------------------------------------------------------------
//	File:		KeyValFindDialog.h
//	Created:	May 12, 2005
//
//	Author:		Adam Clayton
//
//	Desc:	The KeyValFindDialog class handles finding keys and/or values
//
//-----------------------------------------------------------------------------


#ifndef __KEYVALFINDDIALOG__
#define __KEYVALFINDDIALOG__

class KeyValPanelController;
                            
//-----------------------------------------------------------------------------
// Class Declaration:

class KeyValFindDialog
{
public:
	// Constructors & Destructors:
	KeyValFindDialog(HINSTANCE hInstance, HWND hParentWnd,KeyValPanelController *pKeyValPanelController);
	~KeyValFindDialog(void);

	HINSTANCE m_hInstance;
	HWND m_hParentWnd;
	HWND m_hWnd;
	KeyValPanelController *m_pKeyValPanelController;

	HWND m_hFindButton;
	HWND m_hKeyCombo;
	HWND m_hValueCombo;
	HWND m_hKeyCheck;
	HWND m_hValueCheck;
	HWND m_hMatchCaseCheck;
	HWND m_hMatchWholeWordCheck;
	HWND m_hSearchDefaultsCheck;
	HWND m_hSearchUnusedCheck;
	HWND m_hAppendSelectionCheck;
	HWND m_hSearchCurrentSelectionCheck;
	HWND m_hSearchEntireSceneCheck;

	HWND m_hToolTip;

	TupArray<TupString> m_keyHistory;
	TupArray<TupString> m_valueHistory;

	TupString m_findKey;
	TupString m_findValue;

	bool m_bMatchCase;
	bool m_bMatchWholeWord;

	bool m_bSearchDefaults;
	bool m_bSearchUnused;

	bool m_bAppendSelection;
	bool m_bCurrentSelection;

	bool m_bUseKey;
	bool m_bUseValue;

	void Show(void);
	void DialogDone(void);
	void RefreshControls(void);
	BOOL OnNotify(LPARAM lParam);
	BOOL OnSetCursor(UINT nHitTest,UINT message);

	void DoFind(void);
	void SetFindKey(const char *pKeyName);
	void SetFindValue(const char *pValueName);

};

extern KeyValFindDialog* g_pTheKeyValFindDialog;


#endif
