//

#include "stdafx.h"
#include "AddBoneManipulator.h"
#include "../resource.h"
#include "HCGLModelView.h"
#include "HHitArray.h"
#include "HCommandPluginManager.h"
#include "MouseOn.h"
#include "CommandUIAccel.h"
#include "MemoryStream.h"
#include "HSelectionList.h"
#include "HMainInterface.h"
#include "HCBaseDoc.h"
#include "HCModelDoc.h"
#include "HPolyModel.h"
#include "HBone.h"
#include "HTranslateProperty.h"
#include "HRotateProperty.h"
#include "HFloatProperty.h"

// Plugin //////////////////////////////////////////
AddBoneManipulatorPlugin::AddBoneManipulatorPlugin()
{
   m_pAddBoneCommandPlugin = NULL;
}

ManipulatorInstance *AddBoneManipulatorPlugin::CreateInstance()
{
   return SNEW AddBoneManipulator( this );
}

const char *AddBoneManipulatorPlugin::GetTitleName() const { return "AddBone"; }

UINT AddBoneManipulatorPlugin::GetBitmapID() const
{
   return IDB_ADDBONE;
}

void AddBoneManipulatorPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pAddBoneCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "AddBone");
   if (m_pAddBoneCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : AddBone ).");
}


BOOL AddBoneManipulatorPlugin::SupportSOT( SnipeObjectType nType)
{
   return (nType == SOT_Bone);
}


// Instance ////////////////////////////////////////
AddBoneManipulator::AddBoneManipulator( AddBoneManipulatorPlugin *pPluginHelper) :
 ManipulatorInstance( pPluginHelper )
{
   m_bDragging = FALSE;
   m_pPolyModel = NULL;
   m_pParentBone = NULL;
}

AddBoneManipulator::~AddBoneManipulator()
{
}

BOOL AddBoneManipulator::OnApply( HCGLBaseView *pView )
{
   return TRUE;
}

BOOL AddBoneManipulator::OnCancel( HCGLBaseView *pView )
{
   return TRUE;
}

void AddBoneManipulator::ComputePosition( HCGLBaseView *pView, CPoint &point, const KeyModifiers &keymodifiers )
{
   pView->WorldToScreen( m_vPos );
   m_vPos.x = (float)point.x;
   m_vPos.y = (float)point.y;
   pView->ScreenToWorld( m_vPos );
}

void AddBoneManipulator::ExecuteAddBone(HPolyModel *pPolyModel,const Vector &vStart,const Vector &vRotate,float fLength,HBone *pParentBone,BOOL bTempCommand)
{
   HMainInterface *pMI = GetHMainInterface();

   if (bTempCommand)
      GetHMainInterface()->UndoTempCommand();

   String strModelDescription,strParentDescription;
   pMI->ObjectToDescription( pPolyModel, strModelDescription, TRUE );
   ASSERT(pPolyModel);
   pMI->ObjectToDescription( pParentBone, strParentDescription, TRUE );

   char buffer[256];
   sprintf(buffer,"-m %s -s %.2f %.2f %.2f -r %.2f %.2f %.2f -l %.2f -p %s",
      strModelDescription.Get(),
      vStart.x,vStart.y,vStart.z,
      vRotate.x,vRotate.y,vRotate.z,
      fLength,
      strParentDescription.Get());

   String strParameters = buffer;
   if (pParentBone)
      strParameters += " -c";
   StringList strResults;
   BOOL bResult = pMI->DoCommand( GetAddBoneManipulatorPlugin()->m_pAddBoneCommandPlugin, strParameters, strResults, bTempCommand );
   ASSERT(bResult);
}

BOOL AddBoneManipulator::OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView )
{
   HSnipeObject *pSnipeObject = pView->GetDocument()->GetObjectHead();
   if (!pSnipeObject->IsKindOf(SOT_PolyModel))
      return FALSE;
   m_pPolyModel = (HPolyModel*)pSnipeObject;

   m_vPos = Vector(0.0f,0.0f,0.0f);
   ComputePosition(pView,point,keymodifier);
   m_vPivot = m_vPos;

   CRect rcPick;
   rcPick.SetRect( point, point );

   HPickHitArray *pPickHitArray = HPickHitArray::New();
   pView->DoPick(rcPick,pPickHitArray,NULL,SOT_Bone);

   if (pPickHitArray->GetNumHits()>0)
   {
      HSnipeObject *pSniptObject = (HSnipeObject *)pPickHitArray->GetHit(0)->GetName(0);
      if (pSnipeObject->GetSOT() == SOT_Bone)
      {
         m_pParentBone = (HBone*)pSnipeObject;
         Vector vPivot = m_pParentBone->GetStart()->GetValue();
         Vector vRotate = m_pParentBone->GetRotate()->GetValue();
         float fLength = m_pParentBone->GetLength()->GetValue();

         Matrix44 matrix = Translatef(vPivot.x,vPivot.y,vPivot.z);
         matrix *= Rotatef(vRotate.y,0,1,0);
         matrix *= Rotatef(vRotate.x,1,0,0);
         matrix *= Rotatef(vRotate.z,0,0,1);
         matrix *= Scalef(fLength,fLength,fLength);
         
         Vector v(0,0,1);
         m_vPivot = matrix * v;
      }
   }

   delete pPickHitArray;

   m_bDragging = TRUE;

   return TRUE;
}

BOOL AddBoneManipulator::OnMouseMove(  KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView  ) 
{
   if (!m_bDragging)
      return FALSE;

   if (!m_pPolyModel)
      return FALSE;

   ComputePosition(pView,point,keymodifier);
   Vector vBoneVect = m_vPos - m_vPivot;
   RotateEuler euler(RotateVector(vBoneVect,0.0f));
   m_vRotate.x = euler.x;
   m_vRotate.y = euler.y;
   m_vRotate.z = euler.z;
   m_fLength = vBoneVect.Length();
   ExecuteAddBone(m_pPolyModel,m_vPivot,m_vRotate,m_fLength,m_pParentBone,TRUE);

   return TRUE;
}

BOOL AddBoneManipulator::OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView )
{
   m_bDragging = FALSE;
   if (!m_pPolyModel)
      return FALSE;
   ComputePosition(pView,point,keymodifier);
   Vector vBoneVect = m_vPos - m_vPivot;
   RotateEuler euler(RotateVector(vBoneVect,0.0f));
   m_vRotate.x = euler.x;
   m_vRotate.y = euler.y;
   m_vRotate.z = euler.z;
   m_fLength = vBoneVect.Length();
   ExecuteAddBone(m_pPolyModel,m_vPivot,m_vRotate,m_fLength,m_pParentBone,FALSE);

   return TRUE;
}

KeyModifiers AddBoneManipulator::GetKeyModifiers( HCGLBaseView *pView, MouseStates mousestate ) const
{
   return KeyModifiers( KEYMOD_NONE );
}

void AddBoneManipulator::PostDraw(HCGLBaseView *pView)
{
}
