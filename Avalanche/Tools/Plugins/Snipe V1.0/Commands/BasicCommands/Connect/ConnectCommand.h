#pragma once
#include "HCommandPluginHelper.h"
#include "HVertex.h"
#include "HEdge.h"

class SplitEdge
{
private:
   HVertex *m_pV0;
   HVertex *m_pV1;

public:
   SplitEdge( HVertex *pV0, HVertex *pV1 )
   {
      ASSERT(pV0 != pV1);

      m_pV0 = pV0;
      m_pV1 = pV1;
   }

   HVertex *GetV0() const { return m_pV0; }
   HVertex *GetV1() const { return m_pV1; }

   BOOL operator == ( const SplitEdge &other ) const
   {
      if ((m_pV0 == other.GetV0() || m_pV0 == other.GetV1())
         && (m_pV1 == other.GetV0() || m_pV1 == other.GetV1()))
         return TRUE;
      return FALSE;
   }
};

template<class ARG_KEY>
AFX_INLINE UINT AFXAPI HashKey( const SplitEdge &key )
{
   UINT nKey = (UINT)key.GetV0()->GetID() + (UINT)key.GetV1()->GetID();
   return nKey;  
}


class UsedPoly
{
public:
   HPoly *m_pPoly;
   CMap<UINT, UINT, HVertex *, HVertex *> m_usedvertexmap;

   UsedPoly( HPoly *pPoly );
   BOOL operator == ( const UsedPoly &other ) const
   {
      return m_pPoly == other.m_pPoly;
   }
};


class ConnectCommand : public CommandInstance
{
public:
   CMap<UINT, UINT, HVertex *, HVertex *> m_allvertexmap;
   CMap<HPoly *, HPoly *, UsedPoly *, UsedPoly *> m_usedpolymap;
   CList<HEdge *> m_edgelist;

   CList<SplitEdge *> m_splitedgelist;

   CList<HVertex::HDeleteInfo *> m_addedvertices;
   CList<HEdge::HDeleteInfo *> m_addededges;

   ConnectCommand(HCommandPluginHelper *pPlugin);
   ~ConnectCommand();

   BOOL FindBestMatch( HPoly *pPoly, HVertex *pVertex, HVertex *&pOutFirstMatch, HVertex *&pOutSecondMatch );
   BOOL IsValidSplit(HPoly *pPoly, HVertex *pV0, HVertex *pV1);

   // PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL OnValidate(String &strOutError );
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
};

class ConnectCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( ConnectCommandPlugin, HCommandPluginHelper )

   ConnectCommandPlugin() 
   {
   }
//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};