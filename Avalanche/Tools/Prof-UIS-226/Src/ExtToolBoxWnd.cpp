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

#if (!defined __EXT_MFC_NO_TOOLBOX_CTRL)

#if (!defined __EXTTOOLBOXWND_H)
	#include <ExtToolBoxWnd.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

//#if (!defined __EXT_POPUP_MENU_WND_H)
//	#include <ExtPopupMenuWnd.h>
//#endif
//
//#if (!defined __EXT_LOCALIZATION_H)
//	#include <../Src/ExtLocalization.h>
//#endif
//
//#include <../profuisdll/resource.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtToolBoxWnd::TOOLBOX_ITEM_DATA

HICON CExtToolBoxWnd::TOOLBOX_ITEM_DATA::IconGet(
	CSize * pSize // = NULL
	)
{
	ASSERT_VALID( m_pToolBoxWnd );
HICON hIcon =
		m_pToolBoxWnd->OnToolBoxWndQueryItemIcon( this, pSize );
	return hIcon;
}

void CExtToolBoxWnd::TOOLBOX_ITEM_DATA::IconSet( HICON hIcon, bool bCopyIcon )
{
	m_icon.AssignFromHICON( hIcon, bCopyIcon );
}

__EXT_MFC_SAFE_LPCTSTR CExtToolBoxWnd::TOOLBOX_ITEM_DATA::TextGet() const
{
	ASSERT_VALID( m_pToolBoxWnd );
__EXT_MFC_SAFE_LPCTSTR sItemText =
		m_pToolBoxWnd->OnToolBoxWndQueryItemText( this );
	return sItemText;
}

void CExtToolBoxWnd::TOOLBOX_ITEM_DATA::TextSet( __EXT_MFC_SAFE_LPCTSTR sText )
{
	ASSERT_VALID( m_pToolBoxWnd );
	m_pToolBoxWnd->OnToolBoxWndDoItemTextChanging(
		this,
		m_sText,
		sText
		);
}

const CExtToolBoxWnd::TOOLBOX_ITEM_DATA *
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA::ItemGetNext(
		INT nGetCode,
		LONG nIndex, // = 1
		bool bVisibleOnly // = true
		) const
{
CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pNonConstThis =
		const_cast < CExtToolBoxWnd::TOOLBOX_ITEM_DATA * >
			( this );
	return
		pNonConstThis->ItemGetNext(
			nGetCode,
			nIndex,
			bVisibleOnly
			);
}

CExtToolBoxWnd::TOOLBOX_ITEM_DATA *
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA::ItemGetNext(
		INT nGetCode,
		LONG nIndex, // = 1
		bool bVisibleOnly // = true
		)
{
	ASSERT( this != NULL );
	ASSERT_VALID( m_pToolBoxWnd );

CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pRetVal = NULL;
	switch( nGetCode )
	{
	case __TBCGN_SIBLING: // get sibling using positive or negative index
	{
		if( nIndex == 0 )
			return this;
		int nStep =
			(nIndex > 0)
				? (1L)
				: (-1L)
				;
		pRetVal = this;
		for( ; nIndex != 0; )
		{
			pRetVal =
				(nIndex > 0)
					? pRetVal->m_pItemNext
					: pRetVal->m_pItemPrev
					;
			if( pRetVal == NULL )
				break;
			if( bVisibleOnly )
			{
				if( (pRetVal->GetItemStyle() & __TBWI_VISIBLE) == 0 )
					continue;
			}
			nIndex -= nStep;
		}
	}
	break; // get sibling using positive or negative index
	case __TBCGN_CHILD: // get child using positive index
	{
		ASSERT( nIndex >= 0 );
		pRetVal = m_pItemChild;
		if( pRetVal == NULL )
			break;
		if( bVisibleOnly )
		{
			// skip invisible childs at start
			for( ; (pRetVal->GetItemStyle() & __TBWI_VISIBLE) == 0; )
			{
				pRetVal = pRetVal->m_pItemNext;
				if( pRetVal == NULL )
					break;
			}
			if( pRetVal == NULL )
				break;
		}
		ASSERT( pRetVal != NULL );
		if( nIndex != 0 )
		{
			//if( nIndex > 0 )
///////////////////////////////////				nIndex--;
			//else
			//	nIndex++;
			pRetVal =
				pRetVal->ItemGetNext(
					__TBCGN_SIBLING,
					nIndex,
					bVisibleOnly
					);
		} // if( nIndex != 0 )
	}
	break; // get child using positive index
	case __TBCGN_PARENT: // get parent (index not used, bVisibleOnly is not used)
	{
		pRetVal = m_pItemParent;
	}
	break; // get parent (index not used, bVisibleOnly is not used)
	case __TBCGN_FIRST_SIBLING: // get first sibling (index not used)
	{
		if( m_pItemParent == NULL )
		{
			pRetVal = this;
			break;
		}
		TOOLBOX_ITEM_DATA * pTBCI = m_pItemParent->m_pItemChild;
		if( bVisibleOnly )
		{
			for( ;	pTBCI->m_pItemNext != NULL;
					pTBCI = pTBCI->m_pItemNext
					)
			{
				if( (pTBCI->GetItemStyle() & __TBWI_VISIBLE) != 0 )
				{
					pRetVal = pTBCI;
					break;
				}
			}
			if( pRetVal == NULL )
				pRetVal = pTBCI;
		}
		else
			pRetVal = pTBCI;
	}
	break; // get first sibling (index not used)
	case __TBCGN_LAST_SIBLING: // get last sibling (index not used)
	{
		TOOLBOX_ITEM_DATA * pTBCI = this;
		if( bVisibleOnly )
		{
			if( pTBCI->GetItemStyle() & __TBWI_VISIBLE )
				pRetVal = pTBCI;
		}
		for( ; pTBCI->m_pItemNext != NULL; )
		{
			pTBCI = pTBCI->m_pItemNext;
			if( bVisibleOnly )
			{
				if( pTBCI->GetItemStyle() & __TBWI_VISIBLE )
					pRetVal = pTBCI;
			}
		}
		if( !bVisibleOnly )
			pRetVal = pTBCI;
	}
	break; // get last sibling (index not used)
	case __TBCGN_FIRST_CHILD: // get first child (index not used)
	{
		if( bVisibleOnly )
		{
			for(	TOOLBOX_ITEM_DATA * pTBCI = m_pItemChild;
					pTBCI != NULL;
					pTBCI = pTBCI->m_pItemNext
					)
			{
				if( pTBCI->GetItemStyle() & __TBWI_VISIBLE )
				{
					pRetVal = pTBCI;
					break;
				}
			}
		} // if( bVisibleOnly )
		else
			pRetVal = m_pItemChild;
	}
	break; // get first child (index not used)
	case __TBCGN_LAST_CHILD: // get last child (index not used)
	{
		for(	TOOLBOX_ITEM_DATA * pTBCI = m_pItemChild;
				pTBCI != NULL;
				pTBCI = pTBCI->m_pItemNext
				)
		{
			if( bVisibleOnly )
			{
				if( pTBCI->GetItemStyle() & __TBWI_VISIBLE )
					pRetVal = pTBCI;
			}
			else
				pRetVal = pTBCI;
		}
	}
	break; // get last child (index not used)
	case __TBCGN_SIBLING_NR: // get sibling for next/prev row (index greater or less zero)
	{
		if( nIndex == 0
			|| m_pItemParent == NULL
			|| m_pItemParent == m_pToolBoxWnd->m_pItemRoot
			)
		{
			pRetVal = this;
			break;
		}
		if( m_pItemParent->GetItemStyle() & __TBWI_LISTBOX_VIEW )
		{
			pRetVal =
				ItemGetNext(
					__TBCGN_SIBLING,
					(nIndex < 0) ? (-1) : 1,
					bVisibleOnly
					);
			if( pRetVal == NULL )
				pRetVal = this;
			break;
		}

		// find first item in row
/*
		LONG nIndexInRow = 0;
		for( TOOLBOX_ITEM_DATA * pTBCI = this;
				pTBCI->m_pItemPrev != NULL
					&& (pTBCI->m_pItemPrev->GetItemStyle() & __TBWI_WRAP) == 0;
				)
		{
			pTBCI = pTBCI->m_pItemPrev;
			if( bVisibleOnly )
			{
				if( pTBCI->GetItemStyle() & __TBWI_VISIBLE )
					nIndexInRow++;
			}
			else
				nIndexInRow++;
		}
		ASSERT( pTBCI != NULL );
*/
		TOOLBOX_ITEM_DATA * pTBCI = ItemGetNext( __TBCGN_IN_ROW_FIRST, 1, bVisibleOnly );
		ASSERT( pTBCI != NULL );
		LONG nIndexInRow = ItemGetIndexInRow(bVisibleOnly);

		if( nIndex < 0 )
		{
			if( pTBCI->m_pItemPrev == NULL )
			{
				// this is the first row
				pRetVal = this;
				pTBCI = NULL;
				break;
			}
			// find start of the previous row
			pTBCI = pTBCI->ItemGetNext( __TBCGN_IN_ROW_FIRST, 1, false );
			ASSERT( pTBCI != NULL );
			if( pTBCI->m_pItemPrev == NULL )
			{
				// this is the first row
				pRetVal = this;
				pTBCI = NULL;
				break;
			}
			for(	pTBCI = pTBCI->m_pItemPrev;
					pTBCI->m_pItemPrev != NULL
						&& (pTBCI->m_pItemPrev->GetItemStyle() & __TBWI_WRAP) == 0;
					pTBCI = pTBCI->m_pItemPrev
					);
			ASSERT( pTBCI != NULL );
			// correct visibility
			if( bVisibleOnly )
				pTBCI = pTBCI->ItemGetNext( __TBCGN_IN_ROW_FIRST, 1, true );
		} // if( nIndex < 0 )
		else
		{
			// find start of next row
			for(	pTBCI = pTBCI->m_pItemNext;
					pTBCI != NULL
						&& (pTBCI->m_pItemPrev->GetItemStyle() & __TBWI_WRAP) == 0;
					pTBCI = pTBCI->m_pItemNext
					);
			if( pTBCI == NULL )
			{
				// this is the last row
				pRetVal = this;
				break;
			}
			// correct visibility
			if( bVisibleOnly )
				pTBCI = pTBCI->ItemGetNext( __TBCGN_IN_ROW_FIRST, 1, true );
		} // else from if( nIndex < 0 )
		if( pTBCI != NULL )
		{
			for( ; nIndexInRow > 0; )
			{
				if( pTBCI->m_pItemNext == NULL )
					break;
				pTBCI = pTBCI->m_pItemNext;
				if( pTBCI->GetItemStyle() & __TBWI_WRAP )
					break;
				if( bVisibleOnly )
				{
					if( (pTBCI->GetItemStyle() & __TBWI_VISIBLE) == 0 )
						continue;
				}
				nIndexInRow--;
			}
			pRetVal = pTBCI;
		}
	}
	break; // get sibling for next/prev row (index greater or less zero)
	case __TBCGN_IN_ROW_FIRST: // get first in row (index not used)
	{
		pRetVal = this;
		if(		m_pItemParent == NULL
			||	m_pItemParent == m_pToolBoxWnd->m_pItemRoot
			||	(m_pItemParent->GetItemStyle() & __TBWI_LISTBOX_VIEW) != 0
			)
			break;
		if( bVisibleOnly )
		{
			TOOLBOX_ITEM_DATA * pTBCI = ItemGetNext( __TBCGN_IN_ROW_FIRST, 1, false );
			for( ; pTBCI != NULL; pTBCI = pTBCI->m_pItemNext )
			{
				if(pTBCI->GetItemStyle() & __TBWI_VISIBLE)
				{
					pRetVal = pTBCI;
					break;
				}
				if(pTBCI->GetItemStyle() & __TBWI_WRAP)
					break;
			}
			if( pRetVal == NULL )
				pRetVal = this;
		} // if( bVisibleOnly )
		else
		{
			for( ;	pRetVal->m_pItemPrev != NULL
						&& (pRetVal->m_pItemPrev->GetItemStyle() & __TBWI_WRAP) == 0;
					pRetVal = pRetVal->m_pItemPrev
				);
			ASSERT( pRetVal != NULL );
		} // else from if( bVisibleOnly )
	}
	break; // get first in row (index not used)
	case __TBCGN_IN_ROW_LAST: // get last in row (index not used)
	{
		pRetVal = this;
		if(		m_pItemParent == NULL
			||	m_pItemParent == m_pToolBoxWnd->m_pItemRoot
			||	(m_pItemParent->GetItemStyle() & __TBWI_LISTBOX_VIEW) != 0
			)
			break;
		if( bVisibleOnly )
		{
			TOOLBOX_ITEM_DATA * pTBCI = ItemGetNext( __TBCGN_IN_ROW_LAST, 1, false );
			for( ; pTBCI != NULL; pTBCI = pTBCI->m_pItemPrev )
			{
				if(pTBCI->GetItemStyle() & __TBWI_VISIBLE)
				{
					pRetVal = pTBCI;
					break;
				}
				if( pTBCI->m_pItemPrev == NULL )
					break;
				if(pTBCI->m_pItemPrev->GetItemStyle() & __TBWI_WRAP)
					break;
			}
			if( pRetVal == NULL )
				pRetVal = this;
		} // if( bVisibleOnly )
		else
		{
			for( ;	(pRetVal->GetItemStyle() & __TBWI_WRAP) == 0;
					pRetVal = pRetVal->m_pItemNext
					)
			{
				if( pRetVal->m_pItemNext == NULL )
					break;
			}
			ASSERT( pRetVal != NULL );
		} // else from if( bVisibleOnly )
	}
	break; // get last in row (index not used)
	case __TBCGN_PAGE_UP: // get for one page up (index not used, should be visible)
	{
		ASSERT( (GetItemStyle() & __TBWI_VISIBLE) != 0 );
		if(		m_pItemParent == NULL
			||	m_pItemParent == m_pToolBoxWnd->m_pItemRoot
			||	( !bVisibleOnly )
			||	( (GetItemStyle() & __TBWI_VISIBLE) == 0 )
			)
		{
			pRetVal = this;
			break;
		}
		LONG nVerticalPageSize =
			m_pToolBoxWnd->m_rcActiveChildArea.Height();
		ASSERT( nVerticalPageSize >= 0 );
		if( nVerticalPageSize == 0 )
		{
			pRetVal = this;
			break;
		}
		TOOLBOX_ITEM_DATA * pTBCI =
			ItemGetNext( __TBCGN_SIBLING_NR, -1, bVisibleOnly );
		ASSERT( pTBCI != NULL );
		pRetVal = pTBCI;
		if( nVerticalPageSize < m_sizeLastMeasured.cy
			|| pTBCI == this
			)
			break;

		LONG nIndexInRow = ItemGetIndexInRow( bVisibleOnly );
		LONG nStartVerticalOffset =
			//pTBCI->
			GetVerticalOffset();
		
		for( ; pTBCI != NULL; )
		{
			TOOLBOX_ITEM_DATA * pTBCI_Next =
				pTBCI->ItemGetNext( __TBCGN_SIBLING_NR, -1, bVisibleOnly );
			ASSERT( pTBCI_Next != NULL );
//			if( pTBCI_Next->m_pItemPrev == NULL )
//				break;
			if( pTBCI_Next == pTBCI )
				break;
			pTBCI = pTBCI_Next;
			if( (pTBCI->GetItemStyle() & __TBWI_VISIBLE) == 0 )
				continue;

			LONG nCurrVerticalOffset = pTBCI->GetVerticalOffset();
			ASSERT( nCurrVerticalOffset <= nStartVerticalOffset );
			LONG nStep = nStartVerticalOffset - nCurrVerticalOffset;
			if( nStep > nVerticalPageSize )
			{
				break;
			}
			pRetVal = pTBCI;
		}
		ASSERT( pRetVal != NULL );
		pRetVal = pRetVal->ItemGetNext( __TBCGN_IN_ROW_FIRST, 1, bVisibleOnly );
		ASSERT( pRetVal != NULL );
		for( ; nIndexInRow > 0; )
		{
			if( pRetVal->m_pItemNext == NULL )
				break;
			pRetVal = pRetVal->m_pItemNext;
			if( pRetVal->GetItemStyle() & __TBWI_WRAP )
				break;
			if( bVisibleOnly )
			{
				if( (pRetVal->GetItemStyle() & __TBWI_VISIBLE) == 0 )
					continue;
			}
			nIndexInRow--;
		}
	
	}
	break; // get for one page up (index not used, should be visible)
	case __TBCGN_PAGE_DOWN: // get for one page down (index not used, should be visible)
	{
		ASSERT( (GetItemStyle() & __TBWI_VISIBLE) != 0 );
		if(		m_pItemParent == NULL
			||	m_pItemParent == m_pToolBoxWnd->m_pItemRoot
			||	( !bVisibleOnly )
			||	( (GetItemStyle() & __TBWI_VISIBLE) == 0 )
			)
		{
			pRetVal = this;
			break;
		}
		LONG nVerticalPageSize =
			m_pToolBoxWnd->m_rcActiveChildArea.Height();
		ASSERT( nVerticalPageSize >= 0 );
		if( nVerticalPageSize == 0 )
		{
			pRetVal = this;
			break;
		}
		if( nVerticalPageSize < m_sizeLastMeasured.cy )
		{
			pRetVal = ItemGetNext( __TBCGN_SIBLING_NR, 1, bVisibleOnly );
			ASSERT( pRetVal != NULL );
			break;
		}
		pRetVal = ItemGetNext( __TBCGN_IN_ROW_LAST, 1, false /*bVisibleOnly*/ );
		if( pRetVal->m_pItemNext == NULL )
		{
			pRetVal = this;
			break;
		}
		pRetVal = pRetVal->m_pItemNext;
		ASSERT( pRetVal != NULL );
		
		LONG nIndexInRow = ItemGetIndexInRow( bVisibleOnly );
		LONG nStartVerticalOffset = GetVerticalOffset();
		
		for( ; pRetVal->m_pItemNext != NULL; )
		{
			LONG nCurrVerticalOffset = pRetVal->GetVerticalOffset();
			ASSERT( nCurrVerticalOffset >= nStartVerticalOffset );
			LONG nStep =
				nCurrVerticalOffset
				- nStartVerticalOffset
				+ pRetVal->m_sizeLastMeasured.cy
				;
			if( nStep > nVerticalPageSize )
			{
				pRetVal = pRetVal->ItemGetNext( __TBCGN_IN_ROW_FIRST, 1, bVisibleOnly );
				ASSERT( pRetVal != NULL );
				break;
			}
			TOOLBOX_ITEM_DATA * pTBCI =
				pRetVal->ItemGetNext( __TBCGN_IN_ROW_LAST, 1, bVisibleOnly );
			ASSERT( pTBCI != NULL );
			
			TOOLBOX_ITEM_DATA * pTBCI_Next =
				pTBCI->ItemGetNext( __TBCGN_SIBLING_NR, 1, bVisibleOnly );
			if( pTBCI_Next == NULL )
			//if( pTBCI->m_pItemNext == NULL )
			{
				pRetVal = pRetVal->ItemGetNext( __TBCGN_IN_ROW_FIRST, 1, bVisibleOnly );
				ASSERT( pRetVal != NULL );
				break;
			}
			//pRetVal = pTBCI->m_pItemNext;
			pRetVal = pTBCI_Next;
		}
		ASSERT( pRetVal != NULL );
		pRetVal = pRetVal->ItemGetNext( __TBCGN_IN_ROW_FIRST, 1, bVisibleOnly );
		ASSERT( pRetVal != NULL );
		for( ; nIndexInRow > 0; )
		{
			if( pRetVal->m_pItemNext == NULL )
				break;
			pRetVal = pRetVal->m_pItemNext;
			if( pRetVal->GetItemStyle() & __TBWI_WRAP )
				break;
			if( bVisibleOnly )
			{
				if( (pRetVal->GetItemStyle() & __TBWI_VISIBLE) == 0 )
					continue;
			}
			nIndexInRow--;
		}
	}
	break; // get for one page down (index not used, should be visible)
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( nGetCode )
	return pRetVal;
}

LONG CExtToolBoxWnd::TOOLBOX_ITEM_DATA::ItemGetIndexInRow(
	bool bVisibleOnly // = true
	) const
{
	ASSERT( this != NULL );
	ASSERT_VALID( m_pToolBoxWnd );

	if(		m_pItemParent == NULL
		||	m_pItemParent == m_pToolBoxWnd->m_pItemRoot
		||	(m_pItemParent->GetItemStyle() & __TBWI_LISTBOX_VIEW) != 0
		)
		return 0;
	
LONG nIndexInRow = 0;
const TOOLBOX_ITEM_DATA * pTBCI = this;
	for(	;
			pTBCI->m_pItemPrev != NULL
				&& (pTBCI->m_pItemPrev->GetItemStyle() & __TBWI_WRAP) == 0;
			)
	{
		pTBCI = pTBCI->m_pItemPrev;
		if( bVisibleOnly )
		{
			if(pTBCI->GetItemStyle() & __TBWI_VISIBLE)
				nIndexInRow++;
		}
		else
			nIndexInRow++;
	}
	ASSERT( pTBCI != NULL );
	
	return nIndexInRow;
}

LONG CExtToolBoxWnd::TOOLBOX_ITEM_DATA::ItemGetSiblingIndex(
	bool bVisibleOnly // = true
	) const
{
	ASSERT( this != NULL );
	ASSERT_VALID( m_pToolBoxWnd );

	if( m_pItemParent == NULL )
		return 0;
	
LONG nSiblingIndex = 0;
const TOOLBOX_ITEM_DATA * pTBCI =
		m_pItemParent->m_pItemChild;
	ASSERT( pTBCI != NULL );
	for( ; pTBCI != NULL; pTBCI = pTBCI->m_pItemNext )
	{
		if( pTBCI == this )
			break;
		if( bVisibleOnly )
		{
			if(pTBCI->GetItemStyle() & __TBWI_VISIBLE)
				nSiblingIndex++;
		}
		else
			nSiblingIndex++;
	}
	ASSERT( pTBCI != NULL );
	
	return nSiblingIndex;
}

DWORD CExtToolBoxWnd::TOOLBOX_ITEM_DATA::GetItemStyle() const
{
	ASSERT( this != NULL );
	ASSERT_VALID( m_pToolBoxWnd );

	return m_dwItemStyle;
}

DWORD CExtToolBoxWnd::TOOLBOX_ITEM_DATA::ModifyItemStyle(
	DWORD dwStylesAdd,
	DWORD dwStylesRemove // = 0L
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( m_pToolBoxWnd );

DWORD dwOld = m_dwItemStyle;	

bool bWasSelected = (m_dwItemStyle & __TBWI_SELECTED) ? true : false;
bool bWasActive = (m_dwItemStyle & __TBWI_ACTIVE) ? true : false;
bool bWasHover = (m_dwItemStyle & __TBWI_HOVER) ? true : false;

	m_dwItemStyle &= ~dwStylesRemove;
	m_dwItemStyle |= dwStylesAdd;

bool bNowSelected = (m_dwItemStyle & __TBWI_SELECTED) ? true : false;
bool bNowActive = (m_dwItemStyle & __TBWI_ACTIVE) ? true : false;
bool bNowHover = (m_dwItemStyle & __TBWI_HOVER) ? true : false;

	if( bNowSelected && (!bWasSelected) )
	{
		TOOLBOX_ITEM_DATA * pTBCI_OldSelected = NULL;
		TOOLBOX_ITEM_DATA * pTBCI = m_pItemPrev;
		for( ; pTBCI != NULL; pTBCI = pTBCI->m_pItemPrev )
		{
			if( pTBCI->GetItemStyle() & __TBWI_SELECTED )
			{
				pTBCI_OldSelected = pTBCI;
				break;
			}
		}
		if( pTBCI_OldSelected == NULL )
		{
			pTBCI = m_pItemNext;
			for( ; pTBCI != NULL; pTBCI = pTBCI->m_pItemNext )
			{
				if( pTBCI->GetItemStyle() & __TBWI_SELECTED )
				{
					pTBCI_OldSelected = pTBCI;
					break;
				}
			}
		} // if( pTBCI_OldSelected == NULL )
		if( pTBCI_OldSelected != NULL )
			//pTBCI_OldSelected->ModifyItemStyle( 0, __TBWI_SELECTED );
			pTBCI_OldSelected->m_dwItemStyle &= ~(__TBWI_SELECTED);
		m_pToolBoxWnd->OnToolBoxWndItemSelChange(
			pTBCI_OldSelected,
			this
			);
	} // if( bNowSelected && (!bWasSelected) )
	
	if( bWasSelected && !bNowSelected )
	{
		m_pToolBoxWnd->OnToolBoxWndItemSelChange(
			this,
			NULL
			);
	}

	if( bNowActive && (!bWasActive) )
	{
		TOOLBOX_ITEM_DATA * pTBCI_OldActive = NULL;
		TOOLBOX_ITEM_DATA * pTBCI = m_pItemPrev;
		for( ; pTBCI != NULL; pTBCI = pTBCI->m_pItemPrev )
		{
			if( pTBCI->GetItemStyle() & __TBWI_ACTIVE )
			{
				pTBCI_OldActive = pTBCI;
				break;
			}
		}
		if( pTBCI_OldActive == NULL )
		{
			pTBCI = m_pItemNext;
			for( ; pTBCI != NULL; pTBCI = pTBCI->m_pItemNext )
			{
				if( pTBCI->GetItemStyle() & __TBWI_ACTIVE )
				{
					pTBCI_OldActive = pTBCI;
					break;
				}
			}
		} // if( pTBCI_OldActive == NULL )
		if( pTBCI_OldActive != NULL )
			//pTBCI_OldActive->ModifyItemStyle( 0, __TBWI_ACTIVE );
			pTBCI_OldActive->m_dwItemStyle &= ~(__TBWI_ACTIVE);
		m_pToolBoxWnd->OnToolBoxWndItemActivate(
			pTBCI_OldActive,
			this
			);
	} // if( bNowActive && (!bWasActive) )
	
	if( bWasActive && !bNowActive )
	{
		m_pToolBoxWnd->OnToolBoxWndItemActivate(
			this,
			NULL
			);
	}

TOOLBOX_ITEM_DATA * pTBCI_OldHover =
		m_pToolBoxWnd->m_pItemTrackHover;
TOOLBOX_ITEM_DATA * pTBCI_NewHover =
		pTBCI_OldHover;
	if( bNowHover )
	{
		if( pTBCI_OldHover != NULL
			&& pTBCI_OldHover != this
			)
			pTBCI_OldHover->m_dwItemStyle &= ~(__TBWI_HOVER);
		pTBCI_NewHover = m_pToolBoxWnd->m_pItemTrackHover = this;
	} // if( bNowHover )
	else
	{
		if( bWasHover )
		{
			if( m_pToolBoxWnd->m_pItemTrackHover == this )
				m_pToolBoxWnd->m_pItemTrackHover = NULL;
		}
	} // else from if( bNowHover )

	if( pTBCI_OldHover != pTBCI_NewHover )
		m_pToolBoxWnd->OnToolBoxWndItemHoverChange(
			pTBCI_OldHover,
			pTBCI_NewHover
			);

	return dwOld;
}

const CExtToolBoxWnd::TOOLBOX_ITEM_DATA *
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA::FindSelectedChild() const
{
	ASSERT( this != NULL );
	ASSERT_VALID( m_pToolBoxWnd );
	return
		( const_cast < CExtToolBoxWnd::TOOLBOX_ITEM_DATA * > ( this ) ) ->
			FindSelectedChild();
}

CExtToolBoxWnd::TOOLBOX_ITEM_DATA *
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA::FindSelectedChild()
{
	ASSERT( this != NULL );
	ASSERT_VALID( m_pToolBoxWnd );
	for(	TOOLBOX_ITEM_DATA * pTBCI = ItemGetNext(__TBCGN_FIRST_CHILD);
			pTBCI != NULL;
			pTBCI = pTBCI->ItemGetNext(__TBCGN_SIBLING,1)
			)
	{
		if( pTBCI->GetItemStyle() & __TBWI_SELECTED )
			return pTBCI;
	}
	return NULL;
}

LONG CExtToolBoxWnd::TOOLBOX_ITEM_DATA::GetChildCount(
	bool bVisibleOnly // = true
	) const
{
	ASSERT( this != NULL );
LONG nCount = 0L;
	for(	TOOLBOX_ITEM_DATA * pTBCI = m_pItemChild;
			pTBCI != NULL;
			pTBCI = pTBCI->m_pItemNext
			)
	{
		if( bVisibleOnly )
		{
			if( (pTBCI->GetItemStyle() & __TBWI_VISIBLE) == 0 )
				continue;
		}
		nCount ++;
	}
	return nCount;
}

LONG CExtToolBoxWnd::TOOLBOX_ITEM_DATA::GetSiblingCount(
	bool bVisibleOnly // = true
	) const
{
	ASSERT( this != NULL );
	if( m_pItemParent == NULL )
		return 0;
	return m_pItemParent->GetChildCount( bVisibleOnly );
}

void CExtToolBoxWnd::TOOLBOX_ITEM_DATA::DestroyChilds()
{
CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI = m_pItemChild;
	m_pItemChild = NULL;
	for( ; pTBCI != NULL; )
	{
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI2 = 
			pTBCI->m_pItemNext;
		pTBCI->DestroyItem( false );
		pTBCI = pTBCI2;
	}
}

void CExtToolBoxWnd::TOOLBOX_ITEM_DATA::DestroyItem(
	bool bSyncPrevNext
	)
{
	ASSERT_VALID( m_pToolBoxWnd );
	DestroyChilds();
	m_pToolBoxWnd->OnToolBoxWndItemRemove( this );
	if( m_pItemParent->m_pItemChild == this )
		m_pItemParent->m_pItemChild = m_pItemNext;
	if( m_pToolBoxWnd->m_pItemActive == this )
		m_pToolBoxWnd->m_pItemActive = NULL;
	if( m_pToolBoxWnd->m_pItemActive2 == this )
		m_pToolBoxWnd->m_pItemActive2 = NULL;
	if( m_pToolBoxWnd->m_pItemTrackPressed == this )
	{
		m_pToolBoxWnd->m_pItemTrackPressed = NULL;
		if( m_pToolBoxWnd->GetSafeHwnd() != NULL )
			m_pToolBoxWnd->PostMessage( WM_CANCELMODE );
	}
	if( bSyncPrevNext )
	{
		if( m_pItemNext != NULL )
			m_pItemNext->m_pItemPrev = m_pItemPrev;
		if( m_pItemPrev != NULL )
			m_pItemPrev->m_pItemNext = m_pItemNext;
	}
	delete this;
}

CSize CExtToolBoxWnd::TOOLBOX_ITEM_DATA::MeasureItem( CDC & dc )
{
	ASSERT_VALID( m_pToolBoxWnd );
	m_sizeLastMeasured =
		m_pToolBoxWnd->OnToolBoxWndMasureItem(
			this,
			dc
			);
	return m_sizeLastMeasured;
}

void CExtToolBoxWnd::TOOLBOX_ITEM_DATA::DrawItem(
	CDC & dc,
	const CRect & rcItem
	)
{
	ASSERT_VALID( m_pToolBoxWnd );
	m_pToolBoxWnd->OnToolBoxWndDrawItem(
		this,
		rcItem,
		dc
		);
}

CFont * CExtToolBoxWnd::TOOLBOX_ITEM_DATA::ItemGetFont()
{
	ASSERT_VALID( m_pToolBoxWnd );
	return
		m_pToolBoxWnd->OnToolBoxWndQueryItemFont( this );
}

bool CExtToolBoxWnd::TOOLBOX_ITEM_DATA::CanExpandItem() const
{
	ASSERT_VALID( m_pToolBoxWnd );
const TOOLBOX_ITEM_DATA * pTBCI_Parent =
		ItemGetNext(__TBCGN_PARENT);
	if( pTBCI_Parent == NULL )
		return false;
	if( !ItemIsVisible() )
		return false;
const TOOLBOX_ITEM_DATA * pTBCI_Root =
		m_pToolBoxWnd->ItemGetRoot();
	ASSERT( pTBCI_Root != NULL );
DWORD dwToolBoxWndStyle = m_pToolBoxWnd->GetToolBoxWndStyle();
	if( pTBCI_Root == pTBCI_Parent )
	{
		if( dwToolBoxWndStyle & __TBWS_EXPAND_GROUPS )
			return IsContentPartiallyVisible();
	} // if( pTBCI_Root == pTBCI_Parent )
	else
	{
		DWORD dwParentItemStyle = pTBCI_Parent->GetItemStyle();
		if( (dwParentItemStyle & __TBWI_LISTBOX_VIEW) == 0 )
			return false; // can not expand items in icon view
		if( dwToolBoxWndStyle & __TBWS_EXPAND_LISTBOX_ITEMS )
			return IsContentPartiallyVisible();
	} // else from if( pTBCI_Root == pTBCI_Parent )
	return false;
}

bool CExtToolBoxWnd::TOOLBOX_ITEM_DATA::CanShowToolTip() const
{
	ASSERT_VALID( m_pToolBoxWnd );
const TOOLBOX_ITEM_DATA * pTBCI_Parent =
		ItemGetNext(__TBCGN_PARENT);
	if( pTBCI_Parent == NULL )
		return false;
	if( !ItemIsVisible() )
		return false;
CExtSafeString sItemText = TextGet();
	if( sItemText.IsEmpty() )
		return false;
const TOOLBOX_ITEM_DATA * pTBCI_Root =
		m_pToolBoxWnd->ItemGetRoot();
	ASSERT( pTBCI_Root != NULL );
DWORD dwToolBoxWndStyle = m_pToolBoxWnd->GetToolBoxWndStyle();
	if( pTBCI_Root == pTBCI_Parent )
	{
		if( dwToolBoxWndStyle & __TBWS_TOOLTIPS_IN_GROUPS )
			return IsContentPartiallyVisible();
	} // if( pTBCI_Root == pTBCI_Parent )
	else
	{
		DWORD dwParentItemStyle = pTBCI_Parent->GetItemStyle();
		if( dwParentItemStyle & __TBWI_LISTBOX_VIEW )
		{
			if( dwToolBoxWndStyle & __TBWS_TOOLTIPS_IN_LISTBOX_VIEW )
				return IsContentPartiallyVisible();
		}
		else
		{
			if( dwToolBoxWndStyle & __TBWS_TOOLTIPS_IN_ICONS_VIEW )
				return true;
		}
	} // else from if( pTBCI_Root == pTBCI_Parent )
	return false;
}

bool CExtToolBoxWnd::TOOLBOX_ITEM_DATA::IsContentPartiallyVisible() const
{
	ASSERT_VALID( m_pToolBoxWnd );
const TOOLBOX_ITEM_DATA * pTBCI_Parent =
		ItemGetNext(__TBCGN_PARENT);
	if( pTBCI_Parent == NULL )
		return false;
	if( !ItemIsVisible() )
		return false;

CRect rcItem = m_pToolBoxWnd->ItemGetRect( this );
LONG nWidthReal = rcItem.Width();
LONG nLastMeasuredWidth = m_sizeLastMeasured.cx;
	if( nWidthReal < nLastMeasuredWidth )
		return true;
	return false;
}

bool CExtToolBoxWnd::TOOLBOX_ITEM_DATA::ItemIsVisible() const
{
	ASSERT_VALID( m_pToolBoxWnd );
const TOOLBOX_ITEM_DATA * pTBCI_Parent =
		ItemGetNext(__TBCGN_PARENT);
	if( pTBCI_Parent == NULL )
		return false;

DWORD dwItemStyle = GetItemStyle();
	if( (dwItemStyle & __TBWI_VISIBLE) == 0 )
		return false;

const TOOLBOX_ITEM_DATA * pTBCI_Root =
		m_pToolBoxWnd->ItemGetRoot();
	ASSERT( pTBCI_Root != NULL );
	if( pTBCI_Parent == pTBCI_Root )
		return true;

	dwItemStyle = pTBCI_Parent->GetItemStyle();
	if( (dwItemStyle & __TBWI_VISIBLE) == 0 )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CExtToolBoxWnd::CItemTextEditWnd

// CEdit syntax of Create() method
BOOL CExtToolBoxWnd::CItemTextEditWnd::Create(
	DWORD dwStyle,
	const RECT & rect,
	CWnd * pParentWnd,
	UINT nID
	)
{
	ASSERT( m_pTBCI != NULL );
	if( !CEdit::Create(
			dwStyle,
			rect,
			pParentWnd,
			nID
			)
		)
	{
		ASSERT( FALSE );
		delete this;
		return FALSE;
	}
	return TRUE;
}

void CExtToolBoxWnd::CItemTextEditWnd::PostNcDestroy()
{
	delete this;
}

void CExtToolBoxWnd::CItemTextEditWnd::DoEndEdit( bool bUpdateItemText )
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( m_pTBCI != NULL );

	if( m_bInEndEdit )
		return;
	m_bInEndEdit = true;

	if( bUpdateItemText )
	{
		CExtToolBoxWnd * pWndParent =
			STATIC_DOWNCAST(
				CExtToolBoxWnd,
				GetParent()
				);
		ASSERT( pWndParent != NULL );

		if( pWndParent->OnToolBoxWndDeactivateEditor(
				m_pTBCI,
				bUpdateItemText
				)
			)
		{
			CString sWindowText;
			GetWindowText( sWindowText );
			m_pTBCI->TextSet( (LPCTSTR)sWindowText );
		}

		pWndParent->SetFocus();
	} // if( bUpdateItemText )

	DestroyWindow();
}

LRESULT CExtToolBoxWnd::CItemTextEditWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );
	ASSERT( m_pTBCI != NULL );

	switch( message )
	{
	case WM_KILLFOCUS:
	{
		HWND hWndFocus = (HWND)wParam;
		if(	hWndFocus != NULL
			&& hWndFocus == GetParent()->GetSafeHwnd()
			)
			DoEndEdit( true );
		else
			DoEndEdit( false );
		return 0;
	} // case WM_KILLFOCUS
	case WM_ACTIVATEAPP:
		if( !wParam )
			DoEndEdit( false );
		return 0;
	case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;
	case WM_KEYDOWN:
	{
		switch( int(wParam) )
		{
		case VK_RETURN:
			DoEndEdit( true );
			return 0;
		case VK_ESCAPE:
		{
			//DoEndEdit( false );
			//return 0;
			//DoEndEdit( false );

			CWnd * pWndParent = GetParent();
			ASSERT( pWndParent != NULL );
			pWndParent->SetFocus();
			return 0;
		} // case VK_ESCAPE
		case VK_TAB:
		{
			HWND hWndParent = GetParent()->GetSafeHwnd();
			if(		hWndParent != NULL
				&&	::IsWindow( hWndParent )
				//&&	( ((DWORD)::GetWindowLong(hWndParent,GWL_STYLE)) & WS_POPUP ) != 0
				)
				::SendMessage(
					hWndParent,
					WM_NEXTDLGCTL,
					CExtPopupMenuWnd::IsKeyPressed(VK_SHIFT),
					0
					);
			return 0;
		} // case VK_TAB
		case VK_UP:
		case VK_DOWN:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_HOME:
		case VK_END:
		{
			HWND hWndParent = GetParent()->GetSafeHwnd();
			if( hWndParent != NULL )
			{
				if(		int(wParam) == VK_HOME
					||	int(wParam) == VK_END
					)
				{
					if( ! CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
						break;
				}
				DoEndEdit( true );
				return
					::SendMessage(hWndParent,WM_KEYDOWN,wParam,lParam);
			}
			return 0;
		} // case VK_...
		} // switch( int(wParam) )
	}
	break; // case WM_KEYDOWN
	} // switch( message )

LRESULT lResult =
		CEdit::WindowProc(message,wParam,lParam);
	return lResult;
}

/////////////////////////////////////////////////////////////////////////////
// CExtToolBoxWnd

IMPLEMENT_DYNCREATE( CExtToolBoxWnd, CWnd );

CExtToolBoxWnd::CExtToolBoxWnd()
	: m_bDirectCreateCall( false )
	, m_bDelayRecalcLayout( true )
	, m_bPushedBtnUp( false )
	, m_bPushedBtnDown( false )
	, m_bTrackingBtnUp( false )
	, m_bTrackingBtnDown( false )
	, m_dwToolBoxWndStyle( __TBWS_DEF_STYLES )
	, m_nExpandAnimationPercent( -1L )
	, m_pItemRoot( NULL )
	, m_pItemActive( NULL )
	, m_pItemActive2( NULL )
	, m_pItemTrackPressed( NULL )
	, m_pItemTrackHover( NULL )
	, m_nHeightUpper( 0L )
	, m_nHeightLower( 0L )
	, m_nHeightChild( 0L )
	, m_nHeightChild2( 0L )
	, m_nSelRowHeight( 0L )
	, m_nWidth( 0L )
	, m_nScrollStepNo( 0L )
	, m_nScrollPos( 0L )
	, m_nScrollPos2( 0L )
	, m_nScrollSize( 0L )
	, m_nScrollSize2( 0L )
	, m_nOffsetLower( 0L )
	, m_rcBtnUp( 0, 0, 0, 0 )
	, m_rcBtnUp2( 0, 0, 0, 0 )
	, m_rcBtnDown( 0, 0, 0, 0 )
	, m_rcBtnDown2( 0, 0, 0, 0 )
	, m_rcActiveChildArea( 0, 0, 0, 0 )
	, m_rcActiveChildArea2( 0, 0, 0, 0 )
	, m_hWndEditor( NULL )
	, m_ptStartLeftBtnTrack( -1, -1 )
{
	VERIFY( RegisterToolBoxWndClass() );
	m_pItemRoot = new TOOLBOX_ITEM_DATA( this );
	m_pItemRoot->ModifyItemStyle( 0, __TBWI_VISIBLE );
}

CExtToolBoxWnd::~CExtToolBoxWnd()
{
	if( m_pItemRoot != NULL )
	{
		ItemRemoveAll( false );
		delete m_pItemRoot;
	}
}

BEGIN_MESSAGE_MAP(CExtToolBoxWnd, CWnd)
	//{{AFX_MSG_MAP(CExtToolBoxWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CANCELMODE()
	ON_WM_CAPTURECHANGED()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATEAPP()
	ON_REGISTERED_MESSAGE(
		CExtContentExpandWnd::g_nMsgPaintItemContent,
		_OnPaintExpanedItemContent
		)
END_MESSAGE_MAP()

DWORD CExtToolBoxWnd::GetToolBoxWndStyle() const
{
	ASSERT_VALID( this );
	return m_dwToolBoxWndStyle;
}

DWORD CExtToolBoxWnd::ModifyToolBoxWndStyle(
	DWORD dwStylesAdd,
	DWORD dwStylesRemove, // = 0L
	bool bUpdateCtrlNow // = false
	)
{
	ASSERT_VALID( this );

DWORD dwOld = m_dwToolBoxWndStyle;	
	m_dwToolBoxWndStyle |= dwStylesAdd;
	m_dwToolBoxWndStyle &= ~dwStylesRemove;
	UpdateToolBoxWnd( bUpdateCtrlNow );
	return dwOld;
}

void CExtToolBoxWnd::UpdateToolBoxWnd(
	bool bUpdateCtrlNow,
	bool bEnsureVisibleSelection // = false
	)
{
	ASSERT_VALID( this );
	
	m_bDelayRecalcLayout = true;

	if(		GetSafeHwnd() == NULL
		||	( ! ::IsWindow(GetSafeHwnd()) )
		)
		return;

	if( bUpdateCtrlNow
		&& ( (GetStyle() & WS_VISIBLE) != 0 )
		)
	{
		_RecalcLayoutImpl( bEnsureVisibleSelection );
		Invalidate();
		UpdateWindow();
	}
}

void CExtToolBoxWnd::_RecalcLayoutImpl(
	bool bEnsureVisibleSelection // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );

	if(		GetSafeHwnd() == NULL
		||	( ! ::IsWindow(GetSafeHwnd()) )
		)
		return;

	if( !m_bDelayRecalcLayout )
		return;
	m_bDelayRecalcLayout = false;

CRect rcClient;
	GetClientRect( &rcClient );
CSize sizeClient = rcClient.Size();

CClientDC dc( this );
	m_nHeightUpper = m_nHeightLower
		= m_nHeightChild = m_nHeightChild2
		= m_nWidth = m_nOffsetLower
		= m_nSelRowHeight
		= 0L;
	m_pItemActive = m_pItemActive2 = NULL;
	m_rcBtnUp.SetRectEmpty();
	m_rcBtnUp2.SetRectEmpty();
	m_rcBtnDown.SetRectEmpty();
	m_rcBtnDown2.SetRectEmpty();
	m_rcActiveChildArea.SetRectEmpty();
	m_rcActiveChildArea2.SetRectEmpty();
	if( m_rgnActiveChildArea.GetSafeHandle() != NULL )
		m_rgnActiveChildArea.DeleteObject();
	if( m_rgnActiveChildArea2.GetSafeHandle() != NULL )
		m_rgnActiveChildArea2.DeleteObject();

bool bPrevActiveUpper = true;
TOOLBOX_ITEM_DATA * pTBCI0 = m_pItemRoot->ItemGetNext(__TBCGN_FIRST_CHILD);
	for( ; pTBCI0 != NULL; pTBCI0 = pTBCI0->ItemGetNext(__TBCGN_SIBLING,1) )
	{
		pTBCI0->SetHorizontalOffset( 0 );
		pTBCI0->SetVerticalOffset( 0 );
		pTBCI0->ModifyItemStyle(0,__TBWI_DEF_ANY_BUTTONS);
		DWORD dwItemStyle = pTBCI0->GetItemStyle();
		ASSERT(
			( dwItemStyle & (__TBWI_ACTIVE|__TBWI_ACTIVE2) )
				!= (__TBWI_ACTIVE|__TBWI_ACTIVE2)
			);
		CSize sizeItem = pTBCI0->MeasureItem( dc );
		bool bUpperPart = true;
		if( m_pItemActive != NULL )
		{
			bUpperPart = false;
			ASSERT( (dwItemStyle&__TBWI_ACTIVE) == 0 );
		} // if( m_pItemActive != NULL )
		else
		{
			if( pTBCI0->GetItemStyle() & __TBWI_ACTIVE )
			{
				ASSERT( m_pItemActive == NULL );
				m_pItemActive = pTBCI0;
			}
		} // else from if( m_pItemActive != NULL )
		if( m_pItemActive2 == NULL )
		{
			if( dwItemStyle&__TBWI_ACTIVE2 )
			{
				ASSERT( m_pItemActive2 == NULL );
				m_pItemActive2 = pTBCI0;
				if( m_pItemActive != NULL )
					bPrevActiveUpper = false;
			}
		}
#ifdef _DEBUG
		else
		{
			ASSERT( (dwItemStyle&__TBWI_ACTIVE2) == 0 );
		}
#endif // _DEBUG
		LONG & nHeight = bUpperPart
			? m_nHeightUpper
			: m_nHeightLower;
		nHeight += sizeItem.cy + __EXT_TOOLBOXWND_MARGIN_BOTTOM_TOPLEVEL;
		m_nWidth = max( m_nWidth, sizeItem.cx );
	} // for( ; pTBCI0 != NULL; pTBCI0 = pTBCI0->ItemGetNext(__TBCGN_SIBLING,1) )

bool bActiveListViewMode = false;
bool bActiveListViewMode2 = false;
	
	if( m_pItemActive != NULL )
	{
		TOOLBOX_ITEM_DATA * pTBCI1 =
			m_pItemActive->ItemGetNext( __TBCGN_FIRST_CHILD );
		bActiveListViewMode =
			(m_pItemActive->GetItemStyle()&__TBWI_LISTBOX_VIEW)
				? true : false;
		LONG nCountInRow = 0L;
		LONG nRowWidth = 0L;
		LONG nRowHeight = 0L;
		for( ; pTBCI1 != NULL; pTBCI1 = pTBCI1->ItemGetNext(__TBCGN_SIBLING,1) )
		{
			pTBCI1->SetHorizontalOffset( 0 );
			pTBCI1->SetVerticalOffset( 0 );
			pTBCI1->ModifyItemStyle( 0, __TBWI_WRAP );
			CSize sizeItem = pTBCI1->MeasureItem( dc );
			if( bActiveListViewMode )
			{
				m_nHeightChild += sizeItem.cy + __EXT_TOOLBOXWND_MARGIN_BOTTOM_BOTTOMLEVEL;
				m_nWidth = max( m_nWidth, sizeItem.cx );
			} // if( bActiveListViewMode )
			else
			{
				pTBCI1->SetHorizontalOffset( nRowWidth );
				if( (nRowWidth + sizeItem.cx) > sizeClient.cx
					&& nCountInRow > 0
					)
				{
					m_nHeightChild += nRowHeight + __EXT_TOOLBOXWND_MARGIN_BOTTOM_BOTTOMLEVEL;
					m_nWidth = max( m_nWidth, nRowWidth );
					pTBCI1->ItemGetNext(__TBCGN_SIBLING,-1)
						->ModifyItemStyle( __TBWI_WRAP );
					pTBCI1->SetHorizontalOffset( 0 );
					nRowWidth = 0L;
					nRowHeight = 0L;
					nCountInRow = 0L;
				}
				nRowWidth += sizeItem.cx;
				nRowHeight = max( nRowHeight, sizeItem.cy );
				nCountInRow ++;
			} // else from if( bActiveListViewMode )
		}
		if( nRowHeight > 0 )
			m_nHeightChild += nRowHeight + __EXT_TOOLBOXWND_MARGIN_BOTTOM_BOTTOMLEVEL;
		m_pItemActive->ModifyItemStyle(__TBWI_HAS_BUTTON_UP);
		pTBCI1 = m_pItemActive->ItemGetNext(__TBCGN_SIBLING,1);
		if( pTBCI1 != NULL )
			pTBCI1->ModifyItemStyle(__TBWI_HAS_BUTTON_DOWN);
	} // if( m_pItemActive != NULL )

	if( m_pItemActive2 != NULL )
	{
		ASSERT(
				m_nExpandAnimationPercent >= 0
			&&	m_nExpandAnimationPercent <= 100
			);
		ASSERT( m_pItemActive != NULL );

		TOOLBOX_ITEM_DATA * pTBCI2 =
			m_pItemActive2->ItemGetNext( __TBCGN_FIRST_CHILD );
		bActiveListViewMode2 =
			(m_pItemActive2->GetItemStyle()&__TBWI_LISTBOX_VIEW)
				? true : false;
		LONG nCountInRow = 0L;
		LONG nRowWidth = 0L;
		LONG nRowHeight = 0L;
		for( ; pTBCI2 != NULL; pTBCI2 = pTBCI2->ItemGetNext(__TBCGN_SIBLING,1) )
		{
			pTBCI2->SetHorizontalOffset( 0 );
			pTBCI2->SetVerticalOffset( 0 );
			pTBCI2->ModifyItemStyle( 0, __TBWI_WRAP );
			CSize sizeItem = pTBCI2->MeasureItem( dc );
			if( bActiveListViewMode2 )
			{
				m_nHeightChild2 += sizeItem.cy + __EXT_TOOLBOXWND_MARGIN_BOTTOM_BOTTOMLEVEL;
				m_nWidth = max( m_nWidth, sizeItem.cx );
			} // if( bActiveListViewMode2 )
			else
			{
				pTBCI2->SetHorizontalOffset( nRowWidth );
				if( (nRowWidth + sizeItem.cx) > sizeClient.cx
					&& nCountInRow > 0
					)
				{
					m_nHeightChild2 += nRowHeight + __EXT_TOOLBOXWND_MARGIN_BOTTOM_BOTTOMLEVEL;
					m_nWidth = max( m_nWidth, nRowWidth );
					pTBCI2->ItemGetNext(__TBCGN_SIBLING,-1)
						->ModifyItemStyle( __TBWI_WRAP );
					pTBCI2->SetHorizontalOffset( 0 );
					nRowWidth = 0L;
					nRowHeight = 0L;
					nCountInRow = 0L;
				}
				nRowWidth += sizeItem.cx;
				nRowHeight = max( nRowHeight, sizeItem.cy );
				nCountInRow ++;
			} // else from if( bActiveListViewMode2 )
		}
		if( nRowHeight > 0 )
			m_nHeightChild2 += nRowHeight + __EXT_TOOLBOXWND_MARGIN_BOTTOM_BOTTOMLEVEL;
		m_pItemActive2->ModifyItemStyle(__TBWI_HAS_BUTTON_UP);
		pTBCI2 = m_pItemActive2->ItemGetNext(__TBCGN_SIBLING,1);
		if( pTBCI2 != NULL )
			pTBCI2->ModifyItemStyle(__TBWI_HAS_BUTTON_DOWN);

	} // if( m_pItemActive2 != NULL )
//#ifdef _DEBUG
//	else
//	{
//		ASSERT( m_nExpandAnimationPercent < 0 );
//	} // else from if( m_pItemActive2 != NULL )
//#endif // _DEBUG

	// measure selected row
LONG m_nSelRowHeight = 0;
	if( m_pItemActive != NULL )
	{
		TOOLBOX_ITEM_DATA * pTBCI_Sel =
			m_pItemActive->FindSelectedChild();
		if( pTBCI_Sel != NULL )
		{
			TOOLBOX_ITEM_DATA * pTBCI =
				pTBCI_Sel->ItemGetNext( __TBCGN_IN_ROW_FIRST );
			ASSERT( pTBCI != NULL );
			for( ;	pTBCI != NULL;
					pTBCI = pTBCI->ItemGetNext( __TBCGN_SIBLING, 1 )
					)
			{
				LONG nItemHeight =
					pTBCI->GetLastMeasuredSize().cy;
				m_nSelRowHeight = max( m_nSelRowHeight, nItemHeight );
				if( pTBCI->GetItemStyle() & __TBWI_WRAP
						|| (m_pItemActive->GetItemStyle() & __TBWI_LISTBOX_VIEW) != 0
					)
					break;
			}
		} // if( pTBCI_Sel != NULL )
		if( m_nSelRowHeight != 0 )
			m_nSelRowHeight += __EXT_TOOLBOXWND_SCROLL_GAP;
	} // if( m_pItemActive != NULL )

	m_nOffsetLower = sizeClient.cy - m_nHeightLower;
	if( m_nOffsetLower < (m_nHeightUpper + m_nSelRowHeight) )
	{
		m_nOffsetLower = m_nHeightUpper + m_nSelRowHeight;
//		m_nScrollPos = m_nScrollSize =
//			m_nScrollPos2 = m_nScrollSize = 0;
	} // if( m_nOffsetLower < (m_nHeightUpper + m_nSelRowHeight) )
//	else
	
	{ // BLOCK
		LONG nScrollAreaAvail = 
			sizeClient.cy -
			(m_nHeightUpper + m_nHeightLower);
		if( nScrollAreaAvail < m_nSelRowHeight )
			nScrollAreaAvail = m_nSelRowHeight;

		if( m_pItemActive2 != NULL )
		{
			ASSERT( m_pItemActive != NULL );
			LONG nScrollAreaAvail2 =
				::MulDiv(
					nScrollAreaAvail,
					m_nExpandAnimationPercent,
					100
					);
			nScrollAreaAvail -= nScrollAreaAvail2;
			if( bPrevActiveUpper )
			{
				for(	pTBCI0 = m_pItemActive2->ItemGetNext(__TBCGN_SIBLING,1);
						true;
						pTBCI0 = pTBCI0->ItemGetNext(__TBCGN_SIBLING,1)
					)
				{
					ASSERT( pTBCI0 != NULL );
					pTBCI0->SetVerticalOffset(
						nScrollAreaAvail
						);
					if( pTBCI0 == m_pItemActive )
						break;
				}

				m_nScrollSize = 
					m_nHeightChild - nScrollAreaAvail
					+ __EXT_TOOLBOXWND_SCROLL_GAP
					;
				if( m_nScrollSize < 0 )
					m_nScrollSize = 0;
				if( nScrollAreaAvail > 0 )
				{
					m_rcActiveChildArea.SetRect(
						0,
						m_nHeightUpper + nScrollAreaAvail, //m_nHeightUpper,
						sizeClient.cx, // m_nWidth,
						m_nHeightUpper + nScrollAreaAvail + nScrollAreaAvail2 //m_nHeightUpper + nScrollAreaAvail
						);
					m_rcActiveChildArea.bottom -= __EXT_TOOLBOXWND_MARGIN_BOTTOM_TOPLEVEL + 1;
					if(		(m_rcActiveChildArea.bottom - m_rcActiveChildArea.top)
								< __EXT_TOOLBOXWND_MIN_CHILD_AREA_DX
						||	(m_rcActiveChildArea.right - m_rcActiveChildArea.left)
								< __EXT_TOOLBOXWND_MIN_CHILD_AREA_DY
						)
						m_rcActiveChildArea.SetRectEmpty();
				}

				m_nScrollSize2 = 
					m_nHeightChild2 - nScrollAreaAvail
					+ __EXT_TOOLBOXWND_SCROLL_GAP
					;
				if( m_nScrollSize2 < 0 )
					m_nScrollSize2 = 0;
				m_rcActiveChildArea2.SetRect(
					0,
					0,
					sizeClient.cx,
					nScrollAreaAvail
					);
				if(		(m_rcActiveChildArea2.bottom - m_rcActiveChildArea2.top)
							< __EXT_TOOLBOXWND_MIN_CHILD_AREA_DX
					||	(m_rcActiveChildArea2.right - m_rcActiveChildArea2.left)
							< __EXT_TOOLBOXWND_MIN_CHILD_AREA_DY
					)
					m_rcActiveChildArea2.SetRectEmpty();

			} // if( bPrevActiveUpper )
			else
			{
				LONG nTemp = nScrollAreaAvail;
				nScrollAreaAvail = nScrollAreaAvail2;
				nScrollAreaAvail2 = nTemp;
				for(	pTBCI0 = m_pItemActive->ItemGetNext(__TBCGN_SIBLING,1);
						true;
						pTBCI0 = pTBCI0->ItemGetNext(__TBCGN_SIBLING,1)
					)
				{
					ASSERT( pTBCI0 != NULL );
					pTBCI0->SetVerticalOffset(
						-nScrollAreaAvail2
						);
					if( pTBCI0 == m_pItemActive2 )
						break;
				}

				m_nScrollSize = 
					m_nHeightChild - nScrollAreaAvail
					+ __EXT_TOOLBOXWND_SCROLL_GAP
					;
				if( m_nScrollSize < 0 )
					m_nScrollSize = 0;
				if( nScrollAreaAvail > 0 )
				{
					m_rcActiveChildArea.SetRect(
						0,
						m_nHeightUpper,
						sizeClient.cx, // m_nWidth,
						m_nHeightUpper + nScrollAreaAvail
						);
					m_rcActiveChildArea.bottom -= __EXT_TOOLBOXWND_MARGIN_BOTTOM_TOPLEVEL + 1;
					if(		(m_rcActiveChildArea.bottom - m_rcActiveChildArea.top)
								< __EXT_TOOLBOXWND_MIN_CHILD_AREA_DX
						||	(m_rcActiveChildArea.right - m_rcActiveChildArea.left)
								< __EXT_TOOLBOXWND_MIN_CHILD_AREA_DY
						)
						m_rcActiveChildArea.SetRectEmpty();
				}
			
				m_nScrollSize2 = 
					m_nHeightChild2 - nScrollAreaAvail2
					+ __EXT_TOOLBOXWND_SCROLL_GAP
					;
				if( m_nScrollSize2 < 0 )
					m_nScrollSize2 = 0;
				m_rcActiveChildArea2.SetRect(
					0,
					0,
					sizeClient.cx,
					nScrollAreaAvail2
					);
				if(		(m_rcActiveChildArea2.bottom - m_rcActiveChildArea2.top)
							< __EXT_TOOLBOXWND_MIN_CHILD_AREA_DX
					||	(m_rcActiveChildArea2.right - m_rcActiveChildArea2.left)
							< __EXT_TOOLBOXWND_MIN_CHILD_AREA_DY
					)
					m_rcActiveChildArea2.SetRectEmpty();
			
			} // else from if( bPrevActiveUpper )
		} // if( m_pItemActive2 != NULL )
		else
		{
			ASSERT( nScrollAreaAvail >= 0 );
			m_nScrollSize = 
				m_nHeightChild - nScrollAreaAvail
				+ __EXT_TOOLBOXWND_SCROLL_GAP
				;
			if( m_nScrollSize < 0 )
				m_nScrollSize = 0;
			if( nScrollAreaAvail > 0 )
			{
				m_rcActiveChildArea.SetRect(
					0,
					m_nHeightUpper,
					sizeClient.cx, // m_nWidth,
					m_nHeightUpper + nScrollAreaAvail
					);
				m_rcActiveChildArea.bottom -= __EXT_TOOLBOXWND_MARGIN_BOTTOM_TOPLEVEL + 1;
				if(		(m_rcActiveChildArea.bottom - m_rcActiveChildArea.top)
							< __EXT_TOOLBOXWND_MIN_CHILD_AREA_DX
					||	(m_rcActiveChildArea.right - m_rcActiveChildArea.left)
							< __EXT_TOOLBOXWND_MIN_CHILD_AREA_DY
					)
					m_rcActiveChildArea.SetRectEmpty();
			}
		} // else from if( m_pItemActive2 != NULL )
	} // BLOCK

	if( m_nScrollPos < 0 )
		m_nScrollPos = 0;
	else if( m_nScrollPos > m_nScrollSize )
		m_nScrollPos = m_nScrollSize;

	if( m_nScrollPos2 < 0 )
		m_nScrollPos2 = 0;
	else if( m_nScrollPos2 > m_nScrollSize2 )
		m_nScrollPos2 = m_nScrollSize2;

	// setup rects
	pTBCI0 = m_pItemRoot->ItemGetNext(__TBCGN_FIRST_CHILD);
bool bActiveSet = false;
LONG nDyPassed = 0;
	for( ; pTBCI0 != NULL; pTBCI0 = pTBCI0->ItemGetNext(__TBCGN_SIBLING,1) )
	{
		pTBCI0->SetVerticalOffset(
			pTBCI0->GetVerticalOffset()
				+ nDyPassed
			);
		if( pTBCI0 == m_pItemActive )
		{
			ASSERT( !bActiveSet );
			bActiveSet = true;
			nDyPassed = m_nOffsetLower;
		}
		else
		{
			CSize sizeItem = pTBCI0->GetLastMeasuredSize();
			nDyPassed += sizeItem.cy +  __EXT_TOOLBOXWND_MARGIN_BOTTOM_TOPLEVEL;
		}
	}
	
	if( m_pItemActive != NULL )
	{
		//nDyPassed = m_nHeightUpper - m_nScrollPos;
		nDyPassed = 
			m_pItemActive->GetVerticalOffset()
			+ m_pItemActive->GetLastMeasuredSize().cy
			+ __EXT_TOOLBOXWND_MARGIN_BOTTOM_TOPLEVEL
//			- m_nScrollPos
			;
		pTBCI0 =
			m_pItemActive->ItemGetNext( __TBCGN_FIRST_CHILD );
		LONG nRowHeight = 0L;
		for( ; pTBCI0 != NULL; pTBCI0 = pTBCI0->ItemGetNext(__TBCGN_SIBLING,1) )
		{
			CSize sizeItem = pTBCI0->GetLastMeasuredSize();
			pTBCI0->SetVerticalOffset(
				pTBCI0->GetVerticalOffset()
					+ nDyPassed
				);
			nRowHeight = max( nRowHeight, sizeItem.cy );
			if( bActiveListViewMode
				|| (pTBCI0->GetItemStyle() & __TBWI_WRAP) != 0
				)
			{
				nDyPassed += nRowHeight + __EXT_TOOLBOXWND_MARGIN_BOTTOM_BOTTOMLEVEL;
				nRowHeight = 0L;
			}
		}
	} // if( m_pItemActive != NULL )
	
	if( m_pItemActive2 != NULL )
	{
		ASSERT( m_pItemActive != NULL );
		//nDyPassed = m_nHeightUpper - m_nScrollPos2;
		nDyPassed = 
			m_pItemActive2->GetVerticalOffset()
			+ m_pItemActive2->GetLastMeasuredSize().cy
			+ __EXT_TOOLBOXWND_MARGIN_BOTTOM_TOPLEVEL
			;
		if( !m_rcActiveChildArea2.IsRectEmpty() )
		{
			m_rcActiveChildArea2.OffsetRect(
				0,
				nDyPassed
				);
		} // if( !m_rcActiveChildArea2.IsRectEmpty() )
//		nDyPassed -= m_nScrollPos2;
		pTBCI0 =
			m_pItemActive2->ItemGetNext( __TBCGN_FIRST_CHILD );
		LONG nRowHeight = 0L;
		for( ; pTBCI0 != NULL; pTBCI0 = pTBCI0->ItemGetNext(__TBCGN_SIBLING,1) )
		{
			CSize sizeItem = pTBCI0->GetLastMeasuredSize();
			pTBCI0->SetVerticalOffset(
				pTBCI0->GetVerticalOffset()
					+ nDyPassed
				);
			nRowHeight = max( nRowHeight, sizeItem.cy );
			if( bActiveListViewMode2
				|| (pTBCI0->GetItemStyle() & __TBWI_WRAP) != 0
				)
			{
				nDyPassed += nRowHeight + __EXT_TOOLBOXWND_MARGIN_BOTTOM_BOTTOMLEVEL;
				nRowHeight = 0L;
			}
		}
	} // if( m_pItemActive2 != NULL )

	if( m_pItemActive != NULL )
	{
		ASSERT( (m_pItemActive->GetItemStyle() & __TBWI_HAS_BUTTON_UP) != 0 );
		m_rcBtnUp = ItemGetRect( m_pItemActive );
		m_rcBtnUp.right = rcClient.right;
		m_rcBtnUp.left =
			m_rcBtnUp.right - m_rcBtnUp.Height()
			- __EXT_TOOLBOXWND_BTN_EXTRA_DX;
		TOOLBOX_ITEM_DATA * pTBCI1 = m_pItemActive->ItemGetNext(__TBCGN_SIBLING,1);
		if( pTBCI1 != NULL )
		{
			ASSERT( (pTBCI1->GetItemStyle() & __TBWI_HAS_BUTTON_DOWN) != 0 );
			m_rcBtnDown = ItemGetRect( pTBCI1 );
			m_rcBtnDown.right = rcClient.right;
			m_rcBtnDown.left =
				m_rcBtnDown.right - m_rcBtnUp.Height()
				- __EXT_TOOLBOXWND_BTN_EXTRA_DX;
		}
		else
		{
			m_rcBtnDown = m_rcBtnUp;
			m_rcBtnDown.OffsetRect(
				0,
				rcClient.bottom - m_rcBtnDown.bottom - __EXT_TOOLBOXWND_MARGIN_BOTTOM_TOPLEVEL
				);
		}

		if( bEnsureVisibleSelection
&& m_pItemActive2 == NULL
//&& m_nScrollSize >= m_nSelRowHeight && m_nSelRowHeight != 0
			&& m_nScrollSize > 0
			&& (pTBCI1 = m_pItemActive->FindSelectedChild()) != NULL
			&& (pTBCI1->GetItemStyle() &  __TBWI_VISIBLE) != 0
			)
		{
			CRect rcSelection = ItemGetRect( pTBCI1 );
			
			ASSERT( m_nSelRowHeight >= 0 );
			if( m_nSelRowHeight > 0
				//&& m_nSelRowHeight == m_nHeightChild
				)
			{
				ASSERT( m_nSelRowHeight > __EXT_TOOLBOXWND_SCROLL_GAP );
				rcSelection.bottom =
					rcSelection.top
					+ m_nSelRowHeight
					- __EXT_TOOLBOXWND_SCROLL_GAP
					;
			}

			if( rcSelection.bottom > m_rcActiveChildArea.bottom )
				m_nScrollPos +=
					rcSelection.bottom
					- m_rcActiveChildArea.bottom
					;
			if( rcSelection.top < m_rcActiveChildArea.top )
				m_nScrollPos -=
					m_rcActiveChildArea.top
					- rcSelection.top
					;

			if( m_nHeightChild > 0 )
			{
				if( m_nHeightChild <=
						( pTBCI1->GetLastMeasuredSize().cy
						  + __EXT_TOOLBOXWND_SCROLL_GAP )
					)
					m_nScrollPos = pTBCI1->GetVerticalOffset();
			}
		} // if( bEnsureVisibleSelection ...

		if( m_nScrollPos < 0 )
			m_nScrollPos = 0;
		else if( m_nScrollPos > m_nScrollSize )
			m_nScrollPos = m_nScrollSize;

	} // if( m_pItemActive != NULL )

	if( m_pItemActive2 != NULL )
	{
		ASSERT( (m_pItemActive2->GetItemStyle() & __TBWI_HAS_BUTTON_UP) != 0 );
		m_rcBtnUp2 = ItemGetRect( m_pItemActive2 );
		m_rcBtnUp2.right = rcClient.right;
		m_rcBtnUp2.left =
			m_rcBtnUp2.right - m_rcBtnUp2.Height()
			- __EXT_TOOLBOXWND_BTN_EXTRA_DX;
		TOOLBOX_ITEM_DATA * pTBCI2 = m_pItemActive2->ItemGetNext(__TBCGN_SIBLING,1);
		if( pTBCI2 != NULL )
		{
			ASSERT( (pTBCI2->GetItemStyle() & __TBWI_HAS_BUTTON_DOWN) != 0 );
			m_rcBtnDown2 = ItemGetRect( pTBCI2 );
			m_rcBtnDown2.right = rcClient.right;
			m_rcBtnDown2.left =
				m_rcBtnDown2.right - m_rcBtnUp2.Height()
				- __EXT_TOOLBOXWND_BTN_EXTRA_DX;
		}
		else
		{
			m_rcBtnDown2 = m_rcBtnUp2;
			m_rcBtnDown2.OffsetRect(
				0,
				rcClient.bottom - m_rcBtnDown2.bottom - __EXT_TOOLBOXWND_MARGIN_BOTTOM_TOPLEVEL
				);
		}

		if( bEnsureVisibleSelection
			&& m_nScrollSize2 > 0
			&& (pTBCI2 = m_pItemActive2->FindSelectedChild()) != NULL
			&& (pTBCI2->GetItemStyle() &  __TBWI_VISIBLE) != 0
			)
		{
			CRect rcSelection = ItemGetRect( pTBCI2 );
			if( rcSelection.bottom > m_rcActiveChildArea2.bottom )
				m_nScrollPos2 +=
					rcSelection.bottom
					- m_rcActiveChildArea2.bottom
					;
			if( rcSelection.top < m_rcActiveChildArea2.top )
				m_nScrollPos2 -=
					m_rcActiveChildArea2.top
					- rcSelection.top
					;
//			if( m_nHeightChild2 > 0 )
//			{
//				if( m_nHeightChild2 <=
//						( pTBCI2->GetLastMeasuredSize().cy
//						  + __EXT_TOOLBOXWND_SCROLL_GAP )
//					)
//					m_nScrollPos2 = pTBCI2->GetVerticalOffset();
//			}
		} // if( bEnsureVisibleSelection ...

		if( m_nScrollPos2 < 0 )
			m_nScrollPos2 = 0;
		else if( m_nScrollPos2 > m_nScrollSize2 )
			m_nScrollPos2 = m_nScrollSize2;
	} // if( m_pItemActive2 != NULL )

	if( !m_rcActiveChildArea.IsRectEmpty() )
	{
		VERIFY(
			m_rgnActiveChildArea.CreateRectRgnIndirect(
				&m_rcActiveChildArea
				)
			);
	}
	if( !m_rcActiveChildArea2.IsRectEmpty() )
	{
		VERIFY(
			m_rgnActiveChildArea2.CreateRectRgnIndirect(
				&m_rcActiveChildArea2
				)
			);
	}

}

CRect CExtToolBoxWnd::ItemGetRect(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	ASSERT( pTBCI != NULL );
	ASSERT( pTBCI->GetToolBoxWnd() == this );
	ASSERT( pTBCI != m_pItemRoot );
CRect rcItem( 0, 0, 0, 0 );
CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_Parent =
		pTBCI->ItemGetNext( __TBCGN_PARENT );
LONG nVerticalOffset = pTBCI->GetVerticalOffset();
CSize sizeItem = pTBCI->GetLastMeasuredSize();
CRect rcClient;
	GetClientRect( &rcClient );
LONG nWidth = rcClient.Width();
	if( pTBCI_Parent == m_pItemRoot )
	{
		rcItem.SetRect(
			0,
			nVerticalOffset,
			nWidth,
			nVerticalOffset + sizeItem.cy
			);
	} // if( pTBCI_Parent == m_pItemRoot )
	else
	{
		ASSERT( pTBCI_Parent->ItemGetNext( __TBCGN_PARENT ) == m_pItemRoot );
		DWORD dwParentStyle = pTBCI_Parent->GetItemStyle();
		if( dwParentStyle & (__TBWI_ACTIVE|__TBWI_ACTIVE2) )
		{
			if( dwParentStyle & __TBWI_LISTBOX_VIEW )
			{
				rcItem.SetRect(
					0,
					nVerticalOffset,
					nWidth,
					nVerticalOffset + sizeItem.cy
					);
			} // if( dwParentStyle & __TBWI_LISTBOX_VIEW )
			else
			{
				LONG nHorizontalOffset = pTBCI->GetHorizontalOffset();
				rcItem.SetRect(
					nHorizontalOffset,
					nVerticalOffset,
					nHorizontalOffset + sizeItem.cx,
					nVerticalOffset + sizeItem.cy
					);
			} // else from if( dwParentStyle & __TBWI_LISTBOX_VIEW )
		} // if( dwParentStyle & (__TBWI_ACTIVE|__TBWI_ACTIVE2) )
	} // else from if( pTBCI_Parent == m_pItemRoot )
	
	if( !rcItem.IsRectEmpty() )
	{
		if( pTBCI_Parent != m_pItemRoot )
		{
			ASSERT( pTBCI_Parent != NULL );
			DWORD dwParentStyle =
				pTBCI_Parent->GetItemStyle();
			if( dwParentStyle & __TBWI_ACTIVE )
			{
				ASSERT( (dwParentStyle & __TBWI_ACTIVE2) == 0 );
				rcItem.OffsetRect( 0, -m_nScrollPos );
			} // if( dwParentStyle & __TBWI_ACTIVE )
			else if( dwParentStyle & __TBWI_ACTIVE2 )
			{
				ASSERT( (dwParentStyle & __TBWI_ACTIVE) == 0 );
				rcItem.OffsetRect( 0, -m_nScrollPos2 );
			} // else if( dwParentStyle & __TBWI_ACTIVE2 )
		} // if( pTBCI_Parent != m_pItemRoot )

		if( pTBCI->GetItemStyle() & __TBWI_DEF_ANY_BUTTONS )
			rcItem.right -=
				m_rcBtnUp.Width()
				+ __EXT_TOOLBOXWND_BTN2TEXT_DX
				;
	} // if( !rcItem.IsRectEmpty() )
	
	return rcItem;
}

void CExtToolBoxWnd::OnToolBoxWndEraseEntire(
	CDC & dc,
	const CRect & rcClient,
	const CRect & rcActiveChildArea,
	CRgn & rgnActiveChildArea,
	const CRect & rcActiveChildArea2,
	CRgn & rgnActiveChildArea2
	)
{
	ASSERT_VALID( this );
	rcActiveChildArea;
	rgnActiveChildArea;
	rcActiveChildArea2;
	rgnActiveChildArea2;

//	dc.FillSolidRect(
//		&rcClient,
//		g_PaintManager->GetColor( COLOR_3DFACE )
//		);
	if( g_PaintManager->GetCb2DbTransparentMode(this) )
	{
		CExtPaintManager::stat_ExcludeChildAreas(
			dc,
			GetSafeHwnd(),
			CExtPaintManager::stat_DefExcludeChildAreaCallback
			);
		g_PaintManager->PaintDockerBkgnd( dc, this );
	} // if( g_PaintManager->GetCb2DbTransparentMode(this) )
	else
		dc.FillSolidRect(
			&rcClient,
			g_PaintManager->GetColor(
				CExtPaintManager::CLR_3DFACE_OUT
				)
			);
}

void CExtToolBoxWnd::OnToolBoxWndDrawEntire(
	CDC & dc,
	const CRect & rcClient,
	const CRect & rcActiveChildArea,
	CRgn & rgnActiveChildArea,
	const CRect & rcActiveChildArea2,
	CRgn & rgnActiveChildArea2
	)
{
	ASSERT_VALID( this );

	OnToolBoxWndEraseEntire(
		dc,
		rcClient,
		rcActiveChildArea,
		rgnActiveChildArea,
		rcActiveChildArea2,
		rgnActiveChildArea2
		);

	if( m_pItemActive != NULL && rgnActiveChildArea.GetSafeHandle() != NULL )
	{
		dc.SelectClipRgn( &rgnActiveChildArea );
		
		TOOLBOX_ITEM_DATA * pTBCI0 =
			m_pItemActive->ItemGetNext(__TBCGN_FIRST_CHILD);
		for( ; pTBCI0 != NULL; pTBCI0 = pTBCI0->ItemGetNext(__TBCGN_SIBLING,1) )
		{
			ASSERT( (pTBCI0->GetItemStyle() & (__TBWI_ACTIVE|__TBWI_ACTIVE2)) == 0 );
			CRect rcItem = ItemGetRect( pTBCI0 );
			pTBCI0->DrawItem( dc, rcItem );
		} // for( ; pTBCI0 != NULL; pTBCI0 = pTBCI0->ItemGetNext(__TBCGN_SIBLING,1) )
		
		dc.SelectClipRgn( NULL );
	} // if( m_pItemActive != NULL && rgnActiveChildArea.GetSafeHandle() != NULL )

	if( m_pItemActive2 != NULL && rgnActiveChildArea2.GetSafeHandle() != NULL )
	{
		dc.SelectClipRgn( &rgnActiveChildArea2 );
		
		TOOLBOX_ITEM_DATA * pTBCI0 =
			m_pItemActive2->ItemGetNext(__TBCGN_FIRST_CHILD);
		for( ; pTBCI0 != NULL; pTBCI0 = pTBCI0->ItemGetNext(__TBCGN_SIBLING,1) )
		{
			ASSERT( (pTBCI0->GetItemStyle() & (__TBWI_ACTIVE|__TBWI_ACTIVE2)) == 0 );
			CRect rcItem = ItemGetRect( pTBCI0 );
			pTBCI0->DrawItem( dc, rcItem );
		} // for( ; pTBCI0 != NULL; pTBCI0 = pTBCI0->ItemGetNext(__TBCGN_SIBLING,1) )
		
		dc.SelectClipRgn( NULL );
	} // if( m_pItemActive2 != NULL && rgnActiveChildArea2.GetSafeHandle() != NULL )

TOOLBOX_ITEM_DATA * pTBCI0 =
		m_pItemRoot->ItemGetNext(__TBCGN_FIRST_CHILD);
	for( ; pTBCI0 != NULL; pTBCI0 = pTBCI0->ItemGetNext(__TBCGN_SIBLING,1) )
	{
		CRect rcItem = ItemGetRect( pTBCI0 );
		pTBCI0->DrawItem( dc, rcItem );
	} // for( ; pTBCI0 != NULL; pTBCI0 = pTBCI0->ItemGetNext(__TBCGN_SIBLING,1) )

	if( !m_rcBtnUp.IsRectEmpty() )
		OnToolBoxWndDrawScrollButton(
			dc,
			m_rcBtnUp,
			true,
			_IsScrollEnabled(true),
			(m_bPushedBtnUp && m_nExpandAnimationPercent < 0) ? true : false
			);
	
	if( !m_rcBtnDown.IsRectEmpty() )
		OnToolBoxWndDrawScrollButton(
			dc,
			m_rcBtnDown,
			false,
			_IsScrollEnabled(false),
			(m_bPushedBtnDown && m_nExpandAnimationPercent < 0) ? true : false
			);

	if( !m_rcBtnUp2.IsRectEmpty() )
		OnToolBoxWndDrawScrollButton(
			dc,
			m_rcBtnUp2,
			true,
			_IsScrollEnabled2(true),
			false
			);
	
	if( !m_rcBtnDown2.IsRectEmpty() )
		OnToolBoxWndDrawScrollButton(
			dc,
			m_rcBtnDown2,
			false,
			_IsScrollEnabled2(false),
			false
			);
}

bool CExtToolBoxWnd::_IsScrollEnabled(
	bool bByButtonUp
	) const
{
	ASSERT_VALID( this );
	if( bByButtonUp )
		return (m_nScrollPos > 0) ? true : false;
	else
		return (m_nScrollPos < m_nScrollSize) ? true : false;
}

bool CExtToolBoxWnd::_IsScrollEnabled2(
	bool bByButtonUp
	) const
{
	ASSERT_VALID( this );
	if( bByButtonUp )
		return (m_nScrollPos2 > 0) ? true : false;
	else
		return (m_nScrollPos2 < m_nScrollSize2) ? true : false;
}

void CExtToolBoxWnd::OnToolBoxWndEraseScrollButton(
	CDC & dc,
	const CRect & rcBtn,
	bool bUpButton,
	bool bEnabled,
	bool bPushed
	)
{
	ASSERT_VALID( this );

	bUpButton;
	bEnabled;
	bPushed;
	dc.FillSolidRect(
		&rcBtn,
		g_PaintManager->GetColor(COLOR_3DFACE)
		);
}

void CExtToolBoxWnd::OnToolBoxWndDrawScrollButton(
	CDC & dc,
	const CRect & rcBtn,
	bool bUpButton,
	bool bEnabled,
	bool bPushed
	)
{
	ASSERT_VALID( this );

	OnToolBoxWndEraseScrollButton(
		dc,
		rcBtn,
		bUpButton,
		bEnabled,
		bPushed
		);

COLORREF clrHi = g_PaintManager->GetColor( COLOR_3DHILIGHT );
COLORREF clrLo = g_PaintManager->GetColor( COLOR_3DSHADOW );
CRect rcItem( rcBtn );
	dc.Draw3dRect(
		&rcItem,
		bPushed ? clrLo : clrHi,
		bPushed ? clrHi : clrLo
		);
	
	if( bPushed )
		rcItem.OffsetRect( 1, 1 );
COLORREF ColorValues[] =
{
	RGB(0,0,0),
	g_PaintManager->GetColor(
		bEnabled
			? CExtPaintManager::CLR_TEXT_OUT
			: COLOR_3DSHADOW
			)
};
	if( !bEnabled )
	{
		COLORREF ColorValues2[] =
		{
			RGB(0,0,0),
			clrHi
		};
		CRect rcItem2( rcItem );
		rcItem2.OffsetRect( 1, 1 );
		g_PaintManager->PaintGlyphCentered(
			dc,
			rcItem2,
			*(CExtPaintManager::g_DockingCaptionGlyphs[
				bUpButton
					? CExtPaintManager::__DCBT_ARROW_UP
					: CExtPaintManager::__DCBT_ARROW_DOWN
				]),
			ColorValues2
			);
	} // if( !bEnabled )
	g_PaintManager->PaintGlyphCentered(
		dc,
		rcItem,
		*(CExtPaintManager::g_DockingCaptionGlyphs[
			bUpButton
				? CExtPaintManager::__DCBT_ARROW_UP
				: CExtPaintManager::__DCBT_ARROW_DOWN
			]),
		ColorValues
		);
}

HICON CExtToolBoxWnd::OnToolBoxWndQueryItemIcon(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
	CSize * pSize // = NULL
	)
{
	ASSERT_VALID( this );
	ASSERT( pTBCI != NULL );
	if( pTBCI->m_icon.IsEmpty() )
	{
		if( pSize != NULL )
			pSize->cx = pSize->cy = 0;
		return NULL;
	}
	if( pSize != NULL )
		*pSize = pTBCI->m_icon.GetSize();
	return pTBCI->m_icon;
}

__EXT_MFC_SAFE_LPCTSTR CExtToolBoxWnd::OnToolBoxWndQueryItemText(
	const CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI
	)
{
	ASSERT_VALID( this );
	ASSERT( pTBCI != NULL );
	return pTBCI->m_sText;
}

CFont * CExtToolBoxWnd::OnToolBoxWndQueryItemFont(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI
	)
{
	ASSERT_VALID( this );
	ASSERT( pTBCI != NULL );
	pTBCI;
	return &g_PaintManager->m_FontNormal;
}

CSize CExtToolBoxWnd::OnToolBoxWndMasureItem(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
	CDC & dc
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	ASSERT( pTBCI != NULL );
	ASSERT( pTBCI != m_pItemRoot );

__EXT_MFC_SAFE_LPCTSTR sText = pTBCI->TextGet();
INT nTextLen = 0L;
	if( sText == NULL )
		sText = _T("");
	else
		nTextLen = _tcslen( sText );
CSize sizeTmp( 0, 0 );
HICON hIcon = pTBCI->IconGet( &sizeTmp );
	if( hIcon == NULL )
		sizeTmp.cx = sizeTmp.cy = 16;

bool bListViewMode = false;
TOOLBOX_ITEM_DATA * pTBCI_Parent =
		pTBCI->ItemGetNext( __TBCGN_PARENT );
	ASSERT( pTBCI_Parent != NULL );
	if( pTBCI_Parent != m_pItemRoot )
	{
		ASSERT( pTBCI_Parent->ItemGetNext( __TBCGN_PARENT ) == m_pItemRoot );
		if( pTBCI_Parent->GetItemStyle() & __TBWI_LISTBOX_VIEW )
			bListViewMode = true;
	} // if( pTBCI_Parent != m_pItemRoot )
	else
		bListViewMode = true;

LONG nItemMinDy =
		( pTBCI_Parent == m_pItemRoot )
			? __EXT_TOOLBOXWND_ITEM_MIN_DY_TOPLEVEL
			: __EXT_TOOLBOXWND_ITEM_MIN_DY_BOTTOMLEVEL
			;
CSize sizeItem(
		0,
		max( nItemMinDy, sizeTmp.cy )
		);

bool bHasIcon =
		 //( sizeTmp.cx > 0 )
		 (hIcon != NULL)
			? true
			: false
			;
	sizeItem.cx =
		((hIcon != NULL) ? sizeTmp.cx : 0) // sizeTmp.cx
		+ __EXT_TOOLBOXWND_ITEM_MARGIN_DX * 2
		;
	
	if( bListViewMode )
	{
		CRect rcTmp( 0, 0, 0, 0 );
		if( nTextLen > 0 )
		{
			CFont * pOldFont = dc.SelectObject( pTBCI->ItemGetFont() );
			dc.DrawText(
				sText, nTextLen, &rcTmp, DT_CALCRECT|DT_SINGLELINE
				);
			dc.SelectObject( pOldFont );
		} // if( nTextLen > 0 )
		sizeTmp = rcTmp.Size();
		sizeItem.cx += sizeTmp.cx;

		sizeItem.cy = max( sizeItem.cy, sizeTmp.cy );

		if( bHasIcon && sizeTmp.cx > 0 )
			sizeItem.cx +=__EXT_TOOLBOXWND_ICON2TEXT_DX;
	} // if( bListViewMode )
	else
	{
		sizeItem.cx += __EXT_TOOLBOXWND_NONLIST_MARGIN_DY*2;
		sizeItem.cy += __EXT_TOOLBOXWND_NONLIST_MARGIN_DY*2;
	} // else from if( bListViewMode )

	sizeItem.cy += __EXT_TOOLBOXWND_ITEM_MARGIN_DY * 2;

	return sizeItem;
}

void CExtToolBoxWnd::OnToolBoxWndEraseItem(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
	const CRect & rcItem,
	CDC & dc
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	ASSERT( pTBCI != NULL );
	ASSERT( pTBCI != m_pItemRoot );

DWORD dwItemStyle = pTBCI->GetItemStyle();
	if(		(dwItemStyle & __TBWI_VISIBLE) == 0
		||	rcItem.IsRectEmpty()
		||	!dc.RectVisible( &rcItem )
		)
		return;

	if( dwItemStyle & __TBWI_DRAG_ANY )
		dc.FillSolidRect(
			&rcItem,
			g_PaintManager->GetColor(
				( dwItemStyle & __TBWI_DRAG_SOURCE )
					? COLOR_3DDKSHADOW
					: COLOR_3DSHADOW
				)
			);
	else if(	(dwItemStyle & __TBWI_SELECTED) != 0
		&&		(dwItemStyle & (__TBWI_PRESSED|__TBWI_HOVER)) == 0
		)
		dc.FillRect(
			&rcItem,
			&g_PaintManager->m_brushLightDefault
			);
//	else
//		dc.FillSolidRect(
//			&rcItem,
//			g_PaintManager->GetColor( COLOR_3DFACE )
//			);
	else if(
			pTBCI == m_pItemTrackHover
		||	pTBCI->ItemGetNext( __TBCGN_PARENT ) == m_pItemRoot
		)
		dc.FillSolidRect(
			&rcItem,
			g_PaintManager->GetColor(
				CExtPaintManager::CLR_3DFACE_OUT
				)
			);
}

void CExtToolBoxWnd::OnToolBoxWndDrawItem(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
	const CRect & rcItem,
	CDC & dc
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	ASSERT( pTBCI != NULL );
	ASSERT( pTBCI != m_pItemRoot );

DWORD dwItemStyle = pTBCI->GetItemStyle();
	if(		(dwItemStyle & __TBWI_VISIBLE) == 0
		||	rcItem.IsRectEmpty()
		||	!dc.RectVisible( &rcItem )
		)
		return;

	OnToolBoxWndEraseItem( pTBCI, rcItem, dc );

bool bListViewMode = false;
TOOLBOX_ITEM_DATA * pTBCI_Parent =
		pTBCI->ItemGetNext( __TBCGN_PARENT );
	ASSERT( pTBCI_Parent != NULL );
	if( pTBCI_Parent != m_pItemRoot )
	{
		ASSERT( pTBCI_Parent->ItemGetNext( __TBCGN_PARENT ) == m_pItemRoot );
		if( pTBCI_Parent->GetItemStyle() & __TBWI_LISTBOX_VIEW )
			bListViewMode = true;
	} // if( pTBCI_Parent != m_pItemRoot )
	else
		bListViewMode = true;

	ASSERT( m_pItemRoot != pTBCI );
bool bDrawRect = false, bRectPressed = false;
	if( pTBCI_Parent == m_pItemRoot )
	{
		bDrawRect = true;
		if( (dwItemStyle & __TBWI_PRESSED) != 0 )
			bRectPressed = true;
	}
	else
	{
		if( (dwItemStyle & (__TBWI_SELECTED|__TBWI_PRESSED)) != 0 )
		{
			bDrawRect = true;
			bRectPressed = true;
		}
		else if( (dwItemStyle & __TBWI_HOVER) != 0 )
			bDrawRect = true;
	}

bool bEnabled = (dwItemStyle & __TBWI_ENABLED) ? true : false;

CSize sizeIcon( 0, 0 );
HICON hIcon = pTBCI->IconGet( &sizeIcon );
	if( bListViewMode )
	{
		CRect rcText = rcItem;
		rcText.DeflateRect(
			__EXT_TOOLBOXWND_ITEM_MARGIN_DX,
			__EXT_TOOLBOXWND_ITEM_MARGIN_DY
			);
		if( hIcon != NULL && sizeIcon.cx > 0 && sizeIcon.cy > 0 )
		{
			CRect rcIcon = rcText;
			rcIcon.right = rcIcon.left + sizeIcon.cx;
			rcText.left += sizeIcon.cx + __EXT_TOOLBOXWND_ICON2TEXT_DX;
			rcIcon.top += (rcIcon.Height() - sizeIcon.cy) / 2;
			dc.DrawState(
				rcIcon.TopLeft(),
				rcIcon.Size(), 
				hIcon, 
				bEnabled ? DSS_NORMAL : DSS_DISABLED, 
				(CBrush*)NULL
				);
		}

		if(		(rcText.right - rcText.left > 6)
			&&	(rcText.bottom - rcText.top > 6)
			)
		{
			__EXT_MFC_SAFE_LPCTSTR sText = pTBCI->TextGet();
			INT nTextLen = 0;
			if(		sText != NULL
				&&	( nTextLen = _tcslen( sText ) ) > 0
				)
			{
				CFont * pOldFont = dc.SelectObject( pTBCI->ItemGetFont() );
				int nOldBkMode = dc.SetBkMode( TRANSPARENT );
				

				CRect rcTmp( 0, 0, 0, 0 );
				dc.DrawText(
					sText, nTextLen, &rcTmp, DT_CALCRECT|DT_SINGLELINE
					);
				rcText.DeflateRect(
					0,
					( rcText.Height() - rcTmp.Height() ) / 2
					);

//				dc.DrawState(
//					rcText.TopLeft(),
//					rcText.Size(),
//					sText,
//					bEnabled ? DSS_NORMAL : DSS_DISABLED, 
//					TRUE,
//					0,
//					(CBrush*)NULL
//					);
				
				if( bEnabled || ( dwItemStyle & __TBWI_DRAG_ANY ) )
				{
					COLORREF clrTextOld =
						dc.SetTextColor(
							g_PaintManager->GetColor(
								( dwItemStyle & __TBWI_DRAG_ANY )
									? (
										( dwItemStyle & __TBWI_DRAG_SOURCE )
											? COLOR_3DFACE
											: COLOR_3DHILIGHT
										)
									: COLOR_BTNTEXT
								)
						);
					dc.DrawText(
						sText,
						nTextLen,
						rcText,
						DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS
						);
					dc.SetTextColor( clrTextOld );
				} // if( bEnabled || ( dwItemStyle & __TBWI_DRAG_ANY ) )
				else
				{
					COLORREF clrTextOld = dc.SetTextColor(
						g_PaintManager->GetColor( COLOR_3DHILIGHT )
						);
					CRect rcText2( rcText );
					rcText2.OffsetRect(1,1);
					dc.DrawText(
						sText,
						nTextLen,
						rcText2,
						DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS
						);
					dc.SetTextColor(
						g_PaintManager->GetColor( COLOR_3DSHADOW )
						);
					dc.DrawText(
						sText,
						nTextLen,
						rcText,
						DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS
						);
					dc.SetTextColor( clrTextOld );
				} // else from if( bEnabled )

				dc.SetBkMode( nOldBkMode );
				dc.SelectObject( pOldFont );
			}
		}
	} // if( bListViewMode )
	else
	{
		if( hIcon != NULL && sizeIcon.cx > 0 && sizeIcon.cy > 0 )
		{
			CRect rcIcon = rcItem;
			rcIcon.DeflateRect(
				(rcIcon.Width() - sizeIcon.cx) / 2,
				(rcIcon.Height() - sizeIcon.cy) / 2
				);

			dc.DrawState(
				rcIcon.TopLeft(),
				sizeIcon, // rcIcon.Size(), 
				hIcon, 
				bEnabled ? DSS_NORMAL : DSS_DISABLED, 
				(CBrush*)NULL
				);

//			LONG nTmp = pTBCI->ItemGetSiblingIndex(false);
//			CExtSafeString sTmp;
//			sTmp.Format( _T("%ld"), nTmp );
//			dc.DrawText(
//				sTmp,
//				rcIcon,
//				DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOCLIP
//				);
		}
	} // else from if( bListViewMode )


	if( bDrawRect )
	{
		if( bRectPressed )
		{
			if( pTBCI->ItemGetNext(__TBCGN_PARENT) == m_pItemRoot )
			{
				CRect rcItemPressed( rcItem );
				rcItemPressed.InflateRect( 0, 1, 0, 1 );
				dc.Draw3dRect(
					&rcItemPressed,
					g_PaintManager->GetColor( COLOR_3DSHADOW ),
					g_PaintManager->GetColor( COLOR_3DHILIGHT )
					);
				rcItemPressed.DeflateRect( 1, 1 );
				dc.Draw3dRect(
					&rcItemPressed,
					g_PaintManager->GetColor( COLOR_3DDKSHADOW ),
					g_PaintManager->GetColor( COLOR_3DFACE )
					);
			} // if( pTBCI->ItemGetNext(__TBCGN_PARENT) == m_pItemRoot )
			else
			{
				dc.Draw3dRect(
					&rcItem,
					g_PaintManager->GetColor( COLOR_3DSHADOW ),
					g_PaintManager->GetColor( COLOR_3DHILIGHT )
					);
			} // else from if( pTBCI->ItemGetNext(__TBCGN_PARENT) == m_pItemRoot )
		} // if( bRectPressed )
		else
		{
			dc.Draw3dRect(
				&rcItem,
				g_PaintManager->GetColor( COLOR_3DHILIGHT ),
				g_PaintManager->GetColor( COLOR_3DSHADOW )
				);
		} // else from if( bRectPressed )
	} // if( bDrawRect )
}

void CExtToolBoxWnd::OnToolBoxWndItemInsert(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	ASSERT( pTBCI != NULL );
	ASSERT( pTBCI->GetToolBoxWnd() == this );
	ASSERT( m_pItemRoot != pTBCI );
	pTBCI;
}

void CExtToolBoxWnd::OnToolBoxWndItemRemove(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	ASSERT( pTBCI != NULL );
	ASSERT( pTBCI->GetToolBoxWnd() == this );
	ASSERT( m_pItemRoot != pTBCI );
	pTBCI;
}

void CExtToolBoxWnd::OnToolBoxWndItemActivate(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_Old,
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_New
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	if( pTBCI_Old != NULL )
	{
		ASSERT( pTBCI_Old->ItemGetNext(__TBCGN_PARENT) == m_pItemRoot );
		ASSERT( pTBCI_Old->GetToolBoxWnd() == this );
		ASSERT( m_pItemRoot != pTBCI_Old );
	}
	if( pTBCI_New != NULL )
	{
		ASSERT( pTBCI_New->ItemGetNext(__TBCGN_PARENT) == m_pItemRoot );
		ASSERT( pTBCI_New->GetToolBoxWnd() == this );
		ASSERT( m_pItemRoot != pTBCI_New );
	}
	ASSERT( pTBCI_Old != pTBCI_New || pTBCI_New == NULL );
	pTBCI_Old;
	pTBCI_New;
}

void CExtToolBoxWnd::OnToolBoxWndItemSelChange(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_Old,
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_New
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
#ifdef _DEBUG
	if( pTBCI_Old != NULL )
	{
		ASSERT( pTBCI_Old->ItemGetNext(__TBCGN_PARENT)->ItemGetNext(__TBCGN_PARENT) == m_pItemRoot );
		ASSERT( pTBCI_Old->GetToolBoxWnd() == this );
		ASSERT( m_pItemRoot != pTBCI_Old );
	}
	if( pTBCI_New != NULL )
	{
		ASSERT( pTBCI_New->ItemGetNext(__TBCGN_PARENT)->ItemGetNext(__TBCGN_PARENT) == m_pItemRoot );
		ASSERT( pTBCI_New->GetToolBoxWnd() == this );
		ASSERT( m_pItemRoot != pTBCI_New );
	}
#endif // _DEBUG
	ASSERT( pTBCI_Old != pTBCI_New || pTBCI_New == NULL );
	pTBCI_Old;
	pTBCI_New;
}

void CExtToolBoxWnd::OnToolBoxWndItemHoverChange(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_Old,
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_New
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	ASSERT( m_pItemRoot != pTBCI_Old );
	ASSERT( m_pItemRoot != pTBCI_New );
	ASSERT( pTBCI_Old != pTBCI_New );
	pTBCI_Old;
	
bool bDeactivateContentExpandWnd = true;
bool bRemoveTip = false;
bool bTipCtrlValid = false;
	if(		(m_dwToolBoxWndStyle & __TBWS_TOOLTIPS_ALL) != 0
		&&	m_wndToolTip.GetSafeHwnd() != NULL
		&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
		)
		bTipCtrlValid = true;

bool bEnableTipsAndExpanding = OnToolBoxWndCanHandleHover();

	if( pTBCI_New != NULL && bEnableTipsAndExpanding )
	{
		CRect rcItemOrg = ItemGetRect( pTBCI_New );
		CRect rcItem( rcItemOrg );

		if( pTBCI_New->ItemGetNext(__TBCGN_PARENT) != ItemGetRoot() )
		{
			rcItem.left = max( rcItem.left, m_rcActiveChildArea.left );
			rcItem.top = max( rcItem.top, m_rcActiveChildArea.top );
			rcItem.right = min( rcItem.right, m_rcActiveChildArea.right );
			rcItem.bottom = min( rcItem.bottom, m_rcActiveChildArea.bottom );
		} // if( pTBCI_New->ItemGetNext(__TBCGN_PARENT) != ItemGetRoot() )

		if(		rcItem.right > rcItem.left
			&&	rcItem.bottom > rcItem.top
			)
		{ // if item rect valid
			bool bCanExpandItem = pTBCI_New->CanExpandItem();
			bool bCanShowToolTip = pTBCI_New->CanShowToolTip();
			if( bCanExpandItem )
			{
				bDeactivateContentExpandWnd = false;

				rcItem = rcItemOrg;
				LONG nNeededDX =
					pTBCI_New->GetLastMeasuredSize().cx
					;

				rcItem.right =
					rcItem.left
					+ nNeededDX
					;

				m_wndContentExpand.Activate(
					rcItem,
					this
					);
			} // if( bCanExpandItem )
			if( bTipCtrlValid )
			{
				if( (!bCanExpandItem) && bCanShowToolTip )
					m_wndToolTip.AddTool(
						this,
						pTBCI_New->TextGet(),
						&rcItem,
						1
						);
				else
					bRemoveTip = true;
			} // if( bTipCtrlValid )
		} // if item rect valid
		else
			bRemoveTip = 
				(bTipCtrlValid && (pTBCI_Old != NULL))
					? true
					: false
					;
	} // if( pTBCI_New != NULL && bEnableTipsAndExpanding )
	else
	{
		bRemoveTip = 
			(bTipCtrlValid && (pTBCI_Old != NULL))
				? true
				: false
				;
	} // else from if( pTBCI_New != NULL && bEnableTipsAndExpanding )

	if( bRemoveTip )
	{
		ASSERT( bTipCtrlValid );
		m_wndToolTip.DelTool( this, 1 );
		CWnd::CancelToolTips();
	}

	if( bDeactivateContentExpandWnd )
		m_wndContentExpand.Deactivate();

CFrameWnd * pParentFrame = GetParentFrame();
	if( pParentFrame != NULL
		&& pParentFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd))
		)
		SetTimer(
			__EXT_TOOLBOXWND_TIMER_ID_DELAY_UPDATE,
			1,
			NULL
			);
}

bool CExtToolBoxWnd::OnToolBoxWndCanHandleHover() const
{
	ASSERT_VALID( this );
	if( GetEditorHWND() != NULL )
		return false;

	return
		CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND(
			GetSafeHwnd()
			);
}

LONG CExtToolBoxWnd::OnToolBoxWndMeasureScrollStepSize(
	bool bByUpButton,
	LONG nScrollStepNo
	)
{
	ASSERT_VALID( this );
	nScrollStepNo;
const LONG nStepAbsSizeDefault = 5;
TOOLBOX_ITEM_DATA * pTBCI = ItemGetActive();
	if( pTBCI == NULL )
		return bByUpButton ? (-nStepAbsSizeDefault) : nStepAbsSizeDefault;
	pTBCI = pTBCI->ItemGetNext(__TBCGN_FIRST_CHILD);
	if( pTBCI == NULL )
		return bByUpButton ? (-nStepAbsSizeDefault) : nStepAbsSizeDefault;
LONG nMetric =
		pTBCI->GetLastMeasuredSize().cy;
	if( nMetric <= 0 )
		nMetric = __EXT_TOOLBOXWND_ITEM_MIN_DY_BOTTOMLEVEL;
	nMetric += __EXT_TOOLBOXWND_MARGIN_BOTTOM_BOTTOMLEVEL;
	return
		bByUpButton
			? (-nMetric)
			: nMetric;
}

UINT CExtToolBoxWnd::OnToolBoxWndMeasureScrollTimerPeriod(
	bool bByUpButton
	)
{
	ASSERT_VALID( this );
	bByUpButton;
const UINT nElapseDefault = 20;
TOOLBOX_ITEM_DATA * pTBCI = ItemGetActive();
	if( pTBCI == NULL )
		return nElapseDefault;
	pTBCI = pTBCI->ItemGetNext(__TBCGN_FIRST_CHILD);
	if( pTBCI == NULL )
		return nElapseDefault;
	return 50;
}

BOOL CExtToolBoxWnd::Create(
	CWnd * pParentWnd,
	const RECT & rcWnd, // = CRect( 0, 0, 0, 0 )
	UINT nDlgCtrlID, // = UINT( IDC_STATIC )
	DWORD dwWindowStyle, // = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS
	DWORD dwToolBoxWndStyle, // = __TBWS_DEF_STYLES
	CCreateContext * pContext // = NULL
	)
{
	if( !RegisterToolBoxWndClass() )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	m_bDirectCreateCall = true;
	m_dwToolBoxWndStyle = dwToolBoxWndStyle;
	if( ! CWnd::Create(
			__EXT_TOOLBOXWND_CLASS_NAME,
			NULL,
			dwWindowStyle,
			rcWnd,
			pParentWnd,
			nDlgCtrlID,
			pContext
			)
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}

	if( !_CreateHelper() )
	{
		ASSERT( FALSE );
		AfxThrowMemoryException();
	} // if( !Create() )

	return TRUE;
}

bool CExtToolBoxWnd::g_bToolBoxWndClassRegistered = false;

bool CExtToolBoxWnd::RegisterToolBoxWndClass()
{
	if( g_bToolBoxWndClassRegistered )
		return true;

WNDCLASS wndclass;
HINSTANCE hInst = AfxGetInstanceHandle();
	if( ! ::GetClassInfo(
			hInst,
			__EXT_TOOLBOXWND_CLASS_NAME,
			&wndclass
			)
		)
	{
		// otherwise we need to register a new class
		wndclass.style = CS_GLOBALCLASS|CS_DBLCLKS;
		wndclass.lpfnWndProc = ::DefWindowProc;
		wndclass.cbClsExtra = wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInst;
		wndclass.hIcon = NULL;
		wndclass.hCursor =
//			( g_hCursor != NULL )
//				? g_hCursor
//				:
				::LoadCursor(
					NULL, //hInst,
					IDC_ARROW
					)
				;
		ASSERT( wndclass.hCursor != NULL );
		wndclass.hbrBackground = NULL; 
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = __EXT_TOOLBOXWND_CLASS_NAME;
		if( !::AfxRegisterClass( &wndclass ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}

	g_bToolBoxWndClassRegistered = true;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CExtToolBoxWnd message handlers

void CExtToolBoxWnd::PreSubclassWindow() 
{
	CWnd::PreSubclassWindow();

	if( m_bDirectCreateCall )
		return;

DWORD dwStyle = ::GetWindowLong( m_hWnd, GWL_STYLE );
	m_dwToolBoxWndStyle = dwStyle & __EXTMFC_ALL_FORM_MOVABLE_WND_STYLES;
	::SetWindowLong( m_hWnd, GWL_STYLE, dwStyle & (~__EXTMFC_ALL_FORM_MOVABLE_WND_STYLES) );
	
	if( !_CreateHelper() )
	{
		ASSERT( FALSE );
		AfxThrowMemoryException();
	} // if( !Create() )
}

bool CExtToolBoxWnd::_CreateHelper()
{
	EnableToolTips( TRUE );

	if( !m_wndToolTip.Create(this) )
	{
		ASSERT( FALSE );
		return false;
	}
	m_wndToolTip.Activate( TRUE );

	UpdateToolBoxWnd( false );
	return true;
}

void CExtToolBoxWnd::PostNcDestroy() 
{
	if( m_hWndEditor != NULL )
	{
		OnToolBoxWndCancelItemEditor( m_hWndEditor );
		m_hWndEditor = NULL;
	}
	m_wndContentExpand.Deactivate();
	CWnd::PostNcDestroy();
}

LRESULT CExtToolBoxWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(		message == WM_NOTIFY
		&&	m_wndToolTip.GetSafeHwnd() != NULL
		&&	IsWindow( m_wndToolTip.GetSafeHwnd() )
		&&	((LPNMHDR)lParam) != NULL
		&&	((LPNMHDR)lParam)->hwndFrom == m_wndToolTip.GetSafeHwnd()
		&&	((LPNMHDR)lParam)->code == TTN_SHOW
		)
		::SetWindowPos(
			m_wndToolTip.GetSafeHwnd(),
			HWND_TOP,
			0,0,0,0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE
			);
	return CWnd::WindowProc(message, wParam, lParam);
}

BOOL CExtToolBoxWnd::OnEraseBkgnd(CDC* pDC) 
{
//	return CWnd::OnEraseBkgnd(pDC);
	pDC;
	return TRUE;
}

LRESULT CExtToolBoxWnd::_OnPaintExpanedItemContent(WPARAM wParam, LPARAM lParam)
{
	lParam;
CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA * p_picd =
		(CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA *)wParam;
	ASSERT( p_picd != NULL );
	ASSERT( p_picd->m_dc.GetSafeHdc() != NULL );
	if( m_pItemTrackHover == NULL )
		return 0;
	m_pItemTrackHover->DrawItem(
		p_picd->m_dc,
		p_picd->m_rcItem
		);
	return (!0);
}

void CExtToolBoxWnd::OnPaint() 
{
	ASSERT_VALID( this );

	// recalc delayed layout first
	_RecalcLayoutImpl( false );

CPaintDC dcPaint( this );
CRect rcClient;
	GetClientRect( &rcClient );
	if( rcClient.IsRectEmpty() )
		return;

CExtMemoryDC dc( &dcPaint, &rcClient );
	OnToolBoxWndDrawEntire(
		dc,
		rcClient,
		m_rcActiveChildArea,
		m_rgnActiveChildArea,
		m_rcActiveChildArea2,
		m_rgnActiveChildArea2
		);
}

BOOL CExtToolBoxWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( ( !RegisterToolBoxWndClass() )
		|| ( !CWnd::PreCreateWindow(cs) )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	
	cs.lpszClass = __EXT_TOOLBOXWND_CLASS_NAME;

	return TRUE;
	
//	return CWnd::PreCreateWindow(cs);
}

BOOL CExtToolBoxWnd::PreTranslateMessage(MSG* pMsg) 
{
	if(		(m_dwToolBoxWndStyle & __TBWS_TOOLTIPS_ALL) != 0
		&&	m_wndToolTip.GetSafeHwnd() != NULL
		)
		m_wndToolTip.RelayEvent( pMsg );

	if( pMsg->message == WM_MOUSEWHEEL )
	{
		if(	_ProcessMouseWheel(
				UINT( LOWORD(pMsg->wParam) ),
				short( HIWORD(pMsg->wParam) ),
				false
				)
			)
			return TRUE;
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CExtToolBoxWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
DWORD dwWndStyle = GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		return;

	SendMessage( WM_CANCELMODE );

	UpdateToolBoxWnd( true, true );
}

void CExtToolBoxWnd::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CWnd::OnWindowPosChanged(lpwndpos);
	
DWORD dwWndStyle = GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		return;

	SendMessage( WM_CANCELMODE );

	UpdateToolBoxWnd( true, true );
}

void CExtToolBoxWnd::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
	
DWORD dwWndStyle = GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		return;

	SendMessage( WM_CANCELMODE );

	UpdateToolBoxWnd( true );
}

CExtToolBoxWnd::TOOLBOX_ITEM_DATA * CExtToolBoxWnd::ItemInsert(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_Parent, // = NULL // default - root
	__EXT_MFC_SAFE_LPCTSTR sText, // = NULL
	HICON hIcon, // = NULL
	bool bCopyIcon, // = true
	LONG nIndexInsertBefore, // = -1 // default - append
	bool bUpdateCtrlNow // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	if( pTBCI_Parent == NULL )
		pTBCI_Parent = m_pItemRoot;
#ifdef _DEBUG
	else
	{
		ASSERT( pTBCI_Parent->m_pToolBoxWnd == this );
		// only 2 levels supported
		ASSERT(
			pTBCI_Parent->m_pItemParent == m_pItemRoot
			);
	}
#endif // _DEBUG
TOOLBOX_ITEM_DATA * pTBCI_New = new TOOLBOX_ITEM_DATA( this );
	pTBCI_New->IconSet( hIcon, bCopyIcon );
	pTBCI_New->TextSet( sText );
	pTBCI_New->m_pItemParent = pTBCI_Parent;

TOOLBOX_ITEM_DATA * pTBCI_InsertBefore = pTBCI_Parent->m_pItemChild;
	if( pTBCI_InsertBefore != NULL )
	{
		if( nIndexInsertBefore >= 0 )
		{
			pTBCI_InsertBefore =
				pTBCI_InsertBefore->ItemGetNext(
					__TBCGN_SIBLING, // __TBCGN_LAST_SIBLING,
					nIndexInsertBefore,
					false
					);
			if( pTBCI_InsertBefore == NULL )
			{
				pTBCI_InsertBefore =
					pTBCI_Parent->m_pItemChild->ItemGetNext(
						__TBCGN_LAST_SIBLING,
						1,
						false
						);
				ASSERT( pTBCI_InsertBefore != NULL );
				pTBCI_New->m_pItemPrev = pTBCI_InsertBefore;
				ASSERT( pTBCI_InsertBefore->m_pItemNext == NULL );
				pTBCI_InsertBefore->m_pItemNext = pTBCI_New;
			}
			else
			{
				pTBCI_New->m_pItemNext = pTBCI_InsertBefore;
				pTBCI_New->m_pItemPrev = pTBCI_InsertBefore->m_pItemPrev;
				if( pTBCI_New->m_pItemPrev != NULL )
					pTBCI_New->m_pItemPrev->m_pItemNext = pTBCI_New;
				pTBCI_InsertBefore->m_pItemPrev = pTBCI_New;
			}
		}
		else
		{
			pTBCI_InsertBefore =
				pTBCI_InsertBefore->ItemGetNext(
					__TBCGN_LAST_SIBLING,
					1,
					false
					);
			ASSERT( pTBCI_InsertBefore != NULL );
			pTBCI_New->m_pItemPrev = pTBCI_InsertBefore;
			ASSERT( pTBCI_InsertBefore->m_pItemNext == NULL );
			pTBCI_InsertBefore->m_pItemNext = pTBCI_New;
		}
		if( pTBCI_Parent->m_pItemChild == pTBCI_New->m_pItemNext )
			pTBCI_Parent->m_pItemChild = pTBCI_New;
	} // if( && pTBCI_InsertBefore != NULL )
	else
		pTBCI_Parent->m_pItemChild = pTBCI_New;

	OnToolBoxWndItemInsert( pTBCI_New );

	UpdateToolBoxWnd( bUpdateCtrlNow, true );
	
	return pTBCI_New;
}

void CExtToolBoxWnd::ItemRemove(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
	bool bChildrenOnly, // = false
	bool bUpdateCtrlNow // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( pTBCI != NULL );
	ASSERT( pTBCI->m_pToolBoxWnd == this );
	if( bChildrenOnly )
		pTBCI->DestroyChilds();
	else
		pTBCI->DestroyItem( true );
	UpdateToolBoxWnd( bUpdateCtrlNow, true );
}

void CExtToolBoxWnd::ItemRemoveAll(
	bool bUpdateCtrlNow // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	m_pItemActive = NULL;
	m_pItemRoot->DestroyChilds();
	UpdateToolBoxWnd( bUpdateCtrlNow );
}

CExtToolBoxWnd::TOOLBOX_ITEM_DATA * CExtToolBoxWnd::ItemGetRoot()
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	return m_pItemRoot;
}

const CExtToolBoxWnd::TOOLBOX_ITEM_DATA * CExtToolBoxWnd::ItemGetRoot() const
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	return m_pItemRoot;
}

CExtToolBoxWnd::TOOLBOX_ITEM_DATA * CExtToolBoxWnd::ItemGetActive()
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	return m_pItemActive;
}

const CExtToolBoxWnd::TOOLBOX_ITEM_DATA * CExtToolBoxWnd::ItemGetActive() const
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	return m_pItemActive;
}

CExtToolBoxWnd::TOOLBOX_ITEM_DATA * CExtToolBoxWnd::ActivateNextGroup(
	bool bPreviousGroup
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI = NULL;
	if( m_pItemActive == NULL )
	{
		pTBCI =
			m_pItemRoot->ItemGetNext(
				__TBCGN_FIRST_CHILD
				);
	} // if( m_pItemActive == NULL )
	else
	{
		pTBCI =
			m_pItemActive->ItemGetNext(
				__TBCGN_SIBLING,
				bPreviousGroup ? (-1) : 1
				);
	} // else from if( m_pItemActive == NULL )
	if( pTBCI == NULL )
		return NULL;
	SendMessage( WM_CANCELMODE );
	ItemSetActive( pTBCI, true, true );
	return m_pItemActive;
}

CExtToolBoxWnd::TOOLBOX_ITEM_DATA * CExtToolBoxWnd::ActivateFirstGroup(
	bool bPreviousGroup
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI =
		m_pItemRoot->ItemGetNext(
			bPreviousGroup
				? __TBCGN_FIRST_CHILD
				: __TBCGN_LAST_CHILD
			);
	if( pTBCI == NULL )
		return NULL;
	if( pTBCI == m_pItemActive )
		return pTBCI;
	SendMessage( WM_CANCELMODE );
	ItemSetActive( pTBCI, true, true );
	return m_pItemActive;
}

const CExtToolBoxWnd::TOOLBOX_ITEM_DATA * CExtToolBoxWnd::ItemHitTest(
	CPoint ptClient
	) const 
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	return
		( const_cast < CExtToolBoxWnd * > ( this ) )
			-> ItemHitTest( ptClient );
}

CExtToolBoxWnd::TOOLBOX_ITEM_DATA * CExtToolBoxWnd::ItemHitTest(
	CPoint ptClient
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );

	if( !m_rcBtnDown.IsRectEmpty()
		&& m_rcBtnDown.PtInRect(ptClient)
		)
		return NULL;
	if( !m_rcBtnUp.IsRectEmpty()
		&& m_rcBtnUp.PtInRect(ptClient)
		)
		return NULL;

CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_Active = NULL;
CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI = 
		m_pItemRoot->ItemGetNext( __TBCGN_FIRST_CHILD );
	for( ; pTBCI != NULL; pTBCI = pTBCI->ItemGetNext( __TBCGN_SIBLING, 1 ) )
	{
		CRect rcItem = ItemGetRect( pTBCI );
		if( rcItem.PtInRect(ptClient) )
			return pTBCI;
		if( pTBCI->GetItemStyle() & __TBWI_ACTIVE )
		{
			ASSERT( pTBCI_Active == NULL );
			pTBCI_Active = pTBCI;
		}
	}

	if( pTBCI_Active != NULL )
	{
		if( !m_rcActiveChildArea.PtInRect(ptClient) )
			return NULL;
		pTBCI =
			pTBCI_Active->ItemGetNext( __TBCGN_FIRST_CHILD );
		for( ; pTBCI != NULL; pTBCI = pTBCI->ItemGetNext( __TBCGN_SIBLING, 1 ) )
		{
			CRect rcItem = ItemGetRect( pTBCI );
			if( rcItem.PtInRect(ptClient) )
				return pTBCI;
		}
	}

	return NULL;
}

bool CExtToolBoxWnd::ItemSetActive(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
	bool bUpdateCtrlNow, // = false
	bool bEnsureVisibleSelection // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	ASSERT( pTBCI != NULL );
	ASSERT( pTBCI->GetToolBoxWnd() == this );

	if( m_hWndEditor != NULL )
	{
		OnToolBoxWndCancelItemEditor( m_hWndEditor );
		m_hWndEditor = NULL;
	}
	
	if( pTBCI == m_pItemRoot )
		return false;
CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_Parent =
		pTBCI->ItemGetNext(__TBCGN_PARENT);
	ASSERT( pTBCI_Parent != NULL );
	if( pTBCI_Parent != m_pItemRoot )
	{
		ASSERT( pTBCI_Parent->ItemGetNext(__TBCGN_PARENT) == m_pItemRoot );
		if(	!ItemSetActive( pTBCI_Parent, false, false ) )
		{
			ASSERT( FALSE );
			return false;
		}

		pTBCI->ModifyItemStyle( __TBWI_SELECTED );

		UpdateToolBoxWnd( bUpdateCtrlNow, bEnsureVisibleSelection );
		return true;
	}

CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_PrevActive = NULL;
	if( pTBCI != m_pItemActive )
	{
		if( m_pItemActive != NULL )
		{
			pTBCI_PrevActive = m_pItemActive;
			ASSERT( m_pItemActive->GetToolBoxWnd() == this );
			ASSERT( (m_pItemActive->GetItemStyle()&__TBWI_ACTIVE) != 0 );
			//m_pItemActive->ModifyItemStyle(0,__TBWI_ACTIVE);
		}
		m_pItemActive = NULL;
//		ASSERT( (pTBCI->GetItemStyle()&__TBWI_ACTIVE) == 0 );
		pTBCI->ModifyItemStyle(__TBWI_ACTIVE);
		m_pItemActive = pTBCI;
	}

	ASSERT( m_nExpandAnimationPercent < 0 );
	if( bUpdateCtrlNow && pTBCI_PrevActive != NULL )
	{
		ASSERT( m_pItemActive == pTBCI );
		ASSERT( pTBCI_PrevActive != m_pItemActive );
		pTBCI_PrevActive->ModifyItemStyle( __TBWI_ACTIVE2 );
		clock_t nLastAnimTime = clock();
		m_nScrollPos2 = m_nScrollPos;
		m_nScrollSize2 = m_nScrollSize;
		for(	m_nExpandAnimationPercent = 0;
				m_nExpandAnimationPercent < 100;
				)
		{ // expand animation loop
			
			UpdateToolBoxWnd( true, bEnsureVisibleSelection );
			
			clock_t nNewAnimTime = clock();
			clock_t nDuration =
				nLastAnimTime - nNewAnimTime;
			if( nDuration < __EXT_TOOLBOXWND_EXPAND_STEP_TIME )
				::Sleep( __EXT_TOOLBOXWND_EXPAND_STEP_TIME - nDuration );
			nLastAnimTime = clock();
			
			m_nExpandAnimationPercent +=
				( m_nExpandAnimationPercent < __EXT_TOOLBOXWND_EXPAND_STEP_PERCENT_SW )
					? __EXT_TOOLBOXWND_EXPAND_STEP_PERCENT_1
					: __EXT_TOOLBOXWND_EXPAND_STEP_PERCENT_2
					;
		} // expand animation loop
		
		m_nExpandAnimationPercent = -1L;
		pTBCI_PrevActive->ModifyItemStyle( 0, __TBWI_ACTIVE2 );
	} // if( bUpdateCtrlNow && pTBCI_PrevActive != NULL )

//	else
	{
		UpdateToolBoxWnd( bUpdateCtrlNow, bEnsureVisibleSelection );
	} // else from if( bUpdateCtrlNow && pTBCI_PrevActive != NULL )

	return true;
}

void CExtToolBoxWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
//	CWnd::OnLButtonDblClk(nFlags, point);
	nFlags;

CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI =
		ItemHitTest( point );

	if( pTBCI == NULL )
		return;

	SendMessage( WM_CANCELMODE );
	ASSERT( m_hWndEditor == NULL );
	m_hWndEditor = OnToolBoxWndStartItemEditor( pTBCI );
}

void CExtToolBoxWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
//	CWnd::OnLButtonDown(nFlags, point);
	nFlags;

	if( GetFocus() != this )
		SetFocus();

	if( m_hWndEditor != NULL )
	{
		OnToolBoxWndCancelItemEditor( m_hWndEditor );
		m_hWndEditor = NULL;
	}

	m_ptStartLeftBtnTrack.x = m_ptStartLeftBtnTrack.y = -1;

CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI =
		ItemHitTest( point );
	if( pTBCI != NULL )
	{
		if( m_pItemTrackPressed != NULL )
			m_pItemTrackPressed->ModifyItemStyle( 0, __TBWI_PRESSED );
		else
		{
			DWORD dwHitStyle =
				pTBCI->GetItemStyle();
			if( dwHitStyle & (__TBWI_ACTIVE|__TBWI_SELECTED) )
			{
				SendMessage( WM_CANCELMODE );
				ASSERT( m_hWndEditor == NULL );
				m_hWndEditor = OnToolBoxWndStartItemEditor( pTBCI );
//				return; // commented to enable start drag
			} // if( dwHitStyle & (__TBWI_ACTIVE|__TBWI_SELECTED) )
		}
		m_ptStartLeftBtnTrack = point;
		m_pItemTrackPressed = pTBCI;
		m_pItemTrackPressed->ModifyItemStyle( __TBWI_PRESSED );
		CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
		UpdateToolBoxWnd( true );
		return;
	} // if( pTBCI != NULL )

	if(		!m_rcBtnDown.IsRectEmpty()
		&&	m_rcBtnDown.PtInRect( point )
		&& _IsScrollEnabled( false )
		)
	{ // start scroll by down button
		m_nScrollStepNo = 0L;
		m_bTrackingBtnDown = m_bPushedBtnDown = true;
		UpdateToolBoxWnd( true );
		CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
		UINT nElapse =
			OnToolBoxWndMeasureScrollTimerPeriod( false );
		ASSERT( nElapse > 0 );
		SetTimer(
			__EXT_TOOLBOXWND_TIMER_ID_SCROLL,
			nElapse,
			NULL
			);
		return;
	} // start scroll by down button

	if(		!m_rcBtnUp.IsRectEmpty()
		&&	m_rcBtnUp.PtInRect( point )
		&& _IsScrollEnabled( true )
		)
	{ // start scroll by up button
		m_nScrollStepNo = 0L;
		m_bTrackingBtnUp = m_bPushedBtnUp = true;
		UpdateToolBoxWnd( true );
		CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
		UINT nElapse =
			OnToolBoxWndMeasureScrollTimerPeriod( true );
		ASSERT( nElapse > 0 );
		SetTimer(
			__EXT_TOOLBOXWND_TIMER_ID_SCROLL,
			nElapse,
			NULL
			);
		return;
	} // start scroll by up button
}

void CExtToolBoxWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
//	CWnd::OnLButtonUp(nFlags, point);
	nFlags;

CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_Hit =
		ItemHitTest( point );
	
	if( m_pItemTrackPressed != NULL )
	{
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI =
			(pTBCI_Hit == m_pItemTrackPressed)
				? pTBCI_Hit : NULL;

		m_pItemTrackPressed->ModifyItemStyle( 0, __TBWI_PRESSED );
		m_pItemTrackPressed = NULL;
		
		if( pTBCI != NULL )
		{
			CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_Parent =
				pTBCI->ItemGetNext(__TBCGN_PARENT);
			ASSERT( pTBCI_Parent != NULL );
			ItemSetActive( pTBCI, true, true );
		} // if( pTBCI != NULL )
		else
			UpdateToolBoxWnd( true );

		if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
			CExtMouseCaptureSink::ReleaseCapture();

		m_ptStartLeftBtnTrack.x = m_ptStartLeftBtnTrack.y = -1;
		return;
	} // if( m_pItemTrackPressed != NULL )

	if( m_bTrackingBtnDown )
	{
		ASSERT( !m_bTrackingBtnUp );
		KillTimer( __EXT_TOOLBOXWND_TIMER_ID_SCROLL );
		m_nScrollStepNo = 0L;
		m_bTrackingBtnDown = m_bPushedBtnDown = false;
		UpdateToolBoxWnd( true );
		if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
			CExtMouseCaptureSink::ReleaseCapture();
		return;
	} // if( m_bTrackingBtnDown )

	if( m_bTrackingBtnUp )
	{
		ASSERT( !m_bTrackingBtnDown );
		KillTimer( __EXT_TOOLBOXWND_TIMER_ID_SCROLL );
		m_nScrollStepNo = 0L;
		m_bTrackingBtnUp = m_bPushedBtnUp = false;
		UpdateToolBoxWnd( true );
		if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
			CExtMouseCaptureSink::ReleaseCapture();
		return;
	} // if( m_bTrackingBtnUp )

	if( pTBCI_Hit != NULL && m_pItemActive2 == NULL )
	{
		DWORD dwHitStyle =
			pTBCI_Hit->GetItemStyle();
		if( dwHitStyle & (__TBWI_ACTIVE|__TBWI_SELECTED) )
		{
			SendMessage( WM_CANCELMODE );
			ASSERT( m_hWndEditor == NULL );
			m_hWndEditor = OnToolBoxWndStartItemEditor( pTBCI_Hit );
		} // if( dwHitStyle & (__TBWI_ACTIVE|__TBWI_SELECTED) )
	} // if( pTBCI_Hit != NULL && m_pItemActive2 == NULL )
}

void CExtToolBoxWnd::OnCancelMode() 
{
	CWnd::OnCancelMode();

	if( m_hWndEditor != NULL )
	{
		OnToolBoxWndCancelItemEditor( m_hWndEditor );
		m_hWndEditor = NULL;
	}
	
bool bUpdateCtrl = false;
bool bReleaseCapture = false;
	if( m_pItemTrackHover != NULL )
	{
		bUpdateCtrl = true;
		m_pItemTrackHover->ModifyItemStyle( 0, __TBWI_HOVER );
		m_pItemTrackHover = NULL;
	} // if( m_pItemTrackHover != NULL )
	if( m_pItemTrackPressed != NULL
		)
	{
		bUpdateCtrl = true;
		bReleaseCapture = true;
		m_pItemTrackPressed->ModifyItemStyle( 0, __TBWI_PRESSED );
		m_pItemTrackPressed = NULL;
	} // if( m_pItemTrackPressed != NULL )

	if( m_bTrackingBtnUp || m_bTrackingBtnDown )
	{
		bUpdateCtrl = true;
		bReleaseCapture = true;
		m_nScrollStepNo = 0L;
		m_bTrackingBtnUp = m_bTrackingBtnDown =
			m_bPushedBtnUp = m_bPushedBtnDown = false;
		KillTimer( __EXT_TOOLBOXWND_TIMER_ID_SCROLL );
	} // if( m_bTrackingBtnUp || m_bTrackingBtnDown )

	if(		(m_dwToolBoxWndStyle & __TBWS_TOOLTIPS_ALL) != 0
		&&	m_wndToolTip.GetSafeHwnd() != NULL
		&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
		)
		m_wndToolTip.DelTool( this, 1 );
	CWnd::CancelToolTips();

	m_wndContentExpand.Deactivate();

	m_ptStartLeftBtnTrack.x = m_ptStartLeftBtnTrack.y = -1;

	if( bUpdateCtrl )
	{
		if(		bReleaseCapture
			&&	CExtMouseCaptureSink::GetCapture() == GetSafeHwnd()
			)
			CExtMouseCaptureSink::ReleaseCapture();
		UpdateToolBoxWnd( false );
		SetTimer(
			__EXT_TOOLBOXWND_TIMER_ID_DELAY_UPDATE,
			1,
			NULL
			);
	}
}

void CExtToolBoxWnd::OnCaptureChanged(CWnd *pWnd) 
{
	CWnd::OnCaptureChanged(pWnd);
	if(		m_pItemTrackPressed != NULL
		||	m_pItemTrackHover != NULL
		||	m_bTrackingBtnUp
		||	m_bTrackingBtnDown
		)
		SendMessage( WM_CANCELMODE );
}

#if _MFC_VER < 0x700
void CExtToolBoxWnd::OnActivateApp(BOOL bActive, HTASK hTask) 
#else
void CExtToolBoxWnd::OnActivateApp(BOOL bActive, DWORD hTask) 
#endif
{
	CWnd::OnActivateApp(bActive, hTask);
	
	if( !bActive )
		SendMessage( WM_CANCELMODE );
	
}

void CExtToolBoxWnd::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
	if( pNewWnd != NULL )
	{
		if( ::IsChild( m_hWnd, pNewWnd->GetSafeHwnd() ) )
			return;
		if(		m_hWndEditor != NULL
			&&	::IsWindow( m_hWndEditor )
			)
		{
			if( ::IsChild( m_hWndEditor, pNewWnd->GetSafeHwnd() ) )
				return;
		}
	}

	if(		m_pItemTrackPressed != NULL
		||	m_pItemTrackHover != NULL
		||	m_bTrackingBtnUp
		||	m_bTrackingBtnDown
		)
		SendMessage( WM_CANCELMODE );
}

void CExtToolBoxWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
//	CWnd::OnMouseMove(nFlags, point);

	if( m_nExpandAnimationPercent >= 0 )
		return;

	if( m_pItemTrackPressed != NULL )
	{
		if(	OnToolBoxWndCanDoItemDrag(
				m_pItemTrackPressed,
				m_ptStartLeftBtnTrack,
				point,
				nFlags
				)
			)
		{
			TOOLBOX_ITEM_DATA * pTBCI = m_pItemTrackPressed;
			CPoint ptStart( m_ptStartLeftBtnTrack );
			m_ptStartLeftBtnTrack.x = m_ptStartLeftBtnTrack.y = -1;
			SendMessage( WM_CANCELMODE );
			OnToolBoxWndStartItemDrag(
				pTBCI,
				ptStart,
				point,
				nFlags
				);
		}
		return;
	}
	if( m_bTrackingBtnUp || m_bTrackingBtnDown )
		return;

bool bEnableTipsAndExpanding = OnToolBoxWndCanHandleHover();
	if( !bEnableTipsAndExpanding )
		return;

TOOLBOX_ITEM_DATA * pTBCI = ItemHitTest( point );
	if( pTBCI != m_pItemTrackHover )
	{
		if( pTBCI == NULL )
			SendMessage( WM_CANCELMODE );
		else
		{
			bool bSetTimer =
				(m_pItemTrackHover == NULL)
					? true : false;
			pTBCI->ModifyItemStyle( __TBWI_HOVER );
			ASSERT( m_pItemTrackHover == pTBCI );
			UpdateToolBoxWnd( true );
			if( bSetTimer )
			{
				SetTimer(
					__EXT_TOOLBOXWND_TIMER_ID_HOVER,
					50,
					NULL
					);
			} // if( bSetTimer )
		} // else from if( pTBCI == NULL )
	} // if( pTBCI != m_pItemTrackHover )
}

bool CExtToolBoxWnd::_ProcessMouseWheel( UINT nFlags, short zDelta, bool bRealOwnMsg )
{
	ASSERT_VALID( this );
	if(		GetSafeHwnd() == NULL
		||	( ! ::IsWindow(GetSafeHwnd()) )
		||	(GetStyle() & WS_VISIBLE) == 0
		||	( ! IsWindowEnabled() )
		)
		return false;

	if( !bRealOwnMsg )
	{
		CPoint ptCursor;
		if( ! ::GetCursorPos(&ptCursor) )
			return false;
		HWND hWndFromPoint = ::WindowFromPoint( ptCursor );
		if( hWndFromPoint == NULL )
			return false;
		if( hWndFromPoint != m_hWnd )
		{
			HWND hWndContentExpand = m_wndContentExpand.GetSafeHwnd();
			if( hWndFromPoint != hWndContentExpand )
				return false;
		}
	}

	if( nFlags & MK_CONTROL )
	{
		ActivateNextGroup(
			( zDelta > 0 ) ? true :  false
			);
		return true;
	}

	if( _IsScrollEnabled( true )
		&& zDelta > 0
		)
	{
		LONG nScrollStepSize = 
			OnToolBoxWndMeasureScrollStepSize(
				true,
				10
				);
		if( nScrollStepSize != 0 )
		{
			SendMessage( WM_CANCELMODE );
			LONG nScrollPos = m_nScrollPos + nScrollStepSize;
			if( nScrollPos < 0 )
				nScrollPos = 0;
			else if( nScrollPos > m_nScrollSize )
				nScrollPos = m_nScrollSize;
			if( nScrollPos != m_nScrollPos )
			{
				m_nScrollPos = nScrollPos;
				UpdateToolBoxWnd( true );
			} // if( nScrollPos != m_nScrollPos )
		} // if( nScrollStepSize != 0 )
		return true;
	}

	if( _IsScrollEnabled( false )
		&& zDelta < 0
		)
	{
		LONG nScrollStepSize = 
			OnToolBoxWndMeasureScrollStepSize(
				false,
				10
				);
		if( nScrollStepSize != 0 )
		{
			SendMessage( WM_CANCELMODE );
			LONG nScrollPos = m_nScrollPos + nScrollStepSize;
			if( nScrollPos < 0 )
				nScrollPos = 0;
			else if( nScrollPos > m_nScrollSize )
				nScrollPos = m_nScrollSize;
			if( nScrollPos != m_nScrollPos )
			{
				m_nScrollPos = nScrollPos;
				UpdateToolBoxWnd( true );
			} // if( nScrollPos != m_nScrollPos )
		} // if( nScrollStepSize != 0 )
		return true;
	}
	
	return true;
}

BOOL CExtToolBoxWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	pt;
//	return CWnd::OnMouseWheel(nFlags, zDelta, pt);

	return _ProcessMouseWheel( nFlags, zDelta, true ) ? TRUE : FALSE;
}

void CExtToolBoxWnd::OnTimer(UINT nIDEvent) 
{
	switch( nIDEvent )
	{
	case __EXT_TOOLBOXWND_TIMER_ID_HOVER:
	{
		if( m_pItemTrackHover == NULL )
		{
			KillTimer( __EXT_TOOLBOXWND_TIMER_ID_HOVER );
			return;
		}
		if(		m_pItemTrackPressed != NULL
			||	m_bTrackingBtnUp
			||	m_bTrackingBtnDown
			)
		{
			return;
		}
		CRect rcClient;
		GetClientRect( &rcClient );
		CPoint point;
		if( ! ::GetCursorPos(&point) )
			return;
		ScreenToClient( &point );
		if( !rcClient.PtInRect( point ) )
		{
			KillTimer( __EXT_TOOLBOXWND_TIMER_ID_HOVER );
			SendMessage( WM_CANCELMODE );
			return;
		}
		TOOLBOX_ITEM_DATA * pTBCI = ItemHitTest( point );
		if( pTBCI != m_pItemTrackHover )
		{
			KillTimer( __EXT_TOOLBOXWND_TIMER_ID_HOVER );
			SendMessage( WM_CANCELMODE );
			return;
		}
	}
	return; // __EXT_TOOLBOXWND_TIMER_ID_HOVER
	case __EXT_TOOLBOXWND_TIMER_ID_SCROLL:
	{
		if( !(m_bTrackingBtnUp || m_bTrackingBtnDown) )
		{
			KillTimer( __EXT_TOOLBOXWND_TIMER_ID_SCROLL );
			return;
		}
		//if( !(m_bPushedBtnUp || m_bPushedBtnDown) )
		//	return;
		if( m_rcBtnDown.IsRectEmpty()
			|| m_rcBtnUp.IsRectEmpty()
			)
		{
			KillTimer( __EXT_TOOLBOXWND_TIMER_ID_SCROLL );
			SendMessage( WM_CANCELMODE );
			return;
		}
		CPoint point;
		if( ! ::GetCursorPos(&point) )
			return;
		ScreenToClient( &point );
		if( m_bTrackingBtnUp )
		{
			ASSERT( !m_bTrackingBtnDown );
			if( !m_rcBtnUp.PtInRect(point) )
			{
				if( m_bPushedBtnUp )
				{
					m_nScrollStepNo = 0L;
					m_bPushedBtnUp = false;
					UpdateToolBoxWnd(true);
				} // if( m_bPushedBtnUp )
			} // if( !m_rcBtnUp.PtInRect(point) )
			else
			{
				if( !m_bPushedBtnUp )
				{
					m_bPushedBtnUp = true;
					UpdateToolBoxWnd(true);
				} // if( !m_bPushedBtnUp )
				else
				{
					LONG nScrollStepSize = 
						OnToolBoxWndMeasureScrollStepSize(
							true,
							m_nScrollStepNo++
							);
					if( nScrollStepSize != 0 )
					{
						LONG nScrollPos = m_nScrollPos + nScrollStepSize;
						if( nScrollPos < 0 )
							nScrollPos = 0;
						else if( nScrollPos > m_nScrollSize )
							nScrollPos = m_nScrollSize;
						if( nScrollPos != m_nScrollPos )
						{
							m_nScrollPos = nScrollPos;
							UpdateToolBoxWnd( true );
						} // if( nScrollPos != m_nScrollPos )
					} // if( nScrollStepSize != 0 )
				} // else from if( !m_bPushedBtnUp )
			} // else from if( !m_rcBtnUp.PtInRect(point) )
		} // if( m_bTrackingBtnUp )
		else
		{
			ASSERT( m_bTrackingBtnDown );
			if( !m_rcBtnDown.PtInRect(point) )
			{
				if( m_bPushedBtnDown )
				{
					m_nScrollStepNo = 0L;
					m_bPushedBtnDown = false;
					UpdateToolBoxWnd(true);
				} // if( m_bPushedBtnDown )
			} // if( !m_rcBtnDown.PtInRect(point) )
			else
			{
				if( !m_bPushedBtnDown )
				{
					m_bPushedBtnDown = true;
					UpdateToolBoxWnd(true);
				} // if( !m_bPushedBtnDown )
				else
				{
					LONG nScrollStepSize = 
						OnToolBoxWndMeasureScrollStepSize(
							false,
							m_nScrollStepNo++
							);
					if( nScrollStepSize != 0 )
					{
						LONG nScrollPos = m_nScrollPos + nScrollStepSize;
						if( nScrollPos < 0 )
							nScrollPos = 0;
						else if( nScrollPos > m_nScrollSize )
							nScrollPos = m_nScrollSize;
						if( nScrollPos != m_nScrollPos )
						{
							m_nScrollPos = nScrollPos;
							UpdateToolBoxWnd( true );
						} // if( nScrollPos != m_nScrollPos )
					} // if( nScrollStepSize != 0 )
				} // else from if( !m_bPushedBtnDown )
			} // else from if( !m_rcBtnDown.PtInRect(point) )
		} // else from if( m_bTrackingBtnUp )
	}
	return; // __EXT_TOOLBOXWND_TIMER_ID_SCROLL
	case __EXT_TOOLBOXWND_TIMER_ID_DELAY_UPDATE:
		KillTimer( __EXT_TOOLBOXWND_TIMER_ID_DELAY_UPDATE );
		//UpdateToolBoxWnd( true, false );
		//Invalidate();
		//UpdateWindow();
		RedrawWindow(
			NULL,
			NULL,
			RDW_INVALIDATE|RDW_UPDATENOW|RDW_FRAME
			);
		return;
	} // switch( nIDEvent )
	
	CWnd::OnTimer(nIDEvent);
}

UINT CExtToolBoxWnd::OnGetDlgCode() 
{
	//return CWnd::OnGetDlgCode();
	return DLGC_WANTARROWS;
}

void CExtToolBoxWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	nRepCnt;
	nFlags;
//	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
	switch( nChar )
	{
	case VK_UP:
	{
		if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
			ActivateNextGroup( true );
		else
			ActivateNextItem( true, true, false, false );
	}
	break;  // case VK_UP
	case VK_DOWN:
	{
		if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
			ActivateNextGroup( false );
		else
			ActivateNextItem( false, true, false, false );
	}
	break; // case VK_DOWN
	case VK_LEFT:
	{
		ActivateNextItem( true, false, false, true );
	}
	break; // case VK_LEFT
	case VK_RIGHT:
	{
		ActivateNextItem( false, false, false, true );
	}
	break; // case VK_RIGHT
	case VK_PRIOR:
	{
		ActivateNextPage( true );
	}
	break; // case VK_PRIOR
	case VK_NEXT:
	{
		ActivateNextPage( false );
	}
	break; // case VK_NEXT
	case VK_HOME:
	{
		if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
			ActivateFirstGroup( true );
		else
			ActivateFirstItem( true );
	}
	break; // case VK_HOME
	case VK_END:
	{
		if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
			ActivateFirstGroup( false );
		else
			ActivateFirstItem( false );
	}
	break; // case VK_END
	case VK_TAB:
	{
		if( (GetStyle() & WS_POPUP) != 0 )
			break;
		CWnd * pWndParent = GetParent();
		if( pWndParent == NULL )
			break;
		SendMessage( WM_CANCELMODE );
		pWndParent->SendMessage(
			WM_NEXTDLGCTL,
			CExtPopupMenuWnd::IsKeyPressed(VK_SHIFT),
			0
			);
	}
	break; // case VK_TAB
	} // switch( nChar )
	
}

CExtToolBoxWnd::TOOLBOX_ITEM_DATA * CExtToolBoxWnd::ActivateNextItem(
	bool bPreviousItem,
	bool bJumpRow,
	bool bSkipInIconViewMode,
	bool bSkipInListViewMode
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	if( bSkipInIconViewMode && bSkipInListViewMode )
		return NULL;
	if( m_pItemActive == NULL )
		ActivateNextGroup( false );
	if( m_pItemActive == NULL )
		return NULL;
CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI =
		m_pItemActive->FindSelectedChild();
	if( pTBCI == NULL )
		pTBCI = m_pItemActive->ItemGetNext( __TBCGN_FIRST_CHILD );
	if( pTBCI == NULL )
		return NULL;

bool bListViewMode =
		(m_pItemActive->GetItemStyle() & __TBWI_LISTBOX_VIEW)
			? true : false;
	if( bListViewMode )
	{
		if( bSkipInListViewMode )
			return NULL;
		bJumpRow = true;
	} // if( bListViewMode )
	else
	{
		if( bSkipInIconViewMode )
			return NULL;
	} // else from if( bListViewMode )

	if( bJumpRow )
	{
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI2 =
			pTBCI->ItemGetNext(
				bListViewMode
					? __TBCGN_SIBLING
					: __TBCGN_SIBLING_NR,
				bPreviousItem ? (-1) : 1
				);
		if( pTBCI2 == NULL )
			return pTBCI;
		pTBCI = pTBCI2;
	} // if( bJumpRow )
	else
	{
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI2 =
			pTBCI->ItemGetNext(
				__TBCGN_SIBLING,
				bPreviousItem ? (-1) : 1
				);
		if( pTBCI2 == NULL )
			return pTBCI;
		pTBCI = pTBCI2;
	} // else from if( bJumpRow )

	pTBCI->ModifyItemStyle( __TBWI_SELECTED );

	UpdateToolBoxWnd( true, true );
	return pTBCI;
}

CExtToolBoxWnd::TOOLBOX_ITEM_DATA * CExtToolBoxWnd::ActivateFirstItem(
	bool bPreviousItem
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	if( m_pItemActive == NULL )
		ActivateNextGroup( false );
	if( m_pItemActive == NULL )
		return NULL;
CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI =
		m_pItemActive->ItemGetNext(
			bPreviousItem
				? __TBCGN_FIRST_CHILD
				: __TBCGN_LAST_CHILD
			);
	if( pTBCI == NULL )
		return NULL;
	if( pTBCI->GetItemStyle() & __TBWI_SELECTED )
		return pTBCI;

	if( !ItemSetActive(
			pTBCI,
			true,
			true
			)
		)
		return NULL;
//	pTBCI->ModifyItemStyle( __TBWI_SELECTED );
//	if( bPreviousItem )
//		m_nScrollPos = 0;
//	else
//		m_nScrollPos = m_nScrollSize;
//	UpdateToolBoxWnd( true, false );

	return pTBCI;
}

CExtToolBoxWnd::TOOLBOX_ITEM_DATA * CExtToolBoxWnd::ActivateNextPage(
	bool bPreviousPage
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pItemRoot != NULL );
	if( m_pItemActive == NULL )
		ActivateNextGroup( false );
	if( m_pItemActive == NULL )
		return NULL;
CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI =
		m_pItemActive->FindSelectedChild();
	if( pTBCI == NULL )
		pTBCI = m_pItemActive->ItemGetNext( __TBCGN_FIRST_CHILD );
	if( pTBCI == NULL )
		return NULL;
	
	pTBCI =
		pTBCI->ItemGetNext(
			bPreviousPage
				? __TBCGN_PAGE_UP
				: __TBCGN_PAGE_DOWN
			);
	ASSERT( pTBCI != NULL );

	pTBCI->ModifyItemStyle( __TBWI_SELECTED );
	UpdateToolBoxWnd( true, true );
	return pTBCI;
}

HWND CExtToolBoxWnd::OnToolBoxWndStartItemEditor(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI
	)
{
	ASSERT_VALID( this );
	ASSERT( pTBCI != NULL );

DWORD dwItemStyle =
		pTBCI->GetItemStyle();
	if( (dwItemStyle & __TBWI_EDITABLE) == 0 )
		return NULL;
	if( (pTBCI->GetItemStyle() & __TBWI_VISIBLE) == 0 )
		return false;
	
CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_Parent =
		pTBCI->ItemGetNext( __TBCGN_PARENT );
	ASSERT( pTBCI_Parent != NULL );
	if(		pTBCI_Parent != ItemGetRoot()
		&&	pTBCI_Parent != ItemGetActive()
		)
	{
		// not a group item and not a child of
		// an active group
		return NULL;
	}

	if( pTBCI_Parent == ItemGetActive() )
	{
		DWORD dwParentStyle =
			pTBCI_Parent->GetItemStyle();
		if( (dwParentStyle & __TBWI_LISTBOX_VIEW) == 0 )
			return NULL; // do not edit items in the icon view
	}

CRect rcItem = ItemGetRect( pTBCI );
	rcItem.DeflateRect( 3, 2 );
CSize sizeIcon( 0, 0 );
HICON hItemIcon = pTBCI->IconGet( &sizeIcon );
	if( hItemIcon != NULL && sizeIcon.cx > 0 )
		rcItem.left +=
			sizeIcon.cx
			+ __EXT_TOOLBOXWND_ICON2TEXT_DX
			+ __EXT_TOOLBOXWND_BTN2TEXT_DX
			- 2
			;

CRect rcClient;
	GetClientRect( &rcClient );
	if( rcItem.top < rcClient.top )
		rcItem.OffsetRect(
			0,
			rcClient.top - rcItem.top
			);
	if( rcItem.bottom > rcClient.bottom )
		rcItem.OffsetRect(
			0,
			-(rcItem.bottom - rcClient.bottom)
			);
	if( rcItem.top < rcClient.top )
		rcItem.top = rcClient.top;
	if( rcItem.left < rcClient.left )
		rcItem.left = rcClient.left;
	if( rcItem.right > rcClient.right )
		rcItem.right = rcClient.right;
	if( (rcItem.right - rcItem.left) < __EXT_TOOLBOXWND_MIN_EDITOR_DX )
		return false;
	if( (rcItem.bottom - rcItem.top) < __EXT_TOOLBOXWND_MIN_EDITOR_DY )
		return false;

CItemTextEditWnd * pWndEdit =
		new CItemTextEditWnd( pTBCI );
	if( !pWndEdit->Create(
			WS_CHILD|ES_AUTOHSCROLL,
			rcItem,
			this,
			(UINT)IDC_STATIC
			)
		)
		return NULL;

HWND hWndEditor = pWndEdit->GetSafeHwnd();

CFont * pItemFont = pTBCI->ItemGetFont();
	if( pItemFont != NULL )
		pWndEdit->SetFont( pItemFont );
	else
		::SendMessage(
			hWndEditor,
			WM_SETFONT,
			(WPARAM)::GetStockObject( DEFAULT_GUI_FONT ),
			(LPARAM)TRUE
			);
__EXT_MFC_SAFE_LPCTSTR sItemText = pTBCI->TextGet();
	if( sItemText == NULL )
		sItemText = _T("");
	pWndEdit->SetWindowText( sItemText );
	pWndEdit->ShowWindow( SW_SHOW );
	pWndEdit->SetFocus();

	return hWndEditor;
}

void CExtToolBoxWnd::OnToolBoxWndCancelItemEditor(
	HWND hWndEditor
	)
{
	ASSERT_VALID( this );
	ASSERT( hWndEditor != NULL );
	if( !::IsWindow(hWndEditor) )
		return;
	::DestroyWindow( hWndEditor );
}

bool CExtToolBoxWnd::OnToolBoxWndDeactivateEditor(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
	bool bUpdateDeactivation
	)
{
	ASSERT_VALID( this );
	ASSERT( pTBCI != NULL );
	pTBCI;
	bUpdateDeactivation;
	if( m_hWndEditor != NULL )
		::PostMessage(
			m_hWndEditor,
			WM_CLOSE,
			0,
			0
			);
	m_hWndEditor = NULL;
	return true;
}

void CExtToolBoxWnd::OnToolBoxWndDoItemTextChanging(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
	CExtSafeString & sItemText,
	__EXT_MFC_SAFE_LPCTSTR sNewText
	)
{
	ASSERT_VALID( this );
	ASSERT( pTBCI != NULL );
	pTBCI;
	sItemText = (sNewText == NULL) ?  _T("") : sNewText;
}

bool CExtToolBoxWnd::ItemBeginEdit(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI
	)
{
	ASSERT_VALID( this );
	if( pTBCI == NULL
		|| pTBCI == m_pItemRoot
		)
		return false;
	ASSERT( pTBCI->GetToolBoxWnd() == this );
	if( (GetStyle() & WS_VISIBLE) == 0 )
		return false;
	SendMessage( WM_CANCELMODE );
	if( GetFocus() != this )
		SetFocus();
	if( pTBCI->ItemGetNext(__TBCGN_PARENT) != m_pItemRoot )
	{
		ASSERT( pTBCI->ItemGetNext(__TBCGN_PARENT)->ItemGetNext(__TBCGN_PARENT) == m_pItemRoot );
		if( (pTBCI->GetItemStyle() & __TBWI_SELECTED) == 0 )
			pTBCI->ModifyItemStyle(__TBWI_SELECTED);
		UpdateToolBoxWnd( true, true );
	}
	else
	{
		if( m_bDelayRecalcLayout )
			UpdateToolBoxWnd( true, false );
	}
	ASSERT( m_hWndEditor == NULL );
	m_hWndEditor = OnToolBoxWndStartItemEditor( pTBCI );
	if( m_hWndEditor == NULL )
		return false;
	return true;
}

void CExtToolBoxWnd::OnToolBoxWndStartItemDrag(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
	CPoint ptClientStart,
	CPoint ptClientCurrent,
	UINT nMouseMoveFlags
	)
{
	ASSERT_VALID( this );
	ASSERT( pTBCI != NULL );
	ASSERT( pTBCI != m_pItemRoot );
	ASSERT( pTBCI->m_pItemParent != NULL );
	pTBCI;
	ptClientStart;
	ptClientCurrent;
	nMouseMoveFlags;
}

bool CExtToolBoxWnd::OnToolBoxWndCanDoItemDrag(
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
	CPoint ptClientStart,
	CPoint ptClientCurrent,
	UINT nMouseMoveFlags
	)
{
	ASSERT_VALID( this );
	ASSERT( pTBCI != NULL );
	ASSERT( pTBCI != m_pItemRoot );
	ASSERT( pTBCI->m_pItemParent != NULL );
	if( pTBCI->m_pItemParent == m_pItemRoot )
	{
		if( (GetToolBoxWndStyle() & __TBWS_DND_GROUPS) == 0 )
			return false;
	}
	else
	{
		ASSERT( pTBCI->m_pItemParent->m_pItemParent == m_pItemRoot );
		if( (GetToolBoxWndStyle() & __TBWS_DND_ITEMS) == 0 )
			return false;
	}
	nMouseMoveFlags;
CPoint ptDiff(
		labs( ptClientStart.x - ptClientCurrent.x ),
		labs( ptClientStart.y - ptClientCurrent.y )
		);
	if( ptDiff.x >= 10 || ptDiff.y >= 10 )
		return true;
	return false;
}

#endif // (!defined __EXT_MFC_NO_TOOLBOX_CTRL)

