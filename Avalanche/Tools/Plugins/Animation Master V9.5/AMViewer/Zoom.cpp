//

#include "stdafx.h"
#include "AMViewerDoc.h"
#include "AMViewerView.h"
#include "Model.h"

#define MINZOOM 1.0e-8
#define PADDING 1.1f
#define SMALL .001f

static CPoint f_prev, f_start;
static float f_startzoom, f_startfocaldistance;
static CPen   *oldpen, pen;
static CBrush *oldbrush;
static Vector f_startoffset, f_cameradir;

static BOOL IsRight()
{
   if (GetSystemMetrics(SM_SWAPBUTTON))
      return GetAsyncKeyState(VK_LBUTTON)<0;
   else
      return GetAsyncKeyState(VK_RBUTTON)<0;
}

void CAMViewerView::StartZoom()
{
   f_startzoom = m_zoom;
   f_start.x = m_mouse.x;
   f_start.y = m_mouse.y;
   if (m_bperspective) {
      f_startfocaldistance = m_focaldistance;
      f_startoffset = m_offset;
      f_cameradir = m_matrix.Transpose().GetZAxis().Normalized();
   }
}

void CAMViewerView::StartBoundZoom()
{
   CDC *dc = GetDC();
   f_start.x = f_prev.x = m_mouse.x;
   f_start.y = f_prev.y = m_mouse.y;

   dc->SelectClipRgn(NULL);
   pen.CreatePen(PS_DOT, 0, RGB(255,255,255));
   dc->SetROP2( R2_XORPEN );
   dc->SetBkMode( TRANSPARENT );
   oldbrush = (CBrush *)dc->SelectStockObject( NULL_BRUSH );
   oldpen = dc->SelectObject(&pen);
   dc->Rectangle( f_start.x, f_start.y, f_prev.x, f_prev.y );
   ReleaseDC(dc);
}

void CAMViewerView::DoZoom()
{
   if (m_bperspective) {
      float factor = (float)(m_mouse.x - f_start.x)* 10;
      m_offset = f_startoffset + factor * f_cameradir;
   }
   else {
      float factor = (float)m_mouse.x/(float)f_start.x;
      m_zoom = (float)max(f_startzoom * factor * factor, MINZOOM);
   }

   ComputeSpan();
   InvalidateRect(NULL, FALSE);
   UpdateWindow();
}

void CAMViewerView::DoBoundZoom()
{
   CDC *dc = GetDC();
   dc->Rectangle( f_start.x, f_start.y, f_prev.x, f_prev.y );
   dc->Rectangle( f_start.x, f_start.y, m_mouse.x, m_mouse.y );
   f_prev = m_mouse;
   ReleaseDC(dc);
}

void CAMViewerView::EndBoundZoom()
{
   CPoint delta;
   int temp;

   CDC *dc = GetDC();
   dc->Rectangle( f_start.x, f_start.y, f_prev.x, f_prev.y );
   dc->SetROP2( R2_COPYPEN );
   dc->SelectObject(oldbrush);
   dc->SelectObject(oldpen);
   pen.DeleteObject();
   ReleaseDC(dc);   

   if ((delta.x = f_prev.x - f_start.x) < 0) {
      delta.x = -delta.x;
      temp = f_start.x;
      f_start.x = f_prev.x;
      f_prev.x = temp;
   }   
   if ((delta.y = f_prev.y - f_start.y) < 0) {
      delta.y = -delta.y;
      temp = f_start.y;
      f_start.y = f_prev.y;
      f_prev.y = temp;
   }

   CPoint center;
   center.x = m_width/2;
   center.y = m_height/2;

   if (m_bperspective) {
      float perspective = m_focaldistance/m_focallength;
      m_offset.x = (f_prev.x+f_start.x)/2.0f-center.x;
      m_offset.y = (f_prev.y+f_start.y)/-2.0f+center.y;
      m_offset.z = -m_focaldistance;
      UnprojectNoNormalize(m_offset);
      m_offset *= m_matrix.InvertRotateTranslate();
      m_offset = -m_offset;
      float ratio;
      if ((float)delta.x/delta.y > (float)m_width/m_height)
         ratio = (float)delta.x/m_width;
      else
         ratio = (float)delta.y/m_height;
      Vector direction = m_matrix.Transpose().GetZAxis().Normalized();
      m_offset += (1.0f-ratio) * m_focaldistance * direction;
   }
   else {
      m_offset.x = m_offset.x - ((f_prev.x+f_start.x)/2-center.x)/m_zoom;
      m_offset.y = m_offset.y + ((f_prev.y+f_start.y)/2-center.y)/m_zoom;
      if ((float)delta.x/delta.y > (float)m_width/m_height)
         m_zoom *= (float)m_width/delta.x;
      else
         m_zoom *= (float)m_height/delta.y;
   }

   ComputeSpan();
   InvalidateRect(NULL, FALSE);
   UpdateWindow();
}

void CAMViewerView::ZoomFit()
{
   Vector min, max;

   Model *model = GetDocument()->m_model;

   if (model==NULL)
      return;

   min.Set(-model->m_span, 0.0f, -model->m_span);
   max.Set(model->m_span);

   m_offset.x = (max.x + min.x)/-2.0f;
   m_offset.y = (max.y + min.y)/-2.0f;
   m_offset.z = 0.0f;
   
   float deltax = max.x-min.x;
   float deltay = max.y-min.y;

   if (deltax < SMALL) {
	  deltax = SMALL;
	  if (deltay < SMALL)
	     return;
   }

   if (deltay < SMALL)
	  deltay = SMALL;

   if (m_bperspective) {
      float width = MAX( -min.x, max.x );
      width = MAX( width, -min.y );
      width = MAX( width, max.y );
      width *= 2;
      m_focaldistance = width * (m_focallength / 35.0f );// this should actually change offset instead of focaldistance
   }
   else {
      if (deltax/deltay > (float)m_width/(float)m_height )
         m_zoom = m_width/deltax/PADDING;
      else 
         m_zoom = m_height/deltay/PADDING;
   }
   ComputeSpan();
}

void CAMViewerView::OnZoomFit()
{
   ZoomFit();
   InvalidateRect(NULL, FALSE);
   UpdateWindow();
}
