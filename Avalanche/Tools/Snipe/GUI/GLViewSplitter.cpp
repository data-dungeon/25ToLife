
#include "stdafx.h"
#include "Snipe.h"
#include "GLViewSplitter.h"
#include "GLRenderWnd.h"
#include "Resource.h"

GLImageList CGLSplitterControl::m_glImageListKnob;

IMPLEMENT_SERIAL(CGLSplitterControl, CGLControl, 1)

BEGIN_MESSAGE_MAP(CGLSplitterControl, CGLControl)
   ON_COMMAND(ID_SPLITTER_ADDHORIZONTALSPLIT, OnSplitterAddHorizontalSplit)
   ON_COMMAND(ID_SPLITTER_ADDVERTICALSPLIT, OnSplitterAddVerticalSplit)
   ON_COMMAND(ID_SPLITTER_REMOVEHORIZONTALSPLIT, OnSplitterRemoveHorizontalSplit)
   ON_COMMAND(ID_SPLITTER_REMOVEVERTICALSPLIT, OnSplitterRemoveVerticalSplit)
END_MESSAGE_MAP()

CGLSplitterControl::CGLSplitterControl()
{
   m_nID = IDR_SPLITTER;
   m_pDynamicControlClass = NULL;
   m_nHandleSize = DEFAULTHANDLESIZE;
   m_bFixedSize = FALSE;
}

CGLSplitterControl::CGLSplitterControl( CRuntimeClass *pDynamicControlClass, int nHandleSize/*=DEFAULTHANDLESIZE*/ )
{
   m_nID = IDR_SPLITTER;
   m_pDynamicControlClass = pDynamicControlClass;
   m_nHandleSize = nHandleSize;
   m_bFixedSize = FALSE;
}

CGLSplitterControl::CGLSplitterControl( CGLControl *pFirstCell, int nInitialWidth, int nInitialHeight, int nHandleSize/*=DEFAULTHANDLESIZE*/ )
{
   m_nID = IDR_SPLITTER;
   m_pDynamicControlClass = pFirstCell->GetRuntimeClass();
   m_nHandleSize = nHandleSize;
   m_bFixedSize = FALSE;

   Init( pFirstCell->m_pParentControl );
   pFirstCell->m_pParentControl->m_childcontrolarray.RemoveAt( pFirstCell->m_pParentControl->FindChild( this ));
   m_rowheightarray.Add( 100.0f );
   m_colwidtharray.Add( 100.0f );
   m_childcontrolarray.SetSize( m_colwidtharray.GetCount() * m_rowheightarray.GetCount());

   NotifySwapChild swapchild( pFirstCell, this );
   pFirstCell->m_pParentControl->OnControlNotify( NOTIFYCTRL_SWAPCHILD, 0, (LPARAM)&swapchild );
   pFirstCell->m_pParentControl = this;

   SetAt( 0, 0, pFirstCell );

   for (int i=1; i<nInitialWidth; i++)
      AddColumn();
   for (int i=1; i<nInitialHeight; i++)
      AddRow();

   m_pParentControl->Move( m_pParentControl->m_ptControl, m_pParentControl->GetControlSize() );
//   Move( pFirstCell->m_ptControl, pFirstCell->GetControlSize() );
}

CGLSplitterControl::~CGLSplitterControl()
{
}

void CGLSplitterControl::PostInit()
{
   CGLControl::PostInit();

   if (m_glImageListKnob.IsEmpty())
      m_glImageListKnob.Create( IDB_SPLITTERKNOB );
}

void CGLSplitterControl::SetSize( int nCols, int nRows )
{
   m_colwidtharray.RemoveAll();
   for (int nCol=0; nCol<nCols; nCol++)
      m_colwidtharray.Add( 100.0f );

   m_rowheightarray.RemoveAll();
   for (int nRow=0; nRow<nRows; nRow++)
      m_rowheightarray.Add( 100.0f );
}

void CGLSplitterControl::AddColumn()
{
   CGLControl *pFirstCell = NULL;
   if (GetRowCount() && GetColumnCount())
      pFirstCell = GetAt( 0, 0 );

   m_colwidtharray.Add( 100.0f );
   m_childcontrolarray.SetSize( m_colwidtharray.GetCount() * m_rowheightarray.GetCount());

   // Transfer old cells to new locations
   for (int nRow=m_rowheightarray.GetCount()-1; nRow>=0; nRow--)
   {
      for (int nCol=m_colwidtharray.GetCount()-2; nCol>=0; nCol--)
      {
         CGLControl *pSource = m_childcontrolarray.GetAt( nRow * (m_colwidtharray.GetCount()-1) + nCol );
         SetAt( nCol, nRow, pSource );
      }
   }

   // Fill in new column
   for (int nRow=0; nRow<m_rowheightarray.GetCount(); nRow++)
   {
      if (m_pDynamicControlClass)
      {
         CGLControl *pSource = (CGLControl *)m_pDynamicControlClass->CreateObject();
         if (pFirstCell)
            pSource->OnSplit( pFirstCell );
         pSource->Init( this );
         // remove the one that was added by the above Init
         m_childcontrolarray.RemoveAt( m_childcontrolarray.GetCount()-1 );
         SetAt( m_colwidtharray.GetCount()-1, nRow, pSource );
      }
   }

   Move( m_ptControl, GetControlSize() );
}

void CGLSplitterControl::AddRow()
{
   CGLControl *pFirstCell = NULL;
   if (GetRowCount() && GetColumnCount())
      pFirstCell = GetAt( 0, 0 );

   m_rowheightarray.Add( 100.0f );

   // Fill in new column
   // horizontal is a little easier.  We just let the Init call add to the end of the array
   for (int nCol=0; nCol<m_colwidtharray.GetCount(); nCol++) {
      if (m_pDynamicControlClass) {
         CGLControl *pSource = (CGLControl *)m_pDynamicControlClass->CreateObject();
         if (pFirstCell)
            pSource->OnSplit( pFirstCell );
         pSource->Init( this );
      }
   }

   Move( m_ptControl, GetControlSize() );
}

void CGLSplitterControl::RemoveColumn()
{
   int nColCount = GetColumnCount();
   if (nColCount <= 1)
      return;

   for (int nRow=0; nRow<GetRowCount(); nRow++) {
      CGLControl *pControl = GetAt( nColCount-1, nRow );
      delete pControl;
   }
// TODO: resituate cells
   m_colwidtharray.RemoveAt( m_colwidtharray.GetCount()-1 );
   m_childcontrolarray.SetSize( m_colwidtharray.GetCount() * m_rowheightarray.GetCount());

   Move( m_ptControl, GetControlSize() );
}

void CGLSplitterControl::RemoveRow()
{
   int nRowCount = GetRowCount();
   if (nRowCount <= 1)
      return;

   for (int nCol=0; nCol<GetColumnCount(); nCol++) {
      CGLControl *pControl = GetAt( nCol, nRowCount-1 );
      delete pControl;
   }

// TODO: resituate cells
   m_rowheightarray.RemoveAt( m_rowheightarray.GetCount()-1 );
   m_childcontrolarray.SetSize( m_colwidtharray.GetCount() * m_rowheightarray.GetCount());

   Move( m_ptControl, GetControlSize() );
}

void CGLSplitterControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (m_nHandleSize < 9)
      glClear( GL_COLOR_BUFFER_BIT );
   else if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   // DRAW SPLITTER BARS
   glColor3f( 0.5f, 0.5f, 0.5f );

   float fTotalWidth=0;
   for (int nCol=0; nCol<GetColumnCount(); nCol++)
      fTotalWidth += GetColumnWidth( nCol );

   int x = 0;
   for (int nCol=0; nCol<GetColumnCount()-1; nCol++) {
      if (m_bFixedSize)
         if (GetColumnWidth( nCol ))
            x += GetColumnWidth( nCol ) - m_nHandleSize/2 + 0.5f;
         else
            x += (m_szControl.cx - fTotalWidth) - m_nHandleSize/2 + 0.5f;
      else
         x += GetColumnWidth( nCol ) / fTotalWidth * m_szControl.cx - m_nHandleSize/2 + 0.5f;
      if (m_nHandleSize < 9) {
         glBegin( GL_QUADS );
         glVertex3f( x+1, 0, 0 );
         glVertex3f( x+m_nHandleSize-1, 0, 0 );
         glVertex3f( x+m_nHandleSize-1, m_szControl.cy, 0 );
         glVertex3f( x+1, m_szControl.cy, 0 );
         glEnd();
      }
      else
         m_glImageListKnob.Draw( x + (m_nHandleSize - m_glImageListKnob.GetWidthEach()) / 2, (m_szControl.cy - m_glImageListKnob.GetHeight()) / 2 );
      x += m_nHandleSize;
   }

   float fTotalHeight=0;
   for (int nRow=0; nRow<GetRowCount(); nRow++)
      fTotalHeight += GetRowHeight( nRow );

   int y = 0;
   for (int nRow=0; nRow<GetRowCount()-1; nRow++) {
      if (m_bFixedSize)
         if (GetRowHeight( nRow ))
            y += GetRowHeight( nRow ) - m_nHandleSize/2 + 0.5f;
         else
            y += (m_szControl.cy - fTotalHeight) - m_nHandleSize/2 + 0.5f;
      else
         y += GetRowHeight( nRow ) / fTotalHeight * m_szControl.cy - m_nHandleSize/2 + 0.5f;
      if (m_nHandleSize < 9) {
         glBegin( GL_QUADS );
         glVertex3f( 0, y+1, 0 );
         glVertex3f( m_szControl.cx, y+1, 0 );
         glVertex3f( m_szControl.cx, y+m_nHandleSize-1, 0 );
         glVertex3f( 0, y+m_nHandleSize-1, 0 );
         glEnd();
      }
      else
         m_glImageListKnob.Draw( (m_szControl.cx - m_glImageListKnob.GetWidthEach()) / 2, y + (m_nHandleSize - m_glImageListKnob.GetHeight()) / 2 );
      y += m_nHandleSize;
   }

   CGLControl::Draw( nDrawFlags );
}

void CGLSplitterControl::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLControl::Move( ptControl, szControl, justification );

   CRect rcThisPane( CPoint(0,0), szControl );

   float fTotalWidth=0, fTotalHeight=0;
   for (int nCol=0; nCol<GetColumnCount(); nCol++)
      fTotalWidth += GetColumnWidth( nCol );
   for (int nRow=0; nRow<GetRowCount(); nRow++)
      fTotalHeight += GetRowHeight( nRow );

   CPoint topleft(0,0), bottomright;
   for (int nRow=0; nRow<GetRowCount(); nRow++)
   {
      topleft.x = 0;
      if (nRow == GetRowCount()-1)
         bottomright.y = rcThisPane.bottom;
      else if (m_bFixedSize)
         if (GetRowHeight( nRow ))
            bottomright.y = topleft.y + GetRowHeight( nRow ) - m_nHandleSize/2 + 0.5f;
         else
            bottomright.y = topleft.y + (rcThisPane.Height() - fTotalHeight) - m_nHandleSize/2 + 0.5f;
      else
         bottomright.y = topleft.y + GetRowHeight( nRow ) / fTotalHeight * rcThisPane.Height() - m_nHandleSize/2 + 0.5f;

      for (int nCol=0; nCol<GetColumnCount(); nCol++)
      {
         CGLControl *pPane = GetAt( nCol, nRow );
         if (nCol == GetColumnCount()-1)
            bottomright.x = rcThisPane.right;
         else if (m_bFixedSize)
            if (GetColumnWidth( nCol ))
               bottomright.x = topleft.x + GetColumnWidth( nCol ) - m_nHandleSize/2 + 0.5f;
            else
               bottomright.x = topleft.x + (rcThisPane.Width() - fTotalWidth) - m_nHandleSize/2 + 0.5f;
         else
            bottomright.x = topleft.x + GetColumnWidth( nCol ) / fTotalWidth * rcThisPane.Width() - m_nHandleSize/2 + 0.5f;

         CRect rcPane( topleft, bottomright );
         if (pPane)
            pPane->Move( rcPane.TopLeft(), rcPane.Size());

         topleft.x = bottomright.x + m_nHandleSize;
      }
      topleft.y = bottomright.y + m_nHandleSize;
   }
}

static int f_nLeft = 0;
static int f_nTop = 0;

BOOL CGLSplitterControl::HitTest( const CPoint &point, int &nColHit, int &nRowHit, bool bSetTopLeft )
{
   nColHit = nRowHit = -1;

   if (GetColumnCount() > 1) {
      float fTotalWidth=0;
      for (int nCol=0; nCol<GetColumnCount(); nCol++)
         fTotalWidth += GetColumnWidth( nCol );

      CRect rcSplitter( CPoint(0,0), m_szControl );
      for (int nCol=0; nCol<GetColumnCount(); nCol++) {
			if (bSetTopLeft)
				f_nLeft = rcSplitter.left;
         if (m_bFixedSize)
            if (GetColumnWidth( nCol ))
               rcSplitter.left = rcSplitter.left + GetColumnWidth( nCol ) - m_nHandleSize/2 + 0.5f;
            else
               rcSplitter.left = rcSplitter.left + (m_szControl.cx - fTotalWidth) - m_nHandleSize/2 + 0.5f;
         else
            rcSplitter.left = rcSplitter.left + GetColumnWidth( nCol ) / fTotalWidth * m_szControl.cx - m_nHandleSize/2 + 0.5f;
         rcSplitter.right = rcSplitter.left + m_nHandleSize;
         if (rcSplitter.PtInRect( point )) {
            nColHit = nCol;
            break;
         }
         rcSplitter.left = rcSplitter.right;
      }
   }

   if (GetRowCount() > 1) {
      float fTotalHeight=0;
      for (int nRow=0; nRow<GetRowCount(); nRow++)
         fTotalHeight += GetRowHeight( nRow );

      CRect rcSplitter( CPoint(0,0), m_szControl );
      for (int nRow=0; nRow<GetRowCount(); nRow++) {
			if (bSetTopLeft)
	         f_nTop = rcSplitter.top;
         if (m_bFixedSize)
            if (GetRowHeight( nRow ))
               rcSplitter.top = rcSplitter.top + GetRowHeight( nRow ) - m_nHandleSize/2 + 0.5f;
            else
               rcSplitter.top = rcSplitter.top + (m_szControl.cy - fTotalHeight) - m_nHandleSize/2 + 0.5f;
         else
            rcSplitter.top = rcSplitter.top + GetRowHeight( nRow ) / fTotalHeight * m_szControl.cy - m_nHandleSize/2 + 0.5f;
         rcSplitter.bottom = rcSplitter.top + m_nHandleSize;
         if (rcSplitter.PtInRect( point )) {
            nRowHit = nRow;
            break;
         }
         rcSplitter.top = rcSplitter.bottom;
      }
   }

   return TRUE;
}

static int f_nColHit = -1;
static int f_nRowHit = -1;

BOOL CGLSplitterControl::OnLButtonDown( UINT nFlags, CPoint point )
{
   return HitTest( point, f_nColHit, f_nRowHit, true );
}

BOOL CGLSplitterControl::OnMouseMove( UINT nFlags, CPoint point )
{
//   CRect rcViewport = GetViewportRect();

   if (f_nColHit != -1) {
      float fTotalWidth=0;
      for (int nCol=0; nCol<GetColumnCount(); nCol++)
         fTotalWidth += GetColumnWidth( nCol );

      if (m_bFixedSize)
      {
         int nWidthOfTheseTwo;
         if (GetColumnWidth( f_nColHit ))
            nWidthOfTheseTwo = GetColumnWidth( f_nColHit ) + 0.5f;
         else
            nWidthOfTheseTwo = (m_szControl.cx - fTotalWidth) + 0.5f;

         if (GetColumnWidth( f_nColHit + 1 ))
            nWidthOfTheseTwo += GetColumnWidth( f_nColHit + 1 ) + 0.5f;
         else
            nWidthOfTheseTwo += (m_szControl.cx - fTotalWidth) + 0.5f;

         if (m_colwidtharray[f_nColHit])
            m_colwidtharray[f_nColHit] = min( nWidthOfTheseTwo-200.0f, max( 200.0f, point.x - f_nLeft ));
         if (m_colwidtharray[f_nColHit+1])
            m_colwidtharray[f_nColHit+1] = min( nWidthOfTheseTwo-200.0f, max( 200.0f, nWidthOfTheseTwo - (point.x - f_nLeft)));
      }
      else
      {
         int nWidthOfTheseTwo = GetColumnWidth( f_nColHit ) / fTotalWidth * m_szControl.cx + 0.5f;
         nWidthOfTheseTwo += GetColumnWidth( f_nColHit+1 ) / fTotalWidth * m_szControl.cx + 0.5f;

         float fWidthOfTheseTwo = m_colwidtharray[f_nColHit] + m_colwidtharray[f_nColHit+1];
         float fPercent = (float)(point.x - f_nLeft) / nWidthOfTheseTwo;
         fPercent = min( 0.9f, max( 0.1f, fPercent ));

         m_colwidtharray[f_nColHit] = fWidthOfTheseTwo * fPercent;
         m_colwidtharray[f_nColHit+1] = fWidthOfTheseTwo * (1.0f - fPercent);
      }
   }

   if (f_nRowHit != -1) {
      float fTotalHeight=0;
      for (int nRow=0; nRow<GetRowCount(); nRow++)
         fTotalHeight += GetRowHeight( nRow );

      if (m_bFixedSize)
      {
         int nHeightOfTheseTwo;
         if (GetRowHeight( f_nRowHit ))
            nHeightOfTheseTwo = GetRowHeight( f_nRowHit ) + 0.5f;
         else
            nHeightOfTheseTwo = (m_szControl.cy - fTotalHeight) + 0.5f;

         if (GetRowHeight(f_nRowHit+1))
            nHeightOfTheseTwo += GetRowHeight( f_nRowHit + 1 ) + 0.5f;
         else
            nHeightOfTheseTwo += (m_szControl.cy - fTotalHeight) + 0.5f;

         if (GetRowHeight( f_nRowHit ))
            m_rowheightarray[f_nRowHit] = min( nHeightOfTheseTwo-100.0f, max( 100.0f, point.y - f_nTop ));
         if (GetRowHeight( f_nRowHit+1 ))
            m_rowheightarray[f_nRowHit+1] = min( nHeightOfTheseTwo-100.0f, max( 100.0f, nHeightOfTheseTwo - (point.y - f_nTop)));
      }
      else
      {
         int nHeightOfTheseTwo = GetRowHeight(f_nRowHit) / fTotalHeight * m_szControl.cy + 0.5f;
         nHeightOfTheseTwo += GetRowHeight(f_nRowHit+1) / fTotalHeight * m_szControl.cy + 0.5f;

         float fHeightOfTheseTwo = GetRowHeight(f_nRowHit) + GetRowHeight(f_nRowHit+1);
         float fPercent = (float)(point.y - f_nTop) / nHeightOfTheseTwo;
         fPercent = min( 0.9f, max( 0.1f, fPercent ));

         m_rowheightarray[f_nRowHit] = fHeightOfTheseTwo * fPercent;
         m_rowheightarray[f_nRowHit+1] = fHeightOfTheseTwo * (1.0f - fPercent);
      }
   }

   if (f_nColHit != -1 || f_nRowHit != -1) {
      Move( m_ptControl, GetControlSize() );
      GetApp().DrawInvalidControls();
   }

   return TRUE;
}

BOOL CGLSplitterControl::OnLButtonUp( UINT nFlags, CPoint point )
{
   f_nColHit = f_nRowHit = -1;
   return TRUE;
}

BOOL CGLSplitterControl::OnSetCursor( UINT message, CPoint point )
{
	// if already dragging splitter around
	if (f_nColHit != -1 || f_nRowHit != -1) {
		if (f_nColHit >= 0 && f_nRowHit >= 0)
			::SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZEALL ));
		else if (f_nColHit >= 0)
			::SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZEWE ));
		else if (f_nRowHit >= 0)
			::SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZENS ));
		else
			return FALSE;
		return TRUE;
	}
	else {
		int nColHit, nRowHit;
		if (HitTest( point, nColHit, nRowHit, false )) {
			if (nColHit >= 0 && nRowHit >= 0)
				::SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZEALL ));
			else if (nColHit >= 0)
				::SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZEWE ));
			else if (nRowHit >= 0)
				::SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZENS ));
			else
				return FALSE;
			return TRUE;
		}
	}
   return FALSE;
}

void CGLSplitterControl::OnSplitterAddHorizontalSplit()
{
   AddRow();
   GetApp().DrawInvalidControls();
}

void CGLSplitterControl::OnSplitterAddVerticalSplit()
{
   AddColumn();
   GetApp().DrawInvalidControls();
}

void CGLSplitterControl::OnSplitterRemoveHorizontalSplit()
{
   RemoveRow();
   GetApp().DrawInvalidControls();
}

void CGLSplitterControl::OnSplitterRemoveVerticalSplit()
{
   RemoveColumn();
   GetApp().DrawInvalidControls();
}

void CGLSplitterControl::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   CGLControlHeader header( this, bWriteHeader, bs );

   bs << DataHeader( "PaneClass", m_pDynamicControlClass->m_lpszClassName ) << nl;

   // Rows
   {
      DataHeader dataheader( bs, (String)"Rows="+GetRowCount() );
      for (int i=0; i<GetRowCount(); i++)
         bs << mar << GetRowHeight( i ) << nl;
   }

   // Columns
   {
      DataHeader dataheader( bs, (String)"Columns="+GetColumnCount() );
      for (int i=0; i<GetColumnCount(); i++)
         bs << mar << GetColumnWidth( i ) << nl;
   }

   CGLControl::Save( bs );
}

BOOL CGLSplitterControl::ParseData( BaseStream &bs, const DataHeader &dataheader )
{
   if (dataheader.m_strKey == "PaneClass")
   {
      m_pDynamicControlClass = CRuntimeClass::FromName( dataheader.m_strValue.Get() );
      return TRUE;
   }
   else if (dataheader.m_strKey == "Rows")
   {
      int nCount = atoi( dataheader.m_strValue.Get() );
      m_rowheightarray.SetSize( nCount );
      for (int i=0; i<nCount; i++)
      {
         bs.ReadMargin();
         float fHeight;
         bs >> fHeight;
         m_rowheightarray[i] = fHeight;
      }
      return TRUE;
   }
   else if (dataheader.m_strKey == "Columns")
   {
      int nCount = atoi( dataheader.m_strValue.Get() );
      m_colwidtharray.SetSize( nCount );
      for (int i=0; i<nCount; i++)
      {
         bs.ReadMargin();
         float fWidth;
         bs >> fWidth;
         m_colwidtharray[i] = fWidth;
      }
      return TRUE;
   }
   else
      return CGLControl::ParseData( bs, dataheader );
}
