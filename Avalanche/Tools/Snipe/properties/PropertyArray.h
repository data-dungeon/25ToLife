#pragma once

class Property;

class PropertyArray : public CArray<Property *>
{
public:
   Property *Find( const String &strMatchName, BOOL bRecursive=TRUE );
   BOOL Remove( Property *pProperty );
   void SetCachePointers( const PropertyArray *pCachePropertyArray );
};

