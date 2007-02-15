#pragma once

#include "HParentableObject.h"

class String;
class HStringProperty;
class HPlaceholderObject;
class HRotoscope;

class SNIPEEXPORT HHierObject : public HParentableObject
{
public:
   class SNIPEEXPORT HConvertInfo
   {
   public:
      void ReConvert();
      void UnConvert();

      void operator delete (void *pVoid);
   };

   class SNIPEEXPORT HParentInfo
   {
   public:
      void ReParent();
      void UnParent();

      void operator delete (void *pVoid);
   };

   void SetName(const String &name);
   String GetName() const;

   HStringProperty *GetNotes();

   HPlaceholderObject *AddPlaceholderObject( const String &strFileText );
   BOOL AddRotoscope( HRotoscope *pRotoscope );

   HConvertInfo *ConvertTo(SnipeObjectType nSOT);

   void EditObject();

   HParentInfo *Parent( HHierObject *pHierObject );
   void EditDocument();
};