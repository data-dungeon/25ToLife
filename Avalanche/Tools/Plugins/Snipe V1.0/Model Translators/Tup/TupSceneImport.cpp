#include "stdafx.h"
#include "Tup.h"

#include "HPolyModel.h"
#include "HScene.h"
#include "Matrix44.h"
#include "HTransformProperty.h"
#include "HTranslateProperty.h"
#include "HRotateProperty.h"

void TupSceneTranslator::OnImport( HScene *pScene, const char *szFileName)
{
   m_pTupModelTranslator = SNEW TupModelTranslator;
   // read tup file
   int result = TupperwareLoadSaveManager::Read(szFileName,&m_pTupModelTranslator->m_pSceneAgg);
   if (!result)
      THROW_FATAL_ERROR("TupperwareLoadSaveManager::Read failed");

   // exit if scene empty
   if (!m_pTupModelTranslator->m_pSceneAgg)
      THROW_FATAL_ERROR("SceneAgg empty");

   m_objecthash.Clear();

   m_pScene = pScene;

   m_pTupModelTranslator->InitTupSceneForImport(szFileName);
   RecurseTupTree(m_pTupModelTranslator->m_pWorldNodeAgg,(HHierObject*)pScene);
   m_pTupModelTranslator->DestroyTupScene();
   delete m_pTupModelTranslator;
}

TupObject::ObjectTypes TupSceneTranslator::GetTupObjectType(TupperwareAggregate *pNodeAgg)
{
   TupNode tupNode(pNodeAgg);

   int objectRef;
   tupNode.GetObjectReference(objectRef);
   if (objectRef==-1)
      return TupObject::UNKNOWN;

   // find base object reference
   int baseObjRef = TupObjectHelper::FindBaseObjectRef(*m_pTupModelTranslator->m_pTupObjectPool,objectRef);
   TupperwareAggregate *pObjectAgg = m_pTupModelTranslator->m_pTupObjectPool->GetObjectAggregate(baseObjRef);

   TupObject tupObject(pObjectAgg);

   return tupObject.GetObjectType();
}

void TupSceneTranslator::RecurseTupTree(TupperwareAggregate *pNodeAgg,HHierObject *pParent)
{
   if (!pNodeAgg)
      return;
   if (!pParent)
      return;

   TupNode tupNode(pNodeAgg);

   char *name;
   tupNode.GetName(&name);

   HHierObject *pHierObject = pParent;

   HProject *pProject = GetHMainInterface()->GetCurrentProject();

   int nObjectRef,nValue;
   tupNode.GetObjectReference(nObjectRef);
   if (nObjectRef>=0)
   {
      TupObject::ObjectTypes objType = GetTupObjectType(pNodeAgg);

      switch(objType)
      {
      case TupObject::POLY_MESH :
      case TupObject::CAMERA :
      case TupObject::LIGHT :
         {
            if (!m_objecthash.FindKey(nObjectRef,nValue))
            {
               pHierObject = GetHMainInterface()->GetCurrentProject()->AddPolyModel(name);
               m_objecthash.AddKey(nObjectRef,(int)pHierObject);
               if (objType == TupObject::POLY_MESH)
                  m_pTupModelTranslator->ImportPolyMesh(pNodeAgg,(HPolyModel*)pHierObject);
               String message = "Loading: ";
               message += name;
               message += "\n";
               //DISPLAY_MESSAGE(message);
               TRACE0(message);
             }
            else
            {
               pHierObject = (HHierObject*)nValue;
            }
            HHierObject *pInstance = (HHierObject*)m_pScene->AddInstance(pHierObject);
            ImportTransform( pNodeAgg, (HTransformableObject*)pInstance );
          }
         break;
      }
   }

   int nNumChildren = tupNode.GetNumChildNodes();
   int nChildNodeReference;
   for (int nChildNodeIndex=0; nChildNodeIndex < tupNode.GetNumChildNodes(); nChildNodeIndex++)
   {
      tupNode.GetChildNodeReference(nChildNodeIndex,nChildNodeReference);
      TupperwareAggregate *pChildNodeAgg = m_pTupModelTranslator->m_pTupNodePool->GetNode(nChildNodeReference);
      if (!pChildNodeAgg)
         continue;
      RecurseTupTree(pChildNodeAgg,pHierObject);
   }
}

void TupSceneTranslator::ImportTransform(TupperwareAggregate *pNodeAgg,HTransformableObject *pTransformableObject)
{
   TupNode tupNode(pNodeAgg);
   TupperwareAggregate *pTransformAgg = tupNode.GetTransform();
   TupTransform tupTransform(pTransformAgg);
   if (tupTransform.GetTransformType() != TupTransform::STATIC_MATRIX)
      return;

   TupStaticMatrix tupStaticMatrix(pTransformAgg);
   float *pMatrix;
   tupStaticMatrix.GetMatrix(&pMatrix);

   Matrix44 matrix;
   ConvertTupMatrixToMatrix44(pMatrix,matrix);

   Vector translation,scale,shear,rotation;
   matrix.Decompose(translation,scale,shear,rotation);

   pTransformableObject->GetTransform()->GetTranslate()->StoreValue(translation,FALSE);
   pTransformableObject->GetTransform()->GetRotate()->StoreValue(rotation,FALSE);

   return;
}

void TupSceneTranslator::ConvertTupMatrixToMatrix44(float *pMatrix,Matrix44 &matrix)
{
   for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
         matrix.Set(i,j,pMatrix[(i * 4)+j]);

   matrix.Set(3,0,matrix.Get(3,0)*100.0f);// convert meters to centimeters
   matrix.Set(3,1,matrix.Get(3,1)*100.0f);
   matrix.Set(3,2,matrix.Get(3,2)*100.0f);

   matrix = matrix.Transpose();// convert to row major
}
