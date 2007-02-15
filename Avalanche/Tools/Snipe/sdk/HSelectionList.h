#pragma once

#include "../Utility/BoundingBox.h"

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class HVertex;
class HEdge;
class HPoly;
class HSnipeObject;
class HSnipeObjectList;

class SNIPEEXPORT HSelectionList
{
public:
   // Construction
   static HSelectionList *New();

   void RemoveAll();
   BOOL IsEmpty() const;
   UINT GetCount() const;

   const BoundingBox &GetBoundingBox();
   void SetDirty();

   static void SetMouseOverObject(HSnipeObject *pMouseOverObject);
   static BOOL SetFocusObject(HSnipeObject *pFocusObject, HSnipeObject *pNearestFocusObject);

   static HSnipeObject *GetMouseOverObject();
   static HSnipeObject *GetFocusObject();
   static HSnipeObject *GetNearestFocusObject();

   void Add(HSnipeObject *pSnipeObject);
   void Remove(HSnipeObject *pSnipeObject);
   void Toggle(HSnipeObject *pSnipeObject);

   void Add(HSnipeObjectList *pList);
   void Remove(HSnipeObjectList *pList);
   void Toggle(HSnipeObjectList *pList);
   void SetTo(HSnipeObjectList *pList);

   //BOOL Find(HSnipeObject *pSnipeObject);
   POSITION GetHeadPosition();
   HSnipeObject *GetHead();

   HSnipeObject *GetNext(POSITION &pos);
   HSnipeObjectList *CreateSnipeObjectList();

   void operator delete ( void *pVoid );
};