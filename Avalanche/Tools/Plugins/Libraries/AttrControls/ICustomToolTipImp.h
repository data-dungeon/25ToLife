//
//	ICustomToolTip
//	

#ifndef ATTR_CONTROLS_ICUSTOM_TOOLTIP_IMP_H
#define ATTR_CONTROLS_ICUSTOM_TOOLTIP_IMP_H

#ifndef ATTR_CONTROLS_ICUSTOM_TOOLTIP_H
#include "ICustomToolTip.h"
#endif

namespace AttrControls 
{

class CustomToolTip;

class ICustomToolTipImp : public ICustomToolTip 
{
	CustomToolTip *m_pControl;
public:
	ICustomToolTipImp(CustomToolTip *pControl);

	// ICustomControl
	HWND GetHWnd(void);
	void Enable(BOOL onOff=TRUE);
	void Disable(void);
	BOOL IsEnabled(void);

	// ICustomToolTip
	void SetStyles(DWORD nStyles, int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST); //Sets New Style
	void ModifyStyles(DWORD nAddStyles, DWORD nRemoveStyles, int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST); //Modifies styles
	DWORD GetStyles(int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST); //Gets current Styles
	void SetDefaultStyles(int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST); //Sets default styles

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
	void AddTool(ToolTipInfo ti); //Adds tool
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
	BOOL  SetFont(LPCTSTR lpszFaceName, int nSizePoints = 8,	BOOL bUnderline = FALSE, BOOL bBold = FALSE,	BOOL bStrikeOut = FALSE, BOOL bItalic = FALSE); //set font
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

};



} // AttrControls

#endif
