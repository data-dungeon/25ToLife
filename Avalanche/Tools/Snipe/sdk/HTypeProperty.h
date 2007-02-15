#pragma once

#include "HIntProperty.h"

class SNIPEEXPORT HTypeProperty : public HIntProperty
{
public:
   class SNIPEEXPORT HInfo : public HIntProperty::HInfo
   {
    public:
      class SNIPEEXPORT HNode
      {
      public:
         const String &GetName() const;
         void SetName( const char *szName );

         const String &GetMatchName() const;
         void SetMatchName( const char *szMatchName );

         void operator delete[] (void *pVoid);
      };

      static HNode *AllocNodes( int nSize );
      static HNode *GetNodeAt(HNode nodearray[], int nIndex);

      static HTypeProperty::HInfo *New(const String &strName, const String &strMatchName, HNode nodearray[], int nNodeCount, int nDefaultValue);
      void SetDisplayAsRadios( BOOL bState );
      BOOL GetDisplayAsRadios();
   };

   static HTypeProperty *New( HInfo *pInfo, HProperty *pParent );
};