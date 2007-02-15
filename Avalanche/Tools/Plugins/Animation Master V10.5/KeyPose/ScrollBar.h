// Dan  7/9/2004  \DanLP110\Plugin\HXT\Avalanche\AnimationMaster\KeyPose\ScrollBar.h

#define __SCROLLBAR_H

#ifndef __DATATYPE_H
#include "DataType.h"
#endif
#define SCROLLBARSIZE  16
#define MINKNOBSIZE    16
#define VSCROLLTIMERID 1600

class HashScrollBar 
{
private:
   CWnd *m_wndParent;
   CRect m_rcAll, m_rcScroll, m_rcUp, m_rcDown;
	int   m_min;
	int   m_max;
	int   m_line;
	int   m_page;
	int   m_pos;
	int   m_prevpos, m_clickoffset;
	BITFIELDS32 m_isuppushed       : 1,
	            m_isdownpushed     : 1,
	            m_isscrolling      : 1,
               m_isupdownatbottom : 1,
               m_isvisible        : 1,
               m_ismousecapture   : 1,
               m_isfirsttimer     : 1,
               m_ishorizontal     : 1;

public:
         HashScrollBar();
         HashScrollBar( CWnd *parent, BOOL ishorizontal );
   void  Init();

   void  SetParent( CWnd *parent ) { m_wndParent = parent; }

   CRect GetRect() { return m_rcAll; }
   void  SetRect( const CRect &rect );

	int   GetMin() { return m_min; }
	int   SetMin( int min );

	int   GetMax() { return m_max; }
	int   SetMax( int max );

	int   GetLine() { return m_line; }
	int   SetLine( int line );

	int   GetPage() { return m_page; }
	int   SetPage( int page );

	int   GetPos() const { return m_pos; }
   int   GetPos( int point );
	int   SetPos( int pos );
   int   GetScrollPos() const;
   int   SetScrollPos( int nPos, BOOL bRedraw=TRUE );
   BOOL  SetScrollInfo( LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE );
   void  GetScrollRange( LPINT lpMinPos, LPINT lpMaxPos ) const;

   BOOL  IsMouseCapture() { return m_ismousecapture; }

   BOOL  IsVisible() { return m_isvisible; }
   void  Show( BOOL show );

	CRect GetKnobRect();
   void  Invalidate();
   void  Draw( CDC *dc );

   void  OnLButtonDown( CPoint point );
   void  OnLButtonUp( CPoint point );
   void  OnMouseMove( CPoint point );
   void  OnTimer();
   void  SetHorizontal( BOOL ishorizontal );
};

