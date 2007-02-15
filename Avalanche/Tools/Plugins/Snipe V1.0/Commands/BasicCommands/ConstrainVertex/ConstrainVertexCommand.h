#pragma once
#include "HCommandPluginHelper.h"
#include "Vector.h"

class HComponent;
class HParentableObject;
class MemoryStream;
class HSnipeObjectList;

class HEdge;
class HPatchVertex;
class HPatchEdge;

class ConstrainVertexCommand : public CommandInstance
{
protected:
   enum nConstrainVertexOption { TOGGLE, CONSTRAIN, UNCONSTRAIN } m_nConstrainVertexOption;
   BOOL m_bWasConstrained;
   HPatchEdge *m_pOldEdge;

   HPatchVertex *m_pVertex;
   HPatchEdge *m_pEdge;
   Vector m_vPos;

public:
   ConstrainVertexCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
      m_nConstrainVertexOption = TOGGLE;
      m_bWasConstrained = FALSE;
      m_pOldEdge = NULL;
      m_pVertex = NULL;
      m_pEdge = NULL;
   }
   virtual ~ConstrainVertexCommand();

//PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnUndo();
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
};

class ConstrainVertexCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( ConstrainVertexCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
