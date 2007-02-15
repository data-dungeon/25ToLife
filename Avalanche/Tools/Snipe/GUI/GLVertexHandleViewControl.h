#pragma once

#include "GLViewControl.h"

class CGLBaseView;
class VertexHandle;
class PatchEdge;
class PatchVertex;

class CGLVertexHandleViewControl : public CGLViewControl
{
protected:
   PatchEdge *m_pPatchEdge;
   PatchVertex *m_pPatchVertex;
   VertexHandle *m_pVertexHandle;
   PatchEdge *m_pOppositeEdge;
   VertexHandle *m_pOppositeVertexHandle;

   BOOL m_bIsDragging;

   Vector m_vStart;
   MouseOnLine m_mouseonline;

public:
   CGLVertexHandleViewControl( CGLBaseView *pBaseView, PatchEdge *pPatchEdge, PatchVertex *pPatchVertex );

   // CGLViewControl overrides
   virtual BOOL IsFor( void *pData );

   virtual void Draw();
   virtual BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
   virtual BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
   virtual BOOL OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
   virtual KeyModifiers GetKeyModifiers( MouseStates mousestate, const PickHit &pickHit );

};
