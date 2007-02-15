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

#if (!defined __EXT_TABWND_H)
#define __EXT_TABWND_H

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
// ExtStatusControlBar.h : header file
//

#ifndef __AFXTEMPL_H__
	#include <AfxTempl.h>
#endif

#if (!defined __EXT_CMD_ICON_H)
	#include <ExtCmdIcon.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtTabWnd window

// tab item styles
#define __ETWI_INVISIBLE			0x00000001L
#define __ETWI_GROUP_START			0x00000002L
#define __ETWI_SELECTED				0x00000004L
#define __ETWI_IN_GROUP_ACTIVE		0x00000008L

#define __ETWI_USER_STYLE_27		0x00000010L
#define __ETWI_USER_STYLE_26		0x00000020L
#define __ETWI_USER_STYLE_25		0x00000040L
#define __ETWI_USER_STYLE_24		0x00000080L
#define __ETWI_USER_STYLE_23		0x00000100L
#define __ETWI_USER_STYLE_22		0x00000200L
#define __ETWI_USER_STYLE_21		0x00000400L
#define __ETWI_USER_STYLE_20		0x00000800L
#define __ETWI_USER_STYLE_19		0x00001000L
#define __ETWI_USER_STYLE_18		0x00002000L
#define __ETWI_USER_STYLE_17		0x00004000L
#define __ETWI_USER_STYLE_16		0x00008000L
#define __ETWI_USER_STYLE_15		0x00010000L
#define __ETWI_USER_STYLE_14		0x00020000L
#define __ETWI_USER_STYLE_13		0x00040000L
#define __ETWI_USER_STYLE_12		0x00080000L
#define __ETWI_USER_STYLE_11		0x00100000L
#define __ETWI_USER_STYLE_10		0x00200000L
#define __ETWI_USER_STYLE_09		0x00400000L
#define __ETWI_USER_STYLE_08		0x00800000L
#define __ETWI_USER_STYLE_07		0x01000000L
#define __ETWI_USER_STYLE_06		0x02000000L
#define __ETWI_USER_STYLE_05		0x04000000L
#define __ETWI_USER_STYLE_04		0x08000000L
#define __ETWI_USER_STYLE_03		0x10000000L
#define __ETWI_USER_STYLE_02		0x20000000L
#define __ETWI_USER_STYLE_01		0x40000000L
#define __ETWI_USER_STYLE_00		0x80000000L

// tab window styles
#define __ETWS_ORIENT_TOP			0x00000000L
#define __ETWS_ORIENT_BOTTOM		0x00000001L
#define __ETWS_ORIENT_LEFT			0x00000002L
#define __ETWS_ORIENT_RIGHT			0x00000003L
#define __ETWS_ORIENT_MASK			0x00000003L

#define __ETWS_EQUAL_WIDTHS			0x00000004L
#define __ETWS_SHOW_BTN_HELP		0x00000008L
#define __ETWS_SHOW_BTN_CLOSE		0x00000010L
#define __ETWS_INVERT_VERT_FONT		0x00000020L
#define __ETWS_BOLD_SELECTION		0x00000040L
#define __ETWS_GROUPED				0x00000080L

#define __ETWS_ENABLED_BTN_HELP		0x00000100L
#define __ETWS_ENABLED_BTN_CLOSE	0x00000200L
#define __ETWS_ENABLED_BTN_UP		0x00000400L
#define __ETWS_ENABLED_BTN_DOWN		0x00000800L

#define __ETWS_CENTERED_TEXT		0x00001000L
#define __ETWS_FULL_WIDTH			0x00002000L
#define __ETWS_AUTOHIDE_SCROLL		0x00004000L
#define __ETWS_HOVER_FOCUS			0x00008000L

#define __ETWS_SCROLL_BY_PAGE		0x00010000L

#define __ETWS_SHOW_BTN_TAB_LIST	0x00080000L
#define __ETWS_ENABLED_BTN_TAB_LIST	0x00100000L

#define __ETWS_GROUPED_EXPAND_ITEMS	0x00200000L

#define __ETWS_USER_STYLE_09		0x00400000L
#define __ETWS_USER_STYLE_08		0x00800000L
#define __ETWS_USER_STYLE_07		0x01000000L
#define __ETWS_USER_STYLE_06		0x02000000L
#define __ETWS_USER_STYLE_05		0x04000000L
#define __ETWS_USER_STYLE_04		0x08000000L
#define __ETWS_USER_STYLE_03		0x10000000L
#define __ETWS_USER_STYLE_02		0x20000000L
#define __ETWS_USER_STYLE_01		0x40000000L
#define __ETWS_USER_STYLE_00		0x80000000L

// hit test values
#define __ETWH_NOWHERE			(-1L)
#define __ETWH_BORDER_AREA		(-2L)
#define __ETWH_ITEMS_AREA		(-3L)
#define __ETWH_BUTTON_LEFTUP	(-4L)
#define __ETWH_BUTTON_RIGHTDOWN	(-5L)
#define __ETWH_BUTTON_HELP		(-6L)
#define __ETWH_BUTTON_CLOSE		(-7L)
#define __ETWH_BUTTON_TAB_LIST	(-8L)
#define __ETWH_TAB_FIRST		(0L)

#if (!defined __EXT_MFC_NO_TAB_CTRL)

class __PROF_UIS_API CExtTabWnd
	: public CWnd
	, public CExtMouseCaptureSink
{
	CRuntimeClass * m_pHelperLastPaintManagerRTC;
	CToolTipCtrl m_wndToolTip;
// Construction
public:

	DECLARE_DYNCREATE( CExtTabWnd )

	CExtTabWnd();


	class __PROF_UIS_API TAB_ITEM_INFO : public CObject
	{
		CExtTabWnd * m_pWndTab;
		DWORD m_dwItemStyle;
		CExtSafeString m_sText;
		CExtCmdIcon m_icon;
		CSize m_sizeLastMeasuredItem, m_sizeLastMeasuredText, m_sizeLastMeasuredIcon;
		CRect m_rcItem;
		LPARAM m_lParam;
		bool m_bHelperToolTipAvail:1;

		TAB_ITEM_INFO * m_pPrev, * m_pNext;

		void _AssignFromOther(
			const TAB_ITEM_INFO & other
			);

	public:
		TAB_ITEM_INFO(
			CExtTabWnd * pWndTab = NULL,
			TAB_ITEM_INFO * pPrev = NULL,
			TAB_ITEM_INFO * pNext = NULL,
			__EXT_MFC_SAFE_LPCTSTR sText = NULL,
			HICON hIcon = NULL,
			bool bCopyIcon = true,
			DWORD dwStyle = 0, // visible & non-group
			LPARAM lParam = 0
			);
		TAB_ITEM_INFO( const TAB_ITEM_INFO & other );
		~TAB_ITEM_INFO();
		TAB_ITEM_INFO & operator=(
			const TAB_ITEM_INFO & other
			)
		{
			_AssignFromOther( other );
			return *this;
		}

		CExtTabWnd * GetTabWnd()
		{
			ASSERT_VALID( this );
			ASSERT_VALID( m_pWndTab );
			return m_pWndTab;
		}
		const CExtTabWnd * GetTabWnd() const
		{
			ASSERT_VALID( this );
			ASSERT_VALID( m_pWndTab );
			return m_pWndTab;
		}
		operator CExtTabWnd * ()
		{
			return GetTabWnd();
		}
		operator const CExtTabWnd * () const
		{
			return GetTabWnd();
		}

		__EXT_MFC_SAFE_LPCTSTR TextGet() const;
		operator __EXT_MFC_SAFE_LPCTSTR () const
		{
			return TextGet();
		}
		void TextSet(
			__EXT_MFC_SAFE_LPCTSTR sText = NULL
			)
		{
			ASSERT_VALID( this );
			m_sText = ( sText == NULL ) ? _T("") : sText;
		}
		TAB_ITEM_INFO & operator=(
			__EXT_MFC_SAFE_LPCTSTR sText
			)
		{
			ASSERT_VALID( this );
			TextSet( sText );
			return *this;
		}

		CSize IconGetSize() const;
		CExtCmdIcon * IconGetPtr() const;
		void IconSet(
			HICON hIcon = NULL,
			bool bCopyIcon = true
			)
		{
			ASSERT_VALID( this );
			if( hIcon == NULL )
				m_icon.Empty();
			else
				m_icon.AssignFromHICON( hIcon, bCopyIcon );
		}
		TAB_ITEM_INFO & operator=(
			HICON hIcon
			)
		{
			ASSERT_VALID( this );
			IconSet( hIcon );
			return *this;
		}

		DWORD GetItemStyle() const
		{
			ASSERT_VALID( this );
			return m_dwItemStyle;
		}
		DWORD ModifyItemStyle(
			DWORD dwRemove,
			DWORD dwAdd = 0
			)
		{
			ASSERT_VALID( this );
			ASSERT_VALID( m_pWndTab );
			
			DWORD dwOldStyle = m_dwItemStyle;
			m_dwItemStyle &= ~dwRemove;
			m_dwItemStyle |= dwAdd;

			bool bWasVisible =
				( dwOldStyle & __ETWI_INVISIBLE )
					? false
					: true;
			bool bNowVisible =
				( m_dwItemStyle & __ETWI_INVISIBLE )
					? false
					: true;
			if( bWasVisible != bNowVisible )
			{
				ASSERT(
						m_pWndTab->m_nVisibleItemCount >= 0
					&&	m_pWndTab->m_nVisibleItemCount <= m_pWndTab->m_arrItems.GetSize()
					);

				if( bNowVisible )
					m_pWndTab->m_nVisibleItemCount ++;
				else
					m_pWndTab->m_nVisibleItemCount --;
				
				ASSERT(
						m_pWndTab->m_nVisibleItemCount >= 0
					&&	m_pWndTab->m_nVisibleItemCount <= m_pWndTab->m_arrItems.GetSize()
					);
			} // if( bWasVisible != bNowVisible )

			bool bNowSelected =
				( m_dwItemStyle & __ETWI_SELECTED )
					? true
					: false;
			if( bNowSelected )
				m_dwItemStyle |= __ETWI_IN_GROUP_ACTIVE;

			bool bWasInGroupActive =
				( dwOldStyle & __ETWI_IN_GROUP_ACTIVE )
					? true
					: false;
			bool bNowInGroupActive =
				( m_dwItemStyle & __ETWI_IN_GROUP_ACTIVE )
					? true
					: false;

			if( bWasInGroupActive != bNowInGroupActive
				&& (m_pPrev != NULL || m_pNext != NULL)
				&& bNowInGroupActive
				)
			{
				bool bInGroupActiveWasFoundBeforeThis = false;
				if( (m_dwItemStyle & __ETWI_GROUP_START) == 0 )
				{
					for(	TAB_ITEM_INFO * pTii = m_pPrev;
							pTii != NULL;
							pTii = pTii->m_pPrev
							)
					{ // find active in group item before this
						if( pTii->m_dwItemStyle & __ETWI_IN_GROUP_ACTIVE )
						{
							pTii->m_dwItemStyle &= ~__ETWI_IN_GROUP_ACTIVE;
							bInGroupActiveWasFoundBeforeThis = true;
							break;
						}
						if( pTii->m_dwItemStyle & __ETWI_GROUP_START )
							break;
					} // find active in group item before this
				}
				if( !bInGroupActiveWasFoundBeforeThis )
				{
					for(	TAB_ITEM_INFO * pTii = m_pNext;
							pTii != NULL;
							pTii = pTii->m_pNext
							)
					{ // find active in group item after this
						if( pTii->m_dwItemStyle & __ETWI_GROUP_START )
							break;
						if( pTii->m_dwItemStyle & __ETWI_IN_GROUP_ACTIVE )
						{
							pTii->m_dwItemStyle &= ~__ETWI_IN_GROUP_ACTIVE;
							break;
						}
					} // find active in group item after this
				} // if( !bInGroupActiveWasFoundBeforeThis )
			} // if( bWasInGroupActive != bNowInGroupActive )

			return dwOldStyle;
		}

		CSize Measure(
			CDC * pDcMeasure = NULL
			);
		const CSize & GetLastMeasuredItemSize() const
		{
			return m_sizeLastMeasuredItem;
		}
		const CSize & GetLastMeasuredTextSize() const
		{
			return m_sizeLastMeasuredText;
		}
		const CSize & GetLastMeasuredIconSize() const
		{
			return m_sizeLastMeasuredIcon;
		}
		const CRect & ItemRectGet() const
		{
			return m_rcItem;
		}
		void ItemRectSet( const RECT & rcItem )
		{
			m_rcItem = rcItem;
		}

		TAB_ITEM_INFO * GetPrev()
		{
			ASSERT_VALID( this );
			return m_pPrev;
		}
		const TAB_ITEM_INFO * GetPrev() const
		{
			ASSERT_VALID( this );
			return m_pPrev;
		}
		TAB_ITEM_INFO * GetNext()
		{
			ASSERT_VALID( this );
			return m_pNext;
		}
		const TAB_ITEM_INFO * GetNext() const
		{
			ASSERT_VALID( this );
			return m_pNext;
		}
		bool IsFirst() const
		{
			return (GetPrev() == NULL) ? true : false;
		}
		bool IsLast() const
		{
			return (GetNext() == NULL) ? true : false;
		}

		bool VisibleGet() const
		{
			bool bVisible =
				( (GetItemStyle() & __ETWI_INVISIBLE) != 0 )
					? false : true;
			return bVisible;
		}
		bool VisibleSet( bool bVisible = true )
		{
			bool bWasVisible = VisibleGet();
			ModifyItemStyle(
				bVisible ? __ETWI_INVISIBLE : 0,
				bVisible ? 0 : __ETWI_INVISIBLE
				);
			return bWasVisible;
		}
		
		bool SelectedGet() const
		{
			bool bSelected =
				( (GetItemStyle() & __ETWI_SELECTED) != 0 )
					? true : false;
			return bSelected;
		}
		bool SelectedSet( bool bSelected = true )
		{
			bool bWasSelected = SelectedGet();
			ModifyItemStyle(
				bSelected ? 0 : __ETWI_SELECTED,
				bSelected ? __ETWI_SELECTED : 0
				);
			return bWasSelected;
		}

		bool GroupStartGet() const
		{
			bool bGroupStart =
				( (GetItemStyle() & __ETWI_GROUP_START) != 0 )
					? true : false;
			return bGroupStart;
		}
		bool GroupStartSet( bool bGroupStart = true )
		{
			bool bWasGroupStart = GroupStartGet();
			ModifyItemStyle(
				bGroupStart ? 0 : __ETWI_GROUP_START,
				bGroupStart ? __ETWI_GROUP_START : 0
				);
			return bWasGroupStart;
		}

		bool InGroupActiveGet() const
		{
			bool bInGroupActive =
				( (GetItemStyle() & __ETWI_IN_GROUP_ACTIVE) != 0 )
					? true : false;
			return bInGroupActive;
		}
		bool InGroupActiveSet( bool bInGroupActive = true )
		{
			bool bWasInGroupActive = InGroupActiveGet();
			ModifyItemStyle(
				bInGroupActive ? 0 : __ETWI_IN_GROUP_ACTIVE,
				bInGroupActive ? __ETWI_IN_GROUP_ACTIVE : 0
				);
			return bWasInGroupActive;
		}

		const TAB_ITEM_INFO * GetInGroupFirst() const
		{
			ASSERT_VALID( this );
			const TAB_ITEM_INFO * pTii = this;
			for( ; pTii != NULL; pTii = pTii->GetPrev() )
			{
				ASSERT_VALID( pTii );
				if( pTii->GroupStartGet() )
					return pTii;
			}
			ASSERT( FALSE );
			return NULL;
		}
		TAB_ITEM_INFO * GetInGroupFirst()
		{
			const TAB_ITEM_INFO * pTii = this;
			pTii = pTii->GetInGroupFirst();
			return
				const_cast < TAB_ITEM_INFO * > ( pTii );
		}
		const TAB_ITEM_INFO * GetInGroupLast() const
		{
			ASSERT_VALID( this );
			const TAB_ITEM_INFO * pTii = GetNext();
			const TAB_ITEM_INFO * pTii2 = this;
			for( ; pTii != NULL; )
			{
				ASSERT_VALID( pTii );
				if( pTii->GroupStartGet() )
					return pTii2;
				pTii2 = pTii;
				pTii = pTii->GetNext();
			}
			return pTii2;
		}
		TAB_ITEM_INFO * GetInGroupLast()
		{
			const TAB_ITEM_INFO * pTii = this;
			pTii = pTii->GetInGroupLast();
			return
				const_cast < TAB_ITEM_INFO * > ( pTii );
		}

		LONG GetIndexOf() const
		{
			ASSERT_VALID( this );
			ASSERT_VALID( m_pWndTab );
			return m_pWndTab->ItemGetIndexOf( this );
		}

		bool HitTest(
			const POINT & ptClient
			) const;

		LPARAM LParamGet() const
		{
			return m_lParam;
		}
		LPARAM LParamSet( LPARAM lParam = 0 )
		{
			LPARAM lParamOld = m_lParam;
			m_lParam = lParam;
			return lParamOld;
		}

		bool IsToolTipAvailByExtent() const
		{
			return m_bHelperToolTipAvail;
		}

#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
#endif
	
		friend class CExtTabWnd;
	}; // class TAB_ITEM_INFO

// Attributes
public:
	bool m_bReflectQueryRepositionCalcEffect:1;

protected:
	DWORD m_dwTabWndStyle;
	LONG m_nSelIndex,
		m_nIndexVisFirst, m_nIndexVisLast,
		m_nDelayedSelIndex;
	LONG m_nVisibleItemCount;
	LONG m_nItemsExtent;
	LONG m_nScrollPos, m_nScrollMaxPos;
	LONG m_nScrollDirectionRest; 

	CRect
		m_rcTabItemsArea,
		m_rcTabNearBorderArea,
		m_rcBtnUp,
		m_rcBtnDown,
		m_rcBtnHelp,
		m_rcBtnClose,
		m_rcBtnTabList;

	typedef
		CArray < TAB_ITEM_INFO *, TAB_ITEM_INFO *& >
		TabItemsArr_t;

	TabItemsArr_t m_arrItems;
	void _RemoveAllItemsImpl();

	bool _IsScrollAvail() const
	{
		return (m_nScrollMaxPos > 0) ? true : false;
	}
	bool _IsScrolling() const
	{
		return (m_nScrollDirectionRest != 0) ? true : false;
	}
	virtual CFont & _GetTabWndFont(
		bool bSelected,
		DWORD dwOrientation = DWORD(-1) // default orientation
		) const;
public:
	DWORD GetTabWndStyle() const
	{
		ASSERT_VALID( this );
		return m_dwTabWndStyle;
	}
	DWORD ModifyTabWndStyle(
		DWORD dwRemove,
		DWORD dwAdd = 0,
		bool bUpdateTabWnd = false
		)
	{
		ASSERT_VALID( this );

		DWORD dwOldStyle = m_dwTabWndStyle;
		
		m_dwTabWndStyle &= ~dwRemove;
		m_dwTabWndStyle |= dwAdd;

		if( m_dwTabWndStyle & __ETWS_EQUAL_WIDTHS )
			m_nScrollPos = 0;

		UpdateTabWnd( bUpdateTabWnd );
		return dwOldStyle;
	}

	DWORD OrientationGet() const
	{
		return GetTabWndStyle() & __ETWS_ORIENT_MASK;
	}
	DWORD OrientationSet(
		DWORD dwOrientation,
		bool bUpdateTabWnd = false
		)
	{
		DWORD dwOldOrientation =
			GetTabWndStyle() & __ETWS_ORIENT_MASK;
		ModifyTabWndStyle(
			__ETWS_ORIENT_MASK,
			dwOrientation & __ETWS_ORIENT_MASK
			);
		UpdateTabWnd( bUpdateTabWnd );
		return dwOldOrientation;
	}
	bool OrientationIsHorizontal() const
	{
		DWORD dwOrientation = OrientationGet();
		if( dwOrientation == __ETWS_ORIENT_TOP
			|| dwOrientation == __ETWS_ORIENT_BOTTOM
			)
			return true;
		return false;
	}
	bool OrientationIsVertical() const
	{
		return !OrientationIsHorizontal();
	}
	bool OrientationIsTopLeft() const
	{
		DWORD dwOrientation = OrientationGet();
		if( dwOrientation == __ETWS_ORIENT_TOP
			|| dwOrientation == __ETWS_ORIENT_LEFT
			)
			return true;
		return false;
	}

	LONG ItemGetCount() const
	{
		ASSERT_VALID( this );
		return (LONG)m_arrItems.GetSize();
	}
	LONG ItemGetVisibleCount() const
	{
		ASSERT_VALID( this );
		return m_nVisibleItemCount;
	}
	
	TAB_ITEM_INFO * ItemGet( LONG nIndex )
	{
		ASSERT_VALID( this );
		LONG nCount = ItemGetCount();
		if( nIndex < 0 || nIndex >= nCount )
		{
			ASSERT( FALSE );
			return NULL;
		}
		TAB_ITEM_INFO * pTii = m_arrItems[ nIndex ];
		ASSERT_VALID( pTii );
		ASSERT( pTii->m_pWndTab == this );
		return pTii;
	}
	const TAB_ITEM_INFO * ItemGet( LONG nIndex ) const
	{
		return
			(const_cast < CExtTabWnd * > ( this ))->ItemGet( nIndex );
	}

	__EXT_MFC_SAFE_LPCTSTR ItemTextGet( LONG nIndex ) const
	{
		return ItemGet(nIndex)->TextGet();
	}
	void ItemTextSet(
		LONG nIndex,
		__EXT_MFC_SAFE_LPCTSTR sText = NULL,
		bool bUpdateTabWnd = false
		)
	{
		ItemGet(nIndex)->TextSet( sText );
		UpdateTabWnd( bUpdateTabWnd );
	}

	LPARAM ItemLParamGet( LONG nIndex ) const
	{
		return ItemGet(nIndex)->LParamGet();
	}
	void ItemLParamSet(
		LONG nIndex,
		LPARAM lParam = 0
		)
	{
		ItemGet(nIndex)->LParamSet( lParam );
	}
	
	HICON ItemIconGet( LONG nIndex ) const
	{
		CExtCmdIcon * pIcon =
			ItemGet(nIndex)->IconGetPtr();
		if( pIcon == NULL || pIcon->IsEmpty() )
			return NULL;
		return pIcon->GetIcon();
	}
	void ItemIconSet(
		LONG nIndex,
		HICON hIcon = NULL,
		bool bCopyIcon = true,
		bool bUpdateTabWnd = false
		)
	{
		ItemGet(nIndex)->IconSet( hIcon, bCopyIcon );
		UpdateTabWnd( bUpdateTabWnd );
	}

	DWORD ItemStyleGet( LONG nIndex )
	{
		return ItemGet(nIndex)->GetItemStyle();
	}
	DWORD ItemStyleModify(
		LONG nIndex,
		DWORD dwRemove,
		DWORD dwAdd = 0,
		bool bUpdateTabWnd = false
		)
	{
		DWORD dwOldItemStyle =
			ItemGet(nIndex)->ModifyItemStyle(dwRemove,dwAdd);
		UpdateTabWnd( bUpdateTabWnd );
		return dwOldItemStyle;
	}

	LONG ItemGetIndexOf( const TAB_ITEM_INFO * pTii ) const
	{
		ASSERT_VALID( this );
		ASSERT_VALID( pTii );
		ASSERT( pTii->GetTabWnd() == this );
		LONG nCount = ItemGetCount();
		for( LONG nIndex = 0; nIndex < nCount; nIndex++ )
		{
			const TAB_ITEM_INFO * pTii2 = ItemGet( nIndex );
			if( pTii2 == pTii )
				return nIndex;
		} // for( LONG nIndex = 0; nIndex < nCount; nIndex++ )
		ASSERT( FALSE );
		return -1;
	}

	LONG SelectionGet() const
	{
		ASSERT_VALID( this );
		return m_nSelIndex;
	}
	const TAB_ITEM_INFO * SelectionGetPtr() const
	{
		ASSERT_VALID( this );
		if( m_nSelIndex < 0 || ItemGetCount() == 0 )
			return NULL;
		return ItemGet( m_nSelIndex );
	}
	TAB_ITEM_INFO * SelectionGetPtr()
	{
		ASSERT_VALID( this );
		LONG nCount = ItemGetCount();
		if( m_nSelIndex < 0 || nCount == 0 )
			return NULL;
		return ItemGet( m_nSelIndex );
	}
	LONG SelectionSet(
		LONG nSelIndex,
		bool bEnsureVisible = false,
		bool bUpdateTabWnd = false
		);
	bool SelectionDelay(
		LONG nSelIndex = -1, // -1 - to cancel
		DWORD dwMilliseconds = 0 // should be > 0 if nSelIndex >= 0
		);

	bool SelectionBoldGet() const
	{
		bool bSelectionBold =
			(GetTabWndStyle() & __ETWS_BOLD_SELECTION) ? true : false;
		if( bSelectionBold
			&& (GetTabWndStyle() & __ETWS_GROUPED)
			)
			bSelectionBold = false;
		return bSelectionBold;
	}
	void SelectionBoldSet(
		bool bBold = true,
		bool bUpdateTabWnd = false
		)
	{
		ModifyTabWndStyle(
			bBold ? 0 : __ETWS_BOLD_SELECTION,
			bBold ? __ETWS_BOLD_SELECTION : 0,
			bUpdateTabWnd
			);
	}

	void ItemGetGroupRange(
		LONG nIndex,
		LONG & nGroupStart,
		LONG & nGroupEnd,
		TAB_ITEM_INFO ** ppTiiStart = NULL,
		TAB_ITEM_INFO ** ppTiiEnd = NULL
		)
	{
		ASSERT_VALID( this );
		TAB_ITEM_INFO * pTii = ItemGet( nIndex );
		ASSERT_VALID( pTii );
		TAB_ITEM_INFO * pTiiStart = pTii->GetInGroupFirst();
		ASSERT_VALID( pTiiStart );
		TAB_ITEM_INFO * pTiiEnd = pTii->GetInGroupFirst();
		ASSERT_VALID( pTiiEnd );
		nGroupStart = ItemGetIndexOf( pTiiStart );
		nGroupEnd = ItemGetIndexOf( pTiiEnd );
		if( ppTiiStart != NULL )
			*ppTiiStart = pTiiStart;
		if( ppTiiEnd != NULL )
			*ppTiiEnd = pTiiEnd;
	}
	void ItemGetGroupRange(
		LONG nIndex,
		LONG & nGroupStart,
		LONG & nGroupEnd,
		const TAB_ITEM_INFO ** ppTiiStart = NULL,
		const TAB_ITEM_INFO ** ppTiiEnd = NULL
		) const
	{
		ASSERT_VALID( this );
		const TAB_ITEM_INFO * pTii = ItemGet( nIndex );
		ASSERT_VALID( pTii );
		const TAB_ITEM_INFO * pTiiStart = pTii->GetInGroupFirst();
		ASSERT_VALID( pTiiStart );
		const TAB_ITEM_INFO * pTiiEnd = pTii->GetInGroupFirst();
		ASSERT_VALID( pTiiEnd );
		nGroupStart = ItemGetIndexOf( pTiiStart );
		nGroupEnd = ItemGetIndexOf( pTiiEnd );
		if( ppTiiStart != NULL )
			*ppTiiStart = pTiiStart;
		if( ppTiiEnd != NULL )
			*ppTiiEnd = pTiiEnd;
	}

	TAB_ITEM_INFO * ItemInsert(
		__EXT_MFC_SAFE_LPCTSTR sText = NULL,
		HICON hIcon = NULL,
		bool bCopyIcon = true,
		DWORD dwItemStyle = 0, // visible & non-group
		LONG nIndex = -1, // default - append
		LPARAM lParam = 0,
		bool bUpdateTabWnd = false
		);
	LONG ItemRemove( // returns count of removed items
		LONG nIndex,
		LONG nCountToRemove = 1,
		bool bUpdateTabWnd = false
		);
	void ItemRemoveGroup(
		LONG nIndex,
		LONG * p_nGroupStart = NULL,
		LONG * p_nCountInGroup = NULL,
		bool bUpdateTabWnd = false
		)
	{
		LONG nGroupStart;
		LONG nGroupEnd;
		ItemGetGroupRange(
			nIndex,
			nGroupStart,
			nGroupEnd
			);
		ASSERT( nGroupEnd >= nGroupStart );
		LONG nCountToRemove = nGroupEnd - nGroupStart + 1;
		ItemRemove(
			nGroupStart,
			nCountToRemove,
			bUpdateTabWnd
			);
		if( p_nGroupStart != NULL )
			*p_nGroupStart = nGroupStart;
		if( p_nCountInGroup != NULL )
			*p_nCountInGroup = nCountToRemove;
	}
	void ItemRemoveAll(
		bool bUpdateTabWnd = false
		)
	{
		_RemoveAllItemsImpl();
		UpdateTabWnd( bUpdateTabWnd );
	}

	LONG ItemHitTest(
		const POINT & ptClient
		) const;

	bool ItemEnsureVisible(
		INT nItemIndex,
		bool bUpdateTabWnd = false
		);

	LONG ItemFindByLParam(
		LPARAM lParam,
		LONG nIndexStartSearch = -1,
		bool bIncludeVisible = true,
		bool bIncludeInvisible = false
		) const;
	LONG ItemFindByStyle(
		DWORD dwItemStyleInclude,
		DWORD dwItemStyleExclude = __ETWI_INVISIBLE,
		LONG nIndexStartSearch = -1
		) const;
// Operations
public:

	bool m_bReflectParentSizing:1;
	static HCURSOR g_hCursor;

	virtual BOOL Create(
		CWnd * pParentWnd,
		const RECT & rcWnd = CRect( 0, 0, 0, 0 ),
		UINT nDlgCtrlID = UINT( IDC_STATIC ),
		DWORD dwWindowStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
		DWORD dwTabStyle = __ETWS_ORIENT_TOP|__ETWS_ENABLED_BTN_HELP|__ETWS_ENABLED_BTN_CLOSE,
		CCreateContext * pContext = NULL
		);

	void UpdateTabWnd(
		bool bImmediate = true
		);

	static bool RegisterTabWndClass();

protected:
	
	static bool g_bTabWndClassRegistered;
	bool m_bDirectCreateCall:1;
	bool m_bDelayRecalcLayout:1;
	bool m_bTrackingButtonPushed:1;
	bool m_bTrackingButtonHover:1;
	bool m_bEnableTrackToolTips:1;
	bool m_bPushedUp:1;
	bool m_bPushedDown:1;
	bool m_bPushedHelp:1;
	bool m_bPushedClose:1;
	bool m_bPushedTabList:1;
	LONG m_nPushedTrackingButton; // MK_... values or (-1L)
	LONG m_nPushedTrackingHitTest;
	LONG m_nHoverTrackingHitTest;
	CRect m_rcRecalcLayout;
	CPoint m_ptStartDrag;

	bool _CreateHelper();
	void _RecalcLayoutImpl();
	void _RecalcLayout_GetClientRect(
		CRect & rcClient
		);
	bool _ProcessMouseClick(
		CPoint point,
		bool bButtonPressed,
		INT nMouseButton, // MK_... values
		UINT nMouseEventFlags
		);

	static void _CbPaintCombinedTabListBtnContent(
		LPVOID pCookie,
		CDC & dc,
		const CWnd & refWndMenu,
		const CRect & rcExcludeArea, // in screen coords
		int eCombineAlign // CExtPopupMenuWnd::e_combine_align_t values
		);

	virtual void OnTabWndSyncVisibility();

	virtual bool OnTabWndMouseTrackingPushedStart(
		INT nMouseButton, // MK_... values
		LONG nHitTest
		);
	virtual void OnTabWndMouseTrackingPushedStop(
		bool bEnableReleaseCapture = true
		);
	virtual bool OnTabWndMouseTrackingHoverStart(
		LONG nHitTest
		);
	virtual void OnTabWndMouseTrackingHoverStop(
		bool bEnableReleaseCapture = true
		);

	virtual bool OnTabWndStartDrag( LONG nIndex );

	virtual bool OnTabWndClickedButton(
		LONG nHitTest,
		bool bButtonPressed,
		INT nMouseButton, // MK_... values
		UINT nMouseEventFlags
		);
	virtual bool OnTabWndClickedItem(
		LONG nItemIndex,
		bool bButtonPressed,
		INT nMouseButton, // MK_... values
		UINT nMouseEventFlags
		);

	virtual bool OnTabWndSelectionChange(
		LONG nOldItemIndex,
		LONG nNewItemIndex,
		bool bPreSelectionTest
		);

	virtual CExtCmdIcon * OnTabWndQueryItemIcon(
		const TAB_ITEM_INFO * pTii
		) const;
	virtual __EXT_MFC_SAFE_LPCTSTR OnTabWndQueryItemText(
		const TAB_ITEM_INFO * pTii
		) const;
	
	virtual void OnTabWndItemInsert(
		LONG nItemIndex,
		TAB_ITEM_INFO * pTii
		);
	virtual void OnTabWndRemoveItem(
		LONG nItemIndex,
		LONG nCount,
		bool bPreRemove
		);
	virtual void OnTabWndRemoveAllItems(
		bool bPreRemove
		);

	virtual void OnTabWndMeasureItemAreaMargins(
		CDC & dcMeasure,
		LONG & nSpaceBefore,
		LONG & nSpaceAfter,
		LONG & nSpaceOver
		);
	virtual void OnTabWndDrawEntire(
		CDC & dc,
		CRect & rcClient
		);
	virtual void OnTabWndEraseClientArea(
		CDC & dc,
		CRect & rcClient,
		CRect & rcTabItemsArea,
		CRect & rcTabNearBorderArea,
		DWORD dwOrientation,
		bool bGroupedMode
		);
	virtual void OnTabWndUpdateItemMeasure(
		TAB_ITEM_INFO * pTii,
		CDC & dcMeasure,
		CSize & sizePreCalc
		);
	virtual void OnTabWndDrawItem(
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

	virtual CSize OnTabWndCalcButtonSize(
		CDC & dcMeasure,
		LONG nTabAreaMetric // vertical max width or horizontal max heights of all tabs
		);

	virtual void OnTabWndDoScroll(
		LONG nStep,
		bool bSmoothScroll = true
		);

	virtual bool OnTabWndToolTipQueryEnabled() const;

public:

	CRect CalcPreviewLayout( const CRect & rcAvail )
	{
		m_rcRecalcLayout = rcAvail;
		_RecalcLayoutImpl();
		m_rcRecalcLayout.SetRect( 0, 0, 0, 0 );
		
		CRect rcResult;
		if( m_rcTabItemsArea.IsRectEmpty() )
			GetWindowRect( &rcResult );
		else
			rcResult = m_rcTabItemsArea;

		m_bDelayRecalcLayout = true;
		return rcResult;
	}

	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtTabWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtTabWnd();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtTabWnd)
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnCancelMode();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	afx_msg LRESULT OnSizeParent( WPARAM wParam, LPARAM lParam );
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection);
	afx_msg LRESULT OnDisplayChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnThemeChanged( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT _OnQueryRepositionCalcEffect( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()

	friend class TAB_ITEM_INFO;
}; // class CExtTabWnd

#endif // (!defined __EXT_MFC_NO_TAB_CTRL )


#if (!defined __EXT_MFC_NO_TABMDI_CTRL )

/////////////////////////////////////////////////////////////////////////////
// CExtTabMdiWnd window

class __PROF_UIS_API CExtTabMdiWnd
	: public CExtTabWnd
	, public CExtHookSink
{
	mutable CExtCmdIcon m_iconMdiChildCache;
public:

	DECLARE_DYNCREATE( CExtTabMdiWnd )

	CExtTabMdiWnd();

	virtual BOOL Create(
		CWnd * pParentWnd,
		const RECT & rcWnd = CRect( 0, 0, 0, 0 ),
		UINT nDlgCtrlID = UINT( IDC_STATIC ),
		DWORD dwWindowStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
		DWORD dwTabStyle = __ETWS_ORIENT_TOP|__ETWS_ENABLED_BTN_HELP|__ETWS_ENABLED_BTN_CLOSE
			|__ETWS_SHOW_BTN_CLOSE,
		CCreateContext * pContext = NULL
		);

protected:
	LONG ItemFindByHWND(
		HWND hWnd,
		LONG nIndexStartSearch = -1,
		bool bIncludeVisible = true,
		bool bIncludeInvisible = false
		) const
	{
		return
			CExtTabWnd::ItemFindByLParam(
				(LPARAM)hWnd,
				nIndexStartSearch,
				bIncludeVisible,
				bIncludeInvisible
				);
	}

	bool m_bInSyncLayout:1;

	virtual void _RecalcFrameLayout();
	virtual void _SyncAllItems();

	virtual HWND _GetHwndMainFrame();
	virtual HWND _GetHwndMdiArea();

	virtual bool OnHookWndMsg(
		LRESULT & lResult,
		HWND hWndHooked,
		UINT nMessage,
		WPARAM & wParam,
		LPARAM & lParam
		);

	virtual void OnTabWndSyncVisibility();

	virtual bool OnTabWndClickedButton(
		LONG nHitTest,
		bool bButtonPressed,
		INT nMouseButton, // MK_... values
		UINT nMouseEventFlags
		);
	virtual bool OnTabWndClickedItem(
		LONG nItemIndex,
		bool bButtonPressed,
		INT nMouseButton, // MK_... values
		UINT nMouseEventFlags
		);
	virtual bool OnTabWndSelectionChange(
		LONG nOldItemIndex,
		LONG nNewItemIndex,
		bool bPreSelectionTest
		);
	virtual CExtCmdIcon * OnTabWndQueryItemIcon(
		const TAB_ITEM_INFO * pTii
		) const;
	virtual __EXT_MFC_SAFE_LPCTSTR OnTabWndQueryItemText(
		const TAB_ITEM_INFO * pTii
		) const;
	virtual void OnTabWndItemInsert(
		LONG nItemIndex,
		TAB_ITEM_INFO * pTii
		);
	virtual void OnTabWndRemoveItem(
		LONG nItemIndex,
		LONG nCount,
		bool bPreRemove
		);
	virtual void OnTabWndRemoveAllItems(
		bool bPreRemove
		);
	virtual bool OnTabWndMouseTrackingPushedStart(
		INT nMouseButton, // MK_... values
		LONG nHitTest
		);
	virtual bool OnTabWndMouseTrackingHoverStart(
		LONG nHitTest
		);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtTabMdiWnd)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtTabMdiWnd();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtTabMdiWnd)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
}; // class CExtTabMdiWnd

#endif // (!defined __EXT_MFC_NO_TABMDI_CTRL )

// tab wnd class name
#define __EXT_TAB_WND_CLASS_NAME _T("ProfUIS-TabWindow")

// helper values
#define __EXTTAB_MARGIN_BORDER_HX				2
#define __EXTTAB_MARGIN_BORDER_HY				2 //1
#define __EXTTAB_MARGIN_BORDER_VX				2 //1
#define __EXTTAB_MARGIN_BORDER_VY				2

#define __EXTTAB_MARGIN_ICON2TEXT_X			3
#define __EXTTAB_MARGIN_ICON2TEXT_Y			3

#define __EXTTAB_MIN_HORZ_WIDTH					6
#define __EXTTAB_MIN_HORZ_HEIGHT				20

// IMPORTANT:  the __EXTTAB_MIN_VERT_WIDTH adjusted by Genka
#define __EXTTAB_MIN_VERT_WIDTH					22 // 20
#define __EXTTAB_MIN_VERT_HEIGHT				4

#define __EXTTAB_SEPARATOR_AREA_X				3
#define __EXTTAB_SEPARATOR_AREA_Y				3

#define __EXTTAB_SEPARATOR_DX					1
#define __EXTTAB_SEPARATOR_DY					1
#define __EXTTAB_SEPARATOR_GAP_HORZ				2
#define __EXTTAB_SEPARATOR_GAP_VERT				2

#define __EXTTAB_ADD_END_SCROLL_SPACE			6

#define __EXTTAB_BTN_MIN_DX						12
#define __EXTTAB_BTN_MIN_DY						12
#define __EXTTAB_BTN_GAP_DX						2
#define __EXTTAB_BTN_GAP_DY						2
#define __EXTTAB_BETWEEN_BTN_GAP_DX				2
#define __EXTTAB_BETWEEN_BTN_GAP_DY				2

#define __EXTTAB_SCROLL_TIMER_ID				1
#define __EXTTAB_SCROLL_TIMER_PERIOD			15 // 30 // 20
#define __EXTTAB_SCROLL_STEP					25 // 50 // 50

#define __EXTTAB_BETWEEN_GROUP_GAP_DX			20
#define __EXTTAB_BETWEEN_GROUP_GAP_DY			20

#define __EXTTAB_SELECTION_DELAY_TIMER_ID		2
#define __EXTTAB_SELECTION_DELAY_TIMER_PERIOD	200

#define __EXTTAB_IN_FRAME_GAP_LEFT_DX			6
#define __EXTTAB_IN_FRAME_GAP_RIGHT_DX			6
#define __EXTTAB_IN_FRAME_GAP_TOP_DY			6
#define __EXTTAB_IN_FRAME_GAP_BOTTOM_DY			6

#define __EXTTAB_DRAG_START_DX					5
#define __EXTTAB_DRAG_START_DY					5

#define __EXTTAB_MDI_UPDATE_TIMER_ID			3
#define __EXTTAB_MDI_UPDATE_TIMER_PERIOD		10

#define __EXTTAB_MDI_NC_AREA_GAP_DX_L			2
#define __EXTTAB_MDI_NC_AREA_GAP_DX_R			2
#define __EXTTAB_MDI_NC_AREA_GAP_DY_T			2
#define __EXTTAB_MDI_NC_AREA_GAP_DY_B			2

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __EXT_TABWND_H

