#pragma once

#include "HParentableObject.h"
#include "../Utility/ATime.h"

class SNIPEEXPORT HProperty : public HParentableObject
{
public:
   class SNIPEEXPORT HInfo
   {
   public:
      static HInfo *New( const String &strName, const String &strMatchName );
      void operator delete (void *pVoid);

      void SetName( const String &strName ); // Localized
      String GetName() const;

      void SetMatchName( const String &strMatchName );
      String GetMatchName() const;
   };

   static HProperty *New( HInfo *pInfo, HProperty *pParent );

   BOOL IsSet() const;
   void SetSet( BOOL bSet );

   HSnipeObject *GetObject();

   catch_msg void StoreValueFromString( const String &strValue, const Time &time=TAGGEDTIME, BOOL bStoreUndo=TRUE);
   void GetValueString( String &strOutValue, const Time &time=TAGGEDTIME );
};
