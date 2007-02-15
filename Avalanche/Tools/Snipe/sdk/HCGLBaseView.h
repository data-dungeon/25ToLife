#pragma once

#include "HCGLControl.h"
#include "Enum.h"
#include "Ray.h"

class HPickHitArray;
class HPickHit;
class HCBaseDoc;
class HManipulatorPlugin;
class HSnipeObjectList;

class SNIPEEXPORT HCGLBaseView : public HCGLControl
{
public:
   HCBaseDoc *GetDocument();
   SnipeViewType GetSVT() const;
   const HCGLBaseView *IsKindOf( SnipeViewType nType ) const;
   void DoPick( const CRect &rcPick, HPickHitArray *pPickHitArray, BOOL *bOutSinglePick=NULL, SnipeObjectType kindof=SOT_Unknown, HSnipeObjectList *pExcludeList=NULL );
   HManipulatorPlugin *GetManipulator() const;
   void HitTestManipulator( CPoint point, HPickHit *pPickHit ) const;
   void DefaultZoomFit(HSnipeObjectList *pList);

   BOOL SetCurrentSelectionSOT( SnipeObjectType nSOT );
   SnipeObjectType GetCurrentSelectionSOT() const;
   UINT GetNumSelectionSOTs() const;
   SnipeObjectType GetSelectionSOTAt( UINT nIndex ) const;

   float GetScreenPixelSize( float fDistance ) const;
   void ViewToWorld( Vector &p, BOOL bRotateOnly=FALSE ) const;
   void WorldToView( Vector &p ) const;
   void ScreenToWorld( Vector &p, BOOL bRotateOnly=FALSE ) const;
   void WorldToScreen( Vector &p ) const;
   BOOL Project( Vector &p ) const;
   BOOL UnProject( Vector &p ) const;
   const Matrix44 &GetWorldToViewMatrix() const;
   const Matrix44 &GetViewToWorldMatrix() const;

   Ray GetScreenMouseRay( const CPoint &point );
   Ray GetWorldMouseRay( const CPoint &point );
};
