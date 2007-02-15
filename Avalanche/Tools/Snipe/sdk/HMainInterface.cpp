//

#include "stdafx.h"
#include "HMainInterface.h"
#include "Interface/MainInterface.h"

HMainInterface *GetHMainInterface()
{
   return (HMainInterface *) GetMainInterface();
}

HSnipeObject *HMainInterface::DescriptionToObject(const String &strObject, String *pError/*=NULL*/)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HSnipeObject *) ((MainInterface *)this)->DescriptionToObject( strObject, pError );
}

HSnipeObjectList *HMainInterface::DescriptionToObjects(const String &strObjects, StringList &strOutErrorList)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HSnipeObjectList *) ((MainInterface *)this)->DescriptionToObjects( strObjects, strOutErrorList );
}

BOOL HMainInterface::ObjectToDescription( const HSnipeObject *pMatchObject, String &strOutDescription, BOOL bRelativeToCurrentWorkingObject/*=TRUE*/ )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((MainInterface *)this)->ObjectToDescription((const SnipeObject *)pMatchObject, strOutDescription, bRelativeToCurrentWorkingObject);
}

BOOL HMainInterface::ObjectsToDescription( const HSnipeObjectList *pObjectList, String &strOutDescription, BOOL bRelativeToCurrentWorkingObject/*=TRUE*/ )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((MainInterface *)this)->ObjectsToDescription( (const SnipeObjectList *)pObjectList, strOutDescription, bRelativeToCurrentWorkingObject);
}

HSnipeObject *HMainInterface::CreateObject( SnipeObjectType nSOT, HSnipeObject::HDeleteInfo **pDeleteInfo/*=NULL*/)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HSnipeObject *) ((MainInterface *)this)->CreateObject( nSOT, (SnipeObject::DeleteInfo **) pDeleteInfo);
}

HHierObject *HMainInterface::GetCurrentCommandObject() const
{
   return (HHierObject *) ((MainInterface *)this)->GetCurrentCommandObject();
}

HSelectionList *HMainInterface::GetCurrentSelectionList()
{
   return (HSelectionList *) ((MainInterface *)this)->GetCurrentSelectionList();
}

SnipeObjectType HMainInterface::GetCurrentSelectionSOT()
{
   return ((MainInterface *)this)->GetCurrentSelectionSOT();
}

HProject *HMainInterface::GetCurrentProject() const
{
   return (HProject *) ((MainInterface *)this)->GetProject();
}

BOOL HMainInterface::DoCommand(const String &strCommandName, const String &strParameters, StringList &strResults, BOOL bTempCommand/*=FALSE*/)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((MainInterface *)this)->DoCommand(strCommandName, strParameters, strResults, bTempCommand);
}

BOOL HMainInterface::DoCommand(HCommandPlugin *pCommand, const String &strParameters, StringList &strResults, BOOL bTempCommand/*=FALSE*/)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((MainInterface *)this)->DoCommand((CommandPlugin *)pCommand, strParameters, strResults, bTempCommand);
}

BOOL HMainInterface::UndoCommand()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((MainInterface *)this)->UndoCommand();
}

BOOL HMainInterface::KillLastCommand()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((MainInterface *)this)->KillLastCommand();
}

BOOL HMainInterface::RedoCommand()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((MainInterface *)this)->RedoCommand();
}

BOOL HMainInterface::UndoTempCommand()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((MainInterface *)this)->UndoTempCommand();
}

BOOL HMainInterface::CommitTempCommand()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((MainInterface *)this)->CommitTempCommand();
}

void SNIPEEXPORT DisplayWarning(String msg)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   g_outputWindow << "<W>Warning: " << msg << "\n";
}

void SNIPEEXPORT DisplayMessage(String msg)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   g_outputWindow << msg << "\n";
}
