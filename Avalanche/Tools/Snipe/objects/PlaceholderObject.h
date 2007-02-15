#pragma once
#include "HierObject.h"

class PlaceholderObject : public HierObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(PlaceholderObject, HierObject)

private:
   String m_strFileText;

public:
   PlaceholderObject() {}
   PlaceholderObject( const String &strFileText ) :
      m_strFileText( strFileText )
   {
   }

// Operations
   void SetFileText( const String &strFileText ) { m_strFileText = strFileText; }
   const String &GetFileText() const { return m_strFileText; }

   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual BOOL Load( BaseStream &bs );
};
