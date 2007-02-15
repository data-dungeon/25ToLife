#pragma once

class SnipeObject;
class CGLViewControl;

class PickHit
{
   UINT m_nZMin;
   UINT m_nZMax;
   CArray<UINT> m_names;
public: 
   PickHit(void) : m_nZMin(0), m_nZMax(0) { }
   PickHit(const PickHit &s) : m_nZMin(s.m_nZMin), m_nZMax(s.m_nZMax) 
   { 
      m_names.Copy(s.m_names); 
   }
   PickHit & operator = ( const PickHit &s) 
   { 
      m_nZMin = s.m_nZMin;
      m_nZMax = s.m_nZMax;
      m_names.Copy(s.m_names);
      return *this;
   }
   UINT GetZMin() const { return m_nZMin; }
   UINT GetZMax() const { return m_nZMax; }
   void SetZMin(UINT zMin) { m_nZMin = zMin; }
   void SetZMax(UINT zMax) { m_nZMax = zMax; }

   UINT GetNumNames() const { return m_names.GetSize(); }
   UINT operator[] ( UINT nIndex ) const
   {
      ASSERT(nIndex < (UINT)m_names.GetSize());
      return m_names[nIndex];
   }
   UINT GetName(int nIndex) const 
   { 
      ASSERT(nIndex < m_names.GetSize());
      return m_names[nIndex];
   }

   void SetNumNames(int nCount) { m_names.SetSize(nCount); }
   void SetName(int nIndex, UINT nName)
   {
      ASSERT(nIndex < m_names.GetSize());
      m_names[nIndex] = nName;
   }
   void AddName(UINT nName)
   {
      m_names.Add(nName);
   }
};

class PickHitArray
{
   CArray<PickHit> m_hits;
public:
   PickHitArray() {}
   PickHitArray(const PickHitArray &s) { m_hits.Copy(s.m_hits); }
   PickHitArray & operator = ( const PickHitArray &s) 
   { 
      m_hits.Copy(s.m_hits);
      return *this;
   }
   UINT GetNumHits() const { return m_hits.GetSize(); }
   const PickHit &operator[] ( UINT nIndex ) const
   {
      ASSERT(nIndex < (UINT)m_hits.GetSize());
      return m_hits[nIndex];
   }
   const PickHit &GetHit ( UINT nIndex ) const
   {
      ASSERT(nIndex < (UINT)m_hits.GetSize());
      return m_hits[nIndex];
   }
   void SetNumHits(UINT nCount) { m_hits.SetSize(nCount); }
   void SetHit(UINT nIndex, const PickHit &hit) 
   { 
      ASSERT(nIndex < (UINT)m_hits.GetSize());
      m_hits[nIndex] = hit;
   }
   void AddHit( const PickHit &hit)
   {
      m_hits.Add(hit);
   }
};


class GLHitArray
{
public:
   // this class points into the buffer owned by GLHitArray
   class GLHit
   {
      GLuint m_nNumNames;
      GLuint m_nZMin;
      GLuint m_nZMax;
      GLuint m_nName; // first name (last one pushed on stack, with others to follow
   public:
      GLuint GetNumNames() const { return m_nNumNames; }
      GLuint GetZMin() const { return m_nZMin; }
      GLuint GetZMax() const { return m_nZMax; }
      // get name on the pick stack 
      // 0 represents the first one pushed
      GLuint GetName(UINT nIndex) const
      {
         ASSERT(nIndex < m_nNumNames);
         return *((&m_nName)+nIndex);
      }
   };

private:
   UINT m_nHitBufferSize; // allocated buffer size
   UINT m_nHits; // actual number of hits
   GLuint *m_pHitBuffer; // buffer for gl to store hit information
   GLHit **m_pHitPtrs; // pointers into hit buffer (1 for each hit)

public:
   GLHitArray() :
      m_nHits(0),
      m_nHitBufferSize(0),
      m_pHitBuffer(NULL),
      m_pHitPtrs(NULL)
   {
   }

   GLHitArray(UINT nBufferSize) :
      m_nHits(0),
      m_nHitBufferSize(0),
      m_pHitBuffer(NULL),
      m_pHitPtrs(NULL)
   {
      SetBufferSize(nBufferSize);
   }

   ~GLHitArray()
   {
      if (m_pHitBuffer)
         delete [] m_pHitBuffer;
      if (m_pHitPtrs)
         delete [] m_pHitPtrs;
   }

   void SetBufferSize( UINT nBufferSize )
   {
      if (m_pHitBuffer)
         delete [] m_pHitBuffer;
      if (m_pHitPtrs)
      {
         delete [] m_pHitPtrs;
         m_pHitPtrs = NULL;
      }
      m_pHitBuffer = SNEW GLuint[nBufferSize];
      m_nHitBufferSize = nBufferSize;
   }

   UINT GetBufferSize() { return m_nHitBufferSize; }

   UINT GetBufferSizeInBytes() { return m_nHitBufferSize * sizeof (GLuint); }

   GLuint *GetBuffer() { return m_pHitBuffer; }

   UINT GetNumHits() const { return m_nHits; }

   void SetNumHits( UINT nHits ) 
   { 
      m_nHits = nHits; 
      if (m_pHitPtrs)
         delete [] m_pHitPtrs;
      if (nHits)
      {
         m_pHitPtrs = SNEW GLHit *[nHits];
         UINT currentIndex = 0;
         for (UINT hitIndex = 0; hitIndex < nHits; hitIndex++)
         {
            m_pHitPtrs[hitIndex] = (GLHit *)&(m_pHitBuffer[currentIndex]);
            // move to next one
            currentIndex += m_pHitPtrs[hitIndex]->GetNumNames()+3;
            ASSERT(currentIndex <= m_nHitBufferSize);
         }
      }
   }

   const GLHit *operator[] ( UINT nIndex ) const
   {
      ASSERT(nIndex < m_nHits);
      return m_pHitPtrs[nIndex];
   }

   const GLHit *GetHit( UINT nIndex) const
   {
      ASSERT(nIndex < m_nHits);
      return m_pHitPtrs[nIndex];
   }
};

