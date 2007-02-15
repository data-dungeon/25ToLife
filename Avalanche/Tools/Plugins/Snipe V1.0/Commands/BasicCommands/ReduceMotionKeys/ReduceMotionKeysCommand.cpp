//

#include "stdafx.h"
#include "ReduceMotionKeysCommand.h"
#include "../resource.h"
#include "HSelectionList.h"
#include "HMainInterface.h"
#include "HMotionCurveDriver.h"
#include "HMotionCurve.h"
#include "Vector2.h"
#include "HMotionChannel.h"
#include "HModifiedMotionCurve.h"
#include "HMotion.h"

CommandInstance *ReduceMotionKeysCommandPlugin::CreateCommandInstance() { return SNEW ReduceMotionKeysCommand(this); }

const char *ReduceMotionKeysCommandPlugin::GetCLIName() const { return "ReduceMotionKeys"; }; // Name used in scripts and commandline
const char *ReduceMotionKeysCommandPlugin::GetTitleName() const { return "Reduce Motion Keys"; }

UINT ReduceMotionKeysCommandPlugin::GetBitmapID() const { return IDB_DEFAULT; }

String ReduceMotionKeysCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Motion Compression.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

ReduceMotionKeysCommand::~ReduceMotionKeysCommand()
{
}

BOOL ReduceMotionKeysCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   HMainInterface *pMainInterface = GetHMainInterface();

   if (stricmp( szToken, "-t" ) == 0)
   {
      String strNextToken = tokenlist.GetNext();
      m_fErrorTolerance = strNextToken.AsFloat();
      return TRUE;
   }
   else if (stricmp( szToken, "-m" ) == 0)
   {
      char *szNextValue = tokenlist.GetNext();
      if (szNextValue)
      {
         String error;
         HSnipeObject *pSnipeObject = pMainInterface->DescriptionToObject(szNextValue,&error);
         m_pHierObject = (HHierObject*)pSnipeObject;
         return TRUE;
      }
   }
   return FALSE;
}

UndoType ReduceMotionKeysCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL ReduceMotionKeysCommand::OnDo(StringList &strResults)
{
   if (m_pHierObject->GetSOT()==SOT_Motion)
   {
      HMotion *pMotion = (HMotion*)m_pHierObject;
      for (UINT nMotionChannelIndex = 0; nMotionChannelIndex < pMotion->GetNumMotionChannels(); nMotionChannelIndex++)
      {
         HMotionChannel *pMotionChannel = pMotion->GetMotionChannelAt(nMotionChannelIndex);
         for (UINT nMotionCurveDriverIndex =0; nMotionCurveDriverIndex < pMotionChannel->GetNumMotionCurveDrivers(); nMotionCurveDriverIndex++)
         {
            HMotionCurveDriver *pMotionCurveDriver = (HMotionCurveDriver*)pMotionChannel->GetMotionCurveDriverAt(nMotionCurveDriverIndex);
            HMotionCurveDriver::HRebuildMotionCurveDriverInfo *pRebuildMotionCurveDriverInfo = pMotionCurveDriver->RebuildCurves(m_fErrorTolerance,HMotionCurveDriver::HRebuildMotionCurveDriverInfo::HRebuildMotionCurveType::CUBIC);
            m_rebuildmotioncurvedriverinfos.Add(pRebuildMotionCurveDriverInfo);
         }
      }
   }
   else if (m_pHierObject->IsKindOf(SOT_MotionCurveDriver))
   {
      HMotionCurveDriver *pMotionCurveDriver = (HMotionCurveDriver*)m_pHierObject;
      HMotionCurveDriver::HRebuildMotionCurveDriverInfo *pRebuildMotionCurveDriverInfo = pMotionCurveDriver->RebuildCurves(m_fErrorTolerance,HMotionCurveDriver::HRebuildMotionCurveDriverInfo::HRebuildMotionCurveType::CUBIC);
      m_rebuildmotioncurvedriverinfos.Add(pRebuildMotionCurveDriverInfo);
   }

   return TRUE;
}

void ReduceMotionKeysCommand::OnRedo()
{
   for (int i = 0; i < m_rebuildmotioncurvedriverinfos.GetSize(); i++)
   {
      HMotionCurveDriver::HRebuildMotionCurveDriverInfo *pRebuildMotionCurveDriverInfo = m_rebuildmotioncurvedriverinfos.GetAt(i);
      pRebuildMotionCurveDriverInfo->Redo();
   }
}

void ReduceMotionKeysCommand::OnUndo()
{
   for (int i = 0; i < m_rebuildmotioncurvedriverinfos.GetSize(); i++)
   {
      HMotionCurveDriver::HRebuildMotionCurveDriverInfo *pRebuildMotionCurveDriverInfo = m_rebuildmotioncurvedriverinfos.GetAt(i);
      pRebuildMotionCurveDriverInfo->Undo();
   }
}
