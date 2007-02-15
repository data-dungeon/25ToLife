//
//	CustomRollupPanel
//	

/*
 * $History: CustomRollupPanel.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 4:04p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * Rollups in
 */

#ifndef ATTR_CONTROLS_CUSTOM_ROLLUP_PANEL_H
#define ATTR_CONTROLS_CUSTOM_ROLLUP_PANEL_H

#ifndef ATTR_CONTROLS_ICUSTOM_ROLLUP_PANEL_H
#include "ICustomRollupPanel.h"
#endif

namespace AttrControls
{

class CustomRollupWindow;

class CustomRollupPanel : public ICustomRollupPanel
{
public:

	DLGPROC m_panelDlgProc;
	LPARAM m_userParam;
	BOOL m_bIn;
	BOOL m_bLBDown;
	BOOL m_bOpen;
	CustomRollupWindow *m_pRollupWindow;
	int m_scrollDist;
	int m_flags;
	HINSTANCE m_hInst;
	DWORD_PTR m_resID;

	HWND m_hPanel;
	HWND m_hPanelTitle;
	HWND m_hDlg;

	int m_panelHeight;
	int m_dlgHeight;

	CustomRollupPanel( HINSTANCE hInst, TCHAR *dlgTemplate, HWND hParent, DLGPROC dlgProc, LPARAM param, TCHAR *title, int index, CustomRollupWindow *rw);
	CustomRollupPanel( HINSTANCE hInst, DLGTEMPLATE *dlgTemplate, HWND hParent, DLGPROC dlgProc, LPARAM param, TCHAR *title, int index, CustomRollupWindow *rw);

// ICustomRollupPanel methods
	HINSTANCE GetHInst(void);
	DWORD_PTR GetResID(void);
	BOOL operator==(const ICustomRollupPanel& id);
	HWND GetHWnd(void);
	HWND GetRollupWindowHWND(void);
	HWND GetTitleWnd(void);
	HWND GetPanelWnd(void);
	HWND GetDlgWnd(void);

// CustomRollupPanel methods
	void Show(void);
	void Hide(void);
	void SetPos( int x, int y, int w );
	int GetTop(void);
	int GetBottom(void);
	int GetHeight(void);
	void DrawPanelInset( HDC hdc );
	void ExpandCollapse(void);
	void SetDlgHeight(int height);
	BOOL Open(void);

	BOOL OnTWLButtonDown(HWND hTitle,LPARAM lParam, WPARAM fwKeys);
	BOOL OnTWLButtonUp(HWND hTitle,POINT pt, WPARAM fwKeys);
	BOOL OnTWMouseMove(HWND hTitle,LPARAM lParam, WPARAM fwKeys);
	void SetState( BOOL bOpenState );
	void PaintTitle(HDC hdc);
};

LRESULT CALLBACK RollupPanelWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK RollupPanelTitleWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

} // AttrControls

#endif