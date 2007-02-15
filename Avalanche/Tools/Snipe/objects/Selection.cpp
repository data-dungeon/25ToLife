//

#include "stdafx.h"
#include "Selection.h"

/////////////////////////////////////////////////////
// Selection

IMPLEMENT_SNIPEOBJECT_CREATE( Selection )

Selection::Selection()
{
}

Selection::~Selection()
{
}

void Selection::RemoveAll()
{
   m_strSelection.Empty();
}

void Selection::AddObject( SnipeObject *pObject )
{
   GetMainInterface()->SetCurrentWorkingObject( GetCommandObject() );

   StringList strErrorList;
   SnipeObjectList *pObjectList = GetMainInterface()->DescriptionToObjects( m_strSelection, strErrorList );

   if (!strErrorList.IsEmpty())
   {
      for (POSITION pos = strErrorList.GetHeadPosition(); pos; )
      {
         String &strError = strErrorList.GetNext(pos);
         String strMsg("Error adding object to Selection: %1.\n");
         strMsg.Replace("%1", strError);
         g_outputWindow << strMsg;
      }
   }

   pObjectList->AddTail( pObject );

   GetMainInterface()->ObjectsToDescription( pObjectList, m_strSelection );
   delete pObjectList;
}

void Selection::AddObjects( SnipeObjectList *pList )
{
   GetMainInterface()->SetCurrentWorkingObject( GetCommandObject() );

   StringList strErrorList;
   SnipeObjectList *pObjectList = GetMainInterface()->DescriptionToObjects( m_strSelection, strErrorList );

   if (!strErrorList.IsEmpty())
   {
      for (POSITION pos = strErrorList.GetHeadPosition(); pos; )
      {
         String &strError = strErrorList.GetNext(pos);
         String strMsg("Error adding object to Selection: %1.\n");
         strMsg.Replace("%1", strError);
         g_outputWindow << strMsg;
      }
   }

   pObjectList->AddTail( pList );

   GetMainInterface()->ObjectsToDescription( pObjectList, m_strSelection );
   delete pObjectList;
}

void Selection::RemoveObject( SnipeObject *pObject )
{
   GetMainInterface()->SetCurrentWorkingObject( GetCommandObject() );

   StringList strErrorList;
   SnipeObjectList *pObjectList = GetMainInterface()->DescriptionToObjects( m_strSelection, strErrorList );

   if (!strErrorList.IsEmpty())
   {
      for (POSITION pos = strErrorList.GetHeadPosition(); pos; )
      {
         String &strError = strErrorList.GetNext(pos);
         String strMsg("Error removing object from Selection: %1.\n");
         strMsg.Replace("%1", strError);
         g_outputWindow << strMsg;
      }
   }

   POSITION pos = pObjectList->Find( pObject );
   if (pos)
      pObjectList->RemoveAt( pos );

   GetMainInterface()->ObjectsToDescription( pObjectList, m_strSelection );
   delete pObjectList;
}

void Selection::ToggleObject( SnipeObject *pObject )
{
   GetMainInterface()->SetCurrentWorkingObject( GetCommandObject() );

   StringList strErrorList;
   SnipeObjectList *pObjectList = GetMainInterface()->DescriptionToObjects( m_strSelection, strErrorList );

   if (!strErrorList.IsEmpty())
   {
      for (POSITION pos = strErrorList.GetHeadPosition(); pos; )
      {
         String &strError = strErrorList.GetNext(pos);
         String strMsg("Error toggling object from Selection: %1.\n");
         strMsg.Replace("%1", strError);
         g_outputWindow << strMsg;
      }
   }

   POSITION pos = pObjectList->Find( pObject );
   if (pos)
      pObjectList->RemoveAt( pos );
   else
      pObjectList->AddTail( pObject );

   GetMainInterface()->ObjectsToDescription( pObjectList, m_strSelection );
   delete pObjectList;
}

void Selection::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   SnipeObjectHeader header( this, bWriteHeader, bs );

   if (!m_strSelection.IsEmpty())
   {
      DataHeader header( bs, "Objects" );

      StringList *pStringList = m_strSelection.Split( ' ' );
      if (pStringList)
      {
         bs << mar << pStringList->GetCount() << nl;

         for (POSITION pos=pStringList->GetHeadPosition(); pos; )
         {
            String &strObject = pStringList->GetNext( pos );
            bs << mar << strObject << nl;
         }
         delete pStringList;
      }
   }

   HierObject::Save( bs );
}

BOOL Selection::ParseData( BaseStream &bs, const DataHeader &dataheader )
{
   if (dataheader.m_strKey == "Objects")
   {
      bs.ReadMargin();

      UINT nCount;
      bs >> nCount;

      String strLine;
      for (UINT nIndex=0; nIndex<nCount; nIndex++)
      {
         bs.ReadMargin();
         bs.ReadLine( strLine );

         if (!m_strSelection.IsEmpty())
            m_strSelection += " ";

         m_strSelection += strLine;
      }
   }

   return HierObject::ParseData( bs, dataheader );
}

BOOL Selection::Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError )
{
   // if we are the last selection, we won't detach ourself, we will detach the selection container
   DeleteInfo *&pSelectionDeleteInfo = (DeleteInfo *&)pDeleteInfo;
   if (pSelectionDeleteInfo == NULL)
      pSelectionDeleteInfo = SNEW DeleteInfo( this );

   if (GetParent() && GetParent()->GetNumChildren()==1)
   {
      pSelectionDeleteInfo->SetOldSelectionContainer((SelectionContainer *)GetParent());
      pSelectionDeleteInfo->GetOldSelectionContainer()->Delete( (SnipeObject::DeleteInfo *)pSelectionDeleteInfo->m_pSelectionContainerDeleteInfo, strOutError );
      return SnipeObject::Delete( pDeleteInfo, strOutError );
   }
   else
      return HierObject::Delete( pDeleteInfo, strOutError );
}

BOOL Selection::UnDelete( SnipeObject::DeleteInfo *pDeleteInfo )
{
   DeleteInfo *pSelectionDeleteInfo = (DeleteInfo *)pDeleteInfo;
   if (pSelectionDeleteInfo->GetOldSelectionContainer())
   {
      pSelectionDeleteInfo->GetOldSelectionContainer()->UnDelete( (SnipeObject::DeleteInfo *)pSelectionDeleteInfo->m_pSelectionContainerDeleteInfo );
      pSelectionDeleteInfo->GetOldSelectionContainer()->FreeDeleteInfo( pSelectionDeleteInfo->m_pSelectionContainerDeleteInfo );
      pSelectionDeleteInfo->m_pSelectionContainerDeleteInfo = NULL;
      return TRUE;
   }
   else
      return HierObject::UnDelete( pDeleteInfo );
}

/////////////////////////////////////////////////////
// SelectionContainer

IMPLEMENT_SNIPEOBJECT_CREATE( SelectionContainer )

SelectionContainer::SelectionContainer()
{
   m_strName = "Selections";
}

SelectionContainer::~SelectionContainer()
{
}
