#if (!defined __WND_THIN_FRAME_H)
#define __WND_THIN_FRAME_H

class CProfStudioThinFrame : public CWnd
{
	bool m_bAutoDestroyOnPostNcDestroy;
public:
	CProfStudioThinFrame()
		: m_bAutoDestroyOnPostNcDestroy( false )
	{
	}

	BOOL Create(
		CWnd * pParentWnd,
		UINT nID
		)
	{
		return
			CWnd::Create(
			AfxRegisterWndClass(0), NULL,
			WS_CHILD|WS_VISIBLE,
			CRect( 0,0,0,0 ),
			pParentWnd,
			nID,
			NULL
			);
	}

	BOOL CreateDynamicThinFrame(
		CWnd * pChildWnd
		)
	{
		ASSERT( !m_bAutoDestroyOnPostNcDestroy );
		ASSERT_VALID( pChildWnd );
		CWnd * pParentWnd = pChildWnd->GetParent();
		ASSERT_VALID( pParentWnd );
		UINT nID = pChildWnd->GetDlgCtrlID();
		if( ! Create( pParentWnd, nID ) )
		{
			ASSERT( FALSE );
			return FALSE;
		}
		pChildWnd->SetParent( this );
		m_bAutoDestroyOnPostNcDestroy = true;
		return TRUE;
	}

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch( message )
		{
		case WM_SETFOCUS:
			{
				HWND hWndChild = ::GetWindow( GetSafeHwnd(), GW_CHILD );
				if( hWndChild != NULL )
				{
					::SetFocus( hWndChild );
					return 0;
				}
			}
			break;
		case WM_SIZE:
			if( wParam != SIZE_MINIMIZED )
			{
				HWND hWndChild = ::GetWindow( m_hWnd, GW_CHILD );
				if( hWndChild != NULL )
				{
					::MoveWindow(
						hWndChild,
						0,
						0,
						LOWORD(lParam),
						HIWORD(lParam),
						FALSE
						);
					RedrawWindow(
						NULL, NULL,
						RDW_INVALIDATE|RDW_UPDATENOW
						|RDW_ERASE|RDW_ERASENOW
						|RDW_ALLCHILDREN
						);
				} // if( hWndChild != NULL )
			}
			return 0;
		case WM_NCCALCSIZE:
			{
				NCCALCSIZE_PARAMS * pNCCSP =
					reinterpret_cast < NCCALCSIZE_PARAMS * > ( lParam );
				ASSERT( pNCCSP != NULL );
				CRect rcWnd( pNCCSP->rgrc[0] );
				rcWnd.DeflateRect( 2, 2 );
				::CopyRect( &(pNCCSP->rgrc[0]), rcWnd );
				return 0;
			}
		case WM_ERASEBKGND:
			return 1;
		case WM_NCPAINT:
			{
				CRect rcChildWnd, rcClient;
				GetWindowRect( &rcChildWnd );
				GetClientRect( &rcClient );
				ClientToScreen( &rcClient );
				if( rcChildWnd == rcClient )
					return 0;
				CPoint ptDevOffset = -rcChildWnd.TopLeft();
				rcChildWnd.OffsetRect( ptDevOffset );
				rcClient.OffsetRect( ptDevOffset );

				CWindowDC dc( this );
				ASSERT( dc.GetSafeHdc() != NULL );
				dc.ExcludeClipRect( &rcClient );

				dc.FillSolidRect(
					&rcChildWnd,
					g_PaintManager->GetColor( COLOR_3DFACE )
					);

				COLORREF clrThinFrame = g_PaintManager->GetColor( COLOR_3DSHADOW );
				dc.Draw3dRect(
					&rcChildWnd,
					clrThinFrame,
					clrThinFrame
					);

				return 0;
			}
		case WM_PAINT:
			{
				HWND hWndChild = ::GetWindow( m_hWnd, GW_CHILD );
				if( hWndChild == NULL )
					break;
				if( !CExtPaintManager::stat_DefIsHwndNeedsDirectRepaint(hWndChild) )
					break;
				CRect rcClient;
				GetClientRect( &rcClient );
				CPaintDC dc( this );
				bool bNoFill = false;
				if( g_PaintManager->GetCb2DbTransparentMode(this) )
					bNoFill = g_PaintManager->PaintDockerBkgnd( dc, this );
				if( !bNoFill )
					dc.FillSolidRect(
					&rcClient,
					g_PaintManager->GetColor(
					CExtPaintManager::CLR_3DFACE_OUT
					)
					);
				return 1;
			}
		} // switch( message )

		return CWnd::WindowProc(message,wParam,lParam);
	}

	virtual void PostNcDestroy()
	{
		CWnd::PostNcDestroy();
		if( m_bAutoDestroyOnPostNcDestroy )
			delete this;
	}

}; // class CProfStudioThinFrame

#endif // __WND_THIN_FRAME_H

