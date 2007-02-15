//
//	ICustomToolTip
//	

#ifndef ATTR_CONTROLS_ICUSTOM_TOOLTIP_H
#define ATTR_CONTROLS_ICUSTOM_TOOLTIP_H

#ifndef ATTR_CONTROLS_ICUSTOM_CONTROL_H
#include "ICustomControl.h"
#endif

#define TOOLTIP_USE_SHADE

namespace AttrControls 
{

class ICustomToolTip : public ICustomControl
{
public:

	enum Messages
	{
		UDM_TOOLTIP_DISPLAY				= WM_USER + 100,
	};

	enum Styles
	{
		TOOLTIP_ANCHOR					= 0x00000001,
		TOOLTIP_SHADOW					= 0x00000002,
		TOOLTIP_ROUNDED					= 0x00000004,
		TOOLTIP_BALLOON					= 0x00000007,
		TOOLTIP_VCENTER_ALIGN			= 0x00000008,
		TOOLTIP_BOTTOM_ALIGN			= 0x00000010,
		TOOLTIP_ICON_VCENTER_ALIGN	= 0x00000020,
		TOOLTIP_ICON_BOTTOM_ALIGN	= 0x00000040,
		TOOLTIP_SHOW_INACTIVE			= 0x00000100, //Show tooltip for inacive window
		TOOLTIP_SHOW_DISABLED			= 0x00000200, //Show tooltip for disabled control
	};

	enum Behaviors
	{
		TOOLTIP_MULTIPLE_SHOW			= 0x00000001, //Multiple show for single control
		TOOLTIP_TRACKING_MOUSE		= 0x00000002, //Tracking for mouse
		TOOLTIP_CLOSE_LEAVEWND		= 0x00000004, //Close tooltip if mouse leave the control
		TOOLTIP_NOCLOSE_OVER			= 0x00000008, //No close tooltip if mouse over him
	};

	enum Masks
	{
		TOOLTIP_MASK_STYLES			= 0x0001,	// The styles for the tooltip gets from the structures
		TOOLTIP_MASK_EFFECT			= 0x0002,	// The background's type for the tooltip gets from the structures
		TOOLTIP_MASK_COLORS			= 0x0004,	// The background's colors for the tooltip gets from the structures
		TOOLTIP_MASK_DIRECTION		= 0x0008,  // The align for the tooltip gets from the structures
		TOOLTIP_MASK_BEHAVIOUR		= 0x0010,  // The behaviour for the tooltip gets from the structures
	};

	enum Res
	{
		TYPE_RES_ICON = 0,
		TYPE_RES_BITMAP,
		TYPE_RES_MASK_BITMAP,

		MAX_TYPES_RES
	};

	enum Tools
	{
		TOOLTIP_TOOL_NOEXIST = -1, //the current tool is not exist
		TOOLTIP_TOOL_HELPER = -2   //the current tool is a helper
	};

	enum Directions
	{
		TOOLTIP_LEFT_TOP = 0,
		TOOLTIP_RIGHT_TOP,
		TOOLTIP_LEFT_BOTTOM,
		TOOLTIP_RIGHT_BOTTOM,

		TOOLTIP_MAX_DIRECTIONS
	};

	enum Effects
	{
		TOOLTIP_EFFECT_SOLID = 0,
		TOOLTIP_EFFECT_HGRADIENT,
		TOOLTIP_EFFECT_VGRADIENT,
		TOOLTIP_EFFECT_HCGRADIENT,
		TOOLTIP_EFFECT_VCGRADIENT,
		TOOLTIP_EFFECT_3HGRADIENT,
		TOOLTIP_EFFECT_3VGRADIENT,

#ifdef TOOLTIP_USE_SHADE
		TOOLTIP_EFFECT_NOISE,
		TOOLTIP_EFFECT_DIAGSHADE,
		TOOLTIP_EFFECT_HSHADE,
		TOOLTIP_EFFECT_VSHADE,
		TOOLTIP_EFFECT_HBUMP,
		TOOLTIP_EFFECT_VBUMP,
		TOOLTIP_EFFECT_SOFTBUMP,
		TOOLTIP_EFFECT_HARDBUMP,
		TOOLTIP_EFFECT_METAL,
#endif
		TOOLTIP_MAX_EFFECTS
	};

	struct ToolTipInfo
	{
		HWND m_hWnd;			// Window handle of the control
		UINT m_nIDTool;		// ID of tool   
		RECT m_rectBounds;		// Bounding rect for toolinfo to be displayed
		HICON m_hIcon;			// The icon of the tooltip
		TupString m_sTooltip;		// The string of the tooltip
		UINT m_nMask;			// The mask 
		UINT m_nStyles;		// The tooltip's styles
		UINT m_nDirection;		// Direction display the tooltip relate cursor point
		UINT m_nEffect;		// The color's type or effects
		UINT m_nBehaviour;		// The tooltip's behaviour
		BYTE m_nGranularity;	// The effect's granularity
		COLORREF m_crBegin;		// Begin Color
		COLORREF m_crMid;			// Mid Color
		COLORREF	m_crEnd;			// End Color
	};

	struct ToolTipNameRes
	{
		TupString m_sName;	// Name of the resource (IDB_ - Bitmap, IDI_ - Icon)
		UINT m_nID;		// ID of the resource 
		BYTE m_nTypeRes;	// TYPE_RES_ICON = Icon, TYPE_RES_BITMAP = bitmap, TYPE_RES_TRAN_BITMAP = transparent bitmap
		COLORREF m_crMask;// Color mask of the resource 
	};

	// This structure sent to PPToolTip parent in a WM_NOTIFY message
	struct ToolTipDisplay
	{
		NMHDR hdr;
		POINT *pt;
		ToolTipInfo * ti;
	};

	virtual void SetStyles(DWORD nStyles, int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST)=0; //Sets New Style
	virtual void ModifyStyles(DWORD nAddStyles, DWORD nRemoveStyles, int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST)=0; //Modifies styles
	virtual DWORD GetStyles(int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST)=0; //Gets current Styles
	virtual void SetDefaultStyles(int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST)=0; //Sets default styles

	virtual void Activate(BOOL bActive)=0;

	//functions for color
	virtual void SetColor(int nIndex, COLORREF crColor)=0; //Sets the color
	virtual COLORREF GetColor(int nIndex)=0; //Gets the color
	virtual void SetDefaultColors()=0; //Sets default colors
	virtual void SetGradientColors(COLORREF crBegin, COLORREF crMid, COLORREF crEnd, int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST)=0; //Sets the gradient's colors
	virtual void GetGradientColors(COLORREF & crBegin, COLORREF & crMid, COLORREF & crEnd, int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST)=0; //Gets the gradient's colors

	//functions for tool
	virtual void AddTool(HWND hWnd, UINT nIdText, HICON hIcon = NULL, LPCRECT lpRectTool = NULL, UINT nIDTool = 0)=0; //Adds tool
	virtual void AddTool(HWND hWnd, UINT nIdText, UINT nIdIcon, LPCRECT lpRectTool = NULL, UINT nIDTool = 0)=0; //Adds tool
	virtual void AddTool(HWND hWnd, TupString sTooltipText, HICON hIcon = NULL, LPCRECT lpRectTool = NULL, UINT nIDTool = 0)=0; //Adds tool
	virtual void AddTool(HWND hWnd, TupString sTooltipText, UINT nIdIcon, LPCRECT lpRectTool = NULL, UINT nIDTool = 0)=0; //Adds tool
	virtual void AddTool(ToolTipInfo ti)=0; //Adds tool
	virtual int  FindTool(POINT & pt)=0;
	virtual int  FindTool(HWND hWnd, LPCRECT lpRect = NULL)=0;
	virtual int  FindTool(UINT nIDTool)=0;
	virtual BOOL GetTool(int nIndexTool, ICustomToolTip::ToolTipInfo & ti)=0;
	virtual BOOL RemoveTool(int nIndexTool)=0;  //Removes specified tool
	virtual void RemoveAllTools()=0; // Removes all tools
	virtual BOOL IsExistTool(int nIndexTool)=0;
	virtual void SetAtTool(int nIndexTool, ICustomToolTip::ToolTipInfo & ti)=0;

	//Functions for resources name
	virtual void AddNameOfResource(TupString sName, UINT nID, BYTE nTypeRes = ICustomToolTip::TYPE_RES_MASK_BITMAP, COLORREF crMask = RGB(255, 0, 255))=0;
	virtual int  FindIdOfResource(TupString sName)=0;
	virtual int  FindNameOfResource(UINT nID)=0;
	virtual void RemoveNameOfResource(int nIndex)=0;
	virtual void RemoveAllNamesOfResource()=0;

	//Functions for images
	virtual void SetImageList(UINT nIdBitmap, int cx, int cy, int nCount, COLORREF crMask = RGB(255, 0, 255))=0;
	virtual void SetImageList(HBITMAP hBitmap, int cx, int cy, int nCount, COLORREF crMask = RGB(255, 0, 255))=0;
	virtual HIMAGELIST GetImageList(SIZE & sz)=0;

	//Functions for mask
	virtual void  SetMaskTool(int nIndexTool, UINT nMask = 0)=0;
	virtual void  ModifyMaskTool(int nIndexTool, UINT nAddMask, UINT nRemoveMask)=0;
	virtual UINT  GetMaskTool(int nIndexTool)=0;

	//functions for effects
	virtual void SetEffectBk(UINT nEffect, BYTE nGranularity = 2, int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST)=0;
	virtual UINT GetEffectBk(int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST)=0;
	virtual UINT GetEffectBk(BYTE & nGranularity, int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST)=0;

	//functions for
	virtual void  SetNotify(HWND hWnd)=0;
	virtual void  SetNotify(BOOL bParentNotify = TRUE)=0;
	virtual BOOL  GetNotify()=0; //Is enabled notification

	//functions for delay
	virtual void SetDelayTime(DWORD dwDuration, UINT nTime)=0;
	virtual UINT GetDelayTime(DWORD dwDuration) const =0;

	//functions for sizes
	virtual void SetSize(int nSizeIndex, UINT nValue)=0;
	virtual UINT GetSize(int nSizeIndex)=0;
	virtual void SetDefaultSizes(BOOL bBalloonSize = TRUE)=0;

	//functions for direction
	virtual void SetDirection(UINT nDirection = ICustomToolTip::TOOLTIP_RIGHT_BOTTOM, int nIndex = ICustomToolTip::TOOLTIP_TOOL_NOEXIST)=0;
	virtual UINT GetDirection(int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST)=0;

	// 
	virtual void SetBehaviour(UINT nBehaviour = 0, int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST)=0;
	virtual UINT GetBehaviour(int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST)=0;

	//Functions for the fonts
	virtual BOOL  SetFont(LPLOGFONT lf)=0; //set font
	virtual BOOL  SetFont(LPCTSTR lpszFaceName, int nSizePoints = 8,	BOOL bUnderline = FALSE, BOOL bBold = FALSE,	BOOL bStrikeOut = FALSE, BOOL bItalic = FALSE)=0; //set font
	virtual void  SetDefaultFont()=0; //set default fonts
	virtual void  GetFont(LPLOGFONT lf)=0;

	virtual void RelayEvent(MSG* pMsg)=0;

	//The help tooltip functions
	virtual void ShowHelpTooltip(POINT & pt, UINT nIdText, HICON hIcon = NULL)=0; //show tooltip as helper
	virtual void ShowHelpTooltip(POINT & pt, UINT nIdText, UINT nIdIcon)=0; //show tooltip as helper
	virtual void ShowHelpTooltip(POINT & pt, TupString sTooltipText, HICON hIcon = NULL)=0; //show tooltip as helper
	virtual void ShowHelpTooltip(POINT & pt, TupString sTooltipText, UINT nIdIcon)=0; //show tooltip as helper
	virtual void ShowHelpTooltip(POINT & pt, ICustomToolTip::ToolTipInfo & ti)=0; //show tooltip as helper

	virtual void SetParentWnd(HWND hParentWnd) = 0;


	static ICustomToolTip *GetIToolTip( HWND hCtrl );
	static void ReleaseIToolTip( ICustomToolTip *pControl );

};

} // AttrControls

#endif