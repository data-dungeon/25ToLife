inline UINT PolyModel::GetNumVertices()
{
   return m_vertexlist.GetSize();
}

inline Vertex *PolyModel::AddVertex()
{
   Vertex *pVertex = CreateVertex();
   m_vertexlist.Add(pVertex);
   return pVertex;
}

inline Vertex *PolyModel::AddVertex( UINT nID )
{
   Vertex *pVertex = CreateVertex( nID );
   m_vertexlist.Add(pVertex);
   return pVertex;
}

inline Vertex *PolyModel::AddVertex( const Vector &pos, Vertex::DeleteInfo **pOutVertexDeleteInfo/*=NULL*/, BOOL bMirrorIfInMirrorMode/*=FALSE*/ )
{
   Vertex *pVertex = AddVertex();
   pVertex->SetPos(pos);

   if (pOutVertexDeleteInfo)
      *pOutVertexDeleteInfo = SNEW Vertex::DeleteInfo( pVertex );


   if (bMirrorIfInMirrorMode && SnipeObject::IsMirrorMode())
   {
      static BOOL bInMirror;
      if (!bInMirror)
      {
         bInMirror = TRUE;

         Vector vMirrorPos(pos);
         vMirrorPos.x = -vMirrorPos.x;

         Vertex::DeleteInfo **pMirrorDeleteInfo = NULL;
         if (pOutVertexDeleteInfo)
            pMirrorDeleteInfo = (Vertex::DeleteInfo **) &(*pOutVertexDeleteInfo)->m_pMirrorDeleteInfo;

         Vertex *pMirrorVertex = AddVertex( vMirrorPos, pMirrorDeleteInfo );
         pMirrorVertex->SetMirrorObject( pVertex );

         bInMirror = FALSE;
      }
   }

   return pVertex;
}

inline void PolyModel::RemoveVertex(Vertex *pVertex)
{
   m_vertexlist.Remove( pVertex );
}

inline UINT PolyModel::GetNumVertexNormals()
{
   return m_vertexnormallist.GetSize();
}

inline VertexNormal *PolyModel::AddVertexNormal()
{
   VertexNormal *pVertexNormal = VertexNormal::Create();
   m_vertexnormallist.Add(pVertexNormal);
   return pVertexNormal;
}

inline VertexNormal *PolyModel::AddVertexNormal( UINT nID )
{
   VertexNormal *pVertexNormal = VertexNormal::CreateWithID( nID );
   m_vertexnormallist.Add(pVertexNormal);
   return pVertexNormal;
}

inline VertexNormal *PolyModel::AddVertexNormal(const Vector &dir)
{
   VertexNormal *pVertexNormal = VertexNormal::Create();
   m_vertexnormallist.Add(pVertexNormal);
   pVertexNormal->SetDir(dir);
   return pVertexNormal;
}

inline UINT PolyModel::GetNumEdges()
{
   return m_edgelist.GetSize();
}

inline Edge *PolyModel::AddEdge()
{
   Edge *pEdge = CreateEdge();
   m_edgelist.Add(pEdge);
   return pEdge;
}

inline Edge *PolyModel::AddEdge( UINT nID )
{
   Edge *pEdge = CreateEdge( nID );
   m_edgelist.Add(pEdge);
   return pEdge;
}

inline Edge *PolyModel::AddEdge( Vertex *pV0, Vertex *pV1)
{
   // previous built edges
   Edge *pEdge = Edge::FindEdge( pV0, pV1 );

   if (!pEdge) // Edge doesn't already exist
   {
      pEdge = AddEdge();
      pEdge->SetFirstVertex( pV0 );
      pEdge->SetSecondVertex( pV1 );
   }
   return pEdge;
}

inline void PolyModel::RemoveEdge(Edge *pEdge)
{
   m_edgelist.Remove( pEdge );
}

inline UINT PolyModel::GetNumPolys()
{
   return m_polylist.GetSize();
}

inline Poly *PolyModel::AddPoly()
{ 
   Poly *pPoly = CreatePoly();
   m_polylist.Add( pPoly );
   return pPoly;
}

inline Poly *PolyModel::AddPoly( UINT nID )
{ 
   Poly *pPoly = CreatePoly( nID );
   m_polylist.Add( pPoly );
   return pPoly;
}

inline POSITION PolyModel::GetHeadEdgePosition()
{
   return m_edgelist.GetHeadPosition();
}