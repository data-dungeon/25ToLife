#include "stdafx.h"
#include "Tup.h"
#include "HMotionSet.h"
#include "HMotion.h"
#include "HFileInfoProperty.h"
#include "HFloatProperty.h"

#include "ExportMSetOptionsDlg.h"


BOOL IsAnythingSet(HMotionChannelBiasObject *pMotionChannelBiasObject)
{
   if (pMotionChannelBiasObject->GetTranslationBiasProperty()->IsSet()||
      pMotionChannelBiasObject->GetRotationBiasProperty()->IsSet()||
      pMotionChannelBiasObject->GetScaleBiasProperty()->IsSet())
   {
      return TRUE;
   }
   return FALSE;
}

BOOL IsMotionCurveDriverSet(HMotionChannelBiasObject *pMotionChannelBiasObject)
{
   for (UINT nMotionCurveDriverBiasObjectIndex = 0; nMotionCurveDriverBiasObjectIndex < pMotionChannelBiasObject->GetNumMotionCurveDriverBiasObjects(); nMotionCurveDriverBiasObjectIndex++)
   {
      HMotionCurveDriver *pMotionCurveDriverBiasObject = pMotionChannelBiasObject->GetMotionCurveDriverBiasObjectAt(nMotionCurveDriverBiasObjectIndex);
      if (pMotionCurveDriverBiasObject->GetReductionBiasProperty()->IsSet())
         return TRUE;
   }
    
   return FALSE;
}

void MSetTranslator::OnExport( HMotionSet *pMotionSet, const char *szFileName)
{
//   ExportMSetOptionsDlg dlg;
//   if (dlg.DoModal() != IDOK)
//      return;

   String strFileName = szFileName;
   strFileName = strFileName.GetNameOnlyWithoutSuffix();
   strFileName += ".mdb";

   // build MSet aggregates
   TupperwareAggregate *pAgg = SNEW TupperwareAggregate;
   TupperwareScalar *pDataScalar;
   
   pDataScalar = pAgg->AddScalarString("OutputFileName",NULL,strFileName.GetNameOnly().Get());

   // global tolerances
   pAgg->AddScalarFloat("TranslationErrorTolerance",NULL,pMotionSet->GetTranslationToleranceProperty()->GetValue()); 
   pAgg->AddScalarFloat("RotationErrorTolerance",NULL,pMotionSet->GetRotationToleranceProperty()->GetValue()); 
   pAgg->AddScalarFloat("ScaleErrorTolerance",NULL,pMotionSet->GetScaleToleranceProperty()->GetValue()); 

   TupperwareAggregate *pMotionChannelsAgg = pAgg->AddAggregate("MotionChannelsDefaults",NULL);
   for (UINT nMotionChannelBiasObjectIndex = 0; nMotionChannelBiasObjectIndex < pMotionSet->GetNumMotionChannelBiasObjects(); nMotionChannelBiasObjectIndex++)
   {
      HMotionChannelBiasObject *pMotionChannelBiasObject = pMotionSet->GetMotionChannelBiasObjectAt(nMotionChannelBiasObjectIndex);
  
      if (!IsAnythingSet(pMotionChannelBiasObject))
         continue;

      TupperwareAggregate *pMotionChannelBiasObjectAgg = pMotionChannelsAgg->AddAggregate("MotionChannelBias",NULL);

      pMotionChannelBiasObjectAgg->AddScalarString("Name",NULL,pMotionChannelBiasObject->GetName().Get());

      if (pMotionChannelBiasObject->GetTranslationBiasProperty()->IsSet())
         pMotionChannelBiasObjectAgg->AddScalarFloat("TranslationErrorBias",NULL,pMotionChannelBiasObject->GetTranslationBiasProperty()->GetValue()); 

      if (pMotionChannelBiasObject->GetRotationBiasProperty()->IsSet())
         pMotionChannelBiasObjectAgg->AddScalarFloat("RotationErrorBias",NULL,pMotionChannelBiasObject->GetRotationBiasProperty()->GetValue()); 
 
      if (pMotionChannelBiasObject->GetScaleBiasProperty()->IsSet())
         pMotionChannelBiasObjectAgg->AddScalarFloat("ScaleErrorBias",NULL,pMotionChannelBiasObject->GetScaleBiasProperty()->GetValue());
   }

   TupperwareAggregate *pMotionsAgg = pAgg->AddAggregate("Motions",NULL);

   for (UINT nMotionIndex = 0; nMotionIndex < pMotionSet->GetNumMotionPlaceholders(); nMotionIndex++ )
   {
      HMotionPlaceholder *pMotionPlaceholder = pMotionSet->GetMotionPlaceholderAt(nMotionIndex);
      HMotion *pMotion = pMotionPlaceholder->GetMotion();

      TupperwareAggregate *pMotionAgg = pMotionsAgg->AddAggregate("Motion",String(nMotionIndex).Get());

      String strMotionFileName = pMotion->GetFileInfoProperty()->GetFileNameProperty()->GetValue();
      pMotionAgg->AddScalarString("MotionFileName",NULL,strMotionFileName.Get());
   
      for (UINT nMotionChannelIndex = 0; nMotionChannelIndex < pMotionPlaceholder->GetNumMotionChannelBiasObjects(); nMotionChannelIndex++)
      {
         HMotionChannelBiasObject *pMotionChannelBiasObject = pMotionPlaceholder->GetMotionChannelBiasObjectAt(nMotionChannelIndex);

         if (!IsMotionCurveDriverSet(pMotionChannelBiasObject))
            continue;

         TupperwareAggregate *pMotionChannelBiasObjectAgg = pMotionAgg->AddAggregate("MotionChannelBias",NULL);

         pMotionChannelBiasObjectAgg->AddScalarString("Name",NULL,pMotionChannelBiasObject->GetName().Get());

         for (UINT nMotionCurveDriverIndex = 0; nMotionCurveDriverIndex < pMotionChannelBiasObject->GetNumMotionCurveDriverBiasObjects(); nMotionCurveDriverIndex++)
         {
            HMotionCurveDriver *pMotionCurveDriver = pMotionChannelBiasObject->GetMotionCurveDriverBiasObjectAt(nMotionCurveDriverIndex);

            if (!pMotionCurveDriver->GetReductionBiasProperty()->IsSet())
               continue;

            TupperwareAggregate *pMotionCurveDriverObjectAgg = pMotionChannelBiasObjectAgg->AddAggregate("MotionCurveDriverBias");
 
            pMotionCurveDriverObjectAgg->AddScalarString("Name",NULL,pMotionCurveDriver->GetName().Get());

            pMotionCurveDriverObjectAgg->AddScalarFloat("CurveDriverBias",NULL,pMotionCurveDriver->GetReductionBiasProperty()->GetValue());
         }
      }
   }

   // save the tree
//   TupperwareLoadSaveManager::FormatType formatType = dlg.m_bBinary ? TupperwareLoadSaveManager::BINARY : TupperwareLoadSaveManager::ASCII;
   TupperwareLoadSaveManager::FormatType formatType = TupperwareLoadSaveManager::ASCII;
   int nResult = TupperwareLoadSaveManager::Write(formatType,szFileName,pAgg);

   // delete tree
   delete pAgg;
}

