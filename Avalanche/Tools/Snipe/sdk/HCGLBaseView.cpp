// 
#include "stdafx.h"
#include "HCGLBaseView.h"
#include "../GUI/GLBaseView.h"
#include "HHitArray.h"
#include "Interface/SnipeManipulator.h"
#include "Interface/GlHitArray.h"

void HCGLBaseView::DoPick( const CRect &rcPick, HPickHitArray *pPickHitArray, BOOL *bOutSinglePick/*=NULL*/, SnipeObjectType kindof/*=SOT_Unknown*/, HSnipeObjectList *pExcludeList/*=NULL*/ )
{
 	AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((CGLBaseView *)this)->DoPick( rcPick, *((PickHitArray *)pPickHitArray), bOutSinglePick, kindof, (SnipeObjectList *)pExcludeList );
}

HManipulatorPlugin *HCGLBaseView::GetManipulator() const
{
   if (((CGLBaseView *)this)->GetManipulatorInstance())
      return (HManipulatorPlugin *)((CGLBaseView *)this)->GetManipulatorInstance()->m_pPlugin;
   else
      return NULL;
}

HCBaseDoc *HCGLBaseView::GetDocument()
{
 	AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HCBaseDoc *)((CGLBaseView *)this)->GetDocument();
}

SnipeViewType HCGLBaseView::GetSVT() const
{
   return ((CGLBaseView *)this)->GetSVT();
}

const HCGLBaseView *HCGLBaseView::IsKindOf( SnipeViewType nType ) const
{
   return (const HCGLBaseView *)((CGLBaseView *)this)->IsKindOf( nType );
}

void HCGLBaseView::HitTestManipulator( CPoint point, HPickHit *pPickHit ) const
{
 	AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((CGLBaseView *)this)->HitTestManipulator( point, *((PickHit *)pPickHit) );
}

float HCGLBaseView::GetScreenPixelSize( float fDistance ) const
{
   return ((CGLBaseView *)this)->GetScreenPixelSize( fDistance );
}

void HCGLBaseView::ViewToWorld( Vector &p, BOOL bRotateOnly/*=FALSE*/ ) const
{
   ((CGLBaseView *)this)->ViewToWorld( p, bRotateOnly );
}

void HCGLBaseView::WorldToView( Vector &p ) const
{
   ((CGLBaseView *)this)->WorldToView( p );
}

void HCGLBaseView::ScreenToWorld( Vector &p, BOOL bRotateOnly/*=FALSE*/ ) const
{
   ((CGLBaseView *)this)->ScreenToWorld( p, bRotateOnly );
}

void HCGLBaseView::WorldToScreen( Vector &p ) const
{
   ((CGLBaseView *)this)->WorldToScreen( p );
}

BOOL HCGLBaseView::Project( Vector &p ) const
{
   return ((CGLBaseView *)this)->Project( p );
}

BOOL HCGLBaseView::UnProject( Vector &p ) const
{
   return ((CGLBaseView *)this)->UnProject( p );
}

const Matrix44 &HCGLBaseView::GetWorldToViewMatrix() const
{
   return ((CGLBaseView *)this)->GetWorldToViewMatrix();
}

const Matrix44 &HCGLBaseView::GetViewToWorldMatrix() const
{
   return ((CGLBaseView *)this)->GetViewToWorldMatrix();
}

void HCGLBaseView::DefaultZoomFit(HSnipeObjectList *pList)
{
   ((CGLBaseView *)this)->DefaultZoomFit((SnipeObjectList *)pList);
}

BOOL HCGLBaseView::SetCurrentSelectionSOT( SnipeObjectType nSOT )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((CGLBaseView *)this)->SetCurrentSelectionSOT( nSOT );
}

SnipeObjectType HCGLBaseView::GetCurrentSelectionSOT() const
{
   return ((CGLBaseView *)this)->GetCurrentSelectionSOT();
}

UINT HCGLBaseView::GetNumSelectionSOTs() const
{
   return ((CGLBaseView *)this)->GetNumSelectionSOTs();
}

SnipeObjectType HCGLBaseView::GetSelectionSOTAt( UINT nIndex ) const
{
   return ((CGLBaseView *)this)->GetSelectionSOTAt( nIndex );
}
