// This is part of the Professional User Interface Suite library.
// Copyright (C) 2001-2004 FOSS Software, Inc.
// All rights reserved.
//
// http://www.prof-uis.com
// http://www.fossware.com
// mailto:support@fossware.com
//
// This source code can be used, modified and redistributed
// under the terms of the license agreement that is included
// in the Professional User Interface Suite package.
//
// Warranties and Disclaimers:
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND
// INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
// IN NO EVENT WILL FOSS SOFTWARE INC. BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES,
// INCLUDING DAMAGES FOR LOSS OF PROFITS, LOSS OR INACCURACY OF DATA,
// INCURRED BY ANY PERSON FROM SUCH PERSON'S USAGE OF THIS SOFTWARE
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

#if (! defined __EXT_TAB_FLAT_WND_H)
#define __EXT_TAB_FLAT_WND_H

#if (!defined __EXT_MFC_NO_TABFLAT_CTRL)

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_HOOK_H)
	#include "../Src/ExtHook.h"
#endif

#if (!defined __EXT_MOUSECAPTURESINK_H)
	#include <../Src/ExtMouseCaptureSink.h>
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtTabFlatWnd.h : header file
//

#ifndef __AFXTEMPL_H__
	#include <AfxTempl.h>
#endif

#if (!defined __EXT_CMD_ICON_H)
	#include <ExtCmdIcon.h>
#endif

#if (!defined __EXT_TABWND_H)
	#include <ExtTabWnd.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtTabFlatWnd window

#define __ETWS_FT_NO_ITEMS_INCLINE_BEFORE		0x00020000L
#define __ETWS_FT_NO_ITEMS_INCLINE_AFTER		0x00040000L

class __PROF_UIS_API CExtTabFlatWnd
	: public CExtTabWnd
{
// Construction
public:
	CExtTabFlatWnd();

// Attributes
public:

	bool ItemsHasInclineGet( bool bBefore ) const
	{
		ASSERT_VALID( this );
		DWORD dwTabWndStyle = GetTabWndStyle();
		DWORD dwTestMask = bBefore
			? __ETWS_FT_NO_ITEMS_INCLINE_BEFORE
			: __ETWS_FT_NO_ITEMS_INCLINE_AFTER
			;

		bool bRetVal = ( (dwTabWndStyle&dwTestMask) != 0 ) ? false : true;
		return bRetVal;
	}
	bool ItemsHasInclineBeforeGet() const
	{
		ASSERT_VALID( this );
		return ItemsHasInclineGet( true );
	}
	bool ItemsHasInclineAfterGet() const
	{
		ASSERT_VALID( this );
		return ItemsHasInclineGet( false );
	}
	void ItemsHasInclineSet( bool bBefore, bool bSet = true )
	{
		ASSERT_VALID( this );
		DWORD dwAddRemoveMask = bBefore
			? __ETWS_FT_NO_ITEMS_INCLINE_BEFORE
			: __ETWS_FT_NO_ITEMS_INCLINE_AFTER
			;
		ModifyTabWndStyle(
			bSet ? dwAddRemoveMask : 0,
			bSet ? 0 : dwAddRemoveMask,
			(GetSafeHwnd() != NULL) ? true : false
			);
	}
	void ItemsHasInclineBeforeSet( bool bSet = true )
	{
		ASSERT_VALID( this );
		ItemsHasInclineSet( true, bSet );
	}
	void ItemsHasInclineAfterSet( bool bSet = true )
	{
		ASSERT_VALID( this );
		ItemsHasInclineSet( false, bSet );
	}

	// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtTabFlatWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtTabFlatWnd();

	// Generated message map functions
protected:
	virtual int _CalcRgnShift( bool bHorz, const CRect & rc )
	{
		int nShift = ::MulDiv( bHorz ? rc.Height() : rc.Width(), 1, 4);
		return nShift;
	}
	
	void PaintFlatTabItem(
		LONG nItemIndex,
		CDC & dc,
		CRect & rcTabItemsArea,
		bool bTopLeft,
		bool bHorz,
		bool bSelected,
		bool bCenteredText,
		bool bGroupedMode,
		bool bInGroupActive,
		bool bInvertedVerticalMode,
		const CRect & rcEntireItem,
		CSize sizeTextMeasured,
		CFont * pFont,
		__EXT_MFC_SAFE_LPCTSTR sText,
		CExtCmdIcon * pIcon,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);
	
	void OnTabWndDrawItem(
		CDC & dc,
		CRect & rcTabItemsArea,
		LONG nItemIndex,
		TAB_ITEM_INFO * pTii,
		bool bTopLeft,
		bool bHorz,
		bool bSelected,
		bool bCenteredText,
		bool bGroupedMode,
		bool bInGroupActive,
		bool bInvertedVerticalMode,
		const CRect & rcEntireItem,
		CSize sizeTextMeasured,
		CFont * pFont,
		__EXT_MFC_SAFE_LPCTSTR sText,
		CExtCmdIcon * pIcon
		);
	
	void OnTabWndMeasureItemAreaMargins(
		CDC & dcMeasure,
		LONG & nSpaceBefore,
		LONG & nSpaceAfter,
		LONG & nSpaceOver
		);

	virtual void OnTabWndUpdateItemMeasure(
		TAB_ITEM_INFO * pTii,
		CDC & dcMeasure,
		CSize & sizePreCalc
		);
	
	virtual void OnTabWndEraseClientArea(
		CDC & dc,
		CRect & rcClient,
		CRect & rcTabItemsArea,
		CRect & rcTabNearBorderArea,
		DWORD dwOrientation,
		bool bGroupedMode
		);
	virtual void OnTabWndDrawEntire(
		CDC & dc,
		CRect & rcClient
		);

	virtual void OnTabWndDrawButton(
		CDC & dc,
		CRect & rcButton,
		LONG nHitTest,
		bool bTopLeft,
		bool bHorz,
		bool bEnabled,
		bool bHover,
		bool bPushed,
		bool bGroupedMode
		);
	
	virtual void PaintTabButton(
		CDC & dc,
		CRect & rcButton,
		LONG nHitTest,
		bool bTopLeft,
		bool bHorz,
		bool bEnabled,
		bool bHover,
		bool bPushed,
		bool bGroupedMode,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		); 

	virtual CSize OnTabWndCalcButtonSize(
		CDC & dcMeasure,
		LONG nTabAreaMetric // vertical max width or horizontal max heights of all tabs
		);
	
	virtual void OnFlatTabWndGetItemColors(
		LONG nItemIndex,
		bool bSelected,
		COLORREF &clrLight,
		COLORREF &clrShadow,
		COLORREF &clrDkShadow, 
		COLORREF &clrTabBk, 
		COLORREF &clrText
	)
	{ 
		ASSERT_VALID( this );
		nItemIndex;
		bSelected;
		clrLight = 
			g_PaintManager->GetColor( COLOR_3DHIGHLIGHT );
		clrShadow =  
			g_PaintManager->GetColor( COLOR_3DSHADOW );
		clrDkShadow = 
			g_PaintManager->GetColor( COLOR_3DDKSHADOW );
		clrTabBk = 
			bSelected ? RGB(255,255,255) : g_PaintManager->GetColor( CExtPaintManager::CLR_3DFACE_OUT );
		clrText = 
			g_PaintManager->GetColor(COLOR_BTNTEXT);
	}

	virtual void OnFlatTabWndGetMarginColors( COLORREF &clrMargin)
	{
		ASSERT_VALID( this );
		clrMargin = 
			g_PaintManager->GetColor( COLOR_3DDKSHADOW );
	}

	virtual void OnFlatTabWndQueryItemInclines(
		LONG nItemIndex,
		bool bSelected,
		bool * p_bItemHasInclineBefore,
		bool * p_bItemHasInclineAfter
		)
	{
		ASSERT_VALID( this );
		nItemIndex;
		bSelected;
		if( p_bItemHasInclineBefore != NULL )
		{
			*p_bItemHasInclineBefore = ItemsHasInclineBeforeGet();
		}
		if( p_bItemHasInclineAfter != NULL )
		{
			*p_bItemHasInclineAfter = ItemsHasInclineAfterGet();
		}
	}
	
	virtual int OnFlatTabWndGetSize( bool bHorz )
	{
		ASSERT_VALID( this );
		int nSize = 0;
		if(bHorz) 
			nSize = 16;//::GetSystemMetrics( SM_CXHSCROLL );
		else
			nSize = 16;//::GetSystemMetrics( SM_CYHSCROLL );

		return nSize;
	}
	
	//{{AFX_MSG(CExtTabFlatWnd)
	//}}AFX_MSG
	afx_msg LRESULT OnSizeParent( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

#define __EXTTAB_FLAT_GAP_X 2
#define __EXTTAB_FLAT_GAP_Y 2

#endif // (!defined __EXT_MFC_NO_TABFLAT_CTRL)

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__EXT_TAB_FLAT_WND_H)
