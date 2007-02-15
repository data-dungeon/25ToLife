//

#include "stdafx.h"
#include "Navigator.h"
#include "Resource.h"
#include "HCGL3DView.h"
#include "Vector2.h"
#include "HFloatProperty.h"
#include "HTransformProperty.h"
#include "HTranslateProperty.h"
#include "HRotateProperty.h"
#include "HCamera.h"
#include "Matrix44.h"
#include "BoundingBox.h"
#include "HSnipeObjectList.h"
#include "HGLImageList.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HMainInterface.h"

// Plugin ////////////////////////////////////////////////
SnipeNavigatorPlugin::SnipeNavigatorPlugin()
{
   m_pAnimateCameraInfo = HBoolProperty::HInfo::New( "Animate Camera", "AnimateCamera", FALSE );
   m_pDisplayTargetInfo = HBoolProperty::HInfo::New( "Display Target", "DisplayTarget", FALSE );
   m_pTargetInfo = HVectorProperty::HInfo::New( "Target", "Target", Vector(0.0f), Vector(-MAX_FLOAT), Vector(MAX_FLOAT) );
   m_pRotationInfo = HFloatProperty::HInfo::New( "Rotation", "Rotation", 0.0f, -180.0f, 180.0f );
   m_pAngleInfo = HFloatProperty::HInfo::New( "Angle", "Angle", 0.0f, -90.0f, 90.0f );

   m_pImageList = HGLImageList::New(IDB_TARGET);
}

SnipeNavigatorPlugin::~SnipeNavigatorPlugin()
{
   delete m_pAnimateCameraInfo;
   delete m_pDisplayTargetInfo;
   delete m_pTargetInfo;
   delete m_pRotationInfo;
   delete m_pAngleInfo;

   delete m_pImageList;
}

ManipulatorInstance *SnipeNavigatorPlugin::CreateInstance( HCGLBaseView *pView )
{
   return SNEW SnipeNavigator( this, pView );
}

const char *SnipeNavigatorPlugin::GetTitleName() const 
{ 
   return "Snipe"; 
}

UINT SnipeNavigatorPlugin::GetBitmapID() const
{
   return IDB_SNIPENAVIGATOR;
}

// Instance //////////////////////////////////////////////
SnipeNavigator::SnipeNavigator( SnipeNavigatorPlugin *pPlugin, HCGLBaseView *pView ) :
 NavigatorInstance( pPlugin, pView )
{
   m_bCalculateTransform = TRUE;
   m_bCalculateProperties = TRUE;

   m_nCurrentNavigation = NM_NONE;

   m_pPropertyArray = HPropertyArray::New();

   m_pAnimateCamera = HBoolProperty::New(pPlugin->m_pAnimateCameraInfo, NULL);
   m_pPropertyArray->Add( m_pAnimateCamera );

   m_pPerspective = NULL;
//   m_pViewAngle = NULL;
   if (pView->IsKindOf(SVT_CGL3DView))
   {
      m_nPerspectiveIndex = m_pPropertyArray->Add( m_pPerspective );
//      m_nViewAngleIndex = m_pPropertyArray->Add( m_pViewAngle );
   }
   else
   {
//      m_nViewAngleIndex = -1;
      m_nPerspectiveIndex = -1;
   }

   m_pDisplayTarget = HBoolProperty::New(pPlugin->m_pDisplayTargetInfo, NULL);
   m_pPropertyArray->Add( m_pDisplayTarget );

   m_pFOV = NULL;
   m_nFOVIndex = m_pPropertyArray->Add( m_pFOV );

   m_pFocusDistance = NULL;
   m_nFocusDistanceIndex = m_pPropertyArray->Add( m_pFocusDistance );

   m_pTarget = HVectorProperty::New(pPlugin->m_pTargetInfo, NULL);
   m_pPropertyArray->Add( m_pTarget );

   m_pRotation = HFloatProperty::New(pPlugin->m_pRotationInfo, NULL);
   if (pView->IsKindOf(SVT_CGL3DView))
      m_pPropertyArray->Add( m_pRotation );

	m_pAngle = HFloatProperty::New(pPlugin->m_pAngleInfo, NULL);
   if (pView->IsKindOf(SVT_CGL3DView))
      m_pPropertyArray->Add( m_pAngle );

   m_pManualClip = NULL;
   if (pView->IsKindOf(SVT_CGL3DView))
      m_nManualClipIndex = m_pPropertyArray->Add( m_pManualClip );
   else
      m_nManualClipIndex = -1;
   m_pNearClip = NULL;
   m_pFarClip = NULL;

   m_pAspectRatio = NULL;
   if (pView->IsKindOf(SVT_CGLTimeLineView))
   {
      m_nAspectRatioIndex = m_pPropertyArray->Add( m_pAspectRatio );
   }
   else
   {
      m_nAspectRatioIndex = -1;
   }

   for (int i=0; i < NUM3DVIEWANGLEPRESETS; i++ )
      m_ViewAngleDistance[i]=100.0f;

   ResetCameraProperties();
}

SnipeNavigator::~SnipeNavigator()
{
   delete m_pAnimateCamera;
   delete m_pDisplayTarget;
   delete m_pTarget;
   delete m_pRotation;
   delete m_pAngle;

   delete m_pPropertyArray;
}

void SnipeNavigator::ResetCameraProperties()
{
   HCGL2DView *p2DView = (HCGL2DView *)m_pView;
   HCamera *pCamera = p2DView->GetCurrentCamera();
   if (!pCamera)
      return;

   HFloatProperty *pFocusDistance = pCamera->GetFocusDistance();
   if (m_pFocusDistance != pFocusDistance)
   {
//      if (m_nViewAngleIndex != -1)
//      {
//         ASSERT( p2DView->IsKindOf(SVT_CGL3DView ));
//
//         m_pViewAngle = ((HCGL3DView *)p2DView)->GetViewAngleProperty();
//         m_pPropertyArray->SetAt( m_nViewAngleIndex, m_pViewAngle );
//      }
      if (m_nPerspectiveIndex != -1)
      {
         m_pPerspective = pCamera->GetPerspective();
         m_pPropertyArray->SetAt( m_nPerspectiveIndex, m_pPerspective );
      }

      m_pFOV = pCamera->GetFOV();
      m_pPropertyArray->SetAt( m_nFOVIndex, m_pFOV );

      m_pFocusDistance = pCamera->GetFocusDistance();
      m_pPropertyArray->SetAt( m_nFocusDistanceIndex, m_pFocusDistance );

      if (m_nManualClipIndex != -1)
      {
         m_pManualClip = pCamera->GetManualClip();
         m_pPropertyArray->SetAt( m_nManualClipIndex, m_pManualClip );
      }

      if (m_nAspectRatioIndex != -1)
      {
         m_pAspectRatio = pCamera->GetAspectRatio();
         m_pPropertyArray->SetAt( m_nAspectRatioIndex, m_pAspectRatio );
      }
   }
}

HPropertyArray *SnipeNavigator::GetPropertyArray() const
{
   return m_pPropertyArray;
}

KeyModifiers SnipeNavigator::GetKeyModifiers( HCGLBaseView *pView, MouseStates mousestate ) const 
{
   KeyModifiers keymodifiers;

   if (pView->IsKindOf( SVT_CGL2DView )) 
   {
      switch (mousestate.GetValue())
      {
         case MOUSESTATE_MIDDLE:
            keymodifiers |= (KEYMOD_NONE|KEYMOD_ALT|KEYMOD_SHIFTALT);
            break;

         case MOUSESTATE_LEFTMIDDLE:
            keymodifiers |= (KEYMOD_NONE|KEYMOD_ALT);
            break;

         case MOUSESTATE_RIGHT:
            keymodifiers |= KEYMOD_ALT;
            break;

         case MOUSESTATE_WHEEL:
            keymodifiers |= (KEYMOD_NONE|KEYMOD_ALT);
            break;
      }
   }

   if (pView->IsKindOf( SVT_CGLTimeLineView )) 
   {
      switch (mousestate.GetValue())
      {
         case MOUSESTATE_LEFTMIDDLE:
            keymodifiers |= (KEYMOD_SHIFTALT);
            break;
         case MOUSESTATE_RIGHT:
            keymodifiers |= (KEYMOD_SHIFTALT);
            break;
      }
   }

   if (pView->IsKindOf( SVT_CGL3DView )) 
   {
      switch (mousestate.GetValue())
      {
         case MOUSESTATE_MIDDLE:
            keymodifiers |= KEYMOD_SHIFT;
            break;
         case MOUSESTATE_LEFT:
            keymodifiers |= KEYMOD_ALT;
            break;
         case MOUSESTATE_MIDDLERIGHT:
            keymodifiers |= KEYMOD_NONE;
            break;
      }
   }

   return keymodifiers;
}

const char *SnipeNavigator::GetStatusbarInstructions( KeyModifiers keymodifier, MouseStates mousestate, HCGLBaseView *pView ) const 
{
   if (pView->IsKindOf( SVT_CGL2DView )) 
   {
      if ((mousestate == MOUSESTATE_MIDDLE     && keymodifier == KEYMOD_NONE) 
       || (mousestate == MOUSESTATE_MIDDLE     && keymodifier == KEYMOD_ALT))
         return "Truck";

      if (mousestate == MOUSESTATE_MIDDLE     && keymodifier == KEYMOD_SHIFTALT) 
         return "Truck Axis Locked";

      if ((mousestate == MOUSESTATE_LEFTMIDDLE && keymodifier == KEYMOD_NONE)
       || (mousestate == MOUSESTATE_RIGHT      && keymodifier == KEYMOD_ALT)
       || (mousestate == MOUSESTATE_WHEEL      && keymodifier == KEYMOD_NONE))
         return "Dolly";

      if (mousestate == MOUSESTATE_WHEEL       && keymodifier == KEYMOD_ALT)
         return "Field of view";
   }

   if (pView->IsKindOf( SVT_CGLTimeLineView )) 
   {
      if ((mousestate == MOUSESTATE_LEFTMIDDLE && keymodifier == KEYMOD_SHIFTALT)
         || (mousestate == MOUSESTATE_RIGHT      && keymodifier == KEYMOD_SHIFTALT))
         return "Dolly Axis Locked";
   }


   if (pView->IsKindOf( SVT_CGL3DView )) 
   {
      if ((mousestate ==  MOUSESTATE_MIDDLERIGHT && keymodifier == KEYMOD_NONE)
       || (mousestate ==  MOUSESTATE_MIDDLE      && keymodifier == KEYMOD_SHIFT)
       || (mousestate ==  MOUSESTATE_LEFT        && keymodifier == KEYMOD_ALT)) // Turn
         return "Orbit";
   }

   return NULL;
}

void SnipeNavigator::PostDraw( HCGLBaseView *pView )
{
   if (m_pDisplayTarget->GetValue()) {
      Vector vTarget = m_pTarget->GetValue();
      pView->WorldToScreen( vTarget );

      int nWidth = GetSnipeNavigatorPlugin()->m_pImageList->GetWidthEach();
      int nHeight = GetSnipeNavigatorPlugin()->m_pImageList->GetHeight();

      pView->Push2DAttributes();
         GetSnipeNavigatorPlugin()->m_pImageList->Draw( vTarget.x-(nWidth/2), vTarget.y-(nHeight/2), 0, .5f );
     pView->PopAttributes();
   }
}

BOOL SnipeNavigator::SetViewAngle( HCGL3DView *pView, ViewAngleIndex nIndex, BOOL bSnapToLastBirdsEye )
{
   m_ViewAngleDistance[pView->GetViewAngle()] = m_pFocusDistance->GetValue();

   if (!bSnapToLastBirdsEye && nIndex == VIEW_BIRDSEYE) // start birdseye from current viewangle
      m_ViewAngleDistance[VIEW_BIRDSEYE] = m_pFocusDistance->GetValue();
   
   m_bCalculateTransform = FALSE;
      m_pFocusDistance->StoreValue(m_ViewAngleDistance[nIndex]);
   m_bCalculateTransform = TRUE;   

   pView->DefaultSetViewAngle( nIndex, bSnapToLastBirdsEye );

   ResetCameraProperties();

   return TRUE;
}

BOOL SnipeNavigator::ZoomFit( HCGLBaseView *pView, HSnipeObjectList *pList )
{
   pView->DefaultZoomFit( pList );

   if (pView->IsKindOf(SVT_CGLTimeLineView))
   {
   }
   else if (pView->IsKindOf(SVT_CGL3DView))
   {
      m_bCalculateTransform = FALSE;
      BoundingBox bbox;
      pList->ComputeBoundingBox( bbox );
      if (!bbox.IsEmpty())
      {
         Vector vTarget = bbox.CalculateCenterPoint();
         m_pTarget->StoreValue( vTarget );
         HCamera *pCamera = ((HCGL2DView *)m_pView)->GetCurrentCamera();

         Vector vCamera(0.0f);
         vCamera *= pCamera->GetObjectToWorldMatrix();

         float fDistance = fabs((vTarget - vCamera).Length());
         m_pFocusDistance->StoreValue( fDistance );
      }
      m_bCalculateTransform = TRUE;
   }
   else if (pView->IsKindOf(SVT_CGL2DView))
   {
      m_bCalculateTransform = FALSE;
         HCamera *pCamera = ((HCGL2DView *)m_pView)->GetCurrentCamera();
         Vector vTarget;
         pCamera->GetWorldPos( vTarget );
         vTarget.z = 0.0f;
         m_pTarget->StoreValue( vTarget );

         Vector vCamera(0.0f);
         vCamera *= pCamera->GetObjectToWorldMatrix();

         float fDistance = fabs((vTarget - vCamera).Length());
         m_pFocusDistance->StoreValue( fDistance );
      m_bCalculateTransform = TRUE;
   }

   return TRUE;
}

BOOL SnipeNavigator::EvaluateButtons( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{ 
   if (pView->IsKindOf( SVT_CGL2DView )) 
   {
      if (mousestate == MOUSESTATE_MIDDLE && (keymodifier == KEYMOD_NONE || keymodifier == KEYMOD_ALT)) // Pan
      {
         m_nCurrentNavigation = NM_TRUCK;
         m_ptStart = m_ptPrev = point;
         return TRUE;
      }
      if (mousestate == MOUSESTATE_MIDDLE && keymodifier == KEYMOD_SHIFTALT) // Pan locked to axis
      {
         m_nCurrentNavigation = NM_TRUCK_AXIS_LOCKED;
         m_nNavigationAxisLock = NAL_UNDETERMINED;
         m_ptStart = m_ptPrev = point;
         return TRUE;
      }
      if ((mousestate == MOUSESTATE_LEFTMIDDLE && (keymodifier == KEYMOD_NONE || keymodifier == KEYMOD_ALT))
         || (mousestate == MOUSESTATE_RIGHT      && keymodifier == KEYMOD_ALT)) // Zoom
      {
         m_nCurrentNavigation = NM_DOLLY;
         m_ptStart = m_ptPrev = point;
         return TRUE;
      }
   }

   if (pView->IsKindOf( SVT_CGLTimeLineView )) 
   {
      if ((mousestate == MOUSESTATE_LEFTMIDDLE && keymodifier == KEYMOD_SHIFTALT )
         || (mousestate == MOUSESTATE_RIGHT      && keymodifier == KEYMOD_SHIFTALT)) // Zoom
      {
         m_nCurrentNavigation = NM_DOLLY_AXIS_LOCKED;
         m_nNavigationAxisLock = NAL_UNDETERMINED;
         m_ptStart = m_ptPrev = point;
         return TRUE;
      }
   }

   if (pView->IsKindOf( SVT_CGL3DView )) 
   {
      if ((mousestate == MOUSESTATE_MIDDLERIGHT && keymodifier == KEYMOD_NONE)
       || (mousestate == MOUSESTATE_MIDDLE      && keymodifier == KEYMOD_SHIFT)
       || (mousestate == MOUSESTATE_LEFT        && keymodifier == KEYMOD_ALT)) // Turn
      {
         m_nCurrentNavigation = NM_ORBIT;
         m_ptStart = m_ptPrev = point;
         return TRUE;
      }
   }

   if (m_nCurrentNavigation != NM_NONE) {
      m_nCurrentNavigation = NM_NONE;
      return TRUE;
   }

   return FALSE;
}

BOOL SnipeNavigator::OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{ 
   switch (m_nCurrentNavigation) 
   {
      case NM_NONE:
         return FALSE;

      case NM_TRUCK:
      {
         HCGL2DView *p2DView = (HCGL2DView *)m_pView;

         Vector vTarget = m_pTarget->GetValue();
         p2DView->WorldToScreen( vTarget );
         Vector vPrev( (float)m_ptPrev.x, (float)m_ptPrev.y, vTarget.z );
         p2DView->ScreenToWorld( vPrev );
         Vector vMouse( (float)point.x, (float)point.y, vTarget.z );
         p2DView->ScreenToWorld( vMouse );
         Vector vDelta = vPrev - vMouse;

         m_pTarget->StoreValue( m_pTarget->GetValue() + vDelta );

         m_ptPrev = point;
         pView->Invalidate();
         return TRUE;
      }

      case NM_TRUCK_AXIS_LOCKED:
      {
         HCGL2DView *p2DView = (HCGL2DView *)m_pView;

         Vector vTarget = m_pTarget->GetValue();
         p2DView->WorldToScreen( vTarget );

         CPoint ptPrev = m_ptPrev;
         CPoint ptMouse = point;

         switch (m_nNavigationAxisLock )
         {
            case NAL_UNDETERMINED:
               {
                  CPoint delta = point - m_ptStart;
                  delta.x = abs(delta.x);
                  delta.y = abs(delta.y);
                  if (delta.x > delta.y && delta.x > 3)
                  {
                     m_nNavigationAxisLock = NAL_HORIZONTAL;
                  }
                  else if (delta.y > delta.x && delta.y > 3)
                  {
                     m_nNavigationAxisLock = NAL_VERTICAL;
                  }
                  return FALSE;
               }
            case NAL_HORIZONTAL:
               ptPrev.y = ptMouse.y = m_ptStart.y;
               break;
            case NAL_VERTICAL:
               ptPrev.x = ptMouse.x = m_ptStart.x;
               break;
         }
         Vector vPrev( (float)ptPrev.x, (float)ptPrev.y, vTarget.z );
         p2DView->ScreenToWorld( vPrev );
         Vector vMouse( (float)ptMouse.x, (float)ptMouse.y, vTarget.z );
         p2DView->ScreenToWorld( vMouse );
         Vector vDelta = vPrev - vMouse;

         m_pTarget->StoreValue( m_pTarget->GetValue() + vDelta );

         m_ptPrev = point;
         pView->Invalidate();
         return TRUE;
      }

      case NM_DOLLY:
      {
         float factor = (point.x - m_ptPrev.x)*0.1f;
         DollyCamera(factor);
         m_ptPrev = point;
         pView->Invalidate();
         return TRUE;
      }

      case NM_DOLLY_AXIS_LOCKED:
      {
         HCGL2DView *p2DView = (HCGL2DView *)m_pView;

         CPoint ptPrev = m_ptPrev;
         CPoint ptMouse = point;

         float factorX,factorY;

         switch (m_nNavigationAxisLock )
         {
            case NAL_UNDETERMINED:
               {
                  CPoint delta = point - m_ptStart;
                  delta.x = abs(delta.x);
                  delta.y = abs(delta.y);
                  if (delta.x > delta.y && delta.x > 3)
                  {
                     m_nNavigationAxisLock = NAL_HORIZONTAL;
                  }
                  else if (delta.y > delta.x && delta.y > 3)
                  {
                     m_nNavigationAxisLock = NAL_VERTICAL;
                  }
                  return FALSE;
               }
            case NAL_HORIZONTAL:
               factorX = (float)(point.x-m_ptPrev.x)*0.1f;
               factorY = 0.0f;
               break;
            case NAL_VERTICAL:
               factorX = 0.0f;
               factorY = (float)(m_ptPrev.y-point.y)*0.1f;
               break;
         }

         ScaleCamera(factorX,factorY);
         m_ptPrev = point;
         pView->Invalidate();
         return TRUE;
      }

      case NM_ORBIT:
      {
         HCGL3DView *p3DView = (HCGL3DView *)pView;
         if (p3DView->GetViewAngle() != VIEW_BIRDSEYE)
            p3DView->SetViewAngle(VIEW_BIRDSEYE, FALSE);

         RotateCamera((float)(point.x - m_ptPrev.x), (float)(point.y - m_ptPrev.y));
         m_ptPrev = point;
         pView->Invalidate();
         return TRUE;
      }
   }

   return FALSE;
}

BOOL SnipeNavigator::OnMouseWheel( KeyModifiers keymodifier, MouseStates mousestate, short zDelta, CPoint point, HCGLBaseView *pView ) 
{ 
   float fDelta = (float)zDelta / (float)WHEEL_DELTA;

   HCamera *pCurrentCamera = ((HCGL2DView *)pView)->GetCurrentCamera();
   ASSERT(pCurrentCamera);

   if (keymodifier == KEYMOD_ALT)
   {
      pCurrentCamera->GetFOV()->StoreValue( pCurrentCamera->GetFOV()->GetValue() - fDelta );
      pView->Invalidate();
      return TRUE;
   }
   else if (keymodifier == KEYMOD_NONE)
   {
   	DollyCamera( fDelta * 4.0f );
      pView->Invalidate();
      return TRUE;
   }

   return FALSE;
}

BOOL SnipeNavigator::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg) {
      case NOTIFYOBJ_VALUECHANGED:
         {
            HProperty *pProperty = (HProperty *)lParam;
            if (pProperty==m_pTarget || pProperty==m_pAngle || pProperty==m_pRotation || pProperty==m_pFocusDistance || pProperty == m_pAspectRatio)
               CalculateTransform();
            else {
               if (m_pView->IsKindOf( SVT_CGL2DView ))
               {
                  HTransformProperty *pTransformProperty = ((HCGL2DView *)m_pView)->GetCurrentCamera()->GetTransform();
                  if (pProperty==pTransformProperty->GetTranslate() || pProperty==pTransformProperty->GetRotate())
                     CalculateProperties();
               }
            }
         }
         break;
      case NOTIFYOBJ_SELECTIONCHANGED:
      case NOTIFYOBJ_DELETE:
         CalculateTransform();
         break;
   }
   return NavigatorInstance::OnObjectNotify( msg, wParam, lParam );
}

void SnipeNavigator::CalculateProperties()
{
   if (!m_bCalculateProperties)
      return;

   m_bCalculateTransform = FALSE;

   HCamera *pCamera = ((HCGL2DView *)m_pView)->GetCurrentCamera();

   Vector vTarget(0,0,-m_pFocusDistance->GetValue());
   vTarget *= pCamera->GetObjectToWorldMatrix();
   m_pTarget->StoreValue( vTarget );

   HRotateProperty *pRotate = pCamera->GetTransform()->GetRotate();
   m_pRotation->StoreValue( pRotate->GetValue().y );
   m_pAngle->StoreValue( pRotate->GetValue().x );

   m_bCalculateTransform = TRUE;
}

void SnipeNavigator::CalculateTransform()
{
   if (!m_bCalculateTransform)
      return;

   m_bCalculateProperties = FALSE;

   Matrix44 temp, cameramatrix;
   cameramatrix.Identity();

   temp.RotationMatrix(m_pRotation->GetValue()*(float)DEGTORAD,0.0f,1.0f,0.0f);
   cameramatrix *= temp;
   temp.RotationMatrix(m_pAngle->GetValue()*(float)DEGTORAD,1.0f,0.0f,0.0f);
   cameramatrix *= temp;

   Vector translate = Vector( 0.0f, 0.0f, m_pFocusDistance->GetValue() );
   translate *= cameramatrix;
   translate += m_pTarget->GetValue();

   Vector rotate( m_pAngle->GetValue(), m_pRotation->GetValue(), 0.0f );

   HCamera *pCurrentCamera = ((HCGL2DView *)m_pView)->GetCurrentCamera();
   
   pCurrentCamera->GetTransform()->GetTranslate()->StoreValue( translate );
   pCurrentCamera->GetTransform()->GetRotate()->StoreValue( rotate );

   m_bCalculateProperties = TRUE;
}

void SnipeNavigator::RotateCamera( float dx, float dy )
{
   HSelectionList *pSelectionList = m_pView->GetDocument()->GetSelectionList();

   float fRotation = m_pRotation->GetValue() - dx;
   while (fRotation > 180.0)
      fRotation -= 360.0;
   while (fRotation < -180.0f)
      fRotation += 360.0;

   float fAngle = m_pAngle->GetValue() - dy;
   if (fAngle > 90.0f)
      fAngle = 90.0f;
   else if (fAngle < -90.0f)
      fAngle = -90.0f;

   m_bCalculateTransform = FALSE;

   Vector orbitcenter;
   HSnipeObject *pOrbitObject = HSelectionList::GetFocusObject();
   if (pOrbitObject && pOrbitObject->GetWorldPos( orbitcenter ) && pOrbitObject!=GetHMainInterface()->GetCurrentCommandObject())
   {
      dx = m_pRotation->GetValue() - fRotation;
      dy = m_pAngle->GetValue() - fAngle;

      Matrix44 rotatematrix;
      rotatematrix.RotationMatrix( -dx * (float)DEGTORAD, 0.0f, 1.0f, 0.0f );
      Vector xaxis( 1.0f, 0.0f, 0.0f );
      m_pView->ViewToWorld( xaxis, TRUE );
      Matrix44 temp;
      temp.RotationMatrix( -dy * (float)DEGTORAD, xaxis.x, xaxis.y, xaxis.z );
      rotatematrix *= temp;

      Vector pos;
      pos = m_pTarget->GetValue();
      pos -= orbitcenter;
      pos *= rotatematrix;
      pos += orbitcenter;
      m_pTarget->StoreValue( pos );
   }

   m_pRotation->StoreValue( fRotation );
   m_pAngle->StoreValue( fAngle );

   m_bCalculateTransform = TRUE;

   CalculateTransform();
}

void SnipeNavigator::DollyCamera(float delta)
{
   float distance = m_pFocusDistance->GetValue();
	float cosine = distance - (float)(cos(1.0*DEGTORAD)*distance);
	float sine = (float)sin(1.0*DEGTORAD)*distance;

	// length of line in object coordinates
	float length = sqrt(cosine*cosine+sine*sine);
   
	m_pFocusDistance->StoreValue( distance - (delta * length) );
}

void SnipeNavigator::ScaleCamera(float deltaH,float deltaV)
{
   float distanceV = m_pFocusDistance->GetValue();
   float aspectRatio = m_pAspectRatio->GetValue();
   float distanceH = distanceV * aspectRatio;

   float cosineV = distanceV - (float)(cos(2.0*DEGTORAD)*distanceV);
   float sineV = (float)sin(1.0*DEGTORAD)*distanceV;

   float cosineH = distanceH - (float)(cos(2.0*DEGTORAD)*distanceH);
   float sineH = (float)sin(1.0*DEGTORAD)*distanceH;

   // length of line in object coordinates
   float lengthV = sqrt(cosineV*cosineV+sineV*sineV);
   float lengthH = sqrt(cosineH*cosineH+sineH*sineH);
   float newDistanceV = distanceV - (deltaV * lengthV);
   float newDistanceH = distanceH - (deltaH * lengthH);

   m_pFocusDistance->StoreValue( newDistanceV );
   m_pAspectRatio->StoreValue( newDistanceH/newDistanceV );
}

