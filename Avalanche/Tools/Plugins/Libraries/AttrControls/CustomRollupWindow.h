//
//	CustomRollupWindow
//	

/*
 * $History: CustomRollupWindow.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 4:04p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * Rollups in
 */

#ifndef ATTR_CONTROLS_CUSTOM_ROLLUP_WINDOW_H
#define ATTR_CONTROLS_CUSTOM_ROLLUP_WINDOW_H

#ifndef ATTR_CONTROLS_CUSTOM_CONTROL_H
#include "CustomControl.h"
#endif


namespace AttrControls
{

#define LMARGIN			1
#define RMARGIN			1
#define TMARGIN			1
#define BMARGIN			1

#define SCROLL_LINE		32

#define PANEL_SPACING	4

#define VIRTUAL_TMARGIN	4
#define VIRTUAL_BMARGIN	0

class CustomRollupPanel;

class CustomRollupWindow : public CustomControl 
{
public:

	HWND m_hPanelWnd;
	BOOL m_bLBDown;
	int m_omy;
	int m_virtualHeight;
	int m_scrollPos;
	int m_panelWidth;
	TupArray<CustomRollupPanel *> m_panels;

	CustomRollupWindow( HWND hWnd );
	~CustomRollupWindow();
	int AppendRollup( HINSTANCE hInst, TCHAR *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param, DWORD flags);
	int AppendRollup( HINSTANCE hInst, DLGTEMPLATE *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param, DWORD flags);
	int AppendRollupPreCreate(HINSTANCE hInst,DLGPROC dlgProc, TCHAR *title, LPARAM param,DWORD flags);
	int AppendRollupPostCreate(CustomRollupPanel *rp,HINSTANCE hInst,DLGPROC dlgProc, TCHAR *title, LPARAM param,DWORD flags);
	void RecalcLayout(void);
	int ReplaceRollup( int index, HINSTANCE hInst, TCHAR *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param, DWORD flags);
	int ReplaceRollup( int index, HINSTANCE hInst, DLGTEMPLATE *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param, DWORD flags);
	int ReplaceRollupPreCreate(int index, HINSTANCE hInst, DLGPROC dlgProc, TCHAR *title, LPARAM param,DWORD flags);
	int ReplaceRollupPostCreate(CustomRollupPanel *rp,int index, HINSTANCE hInst, DLGPROC dlgProc, TCHAR *title, LPARAM param,DWORD flags);
	void UpdateFocusEdit(void);
	void DeleteRollup( int index, int count );
	void SetPageDlgHeight(int index,int height);
	void Show(void);
	void Hide(void);
	void Show(int index);
	void Hide(int index);
	HWND GetPanelDlg(int index);
	int GetPanelIndex(HWND hWnd);
	void SetPanelTitle(int index,TCHAR *title);
	void PanelOpenClose(int index,int yoff,BOOL scrollTo);
	int CalcVirtualHeight(void);
	void VirtualHeightChange(int index=-1);
	int GetClientHeight(void);
	void CalcThumbSize(void);
	void DrawScroll(void);
	void SetScrollPos(int spos);
	BOOL IsPanelOpen(int index);
	void SetPanelOpen(int index, BOOL isOpen, BOOL ingoreFlags );
	void DlgMouseMessage( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
	void MoveRollupPanelFrom(CustomRollupWindow *from, HWND hPanel, BOOL top);
	int GetScrollHeight(void);
	int GetPanelHeight(int index);
	void UpdateLayout(void);
	void UpdateVirtualHeight(void);
	ICustomRollupPanel *GetPanel(HWND m_hWnd);
	HWND GetPanelWnd(void);
	int GetNumPanels(void);
	int GetScrollPos(void);


	// windows messages
	LRESULT OnSize( int w, int h );
	LRESULT OnPaint(void);
	LRESULT OnLButDown( int x, int y, WORD keys );
	LRESULT OnMButDown( int x, int y, WORD keys );
	LRESULT OnMouseMove( int x, int y, WORD keys );
	LRESULT OnLButUp( int x, int y, WORD keys );
	LRESULT OnMButUp( int x, int y, WORD keys );
	LRESULT OnMouseWheel( int x, int y, int zDelta, WORD keys );
	LRESULT OnVScroll(WORD code, int pos, HWND hwnd);
};

} // AttrControls

#endif



