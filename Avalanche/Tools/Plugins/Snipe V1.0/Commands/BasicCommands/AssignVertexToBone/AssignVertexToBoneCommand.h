#pragma once
#include "HCommandPluginHelper.h"

#include "HTransformableObject.h"
#include "HSnipeObjectList.h"

class HVertex;
class HBone;

class BoneVertexInfo
{
public:
   HVertex *m_pVertex;
   HBone *m_pOldBone;
   BoneVertexInfo()
   {
      m_pVertex = NULL; m_pOldBone = NULL;
   }
};

class AssignVertexToBoneCommand : public CommandInstance
{
private:
   HBone *m_pBone;
   CArray<BoneVertexInfo*> m_bonevertexinfoarray;

public:

   AssignVertexToBoneCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
   }
   virtual ~AssignVertexToBoneCommand();

//PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
};

class AssignVertexToBoneCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( AssignVertexToBoneCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
