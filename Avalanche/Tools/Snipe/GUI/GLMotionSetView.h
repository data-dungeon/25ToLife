#pragma once
#include "GLSceneView.h"

class CMotionSetDoc;

class CGLMotionSetView : public CGLSceneView
{
   DECLARE_BASEVIEW(CGLMotionSetView, CGLSceneView)

public:
   static TypeProperty::Info m_nViewModeInfo;
   TypeProperty              m_nViewMode; enum { VIEWMODE_ORIGINAL, VIEWMODE_REDUCED, VIEWMODE_COMPOSITED, VIEWMODE_SIDEBYSIDE };

            CGLMotionSetView();
   virtual ~CGLMotionSetView();

   CMotionSetDoc *GetDocument() const { return (CMotionSetDoc *)CGLBaseView::GetDocument(); }

// CGLControl overrides
   virtual void OnDragEnter( CPoint point, CGLControl *pDragControl );
   virtual void OnDragMove( CPoint point, CGLControl *pDragControl );
   virtual void OnDragLeave( CPoint point, CGLControl *pDragControl );
   virtual void OnDragDrop( CPoint point, CGLControl *pDragControl );
   virtual void DrawContents();

// CGLBaseView overrides
   virtual SnipeObjectTypeArray *GetSelectableObjectTypeArray() const;
   virtual void DoPick( const CRect &rcPick, PickHitArray &pickHitArray, BOOL *bOutSinglePick=NULL, SnipeObjectType kindof=SOT_Unknown, SnipeObjectList *pExcludeList=NULL ); // Use SOT_Known for Current Selection Type
};
