#pragma once
#include "HCommandPluginHelper.h"

class HComponent;
class HParentableObject;
class MemoryStream;
class HSnipeObjectList;
class HEdge;
class HPoly;
class HVertex;

class SelectBaseCommand : public CommandInstance
{
protected:
   HSnipeObjectList *m_prevselectedobjects;
   HSnipeObject *m_pFocusObject, *m_pPrevFocusObject;
   HSnipeObject *m_pNearestFocusObject, *m_pPrevNearestFocusObject;

public:
   SelectBaseCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
      m_prevselectedobjects = NULL;
      m_pFocusObject = NULL;
      m_pPrevFocusObject = NULL;
      m_pNearestFocusObject = NULL;
      m_pPrevNearestFocusObject = NULL;
   }
   virtual ~SelectBaseCommand();

   // PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnUndo();
};
