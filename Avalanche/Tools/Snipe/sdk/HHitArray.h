#pragma once

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class SNIPEEXPORT HPickHit
{
public: 
   static HPickHit *New();

   UINT GetZMin() const;
   UINT GetZMax() const;
   UINT GetNumNames() const;
   UINT GetName(int nIndex) const;
   void operator delete (void *pVoid);
};

class SNIPEEXPORT HPickHitArray
{
public:
   static HPickHitArray *New();

   UINT GetNumHits() const;
   HPickHit *GetHit ( UINT nIndex ) const;
   void operator delete (void *pVoid);
};
