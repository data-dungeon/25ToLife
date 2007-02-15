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

#if (!defined __EXT_POPUP_CTRL_MENU_H)
#define __EXT_POPUP_CTRL_MENU_H

#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_TOOLCONTROLBAR_H)
	#include <ExtToolControlBar.h>
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CExtPopupControlMenuWnd

class __PROF_UIS_API CExtPopupControlMenuWnd :
	public CExtPopupMenuWnd
{
public:
	DECLARE_DYNCREATE(CExtPopupControlMenuWnd)

	CExtPopupControlMenuWnd();
	~CExtPopupControlMenuWnd();

// Attributes
public:
	CSize m_sizeChildControl; // child control size
	static UINT g_nMsgControlInputRetranslate;
	static bool g_bControlMenuWithShadows;

// Implementation
protected:
	CRect m_rcChildControl;
	HWND m_hWndChildControl;

	virtual bool _IsPopupWithShadows() const;
	virtual CRect _CalcTrackRect();
	virtual CSize _CalcTrackSize();
	virtual bool _CreateHelper(
		CWnd * pWndCmdReceiver
		);

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	// CExtCustomizeSite::ICustomizeDropTarget
	virtual DROPEFFECT OnCustomizeTargetOver(
		CExtCustomizeSite::CCmdDragInfo & _dragInfo,
		CPoint point,
		DWORD dwKeyState
		);
	virtual void OnCustomizeTargetLeave();
	virtual bool OnCustomizeTargetDrop(
		CExtCustomizeSite::CCmdDragInfo & _dragInfo,
		CPoint point,
		DROPEFFECT de
		);
	// CExtCustomizeSite::ICustomizeDropSource
	virtual void OnCustomizeSourceDragComplete(
		DROPEFFECT de,
		bool bCanceled,
		bool * p_bNoResetActiveItem
		);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

public:
	virtual bool IsAllItemsRarelyUsed() const;

	struct __PROF_UIS_API CONTROLINPUTRETRANSLATEINFO
	{
		enum eRetranslateType
		{
			__ERTT_MOUSE_MOVE = 0,
			__ERTT_MOUSE_CLICK = 1,
			__ERTT_MOUSE_WHEEL = 2,
			__ERTT_KEYBOARD = 3,
		};
		eRetranslateType m_eRTT;
		UINT m_nFlags; // mouse and keyboard
		CPoint m_point; // mouse
		bool & m_bNoEat; // mouse and keyboard - returned value
		UINT m_nChar; // keyboard
		UINT m_nRepCnt; // keyboard
		WPARAM m_wParam; // wheel
		LPARAM m_lParam; // wheel
		CExtPopupControlMenuWnd * m_pControlPopup;
		CONTROLINPUTRETRANSLATEINFO(
			CExtPopupControlMenuWnd * pControlPopup,
			bool bMouseMove, // true - move, false - click
			UINT nFlags,
			CPoint point,
			bool & bNoEat
			)
			: m_eRTT( bMouseMove ? __ERTT_MOUSE_MOVE : __ERTT_MOUSE_CLICK )
			, m_pControlPopup( pControlPopup )
			, m_nFlags( nFlags )
			, m_point( point )
			, m_bNoEat( bNoEat  )
			, m_nChar( 0 )
			, m_nRepCnt( 0 )
			, m_wParam( 0 )
			, m_lParam( 0 )
		{
			ASSERT_VALID( pControlPopup );
		}
		CONTROLINPUTRETRANSLATEINFO(
			CExtPopupControlMenuWnd * pControlPopup,
			UINT nChar,
			UINT nRepCnt,
			UINT nFlags,
			bool & bNoEat
			)
			: m_eRTT( __ERTT_KEYBOARD )
			, m_pControlPopup( pControlPopup )
			, m_nFlags( nFlags )
			, m_point( 0, 0 )
			, m_bNoEat( bNoEat )
			, m_nChar( nChar )
			, m_nRepCnt( nRepCnt )
			, m_wParam( 0 )
			, m_lParam( 0 )
		{
			ASSERT_VALID( pControlPopup );
		}
		CONTROLINPUTRETRANSLATEINFO(
			CExtPopupControlMenuWnd * pControlPopup,
			WPARAM wParam,
			LPARAM lParam,
			bool & bNoEat
			)
			: m_eRTT( __ERTT_MOUSE_WHEEL )
			, m_pControlPopup( pControlPopup )
			, m_nFlags( 0 )
			, m_point( 0, 0 )
			, m_bNoEat( bNoEat )
			, m_nChar( 0 )
			, m_nRepCnt( 0 )
			, m_wParam( wParam )
			, m_lParam( lParam )
		{
			ASSERT_VALID( pControlPopup );
		}
	}; // struct CONTROLINPUTRETRANSLATEINFO
	virtual bool _OnMouseWheel(
		WPARAM wParam,
		LPARAM lParam,
		bool & bNoEat
		);
	virtual bool _OnMouseMove(
		UINT nFlags,
		CPoint point,
		bool & bNoEat
		);
	virtual bool _OnMouseClick(
		UINT nFlags,
		CPoint point,
		bool & bNoEat
		);
	virtual bool _OnKeyDown(
		UINT nChar,
		UINT nRepCnt,
		UINT nFlags,
		bool & bNoEat
		);

	virtual bool _CanStartLevelTracking();

	virtual void _AdjustAnimation( e_animation_type_t & eAT );

	virtual void _RecalcLayoutImpl();
	virtual CRect _RecalcControlRect();

	virtual HWND OnCreateChildControl(
		const RECT & rcChildControl
		);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	// Generated message map functions
	//{{AFX_MSG(CExtPopupControlMenuWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; // class CExtPopupControlMenuWnd

/////////////////////////////////////////////////////////////////////////////
// CExtPopupInplaceListBox

class __PROF_UIS_API CExtPopupInplaceListBox :
	public CListBox
{
public:
	DECLARE_DYNCREATE(CExtPopupInplaceListBox)

	CExtPopupInplaceListBox();
	~CExtPopupInplaceListBox();

// Attributes
public:

	enum e_sel_action_t
	{
	__SAT_SELCHANGE = 0,
	__SAT_SELENDOK = 1,
	__SAT_SELENDCANCEL = 2,
	};

	typedef
		bool (*pCbListBoxSelection)(
			CExtPopupInplaceListBox & wndListBox,
			LPVOID pInitListBoxCoockie,
			int eSAT // e_sel_action_t
			);

	pCbListBoxSelection m_pCbListBoxSelection;
	LPVOID m_pSelectionCoockie;

	typedef
		bool (*pCbInitListBoxContent)(
			CExtPopupInplaceListBox & wndListBox,
			LPVOID pInitListBoxCoockie
			);
	
	pCbInitListBoxContent m_pCbInitListBoxContent;
	LPVOID m_pInitListBoxCoockie;

	typedef
		bool (*pCbListBoxItemDraw)(
			CExtPopupInplaceListBox & wndListBox,
			LPVOID pListBoxCoockie,
			LPDRAWITEMSTRUCT pDIS
			);
	typedef
		bool (*pCbListBoxItemMeasure)(
			CExtPopupInplaceListBox & wndListBox,
			LPVOID pListBoxCoockie,
			LPMEASUREITEMSTRUCT pMIS
			);
	pCbListBoxItemDraw m_pCbListBoxItemDraw;
	pCbListBoxItemMeasure m_pCbListBoxItemMeasure;
	LPVOID m_pListBoxItemCoockie;

	LPARAM m_lParamCookie;
	static UINT g_nMsgPopupListBoxInitContent;
	static UINT g_nMsgPopupListBoxItemMeasure;
	static UINT g_nMsgPopupListBoxItemDraw;
	static UINT g_nMsgPopupListBoxSelEndOK;
	static UINT g_nMsgPopupListBoxSelEndCancel;
	static UINT g_nMsgPopupListBoxSelChange;
	struct __PROF_UIS_API POPUPLISTBOXITEMMEASUREINFO
	{
		CExtPopupInplaceListBox * m_pListBox;
		LPMEASUREITEMSTRUCT m_pMIS;
		POPUPLISTBOXITEMMEASUREINFO(
			CExtPopupInplaceListBox * pListBox,
			LPMEASUREITEMSTRUCT pMIS
			)
			: m_pListBox( pListBox )
			, m_pMIS( pMIS )
		{
			ASSERT_VALID( m_pListBox );
			ASSERT(
					m_pListBox->GetSafeHwnd() != NULL
				&&	::IsWindow( m_pListBox->GetSafeHwnd() )
				);
			ASSERT( m_pMIS != NULL );
		}
	}; // struct POPUPLISTBOXITEMMEASUREINFO
	struct __PROF_UIS_API POPUPLISTBOXITEMDRAWINFO
	{
		CExtPopupInplaceListBox * m_pListBox;
		LPDRAWITEMSTRUCT m_pDIS;
		POPUPLISTBOXITEMDRAWINFO(
			CExtPopupInplaceListBox * pListBox,
			LPDRAWITEMSTRUCT pDIS
			)
			: m_pListBox( pListBox )
			, m_pDIS( pDIS )
		{
			ASSERT_VALID( m_pListBox );
			ASSERT(
					m_pListBox->GetSafeHwnd() != NULL
				&&	::IsWindow( m_pListBox->GetSafeHwnd() )
				);
			ASSERT( m_pDIS != NULL );
			ASSERT( m_pDIS->hDC != NULL );
		}
	}; // struct POPUPLISTBOXITEMDRAWINFO
	struct __PROF_UIS_API POPUPLISTBOXITEMSELENDINFO
	{
		CExtPopupInplaceListBox * m_pListBox;
		INT m_nCurSel;
		POPUPLISTBOXITEMSELENDINFO(
			CExtPopupInplaceListBox * pListBox
			)
			: m_pListBox( pListBox )
		{
			ASSERT_VALID( m_pListBox );
			ASSERT(
					m_pListBox->GetSafeHwnd() != NULL
				&&	::IsWindow( m_pListBox->GetSafeHwnd() )
				);
			m_nCurSel = m_pListBox->GetCurSel();
		}
	}; // struct POPUPLISTBOXITEMSELENDINFO

// Implementation
private:
	bool m_bSelEndNotificationPassed:1;
protected:
	virtual void _DoSelEndOK();
	virtual void _DoSelEndCancel(
		bool bFinalDestroyMode = false
		);
	virtual void _DoSelChange();

// Overrides
	//{{AFX_VIRTUAL(CExtPopupInplaceListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT pDIS);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT pMIS);
	protected:
	virtual void PostNcDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CExtPopupInplaceListBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; // class CExtPopupInplaceListBox

/////////////////////////////////////////////////////////////////////////////
// CExtPopupListBoxMenuWnd

class __PROF_UIS_API CExtPopupListBoxMenuWnd :
	public CExtPopupControlMenuWnd
{
public:
	DECLARE_DYNCREATE(CExtPopupListBoxMenuWnd)

	CExtPopupListBoxMenuWnd(
		LPARAM lParamListCookie = 0L,
		DWORD dwListBoxStyles = WS_CHILD|WS_VISIBLE|WS_VSCROLL|LBS_NOINTEGRALHEIGHT|LBS_HASSTRINGS|LBS_OWNERDRAWVARIABLE
		);
	~CExtPopupListBoxMenuWnd();

// Attributes
public:
	LPARAM m_lParamListCookie;
	DWORD m_dwListBoxStyles;
	CExtPopupInplaceListBox::pCbListBoxSelection
		m_pCbListBoxSelection;
	LPVOID m_pSelectionCoockie;
	CExtPopupInplaceListBox::pCbInitListBoxContent
		m_pCbInitListBoxContent;
	LPVOID m_pInitListBoxCoockie;
	CExtPopupInplaceListBox::pCbListBoxItemDraw
		m_pCbListBoxItemDraw;
	CExtPopupInplaceListBox::pCbListBoxItemMeasure
		m_pCbListBoxItemMeasure;
	LPVOID m_pListBoxItemCoockie;

// Implementation
protected:
	virtual HWND OnCreateChildControl(
		const RECT & rcChildControl
		);

protected:
	// Generated message map functions
	//{{AFX_MSG(CExtPopupListBoxMenuWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; // class CExtPopupListBoxMenuWnd

/////////////////////////////////////////////////////////////////////////////
// CExtBarTextFieldButton

class __PROF_UIS_API CExtBarTextFieldButton : public CExtBarButton
{
public:
	class CInPlaceEditWnd;
	typedef
		bool (*pCbVerifyTextInput)(
			CInPlaceEditWnd & edit,
			LPVOID pCoockie,
			__EXT_MFC_SAFE_LPCTSTR sTextOld,
			__EXT_MFC_SAFE_LPCTSTR sTextNew
			);
	typedef
		bool (*pCbInplaceEditWndProc)(
			LRESULT & lResult,
			UINT message,
			WPARAM wParam,
			LPARAM lParam,
			CInPlaceEditWnd & edit,
			LPVOID pCoockie
			);
	static void _CancelInplaceEditor();
protected:
	CExtSafeString m_sTextField;
	CExtSafeString m_sTextInplaceBuffer;
	INT m_nTextFieldWidth;
	bool m_bComboField:1;
	bool m_bComboPopupDropped:1;
	bool m_bHelperFindListInitialItem:1;
	static bool stat_CbVerifyTextInput(
		CInPlaceEditWnd & edit,
		CExtBarTextFieldButton * pTextFieldTBB,
		__EXT_MFC_SAFE_LPCTSTR sTextOld,
		__EXT_MFC_SAFE_LPCTSTR sTextNew
		);
	static void stat_CbPutTextInputResult(
		CInPlaceEditWnd & edit,
		CExtBarTextFieldButton * pTextFieldTBB,
		__EXT_MFC_SAFE_LPCTSTR sTextNew
		);
	static bool stat_CbInplaceEditWndProc(
		LRESULT & lResult,
		UINT message,
		WPARAM wParam,
		LPARAM lParam,
		CEdit & wndEdit,
		CExtBarTextFieldButton * pTextFieldTBB
		);
	static bool stat_CbInitListBoxContent(
			CExtPopupInplaceListBox & wndListBox,
			CExtBarTextFieldButton * pTextFieldTBB
			);
	static bool stat_CbListBoxSelection(
		CExtPopupInplaceListBox & wndListBox,
		CExtBarTextFieldButton * pTextFieldTBB,
		int eSAT // CExtPopupInplaceListBox::e_sel_action_t
		);
	static bool stat_CbListBoxItemDraw(
		CExtPopupInplaceListBox & wndListBox,
		CExtBarTextFieldButton * pTextFieldTBB,
		LPDRAWITEMSTRUCT pDIS
		);
	static bool stat_CbListBoxItemMeasure(
		CExtPopupInplaceListBox & wndListBox,
		CExtBarTextFieldButton * pTextFieldTBB,
		LPMEASUREITEMSTRUCT pMIS
		);
public:

	LPARAM m_lParamCookie;
	DWORD m_dwListBoxStyles; // popup list-box styles
	CExtSafeStringArray m_arrLbItems;
	INT m_nDropDownWidth; // (-1) - auto calc, (-2) - same as button area
	INT m_nDropDownHeightMax;
	bool m_bTextFieldIsNotEditable:1;

	class CInPlaceEditWnd : public CEdit
	{
		CExtToolControlBar * m_pBar;
		CRuntimeClass * m_pBtnRTC;
		CExtBarTextFieldButton * m_pTbbTextField;
		pCbVerifyTextInput m_pCbVerifyTextInput;
		pCbInplaceEditWndProc m_pCbWndProc;
		LPVOID m_pCbCoockie;
		CExtSafeString * m_pStr;
		bool m_bCanceling:1;
		bool _IsValidState();
	public:
		CInPlaceEditWnd(
			CExtBarTextFieldButton * pTextFieldTBB,
			CExtSafeString * pStr,
			pCbVerifyTextInput pCbVerify = NULL,
			pCbInplaceEditWndProc pCbWndProc = NULL,
			LPVOID pCbCoockie = NULL
			);
		~CInPlaceEditWnd();
#ifdef _DEBUG
		virtual void AssertValid() const;
#endif // _DEBUG
		virtual bool Create();
		virtual LRESULT WindowProc(
			UINT message,
			WPARAM wParam,
			LPARAM lParam
			);
	protected:
		virtual void PostNcDestroy();
		virtual BOOL PreTranslateMessage( MSG * pMsg );
	public:
		CExtBarTextFieldButton * GetBarTextFieldButton()
		{
			ASSERT_VALID( this );
			return m_pTbbTextField;
		}
		const CExtBarTextFieldButton * GetBarTextFieldButton() const
		{
			ASSERT_VALID( this );
			return m_pTbbTextField;
		}
		static CInPlaceEditWnd * g_pWndInplaceEditor;
	}; // class CInPlaceEditWnd

	DECLARE_DYNCREATE(CExtBarTextFieldButton);

	CExtBarTextFieldButton(
		bool bComboField = false,
		INT nTextFieldWidth = __EXT_MENU_DEF_INPLACE_EDIT_WIDTH,
		CExtToolControlBar * pBar = NULL,
		UINT nCmdID = ID_SEPARATOR,
		UINT nStyle = 0
		);
	~CExtBarTextFieldButton();
	
	virtual INT GetTextFieldWidth() const;
	virtual INT SetTextFieldWidth( INT nTextFieldWidth );
	virtual __EXT_MFC_SAFE_LPCTSTR GetFieldText() const;
	virtual bool SetFieldText(
		__EXT_MFC_SAFE_LPCTSTR sText,
		bool bVerify = true
		);
	bool IsComboTextField() const
	{
		ASSERT_VALID( this );
		return m_bComboField;
	}
	bool IsComboPopupDropped() const
	{
		ASSERT_VALID( this );
		return (IsComboTextField() && m_bComboPopupDropped) ? true : false;
	}

	// CExtBarTextFieldButton overrides
	virtual CRect OnCalcComboDropRect( const RECT & rcBtnArea ) const;
	virtual CRect OnInplaceControlCalcRect( const RECT & rcBtnArea ) const;
	virtual CRect OnInplaceControlCalcMarginSizes() const;
	virtual CWnd * OnInplaceControlCreate();
	virtual void OnInplaceControlRun();
	virtual void OnInplaceControlSessionEnd();
	virtual void OnInplaceControlSessionCancel();
	virtual bool OnInplaceControlVerifyTextInput(
		CEdit * pEdit,
		__EXT_MFC_SAFE_LPCTSTR sTextOld,
		__EXT_MFC_SAFE_LPCTSTR sTextNew
		);
	virtual void OnInplaceControlPutTextInputResult(
		CEdit * pEdit,
		__EXT_MFC_SAFE_LPCTSTR sTextNew
		);
	virtual bool OnInplaceControlWndProcCall(
		LRESULT & lResult,
		UINT message,
		WPARAM wParam,
		LPARAM lParam,
		CEdit & wndEdit
		);
	virtual CExtPopupMenuWnd * OnCreateDropPopup(
		bool bContentExpand // true - inserting to content expand menu, false - tracking dropped popup
		);
	virtual CSize OnPopupListBoxMeasureTrackSize() const;
	virtual bool OnPopupListBoxInitContent(
		CExtPopupInplaceListBox & wndListBox
		);
	virtual bool OnPopupListBoxSelChange(
		CExtPopupInplaceListBox & wndListBox
		);
	virtual bool OnPopupListBoxSelEndOK(
		CExtPopupInplaceListBox & wndListBox
		);
	virtual bool OnPopupListBoxSelEndCancel(
		CExtPopupInplaceListBox & wndListBox
		);
	virtual bool OnPopupListBoxItemDraw(
		CExtPopupInplaceListBox & wndListBox,
		LPDRAWITEMSTRUCT pDIS
		);
	virtual bool OnPopupListBoxItemMeasure(
		CExtPopupInplaceListBox & wndListBox,
		LPMEASUREITEMSTRUCT pMIS
		);
	virtual CSize OnPopupListBoxCalcItemExtraSizes() const;

	// CExtBarButton overrides
protected:
	virtual void _UpdateCtrl();
public:
	virtual CExtSafeString GetText() const;
	virtual bool GetSeparatedDropDown() const
	{
		return false;
	}
	virtual void SetSeparatedDropDown( bool bSeparatedDropDown = true )
	{
		ASSERT( FALSE );
		bSeparatedDropDown;
		return;
	}
	virtual bool GetAutoChangeID() const
	{
		return false;
	}
	virtual void SetAutoChangeID( bool bAutoChangeID = true )
	{
		ASSERT( FALSE );
		bAutoChangeID;
		return;
	}
//	virtual bool GetNoRotateVerticalLayout() const
//	{
//		return true;
//	}
//	virtual void SetNoRotateVerticalLayout( bool bNoRotateVerticalLayout = true )
//	{
//		ASSERT( FALSE );
//		bNoRotateVerticalLayout;
//		return;
//	}
	virtual void SetMenu(
		HMENU hMenu,
		bool bPopupMenu,
		bool bAutoDestroyMenu
		);
	virtual HMENU GetMenu();
//	virtual bool IsLargeIcon() const;
//	virtual bool IsDisplayScreenTip() const;
//	virtual bool IsDisplayShortcutKeysOnScreenTip() const;
	virtual void SetHover( bool bHover = true );
	virtual void SetRect( const RECT & rectButton );
	virtual bool IsAbleToTrackMenu(
		bool bCustomizeMode = false
		);
	virtual bool IsContainerOfPopupLikeMenu();
//	virtual CExtCmdIcon * GetIconPtr();
	virtual CSize CalculateLayout(
		CDC & dc,
		CSize sizePreCalc,
		BOOL bHorz
		);
	virtual BOOL PutToPopupMenu(
		CExtPopupMenuWnd * pPopup
		);
	virtual UINT OnTrackPopup(
		CPoint point,
		bool bSelectAny
		);
	virtual void OnHover(
		CPoint point,
		bool bOn
		);
	virtual void OnClick(
		CPoint point,
		bool bDown
		);
	virtual void OnDeliverCmd();
	virtual int OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		);
	virtual LRESULT OnHelpHitTest(
		CPoint point
		);
	virtual void OnUpdateCmdUI(
		CWnd * pTarget,
		BOOL bDisableIfNoHndler,
		int nIndex
		);
	virtual bool OnSetCursor( CPoint point );

	virtual void Paint(
		CDC & dc,
		bool bHorz
		);

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	virtual int GetInitialResizingStateH( // -1 - left side resizing, 1 - right side resizing, 0 - no resizing at specified point (in bar's client coord)
		CPoint point,
		int * p_nWidthMin = NULL,
		int * p_nWidthMax = NULL
		);
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
/*
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
protected:
	virtual void _UpdateCmdNodeFromStyle();
public:
	virtual bool IsCustomizeActiveItem();
	virtual void SetCustomizeActiveItem();
	virtual int CalculateDropOverEffect(
		CPoint point,
		CExtCustomizeSite::CCmdDragInfo & _dragInfo,
		bool bAllowTrackNodePopup = true
		);
	virtual CExtCustomizeCmdTreeNode * GetCmdNode(
		bool bInitial = false
		);
	virtual void OnCustomizeUpdateProps(
		CExtCustomizeCmdTreeNode * pNode = NULL // update from basic node
		);
	virtual void SetBasicCmdNode(
		CExtCustomizeCmdTreeNode * pNode
		);
	virtual void SetCustomizedCmdNode(
		CExtCustomizeCmdTreeNode * pNode
		);
	virtual CExtCustomizeCmdTreeNode * OnCustomizeNodeInit(
		CExtCustomizeSite * pSite,
		CExtCustomizeCmdTreeNode * pParentNode // toolbar node
		);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
*/

}; // class CExtBarTextFieldButton

#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

#endif // __EXT_POPUP_CTRL_MENU_H

