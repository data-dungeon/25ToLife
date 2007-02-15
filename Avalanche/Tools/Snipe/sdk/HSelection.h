#pragma once

#include "HContainerObject.h"

class SNIPEEXPORT HSelection : public HHierObject
{
public:
   String GetSelectionDescription() const;
   void SetSelectionDescription( const String &strDescription );

   void RemoveAll();
   void AddObject( HSnipeObject *pObject );
   void AddObjects( HSnipeObjectList *pList );
   void RemoveObject( HSnipeObject *pObject );
   void ToggleObject( HSnipeObject *pObject );
};

class SNIPEEXPORT HSelectionContainer : public HContainerObject
{
public:
   UINT GetNumSelections();
   HSelection *GetSelectionAt( UINT nIndex );
   HSelection *FindSelection( const String &strName );
};
