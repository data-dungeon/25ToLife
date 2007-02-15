
#ifndef ATTR_CONTROLS_CUSTOM_TOOLTIP_H
#define ATTR_CONTROLS_CUSTOM_TOOLTIP_H

#ifndef ATTR_CONTROLS_CUSTOM_CONTROL_H
#include "CustomControl.h"
#endif

#ifndef ATTR_CONTROLS_ICUSTOM_TOOLTIP_H
#include "ICustomToolTip.h"
#endif

#ifdef TOOLTIP_USE_SHADE
#include "CeXDib.h"
#endif

namespace AttrControls 
{

/////////////////////////////////////////////////////////////////////////////
// CustomToolTip window

class CustomToolTip : public CustomControl
{
	// Construction
public:
	CustomToolTip(HWND hWnd);
	virtual ~CustomToolTip();

	// Attributes
public:
	enum Sizes 
	{
		TOOLTIP_SIZE_ROUNDED_CX = 0,
		TOOLTIP_SIZE_ROUNDED_CY,
		TOOLTIP_SIZE_MARGIN_CX,
		TOOLTIP_SIZE_MARGIN_CY,
		TOOLTIP_SIZE_SHADOW_CX,
		TOOLTIP_SIZE_SHADOW_CY,
		TOOLTIP_SIZE_WIDTH_ANCHOR,
		TOOLTIP_SIZE_HEIGHT_ANCHOR,
		TOOLTIP_SIZE_MARGIN_ANCHOR,
		TOOLTIP_SIZE_BORDER_CX,
		TOOLTIP_SIZE_BORDER_CY,

		TOOLTIP_SIZE_MAX_SIZES
	};

	enum Colors
	{
		TOOLTIP_COLOR_0 = 0,
		TOOLTIP_COLOR_1,
		TOOLTIP_COLOR_2,
		TOOLTIP_COLOR_3,
		TOOLTIP_COLOR_4,
		TOOLTIP_COLOR_5,
		TOOLTIP_COLOR_6,
		TOOLTIP_COLOR_7,
		TOOLTIP_COLOR_8,
		TOOLTIP_COLOR_9,
		TOOLTIP_COLOR_10,
		TOOLTIP_COLOR_11,
		TOOLTIP_COLOR_12,
		TOOLTIP_COLOR_13,
		TOOLTIP_COLOR_14,
		TOOLTIP_COLOR_15,
		TOOLTIP_COLOR_16,
		TOOLTIP_COLOR_FG,
		TOOLTIP_COLOR_BK_BEGIN,
		TOOLTIP_COLOR_BK_MID,
		TOOLTIP_COLOR_BK_END,
		TOOLTIP_COLOR_LINK,		// Color for hyperlink text
		TOOLTIP_COLOR_VISITED,	// Color for visited hyperlink text
		TOOLTIP_COLOR_HOVER,		// Color for hover hyperlink text
		TOOLTIP_COLOR_SHADOW,		// Color for the shadow
		TOOLTIP_COLOR_BORDER,		// Color for border of the tooltip

		TOOLTIP_MAX_COLORS
	};


protected:
	enum Timers
	{
		TOOLTIP_SHOW = 0x100, //the identifier of the timer for show the tooltip
		TOOLTIP_HIDE = 0x101  //the identifier of the timer for hide the tooltip
	};

	//	typedef CArray<TOOLTIP_HOTRECT, TOOLTIP_HOTRECT&> m_arrToolsWnd;
	//	CMap <HWND, HWND, m_arrToolsWnd, m_arrToolsWnd&> m_mapTools;
	TupArray<ICustomToolTip::ToolTipInfo> m_arrTools; //Tools Archive
	TupArray<ICustomToolTip::ToolTipNameRes> m_arrNameRes; //Names of the resource

	HWND m_hNotifyWnd; // Handle to window for notification about change data
	HWND m_hParentWnd; // Handle to the parent window
	UINT m_nLastDirection;
	TupArray<UINT> m_nLengthLines; //The real length of the lines in the tooltip
	TupArray<UINT> m_nHeightLines; //The real heigth of the lines in the tooltip

	int m_nIndexCurrentWnd;
	int m_nIndexDisplayWnd;

	BOOL m_bActive; // if active or not

	LOGFONT m_logFont;                  // Current font in use

	//Default setting
	COLORREF m_crColor [TOOLTIP_MAX_COLORS]; //The indexing colors
	UINT m_nSizes [TOOLTIP_SIZE_MAX_SIZES]; //All sizes 
	UINT m_nStyles;
	UINT m_nDirection;
	UINT m_nEffect;
	UINT m_nGranularity;
	UINT m_nBehaviour;	 //The tooltip's behaviour 

	UINT m_nTimeAutoPop; //Retrieve the length of time the tool tip window remains visible if the pointer is stationary within a tool's bounding rectangle
	UINT m_nTimeInitial; //Retrieve the length of time the pointer must remain stationary within a tool's bounding rectangle before the tool tip window appears
	UINT m_nTimeReShow; //Retrieve the length of time the pointer must remain stationary within a tool's bounding rectangle before the tool tip window appears (but last time the tip was up)

	//The properties of the current tooltip
	POINT  m_ptOriginal;

	HRGN	m_rgnToolTip;
	HRGN	m_rgnShadow;

	SIZE m_szToolIcon; //the size of the current icon
	SIZE m_szTextTooltip; //the size of the tooltip's text

	HIMAGELIST m_imgTooltip; //the image list 
	SIZE m_szImage;

	ICustomToolTip::ToolTipInfo m_toolInfo; //info of the current tooltip

#ifdef TOOLTIP_USE_SHADE
	void SetShade(RECT rect, UINT shadeID = 0, BYTE granularity = 8, BYTE coloring = 0, COLORREF color = 0);
	CCeXDib m_dNormal,m_dh,m_dv;
#endif

	// Implementation
public:
	//Functions for the styles
	void  SetStyles(DWORD nStyles, int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST); //Sets New Style
	void  ModifyStyles(DWORD nAddStyles, DWORD nRemoveStyles, int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST); //Modifies styles
	DWORD GetStyles(int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST); //Gets current Styles
	void  SetDefaultStyles(int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST); //Sets default styles

	void Activate(BOOL bActive);

	//functions for color
	void SetColor(int nIndex, COLORREF crColor); //Sets the color
	COLORREF GetColor(int nIndex); //Gets the color
	void SetDefaultColors(); //Sets default colors
	void SetGradientColors(COLORREF crBegin, COLORREF crMid, COLORREF crEnd, int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST); //Sets the gradient's colors
	void GetGradientColors(COLORREF & crBegin, COLORREF & crMid, COLORREF & crEnd, int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST); //Gets the gradient's colors

	//functions for tool
	void AddTool(HWND hWnd, UINT nIdText, HICON hIcon = NULL, LPCRECT lpRectTool = NULL, UINT nIDTool = 0); //Adds tool
	void AddTool(HWND hWnd, UINT nIdText, UINT nIdIcon, LPCRECT lpRectTool = NULL, UINT nIDTool = 0); //Adds tool
	void AddTool(HWND hWnd, TupString sTooltipText, HICON hIcon = NULL, LPCRECT lpRectTool = NULL, UINT nIDTool = 0); //Adds tool
	void AddTool(HWND hWnd, TupString sTooltipText, UINT nIdIcon, LPCRECT lpRectTool = NULL, UINT nIDTool = 0); //Adds tool
	void AddTool(ICustomToolTip::ToolTipInfo ti); //Adds tool
	int  FindTool(POINT & pt);
	int  FindTool(HWND hWnd, LPCRECT lpRect = NULL);
	int  FindTool(UINT nIDTool);
	BOOL GetTool(int nIndexTool, ICustomToolTip::ToolTipInfo & ti);
	BOOL RemoveTool(int nIndexTool);  //Removes specified tool
	void RemoveAllTools(); // Removes all tools
	BOOL IsExistTool(int nIndexTool);
	void SetAtTool(int nIndexTool, ICustomToolTip::ToolTipInfo & ti);

	//Functions for resources name
	void AddNameOfResource(TupString sName, UINT nID, BYTE nTypeRes = ICustomToolTip::TYPE_RES_MASK_BITMAP, COLORREF crMask = RGB(255, 0, 255));
	int  FindIdOfResource(TupString sName);
	int  FindNameOfResource(UINT nID);
	void RemoveNameOfResource(int nIndex);
	void RemoveAllNamesOfResource();

	//Functions for images
	void SetImageList(UINT nIdBitmap, int cx, int cy, int nCount, COLORREF crMask = RGB(255, 0, 255));
	void SetImageList(HBITMAP hBitmap, int cx, int cy, int nCount, COLORREF crMask = RGB(255, 0, 255));
	HIMAGELIST GetImageList(SIZE & sz);

	//Functions for mask
	void  SetMaskTool(int nIndexTool, UINT nMask = 0);
	void  ModifyMaskTool(int nIndexTool, UINT nAddMask, UINT nRemoveMask);
	UINT  GetMaskTool(int nIndexTool);

	//functions for effects
	void SetEffectBk(UINT nEffect, BYTE nGranularity = 2, int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST);
	UINT GetEffectBk(int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST);
	UINT GetEffectBk(BYTE & nGranularity, int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST);

	//functions for
	void  SetNotify(HWND hWnd);
	void  SetNotify(BOOL bParentNotify = TRUE);
	BOOL  GetNotify(); //Is enabled notification

	//functions for delay
	void SetDelayTime(DWORD dwDuration, UINT nTime);
	UINT GetDelayTime(DWORD dwDuration) const;

	//functions for sizes
	void SetSize(int nSizeIndex, UINT nValue);
	UINT GetSize(int nSizeIndex);
	void SetDefaultSizes(BOOL bBalloonSize = TRUE);

	//functions for direction
	void SetDirection(UINT nDirection = ICustomToolTip::TOOLTIP_RIGHT_BOTTOM, int nIndex = ICustomToolTip::TOOLTIP_TOOL_NOEXIST);
	UINT GetDirection(int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST);

	// 
	void SetBehaviour(UINT nBehaviour = 0, int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST);
	UINT GetBehaviour(int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST);

	//Functions for the fonts
	BOOL  SetFont(LPLOGFONT lf); //set font
	BOOL  SetFont(LPCTSTR lpszFaceName, int nSizePoints = 8,
		BOOL bUnderline = FALSE, BOOL bBold = FALSE,
		BOOL bStrikeOut = FALSE, BOOL bItalic = FALSE); //set font
	void  SetDefaultFont(); //set default fonts
	void  GetFont(LPLOGFONT lf);

	void RelayEvent(MSG* pMsg);

	//The help tooltip functions
	void ShowHelpTooltip(POINT & pt, UINT nIdText, HICON hIcon = NULL); //show tooltip as helper
	void ShowHelpTooltip(POINT & pt, UINT nIdText, UINT nIdIcon); //show tooltip as helper
	void ShowHelpTooltip(POINT & pt, TupString sTooltipText, HICON hIcon = NULL); //show tooltip as helper
	void ShowHelpTooltip(POINT & pt, TupString sTooltipText, UINT nIdIcon); //show tooltip as helper
	void ShowHelpTooltip(POINT & pt, ICustomToolTip::ToolTipInfo & ti); //show tooltip as helper

	void SetParentWnd(HWND hParentWnd);

	static short GetVersionI()		{return 0x13;}
	static LPCTSTR GetVersionC()	{return (LPCTSTR)_T("1.3");}
	// Generated message map functions
protected:
	void KillTimers(UINT nIDTimer = NULL);

	SIZE PrintTitleString(HDC hDC, LPRECT rect, TupString str, BOOL bCalculate = TRUE); //Prints the title string
	void  DrawHorzLine(HDC  hDC, int xStart, int xEnd, int y) const;

	void PrepareDisplayToolTip(LPPOINT pt); //Notifying parent, Determines the size of tooltip's rect
	void DisplayToolTip(LPPOINT pt, LPRECT rect);

	void SetNewToolTip(int nIndexTool, BOOL bWithDelay = TRUE);
	void Pop();

	BOOL IsEnabledIndexTool(int nIndex);
	BOOL IsCursorInToolTip() const;
	inline BOOL IsVisible() const { return (((DWORD)GetWindowLong(m_hWnd, GWL_STYLE) & WS_VISIBLE) == WS_VISIBLE); }

	SIZE GetTooltipSize(TupString str); //Gets max rectangle for display tooltip text
	SIZE GetSizeIcon(HICON hIcon) const;
	void CalculateInfoBoxRect(LPPOINT pt, LPRECT rect);

	LPLOGFONT GetSystemToolTipFont() const;

	int   GetNextHorizDirection(int nDirection) const;
	int   GetNextVertDirection(int nDirection) const;
	BOOL  TestHorizDirection(int x, int cx, int w_cx, int nDirection, LPRECT rect) const;
	BOOL  TestVertDirection(int y, int cy, int w_cy, int nDirection, LPRECT rect) const;

	RECT GetWindowRegion(HRGN &rgn, SIZE sz, POINT pt);
	void  FillGradient(HDC hDC, LPRECT rect, COLORREF colorStart, COLORREF colorFinish, BOOL bHorz = TRUE);

	LRESULT SendNotify(LPPOINT pt, ICustomToolTip::ToolTipInfo &ti);

	void  OnRedrawClient(HDC hDC);
	void  OnDraw(HDC hDC, RECT rect);
	void  OnDrawBackground(HDC hDC, LPRECT rect);

	SIZE DrawIconFromImageList(HDC hDC, POINT pt, SIZE sz, HIMAGELIST img, int nIndex = 0, BOOL bCalculate = TRUE);
	SIZE DrawResource(TupString sName, HDC hDC, POINT pt, int nMaxHeight, BOOL bCalculate);
	SIZE DrawBitmap(HDC hDC, POINT pt, int nMaxHeight, UINT nID, BOOL bUseMask, COLORREF crMask, BOOL bCalculate);
	SIZE DrawIcon(HDC hDC, POINT pt, int nMaxHeight, UINT nID, BOOL bCalculate);

	HWND GetWndFromPoint(POINT &pt, BOOL bGetDisabled = TRUE) const;

	HICON GetIconFromResources(UINT nID) const;
	HBITMAP GetBitmapFromResources(UINT nID) const;
	HICON GetIconFromResources(TupString sResName) const;

	// messages
	LRESULT OnPaint();
	LRESULT OnTimer(UINT nIDEvent);
	LRESULT OnDestroy();
	LRESULT OnKillFocus(HWND hNewWnd);
	LRESULT OnEraseBkgnd(HDC hDC);
};

/////////////////////////////////////////////////////////////////////////////

} // namespace AttrControls

#endif
