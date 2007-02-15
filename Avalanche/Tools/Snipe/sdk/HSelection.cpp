#include "stdafx.h"
#include "HSelection.h"
#include "Selection.h"

String HSelection::GetSelectionDescription() const
{
   return ((Selection *)this)->GetSelectionDescription();
}

void HSelection::SetSelectionDescription( const String &strDescription )
{
   ((Selection *)this)->SetSelectionDescription( strDescription );
}

void HSelection::RemoveAll()
{
   ((Selection *)this)->RemoveAll();
}

void HSelection::AddObject( HSnipeObject *pObject )
{
   ((Selection *)this)->AddObject( (SnipeObject *)pObject );
}

void HSelection::AddObjects( HSnipeObjectList *pList )
{
   ((Selection *)this)->AddObjects( (SnipeObjectList *)pList );
}

void HSelection::RemoveObject( HSnipeObject *pObject )
{
   ((Selection *)this)->RemoveObject( (SnipeObject *)pObject );
}

void HSelection::ToggleObject( HSnipeObject *pObject )
{
   ((Selection *)this)->ToggleObject( (SnipeObject *)pObject );
}

UINT HSelectionContainer::GetNumSelections()
{
   return ((SelectionContainer *)this)->GetNumChildren();
}

HSelection *HSelectionContainer::GetSelectionAt( UINT nIndex )
{
   return (HSelection *)((SelectionContainer *)this)->GetSelectionAt( nIndex );
}

HSelection *HSelectionContainer::FindSelection( const String &strName )
{
   return (HSelection *)((SelectionContainer *)this)->FindObject( strName );
}
