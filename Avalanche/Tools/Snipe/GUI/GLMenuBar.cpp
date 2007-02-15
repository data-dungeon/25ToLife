//

#include "stdafx.h"
#include "GLMenuBar.h"

void CGLMenuBarControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already
/*
   // Background
   RGBAFloat c[4] = {
      RGBAFloat(0.70f,0.70f,0.70f,1.0f),
      RGBAFloat(0.85f,0.85f,0.85f,1.0f),
      RGBAFloat(0.85f,0.85f,0.85f,1.0f),
      RGBAFloat(0.70f,0.70f,0.70f,1.0f),
   };
   DrawGradient( c );

   // Bottom border
   glColor4f( 0.0f, 0.0f, 0.0f, 0.3f );
   glBegin( GL_LINES );
   glVertex2f( 0, m_szControl.cy-0.5f );
   glVertex2f( m_szControl.cx, m_szControl.cy-0.5f );
   glEnd();
*/
   glColor3f( 0.0f, 0.0f, 0.0f );
   DrawText( 6, 12, "File    View   Tools    Help" );

   CGLControl::Draw( nDrawFlags );
}

