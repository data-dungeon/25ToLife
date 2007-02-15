//
//	CustomControl
//	

/*
 * $History: CustomControl.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/21/02   Time: 4:07p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * spinner, sliders, interfaces, and implemtations
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/20/02   Time: 4:20p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * working on custom control port
 */

#ifndef ATTR_CONTROLS_CUSTOM_CONTROL_H
#define ATTR_CONTROLS_CUSTOM_CONTROL_H

namespace AttrControls 
{

class CustomControl
{
protected:
	HWND m_hWnd;
public:
	CustomControl(HWND hWnd);
	virtual ~CustomControl() {};

	virtual HWND GetHWnd();
	virtual void Enable(BOOL onOff=TRUE);
	virtual void Disable();
	virtual BOOL IsEnabled();

	virtual LRESULT OnWMNextDlgCtl( WORD wParam);
	virtual LRESULT OnLButDown( int x, int y, WORD keys );
	virtual LRESULT OnMButDown( int x, int y, WORD keys );
	virtual LRESULT OnRButDown( int x, int y, WORD keys );
	virtual LRESULT OnMouseMove( int x, int y, WORD keys );
	virtual LRESULT OnMouseWheel( int x, int y, int delta, WORD keys );
	virtual LRESULT OnVScroll(WORD code,int pos, HWND hwnd);
	virtual LRESULT OnLButUp( int x, int y, WORD keys );
	virtual LRESULT OnMButUp( int x, int y, WORD keys );
	virtual LRESULT OnCaptureReleased(HWND hWnd);
	virtual LRESULT OnCommand( WORD id, WORD notify, HWND hCtrl );
	virtual LRESULT OnSize( int x, int y);
	virtual LRESULT OnPaint(void);
	virtual LRESULT OnDroppedFiles(HDROP hDrop);
	virtual LRESULT OnDrawItem(UINT idCtl, LPDRAWITEMSTRUCT lpdis);
	virtual LRESULT OnTimer( UINT nIDEvent );
	virtual LRESULT OnSetFocus( HWND hWnd );
	virtual LRESULT OnKillFocus( HWND hWnd );
	virtual LRESULT OnActivate( BOOL onOff );
	virtual LRESULT OnWMEnable( BOOL bEnabled );
	virtual LRESULT OnWMNotify(LPNMHDR hdr);
	virtual LRESULT OnCTBRelay( MSG *msg );
	virtual LRESULT OnSpinnerChange(int id, BOOL drag);
	virtual LRESULT OnSpinnerButtonDown(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnSpinnerButtonUp(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnSliderChange(int id, BOOL drag);
	virtual LRESULT OnSliderButtonDown(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnSliderButtonUp(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnWMCustEdit(int id,HWND hEdit);
	virtual LRESULT OnWMMenuSelect(WPARAM wParam,LPARAM lParam);
	virtual LRESULT OnColorEdit(HDC hdc,HWND hCtrl);
	virtual LRESULT OnKeyDown(int key, LPARAM keyData);
	virtual LRESULT OnKeyUp(int key, LPARAM keyData);
	virtual LRESULT OnWMChar(TCHAR ch, LPARAM keyData);
	virtual LRESULT OnDestroy(void);
	virtual LRESULT OnEraseBkgnd( HDC hDC);

	virtual LRESULT CALLBACK WindowProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

};

} // AttrControls
#endif