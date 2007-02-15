#pragma once
#include "HCommandPluginHelper.h"

class HComponent;
class HParentableObject;
class MemoryStream;
class HSnipeObjectList;
class HPatch;
class HPatchModel;

class PatchSubdivideInfo
{
public:
   HPatch *m_pPatch;
   UINT m_nSubdivisions;
   UINT m_nPrevSubdivisions;
   PatchSubdivideInfo()
   {
      m_pPatch = NULL;
      m_nSubdivisions = 0;
      m_nPrevSubdivisions = 0;
   }
};

class SubdivideCommand : public CommandInstance
{
protected:
   UINT m_nSubdivisions,m_nPrevSubdivision;
   enum nSubdivideTypes { FIXED, INCREMENT, DECREMENT } m_nSubdivideType;
   CArray<PatchSubdivideInfo*> m_patchsubdivideinfoarray;

public:
   SubdivideCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
      m_nSubdivisions = 0;
      m_nPrevSubdivision = 0;
      m_nSubdivideType = FIXED;
    }
   virtual ~SubdivideCommand();

//PluginCommand Overrides
   BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
   virtual BOOL OnValidate( String &strOutError );
};

class SubdivideCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( SubdivideCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
