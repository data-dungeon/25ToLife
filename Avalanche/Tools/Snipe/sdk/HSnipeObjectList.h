#pragma once

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

#include "Enum.h"

class HSnipeObject;
class BoundingBox;
class HCGLBaseView;

class SNIPEEXPORT HSnipeObjectList
{
public:
   // Construction
   static HSnipeObjectList *New();

   // Head / Tail Access
   HSnipeObject *GetHead(); // Cannot be empty
   HSnipeObject *GetTail(); // Cannot be empty

   // Operations
   POSITION AddHead( HSnipeObject *pSnipeObject );
   POSITION AddTail( HSnipeObject *pSnipeObject );
   void AddTail( HSnipeObjectList *pSnipeObjectList );
   void RemoveAll();
   void RemoveHead();
   void RemoveTail();

   //Iteration
   POSITION GetHeadPosition() const;
   HSnipeObject *GetNext(POSITION &pos);
   HSnipeObject *GetPrev(POSITION &pos);
   POSITION GetTailPosition() const;

   //Retrieval/Modification
   HSnipeObject *GetAt( POSITION pos ) const;
   void RemoveAt( POSITION pos );
   void SetAt( POSITION pos, HSnipeObject *pSnipeObject );

   void ConvertCompatibleObjectsToSOT( SnipeObjectType nSOT );

   //Insertion
   POSITION InsertAfter( POSITION pos, HSnipeObject *pSnipeObject );
   POSITION InsertBefore( POSITION pos, HSnipeObject *pSnipeObject );

   //Searching
	POSITION Find( HSnipeObject *pObject, POSITION startAfter = NULL) const;
   POSITION FindIndex( UINT nIndex ) const;

   //Status
   UINT GetSize() const;
   BOOL IsEmpty() const;

   void ComputeBoundingBox( BoundingBox &bbox, HCGLBaseView *pView=NULL );

   void operator delete ( void *pVoid );
};