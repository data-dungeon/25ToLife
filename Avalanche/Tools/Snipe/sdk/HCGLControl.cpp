//

#include "stdafx.h"
#include "HCGLControl.h"
#include "../GUI/GLControls.h"

void HCGLControl::Push2DAttributes()
{
   CGLControl *pControl = (CGLControl *)this;
   ASSERT( pControl->m_pPrepper == NULL );
   pControl->m_pPrepper = SNEW GLDrawPrepper( pControl );
}

void HCGLControl::PopAttributes()
{
   CGLControl *pControl = (CGLControl *)this;
   ASSERT( pControl->m_pPrepper != NULL );
   delete pControl->m_pPrepper;
   pControl->m_pPrepper = NULL;
}

void HCGLControl::Invalidate()
{
   ((CGLControl *)this)->Invalidate();
}

CSize HCGLControl::GetTextExtent( const String &text )
{
   return ((CGLControl *)this)->GetTextExtent( text );
}

void HCGLControl::DrawText( float x, float y, const String &text )
{
   ((CGLControl *)this)->DrawText( x, y, text );
}
