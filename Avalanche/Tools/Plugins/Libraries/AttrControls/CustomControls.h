//
//	CustomControls
//	

/*
 * $History: CustomControls.h $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 2/14/03    Time: 2:15p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * use of new window class prefixes
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 12/19/02   Time: 3:47p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 12/13/02   Time: 12:03p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * added new cursors
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 11/27/02   Time: 10:21a
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * callbacks setup and focus bugs
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 4:04p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * Rollups in
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 9:58a
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * added new controls to numeric type.
 * Fixed color issues with Max
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

#ifndef ATTR_CONTROLS_CUSTOM_CONTROLS_H
#define ATTR_CONTROLS_CUSTOM_CONTROLS_H

#define ID_ATUPDATE_SPINNER	52391
#define ID_ATUPDATE_SLIDER	52392

namespace AttrControls 
{

class CustomControls
{

	static HFONT m_hFixedFont;
	static HFONT m_hFixedFontBold;
	static HFONT m_hFixedFontItalic;
	static HFONT m_hFixedFontBoldItalic;
	static HBRUSH m_hLTGrayBrush;
	static HBRUSH m_hDKGrayBrush;
	static HCURSOR m_hPanCur;
	static HCURSOR m_hSpinCur;
	static HCURSOR m_hDragOK;
	static HCURSOR m_hDragNO;
	static HCURSOR m_hSliderCur;
	static HCURSOR m_hDockCur;
	static HCURSOR m_hFloatCur;
	static HCURSOR m_hCUICur;

	static COLORREF m_colorWindow;
	static COLORREF m_colorWindowText;
	static COLORREF m_colorBtnFace;
	static COLORREF m_colorBtnHighlight;
	static COLORREF m_colorBtnShadow;
	static COLORREF m_colorBtnText;
	static COLORREF m_colorRollupTitleFace;
	static COLORREF m_colorRollupTitleText;
	static COLORREF m_colorRollupTitleHiLight;
	static COLORREF m_colorRollupTitleShadow;

	static HBRUSH m_hBrushWindow;
	static HBRUSH m_hBrushBtnShadow;
	static HBRUSH m_hBrushBtnFace;
	static HBRUSH m_hBrushBtnText;

	static WNDPROC m_editWndProc;

	static int m_hEditSelStart, m_hEditSelEnd;

	static void (CALLBACK *m_pEditSetFocusCallback)(void);
	static void (CALLBACK *m_pEditKillFocusCallback)(void);

	static HINSTANCE m_hInstance;

public:
	enum WindowClassNames
	{
		GRAY_WINDOW_CLASS,
		LTGRAY_WINDOW_CLASS,
		WHITE_WINDOW_CLASS,
		EDIT_DUMMY_WINDOW_CLASS,
		SPINNER_WINDOW_CLASS,
		SLIDER_WINDOW_CLASS,
		ROLLUP_PANEL_WINDOW_CLASS,
		ROLLUP_PANEL_TITLE_WINDOW_CLASS,
		ROLLUP_WINDOW_CLASS,
		EDIT_WINDOW_CLASS,
		TOOLTIP_WINDOW_CLASS,
		NUM_WINDOW_CLASSES,
	};

	enum ExtraColors
	{
		COLOR_ROLLUP_TITLE_FACE = 29,
		COLOR_ROLLUP_TITLE_TEXT = 30,
		COLOR_ROLLUP_TITLE_HILIGHT = 31,
		COLOR_ROLLUP_TITLE_SHADOW = 32,
	};

	static void InitVariables( HINSTANCE hInst );
	static void InitCustomControls( HINSTANCE hInst,const char *pWindowNamePrefix);

	static LRESULT CALLBACK ControlWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
	static LRESULT CALLBACK EditDummyWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
	static LRESULT CALLBACK EditSubWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

	static WNDPROC GetEditWndProc(void);
	static SetEditWndProc(WNDPROC editWndProc);
	static HFONT GetFixedFont(void);
	static HFONT GetFixedFontBold(void);
	static HFONT GetFixedFontItalic(void);
	static HFONT GetFixedFontBoldItalic(void);
	static void SetEditSetFocusCallback(void (CALLBACK *editSetFocusCallback)(void));
	static void SetEditKillFocusCallback(void (CALLBACK *editKillFocusCallback)(void));
	static HCURSOR GetSpinCur(void);
	static HCURSOR GetSliderCur(void);
	static HCURSOR GetPanCur(void);

	static HBRUSH GetColorBrush(int index);
	static COLORREF GetColor(int index);
	static void SetColorBrush(int index,COLORREF color);
	static void SetColor(int index,COLORREF color);

	static HINSTANCE GetHInstance(void);

	static const char *GetWindowClassName(int index);

private:
	static const char *m_windowClassNames[NUM_WINDOW_CLASSES];

};

} // AttrControls
#endif