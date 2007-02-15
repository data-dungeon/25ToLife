#pragma once
#include "HCommandPluginHelper.h"

class HComponent;
class HParentableObject;
class MemoryStream;
class HSnipeObjectList;

class DeleteCommand : public CommandInstance
{
protected:
   //  ------*-------
   // We keep strings instead of pointers for one primary reason: Edges.  When you delete and edge that is attached solely to another edge by a vertex it invokes the vertex delete of the shared vertex.
   // This will remove the two edges and replace it with a new edge but the edge will have the same ID as the edge that wasn't deleted, this if I keep the descrition here rather than the Edge pointer
   // it will find the new edge as if it was the old edge.  This is the case where both edges are trying to be deleted and they will both go away.

   CList<String> m_deletelist; 
   CList<HSnipeObject::HDeleteInfo *> m_DetachedInfoList;

public:

   DeleteCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
   }
   virtual ~DeleteCommand();
  
   BOOL DeleteSnipeObject( HSnipeObject *pSnipeObject, String &strError );
   void RemoveDeletedObjectsFromList( CList<HSnipeObject *> &deletelist );

//PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL ParseToken(const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
};

class DeleteCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( DeleteCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
