//

#include "stdafx.h"
#include "AMViewerView.h"

static CPoint f_start;
static Vector f_startoffset;

void CAMViewerView::StartMove()
{
   f_start = m_mouse;
   f_startoffset = m_offset;
}

void CAMViewerView::DoMove()
{
   float perspective;
   if (m_bperspective)
      perspective = m_focaldistance/m_focallength;
   else
      perspective = 1.0f;

   m_offset.x = (m_mouse.x - f_start.x)/m_zoom*perspective;
   m_offset.y = (f_start.y - m_mouse.y)/m_zoom*perspective;
   m_offset.z = 0;
   m_offset *= ToMatrix33(m_matrix).Transpose();
   m_offset += f_startoffset;
   ComputeSpan();
   InvalidateRect(NULL, FALSE);
   UpdateWindow();
}


