////////////////////////////////////////////////////////////////////////////
//
// TupAnimExport
//
// Tupperware animation exporter
//
////////////////////////////////////////////////////////////////////////////

#include "TupImpExpPch.h"
#include "TupAnimExport.h"
#include "TupExport.h"

TupAnimExport::TupAnimExport(bool bBinaryOutput,bool bExportSelected)
{
   m_bBinaryOutput = bBinaryOutput;
   m_bExportSelected = bExportSelected;
   m_pExportDagPaths = NULL;
}

TupAnimExport::TupAnimExport(bool bExportSelected,Keeper<MDagPath> *pExportDagPaths)
{
   m_bExportSelected = bExportSelected;
   m_pExportDagPaths = pExportDagPaths;
}

MStatus TupAnimExport::ExportAnimation(const TCHAR *outputFilename)
{
   MStatus status;

   MSelectionList activeselection;
   // get active selection
   status = MGlobal::getActiveSelectionList(activeselection);
   if (activeselection.length()==0)
   {
      MGlobal::displayError("Nothing selected for output.");
      return MS::kFailure;
   }

   // create a new scene
   m_pSceneAgg = new TupperwareAggregate;
   m_pTupScene = new TupScene(m_pSceneAgg);

   // create the animation set pool
   TupperwareAggregate *pAnimSetPoolAgg = m_pTupScene->AddAnimSetPool();
   m_pAnimSetPool = new TupAnimSetPool(pAnimSetPoolAgg);

   // add in the date, time and version
   m_pTupScene->AddExporterDate(g_pExporterDate);
   m_pTupScene->AddExporterTime(g_pExporterTime);
   m_pTupScene->AddVersion(TupScene::GetSceneVersion());
   TupExport::AddUnits(m_pTupScene);

   // find root bone
   MDagPath dagPath;
   activeselection.getDagPath(0,dagPath);
   MObject object;
   activeselection.getDependNode(0,object);
   MDagPath rootJointDagPath;
   if (object.apiType() == MFn::kClip)
   {
      TupAnimExport::AddClipAnimationAsGlobal(object,m_pAnimSetPool);

      // save the tree
      TupperwareLoadSaveManager::FormatType formatType = m_bBinaryOutput ? TupperwareLoadSaveManager::BINARY : TupperwareLoadSaveManager::ASCII;
      int nResult = TupperwareLoadSaveManager::Write(formatType,outputFilename,m_pSceneAgg);

      // delete tree
      delete m_pSceneAgg;
      delete m_pTupScene;

      if (nResult)
         return MS::kSuccess;
      return MS::kFailure;
   }

   MTools::GetRootBone(dagPath,rootJointDagPath);

   TupAnimExport::AddAnimationGlobals(m_pTupScene);
   TupAnimExport::AddAnimation(rootJointDagPath,m_pAnimSetPool,NULL);

   // save the tree
   TupperwareLoadSaveManager::FormatType formatType = m_bBinaryOutput ? TupperwareLoadSaveManager::BINARY : TupperwareLoadSaveManager::ASCII;
   int nResult = TupperwareLoadSaveManager::Write(formatType,outputFilename,m_pSceneAgg);

   // delete tree
   delete m_pSceneAgg;
   delete m_pTupScene;

   if (nResult)
      return MS::kSuccess;
   return MS::kFailure;
}

bool TupAnimExport::ValidNode(MDagPath &mDagPath)
{
   // make sure it is not an intermediate object
   MFnDagNode mDagNode(mDagPath);
   if (mDagNode.isIntermediateObject())
      return false;

   if ((m_bExportSelected)&&(m_pExportDagPaths))
   {
      // if not in the list of nodes to output
      if (m_pExportDagPaths->GetIndex(mDagPath)==-1)
         return false;
   }

   MFn::Type type = mDagPath.apiType();
   if (type == MFn::kTransform)
   {
      return true;
   }
   else if (type == MFn::kLookAt)
   {
      return true;
   }
   else if (type == MFn::kJoint)
   {
      int numChildren = mDagPath.childCount();
      // find an child joint (if there is one we are a valid joint)
      for (int childIndex = 0; childIndex < numChildren; childIndex++)
      {
         MObject childObject = mDagPath.child(childIndex);
         MDagPath childPath = mDagPath;
         childPath.push(childObject);
         // found a child joint
         if (childPath.apiType()==MFn::kJoint)
         {
            return true;
         }
      }
   }
   return false;
}