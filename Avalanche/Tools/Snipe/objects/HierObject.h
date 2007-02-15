#pragma once
#include "ParentableObject.h"
#include "Properties/Property.h"

class CBaseDoc;
class CGL3DView;
class Matrix44;
class CSnipeDocTemplate;
class PlaceholderObject;
class Rotoscope;

class HierObject : public ParentableObject
{
   DECLARE_MESSAGE_MAP()

public:
   DECLARE_SNIPEOBJECT_PURE(HierObject, ParentableObject)

   class DeleteInfo : public ParentableObject::DeleteInfo
   {
   public:
      int m_nChildIndex;

      DeleteInfo( SnipeObject *pSnipeObject )
       : ParentableObject::DeleteInfo( pSnipeObject )
      {
      }
   };
   virtual SnipeObject::DeleteInfo *AllocDeleteInfo() { return SNEW DeleteInfo(this); }

   class ConvertInfo
   {
   public:
      ConvertInfo(HierObject *pFrom, HierObject *pTo)
      {
         m_pFrom = pFrom;
         m_pTo = pTo;
         m_pFromDeleteInfo = NULL;
         m_pToDeleteInfo = NULL;
      }
      virtual ~ConvertInfo()
      {
         m_pFrom->DeReference();
         m_pTo->DeReference();
         if (m_pFromDeleteInfo)
            delete m_pFromDeleteInfo;
         if (m_pToDeleteInfo)
            delete m_pToDeleteInfo;
      }
      HierObject *m_pFrom, *m_pTo;
      DeleteInfo *m_pFromDeleteInfo;
      DeleteInfo *m_pToDeleteInfo;
      String m_name;

   public:
      BOOL Convert();
      void ReConvert();
      void UnConvert();
   };

   class ParentInfo
   {
   public:
      UINT m_nPrevChildIndex,m_nNewChildIndex;
      HierObject *m_pPrevParent,*m_pNewParent;
      HierObject *m_pChild;
   public:
      ParentInfo()
      {
         m_pPrevParent = NULL;
         m_pNewParent = NULL;
         m_pChild = NULL;
         m_nPrevChildIndex = -1;
         m_nNewChildIndex = -1;
      }
      virtual ~ParentInfo()
      {
      }
      void ReParent();
      void UnParent();
   };

   static StringProperty::Info m_strNotesInfo;

   StringProperty m_strNotes;

   String m_strName;
   CArray<HierObject *> m_childarray;
   Vector2   m_v2SchematicPos;
   CBaseDoc *m_pDocument; // set if it has an open doc.  This could be in a derived class called EditableObject

            HierObject();
   virtual ~HierObject();

// Operations
public:
//   const String &GetName() { return m_strName; }

   int FindChildIndex( HierObject *pChild );
   int AddChildAtTail( HierObject *pChild );
   void InsertChildAt( int nIndex, HierObject *pChild );
   UINT RemoveChild( HierObject *pChild );
   void DestroyChildren();

   PlaceholderObject *AddPlaceholderObject( const String &strFileText );
   void ComputeSchematicPositions();
   void CountDescendants( CArray<int> &descendantcountarray, int nLevel );
   void SetSchematicPosition( CArray<int> &descendantcountarray, CArray<int> &totaldescendantcountarray, int nLevel );

   void NotifyDescendants( ObjectNotificationMsg msg, WPARAM wParam = 0, LPARAM lParam = 0 );
   void NotifyObjectForEachDescendant( HierObject *pNotifyObject, ObjectNotificationMsg nMsg, WPARAM wParam ); // lParam will always be each descendant

   afx_msg void OnObjectEdit();
   afx_msg void OnEditDelete();
   afx_msg void OnUpdateEditDelete(CCmdUI *pCmdUI);

// Overridables
   virt_base virtual CSnipeDocTemplate *GetDocTemplate() { return NULL; }
   virt_base virtual String GetFileName();
   virt_base virtual ConvertInfo *ConvertTo(SnipeObjectType nType);
   virt_base virtual void ConvertFrom(HierObject *pHierObject) {}
   virt_base virtual BOOL AddRotoscope( Rotoscope *pRotoscope ) { return FALSE; }
   virt_base virtual void OnParent( HierObject *pHierObject ) {}// make this a child of pHierObject
   virt_base virtual ParentInfo *Parent( HierObject *pHierObject );
   virt_base virtual BOOL IsDraggable() { return FALSE; }

// InstanceObject overrides
   virtual String GetTitle() const;
   virtual void Draw( CGLBaseView *pView );
   virtual void DrawSchematic( CGLSchematicView *pView );

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual void PostConstruct();
   virtual void CountPickObjectType( CGLBaseView *pView, UINT &nCount );
   virtual void SetName( const String &name );
   virtual String GetMatchName() const;
   virtual int GetNumChildren() const { return m_childarray.GetSize(); }
   virtual BOOL HasVisibleChild() const;
   virtual SnipeObject *GetChildAt( int index ) { return m_childarray[index]; }
   virtual void FreeDeleteInfo( SnipeObject::DeleteInfo *pDeleteInfo ) { delete (DeleteInfo *)pDeleteInfo; }// DeleteInfo does not have a v-table, this must be here to get to the right destructor
   virtual BOOL Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError );
   virtual BOOL UnDelete( SnipeObject::DeleteInfo *pDeleteInfo );
   virtual void MarkSaveDependents();
   virtual catch_msg SnipeObject *CreateChild( const SnipeObjectInfo *pInfo );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL ParseData( BaseStream &bs, const DataHeader &dataheader );
   virtual void OnBeginDrag( CPoint point, CGLObjectTreeItem *pDragTreeItem );
   virtual void OnDoDrag( CPoint point, CGLObjectTreeItem *pDragTreeItem, CGLObjectTreeItem *pHitTreeItem );
   virtual void OnEndDrag( CPoint point, CGLObjectTreeItem *pDragTreeItem, CGLObjectTreeItem *pHitTreeItem );
   virtual void PreDestruct();
};
