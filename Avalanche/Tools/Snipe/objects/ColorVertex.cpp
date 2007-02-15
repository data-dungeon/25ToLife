#include "stdafx.h"
#include "ColorVertex.h"
#include "ColorEdge.h"
#include "gui\Mainfrm.h"

IMPLEMENT_COMPONENT_CREATE( ColorVertex, ColorVertex )

extern CMainFrame *g_pMainFrame;

ColorVertex::ColorVertex()
{
}

ColorVertex::ColorVertex( UINT nID )
: PropertyVertex( nID )
{
}

ColorVertex::~ColorVertex()
{
}

const RGBAFloat &ColorVertex::GetColor() const 
{ 
   return m_color; 
}

void ColorVertex::SetColor( const RGBAFloat &color )
{
   m_color = color;
   SetDirty();

   if (IsSelected())
      g_pMainFrame->GetMainInterface()->GetCurrentSelectionList()->SetBoundsDirty(); // master to use global
}

/*
BOOL ColorVertex::GetWorldPos( Vector &pos )
{
   m_color = color;
   return TRUE;
}


void ColorVertex::Transform( const Matrix44 &matrix )
{
   if (m_bTransformed || IsLocked())
      return;

   Vector color = m_color;
   color *= matrix;
   SetPos( pos );

   m_bTransformed = TRUE;
}
*/

void ColorVertex::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   Component::Save( bs );

   bs << sp << GetColor() << nl;
}

BOOL ColorVertex::Load( BaseStream &bs )
{
   if (!Component::Load( bs ))
      return FALSE;

   RGBAFloat color;
   bs >> color;
   SetColor( color );

   return TRUE;
}

void ColorVertex::SetPosFromPointAlongEdge( float fPosition, PropertyEdge *pEdge, PropertyPoly *pPoly )
{
   SetPos(((ColorEdge*)pEdge)->GetPosAlongEdge(fPosition,(ColorPoly*)pPoly));
}

