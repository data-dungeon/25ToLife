#ifndef _VIEW_H_
#define _VIEW_H_

class SevenupView;

#include "gameview.h"
#include "projectview.h"
#include "rulerbar.h"

class SevenupView : public CView
{
	DECLARE_DYNCREATE( SevenupView );

	public:
		SevenupView();
		virtual ~SevenupView();

		//////////////////////////////////////////////////////////////////////////
		
		SevenupDoc * GetDocument( void );
		void OnNewDocument( void );

		//////////////////////////////////////////////////////////////////////////
		
		RECT GetThumbnailRect( void );
		RECT GetGameViewRect( void );
		RECT GetProjectViewRect( void );
		RECT GetRuler1Rect( void );
		RECT GetRuler2Rect( void );

		//////////////////////////////////////////////////////////////////////////

		BOOL ShowTextEntryWindow( void );

		//////////////////////////////////////////////////////////////////////////
		
		void DocumentSetTexture( t_Handle Handle, int TextureIndex );
		t_Handle DocumentGetTextureHandle( void );
		int DocumentGetTextureIndex( void );
		t_Handle ThumbnailGetTextureHandle( void );
		int ThumbnailGetTextureIndex( void );
		ImageExplorer *ThumbnailGetImageExplorer( void );
		DirectoryImage *ImageExplorerGetDirectoryImage( void );

		//////////////////////////////////////////////////////////////////////////		

		ThumbnailView *m_pThumbnailView;
		ProjectView *m_pProjectView;
		GameView *m_pGameView;
		RulerBar m_RulerBar1;
		RulerBar m_RulerBar2;
		

		int m_ThumbnailPictureSize;
		int m_ThumbnailRectHeight;
		int m_SideBarSize;
		int m_RulerSize;

	protected:
		virtual void OnDraw( CDC *pdc );
		
		BOOL CreateThumbnailView( void );
		BOOL CreateProjectView( void );
		BOOL CreateGameView( void );
		BOOL CreateRulerBars( void );

	protected:
		afx_msg LRESULT OnDirectoryChanged( WPARAM wParam, LPARAM lParam );
		afx_msg int OnCreate( LPCREATESTRUCT pcs );
		afx_msg BOOL OnEraseBkgnd( CDC *pdc );
		afx_msg void OnMouseMove( UINT Flags, CPoint p );
		afx_msg void OnSize( UINT Type, int x, int y );
		afx_msg void OnTextureLocate( void );
		afx_msg BOOL OnThumbnailSelect( UINT Id, NMHDR *pNotifyStruct, LRESULT *pResult );
		afx_msg BOOL OnNotifyClick( UINT Id, NMHDR* pNotify, LRESULT* pResult );

	protected:
		DECLARE_MESSAGE_MAP();
};

#endif // _VIEW_H_