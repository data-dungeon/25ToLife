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

#include "stdafx.h"

#if (!defined __EXT_MFC_NO_TABFLAT_CTRL)

#if (!defined __AFXPRIV_H__)
	#include <AfxPriv.h>
#endif 

#if _MFC_VER < 0x700
	#include <../src/AfxImpl.h>
#else
	#include <../src/mfc/AfxImpl.h>
#endif

#if (! defined __EXT_TAB_FLAT_WND_H)
	#include <ExtTabFlatWnd.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtTabFlatWnd

CExtTabFlatWnd::CExtTabFlatWnd()
{
}

CExtTabFlatWnd::~CExtTabFlatWnd()
{
}

BEGIN_MESSAGE_MAP(CExtTabFlatWnd, CExtTabWnd)
//{{AFX_MSG_MAP(CExtTabFlatWnd)
//}}AFX_MSG_MAP
ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CExtTabFlatWnd message handlers

void CExtTabFlatWnd::OnTabWndDrawItem(
	CDC & dc,
	CRect & rcTabItemsArea,
	LONG nItemIndex,
	CExtTabWnd::TAB_ITEM_INFO * pTii,
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
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTii );
	pTii;
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pFont != NULL );
	ASSERT( pFont->GetSafeHandle() != NULL );
	nItemIndex;
	ASSERT( !bGroupedMode );
	bGroupedMode;
	
	PaintFlatTabItem(
		nItemIndex,
		dc,
		rcTabItemsArea,
		bTopLeft,
		bHorz,
		bSelected,
		bCenteredText,
		bGroupedMode,
		bInGroupActive,
		bInvertedVerticalMode,
		rcEntireItem,
		sizeTextMeasured,
		pFont,
		sText,
		pIcon,
		this
		);
}

//////////////////////////////////////////////////////////////////////////

void CExtTabFlatWnd::OnTabWndMeasureItemAreaMargins(
	CDC & dcMeasure,
	LONG & nSpaceBefore,
	LONG & nSpaceAfter,
	LONG & nSpaceOver
	)
{
	ASSERT_VALID( this );
	ASSERT( dcMeasure.GetSafeHdc() != NULL );
	dcMeasure;
	CRect rcClient;
	GetClientRect( &rcClient );
	int nShift = _CalcRgnShift( CExtTabWnd::OrientationIsHorizontal(), rcClient );
	if( ItemsHasInclineBeforeGet() && nShift > 0 )
		nSpaceBefore	= nShift-2;
	else 
		nSpaceBefore	= -1;
	nSpaceAfter		= 0; 
	nSpaceOver		= 0;
}

//////////////////////////////////////////////////////////////////////////

void CExtTabFlatWnd::OnTabWndUpdateItemMeasure(
   TAB_ITEM_INFO * pTii,
   CDC & dcMeasure,
   CSize & sizePreCalc
   )
{
	ASSERT_VALID( this );
	ASSERT( dcMeasure.GetSafeHdc() != NULL );
	dcMeasure;
	pTii;
	dcMeasure;
CRect rcClient;
	GetClientRect( &rcClient );
bool bHorz = CExtTabWnd::OrientationIsHorizontal();
int nShift = _CalcRgnShift( bHorz, rcClient );
	
bool bItemHasInclineBefore = false;
bool bItemHasInclineAfter = false;

	OnFlatTabWndQueryItemInclines(
		pTii->GetIndexOf(),
		pTii->SelectedGet(),
		&bItemHasInclineBefore,
		&bItemHasInclineAfter
		);

	if( !bItemHasInclineBefore && !bItemHasInclineAfter )
		nShift = 0;

	LONG nSpaceBefore = 0, nSpaceAfter = 0, nSpaceOver = 0;
	OnTabWndMeasureItemAreaMargins(
		dcMeasure,
		nSpaceBefore,
		nSpaceAfter,
		nSpaceOver
		);

	if( bHorz )
	{
		sizePreCalc.cy = rcClient.Height() - nSpaceOver;
		if( nShift > 0 )
			sizePreCalc.cx += 2*nShift + __EXTTAB_FLAT_GAP_X;
	}
	else
	{
		sizePreCalc.cx = rcClient.Width() - nSpaceOver;
		if( nShift > 0 )
			sizePreCalc.cy += 2*nShift + __EXTTAB_FLAT_GAP_Y;
	}
}

//////////////////////////////////////////////////////////////////////////

void CExtTabFlatWnd::PaintFlatTabItem(
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
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( !bGroupedMode );
	rcTabItemsArea;
	bGroupedMode;
	bInGroupActive;
	pHelperSrc;
	lParam;

	CRect rcItem( rcEntireItem );
	CRect rcBuildPointSrc( rcItem );
	
	POINT arrPoints[4] =
	{
		{rcBuildPointSrc.left, rcBuildPointSrc.top},
		{rcBuildPointSrc.right, rcBuildPointSrc.top},
		{rcBuildPointSrc.right, rcBuildPointSrc.bottom},
		{rcBuildPointSrc.left, rcBuildPointSrc.bottom},
	};
	
	bool bItemHasInclineBefore = false;
	bool bItemHasInclineAfter = false;

	OnFlatTabWndQueryItemInclines(
		nItemIndex,
		bSelected,
		&bItemHasInclineBefore,
		&bItemHasInclineAfter
		);
	
	int nShift = _CalcRgnShift( bHorz, rcItem );

	if( nShift <= 0 )
	{
		bItemHasInclineBefore = false;
		bItemHasInclineAfter = false;
	}

	if( !bItemHasInclineBefore && !bItemHasInclineAfter )
		nShift = 0;
	
	if( bHorz )
	{
		arrPoints[0].x += 1;
		arrPoints[1].x -= 1;
		arrPoints[2].x -= 1;
		arrPoints[3].x += 1;
		
		if( bTopLeft )
		{
			if( bItemHasInclineAfter ){
				arrPoints[1].x -= nShift;
				arrPoints[2].x += nShift;
			}
			if( bItemHasInclineBefore ){
				arrPoints[0].x += nShift+1;
				arrPoints[3].x -= nShift-1;
			}
		}
		else
		{
			if( bItemHasInclineAfter ){
				arrPoints[1].x -= nShift;
				arrPoints[2].x += nShift;
			}
			if( bItemHasInclineBefore ){
				arrPoints[0].x += nShift+1;
				arrPoints[3].x -= nShift-1;
			}

			POINT ptTemp;
			ptTemp.x = arrPoints[0].x;
			arrPoints[0].x = arrPoints[3].x;
			arrPoints[3].x = ptTemp.x;
			ptTemp.x = arrPoints[2].x;
			arrPoints[2].x = arrPoints[1].x;
			arrPoints[1].x = ptTemp.x;
		}
	}
	else
	{
		arrPoints[0].y += 1;
		arrPoints[1].y += 1;
		arrPoints[2].y -= 1;
		arrPoints[3].y -= 1;
		
		if( bTopLeft )
		{
			if( bItemHasInclineAfter ){
				arrPoints[3].y -= nShift;
				arrPoints[2].y += nShift;
			}
			if( bItemHasInclineBefore ){
				arrPoints[1].y -= nShift;
				arrPoints[0].y += nShift;
			}
		}
		else
		{
			arrPoints[0].y += 1;
			arrPoints[1].y += 1;
			
			if( bItemHasInclineAfter ){
				arrPoints[3].y -= nShift;
				arrPoints[2].y += nShift;
			}
			if( bItemHasInclineBefore ){
				arrPoints[1].y -= nShift;
				arrPoints[0].y += nShift;
			}
			else
			{
				arrPoints[0].y -= 1;
				arrPoints[1].y -= 1;
			}
			
			POINT ptTemp;
			ptTemp.x = arrPoints[0].x;
			arrPoints[0].x = arrPoints[1].x;
			arrPoints[1].x = ptTemp.x;
			ptTemp.x = arrPoints[2].x;
			arrPoints[2].x = arrPoints[3].x;
			arrPoints[3].x = ptTemp.x;
		}
	}
	
	COLORREF clrLight;
	COLORREF clrShadow;
	COLORREF clrDkShadow;
	COLORREF clrTabBk;
	COLORREF clrText;
	
	OnFlatTabWndGetItemColors( 
		nItemIndex,
		bSelected,
		clrLight,
		clrShadow,
		clrDkShadow,
		clrTabBk,
		clrText
		);
	
	CBrush brushBk( clrTabBk );
	CRgn rgnBk;
	VERIFY( 
		rgnBk.CreatePolygonRgn( arrPoints, 4, ALTERNATE ) 
		);
	dc.FillRgn( &rgnBk, &brushBk );
	
	CPen penLight(PS_SOLID,1,clrLight);
	CPen penShadow(PS_SOLID,1,clrShadow);
	CPen penDkShadow(PS_SOLID,1,clrDkShadow);
	
	if( bHorz )
	{
		if( bTopLeft )
		{
			CPen * pOldPen = dc.SelectObject( &penDkShadow );
			// darkest left
			dc.MoveTo( arrPoints[0].x, arrPoints[0].y );
			dc.LineTo( arrPoints[3].x, arrPoints[3].y );
			// darkest right
			dc.MoveTo( arrPoints[2].x, arrPoints[2].y );
			dc.LineTo( arrPoints[1].x, arrPoints[1].y );
			// light left
			dc.SelectObject( &penLight );
			dc.MoveTo( arrPoints[0].x+1, arrPoints[0].y );
			dc.LineTo( arrPoints[3].x+1, arrPoints[3].y );
			if(bSelected){
				// dark top 
				dc.SelectObject( &penShadow );
				dc.MoveTo( arrPoints[0].x, arrPoints[0].y );
				dc.LineTo( arrPoints[1].x+1, arrPoints[1].y );
			}else{
				// darkest top
				dc.SelectObject( &penDkShadow );
				dc.MoveTo( arrPoints[0].x, arrPoints[0].y );
				dc.LineTo( arrPoints[1].x+1, arrPoints[1].y );
			}
			// light top
			dc.SelectObject( &penLight );
			dc.MoveTo( arrPoints[0].x+1, arrPoints[0].y+1 );
			dc.LineTo( arrPoints[1].x, arrPoints[1].y+1 );
			
			dc.SelectObject( pOldPen );
		} // if( bTopLeft )
		else
		{
			CPen * pOldPen = dc.SelectObject( &penDkShadow );
			// darkest left
			dc.MoveTo( arrPoints[0] );
			dc.LineTo( arrPoints[3].x, arrPoints[3].y );
			// darkest right
			dc.MoveTo( arrPoints[1].x, arrPoints[1].y );
			dc.LineTo( arrPoints[2].x, arrPoints[2].y );
			// light left
			dc.SelectObject( &penLight );
			dc.MoveTo( arrPoints[0].x+1, arrPoints[0].y );
			dc.LineTo( arrPoints[3].x+1, arrPoints[3].y );
			if(bSelected){
				// dark bottom
				dc.SelectObject( &penShadow );
				dc.MoveTo( arrPoints[2].x, arrPoints[2].y-1 );
				dc.LineTo( !bItemHasInclineBefore ? arrPoints[3].x-1 : arrPoints[3].x-2, arrPoints[3].y-1 );
			}else{
				// darkest bottom
				dc.SelectObject( &penDkShadow );
				dc.MoveTo( arrPoints[2].x, arrPoints[2].y-1 );
				dc.LineTo( arrPoints[3].x-1, arrPoints[3].y-1 );
			}
			dc.SelectObject( pOldPen );
		} // else from if( bTopLeft )
	} // if( bHorz )
	else
	{
		if( bTopLeft )
		{
			CPen * pOldPen = dc.SelectObject( &penDkShadow );
			// dark bottom
			dc.MoveTo( arrPoints[2].x, arrPoints[2].y );
			dc.LineTo( arrPoints[3].x, arrPoints[3].y );
			// dark top
			dc.MoveTo( arrPoints[0].x, arrPoints[0].y );
			dc.LineTo( arrPoints[1].x, arrPoints[1].y );
			if(bSelected){
				// dark left
				dc.SelectObject( &penShadow );
				dc.MoveTo( arrPoints[0].x, arrPoints[0].y );
				dc.LineTo( arrPoints[3].x, arrPoints[3].y+1 );
			}else{
				// darkest left
				dc.SelectObject( &penDkShadow );
				dc.MoveTo( arrPoints[0].x, arrPoints[0].y );
				dc.LineTo( arrPoints[3].x, arrPoints[3].y+1 );
			}
			// light left
			dc.SelectObject( &penLight );
			dc.MoveTo( arrPoints[0].x+1, arrPoints[0].y+1 );
			dc.LineTo( arrPoints[3].x+1, arrPoints[3].y );
			// light top
			dc.SelectObject( &penLight ); 
			dc.MoveTo( arrPoints[0].x+1, arrPoints[0].y+1 );
			dc.LineTo( arrPoints[1].x, arrPoints[1].y+1 );
			dc.SelectObject( pOldPen );
		} // if( bTopLeft )
		else
		{
			CPen * pOldPen = dc.SelectObject( &penDkShadow );
			// dark bottom
			dc.MoveTo( arrPoints[2].x, arrPoints[2].y );
			dc.LineTo( arrPoints[3].x, arrPoints[3].y );
			// dark top
			dc.MoveTo( arrPoints[0].x, arrPoints[0].y );
			dc.LineTo( arrPoints[1].x + !bItemHasInclineBefore ? -1 : 0, arrPoints[1].y );
			if(bSelected){
				// dark right
				dc.SelectObject( &penShadow );
				dc.MoveTo( arrPoints[0].x-1, arrPoints[0].y );
				dc.LineTo( arrPoints[3].x-1, arrPoints[3].y+1 );
			}else{
				// darkest right
				dc.SelectObject( &penDkShadow );
				dc.MoveTo( arrPoints[0].x-1, arrPoints[0].y );
				dc.LineTo( arrPoints[3].x-1, arrPoints[3].y+1 );
			}
			// light top
			dc.SelectObject( &penLight ); 
			dc.MoveTo( arrPoints[0].x-2, bItemHasInclineBefore ? arrPoints[0].y : arrPoints[0].y+1 );
			dc.LineTo( arrPoints[1].x, arrPoints[1].y+1 );
			dc.SelectObject( pOldPen );
		} // else from if( bTopLeft )
	} // else from if( bHorz )

	CSize _sizeIcon( 0, 0 );
	if( pIcon != NULL && (!pIcon->IsEmpty()) )
	{
		_sizeIcon = pIcon->GetSize();
		ASSERT( _sizeIcon.cx > 0 && _sizeIcon.cy > 0 );
	} // if( pIcon != NULL && (!pIcon->IsEmpty()) )
	
	// rcText calculation 
CRect rcText( rcItem );

	if( bHorz )
	{
		if( !bItemHasInclineBefore )
			rcText.left -= nShift;
	}
	else
	{
		if( !bItemHasInclineBefore )
			rcText.top -= nShift;
	}

CRect rcItemForIcon( rcItem );
	if(		pIcon != NULL
		&&	(! pIcon->IsEmpty())
		&&	_sizeIcon.cx > 0
		&&	_sizeIcon.cy > 0
		)
	{
		rcItemForIcon.right = rcItemForIcon.left + _sizeIcon.cx;
		rcItemForIcon.bottom = rcItemForIcon.top + _sizeIcon.cy;

		rcItemForIcon.OffsetRect(
			bHorz ? 0 : ((rcItem.Width() - _sizeIcon.cx) / 2),
			bHorz ? ((rcItem.Height() - _sizeIcon.cy) / 2) : 0
			);

		if( rcItemForIcon.left < (rcItem.left+1) )
			rcItemForIcon.left = (rcItem.left+1);
		if( rcItemForIcon.right < (rcItem.right-1) )
			rcItemForIcon.right = (rcItem.right-1);
		if( rcItemForIcon.top < (rcItem.top+1) )
			rcItemForIcon.top = (rcItem.top+1);
		if( rcItemForIcon.bottom < (rcItem.bottom-1) )
			rcItemForIcon.bottom = (rcItem.bottom-1);

		// correct icon coordinates
		if( bHorz )
		{
			rcItemForIcon.OffsetRect( __EXTTAB_FLAT_GAP_X, 0 );
			if( bItemHasInclineBefore ){
				rcItemForIcon.OffsetRect(nShift,0);
		}
			}
			else
			{
			rcItemForIcon.OffsetRect( 0, __EXTTAB_FLAT_GAP_Y );
			if( bItemHasInclineBefore ){
				rcItemForIcon.OffsetRect(0,nShift);
			}
		}
	}

	CExtSafeString sItemText( (sText == NULL) ? _T("") : sText );

	if( !bHorz )
	{
		int nWidth0 = rcText.Width();
		int nWidth1 = rcItem.Width() + __EXTTAB_MARGIN_ICON2TEXT_X*2;
		if( nWidth1 > nWidth0 )
		{
			if( bInvertedVerticalMode )
			{
				rcText.right -= __EXTTAB_MARGIN_ICON2TEXT_X;
				rcText.left = rcText.right - nWidth1;
			} // if( bInvertedVerticalMode )
			else
			{
				rcText.left += __EXTTAB_MARGIN_ICON2TEXT_X;
				rcText.right = rcText.left + nWidth1;
			} // else from if( bInvertedVerticalMode )
		} // if( nWidth1 > nWidth0 )
	} // if( !bHorz )
	
	CSize _sizeText = rcText.Size();
	
	bool bDrawText = false;
	if(		( bHorz		&& _sizeText.cx >= ( max(16,_sizeIcon.cx) ) )
		||	( (!bHorz)	&& _sizeText.cy >= ( max(16,_sizeIcon.cy) ) )
		)
		bDrawText = true;

	bool bDrawIcon = (pIcon != NULL && (! pIcon->IsEmpty()));
				
	if( bDrawIcon )
	{
		rcItemForIcon.right = rcItemForIcon.left + _sizeIcon.cx;
		rcItemForIcon.bottom = rcItemForIcon.top + _sizeIcon.cy;
		if(		(	bHorz
				&&	rcItemForIcon.Width() >= _sizeIcon.cx
				&&	rcItemForIcon.right < rcItem.right
				)
			||	(	( ! bHorz )
				&&	rcItemForIcon.Height() >= _sizeIcon.cy
				&&	rcItemForIcon.bottom < rcItem.bottom
				)
			)
		{
			CRect rcTmpText( 0, 0, 0, 0 );
			g_PaintManager->PaintIcon(
				dc,
				true,
				sItemText,
				pIcon,
				rcItemForIcon,
				rcTmpText,
				false,
				true,
				false,
				0
				);
		}
	} // if( pIcon != NULL && (! pIcon->IsEmpty()) )

	if( bDrawText )
	{ // if we have sense to paint text on tab item
		ASSERT( pFont != NULL );
		ASSERT( pFont->GetSafeHandle() != NULL );
		COLORREF clrOldText = dc.SetTextColor( clrText );
		INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
		
		CFont * pOldFont = dc.SelectObject( pFont );
		if( !bHorz )
		{
				rcText.bottom -= nShift;
			if( !bDrawIcon ){ 
				rcText.top += nShift+4;
			}else{
				rcText.top =
					rcItemForIcon.top
					+ _sizeIcon.cy
					+ __EXTTAB_MARGIN_ICON2TEXT_Y;
			}
			
				rcText.OffsetRect(
					bInvertedVerticalMode
				?   ((sizeTextMeasured.cy/2) + 3)
				: - ((sizeTextMeasured.cy/2) + 3)
					,
					0
					);
				
				CPoint ptCenter = rcText.CenterPoint();
			
			if( bCenteredText )
			{
				UINT nOldTA = dc.SetTextAlign( TA_CENTER | TA_BASELINE );

				dc.ExtTextOut(
					ptCenter.x,
					ptCenter.y,
					ETO_CLIPPED,
					&rcText,
					sItemText,
					sItemText.GetLength(),
					NULL
					);
				dc.SetTextAlign( nOldTA );
			} // if( bCenteredText )
			else
			{
				UINT nOldTA = dc.SetTextAlign( TA_TOP | TA_BASELINE	);

				if( bInvertedVerticalMode )
					ptCenter.y =
					rcText.bottom - (rcText.Height() - sizeTextMeasured.cx)
					;
				else
					ptCenter.y =
					rcText.top
					;

				dc.ExtTextOut(
					ptCenter.x,
					ptCenter.y,
					ETO_CLIPPED,
					&rcText,
					sItemText,
					sItemText.GetLength(),
					NULL
					);
				dc.SetTextAlign( nOldTA );
			} // else from if( bCenteredText )
		} // if( !bHorz )
		else
		{
			UINT nFormat =
				DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS;
			if( bCenteredText )
				nFormat |= DT_CENTER;
			else
				nFormat |= DT_LEFT;
			
			rcText.right -= nShift;
			if( !bDrawIcon ){ 
				rcText.left += nShift+4;
			}else{
				rcText.left =
					rcItemForIcon.left
					+ _sizeIcon.cx
					+ __EXTTAB_MARGIN_ICON2TEXT_X;
			}
			
			dc.DrawText(
				sItemText,
				sItemText.GetLength(),
				rcText,
				nFormat
				);
		}
		dc.SelectObject( pOldFont );
		
		dc.SetBkMode( nOldBkMode );
		dc.SetTextColor( clrOldText );
	} // if we have sense to paint text on tab item

}

//////////////////////////////////////////////////////////////////////////

void CExtTabFlatWnd::OnTabWndEraseClientArea(
	CDC & dc,
	CRect & rcClient,
	CRect & rcTabItemsArea,
	CRect & rcTabNearBorderArea,
	DWORD dwOrientation,
	bool bGroupedMode
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( !bGroupedMode );
	rcTabItemsArea;
	rcTabNearBorderArea;
	dwOrientation;
	bGroupedMode;

	if( g_PaintManager->IsKindOf( RUNTIME_CLASS( CExtPaintManagerOffice2003 ) ) )
	{
		g_PaintManager->PaintTabClientArea(
		dc,
		rcClient,
		rcTabItemsArea,
		rcTabNearBorderArea,
		dwOrientation,
		bGroupedMode,
		this
		);
	}
	else
	{
		dc.FillSolidRect( &rcClient, g_PaintManager->GetColor( CExtPaintManager::CLR_3DFACE_OUT ) );
	}
	
	bool bHorz = OrientationIsHorizontal();
	bool bTopLeft = OrientationIsTopLeft();
	CRect rcMargin( rcClient );
	
	if( bHorz )
	{
		if( bTopLeft )
		{
			rcMargin.top = rcMargin.bottom - 1;
		} // if( bTopLeft )
		else
		{
			rcMargin.bottom = rcMargin.top + 1;
		} // else from if( bTopLeft )
	} // if( bHorz )
	else
	{
		if( bTopLeft )
		{
			rcMargin.left = rcMargin.right - 1;
		} // if( bTopLeft )
		else
		{
			rcMargin.right = rcMargin.left + 1;
		} // else from if( bTopLeft )
	} // else from if( bHorz )

	COLORREF clrMargin;
	OnFlatTabWndGetMarginColors(clrMargin);

	dc.FillSolidRect( &rcMargin, clrMargin );
}



void CExtTabFlatWnd::OnTabWndDrawEntire(
	CDC & dc,
	CRect & rcClient
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	OnTabWndEraseClientArea(
		dc,
		rcClient,
		m_rcTabItemsArea,
		m_rcTabNearBorderArea,
		OrientationGet(),
		false // bGroupedMode
		);
	
	CSize _sizeTextMeasuredSel( 0, 0 );
	TAB_ITEM_INFO * pSelTii = NULL;
	LONG nSelIdx = -1;
	CRect rcSel( 0, 0, 0, 0 );
	
	LONG nItemCount = ItemGetCount();
	LONG nVisCount = ItemGetVisibleCount();
	bool bHorz = OrientationIsHorizontal();
	bool bTopLeft = OrientationIsTopLeft();
	bool bCenteredText =
		(GetTabWndStyle() & __ETWS_CENTERED_TEXT) ? true : false;
	bool bInvertedVerticalMode =
		(GetTabWndStyle() & __ETWS_INVERT_VERT_FONT) ? true : false;
	
	if( nItemCount > 0 && nVisCount > 0 && m_nIndexVisFirst >= 0 )
	{
		ASSERT( m_nIndexVisFirst <= m_nIndexVisLast );
		ASSERT( 0 <= m_nIndexVisFirst && m_nIndexVisFirst < nItemCount );
		ASSERT( 0 <= m_nIndexVisLast && m_nIndexVisLast < nItemCount );


		CRect rcSetMargins(
			( (!bHorz) && (!bTopLeft) ) ? 1 : 0,
			(   bHorz  && (!bTopLeft) ) ? 1 : 0,
			( (!bHorz) &&   bTopLeft  ) ? 1 : 0,
			(   bHorz  &&   bTopLeft  ) ? 1 : 0
			);
		CRect rcPaintItems( m_rcTabItemsArea );
		rcPaintItems.DeflateRect( rcSetMargins );
		if( bHorz )
			rcPaintItems.left = 0;
		else
			rcPaintItems.top = 0;
		CRgn rgnPaint;
		if( !rgnPaint.CreateRectRgnIndirect(&rcPaintItems) )
		{
			ASSERT( FALSE );
			return;
		}
		dc.SelectClipRgn( &rgnPaint );

		for( LONG nIndex = m_nIndexVisFirst; nIndex <= m_nIndexVisLast; nIndex++ )
		{ // paint items
			TAB_ITEM_INFO * pTii = ItemGet( nIndex );
			ASSERT_VALID( pTii );
			if( !pTii->VisibleGet() )
				continue;

			CRect rcItem = pTii->ItemRectGet();
			rcItem.OffsetRect(
				bHorz
					? (m_rcTabItemsArea.left - m_nScrollPos)
					: 0
				,
				bHorz
					? 0
					: (m_rcTabItemsArea.top - m_nScrollPos)
				);
			if( !dc.RectVisible( &rcItem ) )
				continue;

			CSize _sizeTextMeasured = pTii->GetLastMeasuredTextSize();
			bool bSelected = pTii->SelectedGet();
			CFont & font = _GetTabWndFont( bSelected );
			CFont * pFont = &font;
			CExtSafeString sText = pTii->TextGet();
			CExtCmdIcon * pIcon = pTii->IconGetPtr();

			if( bSelected )
			{
				_sizeTextMeasuredSel = _sizeTextMeasured;
				pSelTii = pTii;
				nSelIdx = nIndex;
				rcSel = rcItem;
				continue;
			} // if( bSelected )
			
			OnTabWndDrawItem(
				dc,
				m_rcTabItemsArea,
				nIndex,
				pTii,
				bTopLeft,
				bHorz,
				bSelected,
				bCenteredText,
				false, // bGroupedMode,
				false, // bInGroupActive,
				bInvertedVerticalMode,
				rcItem,
				_sizeTextMeasured,
				pFont,
				sText,
				pIcon
				);
		} // paint items

		dc.SelectClipRgn( NULL );
	} // if( nItemCount > 0 && nVisCount > 0 && m_nIndexVisFirst >= 0 )


	if( nSelIdx >= 0 )
	{
		CFont & font = _GetTabWndFont( true );
		CFont * pFont = &font;
		CExtSafeString sText = pSelTii->TextGet();
		CExtCmdIcon * pIcon = pSelTii->IconGetPtr();
		ASSERT( pSelTii != NULL && (!rcSel.IsRectEmpty()) );
		CRect rcPaintItems( m_rcTabItemsArea );
		if( bHorz )
			rcPaintItems.left = 0;
		else
			rcPaintItems.top = 0;
		CRgn rgnPaint;
		if( !rgnPaint.CreateRectRgnIndirect(&rcPaintItems) )
		{
			ASSERT( FALSE );
			return;
		}
		dc.SelectClipRgn( &rgnPaint );
		OnTabWndDrawItem(
			dc,
			m_rcTabItemsArea,
			nSelIdx, // nIndex
			pSelTii,
			bTopLeft,
			bHorz,
			true, // bSelected,
			bCenteredText,
			false, // bGroupedMode,
			false, // bInGroupActive,
			bInvertedVerticalMode,
			rcSel, // rcItem,
			_sizeTextMeasuredSel, // _sizeTextMeasured,
			pFont,
			sText,
			pIcon
			);
		dc.SelectClipRgn( NULL );
	} // if( nSelIdx >= 0 )
	

	///////////////////////////////////////////////////////////////////////
	// draw buttons
	///////////////////////////////////////////////////////////////////////
	
CPoint ptCursor( 0, 0 );
	::GetCursorPos( &ptCursor );
	ScreenToClient( &ptCursor );
	if( !m_rcBtnUp.IsRectEmpty()
		&& dc.RectVisible( &m_rcBtnUp )
		)
	{
		bool bEnabled = (GetTabWndStyle() & __ETWS_ENABLED_BTN_UP) ? true : false;
		bool bHover = bEnabled && m_rcBtnUp.PtInRect(ptCursor) ? true : false;
		OnTabWndDrawButton(
			dc,
			m_rcBtnUp,
			__ETWH_BUTTON_LEFTUP,
			bTopLeft,
			bHorz,
			bEnabled,
			bHover,
			m_bPushedUp,
			false // bGroupedMode
			);
	}
	if( !m_rcBtnDown.IsRectEmpty()
		&& dc.RectVisible( &m_rcBtnDown )
		)
	{
		bool bEnabled = (GetTabWndStyle() & __ETWS_ENABLED_BTN_DOWN) ? true : false;
		bool bHover = bEnabled && m_rcBtnDown.PtInRect(ptCursor) ? true : false;
		OnTabWndDrawButton(
			dc,
			m_rcBtnDown,
			__ETWH_BUTTON_RIGHTDOWN,
			bTopLeft,
			bHorz,
			bEnabled,
			bHover,
			m_bPushedDown,
			false // bGroupedMode
			);
	}
	if( !m_rcBtnHelp.IsRectEmpty()
		&& dc.RectVisible( &m_rcBtnHelp )
		)
	{
		bool bEnabled = (GetTabWndStyle() & __ETWS_ENABLED_BTN_HELP) ? true : false;
		bool bHover = bEnabled && m_rcBtnHelp.PtInRect(ptCursor) ? true : false;
		OnTabWndDrawButton(
			dc,
			m_rcBtnHelp,
			__ETWH_BUTTON_HELP,
			bTopLeft,
			bHorz,
			bEnabled,
			bHover,
			m_bPushedHelp,
			false // bGroupedMode
			);
	}

	if( !m_rcBtnClose.IsRectEmpty()
		&& dc.RectVisible( &m_rcBtnClose )
		)
	{
		bool bEnabled = (GetTabWndStyle() & __ETWS_ENABLED_BTN_CLOSE) ? true : false;
		bool bHover = bEnabled && m_rcBtnClose.PtInRect(ptCursor) ? true : false;
		OnTabWndDrawButton(
			dc,
			m_rcBtnClose,
			__ETWH_BUTTON_CLOSE,
			bTopLeft,
			bHorz,
			bEnabled,
			bHover,
			m_bPushedClose,
			false // bGroupedMode
			);
	}

	if( !m_rcBtnTabList.IsRectEmpty()
		&& dc.RectVisible( &m_rcBtnTabList )
		)
	{
		bool bEnabled = (GetTabWndStyle() & __ETWS_ENABLED_BTN_TAB_LIST) ? true : false;
		bool bHover = bEnabled && m_rcBtnTabList.PtInRect(ptCursor) ? true : false;
		OnTabWndDrawButton(
			dc,
			m_rcBtnTabList,
			__ETWH_BUTTON_TAB_LIST,
			bTopLeft,
			bHorz,
			bEnabled,
			bHover,
			m_bPushedTabList,
			false // bGroupedMode
			);
	}
}

//////////////////////////////////////////////////////////////////////////

void CExtTabFlatWnd::OnTabWndDrawButton(
	CDC & dc,
	CRect & rcButton,
	LONG nHitTest,
	bool bTopLeft,
	bool bHorz,
	bool bEnabled,
	bool bHover,
	bool bPushed,
	bool bGroupedMode
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	
	PaintTabButton(
		dc,
		rcButton,
		nHitTest,
		bTopLeft,
		bHorz,
		bEnabled,
		bHover,
		bPushed,
		bGroupedMode,
		this
		);
}

//////////////////////////////////////////////////////////////////////////

void CExtTabFlatWnd::PaintTabButton(
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
	  LPARAM lParam // = 0L
	  )
{

	pHelperSrc;
	lParam;
	COLORREF clrGlyph = 
		g_PaintManager->GetColor(
		bEnabled
		? COLOR_BTNTEXT
		: COLOR_3DSHADOW
		);
	COLORREF clrTL =
		g_PaintManager->GetColor(
		bPushed
		? COLOR_3DDKSHADOW
		: COLOR_3DHILIGHT
		);
	COLORREF clrBR =
		g_PaintManager->GetColor(
		bPushed
		? COLOR_3DHILIGHT
		: COLOR_3DDKSHADOW
		);

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	
	bTopLeft;
	bGroupedMode;
	
	CExtPaintManager::glyph_t * pGlyph = NULL;
	switch( nHitTest )
	{
	case __ETWH_BUTTON_LEFTUP:
		pGlyph =
			CExtPaintManager::g_DockingCaptionGlyphs[
			bHorz
			? (	bEnabled
			? CExtPaintManager::__DCBT_ARROW_LEFT
			: CExtPaintManager::__DCBT_HOLLOW_LEFT
			)
			: (	bEnabled
			? CExtPaintManager::__DCBT_ARROW_UP
			: CExtPaintManager::__DCBT_HOLLOW_UP
			)
			];
		ASSERT( pGlyph != NULL );
		break;
	case __ETWH_BUTTON_RIGHTDOWN:
		pGlyph =
			CExtPaintManager::g_DockingCaptionGlyphs[
			bHorz
			? (	bEnabled
			? CExtPaintManager::__DCBT_ARROW_RIGHT
			: CExtPaintManager::__DCBT_HOLLOW_RIGHT
			)
			: (	bEnabled
			? CExtPaintManager::__DCBT_ARROW_DOWN
			: CExtPaintManager::__DCBT_HOLLOW_DOWN
			)
			];
		ASSERT( pGlyph != NULL );
		break;
	case __ETWH_BUTTON_HELP:
		pGlyph =
			CExtPaintManager::g_DockingCaptionGlyphs[
			CExtPaintManager::__DCBT_WND_CONTEXTHELP
			];
		ASSERT( pGlyph != NULL );
		break;
	case __ETWH_BUTTON_CLOSE:
		pGlyph =
			CExtPaintManager::g_DockingCaptionGlyphs[
			CExtPaintManager::__DCBT_CLOSE
			];
		ASSERT( pGlyph != NULL );
		break;
	case __ETWH_BUTTON_TAB_LIST:
		pGlyph =
			CExtPaintManager::g_DockingCaptionGlyphs[
			CExtPaintManager::__DCBT_TABS_LIST_ARROW
			];
		ASSERT( pGlyph != NULL );
		break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
		break;
#endif // _DEBUG
	} // switch( nHitTest )
	
	ASSERT( pGlyph != NULL );
	//dc.FillSolidRect( &rcButton, RGB( 255, 128, 128 ) );
	
	COLORREF ColorValues[2] =
	{
		RGB(0,0,0),
			clrGlyph
	};
	
	g_PaintManager->PaintGlyphCentered(
		dc,
		rcButton,
		*pGlyph,
		ColorValues
		);
	
	if( bHover || bPushed )
		dc.Draw3dRect( &rcButton, clrTL, clrBR );
}

//////////////////////////////////////////////////////////////////////////

CSize CExtTabFlatWnd::OnTabWndCalcButtonSize(
	 CDC & dcMeasure,
	 LONG nTabAreaMetric // vertical max width or horizontal max heights of all tabs
	 )
{
	CSize _size = CExtTabWnd::OnTabWndCalcButtonSize(
		dcMeasure,
		nTabAreaMetric
		);
	
	CRect rcClient;
	GetClientRect(&rcClient);
	
	bool bHorz = OrientationIsHorizontal();
	bool bTopLeft = OrientationIsTopLeft();
	
	if( bHorz )
	{
		int nWidth = ::MulDiv(_size.cx,8,10);
		if( _size.cx > rcClient.Height() ) 
			_size.cx = nWidth;

		if( _size.cy > rcClient.Height()-1 )
			_size.cy = rcClient.Height()-1;

		if( !bTopLeft && _size.cy > rcClient.Height()-2 )
			_size.cy -= 1;
	} // if( bHorz )
	else
	{
		int nHeight = ::MulDiv(_size.cy,8,10);
		if( _size.cy > rcClient.Width()-1 ) 
			_size.cy = nHeight;
		
		if( _size.cx > rcClient.Width()-1 )
			_size.cx = rcClient.Width()-1;

		if( !bTopLeft && _size.cx > rcClient.Width()-2 )
			_size.cx -= 1;

	} // else from if( bHorz )

	if( _size.cx < 3 )
		_size.cx = 3;
	if( _size.cy < 3 )
		_size.cy = 3;
	
	return _size;
}


//////////////////////////////////////////////////////////////////////////

LRESULT CExtTabFlatWnd::OnSizeParent( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );

	if( !m_bReflectParentSizing )
		return (LRESULT)0;
	
	wParam;
	AFX_SIZEPARENTPARAMS * lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;
	ASSERT( lpLayout != NULL );
	
	CRect rcFrameRest = &lpLayout->rect;
	if( rcFrameRest.left >= rcFrameRest.right
		|| rcFrameRest.top >= rcFrameRest.bottom
		)
	{
		if( lpLayout->hDWP == NULL )
			return 0;
		::SetWindowPos(
			m_hWnd,
			NULL, 0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER
				|SWP_HIDEWINDOW
			);
		return 0;
	}

	OnTabWndSyncVisibility();
	DWORD dwWndStyle = GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		return 0;
		
	m_rcRecalcLayout = rcFrameRest;
	m_bDelayRecalcLayout = true;
	_RecalcLayoutImpl();

	if( OrientationIsHorizontal() )
	{
		int nSize = OnFlatTabWndGetSize(true);
		ASSERT(nSize>0);
		m_rcTabItemsArea.bottom = m_rcTabItemsArea.top + nSize;
	}
	else
	{
		int nSize = OnFlatTabWndGetSize(false);
		ASSERT(nSize>0);
		m_rcTabItemsArea.right = m_rcTabItemsArea.left + nSize;
	}

	m_rcRecalcLayout.SetRect( 0, 0, 0, 0 );
	
	if( m_rcTabItemsArea.IsRectEmpty() )
		return 0;
	
	CSize _sizeNeeded = m_rcTabItemsArea.Size();
	if( _sizeNeeded.cx <= 0 || _sizeNeeded.cy <= 0 )
		return 0;
	
	CRect rcOwnLayout( rcFrameRest );
	
	DWORD dwOrientation = OrientationGet();
	switch( dwOrientation )
	{
	case __ETWS_ORIENT_TOP:
		{
			ASSERT( _sizeNeeded.cy > 0 );
			lpLayout->rect.top += _sizeNeeded.cy;
			rcOwnLayout.bottom = rcOwnLayout.top + _sizeNeeded.cy;
			lpLayout->sizeTotal.cy += _sizeNeeded.cy;
		}
		break;
	case __ETWS_ORIENT_BOTTOM:
		{
			ASSERT( _sizeNeeded.cy > 0 );
			lpLayout->rect.bottom -= _sizeNeeded.cy;
			rcOwnLayout.top = rcOwnLayout.bottom - _sizeNeeded.cy;
			lpLayout->sizeTotal.cy += _sizeNeeded.cy;
		}
		break;
	case __ETWS_ORIENT_LEFT:
		{
			ASSERT( _sizeNeeded.cx > 0 );
			lpLayout->rect.left += _sizeNeeded.cx;
			rcOwnLayout.right = rcOwnLayout.left + _sizeNeeded.cx;
			lpLayout->sizeTotal.cx += _sizeNeeded.cx;
		}
		break;
	case __ETWS_ORIENT_RIGHT:
		{
			ASSERT( _sizeNeeded.cx > 0 );
			lpLayout->rect.right -= _sizeNeeded.cx;
			rcOwnLayout.left = rcOwnLayout.right - _sizeNeeded.cx;
			lpLayout->sizeTotal.cx += _sizeNeeded.cx;
		}
		break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
		break;
#endif // _DEBUG
	} // switch( dwOrientation )
	
	ASSERT( ! rcOwnLayout.IsRectEmpty() );
	if( lpLayout->hDWP != NULL )
	{
		::AfxRepositionWindow(
			lpLayout,
			m_hWnd,
			&rcOwnLayout
			);
		::SetWindowPos(
			m_hWnd,
			NULL, 0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER
			|SWP_FRAMECHANGED
			);
		UpdateTabWnd( true );
	} // if( lpLayout->hDWP != NULL )
	return 0L;
}

#endif // (!defined __EXT_MFC_NO_TABFLAT_CTRL)


