#include "stdafx.h"
#include "Snipe.h"
#include "GLProgressControl.h"
#include "GLRenderWnd.h"
#include "Resource.h"

static LARGE_INTEGER f_prevtime;
static LARGE_INTEGER f_frequency;

GLImageList CGLProgressControl::m_glImageListProgress;

CGLProgressControl::CGLProgressControl( BOOL bAutoHide/*=FALSE*/ )
{
   m_nPixelPos = -1;
   m_nPos = 0;
   m_nStep = 1;
   m_nMax = 100;
   m_bAutoHide = bAutoHide;

   QueryPerformanceFrequency(&f_frequency);
}

CGLProgressControl::~CGLProgressControl()
{
}

void CGLProgressControl::PostInit()
{
   CGLControl::PostInit();

   if (m_glImageListProgress.IsEmpty())
      m_glImageListProgress.Create( IDB_PROGRESSCONTROL, CRect(5, 0, 7, 15) );

   if (m_bAutoHide)
      SetVisible( FALSE );
}

void CGLProgressControl::SetPixelPos( int nPixelPos )
{
   if (m_nPixelPos == nPixelPos)
      return;

   m_nPixelPos = nPixelPos;

   LARGE_INTEGER curtime;
   QueryPerformanceCounter(&curtime);
   double elapsed = (double)(curtime.QuadPart - f_prevtime.QuadPart)/f_frequency.QuadPart;
   if ((elapsed > 0.1 || GetPos()==GetMax()) && IsVisible()) {
      glScissor( 0,0,0,0 );
      Draw( DRAW_PARENT );
      Update();
      f_prevtime = curtime;
   }
}

void CGLProgressControl::SetPos( int nPos )
{
   if (nPos > m_nMax)
      nPos = m_nMax;

   if (m_nPos == nPos)
      return;

   m_nPos = nPos;

   SetPixelPos( ((float)GetPos() / GetMax()) * m_szControl.cx );
}

void CGLProgressControl::SetStep( int nStep )
{
   m_nStep = nStep;
}

void CGLProgressControl::SetMax( int nMax )
{
   m_nMax = nMax;
}

void CGLProgressControl::Start( const String &strMessage, int nTotalItems, int nStep/*=1*/ )
{
   SetStep( nStep );
   SetMax( nTotalItems );
   SetPos( 0 );

   if (m_bAutoHide)
   {
      SetVisible( TRUE );
      glScissor( 0,0,0,0 );
      Draw( DRAW_PARENT );
      Update();
   }
}

void CGLProgressControl::Step()
{
   SetPos( GetPos() + GetStep() );
}

void CGLProgressControl::Stop()
{
   if (m_bAutoHide)
   {
      SetVisible( FALSE );
      glScissor( 0,0,0,0 );
      m_pParentControl->Draw( DRAW_PARENT );
      m_pParentControl->Update();
   }
}

void CGLProgressControl::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLControl::Move( ptControl, szControl, justification );

   SetPixelPos( ((float)GetPos() / GetMax()) * m_szControl.cx );
}

void CGLProgressControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   m_glImageListProgress.DrawDiced( CRect( CPoint(0,0), CSize(m_nPixelPos, m_szControl.cy)));
}
