#pragma once
#include "HCommandPluginHelper.h"

#include "HTransformableObject.h"
#include "HSnipeObjectList.h"
#include "..\Utility\Vector.h"
#include "HBone.h"

class HDeleteInfo;

class AddBoneCommand : public CommandInstance
{
private:
   HBone *m_pParentBone;
   HBone *m_pBone;
   HPolyModel *m_pPolyModel;
   BOOL m_bIsChainedToParent;
   Vector m_vStart;
   Vector m_vRotate;
   float m_fBoneLength;
   HBone::HDeleteInfo *m_pDeleteInfo;

public:

   AddBoneCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
      m_pBone = NULL;
      m_pPolyModel = NULL;
      m_pParentBone = NULL;
      m_bIsChainedToParent = FALSE;
      m_fBoneLength = 0.0f;
      m_pDeleteInfo = NULL;
   }
   virtual ~AddBoneCommand();

//PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
};

class AddBoneCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( AddBoneCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
