//
//	ICustomRollupWindow
//	

/*
 * $History: ICustomRollupWindow.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 4:04p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * Rollups in
 */

#ifndef ATTR_CONTROLS_ICUSTOM_ROLLUP_WINDOW_H
#define ATTR_CONTROLS_ICUSTOM_ROLLUP_WINDOW_H

#ifndef ATTR_CONTROLS_ICUSTOM_CONTROL_H
#include "ICustomControl.h"
#endif

namespace AttrControls
{

class ICustomRollupPanel;

class ICustomRollupWindow : public ICustomControl 
{
public:
	// Shows or hides all
	virtual void Show(void)=0;
	virtual void Hide(void)=0;

	// Shows or hides by index
	virtual void Show(int index)=0;
	virtual void Hide(int index)=0;

	virtual HWND GetPanelDlg(int index)=0;
	virtual int GetPanelIndex(HWND hWnd)=0;
	virtual void SetPanelTitle(int index,TCHAR *title)=0;

	// returns index of new panel
	virtual int AppendRollup( HINSTANCE hInst, TCHAR *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param=0,DWORD flags=0)=0;
	virtual int AppendRollup( HINSTANCE hInst, DLGTEMPLATE *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param=0,DWORD flags=0)=0;
	virtual int ReplaceRollup( int index, HINSTANCE hInst, TCHAR *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param=0,DWORD flags=0)=0;
	virtual int ReplaceRollup( int index, HINSTANCE hInst, DLGTEMPLATE *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param=0,DWORD flags=0)=0;
	virtual void DeleteRollup( int index, int count )=0;
	virtual void SetPageDlgHeight(int index,int height)=0;

	// Passing WM_LBUTTONDOWN, WM_MOUSEMOVE, and WM_LBUTTONUP to
	// this function allows scrolling with unused areas in the dialog.
	virtual void DlgMouseMessage( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )=0;

	virtual int GetNumPanels(void)=0;
	virtual BOOL IsPanelOpen(int index) = 0;
	virtual void SetPanelOpen(int index, BOOL isOpen, BOOL ignoreFlags = TRUE) =0;
	virtual int GetScrollPos(void)=0;
	virtual void SetScrollPos(int spos)=0;

	// This methods moves a RollupPanel to another RollupWindow. It either inserts it
	// at the top, or appends it at the end (depending on the top parameter)

	virtual void MoveRollupPanelFrom(ICustomRollupWindow *from, HWND hPanel, BOOL top)=0;
	
	// Returns the Height of a RollupPanel
	virtual int GetPanelHeight(int index)=0;
	
	// Returns the Height of a RollupWindow, that it is longer than the visible area
	virtual int GetScrollHeight(void)=0;
	
	// Used internally
	virtual void UpdateLayout(void)=0;
	
	virtual ICustomRollupPanel *GetPanel(HWND hWnd)=0;

	static ICustomRollupWindow *GetICustomRollupWindow( HWND hCtrl );
	static void ReleaseICustomRollupWindow( ICustomRollupWindow *irw );
};

} // AttrControls

#endif