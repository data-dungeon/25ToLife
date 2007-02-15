#pragma once

#include "HSnipeObject.h"

class HHierObject;
class HSnipeObjectList;
class HSelectionList;
class HCommandPlugin;
class HProject;

class SNIPEEXPORT HMainInterface : public HSnipeObject
{
public:
   HSnipeObject *DescriptionToObject(const String &strObject, String *pError=NULL); // Use | for hier, . for properties and components, ( ) for component ID's and : for ranges
   HSnipeObjectList *DescriptionToObjects(const String &strObjects, StringList &strOutErrorList); // Use | for hier, . for properties and components, ( ) for component ID's and : for ranges
   BOOL ObjectToDescription( const HSnipeObject *pMatchObject, String &strOutDescription, BOOL bRelativeToCurrentWorkingObject=TRUE );
   BOOL ObjectsToDescription( const HSnipeObjectList *pObjectList, String &strOutDescription, BOOL bRelativeToCurrentWorkingObject=TRUE );

   HHierObject *GetCurrentCommandObject() const;
   HSelectionList *GetCurrentSelectionList();
   SnipeObjectType GetCurrentSelectionSOT();
   HProject *GetCurrentProject() const;

   HSnipeObject *CreateObject( SnipeObjectType nSOT, HSnipeObject::HDeleteInfo **pOutDeleteInfo=NULL);

   BOOL DoCommand(const String &strCommandName, const String &strParameters, StringList &strResults, BOOL bTempCommand=FALSE);
   BOOL DoCommand(HCommandPlugin *pCommand, const String &strParameters, StringList &strResults, BOOL bTempCommand=FALSE);
   BOOL UndoCommand();
   BOOL KillLastCommand(); // Just like UndoCommand, except there can not be a redo
   BOOL RedoCommand();
   BOOL UndoTempCommand();
   BOOL CommitTempCommand();
};

SNIPEEXPORT HMainInterface *GetHMainInterface();  