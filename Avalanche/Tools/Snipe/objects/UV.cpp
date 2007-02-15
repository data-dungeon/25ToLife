#include "stdafx.h"
#include "UV.h"
#include "UVEdge.h"
#include "gui\Mainfrm.h"

IMPLEMENT_COMPONENT_CREATE( UV, UV )

extern CMainFrame *g_pMainFrame;

UV::UV()
{
}

UV::UV( UINT nID )
: PropertyVertex( nID )
{
}

UV::~UV()
{
}

const Vector2 &UV::GetPos() const 
{ 
   return m_pos; 
}

void UV::SetPos( const Vector2 &pos)
{
   m_pos = pos;
   SetDirty();

   if (IsSelected())
      g_pMainFrame->GetMainInterface()->GetCurrentSelectionList()->SetBoundsDirty(); // master to use global
}

BOOL UV::GetWorldPos( Vector &pos )
{
   pos = m_pos;

   return TRUE;
}

void UV::Transform( const Matrix44 &matrix )
{
   if (m_bTransformed || IsLocked())
      return;

   Vector2 pos = m_pos;
   pos *= matrix;
   SetPos( pos );

   m_bTransformed = TRUE;
}

void UV::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   Component::Save( bs );

   bs << sp << GetPos() << nl;
}

BOOL UV::Load( BaseStream &bs )
{
   if (!Component::Load( bs ))
      return FALSE;

   Vector2 pos;
   bs >> pos;
   SetPos( pos );

   return TRUE;
}

void UV::SetPosFromPointAlongEdge( float fPosition, PropertyEdge *pEdge, PropertyPoly *pPoly )
{
   SetPos(((UVEdge*)pEdge)->GetPosAlongEdge(fPosition,(UVPoly*)pPoly));
}

void UV::Draw( CGLBaseView *pView )
{
   glBegin( GL_POINTS );
      glVertex2fv( &GetPos().x );
   glEnd();
}
