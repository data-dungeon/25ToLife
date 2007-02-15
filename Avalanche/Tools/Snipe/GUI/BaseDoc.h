#pragma once

#include "SnipeObject.h"
#include "Interface/CommandList.h"
#include "Interface/SnipeManipulator.h"
#include "enum.h"
#include "Interface/SnipeManipulator.h"
#include "CommandUIPluginManager.h"
#include "Interface/SelectionList.h"
#include "Interface/SnipeManipulatorList.h"
#include "CommandUIPlugin.h"

class CGLControl;
class HierObject;
class CSnipeDocTemplate;
class CGLBaseView;
class CBaseDoc;
class HierObject;
class ParentableObject;

typedef CTypedPtrList<CObList, CGLBaseView *> CGLBaseViewList;

#define DECLARE_BASEDOCCOMMON(class_name, baseclass_name) \
   DECLARE_DYNCREATE(class_name) \
   DECLARE_MESSAGE_MAP() \
   public:\
   static SnipeDocType GetThisSOT() { return SDT_##class_name; } \
   virtual SnipeDocType GetSOT() { return SDT_##class_name; } \
   static const char *GetThisClassName() { return #class_name; } \
   virtual const char *GetClassName() { return #class_name; } \

#define DECLARE_BASEDOCROOT(class_name, baseclass_name) \
   DECLARE_BASEDOCCOMMON(class_name, baseclass_name) \
   virtual CommandUIPlugin *GetCommandUIPlugin(UINT nID) { \
      CommandUIPluginList *pList = GetCommandUIPluginList(GetThisSOT()); \
      for (POSITION pos = pList->GetHeadPosition(); pos; ) { \
         CommandUIPlugin *pPlugin = pList->GetNext(pos); \
         if (pPlugin->GetCmdID() == nID) \
            return pPlugin; \
      } \
      return NULL; \
   }

#define DECLARE_BASEDOC(class_name, baseclass_name) \
   DECLARE_BASEDOCCOMMON(class_name, baseclass_name) \
   virtual CommandUIPlugin *GetCommandUIPlugin(UINT nID) { \
      CommandUIPluginList *pList = GetCommandUIPluginList(GetThisSOT()); \
      for (POSITION pos = pList->GetHeadPosition(); pos; ) { \
         CommandUIPlugin *pPlugin = pList->GetNext(pos); \
         if (pPlugin->GetCmdID() == nID) \
            return pPlugin; \
      } \
      return baseclass_name::GetCommandUIPlugin(nID); \
   }

class SnipeObjectInfo;
class SnipeObjectTypeArray
{
private:
   CArray<SnipeObjectInfo *> m_infoarray;
   UINT                    m_nCurrentIndex;
   SnipeManipulatorList    m_AvailableManipulatorList; // given the current index
   

public:
   SnipeObjectTypeArray()
   {
      m_nCurrentIndex = -1;
   }
   int Add( SnipeObjectType nSOT );
   UINT GetNumInfos() const { return m_infoarray.GetCount(); }
   SnipeObjectInfo *GetInfoAt( UINT nIndex ) const
   {
      if (nIndex==-1 || m_infoarray.GetCount()==0)
         return NULL;
      else
         return m_infoarray[nIndex];
   }
   UINT GetCurrentIndex() const { return m_nCurrentIndex; }
   void SetCurrentIndex( UINT nIndex );
   SnipeObjectType GetCurrentSOT() const;
   
   SnipeObjectType GetSnipeObjectTypeAt( UINT nIndex ) const;
   BOOL SetCurrentSOT( SnipeObjectType nSOT );

   SnipeManipulatorInstance *GetManipulatorInstance()
   {
      return m_AvailableManipulatorList.GetCurrentManipulatorInstance();
   }

   SnipeManipulatorInstance *SetManipulator( SnipeManipulator *pManipulator )
   {
      return m_AvailableManipulatorList.SetCurrentManipulatorInstance(pManipulator);
   }

   SnipeManipulatorList *GetAvailableManipulatorList()
   {
      return &m_AvailableManipulatorList;
   }

   void RebuildAvailableManipulatorList( SnipeObjectType sot );
};

class TimeProperty;

class CBaseDoc : public CDocument
{
protected: // create from serialization only
	DECLARE_BASEDOCROOT(CBaseDoc, CDocument)

// Attributes
public:
   CGLControl *m_pControlHead;
   HierObject *m_pObjectHead;
   CGLBaseViewList m_viewlist;
   SnipeObjectTypeArray m_SelectableObjectTypeArray;
   union {
      ULONG    m_basedocflags;
      struct {
         ULONG m_bUnused : 1;
         };
      };

         	CBaseDoc();
	virtual ~CBaseDoc();

// Operations
   CommandUIPluginList *GetCommandUIPluginList(SnipeDocType nType);
   void AddView( CGLBaseView *pView );
   void RemoveView( CGLBaseView *pView );
   void InvalidateViews( SnipeObject *pObject );
   CSnipeDocTemplate *GetDocTemplate() const { return (CSnipeDocTemplate *)CDocument::GetDocTemplate(); }
   void ZoomFitAll();
   SelectionList *GetSelectionList() const;

   BOOL DoCommand( CommandPlugin *pCommand, const String &strParameters, BOOL bTempCommand = FALSE );

   HierObject *GetObjectHead() { return m_pObjectHead; }

   // Message Maps
   afx_msg void OnUpdatePluginCommand( CCmdUI *pCmdUI );
   afx_msg BOOL OnPluginCommand( UINT nID );
   afx_msg void OnUpdateOutputControl(CCmdUI *pCmdUI);
   afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);

// Overridables
   virt_base virtual SnipeDocType GetDocType() { return SDT_CBaseDoc; }
   virt_base virtual void SetObjectHead( HierObject *pObjectHead );
   virt_base virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam ) { return FALSE; }
   virt_base virtual HierObject *GetCommandObject() const;
   virt_base virtual BOOL UsesObject( SnipeObject *pObject );
   virt_base virtual TimeProperty *GetPlaybackStartProperty() { return NULL; }
   virt_base virtual TimeProperty *GetPlaybackEndProperty() { return NULL; }
   virt_base virtual BOOL OnSnipeIdle() { return FALSE; }

// CDocument overrides
   virtual void OnCloseDocument();
};

