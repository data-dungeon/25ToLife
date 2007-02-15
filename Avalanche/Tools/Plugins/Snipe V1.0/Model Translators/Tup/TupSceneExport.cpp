#include "stdafx.h"
#include "Tup.h"

#include "ExportTupSceneOptionsDlg.h"

void TupSceneTranslator::OnExport( HScene *pScene, const char *szFileName )
{
   ExportTupSceneOptionsDlg dlg;
   if (dlg.DoModal() != IDOK)
      return;

   BuildScene(pScene,szFileName);

   // save the tree
   TupperwareLoadSaveManager::FormatType formatType = TupperwareLoadSaveManager::ASCII;
   if (dlg.m_bBinary)
      formatType = TupperwareLoadSaveManager::BINARY;

   int result = TupperwareLoadSaveManager::Write(formatType,szFileName,m_pTupModelTranslator->m_pSceneAgg);

   m_pTupModelTranslator->DestroyTupScene();

   delete m_pTupModelTranslator;

   if (!result)
      THROW_FATAL_ERROR("Failed to write Tup file.");   
}

void TupSceneTranslator::BuildScene( HScene *pScene, const char *szFileName )
{
   m_pTupModelTranslator = SNEW TupModelTranslator;

   m_pTupModelTranslator->InitTupSceneForExport(szFileName);

   m_pTupModelTranslator->RecurseBranch((HHierObject*)pScene,m_pTupModelTranslator->m_pWorldNodeAgg);
}