#ifndef _PROJECTVIEW_H_
#define _PROJECTVIEW_H_

class ProjectTree;
class ProjectView;

#include "projectbar.h"
#include "view.h"

class ProjectTree : public CTreeCtrl
{
	public:
		ProjectTree( void );

		BOOL m_AltKey;
		BOOL m_ControlKey;
		BOOL m_ShiftKey;

	protected:
		virtual void OnAltKey( BOOL KeyDown );
		virtual void OnControlKey( BOOL KeyDown );
		virtual void OnShiftKey( BOOL KeyDown );
		virtual void OnEnterKey( BOOL KeyDown );
		virtual void CheckSpecialKeys( UINT Flags );

	protected:
		afx_msg void OnMouseMove( UINT Flags, CPoint p );
		afx_msg void OnLButtonDown( UINT Flags, CPoint p );
		afx_msg void OnRButtonDown( UINT Flags, CPoint p );
		afx_msg void OnKeyDown( UINT Char, UINT Repeat, UINT Flags );
		afx_msg void OnKeyUp( UINT Char, UINT Repeat, UINT Flags );
		afx_msg void OnSysKeyDown( UINT Char, UINT Repeat, UINT Flags );
		afx_msg void OnSysKeyUp( UINT Char, UINT Repeat, UINT Flags );

	protected:
		DECLARE_MESSAGE_MAP()
};

class ProjectView : public CView
{
	DECLARE_DYNCREATE( ProjectView );

	public:
		ProjectView( void );
		virtual ~ProjectView();

		static void SyncProjectViews( CDocument *pDocument );
		static ProjectView* GetFirst( void );
		static ProjectView* GetNext( ProjectView* pProjectView );

		RECT GetToolBarRect( void );
		RECT GetProjectRect( void );
		void SyncToProject( void );
		void SyncToProjectEvent( void );

		void ResizeProjectView( void );
		void ResizeToolBar( void );

		GameView* GetGameView( void );

		Sprite* GetItemData( HTREEITEM hTreeItem );
		HTREEITEM FindItem( HTREEITEM hTreeItem, Sprite* pSprite );
		HTREEITEM FindParentItem( HTREEITEM hTreeItem, Sprite* pSprite );
		void RemoveSprite( Sprite* pSprite );

		ProjectTree m_ProjectTree;
		ToolBar m_ToolBar;
		int m_ToolBarSize;

	protected:
		virtual void SyncSceneList( HTREEITEM hTreeItem, Sprite *pSprite );
		virtual void SyncChildren( HTREEITEM hTreeItem, Sprite* pSprite );
      virtual void OnDraw( CDC *pdc );

	protected:
		afx_msg int OnCreate( LPCREATESTRUCT pcs );
		afx_msg void OnActivate( UINT State, CWnd* pOtherWnd, BOOL Minimized );
		afx_msg int OnMouseActivate( CWnd* pDesktopWnd, UINT HitTest, UINT message );
		afx_msg void OnSize( UINT Type, int x, int y );
		afx_msg void OnContextMenu( CWnd* pWnd, CPoint p );
		afx_msg BOOL OnSelChanged( UINT Id, NMHDR *pNotify, LRESULT *pResult );
		afx_msg void OnTimer( UINT_PTR TimerEvent );

	private:
		static ProjectView *m_pProjectViews;
		ProjectView *m_pNextProjectView;
		BOOL m_AllowSelChange;

		DECLARE_MESSAGE_MAP();
};

#endif // _PROJECTVIEW_H_