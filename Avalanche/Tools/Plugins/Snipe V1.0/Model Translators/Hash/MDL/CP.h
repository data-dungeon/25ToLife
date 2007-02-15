//

#include "HVertex.h"

class HEdge;
class HPoly;

class CP 
{
private:
   UINT     m_nID;

   HVertex *m_pVertex;
   HEdge   *m_pNextEdge;
   HEdge   *m_pPrevEdge;

   CP      *m_pParentCP;
   CP      *m_pChildCP;
   CP      *m_pNextInSpline;
   CP      *m_pPrevInSpline;

   float    m_fInAlpha, m_fInGamma, m_fInMagnitude;
   float    m_fOutAlpha, m_fOutGamma, m_fOutMagnitude;

   UINT     m_bSmooth      : 1,
            m_bLoop        : 1,
            m_bConstrained : 1;

public:
   CP( UINT nID );
   CP( HVertex *pVertex );

   void Init( UINT nID );

   UINT GetID() const { return m_nID; }
   void SetID( UINT nID ) { m_nID = nID; }

   HVertex *GetVertex() const { return m_pVertex; }
   void SetVertex( HVertex *pVertex ) { m_pVertex = pVertex; }
   HEdge *GetNextEdge() const { return m_pNextEdge; }
   void SetNextEdge( HEdge *pEdge ) { m_pNextEdge = pEdge; }
   HEdge *GetPrevEdge() const { return m_pPrevEdge; }
   void SetPrevEdge( HEdge *pEdge ) { m_pPrevEdge = pEdge; }

   CP *NextInSpline( BOOL bLoop=FALSE ) const
   {
      if (bLoop || !IsLoop())
         return m_pNextInSpline;
      else
         return NULL;
   }
   void SetNextInSpline( CP *pNextInSpline ) { m_pNextInSpline = pNextInSpline; }
   CP *PreviousInSpline( BOOL bLoop=FALSE ) const
   {
      if (bLoop || (m_pPrevInSpline && !m_pPrevInSpline->IsLoop()))
         return m_pPrevInSpline;
      else
         return NULL;
   }
   void SetPreviousInSpline( CP *pPreviousInSpline ) { m_pPrevInSpline = pPreviousInSpline; }

   CP *GetParentCP() const { return m_pParentCP; }
   void SetParentCP( CP *pParentCP ) { m_pParentCP = pParentCP; }
   CP *GetChildCP() const { return m_pChildCP; }
   void SetChildCP( CP *pChildCP ) { m_pChildCP = pChildCP; }

   BOOL IsSmooth() const { return m_bSmooth; }
   void SetSmooth( BOOL bSmooth ) { m_bSmooth = bSmooth; }
   BOOL IsLoop() const { return m_bLoop; }
   void SetLoop( BOOL bLoop ) { m_bLoop = bLoop; }
   BOOL IsConstrained() const;
   void SetConstrained( BOOL bConstrained ) { m_bConstrained = bConstrained; }
   BOOL IsTConstrained() { return IsConstrained() && GetNumSurroundingEdges()==3; }
   BOOL IsAttached( HVertex *pVertex ) const { return m_pVertex == pVertex; }
   BOOL IsAttached( CP *pCP ) const { return m_pVertex == pCP->m_pVertex; }

   UINT GetNumSurroundingEdges() const { return m_pVertex->GetNumSurroundingEdges(); }
   HEdge *GetSurroundingEdgeAt( UINT nIndex ) const { return m_pVertex->GetSurroundingEdgeAt( nIndex ); }
   UINT GetNumSurroundingPolys() const { return m_pVertex->GetNumSurroundingPolys(); }
   HPoly *GetSurroundingPolyAt( UINT nIndex ) const { return m_pVertex->GetSurroundingPolyAt( nIndex ); }

   const Vector &GetPos() const { return m_pVertex->GetPos(); }
   void SetPos( const Vector &vPos ) { m_pVertex->SetPos( vPos ); }

   float GetInAlpha() const { return m_fInAlpha; }
   float GetInGamma() const { return m_fInGamma; }
   float GetInMagnitude() const { return m_fInMagnitude; }
   void SetInAlpha( float fAlpha ) { m_fInAlpha = fAlpha; }
   void SetInGamma( float fGamma ) { m_fInGamma = fGamma; } 
   void SetInMagnitude( float fMagnitude ) { m_fInMagnitude = fMagnitude; }
   float GetOutAlpha() const { return m_fOutAlpha; }
   float GetOutGamma() const { return m_fOutGamma; }
   float GetOutMagnitude() const { return m_fOutMagnitude; }
   void SetOutAlpha( float fAlpha ) { m_fOutAlpha = fAlpha; }
   void SetOutGamma( float fGamma ) { m_fOutGamma = fGamma; } 
   void SetOutMagnitude( float fMagnitude ) { m_fOutMagnitude = fMagnitude; }

   BOOL DrillForCP( CP *pCP ) const;
   CP *DrillForConstrained();
   CP *DrillForTop();
   CP *DrillForBottom();
   CP *DrillForTConstrained();
   BOOL IsTConstrainedCPInEdge( HPoly *pPoly, UINT nIndex );
   void InsertCPAfter( CP *pInsert );
   void MoveToStack( CP *pAttachTo );
   void MoveToStackWithChildren( CP *pAttachTo );
};

