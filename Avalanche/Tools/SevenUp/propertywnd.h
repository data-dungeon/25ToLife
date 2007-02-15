#ifndef _PropertyList_H_
#define _PropertyList_H_

typedef enum
{
	PROPERTYTYPE_UNKNOWN,
	PROPERTYTYPE_CATAGORY,
	PROPERTYTYPE_STRING,
	PROPERTYTYPE_INTEGER,
	PROPERTYTYPE_FLOAT,
	PROPERTYTYPE_BOOL,
}
PROPERTYTYPE;

#define PROPNOTIFY_SELCHANGED		(0x100)
#define PROPNOTIFY_ITEMCHANGED	(0x101)

typedef struct
{
	NMHDR	nmhdr;
	int PropertyCode;
	const char* pPropertyKey;
	const char* pPropertyString;
}
PROPERTYNOTIFYSTRUCT;

class PropertyItem : public CTreeNode
{
	public:
		PropertyItem( void );
		virtual ~PropertyItem();

		PropertyItem* Parent() { return (PropertyItem*) CTreeNode::Parent(); }
		PropertyItem* Child()  { return (PropertyItem*) CTreeNode::Child();  }
		PropertyItem* Next()   { return (PropertyItem*) CTreeNode::Next();   }
		PropertyItem* Prev()   { return (PropertyItem*) CTreeNode::Prev();   }

		void SetProperty( const char *pName, const char *pDescription, PROPERTYTYPE Type, int PropertyId, int Quantity = 0, BOOL ReadOnly = FALSE );
		void SetName( const char *pName );
		void SetDescription( const char *pDescription );
		void SetValue( const char *pString );

	public:
		tstring m_Name;
		tstring m_Description;
		tstring m_Value;
		PROPERTYTYPE m_Type;
		int m_PropertyCode;
		int m_Quantity;
		int m_CellHeight;
		BOOL m_ReadOnly;
		BOOL m_Collapsed;
		RECT m_CellRect;
		RECT m_LeftRect;
		RECT m_RightRect;
		RECT m_SideRect;
};

class PropertyWnd : public CWnd
{
	public:
		PropertyWnd( void );
		virtual ~PropertyWnd();

	public:
		virtual PropertyItem * NewPropertyItem( void );

		void Clear( void );
		BOOL AddProperty( const char *pName, const char *pDescription, PROPERTYTYPE Type, int PropertyId, int Quantity = 0, BOOL ReadOnly = FALSE );
		BOOL AddCatagory( const char *pName );
		PropertyItem* GetPropertyItem( int PropertyId );
		PropertyItem* GetPropertyItem( const char *pPropertyName );
		RECT GetPropertyRect( void );
		RECT GetNonClientRect( void );
		void SelectProperty( PropertyItem *pPropertyItem );
		void SelectProperty( int PropertyCode );
		void SelectProperty( const char *pPropertyName );
		void GotoNextProperty( void );
		void GotoPrevProperty( void );
		void Collapse( const char *pName, BOOL Collapsed = TRUE );

		int SelectedPropertyCode( void );
		BOOL SelectedPropertyString( TCHAR *pString, int StringSize );
		void SetPropertyString( int PropertyCode, const char *pString );
		BOOL GetPropertyString( int PropertyCode, TCHAR *pString, int StringSize );

		void SetUserParam( void* pParam );
		void* GetUserParam( void );

	public:
		PropertyItem m_PropertyList;
		PropertyItem *m_pSelectedProperty;
		NotifyEdit m_EditCtrl;
		CFont m_GuiFont;
		CFont m_BoldFont;
		int m_SeparatorPos;
		BOOL m_SeparatorDrag;
		CPoint m_DragPosition;
		int m_CellHeight;
		BOOL m_ShiftKey;
		BOOL m_TabScroll;
		int m_ScrollWidth;
		int m_ScrollHeight;
		RECT m_WindowRect;
		int m_CurrentProperty;
		tstring m_OriginalText;
		SCROLLINFO m_ScrollInfo;
		void* m_pUserParam;

	protected:
		virtual BOOL InitializeFonts( void );
		virtual void PositionEditCtrl( void );

	protected:
		afx_msg void OnActivate( UINT State, CWnd *pWnd, BOOL Minimized );
		afx_msg int OnCreate( LPCREATESTRUCT pcs );
		afx_msg void OnDestroy( void );
		afx_msg void OnMouseMove( UINT Flags, CPoint p );
		afx_msg void OnLButtonDown( UINT Flags, CPoint p );
		afx_msg void OnLButtonUp( UINT Flags, CPoint p );
		afx_msg void OnPaint( void );
		afx_msg void OnSize( UINT Type, int x, int y );
		afx_msg void OnMove( int x, int y );
		afx_msg void OnKeyDown( UINT Key, UINT RepeatCount, UINT Flags );
		afx_msg void OnVScroll( UINT Code, UINT Pos, CScrollBar *pScrollBar );
		afx_msg void OnShowWindow( BOOL Show, UINT Status );
		afx_msg void OnEditNotify( NMHDR *pNotify, LRESULT *pResult );
		afx_msg void OnEditUpdate( void );

	DECLARE_MESSAGE_MAP();
};

#endif // _PropertyList_H_