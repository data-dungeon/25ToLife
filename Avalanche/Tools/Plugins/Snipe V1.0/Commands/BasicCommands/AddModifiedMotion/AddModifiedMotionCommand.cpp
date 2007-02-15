//

#include "stdafx.h"
#include "AddModifiedMotionCommand.h"
#include "../resource.h"
#include "HSelectionList.h"
#include "HMainInterface.h"
#include "HMotionCurveDriver.h"
#include "HMotionCurve.h"
#include "Vector2.h"
#include "HMotionChannel.h"
#include "HModifiedMotionCurve.h"
#include "HMotion.h"

CommandInstance *AddModifiedMotionCommandPlugin::CreateCommandInstance() { return SNEW AddModifiedMotionCommand(this); }

const char *AddModifiedMotionCommandPlugin::GetCLIName() const { return "AddModifiedMotion"; }; // Name used in scripts and commandline
const char *AddModifiedMotionCommandPlugin::GetTitleName() const { return "Add Modified Motion Curves"; }

UINT AddModifiedMotionCommandPlugin::GetBitmapID() const { return IDB_DEFAULT; }

String AddModifiedMotionCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Add motioncurve modifiers.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

AddModifiedMotionCommand::~AddModifiedMotionCommand()
{
}

BOOL AddModifiedMotionCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   HMainInterface *pMainInterface = GetHMainInterface();

   if (stricmp( szToken, "-m" ) == 0)
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

UndoType AddModifiedMotionCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL AddModifiedMotionCommand::OnDo(StringList &strResults)
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
            pMotionCurveDriver->AddModifiedMotionCurves();
         }
      }
   }
   else if (m_pHierObject->GetSOT()==SOT_MotionCurveDriver)
   {
      HMotionCurveDriver *pMotionCurveDriver = (HMotionCurveDriver*)m_pHierObject;
      pMotionCurveDriver->AddModifiedMotionCurves();
   }

   return TRUE;
}

void AddModifiedMotionCommand::OnRedo()
{
}

void AddModifiedMotionCommand::OnUndo()
{
}
