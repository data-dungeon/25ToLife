#pragma once

#include "SDK/Enum.h"
#include "GUI/GLImageList.h"

#define SNIPEVERSION 1.0f

class SimpleComponentList;
class SnipeObjectList;
class CommandUIPlugin;
typedef CList<CommandUIPlugin *> CommandUIPluginList;
class PropertyArray;
class CGLControl;
class CGLBaseView;
class CGLObjectTreeItem;
class CGLTreeControl;
class CGL3DView;
class CommandList;
class SelectionList;
class BoundingBox;
class FileInfoProperty;
class BaseStream;
class Property;
class Component;
class CGLSchematicView;

#define MIRRORTOLERANCE 1.e-3

enum {
   ICONINDEX_UNKOWN,
   ICONINDEX_PROJECT,
   ICONINDEX_CONTAINER,
   ICONINDEX_CAMERA,
   ICONINDEX_LIGHT,
   ICONINDEX_MODEL,
   ICONINDEX_SCENE,
   ICONINDEX_BONE,
   ICONINDEX_MOTIONCURVE,
   ICONINDEX_MOTION,
   ICONINDEX_MOTIONCHANNEL,
};

class SnipeObject;

typedef BOOL (* PFN_ONOBJECTNOTIFY)( void *pData, ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );

class NotifyNode 
{
public:
   NotifyNode  *m_pNextNotifyNode;
   UINT         m_notificationmsgs;

   NotifyNode( UINT msgs )
   {
      m_pNextNotifyNode = NULL;
      m_notificationmsgs = msgs;
   }
   virtual ~NotifyNode() {}

// Overridables
   virt_base virtual SnipeObject *GetSnipeObject() { return NULL; }
   virt_base virtual PFN_ONOBJECTNOTIFY GetPFN() { return NULL; }
   virt_base virtual void *GetData() { return NULL; }
   virt_base virtual void OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam ) = 0;
};

class CallbackNotifyNode : public NotifyNode
{
public:
   PFN_ONOBJECTNOTIFY m_pfnOnObjectNotify;
   void              *m_pData;

   CallbackNotifyNode( PFN_ONOBJECTNOTIFY pfnOnObjectNotify, void *pData, UINT msgs )
      : NotifyNode( msgs )
   {
      ASSERT( pfnOnObjectNotify );

      m_pfnOnObjectNotify = pfnOnObjectNotify;
      m_pData = pData;
   }

   // NotifyNode overrides
   virtual PFN_ONOBJECTNOTIFY GetPFN() { return m_pfnOnObjectNotify; }
   virtual void *GetData() { return m_pData; }
   virtual void OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
   {
      m_pfnOnObjectNotify( m_pData, msg, wParam, lParam );
   }
};

class SnipeObjectInfo;

class SnipeObjectInfoList
{
public:
   CList< SnipeObjectInfo * > m_snipeobjectinfolist;
   CMap< SnipeObjectType, SnipeObjectType, SnipeObjectInfo *, SnipeObjectInfo *> m_sotmap;
   CMap< String, const String &, SnipeObjectInfo *, SnipeObjectInfo *> m_classnamemap;
   CMap< String, const String &, SnipeObjectInfo *, SnipeObjectInfo *> m_matchnamemap;

    SnipeObjectInfoList();
   ~SnipeObjectInfoList();
   SnipeObjectInfo *GetSnipeObjectInfoBySOT(SnipeObjectType nSOT);
   SnipeObjectInfo *GetSnipeObjectInfoByClassName(const String &strClassName);
   SnipeObjectInfo *GetSnipeObjectInfoByMatchName(const String &strMatchName);
   void Add( SnipeObjectInfo *pInfo );
   void Remove( SnipeObjectInfo *pInfo );
   BOOL IsEmpty() const;
};


typedef SnipeObject *(* PFN_CREATESNIPEOBJECTARRAY)(UINT nCount);
typedef SnipeObject *(* PFN_CREATESNIPEOBJECT)();

SnipeObjectType GetFileType( const String &strFileName );
BOOL IsFileOfType( const String &strFileName, SnipeObjectType nSOT );

class SnipeObjectInfo
{
private:
   String m_strLocalizedName;
   GLImageList m_glImageListIcon;
   static SnipeObjectInfoList *m_pSnipeObjectInfoList;

public:
   SnipeObjectType m_nSOT;
   String m_strClassName;
   String m_strMatchName;
   UINT m_nResourceID;
   PFN_CREATESNIPEOBJECT m_pfnCreate;
   PFN_CREATESNIPEOBJECTARRAY m_pfnCreateArray;
   int m_bNumPickElements;
   BOOL m_bIsComponent;

    SnipeObjectInfo( SnipeObjectType nSOT, const char *szClassName, const char *szMatchName, PFN_CREATESNIPEOBJECT pfnCreate, PFN_CREATESNIPEOBJECTARRAY pfnCreateArray, int nNumPickElements, BOOL bIsComponent=FALSE);
   ~SnipeObjectInfo();

   const String &GetLocalizedName();
   GLImageList &GetImageListIcon();

   static SnipeObjectInfo *GetBySOT(SnipeObjectType nSOT) { return m_pSnipeObjectInfoList->GetSnipeObjectInfoBySOT( nSOT); }
   static SnipeObjectInfo *GetByClassName(const String &strClassName) { return m_pSnipeObjectInfoList->GetSnipeObjectInfoByClassName( strClassName ); }
   static SnipeObjectInfo *GetByMatchName(const String &strMatchName) { return m_pSnipeObjectInfoList->GetSnipeObjectInfoByMatchName( strMatchName ); }

   SnipeObject *CreateObject() const;
   SnipeObject *CreateObjectArray(UINT nCount);

   int GetNumPickElements() { return m_bNumPickElements; }
};

class CGLTexCoordView;
class SelectionContainer;
class Selection;
class DataHeader;
class PointerPropertyNode;

class SnipeObject
{
friend class SelectionList;
protected:
   DECLARE_MESSAGE_MAP()       // base class - no {{ }} macros
   void SetSelected( BOOL bSelect ); // Never call directly, call SelectionList::Add

public:
   class DeleteInfo
   {
   private:
      SnipeObject *m_pSnipeObject;

   public:
      DeleteInfo *m_pMirrorDeleteInfo;
      PointerPropertyNode *m_pPointerPropertyNodeHead; // list of PointerProperties that pointed at us

      DeleteInfo( SnipeObject *pSnipeObject );
      virtual ~DeleteInfo();

      void SetSnipeObject( SnipeObject *pSnipeObject );
      SnipeObject *GetSnipeObject() { return m_pSnipeObject; }

      void UnDelete();
      void Delete();
   };
   virtual SnipeObject::DeleteInfo *AllocDeleteInfo() { return SNEW DeleteInfo(this); }

   NotifyNode  *m_pNotifyNodeHead;
   SnipeObject *m_pMirror;
   static SnipeObject *m_pPrevCommandObject;

   union {
      ULONG    m_nSnipeObjectFlags;
      struct {
      ULONG    m_nRefCount      : 8,
               m_bSelected      : 1,
               m_bSelectedAtDisconnect : 1,
               m_bFocusAtDisconnect : 1,
               m_bNearestFocusAtDisconnect : 1,
               m_bVisible       : 1,
               m_bLocked        : 1,
               m_bTransformed   : 1,
               m_bUserNamed     : 1,
               m_bSaveDependent : 1,
               m_bMirrorDirty   : 1,
               m_bComponent     : 1, 
               m_bInList        : 1,
               m_bAllowReinsert : 1, // To aid in undo so that things get reinserted in proper order
               m_bCurrent       : 1, // PropertyDriver
               m_bNotifiedEndProjectLoad : 1,
               m_bSchematicPositionsDirty : 1,
               m_bTempBit       : 1, // Used for anyone who wants it, but it's only good for the duration of your function
               m_bIsVisibleInTree : 1;
      };
   };

   static BOOL m_bMirrorMode;
   static int m_nCurrentPickElement;

            SnipeObject();
   virtual ~SnipeObject();

// Operations
   static int GetNumPickElements() { return 1; }
   static int GetCurrentPickElement() { return m_nCurrentPickElement; }
   static void SetCurrentPickElement(int nCurrentPickElement) { m_nCurrentPickElement = nCurrentPickElement; }

   void Destroy() // Always call this instead of delete!!!!!!, works for components also.  SDK can call delete because operator handles it
   {
      Reference(); // so that PreDestruct doesn't delete this
      PreDestruct();
      delete this; // because DeReference would recall Destroy
   }

   static SnipeObjectType GetThisSOT() { return SOT_SnipeObject; }

   void Reference()
   {
      ASSERT(m_nRefCount<255);

      m_nRefCount++;
   }
   BOOL DeReference( BOOL bDestroyWhenUnreferenced=TRUE )
   {
      ASSERT(this);
      ASSERT( m_nRefCount );

      m_nRefCount--;

      if (bDestroyWhenUnreferenced && !IsReferenced())
      {
         ASSERT(!m_bInList);
         Destroy();
         return TRUE;
      }
      return FALSE;
   }
   BOOL IsReferenced()
   {
      return m_nRefCount > 0;
   }
   UINT GetRefCount() { return m_nRefCount; }

   BOOL IsInstance() const { return !IsCache(); }
   BOOL IsSelected() const { return m_bSelected; }
   void Select();

   BOOL IsMouseOverObject();
   BOOL IsFocusObject();
   BOOL IsNearestFocusObject();

   BOOL IsVisible() { return m_bVisible; }
   BOOL IsLocked() { return m_bLocked; }
   BOOL IsInList() const { return m_bInList; }

   static void SetMirrorMode( BOOL bMirrorMode ) { m_bMirrorMode = bMirrorMode; }
   static BOOL IsMirrorMode() { return m_bMirrorMode; }

   void SetMirrorObject( SnipeObject *pObject );
   SnipeObject *GetMirrorObject();
   void SetMirrorDirty() { m_bMirrorDirty=TRUE; m_pMirror=NULL; } 

   CommandUIPluginList *GetCommandUIPluginList(SnipeObjectType nType);
   BOOL IsComponent() const { return m_bComponent; }

   void InvalidateDependantViews();

   BOOL AddNotifyNode( UINT msgs, SnipeObject *pObject );
   BOOL RemoveNotifyNode( SnipeObject *pObject, UINT msgs=NOTIFYOBJ_ALL );
   BOOL AddCallback( UINT msgs, PFN_ONOBJECTNOTIFY pfnOnObjectNotify, void *pData );
   BOOL RemoveCallback( PFN_ONOBJECTNOTIFY pfnOnObjectNotify, void *pData, UINT msgs=NOTIFYOBJ_ALL );
   BOOL NotifyNodes( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   BOOL IsUsedBy( SnipeObject *pObject ) const;

   // Search
   SnipeObject *FindComponent(const String &strComponentConst, String *pError);
   BOOL FindComponents(SnipeObjectList *pObjectList, const String &strComponentConst, StringList &strErrorList);
   BOOL IsDesendantOf(SnipeObject *pObject) const;
   SnipeObject *GetCommandObject(BOOL bSkipThis=TRUE);

   SnipeObject *FindObject( const String &strMatchName, SnipeObject *pExclude=NULL );
   String GetUniqueName( const String &strName );
   SnipeObject *FindDescendant(const String &strMatchName);

   Property *FindProperty( const String &strPropertyMatchName );

   SnipeObjectList *CreateListFromSOT(SnipeObjectType sot);
   SnipeObjectList *CreateListFromSOT(const String &strObjectType);

   void SaveFile( BOOL bSaveAs=FALSE );

   Selection *AddSelection();

   void Copy( SnipeObject *pSnipeObject );
   SnipeObject *Duplicate();

   afx_msg void OnUpdatePluginCommand( CCmdUI *pCmdUI );
   afx_msg BOOL OnPluginCommand( UINT nID );

// Overridables
   virt_base virtual void PostConstruct();
   virt_base virtual void PreDestruct();
   virt_base virtual void SetName( const String &name ) {}
   virt_base virtual SnipeObject *GetParent() const { return NULL; }
   virt_base virtual FileInfoProperty *GetFileInfo() { return NULL; }
   virt_base virtual int GetNumChildren() const { return 0; }
   virt_base virtual SnipeObject *GetChildAt( int index ) { return NULL; }
   virt_base virtual BOOL HasVisibleChild() const { return FALSE; }
   virt_base virtual CommandUIPlugin *GetCommandUIPlugin(UINT nID);
   virt_base virtual const SnipeObject *IsKindOf( SnipeObjectType nType ) const;
   virt_base virtual SnipeObjectInfo *GetSnipeObjectInfo() { return NULL; }
	virt_base virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
   virt_base virtual String GetTitle() const;
   virt_base virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virt_base virtual BOOL IsCache() const { return TRUE; }
   virt_base virtual BOOL IsCommandObject()  { return FALSE; }
   virt_base virtual BOOL IsPickable() { return FALSE; }
   virt_base virtual BOOL IsDeletable() { return TRUE; }
   virt_base virtual BOOL IsHidable() { return FALSE; }
   virt_base virtual BOOL IsLockable() { return FALSE; }
   virt_base virtual BOOL IsSelectable() { return TRUE; }
   virt_base virtual BOOL IsProperty() const { return FALSE; }
   virt_base virtual void SetVisible( BOOL bVisible );
   virt_base virtual void SetVisibleInTree( BOOL bVisible );
   virt_base virtual void SetLocked( BOOL bLocked );
   virt_base virtual BOOL GetContextMenu( CMenu &menu );
   virt_base virtual UINT GetDefaultMenuItemID() { return -1; }
   virt_base virtual BOOL OnCommand( UINT nID ) { return FALSE; }
   virt_base virtual SnipeObjectType GetSOT() const { return SOT_SnipeObject; }
   virt_base virtual void GetSOTList( CList< SnipeObjectType > &list ) const { list.AddTail(GetThisSOT()); }
   virt_base virtual const char *GetClassName() const = 0;
   virt_base virtual String GetMatchName() const = 0;
   virt_base virtual SimpleComponentList *FindComponentList( const String &strComponentType ) { return NULL; }
   virt_base virtual void OnSelectedChanged() {}
   virt_base virtual void OnMouseOverChanged( BOOL bMouseOver ) {}
   virt_base virtual void OnFocusChanged( BOOL bFocus );
   virt_base virtual BOOL GetWorldPos( Vector &pos ) { return FALSE; }
   virt_base virtual void PreTransform() { m_bTransformed = FALSE; }
   virt_base virtual void Transform( const Matrix44 &matrix ) { m_bTransformed = TRUE; }
   virt_base virtual PropertyArray *GetPropertyArray() { return NULL; };
   virt_base virtual UINT GetIconIndex() { return ICONINDEX_UNKOWN; };
   virt_base virtual void DrawTreeItem( CGLControl *pItem, float x=0.0f );
   virt_base virtual void RebuildWorldMatrices() {}
   virt_base virtual void CountPickObjectType( CGLBaseView *pView, UINT &nCount );
   virt_base virtual int GetItemHeight() { return 16; }
   virt_base virtual CGLObjectTreeItem *AllocTreeItem( CGLTreeControl *pTreeControl );
   virt_base virtual void OnTreeItemCreated( CGLObjectTreeItem *pItem ) {}
   virt_base virtual CommandList *GetCommandList();
   virt_base virtual SelectionList *GetSelectionList() { return NULL; }
   virt_base virtual void Draw( CGLBaseView *pView ) {}
   virt_base virtual void DrawSelected( CGLBaseView *pView ) {}
   virt_base virtual void DrawMirrored( CGLBaseView *pView ) {}
   virt_base virtual void DrawFocused( CGLBaseView *pView ) {}
   virt_base virtual void DrawNearestFocused( CGLBaseView *pView, SnipeObject *pFocusObject ) {}
   virt_base virtual void DrawSchematic( CGLSchematicView *pView ) {}
   virt_base virtual float DrawUVExtras( CGLTexCoordView *pView ) { return 1.0f; }
   virt_base virtual SnipeObject *GetOwner() { return this; }
   virt_base virtual void FreeDeleteInfo( SnipeObject::DeleteInfo *pDeleteInfo ) { delete (DeleteInfo *)pDeleteInfo; }// DeleteInfo does not have a v-table, this must be here to get to the right destructor
   virt_base virtual BOOL Delete( DeleteInfo *&pDeleteInfo, String &strOutError ); // pass in the block of memory returned by AllocDeleteInfo
   virt_base virtual BOOL UnDelete( DeleteInfo *pDeleteInfo );
   virt_base virtual void DirtyDependentFaces() {}
   virt_base virtual void DirtyDependentEdges() {}
   virt_base virtual void MarkSaveDependents();
   virt_base virtual SnipeObject *FindMirrorObject() { return NULL; }
   virt_base virtual BOOL IsDrawMirrorColor() { return IsMirrorMode() && m_pMirror && m_pMirror->IsSelected(); }
   virt_base virtual BOOL IsDrawFocusColor() { return IsFocusObject(); }
   virt_base virtual BOOL IsDrawMouseOverColor() { return IsMouseOverObject(); }
   virt_base virtual BOOL IsDraw();
   virt_base virtual BOOL GetDrawColor( RGBAFloat &color );
   virt_base virtual void PushBounds( BoundingBox &box, CGLBaseView *pView=NULL );
   virt_base virtual Component *CreateComponent( SnipeObjectType nSOT ) { return NULL; }
   virt_base virtual catch_msg SnipeObject *CreateChild( const SnipeObjectInfo *pInfo );
   virt_base virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virt_base virtual catch_msg BOOL Load( BaseStream &bs );
   virt_base virtual catch_msg BOOL ParseData( BaseStream &bs, const DataHeader &dataheader ) { return FALSE; }
   virt_base virtual catch_msg void OnEndObjectLoad() {}
   virt_base virtual catch_msg void OnEndProjectLoad();
   virt_base virtual BOOL GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList ) { return TRUE; }
   virt_base virtual void SetWorldMatricesDirty() {}
   virt_base virtual const Matrix44 &GetObjectToWorldMatrix( const Time &time=GetTime() );
   virt_base virtual const Matrix44 &GetWorldToObjectMatrix( const Time &time=GetTime() );
   virt_base virtual void SetDrawColor(const RGBAFloat &color) {}
   virt_base virtual void Draw() {}
   virt_base virtual void OnBeginDrag( CPoint point, CGLObjectTreeItem *pDragTreeItem ) {}
   virt_base virtual void OnDoDrag( CPoint point, CGLObjectTreeItem *pDragTreeItem, CGLObjectTreeItem *pHitTreeItem ) {}
   virt_base virtual void OnEndDrag( CPoint point, CGLObjectTreeItem *pDragTreeItem, CGLObjectTreeItem *pHitTreeItem ) {}
   virt_base virtual Property *GetStatisticsProperty() { return NULL; }
   virt_base virtual BOOL IsVisibleInTree() { return m_bIsVisibleInTree; }

   // Selections
   virt_base virtual SelectionContainer *CreateSelectionContainer() { return NULL; }
   virt_base virtual SelectionContainer *GetSelectionContainer() { return NULL; }
   virt_base virtual void SetSelectionContainer( SelectionContainer *pSelectionContainer ) {}
};

class SnipeObjectNotifyNode : public NotifyNode
{
public:
   SnipeObject *m_pObject;

   SnipeObjectNotifyNode( SnipeObject *pObject, UINT msgs )
      : NotifyNode( msgs )
   {
      m_pObject = pObject;
   }

// NotifyNode overrides
   virtual SnipeObject *GetSnipeObject() { return m_pObject; }
   virtual void OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
   {
      m_pObject->OnObjectNotify( msg, wParam, lParam );
   }
};

inline void AssignWithReference( SnipeObject *&pThis, SnipeObject *pOther )
{
   if ( pThis == pOther )
      return;

   if (pThis)
      pThis->DeReference();

   pThis = pOther;

   if (pThis)
      pThis->Reference();
}

inline void SnipeObject::DeleteInfo::SetSnipeObject( SnipeObject *pSnipeObject ) 
{
   AssignWithReference( m_pSnipeObject, pSnipeObject ); 
}

inline void SnipeObject::DeleteInfo::UnDelete()
{
   ASSERT(GetSnipeObject());
   GetSnipeObject()->UnDelete( this );
}

inline void SnipeObject::DeleteInfo::Delete()
{
   ASSERT(GetSnipeObject());
   String strError;
   SnipeObject::DeleteInfo *pDeleteInfo = this;
   GetSnipeObject()->Delete( pDeleteInfo, strError );
}

class MirrorModePusher
{
public:
   BOOL m_bMirrorMode;
   MirrorModePusher( BOOL bMirrorMode )
   {
      m_bMirrorMode = SnipeObject::IsMirrorMode();
      SnipeObject::SetMirrorMode( bMirrorMode );
   }
   ~MirrorModePusher()
   {
      SnipeObject::SetMirrorMode(m_bMirrorMode);
   }
};

class SnipeObjectList : public CList< SnipeObject *>
{
public:
   SnipeObjectList() : CList<SnipeObject *>( 128 ) {}
   
   void ComputeBoundingBox( BoundingBox &box, CGLBaseView *pView=NULL );
   void ConvertCompatibleObjectsToSOT( SnipeObjectType nSOT );
};

#define DECLARE_SNIPEOBJECT_PURE(class_name, baseclass_name) \
   static SnipeObjectType GetThisSOT() { return SOT_##class_name; } \
   virtual SnipeObjectType GetSOT() const { return SOT_##class_name; } \
   virtual void GetSOTList( CList< SnipeObjectType > &list ) const { baseclass_name::GetSOTList(list); list.AddTail(GetThisSOT()); } \
   static const char *GetThisClassName() { return #class_name; } \
   virtual const char *GetClassName() const { return #class_name; } \
   virtual CommandUIPlugin *GetCommandUIPlugin(UINT nID) { \
   CommandUIPluginList *pList = GetCommandUIPluginList(GetThisSOT()); \
   for (POSITION pos = pList->GetHeadPosition(); pos; ) { \
   CommandUIPlugin *pPlugin = pList->GetNext(pos); \
   if (pPlugin->GetCmdID() == nID) \
   return pPlugin; \
   } \
   return baseclass_name::GetCommandUIPlugin(nID); \
   } \
   virtual const SnipeObject *IsKindOf( SnipeObjectType nType ) const \
{ \
   if (nType == SOT_##class_name) \
   return this; \
   return baseclass_name::IsKindOf( nType ); \
} \
   static SnipeObjectInfo m_snipeobjectinfo; \
   static SnipeObjectInfo *GetThisSnipeObjectInfo() { return &m_snipeobjectinfo; } \
   virtual SnipeObjectInfo *GetSnipeObjectInfo() { return &m_snipeobjectinfo; } \
   virtual size_t GetSizeOf() { return sizeof(class_name); }

#define DECLARE_COMPONENT_PURE(class_name, baseclass_name) \
   DECLARE_SNIPEOBJECT_PURE(class_name, baseclass_name)

#define DECLARE_SNIPEOBJECT_CREATE(class_name, baseclass_name) \
   DECLARE_SNIPEOBJECT_PURE(class_name, baseclass_name) \
   static class_name *Create() { class_name *pObject = SNEW class_name; pObject->PostConstruct(); return pObject; } \
   static class_name *CreateArray(UINT nCount) { \
         class_name *pObject = SNEW class_name[nCount]; \
         for (UINT i = 0; i < nCount; i++) \
            pObject[i].PostConstruct(); \
         return pObject; \
      }

#define DECLARE_COMPONENT_CREATE(class_name, baseclass_name ) \
   DECLARE_SNIPEOBJECT_CREATE(class_name, baseclass_name) \
   static class_name *CreateWithID( UINT nID ) { class_name *pObject = SNEW class_name( nID ); pObject->PostConstruct(); return pObject; } \
   virtual const char *GetCommandLineName() const;

#define IMPLEMENT_SNIPEOBJECT_CREATE( class_name ) \
   SnipeObjectInfo class_name::m_snipeobjectinfo( SOT_##class_name, #class_name, #class_name, (PFN_CREATESNIPEOBJECT)class_name::Create, (PFN_CREATESNIPEOBJECTARRAY)class_name::CreateArray, class_name::GetNumPickElements(), FALSE );

#define IMPLEMENT_SNIPEOBJECT_PURE( class_name ) \
   SnipeObjectInfo class_name::m_snipeobjectinfo( SOT_##class_name, #class_name, #class_name, NULL, NULL, class_name::GetNumPickElements(), FALSE );

#define IMPLEMENT_COMPONENT_CREATE( class_name, match_name ) \
   SnipeObjectInfo class_name::m_snipeobjectinfo( SOT_##class_name, #class_name, #match_name, (PFN_CREATESNIPEOBJECT)class_name::Create, (PFN_CREATESNIPEOBJECTARRAY)class_name::CreateArray, class_name::GetNumPickElements(), TRUE ); \
   const char *class_name::GetCommandLineName() const { return #match_name; }

#define IMPLEMENT_COMPONENT_PURE( class_name  ) \
   SnipeObjectInfo class_name::m_snipeobjectinfo( SOT_##class_name, #class_name, #class_name, NULL, NULL, class_name::GetNumPickElements(), TRUE );

typedef void (AFX_MSG_CALL SnipeObject::*SNIPEOBJECT_AFX_PMSG)(void);

#define SNIPEOBJECT_ON_COMMAND_EX_RANGE(id, idLast, memberFxn) \
{ WM_COMMAND, CN_COMMAND, (WORD)id, (WORD)idLast, AfxSigCmd_EX, \
   (AFX_PMSG) \
   (static_cast< BOOL (AFX_MSG_CALL SnipeObject::*)(UINT) > \
   (memberFxn)) },

#define SNIPEOBJECT_ON_UPDATE_COMMAND_UI_RANGE(id, idLast, memberFxn) \
{ WM_COMMAND, CN_UPDATE_COMMAND_UI, (WORD)id, (WORD)idLast, AfxSigCmdUI, \
   (AFX_PMSG) \
   (static_cast< void (AFX_MSG_CALL SnipeObject::*)(CCmdUI*) > \
   (memberFxn)) },

#define SNIPEOBJECT_ON_COMMAND(id, memberFxn) \
{ WM_COMMAND, CN_COMMAND, (WORD)id, (WORD)id, AfxSigCmd_v, \
   (AFX_PMSG) \
   (static_cast<SNIPEOBJECT_AFX_PMSG> (memberFxn)) },
// ON_COMMAND(id, OnBar) is the same as
//   ON_CONTROL(0, id, OnBar) or ON_BN_CLICKED(0, id, OnBar)

#define SNIPEOBJECT_ON_UPDATE_COMMAND_UI(id, memberFxn) \
{ WM_COMMAND, CN_UPDATE_COMMAND_UI, (WORD)id, (WORD)id, AfxSigCmdUI, \
   (AFX_PMSG) \
   (static_cast< void (AFX_MSG_CALL SnipeObject::*)(CCmdUI*) > \
   (memberFxn)) },

///////////////////////////////////////////////////////////////////////

class HeaderBase
{
public:
   BaseStream *m_pBS;          // OUT
   BOOL        m_bWriteHeader; // OUT
   UINT        m_nFilePos;     // OUT
   String      m_strKey;       // IN
   BOOL        m_bBeginHeader; // IN - true when the header is a beginning one [FOO] or [FOO/]
   BOOL        m_bEndHeader;   // IN - true when the header is an ending one  [/FOO] or [FOO/]

   HeaderBase()
   {
      m_pBS = NULL;
      m_bWriteHeader = FALSE;
      m_nFilePos = 0L;
      m_bBeginHeader = FALSE;
      m_bEndHeader = FALSE;
   }
   HeaderBase( BOOL bWriteHeader, BaseStream &bs )
   {
      m_pBS = &bs;
      m_bWriteHeader = bWriteHeader;
   }
};

class SnipeObjectHeaderBase : public HeaderBase
{
public:
   SnipeObject *m_pObject;

   SnipeObjectHeaderBase()
   {
      m_pObject = NULL;
   }
   SnipeObjectHeaderBase( SnipeObject *pObject, BOOL bWriteHeader, BaseStream &bs ) :
    HeaderBase( bWriteHeader, bs )
   {
      m_pObject = pObject;
   }
};

class SnipeObjectHeader : public SnipeObjectHeaderBase
{
public:
    SnipeObjectHeader() {}
    SnipeObjectHeader( SnipeObject *pObject, BOOL bWriteHeader, BaseStream &bs );
   ~SnipeObjectHeader();

   static BOOL IsHeader( MemoryStream &ms );
};

class DataHeader : public HeaderBase
{
public:
   String m_strValue;

    DataHeader() {}
    DataHeader( const String &strKey, const String &strValue ); // i.e. {KEY=VALUE/}
    DataHeader( BaseStream &bs, const String &strKey ); // i.e. {KEY} ... {/KEY}
   ~DataHeader();

   static BOOL IsHeader( MemoryStream &ms );
};

BaseStream &operator << (BaseStream &bs, const DataHeader &value);
BaseStream &operator >> (BaseStream &bs, DataHeader &value);
BaseStream &operator >> (BaseStream &bs, SnipeObjectHeader &value);
BaseStream &operator << (BaseStream &bs, const SnipeObject *&value );
BaseStream &operator >> (BaseStream &bs, SnipeObject *&value );
