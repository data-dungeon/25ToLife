#pragma once

#include "../Utility/BoundingBox.h"
#include "SnipeObject.h"

class Vertex;
class Edge;
class Poly;

class SelectionList
{
protected:
   SnipeObjectList m_objectlist;
   CMap<SnipeObject*,SnipeObject*,POSITION,POSITION> m_objectmap; // only for quick find for removing objects from list
   BoundingBox m_BoundingBox;
   UINT        m_bBoundsDirty    : 1,
               m_bSelectionDirty : 1,
               m_bSelectionDirtyOnTempCommand : 1,
               m_bMapDirty : 1;

public:
   static SnipeObject *m_pFocusObject;
   static SnipeObject *m_pNearestFocusObject;
   static SnipeObject *m_pMouseOverObject;

   static SnipeObject *GetMouseOverObject() { return m_pMouseOverObject; }
   static SnipeObject *GetFocusObject() { return m_pFocusObject; }
   static SnipeObject *GetNearestFocusObject() { return m_pNearestFocusObject; }

   static void SetMouseOverObject( SnipeObject *pMouseOverObject );
   static BOOL SetFocusObject( SnipeObject *pFocusObject, SnipeObject *pNearestFocusObject );
   static void SetFocusObjectOnlyNoCheck( SnipeObject *pFocusObject );
   static void SetNearestFocusObjectOnlyNoCheck( SnipeObject *pNearestFocusObject );

   SelectionList();
   ~SelectionList()
   {
      RemoveAll();
   }

   void RebuildMap();

   void RemoveAll();

   BOOL IsEmpty() const { return m_objectlist.IsEmpty(); }
   UINT GetCount() const { return m_objectlist.GetCount(); }

   const BoundingBox &GetBoundingBox();
   void SetBoundsDirty();
   
   void SetDirty();

   BOOL IsSelectionChanged(); // this resets the selection dirty flag.
   BOOL WasSelectionChangedOnTempCommand() const { return m_bSelectionDirtyOnTempCommand; } // If the IsSelectionChanged was set on a temp command then when it gets reset, this bit gets set so we know to notify on a CommitTempCommand
   void SetSelectionChangedOnTempCommand( BOOL bState ) { m_bSelectionDirtyOnTempCommand = bState; }

   void Add(SnipeObject *pSnipeObject);
   void Remove(SnipeObject *pSnipeObject);
   void Toggle(SnipeObject *pSnipeObject);
   
   void Add(SnipeObjectList *pList);
   void Remove(SnipeObjectList *pList);
   void SetTo(SnipeObjectList *pList);
   void Toggle(SnipeObjectList *pList);

   POSITION GetHeadPosition();
   SnipeObject *GetHead();

   SnipeObject *GetNext(POSITION &pos);
   SnipeObjectList *CreateSnipeObjectList();
};
