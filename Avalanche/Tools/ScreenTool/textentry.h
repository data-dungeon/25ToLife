#ifndef _TEXTENTRY_H_
#define _TEXTENTRY_H_

class TextEntryWindow : public CWnd
{
	public:
		TextEntryWindow( void );
		virtual ~TextEntryWindow();

		void SetNotification( HWND hWnd, UINT ControlId );

	public:
		NotifyEdit m_EditCtrl;

	protected:
		afx_msg int OnCreate( LPCREATESTRUCT pcs );
		afx_msg void OnPaint( void );
		afx_msg void OnSize( UINT Type, int x, int y );
		afx_msg void OnSetFocus( CWnd *pOldWnd );
		afx_msg int OnMouseActivate( CWnd *pDesktopWnd, UINT HitTest, UINT message );
		afx_msg void OnEditNotify( NMHDR *pNotify, LRESULT *pResult );

		// text edit nofications

		afx_msg void OnTextEditChange( void );
		afx_msg void OnTextEditUpdate( void );

	protected:
		HWND m_NotifyWnd;
		int m_NotifyControlId;
		BOOL m_ShiftKeyDown;

	protected:
		DECLARE_MESSAGE_MAP();
};

#endif // _TEXTENTRY_H_