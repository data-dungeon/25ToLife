//
//	ICustomRollupWindow
//	

/*
 * $History: ICustomRollupWindowImp.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 4:04p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * Rollups in
 */

#ifndef ATTR_CONTROLS_ICUSTOM_ROLLUP_WINDOW_IMP_H
#define ATTR_CONTROLS_ICUSTOM_ROLLUP_WINDOW_IMP_H

#ifndef ATTR_CONTROLS_ICUSTOM_ROLLUP_WINDOW_H
#include "ICustomRollupWindow.h"
#endif

namespace AttrControls
{

class CustomRollupWindow;

class ICustomRollupWindowImp : public ICustomRollupWindow 
{
	CustomRollupWindow *m_pControl;

public:
	ICustomRollupWindowImp(CustomRollupWindow *pControl);

// ICustomControl
	HWND GetHWnd(void);
	void Enable(BOOL onOff=TRUE);
	void Disable(void);
	BOOL IsEnabled(void);

	// Shows or hides all
	void Show(void);
	void Hide(void);

	// Shows or hides by index
	void Show(int index);
	void Hide(int index);

	HWND GetPanelDlg(int index);
	int GetPanelIndex(HWND hWnd);
	void SetPanelTitle(int index,TCHAR *title);

	// returns index of new panel
	int AppendRollup( HINSTANCE hInst, TCHAR *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param=0,DWORD flags=0);
	int AppendRollup( HINSTANCE hInst, DLGTEMPLATE *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param=0,DWORD flags=0);
	int ReplaceRollup( int index, HINSTANCE hInst, TCHAR *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param=0,DWORD flags=0);
	int ReplaceRollup( int index, HINSTANCE hInst, DLGTEMPLATE *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param=0,DWORD flags=0);
	void DeleteRollup( int index, int count );
	void SetPageDlgHeight(int index,int height);

	// Passing WM_LBUTTONDOWN, WM_MOUSEMOVE, and WM_LBUTTONUP to
	// this function allows scrolling with unused areas in the dialog.
	void DlgMouseMessage( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );

	int GetNumPanels(void);
	BOOL IsPanelOpen(int index);
	void SetPanelOpen(int index, BOOL isOpen, BOOL ignoreFlags = TRUE);
	int GetScrollPos(void);
	void SetScrollPos(int spos);

	// This methods moves a RollupPanel to another RollupWindow. It either inserts it
	// at the top, or appends it at the end (depending on the top parameter)

	void MoveRollupPanelFrom(ICustomRollupWindow *from, HWND hPanel, BOOL top);
	
	// Returns the Height of a RollupPanel
	int GetPanelHeight(int index);
	
	// Returns the Height of a RollupWindow, that it is longer than the visible area
	int GetScrollHeight(void);
	
	// Used internally
	void UpdateLayout(void);
	
	ICustomRollupPanel *GetPanel(HWND hWnd);
};

} // AttrControls

#endif
