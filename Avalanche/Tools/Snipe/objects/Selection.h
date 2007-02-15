#pragma once
#include "ContainerObject.h"

class Selection : public HierObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(Selection, HierObject)

   class DeleteInfo : public HierObject::DeleteInfo
   {
   private:
      SelectionContainer *m_pSelectionContainer;

   public:
      SnipeObject::DeleteInfo *m_pSelectionContainerDeleteInfo;

      DeleteInfo( SnipeObject *pSnipeObject )
       : HierObject::DeleteInfo( pSnipeObject )
      {
         m_pSelectionContainer = NULL;
         m_pSelectionContainerDeleteInfo = NULL;
      }

      ~DeleteInfo()
      {
         if (m_pSelectionContainerDeleteInfo)
            delete m_pSelectionContainerDeleteInfo;
         SetOldSelectionContainer(NULL);
      }
      void SetOldSelectionContainer( SelectionContainer *pContainer ) { AssignWithReference( (SnipeObject *&)m_pSelectionContainer, (SnipeObject *)pContainer );  }
      SelectionContainer *GetOldSelectionContainer() const { return m_pSelectionContainer; }
   };
   virtual SnipeObject::DeleteInfo *AllocDeleteInfo() { return SNEW DeleteInfo(this); }

   String m_strSelection;

            Selection();
   virtual ~Selection();

   String GetSelectionDescription() const { return m_strSelection; }
   void SetSelectionDescription( const String &strDescription ) { m_strSelection = strDescription; }

   void RemoveAll();
   void AddObject( SnipeObject *pObject );
   void AddObjects( SnipeObjectList *pList );
   void RemoveObject( SnipeObject *pObject );
   void ToggleObject( SnipeObject *pObject );

// SnipeObject overrides
   virtual void FreeDeleteInfo( SnipeObject::DeleteInfo *pDeleteInfo ) { delete (DeleteInfo *)pDeleteInfo; }// DeleteInfo does not have a v-table, this must be here to get to the right destructor
   virtual BOOL Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError );
   virtual BOOL UnDelete( SnipeObject::DeleteInfo *pDeleteInfo );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL ParseData( BaseStream &bs, const DataHeader &dataheader );
};

class SelectionContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(SelectionContainer, ContainerObject)

            SelectionContainer();
   virtual ~SelectionContainer();

   UINT GetNumSelections()
   {
      return GetNumChildren();
   }

   Selection *GetSelectionAt( UINT nIndex )
   {
      return (Selection *)GetChildAt( nIndex );
   }

// SnipeObject overrides
   virtual void Draw( CGLBaseView *pView ) {} // don't recurse, it's a waiste of time
};