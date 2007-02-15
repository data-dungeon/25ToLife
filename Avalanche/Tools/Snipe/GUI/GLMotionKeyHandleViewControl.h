#pragma once

#include "GLViewControl.h"


class MotionKey;

class CGLMotionKeyHandleViewControl : public CGLViewControl
{
protected:
   MotionKey *m_pMotionKey;

   BOOL m_bIsDragging;

public:
   CGLMotionKeyHandleViewControl( CGLBaseView *pBaseView, MotionKey *pMotionKey );

   // CGLViewControl overrides
   virtual BOOL IsFor( void *pData );

   virtual void Draw();
   virtual BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
   virtual BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
   virtual BOOL OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
   virtual KeyModifiers GetKeyModifiers( MouseStates mousestate, const PickHit &pickHit );
};
