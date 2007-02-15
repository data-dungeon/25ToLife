#pragma once
#include "HCommandPluginHelper.h"
#include "HVertex.h"

class MyVertexPeakInfo
{
public:
   BOOL m_bPeak;
   HVertexPeakInfo *m_pInfo;

   MyVertexPeakInfo(BOOL bPeak, HVertexPeakInfo *pInfo)
   {
      m_bPeak = bPeak;
      m_pInfo = pInfo;
   }

   ~MyVertexPeakInfo()
   {
      delete m_pInfo;
   }

   void Undo()
   {
      if (m_bPeak)
         m_pInfo->Smooth();
      else
         m_pInfo->Peak();
   }

   void Redo()
   {
      if (m_bPeak)
         m_pInfo->Peak();
      else
         m_pInfo->Smooth();
   }
};

////////////////////////////////////////////////////////////////////////////////////
class VertexEdge
{
public:
   HVertex *m_pVertex;
   HEdge *m_pEdge;

   VertexEdge(HVertex *pVertex, HEdge *pEdge )
   {
      m_pVertex = pVertex;
      m_pEdge = pEdge;
   }

   BOOL operator == ( const VertexEdge &other ) const
   {
      if (m_pVertex == other.m_pVertex && m_pEdge == other.m_pEdge)
         return TRUE;
      return FALSE;
   }
};

template<class ARG_KEY>
AFX_INLINE UINT AFXAPI HashKey( const VertexEdge &key )
{
   CRC crc;
   crc.AnalyzeData((unsigned long)key.m_pVertex->GetID());
   crc.AnalyzeData((unsigned long)key.m_pEdge->GetID());
   return (UINT)crc.GetResult();
}

////////////////////////////////////////////////////////////////////////////////////

class PeakCommand : public CommandInstance
{
public:
   CMap<VertexEdge *, VertexEdge *, UINT, UINT > m_vertexedgemap; // Input

   CList<MyVertexPeakInfo *> m_peakvertexinfolist;

   enum PeakType {PT_PEAK, PT_SMOOTH, PT_TOGGLE};
   PeakType m_nPeakType;

   PeakCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
      m_nPeakType = PT_PEAK;
      m_vertexedgemap.InitHashTable(GetPrimeLargerThan(100));
   }

   ~PeakCommand();

   void Add(VertexEdge *pVertexEdge);
   void AddVertexEdges(HEdge *pEdge);
   void AddVertexEdges(HPoly *pFace);
   void AddVertexEdges(HVertex *pVertex, HEdge *pEdge);

   MyVertexPeakInfo *DoPeak( VertexEdge *pVertexEdge );

   // PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
};

class PeakCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( PeakCommandPlugin, HCommandPluginHelper )

   //HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};