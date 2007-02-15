//

#include "stdafx.h"
#include "AMViewerView.h"

static int f_prevrollx, f_prevtiltx, f_prevy;
static Vector f_startrotate;

void CAMViewerView::StartTurn()
{
   f_prevrollx = f_prevtiltx = m_mouse.x;
   f_prevy = m_mouse.y;
   f_startrotate = m_rotate;
}

void CAMViewerView::DoTurn()
{
   if (GetAsyncKeyState(VK_CONTROL)<0) {
      m_rotate.z = f_startrotate.z - f_prevtiltx + m_mouse.x;
      f_startrotate.y = m_rotate.y;
      f_prevrollx = m_mouse.x;
   }
   else {
      m_rotate.x = f_startrotate.x - f_prevy + m_mouse.y;
      m_rotate.y = f_startrotate.y - f_prevrollx + m_mouse.x;
      f_startrotate.z = m_rotate.z;
      f_prevtiltx = m_mouse.x;
   }
   m_matrix = YXZRotate( -m_rotate.y, -m_rotate.x, -m_rotate.z ).Transpose();
   ComputeSpan();
   InvalidateRect(NULL, FALSE);
   UpdateWindow();
}
