#pragma once

class HProperty;

class SNIPEEXPORT HPropertyArray
{
public:
   //construction
   static HPropertyArray *New();

   //Atributes
   UINT GetSize() const;
   BOOL IsEmpty() const;
   void SetSize( UINT nSize, UINT nGrowBy = -1);

   //Operations
   void RemoveAll();
   void FreeExtra();

   //Element Access
   HProperty *&GetAt(UINT nIndex);
   const HProperty *&GetAt(UINT nIndex) const;
   void SetAt(UINT nIndex, HProperty *pProperty);

   //Growing the Array
   UINT Add(HProperty *pProperty);
   UINT Append( const HPropertyArray & );
   void Copy( const HPropertyArray & );
   void SetAtGrow( UINT nIndex, HProperty *pProperty);

   //Insertion - Removal
   void InsertAt( UINT nIndex, HProperty *pProperty, UINT nCount = 1 );
   void RemoveAt( UINT nIndex, UINT nCount = 1 );

   //Operators
   HProperty *&operator[] (int i );
   const HProperty *&operator[] (int i ) const;
   void operator delete( void *pVoid ); 

   //Find
   HProperty *Find(const String &strMatchName, BOOL bRecursive=TRUE);
};
