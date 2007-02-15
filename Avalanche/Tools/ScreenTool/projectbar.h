#ifndef _PROJECTBAR_H_
#define _PROJECTBAR_H_

class ToolBarButton : public CTreeNode
{
	public:
		ToolBarButton( void );
		virtual ~ToolBarButton();

		virtual int GetButtonId( void );
		virtual void SetButtonId( int ButtonId );
		virtual BOOL SetButtonImage( HBITMAP hbm );
		virtual void SetButtonSize( int Width, int Height );
		virtual void SetButtonDown( BOOL TrueOrFalse );
		virtual void DestroyButtonImage( void );

	public:
		HBITMAP m_hbmButton;
		int m_Width;
		int m_Height;
		BOOL m_ButtonDown;
};

class ToolBar : public CWnd
{
	public:
		ToolBar( void );
		virtual ~ToolBar();

		virtual void AddButton( int ButtonId, HBITMAP ButtonImage );
		virtual void RemoveButton( int ButtonId );
		virtual ToolBarButton * GetButtonAtPosition( int x, int y );

		ToolBarButton m_Buttons;
		ToolBarButton *m_pButtonHovered;
		ToolBarButton *m_pButtonSelected;
		ToolBarButton *m_pButtonDown;

		CPoint m_MousePos;

	protected:
		virtual ToolBarButton * NewButton( void );

	protected:
		afx_msg void OnMouseMove( UINT Flags, CPoint p );
		afx_msg void OnLButtonDown( UINT Flags, CPoint p );
		afx_msg void OnLButtonUp( UINT Flags, CPoint p );
		afx_msg void OnRButtonDown( UINT Flags, CPoint p );
		afx_msg void OnPaint( void );

		DECLARE_MESSAGE_MAP()
};

#endif // _PROJECTBAR_H_