//-----------------------------------------------------------------------------
//	File:		AssetManagerTreeCtrl.cpp
//	Created:	Oct 7, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The CAssetManagerTreeCtrl class derives from SECTreeCtrl and adds
//			some extra data and methods needed to operate in the Asset Manager.
//
//	History:	10/7/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "assetmanager.h"
#include "AssetManagerTreeCtrl.h"
#include "AssetManagerDlg.h"
#include "UpdateDuplicatesDialog.h"
#include "AssociatedIcon.h"
//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#undef SEC_TREEBASE
#define SEC_TREEBASE SECListCtrl

#define IMG_NORM_IDX_BLANK	3



//-----------------------------------------------------------------------------
// CAssetManagerTreeCtrl class definition:


//-----------------------------------------------------------------------------
// CAssetManagerTreeCtrl::CAssetManagerTreeCtrl
// This method is the default constructor for this class.
//
// Input:
//	CAssetManagerDlg* pParent -- The parent dialog of this control.
//
// Output: None.
//-----------------------------------------------------------------------------
CAssetManagerTreeCtrl::CAssetManagerTreeCtrl(CDialog *pParent /*=NULL*/)
{
	m_pParent = pParent;
	m_bTellParentOfVScroll = true;
	m_bTellParentOfExpand = true;
	m_bTellParentOfSel = true;
	m_role = LEFT_TREE;
	m_rgbDeleteTextBk = ::GetSysColor(COLOR_WINDOW);
	m_rgbCopySourceTextBk = ::GetSysColor(COLOR_WINDOW);
	m_rgbCopyDestTextBk = ::GetSysColor(COLOR_WINDOW);
	m_rgbFrozenText = ::GetSysColor(COLOR_GRAYTEXT);
	m_pOtherFileTree = NULL;
	m_pIndicatorTree = NULL;
	m_bAllowPaint = true;
}


//-----------------------------------------------------------------------------
// CAssetManagerTreeCtrl::~CAssetManagerTreeCtrl
// This method is the destructor for this dialog class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
CAssetManagerTreeCtrl::~CAssetManagerTreeCtrl()
{
}


//-----------------------------------------------------------------------------
// CAssetManagerTreeCtrl message map:

BEGIN_MESSAGE_MAP(CAssetManagerTreeCtrl, SECTreeCtrl)
	//{{AFX_MSG_MAP(CAssetManagerTreeCtrl)
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//-----------------------------------------------------------------------------
// CAssetManagerTreeCtrl::SetTellParentOfExpand
// This method can be called to set the m_bTellParentOfExpand member which
// dictates whether or not this control notifies it's parent of an expand
// message. 
//
// Input:
//  bool bTellParentOfExpand -- Pass in true to notify the parent of the
//								expand, false otherwise.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerTreeCtrl::SetTellParentOfExpand(bool bTellParentOfExpand)
{
	m_bTellParentOfExpand = bTellParentOfExpand;
}


//-----------------------------------------------------------------------------
// CAssetManagerTreeCtrl::SetUsedAsIndicator
// This mutator can be called to set the m_role member which dictates what this
// tree is being used for.
// tree.
//
// Input:
//  Role newRole -- The new role for the tree.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerTreeCtrl::SetRole(Role newRole)

{
	m_role = newRole;
}

void CAssetManagerTreeCtrl::SetDeleteTextBkColor(COLORREF newColor)
{
	m_rgbDeleteTextBk = newColor;
}

void CAssetManagerTreeCtrl::SetCopySourceTextBkColor(COLORREF newColor)
{
	m_rgbCopySourceTextBk = newColor;
}

void CAssetManagerTreeCtrl::SetCopyDestTextBkColor(COLORREF newColor)
{
	m_rgbCopyDestTextBk = newColor;
}

void CAssetManagerTreeCtrl::SetFrozenTextColor(COLORREF newColor)
{
	m_rgbFrozenText = newColor;
}

void CAssetManagerTreeCtrl::SetAssociatedIconManager(AssociatedIconManager *pAssociatedIconManager)
{
	m_pAssociatedIconManager = pAssociatedIconManager;
}

void CAssetManagerTreeCtrl::SetAllowPaint(bool bAllowPaint)
{
	m_bAllowPaint = bAllowPaint;
}

//-----------------------------------------------------------------------------
// CAssetManagerTreeCtrl::DrawSubItem
// This method is overridden here to provide special functionality for
// drawing the indicator tree control.  This medhod is called whenever the tree
// controls are to be drawn.
//
// Input:
//  LvPaintContext* pPC -- The paint context to draw into.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerTreeCtrl::DrawSubItem(LvPaintContext* pPC)
{
	if (pPC->lvi.iSubItem == 0)
	{
		//Get our owner drawn data
		CString  strText;
		int      iImage;
		CRect    rectText;
		CRect rcIndent;

		BOOL bEditing = (pPC->lvi.iItem == m_nEditItem) && (pPC->iCol == m_nActiveColumn);

		TvPaintContext* pTvPC = (TvPaintContext*)pPC;
		BOOL bSelected = (pTvPC->tvi.state & TVIS_SELECTED) != 0;
		iImage = bSelected ? pTvPC->tvi.iSelectedImage : pTvPC->tvi.iImage;
		BOOL bHasImage = HasImage(&(pTvPC->tvi));
		
		BOOL bBlankImage = FALSE;
		
		if((pPC->lvix.stateEx & LVIS_EX_ASSOCIATED_ICON)==0 && (iImage == IMG_NORM_IDX_BLANK))
			bBlankImage = TRUE;

		strText = pTvPC->tvi.pszText;

		// Fill with background color
		CRect  rectFill;
		CBrush brush(pPC->rgbItemBkgnd);
		rectFill = pPC->rect;

		int xOrigin = pPC->rect.left;

		// Compute text size and position
		if (m_role == INDICATOR_TREE)
		{
			int nNewWidth = pPC->pItem->rcText.right - pPC->pItem->rcText.left;
			pPC->pItem->rcText.left = 54;
			pPC->pItem->rcText.right = pPC->pItem->rcText.left + nNewWidth;
		}
		rectText = pPC->pItem->rcText;
		rectText.left += xOrigin;
		rectText.top += pPC->rect.top;
		rectText.right += xOrigin;
		rectText.bottom += pPC->rect.top;
		
		// If we have multiple columns, next columns clip the text of previous columns
		if ( GetColumnCount() > 1 || pPC->pdc->IsPrinting() )
			rectText.right = pPC->rect.right - SEC_RIGHT_OFFSET;


		// Unless we have an icon, the hierarchy lines end
		// where the label begins.
		rcIndent = pPC->rect;
		rcIndent.right = rectText.left;

		if( pPC->da == DrawEntire && m_pStateImageList)
		{
			//Then draw icons/bitmaps/whatever
			// If we are using a state image then select it!
			int iOurImage = GET_STATE_IMAGE(pTvPC->tvi.state);

			if( iOurImage >= m_pStateImageList->GetImageCount() )
			{
				TRACE( _T("Warning: Image does not exist\n") );
				iOurImage=0;
			}

			if ( iOurImage > 0 )
			{
			
	 			CPoint pt(  xOrigin + pPC->pItem->rcStateIcon.left,
	 					pPC->rect.top  + pPC->pItem->rcStateIcon.top  );

				m_pStateImageList->SetBkColor(pPC->rgbIconBkgnd);
				m_pStateImageList->Draw( pPC->pdc, iOurImage, pt, ILD_NORMAL);

				// Erase background to left of icon.
				// Don't erase icon or it will flicker.
				IMAGEINFO ii;
				m_pStateImageList->GetImageInfo( iOurImage, &ii );
				rectFill.right = pt.x;
				pPC->pdc->FillRect(rectFill, &brush);

				// Move the fill rect over for next fill between icon and label
				// Erase background between icon and label
				rectFill.left = rectFill.right + (ii.rcImage.right-ii.rcImage.left);
				rectFill.right = rectText.left;
				pPC->pdc->FillRect(rectFill, &brush);

				// Since we have an icon, the hierarchy lines end where
				// the icon begins
				rcIndent.right = pt.x;
			}
		}

		// do we draw an associated icon?
		if( pPC->lvix.stateEx &  LVIS_EX_ASSOCIATED_ICON )
		{
			if( pPC->da == DrawEntire && bHasImage)
			{
				assert(m_pAssociatedIconManager);
				HICON hIcon = m_pAssociatedIconManager->GetIcon(iImage);

				//Then draw icons/bitmaps/whatever
				CPoint pt(  xOrigin + pPC->pItem->rcIcon.left,
					pPC->rect.top  + pPC->pItem->rcIcon.top  );
				DrawIconEx(	pPC->pdc->GetSafeHdc(),pt.x,pt.y+2,hIcon,16,16,0,NULL,DI_NORMAL);
				
				// Erase background to left of icon.
				// Don't erase icon or it will flicker.
				IMAGEINFO ii;
				m_pImageList->GetImageInfo( 0, &ii );
				rectFill.right = pt.x;
				pPC->pdc->FillRect(rectFill, &brush);
				
				// Move the fill rect over for next fill between icon and label
				// Erase background between icon and label
				rectFill.left = rectFill.right + (ii.rcImage.right-ii.rcImage.left);
				rectFill.right = rectText.left;
				pPC->pdc->FillRect(rectFill, &brush);
				
				// Since we have an icon, the hierarchy lines end where
				// the icon begins
				rcIndent.right = pt.x;
			}
		}
		else
		{
			if( pPC->da == DrawEntire && bHasImage)
			{
				//Then draw icons/bitmaps/whatever

				if( iImage >= m_pImageList->GetImageCount() )
				{
					TRACE( _T("Warning: Image does not exist\n") );
					iImage=0;
				}

				if (m_role == INDICATOR_TREE)
				{
					int nWidth = pPC->pItem->rcIcon.right - pPC->pItem->rcIcon.left;
					pPC->pItem->rcIcon.left = 0;
					pPC->pItem->rcIcon.right = pPC->pItem->rcIcon.left + nWidth;
				}
	 			CPoint pt(  xOrigin + pPC->pItem->rcIcon.left,
	 						pPC->rect.top  + pPC->pItem->rcIcon.top  );

				m_pImageList->SetBkColor(pPC->rgbIconBkgnd);

				// blend the icon image with the selection color if selected or drop highlighted
				// and the LVXS_BLENDIMAGE style is present
				BOOL bBlend = ((bSelected && GetFocus() == this)  || 
								(pPC->lvi.iItem == GetDropHilightIndex())) &&
								(GetListCtrlStyleEx() & LVXS_BLENDIMAGE);

				UINT nStyle = bBlend ? ILD_BLEND50 : ILD_NORMAL;
				// check for overlay image
				int iOimage =  GET_OVERLAY_IMAGE(pTvPC->tvi.state);
				if(iOimage > 0 )
					nStyle |= INDEXTOOVERLAYMASK(iOimage);
				
				m_pImageList->Draw( pPC->pdc, iImage, pt, nStyle );

				// Erase background to left of icon.
				// Don't erase icon or it will flicker.
				IMAGEINFO ii;
				m_pImageList->GetImageInfo( iImage, &ii );
				rectFill.right = pt.x;
				pPC->pdc->FillRect(rectFill, &brush);


				// Move the fill rect over for next fill between icon and label
				// Erase background between icon and label
				rectFill.left = rectFill.right + (ii.rcImage.right-ii.rcImage.left);
				rectFill.right = rectText.left;
				pPC->pdc->FillRect(rectFill, &brush);

				// Since we have an icon, the hierarchy lines end where
				// the icon begins
				rcIndent.right = pt.x;
			}
		}

		// Now erase everything to the right of the text string.
		// This is necessary because the last string painted here
		// may have been longer than the current, so we have to
		// erase everything to the right of the current text.
		rectFill.left = rectText.right;
		rectFill.right = pPC->rect.right;
		if ( rectFill.left < rectFill.right )
			pPC->pdc->FillRect(rectFill, &brush);

		// Now we can draw the lines and button since we've erased background
		if (pPC->pdc->RectVisible(rcIndent))
		{
			if (m_dwTreeStyle&TVS_HASLINES)
				DrawHierarchyLines(pPC);
			if (!bBlankImage)
			{
				if (m_dwTreeStyle&TVS_HASBUTTONS)
					DrawButton(pPC);
			}
		}

		// Compute fill rect
		rectFill.left = rectText.left - (WIDTH_GAP/2);
		rectFill.top = pPC->rect.top;
		rectFill.bottom = pPC->rect.bottom;

		if ((GetListCtrlStyleEx() & LVXS_HILIGHTSUBITEMS) && GetColumnCount() >= 1)
			rectFill.right = pPC->rect.right;	// full row select
		else if ( GetColumnCount() > 1 )
		{
			// NOT full row select, but multiple columns. Use the minimum of
			// the right side of the logical text rectangle, or the right side of the column.
			int iTextRight = xOrigin + pPC->pItem->rcText.right + (WIDTH_GAP/2);
			int iColumnRight = xOrigin + GetColumnWidth(pPC->iCol) - (SEC_RIGHT_OFFSET/2);
			rectFill.right = otmin( iTextRight, iColumnRight);
		}
		else	// plain vanilla single column and not full row select
			rectFill.right = xOrigin + pPC->pItem->rcText.right + (WIDTH_GAP/2);


		// Fill background
		BOOL bFullRow = GetListCtrlStyleEx() & LVXS_HILIGHTSUBITEMS;
		// if we are editing and NOT in full row select, do not show
		// any highlighting or focus rect
		if ( !(bEditing && !bFullRow) )
		{
			CFont* pOldFont;
			COLORREF oldFore, oldBack;
			COLORREF textColor = pPC->rgbText;
			COLORREF textBkColor = pPC->rgbTextBkgnd;

			// if the default background color
			if (textBkColor == m_rgbTextBk)
			{
				if( pPC->lvix.stateEx &  LVIS_EX_TO_DELETE )
					textBkColor = m_rgbDeleteTextBk;
				if( pPC->lvix.stateEx &  LVIS_EX_COPY_SOURCE )
					textBkColor = m_rgbCopySourceTextBk;
				if( pPC->lvix.stateEx &  LVIS_EX_COPY_DEST )
					textBkColor = m_rgbCopyDestTextBk;
			}

			if( pPC->lvix.stateEx &  LVIS_EX_EXCLUDED )
				textColor = m_rgbDisabledText;

			if( pPC->lvix.stateEx &  LVIS_EX_FROZEN )
				textColor = m_rgbFrozenText;


			// Pick appropriate colors and select them into the DC
			pOldFont = pPC->pdc->SelectObject(pPC->pFont);
			oldFore = pPC->pdc->SetTextColor( textColor );
			oldBack = pPC->pdc->SetBkColor( textBkColor );


			// Fill background (shows selection)
			CBrush brush( textBkColor );
			pPC->pdc->FillRect( rectFill, &brush );

			rectText.OffsetRect(0, 1);

			if( IsWordWrapEnabled() && !bEditing)
			{
				if( pTvPC->pNode->m_bHangingIndent )
				{
					//Hanging indent. Draw text before and after indent seperately
					CRect rectBefore,
						  rectAfter;

					rectBefore = rectText;
					rectBefore.right = rectBefore.left + pTvPC->pNode->m_cxTextBeforeIndent;
					pPC->pdc->DrawText( pTvPC->pNode->m_strBeforeIndent,
								   -1,
								   rectBefore,
								   DT_SINGLELINE|DT_NOPREFIX|DT_EXPANDTABS );
					
					rectAfter = rectText;
					rectAfter.left += pTvPC->pNode->m_cxTextBeforeIndent;
					pPC->pdc->DrawText( pTvPC->pNode->m_strAfterIndent,
								   -1,
								   rectAfter,
								   DT_WORDBREAK|DT_NOPREFIX|DT_EXPANDTABS );
				}
				else
				{
					// draw
					pPC->pdc->DrawText( strText,
						   -1,
						   rectText,
						   DT_VCENTER |DT_WORDBREAK|DT_NOPREFIX|DT_EXPANDTABS );
				}
			}
			else if ( !bEditing )
			{
				//no wordwrap

				//	Make the text-drawing behavior consistent with that of CTreeCtrl.
				int charNum;
				charNum = strText.Find('\n');
				if (charNum == -1) 
				{
					charNum = min(MAX_DISPLAY_CHAR_NUM, strText.GetLength());
				}
				else 
				{
					charNum = min(MAX_DISPLAY_CHAR_NUM, charNum);
				}

#ifndef UNDER_CE
				pPC->pdc->DrawText( strText, charNum, rectText,
					   DT_SINGLELINE|DT_LEFT| DT_VCENTER |
					   DT_NOPREFIX|DT_EXPANDTABS|DT_END_ELLIPSIS );
#else
				pPC->pdc->DrawText( strText, charNum, rectText,
					   DT_SINGLELINE|DT_LEFT| DT_VCENTER | DT_NOPREFIX|DT_EXPANDTABS);

#endif //UNDER_CE


			}

			pPC->pdc->SelectObject(pOldFont);
			pPC->pdc->SetTextColor( oldFore );
			pPC->pdc->SetBkColor( oldBack );

			//If we are in full row select, we will wait till we completely draw the row,
			// before drawing the focus rect. Unless there is only one column
			if( ( ((GetListCtrlStyleEx() & LVXS_HILIGHTSUBITEMS) == 0) || (GetColumnCount() == 1)) 
				&& pPC->lvi.iItem == SEC_TREEBASE::GetFocusIndex() 
				&& GetFocus() == this 
				&& !bEditing
				)
			{
				//Draw a selected rectangle
				pPC->pdc->DrawFocusRect( &rectFill );
			}
		}
	}
	else
		SEC_TREEBASE::DrawSubItem(pPC);
}


//-----------------------------------------------------------------------------
// CAssetManagerTreeCtrl::OnMouseWheel
// This method is called by the framework as a user rotates the mouse wheel and
// encounters the wheel's next notch.  For now we will just catch and absorb
// this message to basically disable the mousewheel in the tree controls.
//
// Input:
//  UINT nFlags -- Indicates whether various virtual keys are down.
//  short zDelta -- Indicates distance rotated.
//  CPoint pt -- Specifies the x- and y-coordinate of the cursor.
//
// Output: Nonzero if mouse wheel scrolling is enabled, otherwise 0.
//-----------------------------------------------------------------------------
BOOL CAssetManagerTreeCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	int wheelMouseLines;

	//-- get the wheel mouse information
	SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &wheelMouseLines, 0);

	m_iDeltaAccum += zDelta*wheelMouseLines;

	int iActions = m_iDeltaAccum / WHEEL_DELTA;	// per SDK docs.
	if ( iActions )
	{
		if ( m_dwScrollStyle & WS_VSCROLL)
		{
			CScrollBar* pVertSB = GetScrollBarCtrl(SB_VERT);
			if ( pVertSB )
			{
				SCROLLINFO sbi;
				pVertSB->GetScrollInfo(&sbi);

				// scroll up
				if ( (iActions > 0) && pVertSB && (sbi.nPos > 0) )
				{
					if (sbi.nPos<iActions)
					{
						iActions = sbi.nPos;
					}
					ScrollVert(-iActions);
				}
				// scroll down
				else if ( (iActions < 0) && pVertSB &&
					((sbi.nPos + (int)sbi.nPage) < (sbi.nMax+1)) ) 
				{
					if (((sbi.nPos + (int)sbi.nPage) - (sbi.nMax+1)) > iActions)
					{
						iActions = (sbi.nPos + (int)sbi.nPage) - (sbi.nMax+1);
					}
					ScrollVert(-iActions);
				}
			}

		}
		else if ( m_dwScrollStyle & WS_HSCROLL )
			ScrollHorz(iActions * 15);

		m_iDeltaAccum = 0;
	}


	CPoint parentClientPoint=pt;

	CAssetManagerDlg *pAssetManagerDlg = dynamic_cast<CAssetManagerDlg *>(m_pParent);
	if (pAssetManagerDlg)
	{
		pAssetManagerDlg->ScreenToClient(&parentClientPoint);
		pAssetManagerDlg->TreeMouseMove(nFlags,parentClientPoint);
	}
	UpdateDuplicatesDialog *pUpdateDuplicatesDlg = dynamic_cast<UpdateDuplicatesDialog *>(m_pParent);
	if (pUpdateDuplicatesDlg)
	{
		pUpdateDuplicatesDlg->ScreenToClient(&parentClientPoint);
		pUpdateDuplicatesDlg->TreeMouseMove(nFlags,parentClientPoint);
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// CAssetManagerTreeCtrl::OnLButtonDblClk
// This method is called by the framework when the user double-clicks the left
// mouse button in an item in the tree control.  Here the message is absorbed
// if the tree control is used as an indicator tree, thus disabling double
// clicks in that tree.
//
// Input:
//  UINT nFlags -- Indicates whether various virtual keys are down.
//  CPoint point -- Specifies the x- and y-coordinate of the cursor.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (m_role != INDICATOR_TREE)	
		SECTreeCtrl::OnLButtonDblClk(nFlags, point);
}


//-----------------------------------------------------------------------------
// CAssetManagerTreeCtrl::OnContextMenu
// This method is called by the framework when the user has clicked the right
// mouse button (right clicked) in this control.
//
// Input:
//  CWnd* pWnd -- Handle to the window in which the user right clicked the
//				  mouse.
//  CPoint point -- Position of the cursor, in screen coordinates, at the time
//					of the mouse click.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CAssetManagerDlg *pAssetManagerDlg = dynamic_cast<CAssetManagerDlg *>(m_pParent);
	if (pAssetManagerDlg)
	{
		if (m_role != INDICATOR_TREE)	
		{
			pAssetManagerDlg->ShowContextMenu(point, m_role);
		}
	}
	UpdateDuplicatesDialog *pUpdateDuplicatesDlg = dynamic_cast<UpdateDuplicatesDialog *>(m_pParent);
	if (pUpdateDuplicatesDlg)
	{
		pUpdateDuplicatesDlg->ShowContextMenu(point);
	}
}


//-----------------------------------------------------------------------------
// CAssetManagerTreeCtrl::NotifyItemExpanded
// This method is called by the framework when the user clicks on an item in a
// tree control to expand or collapse it.  This method will tell the other
// tree controls to expand or collapse themselves as well.
//
// Input:
//  HTREEITEM hti -- The item being expanded or collapsed.
//  UINT nCode -- The action taking place.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerTreeCtrl::NotifyItemExpanded(HTREEITEM hti, UINT nCode) const
{
	if (!m_bNodifyExpanded)
		return;

	NM_TREEVIEW nmtv;
	Node* pNode;

	// Fill an NM_TREEVIEW
	memset(&nmtv, 0, sizeof(NM_TREEVIEW));

	// Fill a NMHDR
	nmtv.hdr.hwndFrom = GetSafeHwnd();
	nmtv.hdr.idFrom   = GetDlgCtrlID();
	nmtv.hdr.code	  = TVN_ITEMEXPANDED;

	// Drag Item
	pNode = GetNode(hti);
	ASSERT_POINTER(pNode, Node);
	nmtv.itemNew = pNode->m_item;
	nmtv.itemNew.state = pNode->IsExpanded() ? TVIS_EXPANDED : 0;
	nmtv.action = nCode;

	// Send notification message to parent window:
	CWnd *pwndParent = NotifyWnd();
	pwndParent->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nmtv);

	// Tell the parent to expand or collapse the other trees:
	if (m_bTellParentOfExpand == true)
	{
		SEC_DWORD sDWItemData = this->GetItemData(hti);

		CAssetManagerDlg *pAssetManagerDlg = dynamic_cast<CAssetManagerDlg *>(m_pParent);
		if (pAssetManagerDlg)
		{
			if (m_role != INDICATOR_TREE)	
			{
				pAssetManagerDlg->ExpandItem(hti, sDWItemData, nCode);
			}
		}
	}
	else
	{
		m_bTellParentOfExpand = true;
	}
}

void CAssetManagerTreeCtrl::DrawHierarchyLines(LvPaintContext* pPC)
{
	SECTreeNode *pParent;
	CPoint pt;
	int w;

	TvPaintContext* pTvPC = (TvPaintContext*)pPC;
	int  iOrigDist = pTvPC->pNode->GetDistanceFromRoot()-1;
	BOOL bLinesAtRoot = m_dwTreeStyle&TVS_LINESATROOT && m_dwTreeStyle&(TVS_HASLINES|TVS_HASBUTTONS);

	
	// Position drawing point to far right
	pt.y = pPC->rect.top;

	BOOL bSelected = (pTvPC->tvi.state & TVIS_SELECTED) != 0;
	BOOL bHasImage = HasImage(&(pTvPC->tvi));
	int iImage;
	BOOL bBlankImage = FALSE;

	if (m_pImageList && bHasImage)
	{
		pt.x = pPC->rect.left + pTvPC->pItem->rcIcon.Width()/2 + GetIndent()*(iOrigDist-1);
		pt.y += pPC->pItem->rcIcon.top + pPC->pItem->rcIcon.Height()/2;
		iImage = bSelected ? pTvPC->tvi.iSelectedImage : pTvPC->tvi.iImage;
		if((pPC->lvix.stateEx &  LVIS_EX_ASSOCIATED_ICON)==0 && (iImage == IMG_NORM_IDX_BLANK))
			bBlankImage = TRUE;
	}
	else
	{
		pt.x = pPC->rect.left + 8 + GetIndent()*(iOrigDist-1);
		pt.y += pPC->pItem->rcText.top + pPC->pItem->rcText.Height()/2;
	}

	if (m_pStateImageList && GET_STATE_IMAGE(pTvPC->tvi.state) )
		w = pPC->rect.left + pPC->pItem->rcStateIcon.left - pt.x - 1;
	else if (m_pImageList && bHasImage)
		w = pPC->rect.left + pPC->pItem->rcIcon.left - pt.x - 1;
	else
		w = pPC->rect.left + pPC->pItem->rcText.left - pt.x - 1;


	if ( ((pt.y-m_ptViewportOrg.y)%2) != 0 )
		pt.y++;

	if ( ((pt.x+m_ptViewportOrg.x)%2) != 0 )
		pt.x++;

	if( bLinesAtRoot )
	{
		pt.x += GetIndent();
		w -= GetIndent();
	}


	// Draw the short horizontal line that connects to the
	// node icon (or text).
	if ( (bLinesAtRoot || (iOrigDist > 0)) && (bHasImage && !bBlankImage))
		DrawHorzHierarchyLine(pPC->pdc, bSelected, pt.x, pt.x + w, pt.y);

	// Draw the upward line segment of the upward L shape.  This
	// is the line connecting this node to its parent or previous
	// sibling.  (NOTE: this line segment should not be drawn for
	// the very first node in the tree.)
	if (pTvPC->lvi.iItem > 0)
	{
		DrawVertHierarchyLine(pPC->pdc, bSelected, pt.x, pt.y, pPC->rect.top);
	}

//	Draw nothing for a parent with all hidden children.
	BOOL bVisibleChildren = FALSE;
	HTREEITEM htChild = GetChildItem((HTREEITEM)pTvPC->pNode);
	while(htChild != NULL)
	{
		if(!IsHidden(htChild)) {
			bVisibleChildren = TRUE;
			break;
		}
		htChild = GetNextSiblingItem(htChild);
	}

	if(bVisibleChildren)	
	{
		// Draw the line under the icon that connects this item to
		// its children.
		if ( (pTvPC->pNode->HasChildren() || ItemHasChildrenOnDemand( (HTREEITEM)(pTvPC->pNode) )) &&
			pTvPC->pNode->IsExpanded())
		{
			int cyOff;
			if (m_pImageList && bHasImage)
				cyOff = pPC->pItem->rcIcon.Height()/2;
			else if (m_pStateImageList && GET_STATE_IMAGE(pTvPC->tvi.state) )
				cyOff = pPC->pItem->rcStateIcon.Height()/2;
			else
				cyOff = pTvPC->pItem->rcText.Height()/2;
			DrawVertHierarchyLine(pPC->pdc, bSelected, pt.x+GetIndent(), pt.y + cyOff, pPC->rect.bottom);
		}
	}
//

	// Draw the vertical lines to the left of the upward L shape.
	// Note, if the node has a next sibling, we do not draw an
	// upward L shape.  The hiearchy lines for the node described
	// above that is five level deep but has a next child is:
	//
	//     | | | | |-
	//
	// 
	pParent = pTvPC->pNode;
	for (int i = iOrigDist; i>=(bLinesAtRoot ? 0 : 1); i--)
	{
		if (pParent->GetNextSibling() != NULL) {
//		Also check to see if the item has visible siblings.
			BOOL bVisibleSibs = FALSE;
			HTREEITEM htSib = GetNextSiblingItem((HTREEITEM)pParent);
			while(htSib != NULL)
			{
				if(!IsHidden(htSib)) {
					bVisibleSibs = TRUE;
					break;
				}
				htSib = GetNextSiblingItem(htSib);
			}
			if (bVisibleSibs) {
				DrawVertHierarchyLine(pPC->pdc, bSelected, pt.x, pt.y, pPC->rect.bottom);
			}
		}
		pParent = (Node*)pParent->GetParent();
		pt.x -= GetIndent();
		pt.y = pPC->rect.top;
	}
}

BOOL CAssetManagerTreeCtrl::SetItemEx(TV_ITEM* pTVI, TV_ITEM_EX* pTVIX)
{
	Node *pNode;
	UINT cbMask;

	pNode = GetNode(pTVI->hItem);
	cbMask = 0;

	if (pTVIX->maskEx & TVIF_EX_STATEEX)
	{
		pNode->stateEx &= ~(pTVIX->stateExMask);
		pNode->stateEx |= (pTVIX->stateEx & pTVIX->stateExMask);
	}

	if (cbMask != 0)
	{
		TV_ITEM_EX copy;
		memmove(&copy, pTVIX, sizeof(TV_ITEM_EX));
		copy.maskEx = cbMask;
		NotifySetDispInfoEx(&copy);
	}

	int iItem;
	iItem = ItemIndex(pTVI->hItem);
	if (iItem != -1)
	{
		LV_ITEM lvi;
		LV_ITEM_EX lvix;

		memset(&lvi, 0, sizeof(LV_ITEM));
		memset(&lvix, 0, sizeof(LV_ITEM_EX));

		lvi.iItem = iItem;
		lvi.iSubItem = 0;

		if (pTVIX->maskEx & TVIF_EX_STATEEX)
		{
			lvix.maskEx |= LVIF_EX_STATEEX;
			if( pTVIX->stateExMask & TVIS_EX_DISABLED )
			{
				lvix.stateEx |= (pTVIX->stateEx & TVIS_EX_DISABLED) ? LVIS_EX_DISABLED : 0;
				lvix.stateExMask |= LVIS_EX_DISABLED;
			}

			if( pTVIX->stateExMask & TVIS_EX_HIDDEN )
			{
				lvix.stateEx |= (pTVIX->stateEx & TVIS_EX_HIDDEN) ? LVIS_EX_HIDDEN : 0;
				lvix.stateExMask |= LVIS_EX_HIDDEN;
			}

			if( pTVIX->stateExMask & TVIS_EX_TO_DELETE )
			{
				lvix.stateEx |= (pTVIX->stateEx & TVIS_EX_TO_DELETE) ? LVIS_EX_TO_DELETE : 0;
				lvix.stateExMask |= LVIS_EX_TO_DELETE;
			}

			if( pTVIX->stateExMask & TVIS_EX_COPY_SOURCE )
			{
				lvix.stateEx |= (pTVIX->stateEx & TVIS_EX_COPY_SOURCE) ? LVIS_EX_COPY_SOURCE : 0;
				lvix.stateExMask |= LVIS_EX_COPY_SOURCE;
			}

			if( pTVIX->stateExMask & TVIS_EX_COPY_DEST )
			{
				lvix.stateEx |= (pTVIX->stateEx & TVIS_EX_COPY_DEST) ? LVIS_EX_COPY_DEST : 0;
				lvix.stateExMask |= LVIS_EX_COPY_DEST;
			}

			if( pTVIX->stateExMask & TVIS_EX_EXCLUDED )
			{
				lvix.stateEx |= (pTVIX->stateEx & TVIS_EX_EXCLUDED) ? LVIS_EX_EXCLUDED : 0;
				lvix.stateExMask |= LVIS_EX_EXCLUDED;
			}

			if( pTVIX->stateExMask & TVIS_EX_FROZEN )
			{
				lvix.stateEx |= (pTVIX->stateEx & TVIS_EX_FROZEN) ? LVIS_EX_FROZEN : 0;
				lvix.stateExMask |= LVIS_EX_FROZEN;
			}

			if( pTVIX->stateExMask & TVIS_EX_ASSOCIATED_ICON )
			{
				lvix.stateEx |= (pTVIX->stateEx & TVIS_EX_ASSOCIATED_ICON) ? LVIS_EX_ASSOCIATED_ICON : 0;
				lvix.stateExMask |= LVIS_EX_ASSOCIATED_ICON;
			}
		}

		if (!SEC_TREEBASE::SetItemEx(&lvi, &lvix))
			return FALSE;
	}

	return TRUE;
}

BOOL CAssetManagerTreeCtrl::GetItemEx(TV_ITEM* pTVI, TV_ITEM_EX* pTVIX, BOOL bGetDispInfoEx) const
{
	Node *pNode;
	UINT cbMask;

	pNode = GetNode(pTVI->hItem);
	cbMask = 0;

	if (pTVIX->maskEx & TVIF_EX_STATEEX)
	{
		pTVIX->stateEx = 0;
		if (GetCallbackMaskEx() != 0)
			cbMask |= TVIF_EX_STATEEX;
		else 
		{
			if ((pTVIX->stateExMask & TVIS_EX_DISABLED) && pNode->IsDisabled())
				pTVIX->stateEx |= TVIS_EX_DISABLED;
			if ((pTVIX->stateExMask & TVIS_EX_HIDDEN) && pNode->IsHidden())
				pTVIX->stateEx |= TVIS_EX_HIDDEN;
			if ((pTVIX->stateExMask & TVIS_EX_TO_DELETE) && (pNode->stateEx & TVIS_EX_TO_DELETE))
				pTVIX->stateEx |= TVIS_EX_TO_DELETE;
			if ((pTVIX->stateExMask & TVIS_EX_COPY_SOURCE) && (pNode->stateEx & TVIS_EX_COPY_SOURCE))
				pTVIX->stateEx |= TVIS_EX_COPY_SOURCE;
			if ((pTVIX->stateExMask & TVIS_EX_COPY_DEST) && (pNode->stateEx & TVIS_EX_COPY_DEST))
				pTVIX->stateEx |= TVIS_EX_COPY_DEST;
			if ((pTVIX->stateExMask & TVIS_EX_EXCLUDED) && (pNode->stateEx & TVIS_EX_EXCLUDED))
				pTVIX->stateEx |= TVIS_EX_EXCLUDED;
			if ((pTVIX->stateExMask & TVIS_EX_FROZEN) && (pNode->stateEx & TVIS_EX_FROZEN))
				pTVIX->stateEx |= TVIS_EX_FROZEN;
			if ((pTVIX->stateExMask & TVIS_EX_ASSOCIATED_ICON) && (pNode->stateEx & TVIS_EX_ASSOCIATED_ICON))
				pTVIX->stateEx |= TVIS_EX_ASSOCIATED_ICON;
		}
	}

	if (bGetDispInfoEx && cbMask != 0)
	{
		pTVIX->maskEx = cbMask;
		NotifyGetDispInfoEx(pTVIX);
	}

	return TRUE;
}


BOOL CAssetManagerTreeCtrl::GetItemEx(LV_ITEM* pLVI, LV_ITEM_EX* pLVIX, BOOL bGetDispInfoEx) const
{
	if (pLVI->iSubItem == 0)
	{
		TV_ITEM tvi;
		TV_ITEM_EX tvix;

		memset(&tvi, 0, sizeof(TV_ITEM));
		memset(&tvix, 0, sizeof(TV_ITEM_EX));

		tvi.mask |= TVIF_HANDLE;
		ASSERT(pLVI->iItem >= 0);  // must have valid item spec
		tvi.hItem = GetItemAt(pLVI->iItem);
		ASSERT(tvi.hItem != NULL); // must have valid item

		if (pLVIX->maskEx & LVIF_EX_STATEEX)
		{
			tvix.maskEx = TVIF_EX_STATEEX;
			if (pLVIX->stateExMask & LVIS_EX_DISABLED)
				tvix.stateEx |= TVIS_EX_DISABLED;
			if (pLVIX->stateExMask & LVIS_EX_HIDDEN)
				tvix.stateEx |= TVIS_EX_HIDDEN;
			if (pLVIX->stateExMask & LVIS_EX_TO_DELETE)
				tvix.stateEx |= TVIS_EX_TO_DELETE;
			if (pLVIX->stateExMask & LVIS_EX_COPY_SOURCE)
				tvix.stateEx |= TVIS_EX_COPY_SOURCE;
			if (pLVIX->stateExMask & LVIS_EX_COPY_DEST)
				tvix.stateEx |= TVIS_EX_COPY_DEST;
			if (pLVIX->stateExMask & LVIS_EX_EXCLUDED)
				tvix.stateEx |= TVIS_EX_EXCLUDED;
			if (pLVIX->stateExMask & LVIS_EX_FROZEN)
				tvix.stateEx |= TVIS_EX_FROZEN;
			if (pLVIX->stateExMask & LVIS_EX_ASSOCIATED_ICON)
				tvix.stateEx |= TVIS_EX_ASSOCIATED_ICON;
		}

		if (!GetItemEx(&tvi, &tvix, bGetDispInfoEx))
			return FALSE;

		if (tvix.maskEx & TVIF_EX_STATEEX)
		{
			if (tvix.stateExMask & TVIS_EX_DISABLED)
				pLVIX->stateEx |= (tvix.stateEx & TVIS_EX_DISABLED) ? LVIS_EX_DISABLED : 0;
			if (tvix.stateExMask & TVIS_EX_HIDDEN)
				pLVIX->stateEx |= (tvix.stateEx & TVIS_EX_HIDDEN) ? LVIS_EX_HIDDEN : 0;
			if( tvix.stateExMask & TVIS_EX_TO_DELETE )
				pLVIX->stateEx |= (tvix.stateEx & TVIS_EX_TO_DELETE) ? LVIS_EX_TO_DELETE : 0;
			if( tvix.stateExMask & TVIS_EX_COPY_SOURCE )
				pLVIX->stateEx |= (tvix.stateEx & TVIS_EX_COPY_SOURCE) ? LVIS_EX_COPY_SOURCE : 0;
			if( tvix.stateExMask & TVIS_EX_COPY_DEST )
				pLVIX->stateEx |= (tvix.stateEx & TVIS_EX_COPY_DEST) ? LVIS_EX_COPY_DEST : 0;
			if( tvix.stateExMask & TVIS_EX_EXCLUDED )
				pLVIX->stateEx |= (tvix.stateEx & TVIS_EX_EXCLUDED) ? LVIS_EX_EXCLUDED : 0;
			if( tvix.stateExMask & TVIS_EX_FROZEN )
				pLVIX->stateEx |= (tvix.stateEx & TVIS_EX_FROZEN) ? LVIS_EX_FROZEN : 0;
			if( tvix.stateExMask & TVIS_EX_ASSOCIATED_ICON )
				pLVIX->stateEx |= (tvix.stateEx & TVIS_EX_ASSOCIATED_ICON) ? LVIS_EX_ASSOCIATED_ICON : 0;
		}

		return TRUE;
	}
	else
		return SEC_TREEBASE::GetItemEx(pLVI, pLVIX);
}

BOOL CAssetManagerTreeCtrl::LoadPaintContext( LvPaintContext* pPC )
{
	if (pPC->lvi.iSubItem == 0)
	{
		TvPaintContext* pTvPC;
		UINT mask;
		UINT stateMask;
	
		pTvPC = (TvPaintContext*)pPC;
		pTvPC->pNode = GetNodeAt(pPC->lvi.iItem);
		pTvPC->tvi = pTvPC->pNode->m_item;
		pTvPC->tvi.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_STATE | TVIF_SELECTEDIMAGE | TVIF_IMAGE;
		pTvPC->tvi.stateMask = 0xffff;
		pTvPC->tvi.pszText = m_pszInternalBuffer;
		pTvPC->tvi.cchTextMax = LVXC_INTERNALBUFFERSIZE;
	
		// The following GetItem call could modify the mask
		// and stateMask values, so we have to make our own
		// copy of their values.
		mask = pTvPC->tvi.mask;
		stateMask = pTvPC->tvi.stateMask;
	
		if (!GetItem(&(pTvPC->tvi), FALSE, TRUE))
			return FALSE;
	
		// Next, we load the state information which exists only for the SEC_TREEBASE
		// class.  For example, LVIS_FOCUSED has meaning in LV_ITEM, but there
		// is not corresponding state in TV_ITEM.
		pPC->lvi.mask = LVIF_STATE;
		pPC->lvi.stateMask = LVIS_FOCUSED;
		if (!GetItem(&(pPC->lvi)))
			return FALSE;
	
		// At this point, the TV_ITEM structure (pTvPC->tvi), has been loaded
		// with all information required to paint the node.  However, some of
		// the information (i.e. selection state TVIS_SELECTED), has an equivalent
		// value (LVIS_SELECTED) in the LV_ITEM structure (pPC->lvi).  Since
		// the SEC_TREEBASE class will rely on the latter, we have to translate TV_ITEM
		// settings to LV_ITEM settings which makes the item's state accessible
		// by the SEC_TREEBASE class.
		if (mask & TVIF_STATE)
		{
			if (stateMask & TVIS_CUT)
				pPC->lvi.state |= (pTvPC->tvi.state & TVIS_CUT) ? LVIS_CUT : 0;
			if (stateMask & TVIS_DROPHILITED)
				pPC->lvi.state |= (pTvPC->tvi.state & TVIS_DROPHILITED) ? LVIS_DROPHILITED : 0;
			if (stateMask & TVIS_OVERLAYMASK)
				pPC->lvi.state |= (pTvPC->tvi.state & TVIS_OVERLAYMASK) ? LVIS_OVERLAYMASK : 0;
			if (stateMask & TVIS_SELECTED)
				pPC->lvi.state |= (pTvPC->tvi.state & TVIS_SELECTED) ? LVIS_SELECTED : 0;
			if (stateMask & TVIS_STATEIMAGEMASK)
				pPC->lvi.state |= INDEXTOSTATEIMAGEMASK(GET_STATE_IMAGE(pTvPC->tvi.state));
		}
	
		if (mask & TVIF_TEXT)
			pPC->lvi.pszText = pTvPC->tvi.pszText;
	
		if (mask & TVIF_IMAGE)
			pPC->lvi.iImage = pTvPC->tvi.iImage;
	
		// Retrieve any extended flags
		pTvPC->tvix.maskEx = TVIF_EX_STATEEX;
		pTvPC->tvix.stateExMask = TVIS_EX_DISABLED|TVIS_EX_HIDDEN|TVIS_EX_TO_DELETE|TVIS_EX_COPY_SOURCE|TVIS_EX_COPY_DEST|TVIS_EX_EXCLUDED|TVIS_EX_FROZEN|TVIS_EX_ASSOCIATED_ICON;
		pTvPC->lvix.maskEx = LVIF_EX_STATEEX;
		pTvPC->lvix.stateExMask = LVIS_EX_DISABLED|LVIS_EX_HIDDEN|LVIS_EX_TO_DELETE|LVIS_EX_COPY_SOURCE|LVIS_EX_COPY_DEST|LVIS_EX_EXCLUDED|LVIS_EX_FROZEN|LVIS_EX_ASSOCIATED_ICON;
		pTvPC->lvix.stateEx = 0;
		if (!GetItemEx(&(pTvPC->tvi), &(pTvPC->tvix)))
			return FALSE;
	
		if (pTvPC->tvix.stateEx & TVIS_EX_DISABLED)
			pTvPC->lvix.stateEx |= LVIS_EX_DISABLED;
	
		if (pTvPC->tvix.stateEx & TVIS_EX_HIDDEN)
			pTvPC->lvix.stateEx |= LVIS_EX_HIDDEN;

		if (pTvPC->tvix.stateEx & TVIS_EX_TO_DELETE)
			pTvPC->lvix.stateEx |= LVIS_EX_TO_DELETE;

		if (pTvPC->tvix.stateEx & TVIS_EX_COPY_SOURCE)
			pTvPC->lvix.stateEx |= LVIS_EX_COPY_SOURCE;

		if (pTvPC->tvix.stateEx & TVIS_EX_COPY_DEST)
			pTvPC->lvix.stateEx |= LVIS_EX_COPY_DEST;

		if (pTvPC->tvix.stateEx & TVIS_EX_EXCLUDED)
			pTvPC->lvix.stateEx |= LVIS_EX_EXCLUDED;

		if (pTvPC->tvix.stateEx & TVIS_EX_FROZEN)
			pTvPC->lvix.stateEx |= LVIS_EX_FROZEN;

		if (pTvPC->tvix.stateEx & TVIS_EX_ASSOCIATED_ICON)
			pTvPC->lvix.stateEx |= LVIS_EX_ASSOCIATED_ICON;
		
		PickItemBkgnd(pPC);
		PickTextColors(pPC);
		PickTextFont(pPC);
		PickIconBkgnd(pPC);

		return TRUE;
	}
	else
		return SEC_TREEBASE::LoadPaintContext(pPC);
}

BOOL CAssetManagerTreeCtrl::SelectItemNew(HTREEITEM hti,bool bDeselectAll,bool bSetFocus)
{
	int nItem = ItemIndex(hti);
	if (nItem!=-1)
	{
		if (bDeselectAll)
			DeselectAllItems();

		SEC_TREEBASE::SelectItem(nItem,TRUE);
//		SelectItem(hti);

		if (bSetFocus)
			Select(hti, TVGN_CARET);
		return TRUE;
	}
	return FALSE;
#if 0

	int nItem = NewItemIndex(hti);
	if (nItem!=-1)
	{
		return SEC_TREEBASE::SelectItem(nItem,TRUE);
	}
	return false;
#endif
}

BOOL CAssetManagerTreeCtrl::IsFocus(HTREEITEM hti)
{
	return (GetCaretItem()==hti);
}

void CAssetManagerTreeCtrl::SetTopIndex(int index)
{
	m_nTopIndex = index;
}

void CAssetManagerTreeCtrl::SelectAll(void)
{
	int numItems = GetItemCount();
	SEC_TREEBASE::SelectItemRange(0,numItems-1,TRUE);
}

void CAssetManagerTreeCtrl::DeselectAll(void)
{
	DeselectAllItems();
}

void CAssetManagerTreeCtrl::InvertSelection(void)
{
	int numItems = GetItemCount();
	if (numItems==0)
		return;
	bool *pSelectedArray = new bool[numItems];
	memset(pSelectedArray,0,numItems*sizeof(bool));

	CDWordArray* pArray = GetSelectionArray();
	int nSz = pArray->GetSize();
	for(int i=0;i<nSz; i++)
	{
		int index = pArray->GetAt(i);
		ASSERT(index<numItems);
		pSelectedArray[index] = true;
	}
	DeselectAllItems();
	for(i=0;i<numItems; i++)
	{
		if (pSelectedArray[i]==false) // if it was not selected before
			SEC_TREEBASE::SelectItem(i,TRUE);
	}
	delete [] pSelectedArray;
}

void CAssetManagerTreeCtrl::SetNodifyExpanded(bool bNodifyExpanded)
{
	m_bNodifyExpanded = bNodifyExpanded;
}

void CAssetManagerTreeCtrl::SetOtherFileTree(CAssetManagerTreeCtrl *pOtherFileTree)
{
	m_pOtherFileTree = pOtherFileTree;
}

void CAssetManagerTreeCtrl::SetIndicatorTree(CAssetManagerTreeCtrl *pIndicatorTree)
{
	m_pIndicatorTree = pIndicatorTree;
}

//
// ScrollVert()
//
//  This function scrolls the list up or down.  The nScrollItems
//  argument indicates how many items to scroll the list by.
//  There is a complication:  each item in the list can have a
//	different height.  Therefore, it becomes ambiguous as to which
//  item's you mean to scroll.  For example, you can scroll three
//	items off of the top of the list or three items into view from
//  the bottom of the list.  These two cases may scroll the list
//  by different pixel deltas since the items at the top of the
//  list may have different heights than the items at the bottom.
//  This is what the bFromBottom flag is all about.  If true, the
//  height of the items just off the bottom of the display are
//  summed in computing the total distance to scroll.  If false,
//  the items at the top of the display are summed instead.
//
void CAssetManagerTreeCtrl::ScrollVert(int nScrollItems, BOOL bFromBottom)
{
	int nFirst, nLast;

	if (!bFromBottom)
	{
		nFirst = GetTopIndex();
		nLast = nFirst + nScrollItems;
		nLast = otmax(nLast, 0);
		nLast = otmin(nLast, GetItemCount()-1);
		nScrollItems = nLast - nFirst;

		if (nScrollItems != 0)
		{
			// Since we scroll by items (i.e. item granularity), all
			// we really have to do to scroll is increment/decrement
			// the value of m_nTopIndex.
			m_nTopIndex += nScrollItems;
			if(m_nTopIndex < 0)	//m_nTopIndex should never be negative
				m_nTopIndex = 0;
			// removed ScrollWindow optimization because it did not behave
			// well with word wrap and subitems with callback text.

			Invalidate();
			UpdateWindow();

			// set the other two to the same location and invalidate
			if (m_pOtherFileTree)
			{
				m_pOtherFileTree->SetTopIndex(m_nTopIndex);
				m_pOtherFileTree->Invalidate();
				m_pOtherFileTree->UpdateWindow();
			}
			if (m_pIndicatorTree)
			{
				m_pIndicatorTree->SetTopIndex(m_nTopIndex);
				m_pIndicatorTree->Invalidate();
				m_pIndicatorTree->UpdateWindow();
			}
		}
	}
	else
	{
		nFirst = GetLastFullyVisibleItem();
		nLast = nFirst + nScrollItems;
		int cyScroll = CalcRangeHeight(nFirst, nLast, TRUE);
		nScrollItems = CalcItemsInRange(cyScroll, GetTopIndex(), TRUE);
		ScrollVert(nScrollItems, FALSE);
	}
}


BOOL CAssetManagerTreeCtrl::ProcessSelectChange
	(
		int nHitIndex,
		UINT iHitFlags,
		UINT nFlags,
		SECListCtrl::SelectAction sa
	) 
{
	iHitFlags;

	m_nPendingSelect = -1;
	
	int nPrevSel = GetFocusIndex();


	// Single select, or multi select without control or shift
	if( 
		( GetListCtrlStyle() & LVS_SINGLESEL ) || 
		( (nFlags & (MK_CONTROL|MK_SHIFT)) == 0 ) 
	  )
	{
		// no change
//		Click on any selected item will be ok.
//		if (nPrevSel == nHitIndex && IsSelected(nHitIndex))
		if (nPrevSel == nHitIndex || SECListCtrl::IsSelected(nHitIndex))
			return TRUE;

		// multiple selection and clicked on already selected item
		if(SECListCtrl::IsSelected(nHitIndex) && (GetSelectedCount()>1) )
			return TRUE;

		// if right click on selected item, just move the focus
		if ( ( nFlags & MK_RBUTTON ) && SECListCtrl::IsSelected( nHitIndex ) )
		{
			m_pToolTipWnd->Hide();
			int nOldAnchor = m_nAnchor;	// don't change the multi-select anchor
			SetFocusItem( nHitIndex );
			m_nAnchor = nOldAnchor;
			return TRUE;
		}

		// Was selection change cancelled? Abort if it was
		m_bAbortPendingSelect = NotifySelChanging(nHitIndex, nPrevSel,sa);
		if(m_bAbortPendingSelect)
			return FALSE;

		m_pToolTipWnd->Hide();

		DeselectAllItems(nHitIndex);
		SelectFocusItem(nHitIndex);
		NotifySelChanged(nHitIndex, nPrevSel,sa);
	}
	// multi-select with shift or control
	else if ( ! (GetListCtrlStyle() & LVS_SINGLESEL) )
	{
		if( ! m_bAbortPendingSelect )
		{
			// Note: must query MK_SHIFT before MK_CONTROL to properly support
			// ctrl-shift multiple selection
			if (nFlags & MK_SHIFT)
			{
				BOOL shifSel = FALSE;

				if( (sa == ByKeyboard) && (nPrevSel == nHitIndex) )
				{//We are pressing VK_SPACE
					shifSel = TRUE;
				}

				if (m_nAnchor != -1 && (nPrevSel != nHitIndex))
				{
					shifSel = TRUE;
				}

				if(shifSel)
				{
					m_bAbortPendingSelect = NotifySelChanging(nHitIndex, nPrevSel,sa);
					if(m_bAbortPendingSelect)
						return FALSE;
					int nOldAnchor = m_nAnchor;
					m_pToolTipWnd->Hide();
// ATC - Added code to not deselect items when control is pressed
					if (!(nFlags & MK_CONTROL))
						DeselectAllItems();
					SECListCtrl::SelectItemRange(m_nAnchor, nHitIndex);
					SelectFocusItem(nHitIndex);
					// restore anchor, we don't change it when shift is pressed
					m_nAnchor = nOldAnchor;
					RedrawItems(m_nAnchor, nHitIndex);
					NotifySelChanged(nHitIndex, nPrevSel,sa);
				}
			}
			else if (nFlags & MK_CONTROL)
			{
				BOOL ctrlSel = FALSE;
				if ( sa == ByKeyboard )// no change to selection, just focus
				{
					int nOldAnchor = m_nAnchor;
					m_pToolTipWnd->Hide();
					SetFocusItem( nHitIndex );
					m_nAnchor = nOldAnchor;

					if(nHitIndex == nPrevSel)//We are pressing VK_SPACE, so act accordingly.
					{
						ctrlSel = TRUE;
					}

				}
				else
					ctrlSel = TRUE;

				if (ctrlSel)
				{
					m_pToolTipWnd->Hide();
					if (!m_bLookForBeginDrag)
					{
						m_bAbortPendingSelect = NotifySelChanging(nHitIndex, nPrevSel,sa);
						if(m_bAbortPendingSelect)
							return FALSE;
						SelectFocusItem(nHitIndex, !SECListCtrl::IsSelected(nHitIndex));
						NotifySelChanged(nHitIndex, nPrevSel,sa);
					}
				}
			}
		}
	}
	return TRUE;
}   

void CAssetManagerTreeCtrl::MeasureSubItem(LvPaintContext* pPC, UINT& cySubItem)
{
	int winver = WINVER;
	int winnt = _WIN32_WINNT;
	int explorer = _WIN32_IE;
	int lviSize = sizeof(pPC->lvi);

	if (pPC->lvi.iSubItem == 0)
	{
		CString strText;
		WORD    leftText = 0;

		// Tree items will be indented according to distance down the tree
		int cxIndent;
		TvPaintContext* pTvPC = (TvPaintContext*)pPC;
		cxIndent=(GetIndent()*(pTvPC->pNode->GetDistanceFromRoot()-1));

		if( m_dwTreeStyle&TVS_LINESATROOT && m_dwTreeStyle&(TVS_HASLINES|TVS_HASBUTTONS) )
			cxIndent += GetIndent();
		

		// test for state image
		BOOL bHasStateImage = FALSE;
		IMAGEINFO ii;
		::ZeroMemory( &ii, sizeof(IMAGEINFO));

		if( m_pStateImageList && 
			GET_STATE_IMAGE(pTvPC->tvi.state) && 
			m_pStateImageList->GetImageInfo( GET_STATE_IMAGE(pTvPC->tvi.state), &ii))
			bHasStateImage = TRUE;

		if( bHasStateImage )
		{

			if (!m_pImageList) {	// Make sure the icon is valid.
				pPC->pItem->rcIcon.left   = cxIndent;
				pPC->pItem->rcIcon.top    = 0;
				pPC->pItem->rcIcon.right  = pPC->pItem->rcIcon.left ;
				pPC->pItem->rcIcon.bottom = ii.rcImage.bottom - ii.rcImage.top;
			}
			
			pPC->pItem->rcStateIcon.left   = cxIndent;
			pPC->pItem->rcStateIcon.top    = 0;
			pPC->pItem->rcStateIcon.right  = pPC->pItem->rcStateIcon.left + (ii.rcImage.right-ii.rcImage.left) ; //WIDTH_BITMAP;
			pPC->pItem->rcStateIcon.bottom = ii.rcImage.bottom - ii.rcImage.top;

			//figure out where text starts
			leftText = (WORD)(pPC->pItem->rcStateIcon.right + (WIDTH_GAP));
		}


		// test for normal image
		BOOL bHasNormalImage = FALSE;
		int iImage = pTvPC->tvi.iImage;

		if( iImage >= m_pImageList->GetImageCount() )
			iImage = 0;
			
		if( m_pImageList && m_pImageList->GetImageInfo( iImage, &ii ))
			bHasNormalImage = TRUE;

		if( bHasNormalImage )
		{
			//Set icon rectangle
			if (!(m_pStateImageList && GET_STATE_IMAGE(pTvPC->tvi.state))) {	// Make sure the icon is valid.
				pPC->pItem->rcStateIcon.left   = cxIndent;
				pPC->pItem->rcStateIcon.top    = 0;
				pPC->pItem->rcStateIcon.right  = pPC->pItem->rcStateIcon.left ;
				pPC->pItem->rcStateIcon.bottom = ii.rcImage.bottom - ii.rcImage.top;
			}

			pPC->pItem->rcIcon.left   = pPC->pItem->rcStateIcon.right;
			pPC->pItem->rcIcon.top    = 0;
			pPC->pItem->rcIcon.right  = pPC->pItem->rcIcon.left + (ii.rcImage.right-ii.rcImage.left); //WIDTH_BITMAP;
			pPC->pItem->rcIcon.bottom = ii.rcImage.bottom - ii.rcImage.top;

			//figure out where text starts
			leftText = (WORD)(pPC->pItem->rcIcon.right + (WIDTH_GAP));
		}

		// no images
		if (!bHasNormalImage && !bHasStateImage)
		{
			pPC->pItem->rcIcon.left   = cxIndent;
			pPC->pItem->rcIcon.top    = 0;
			pPC->pItem->rcIcon.right  = cxIndent;
			pPC->pItem->rcIcon.bottom = 0;		
			leftText = (WORD)(cxIndent + (WIDTH_GAP));

			// Make sure the state is valid.
			pPC->pItem->rcStateIcon.left = pPC->pItem->rcStateIcon.right = pPC->pItem->rcIcon.left ;
			pPC->pItem->rcStateIcon.top = pPC->pItem->rcIcon.top ;
			pPC->pItem->rcStateIcon.bottom = pPC->pItem->rcIcon.bottom ;
		}

		//measure and SAVE the text RECTANGLE

		int w = 0;
		if ( GetColumnCount() > 1 )
			w = GetColumnWidth(pPC->iCol) - SEC_RIGHT_OFFSET;
		else if (  IsWordWrapEnabled() )
		{
			CRect r;
			GetInsideRect( &r );
			w = r.Width() - SEC_RIGHT_OFFSET;
		}
		else
			w = SECLIST_MAX_LOGICAL_WIDTH;


		pPC->pItem->rcText.SetRect(leftText, 0, w, 0);

		CFont* pOldFont=pPC->pdc->SelectObject(pPC->pFont);
		MeasureText(pPC->pdc, pTvPC->pNode, pTvPC->tvi.pszText, pPC->pItem->rcText);
		pPC->pdc->SelectObject(pOldFont);

		// Save the item's measurements
		UINT cy = otmax(pPC->pItem->rcIcon.Height(), pPC->pItem->rcStateIcon.Height());
		cy = otmax((UINT)pPC->pItem->rcText.Height(), cy);
		cySubItem = otmax(cy, cySubItem);

		// Since we're using a dotted pen in drawing the hierarchy
		// lines, we have to make sure each row is an even number of
		// pixels in height.  If we don't do this, the hierarchy lines
		// will appear blotchy.
		if ( (cySubItem%2) != 0 )
			cySubItem++;
		
		// Center the text vertically
		if (cySubItem > (UINT)pPC->pItem->rcText.Height())
			pPC->pItem->rcText.OffsetRect(0, (cySubItem - pPC->pItem->rcText.Height())/2);
	}
	else
		SEC_TREEBASE::MeasureSubItem(pPC, cySubItem);  
}

void CAssetManagerTreeCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if (m_bAllowPaint)
	{
		DoPaint(&dc);
	}
}

void CAssetManagerTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	CPoint parentClientPoint=point;
	ClientToScreen(&parentClientPoint);

	CAssetManagerDlg *pAssetManagerDlg = dynamic_cast<CAssetManagerDlg *>(m_pParent);
	if (pAssetManagerDlg)
	{
		pAssetManagerDlg->ScreenToClient(&parentClientPoint);
		pAssetManagerDlg->TreeMouseMove(nFlags,parentClientPoint);
	}
	UpdateDuplicatesDialog *pUpdateDuplicatesDlg = dynamic_cast<UpdateDuplicatesDialog *>(m_pParent);
	if (pUpdateDuplicatesDlg)
	{
		pUpdateDuplicatesDlg->ScreenToClient(&parentClientPoint);
		pUpdateDuplicatesDlg->TreeMouseMove(nFlags,parentClientPoint);
	}
	SECTreeCtrl::OnMouseMove(nFlags, point);
}


