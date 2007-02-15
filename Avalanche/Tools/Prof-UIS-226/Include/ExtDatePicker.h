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

#if (!defined __EXT_DATE_PICKER_H)
#define __EXT_DATE_PICKER_H

#if (!defined __EXT_MFC_NO_DATE_PICKER)

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_HOOK_H)
	#include "../Src/ExtHook.h"
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_MFC_NO_DATE_PICKER_POPUP)
	#if (!defined __EXT_POPUP_CTRL_MENU_H)
		#include <ExtPopupCtrlMenu.h>
	#endif
#endif // (!defined __EXT_MFC_NO_DATE_PICKER_POPUP)

#define __EXT_DATE_PICKER_CLASS_NAME		_T("ProfUIS-DatePickerWnd")

// general hit-testing values
#define __EDPWH_NOWHERE						(0L)
#define __EDPWH_BTN_NONE					(1L)
#define __EDPWH_BTN_TODAY					(2L)
#define __EDPWH_INNER_AREA					(3L)
// month-picker button values
#define __EDPWH_BTN_BACKWARD				(4L)
#define __EDPWH_BTN_FORWARD					(5L)
// month-picker date values
#define __EDPWH_HEADER_DATE					(6L)
#define __EDPWH_DAYS_OF_WEEK				(7L)
#define __EDPWH_DAYS_AREA					(8L)

// scrolling via buttons
#define __EDPW_SCROLL_TIMER_ID				1
#define __EDPW_SCROLL_TIMER_PERIOD			150 

// scrolling via popup month list
#define __EDPW_SCROLL_MONTH_LIST_ACCELERATION_STEP  10  // pixels
#define __EDPW_SCROLL_MONTH_LIST_UP_TIMER_ID		2
#define __EDPW_SCROLL_MONTH_LIST_DOWN_TIMER_ID		3

#define __EDPW_SCROLL_MONTH_LIST_TIMER_ELAPSE1		500 // milliseconds
#define __EDPW_SCROLL_MONTH_LIST_TIMER_ELAPSE2		300 
#define __EDPW_SCROLL_MONTH_LIST_TIMER_ELAPSE3		200 
#define __EDPW_SCROLL_MONTH_LIST_TIMER_ELAPSE4		100  
#define __EDPW_SCROLL_MONTH_LIST_TIMER_ELAPSE5		50  

// date picker styles
#define __EDPWS_BORDER_RESERVE				0x00000001L
#define __EDPWS_BORDER_SHOW					0x00000002L
#define __EDPWS_BORDER						(__EDPWS_BORDER_RESERVE|__EDPWS_BORDER_SHOW)
#define __EDPWS_BUTTON_TODAY				0x00000004L
#define __EDPWS_BUTTON_NONE					0x00000008L
#define __EDPWS_BUTTON_ANY					(__EDPWS_BUTTON_TODAY|__EDPWS_BUTTON_NONE)
#define __EDPWS_BUTTON_SEPARATOR				0x00000010L
#define __EDPWS_BOLD_HEADER					0x00000020L
#define __EDPWS_BOLD_HEADER_POPUP			0x00000040L
#define __EDPWS_BOLD_WEEK_DAYS				0x00000080L
#define __EDPWS_BOLD_DAY_CELLS				0x00000100L
#define __EDPWS_BOLD_ANY					(__EDPWS_BOLD_HEADER|__EDPWS_BOLD_HEADER_POPUP|__EDPWS_BOLD_WEEK_DAYS|__EDPWS_BOLD_DAY_CELLS)
#define __EDPWS_SHOW_NON_MONTH_DAYS			0x00000200L
#define __EDPWS_HIDE_INNER_NON_MONTH_DAYS	0x00000400L
#define __EDPWS_SHOW_TODAY_HIGHLIGHT		0x00000800L
#define __EDPWS_SHOW_MONTH_LIST_SHADOW		0x00001000L
#define __EDPWS_MULTIPLE_SELECTION			0x00002000L
#define __EDPWS_AUTO_DIMENSION_MODE			0x00004000L
#define __EDPWS_NO_HEADER_BACKGROUND		0x00008000L
#define __EDPWS_USER_STYLE_15				0x00010000L
#define __EDPWS_USER_STYLE_14				0x00020000L
#define __EDPWS_USER_STYLE_13				0x00040000L
#define __EDPWS_USER_STYLE_12				0x00080000L
#define __EDPWS_USER_STYLE_11				0x00100000L
#define __EDPWS_USER_STYLE_10				0x00200000L
#define __EDPWS_USER_STYLE_09				0x00400000L
#define __EDPWS_USER_STYLE_08				0x00800000L
#define __EDPWS_USER_STYLE_07				0x01000000L
#define __EDPWS_USER_STYLE_06				0x02000000L
#define __EDPWS_USER_STYLE_05				0x04000000L
#define __EDPWS_USER_STYLE_04				0x08000000L
#define __EDPWS_USER_STYLE_03				0x10000000L
#define __EDPWS_USER_STYLE_02				0x20000000L
#define __EDPWS_USER_STYLE_01				0x40000000L
#define __EDPWS_USER_STYLE_00				0x80000000L

// default set of styles
#define __EDPWS_DEFAULT \
	( __EDPWS_BORDER \
	| __EDPWS_SHOW_NON_MONTH_DAYS \
	| __EDPWS_HIDE_INNER_NON_MONTH_DAYS \
	| __EDPWS_SHOW_TODAY_HIGHLIGHT \
	| __EDPWS_SHOW_MONTH_LIST_SHADOW \
	| __EDPWS_BUTTON_TODAY \
	| __EDPWS_BUTTON_SEPARATOR \
	)

//////////////////////////////////////////////////////////////////////////
// class CExtDatePickerHeaderPopupWnd
//////////////////////////////////////////////////////////////////////////

class CExtDatePickerWnd;

class __PROF_UIS_API CExtDatePickerHeaderPopupWnd : public CWnd
{
protected:
	CSize m_sizeDesiredSize, m_sizeItemSize;
	INT	m_nMonth, m_nYear, m_nSelMonth, m_nSelYear, m_nItemsPerPage;
	CExtDatePickerWnd * m_pDPW;
	CExtWndShadow m_shadow;
	bool m_bScrollingUp:1, m_bScrollingDown:1, m_bUseShadow:1;
	UINT m_nLastElapseTimerUp, m_nLastElapseTimerDown;

public:
	DECLARE_DYNAMIC( CExtDatePickerHeaderPopupWnd );
	CExtDatePickerHeaderPopupWnd(
		CExtDatePickerWnd * pDPW,
		bool bUseShadow
		);
	virtual ~CExtDatePickerHeaderPopupWnd();
	void SetInitialDate( INT nMonth, INT nYear );
	bool GetSelectedDate( INT & nMonth, INT & nYear ) const;
	void SetItemsPerPage( INT nItemsPerPage );
	CSize GetSize(
		bool bOnlyClientArea = false
		) const;
	UINT GetScrollingSpeed( int nPixels );
	void AnalyzeChangings();

protected:
	void _CalcSize();
	INT _HitTest( CPoint pt );

	//{{AFX_VIRTUAL(CExtDatePickerHeaderPopupWnd)
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PostNcDestroy();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CExtDatePickerHeaderPopupWnd)
protected:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
}; /// class CExtDatePickerHeaderPopupWnd
	
//////////////////////////////////////////////////////////////////////////
// class CExtDatePickerWnd
//////////////////////////////////////////////////////////////////////////

class __PROF_UIS_API CExtDatePickerWnd
	: public CWnd
	, public CExtMouseCaptureSink
{
	class __PROF_UIS_API MONTH_INFO : public CObject
	{
	public:
		class __PROF_UIS_API DATE_INFO : public CObject
		{
		protected:
			CRect m_rcCell;
			COleDateTime m_dt;
			CExtDatePickerWnd * m_pDPW;
			MONTH_INFO * m_pMI;
		public:
			DATE_INFO(
				CExtDatePickerWnd * pDPW = NULL
				);
			~DATE_INFO();
#ifdef _DEBUG
			void AssertValid() const;
			void Dump( CDumpContext & dc ) const;
#endif //_DEBUG
			void Clear();
			void SetDatePickerWnd( 
				CExtDatePickerWnd * pDPW,
				MONTH_INFO * pMI
				);
			void SetRect( const RECT & rcCell );
			CRect GetRect() const;
			void SetDate( const COleDateTime & dt );
			COleDateTime GetDate() const;
			void Draw( 
				CDC & dc,
				bool bShowNonMonthDays = true,
				bool bShowTodayHighlight = true
				) const;
		}; // class DATE_INFO
	
	protected:
		DATE_INFO m_aDays[6][7];
		INT m_nMonth;
		INT m_nYear;
		CRect	m_rcScrollBtnBackward, m_rcScrollBtnForward,
				m_rcHeaderDate, m_rcHeader, m_rcHeaderWithoutBorders,
				m_rcDaysCells, m_rcDaysOfWeek;
		CExtDatePickerWnd * m_pDPW;
	public:
		INT m_nRow, m_nCol;
		CRect m_rcClient;
		MONTH_INFO( 
			INT nRow, 
			INT nCol, 
			INT nMonth, 
			INT nYear, 
			CRect rcClient,
			CExtDatePickerWnd * pDPW
			);
#ifdef _DEBUG
		void AssertValid() const;
		void Dump( CDumpContext & dc) const;
#endif // _DEBUG

	protected:
		void _RecalcLayout();
	public:
		CRect GetRect() const;
		CRect GetHeaderRect() const;
		void GetMonth( INT & nMonth, INT & nYear ) const;
		COleDateTime GetMonthInfoDT() const;
		const DATE_INFO * HitTestDay( const POINT & ptClient ) const;
		LONG HitTest(
			const POINT & ptClient,
			COleDateTime * pDT = NULL
			) const;
		void Draw( CDC & dc ) const;
	}; // class MONTH_INFO 
	
protected:
	enum e_MouseTrackType_t
	{
		__EMTT_NOTHING = 0,
		__EMTT_SCROLLING_BACKWARD = 1,
		__EMTT_SCROLLING_FORWARD = 2,
		__EMTT_BTN_NONE_PRESSED = 3,
		__EMTT_BTN_TODAY_PRESSED = 4,
		__EMTT_DAYS_SELECTING = 5
	};
	e_MouseTrackType_t m_eMTT;
	DWORD m_dwDatePickerStyle;
	CSize	m_sizeCalendarDimensionsMin,
			m_sizeCalendarDimensionsCurrent,
			m_sizeCalendarDimensionsMax;
	CRect	m_rcDatePickerClient, m_rcDatePickerInnerArea,
			m_rcBtnNone, m_rcBtnToday;
	CTypedPtrArray < CPtrArray, MONTH_INFO * > m_arrMonths;
	COleDateTime m_dtCurrentDate, // currently selected date
		m_dtSelBegin, m_dtSelEnd,
		m_dtDisplayedBegin, m_dtDisplayedEnd;
	static bool g_bDatePickerWndClassRegistered;
	bool	m_bDirectCreateCall:1, m_bCanceling:1, m_bUpdatingLayout:1,
			m_bBtnTodayMouseHover:1, m_bBtnNoneMouseHover:1;
	CSize	m_sizeMonthPicker, m_sizeDateCell,
			m_sizePushButton, m_sizeScrollButton;
	INT		m_nMonthHeaderHeight, m_nButtonAreaHeight,
			m_nDaysOfWeekHeight, m_nIndentSpace;
	
	bool _CreateHelper();
	void _RecalcLayout();
	void _RecalcCalendar(
		bool bUpdateNow = false
		);
	MONTH_INFO * _HitTestMonth( 
		const POINT & ptClient 
		) const;
	const MONTH_INFO * _FindMonth( INT nRow, INT nCol ) const;

public:
	DECLARE_DYNCREATE( CExtDatePickerWnd );
	CExtDatePickerWnd();
	virtual ~CExtDatePickerWnd();

	struct __PROF_UIS_API SELECTION_INFO
	{
		COleDateTime m_dtBegin, m_dtEnd;
		SELECTION_INFO(
			const COleDateTime & dt
			)
			: m_dtBegin( dt )
			, m_dtEnd( dt )
		{
		}
		SELECTION_INFO(
			const COleDateTime & dtBegin,
			const COleDateTime & dtEnd
			)
			: m_dtBegin( dtBegin )
			, m_dtEnd( dtEnd )
		{
		}
		SELECTION_INFO(
			const SELECTION_INFO * pOther = NULL
			)
		{
			if( pOther != NULL )
			{
				m_dtBegin = pOther->m_dtBegin;
				m_dtEnd = pOther->m_dtEnd;
			} // if( pOther != NULL )
		}
		SELECTION_INFO(
			const SELECTION_INFO & other
			)
			: m_dtBegin( other.m_dtBegin )
			, m_dtEnd( other.m_dtEnd )
		{
		}
		SELECTION_INFO & operator = (
			const SELECTION_INFO & other
			)
		{
			m_dtBegin = other.m_dtBegin;
			m_dtEnd = other.m_dtEnd;
			return (*this);
		}
		SELECTION_INFO & operator = (
			const COleDateTime & dt
			)
		{
			m_dtBegin = dt;
			m_dtEnd = dt;
			return (*this);
		}
		bool operator == (
			const SELECTION_INFO & other
			) const
		{
			if(		m_dtBegin == other.m_dtBegin
				&&	m_dtEnd == other.m_dtEnd
				)
				return true;
			if(		m_dtBegin == other.m_dtEnd
				&&	m_dtEnd == other.m_dtBegin
				)
				return true;
			return false;
		}
		bool operator == (
			const COleDateTime & dt
			) const
		{
			if(		m_dtBegin == dt
				&&	m_dtEnd == dt
				)
				return true;
			return false;
		}
		bool IsEmptySelection() const
		{
			if(		m_dtBegin.GetStatus() != COleDateTime::valid
				||	m_dtEnd.GetStatus() != COleDateTime::valid
				)
				return true;
			return false;
		}
	};

	struct __PROF_UIS_API SELECTION_NOTIFICATION : public SELECTION_INFO
	{
		HWND m_hWndDatePicker;
		LPARAM m_lParamCookie;
		bool m_bFinalSelectionChanging:1,
			m_bAllowFinalParentCancelMode:1;
		SELECTION_INFO * m_pExternalSelectionInfo;
		SELECTION_NOTIFICATION(
			SELECTION_INFO * pExternalSelectionInfo,
			HWND hWndDatePicker,
			LPARAM lParamCookie,
			bool bFinalSelectionChanging,
			const COleDateTime & dtBegin,
			const COleDateTime & dtEnd,
			bool bAllowFinalParentCancelMode = true
			);
		SELECTION_NOTIFICATION(
			SELECTION_INFO * pExternalSelectionInfo,
			const CExtDatePickerWnd & wndDatePicker,
			bool bFinalSelectionChanging,
			bool bAllowFinalParentCancelMode = true
			);
		operator WPARAM() const;
		static const SELECTION_NOTIFICATION * FromWPARAM( WPARAM wParam );
		friend class CExtDatePickerWnd;
	private:
		void Notify( HWND hWndNotify ) const;
	};

// Attributes
public:
	LPARAM m_lParamCookie;
	static const UINT g_nMsgSelectionNotification;
	HWND m_hWndNotificationReceiver;
	SELECTION_INFO * m_pExternalSelectionInfo;

// Operations
public:
	static bool RegisterDatePickerWndClass();
	virtual bool Create(
		CWnd * pParentWnd,
		const RECT & rcWnd = CRect( 0, 0, 0, 0 ),
		UINT nDlgCtrlID = UINT( IDC_STATIC ),
		CSize sizeCalendarDimensionsMin = CSize(1,1),
		CSize sizeCalendarDimensionsMax = CSize(1,1),
		DWORD dwWindowStyle = WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,
		DWORD dwDatePickerStyle = __EDPWS_DEFAULT,
		CCreateContext * pContext = NULL
		);
	void AdjustSize( const SIZE & sizeDim );
	void AdjustSize( INT nDimCols, INT nDimRows );
	virtual DWORD GetDatePickerStyle() const;
	virtual DWORD ModifyDatePickerStyle(
		DWORD dwRemove,
		DWORD dwAdd = 0,
		bool bUpdateDatePickerWnd = false
		);
	virtual void UpdateDatePickerWnd(
		bool bUpdateNow = false
		);
	virtual bool SelectionGetForDate( 
		const COleDateTime & dt 
		) const;
	virtual bool EnsureVisible(
		const COleDateTime & dt,
		bool bUpdateNow = false
		);
	void SelectionClear( 
		bool bUpdateNow = false
		);
	virtual void SelectionSet(
		const COleDateTime & dtSelBegin, 
		const COleDateTime & dtSelEnd, 
		bool bNormalize = false,
		bool bUpdateNow = false
		);
	virtual void SelectionGet(
		COleDateTime & dtSelBegin, 
		COleDateTime & dtSelEnd
		) const;
	virtual void GetDisplayedRange(
		COleDateTime & dtBegin, 
		COleDateTime & dtEnd
		) const;
	virtual void CurrentDateSet( 
		const COleDateTime & dtCurrentDate, 
		bool bUpdateNow = false
		);
	void CurrentDateSet( 
		INT nYear,
		INT nMonth,
		INT nDay = 1,
		bool bUpdateNow = false
		);
	virtual COleDateTime CurrentDateGet() const;
	void CurrentDateGet( 
		INT & nYear, 
		INT & nMonth, 
		INT & nDay 
		) const;
	virtual void DimSet( 
		CSize sizeCalendarDimensionsMin = CSize(0,0),
		CSize sizeCalendarDimensionsMax = CSize(0,0),
		bool bUpdateNow = false
		);
	virtual void DimGet( 
		SIZE * pSizeCalendarDimensionsMin = NULL,
		SIZE * pSizeCalendarDimensionsMax = NULL,
		SIZE * pSizeCalendarDimensionsCurrent = NULL
		) const;

	LONG HitTest( 
		const POINT & ptClient,
		COleDateTime * pDT = NULL
		) const;

	virtual CExtSafeString OnDatePickerGetMonthName(
		INT nMonth,
		bool bMakeUpper = false
		) const;
	virtual INT OnDatePickerGetFirstDayOfWeek() const;
	virtual CExtSafeString OnDatePickerGetShortDayName(
		INT nDayOfWeek,
		bool bMakeUpper = false
		) const;
	virtual COleDateTime OnDatePickerGetToday() const;

	virtual HWND OnDatePickerGetNotificationReceiver() const;
	virtual void OnDatePickerDrawTodayBorder(
		CDC & dc,
		bool bShowNonMonthDays,
		bool bShowTodayHighlight,
		bool bDrawNonMonthDay,
		bool bDrawToday,
		const CRect & rcCell,
		const COleDateTime & dt,
		bool bDrawSelected
		) const;
	virtual void OnDatePickerDrawDateCell(
		CDC & dc,
		bool bShowNonMonthDays,
		bool bShowTodayHighlight,
		bool bDrawNonMonthDay,
		bool bDrawToday,
		const CRect & rcCell,
		const COleDateTime & dt,
		bool bDrawSelected
		) const;
	virtual void OnDatePickerDrawButtonsSeparator(
		CDC & dc,
		const CRect & rcSeparator
		) const;
	virtual CExtSafeString OnDatePickerQueryPushButtonCaptionText(
		LONG nButtonType // __EDPWH_BTN_NONE or __EDPWH_BTN_TODAY
		) const;
	virtual void OnDatePickerDrawPushButton(
		CDC & dc,
		const CRect & rcButton,
		LONG nButtonType, // __EDPWH_BTN_NONE or __EDPWH_BTN_TODAY
		__EXT_MFC_SAFE_LPCTSTR strCaption,
		bool bFlat,
		bool bDrawBorder,
		bool bPushed,
		bool bHover
		) const;
	virtual void OnDatePickerEraseHeader(
		INT nCol,
		INT nRow,
		CDC & dc,
		INT nYear,
		INT nMonth,
		const CRect & rcHeader,
		const CRect & rcHeaderWithoutBorders,
		const CRect & rcHeaderDate,
		const CRect & rcScrollBtnBackward,
		const CRect & rcScrollBtnForward
		) const;
	virtual void OnDatePickerDrawHeader(
		INT nCol,
		INT nRow,
		CDC & dc,
		INT nYear,
		INT nMonth,
		const CRect & rcHeader,
		const CRect & rcHeaderWithoutBorders,
		const CRect & rcHeaderDate,
		const CRect & rcScrollBtnBackward,
		const CRect & rcScrollBtnForward
		) const;
	virtual void OnDatePickerDrawDaysOfWeek(
		INT nYear,
		INT nMonth,
		CDC & dc,
		const CRect & rcClient
		) const;
	virtual void OnDatePickerDrawEntire(
		CDC & dc,
		const CRect & rcClient
		) const;
	virtual void OnDatePickerEraseClientArea(
		CDC & dc,
		const CRect & rcClient
		) const;
	virtual void OnDatePickerDrawBorder(
		CDC & dc,
		const CRect & rcClient
		) const;

	virtual CRect OnDatePickerGetBorderMetrics() const;

	virtual CFont * OnDatePickerQueryHeaderPopupFont() const;
	virtual CFont * OnDatePickerQueryDayCellFont(
		const COleDateTime & dt
		) const;
	virtual CFont * OnDatePickerQueryMonthHeaderFont(
		INT nYear,
		INT nMonth
		) const;
	virtual CFont * OnDatePickerQueryDaysOfWeekFont(
		INT nYear,
		INT nMonth
		) const;

	virtual INT OnDatePickerQueryIndentSpace() const;
	virtual CSize OnDatePickerQueryDateCellSize() const;
	virtual CSize OnDatePickerQueryPushButtonSize() const;
	virtual CSize OnDatePickerQueryScrollButtonSize() const;
	virtual INT OnDatePickerQueryMonthHeaderHeight() const;
	virtual INT OnDatePickerQueryDaysOfWeekHeight() const;
	virtual INT OnDatePickerQueryButtonAreaHeight() const;
	virtual CSize OnDatePickerQueryMonthPickerSize() const;
	virtual void OnDatePickerRecalcMetrics(
		CDC & dcMeasure,
		CSize & sizeMonthPicker,
		CSize & sizeDateCell,
		CSize & sizePushButton,
		CSize & sizeScrollButton,
		INT & nMonthHeaderHeight,
		INT & nButtonAreaHeight,
		INT & nDaysOfWeekHeight,
		INT & nIndentSpace
		);
	virtual CSize OnDatePickerCalcContentSize(
		const SIZE & sizeDim
		) const;

protected:

	void _RecalcMetrics();

	bool _ProcessMouseClick(
		CPoint point,
		bool bButtonPressed,
		INT nMouseButton, // MK_... values
		UINT nMouseEventFlags
		);

	bool _ProcessMouseMove(
		CPoint point,
		UINT nMouseEventFlags
		);

	virtual void OnDatePickerDoScroll(
		INT nMonthCount = 1,
		bool bSmoothScroll = true
		);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtDatePickerWnd)
	protected:
	virtual void PreSubclassWindow();
	virtual void PostNcDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	bool m_bAutoDeleteWindow:1;
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CExtDatePickerWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
#if _MFC_VER < 0x700
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#endif
	afx_msg void OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection);
	afx_msg void OnTimeChange();
	afx_msg void OnSysColorChange();
	afx_msg LRESULT OnDisplayChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnThemeChanged( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
}; /// class CExtDatePickerWnd

#if (!defined __EXT_MFC_NO_DATE_PICKER_POPUP)

/////////////////////////////////////////////////////////////////////////////
// CExtPopupDatePickerMenuWnd

class __PROF_UIS_API CExtPopupDatePickerMenuWnd :
	public CExtPopupControlMenuWnd
{
public:
	DECLARE_DYNCREATE(CExtPopupDatePickerMenuWnd)

	CExtPopupDatePickerMenuWnd(
		LPARAM lParamCookie = 0L,
		CSize sizeCalendarDimensions = CSize(1,1),
		DWORD dwDatePickerWindowStyle = WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,
		DWORD dwDatePickerStyle = __EDPWS_DEFAULT
		);
	~CExtPopupDatePickerMenuWnd();

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

// Attributes
public:
	CExtDatePickerWnd::SELECTION_INFO * m_pExternalSelectionInfo;
	LPARAM m_lParamCookie;
	CSize m_sizeCalendarDimensions;
	DWORD m_dwDatePickerWindowStyle;
	DWORD m_dwDatePickerStyle;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtPopupDatePickerMenuWnd)
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
	virtual int OnQueryMenuBorderSize() const;
protected:
	virtual HWND OnCreateChildControl(
		const RECT & rcChildControl
		);

protected:
	// Generated message map functions
	//{{AFX_MSG(CExtPopupDatePickerMenuWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; // class CExtPopupDatePickerMenuWnd

#endif // (!defined __EXT_MFC_NO_DATE_PICKER_POPUP)

/////////////////////////////////////////////////////////////////////////////

#endif // (!defined __EXT_MFC_NO_DATE_PICKER)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __EXT_DATE_PICKER_H
