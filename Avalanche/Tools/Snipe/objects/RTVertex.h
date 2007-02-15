#pragma once

class RTVertex 
{
protected:
   Vector          m_vPos;
   Vector          m_vNormal;
   Vector2         m_v2UV;
   RGBAFloat       m_rgbaColor;
   RTVertex       *m_pNextWithSameVertex;

public:
// Constructors
   RTVertex()
   {
      m_pNextWithSameVertex = NULL;
   }
 
   RTVertex( const RTVertex &other )
   {
      *this = other;
   }

// Operators
   RTVertex & operator = ( const RTVertex &other )
   {
      m_vPos = other.m_vPos;
      m_vNormal = other.m_vNormal;
      m_v2UV = other.m_v2UV;
      m_rgbaColor = other.m_rgbaColor;
      m_pNextWithSameVertex = NULL;
      return *this;
   }

   BOOL operator == ( const RTVertex &other ) const
   {
      return m_vPos == other.m_vPos
          && m_vNormal == other.m_vNormal
          && m_v2UV == other.m_v2UV
          && m_rgbaColor == other.m_rgbaColor;
   }

// Operations
   RTVertex *GetNextWithSameVertex() const { return m_pNextWithSameVertex; }
   void SetNextWithSameVertex( RTVertex *pNextWithSameVertex ) { m_pNextWithSameVertex = pNextWithSameVertex; }

   const Vector &GetPos() const { return m_vPos; }
   void SetPos( const Vector &vPos ) { m_vPos=vPos; if (m_pNextWithSameVertex) m_pNextWithSameVertex->SetPos( vPos ); }

   const Vector &GetNormal() const { return m_vNormal; }
   void SetNormal( const Vector &vNormal ) { m_vNormal = vNormal; }

   const Vector2 &GetUV() const { return m_v2UV; }
   void SetUV( const Vector2 &v2UV ) { m_v2UV = v2UV; }
   void SetUV( float u, float v ) { m_v2UV.Set( u, v ); }

   const RGBAFloat &GetColor() const { return m_rgbaColor; }
   void SetColor( const RGBAFloat &rgbaColor ) { m_rgbaColor = rgbaColor; }
};

class Vertex;

class RTVertexMapEntry : public RTVertex
{
protected:
   Vertex *m_pVertex;

public:
   BOOL operator == ( const RTVertexMapEntry &other ) const
   {
      return m_pVertex == other.m_pVertex
          && m_vNormal == other.m_vNormal
          && m_v2UV == other.m_v2UV
          && m_rgbaColor == other.m_rgbaColor;
   }

   Vertex *GetVertex() { return m_pVertex; }
   void SetVertex( Vertex *pVertex ) { m_pVertex = pVertex; }
};

typedef CMap< RTVertexMapEntry, const RTVertexMapEntry &, UINT, UINT > CRTVertexMap;

#ifdef __AFXTEMPL_H__
template<> AFX_INLINE UINT AFXAPI HashKey<const RTVertexMapEntry &>( const RTVertexMapEntry &key )
{
   return HashKey<const Vector &>(key.GetPos());
}
#endif

class RTEdge
{
private:
   UINT m_nVertexIndex[2];

public:

   RTEdge() {}
   RTEdge( UINT nVertexIndex0, UINT nVertexIndex1 )
   {
      m_nVertexIndex[0] = nVertexIndex0;
      m_nVertexIndex[1] = nVertexIndex1;
   }

   RTEdge( const RTEdge &other )
   {
      *this = other;
   }

   RTEdge &operator = ( const RTEdge &other )
   {
      m_nVertexIndex[0] = other.m_nVertexIndex[0];
      m_nVertexIndex[1] = other.m_nVertexIndex[1];
      return *this;
   }

   BOOL operator == ( const RTEdge &other ) const
   {
      if ((m_nVertexIndex[0] == other.m_nVertexIndex[0]) &&
          (m_nVertexIndex[1] == other.m_nVertexIndex[1]))
         return TRUE;
      if ((m_nVertexIndex[0] == other.m_nVertexIndex[1]) &&
          (m_nVertexIndex[1] == other.m_nVertexIndex[0]))
         return TRUE;
      return FALSE;
   }

   int GetVertexIndex( UINT nIndex ) const
   {
      ASSERT( nIndex < 2 );
      return m_nVertexIndex[nIndex];
   }

   void SetVertexIndex( UINT nIndex, UINT nVertexIndex )
   {
      ASSERT( nIndex < 2 );
      m_nVertexIndex[nIndex] = nVertexIndex;
   }

   void SetVertexIndices( UINT nVertexIndex0, UINT nVertexIndex1 )
   {
      m_nVertexIndex[0] = nVertexIndex0;
      m_nVertexIndex[1] = nVertexIndex1;
   }
};

#ifdef __AFXTEMPL_H__
template<> AFX_INLINE UINT AFXAPI HashKey<const RTEdge &>( const RTEdge &key )
{
	return (DWORD)(((DWORD_PTR)key.GetVertexIndex(0))>>4) + (DWORD)(((DWORD_PTR)key.GetVertexIndex(1))>>4);
}
#endif

class RTQuad
{
private:
   UINT m_nVertexIndex[4];

public:
   RTQuad() {}
   RTQuad( int nVertexIndex0, int nVertexIndex1, int nVertexIndex2, int nVertexIndex3 )
   {
      m_nVertexIndex[0] = nVertexIndex0;
      m_nVertexIndex[1] = nVertexIndex1;
      m_nVertexIndex[2] = nVertexIndex2;
      m_nVertexIndex[3] = nVertexIndex3;
   }

   RTQuad( const RTQuad &other )
   {
      *this = other;
   }

   RTQuad & operator = ( const RTQuad &other )
   {
      memcpy(m_nVertexIndex, other.m_nVertexIndex, sizeof(RTQuad));
      return *this;
   }

   UINT GetVertexIndex( UINT nIndex )
   {
      ASSERT( nIndex < 4 );
      return m_nVertexIndex[nIndex];
   }

   void SetVertexIndex( UINT nIndex, UINT nVertexIndex )
   {
      ASSERT( nIndex < 4 );
      m_nVertexIndex[nIndex] = nVertexIndex;
   }

   void SetVertexIndices( UINT nVertexIndex0, UINT nVertexIndex1, UINT nVertexIndex2, UINT nVertexIndex3 )
   {
      m_nVertexIndex[0] = nVertexIndex0;
      m_nVertexIndex[1] = nVertexIndex1;
      m_nVertexIndex[2] = nVertexIndex2;
      m_nVertexIndex[3] = nVertexIndex3;
   }

   void SetVertexIndices( UINT nVertexArray[/*4*/] )
   {
      memcpy(m_nVertexIndex, nVertexArray, 4*sizeof(UINT));
   }
};

class RTPoly 
{
private:
   UINT  m_nVertexCount;
   UINT *m_nVertexIndexArray;

public:
   RTPoly()
   {
      m_nVertexCount = 0;
      m_nVertexIndexArray=NULL;
   }
   RTPoly( UINT nVertexCount, UINT *nVertexIndexArray )
   {
      SetVertexIndices( nVertexCount, nVertexIndexArray );
   }
   RTPoly( const RTPoly &other )
   {
      *this = other;
   }
   ~RTPoly()
   {
      if (m_nVertexIndexArray)
         delete [] m_nVertexIndexArray;
   }

   RTPoly & operator = ( const RTPoly &other )
   {
      SetVertexCount( other.GetVertexCount());
      memcpy( m_nVertexIndexArray, other.m_nVertexIndexArray, sizeof(UINT) * GetVertexCount());
      return *this;
   }

   UINT GetVertexCount() const
   {
      return m_nVertexCount;
   }
   void SetVertexCount( UINT nVertexCount )
   {
      if (m_nVertexCount == nVertexCount)
         return;

      if (m_nVertexIndexArray)
         delete [] m_nVertexIndexArray;
      m_nVertexCount = nVertexCount;
      m_nVertexIndexArray = SNEW UINT[m_nVertexCount];
   }
   UINT GetVertexIndex( UINT nIndex )
   {
      ASSERT( nIndex < m_nVertexCount );
      return m_nVertexIndexArray[nIndex];
   }

   void SetVertexIndex( UINT nIndex, UINT nVertexIndex )
   {
      ASSERT( nIndex < m_nVertexCount );
      m_nVertexIndexArray[nIndex] = nVertexIndex;
   }

   void SetVertexIndices( UINT nVertexCount, UINT *nVertexIndexArray )
   {
      SetVertexCount( nVertexCount );
      memcpy( m_nVertexIndexArray, nVertexIndexArray, m_nVertexCount * sizeof(UINT));
   }

   UINT *GetVertexIndexArray() { return m_nVertexIndexArray; }
};

