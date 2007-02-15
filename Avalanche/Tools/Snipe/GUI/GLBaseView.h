#pragma once

#include "GLControls.h"
#include "..\Objects\TransformableObject.h"
#include "..\Interface\GLHitArray.h"
#include "Enum.h"

class CGLBaseFrame;
class CGLBaseDocFrame;
class CBaseDoc;
class CGLRenderWnd;
class SnipeObjectTypeArray;
class PickHit;
class PickHitArray;
class SnipeNavigatorInstance;
class SnipeNavigator;

#define DECLARE_BASE_BASEVIEW(class_name, baseclass_name) \
   DECLARE_DYNCREATE(class_name) \
   DECLARE_MESSAGE_MAP() \
   public:\
   static SnipeViewType GetThisSVT() { return SVT_##class_name; } \
   virtual SnipeViewType GetSVT() const { return SVT_##class_name; } \
   static const char *GetThisClassName() { return #class_name; } \
   virtual const char *GetClassName() const { return #class_name; } \

#define DECLARE_BASEVIEW(class_name, baseclass_name) \
   DECLARE_BASE_BASEVIEW(class_name, baseclass_name) \
   virtual String GetMatchName() const { return GetClassName(); } \
   virtual CommandUIPlugin *GetCommandUIPlugin(UINT nID) { \
      CommandUIPluginList *pList = GetCommandUIPluginList(GetThisSVT()); \
      for (POSITION pos = pList->GetHeadPosition(); pos; ) { \
         CommandUIPlugin *pPlugin = pList->GetNext(pos); \
         if (pPlugin->GetCmdID() == nID) \
            return pPlugin; \
      } \
      return baseclass_name::GetCommandUIPlugin(nID); \
   } \
   virtual const CGLBaseView *IsKindOf( SnipeViewType nType ) const \
   { \
      if (nType == GetThisSVT()) \
         return this; \
      return baseclass_name::IsKindOf( nType ); \
   } 

class CGLViewControl;

class CGLBaseView : public CGLControl, public ParentableObject // (for properties)
{
   DECLARE_BASE_BASEVIEW(CGLBaseView, CGLControl)
public:
   enum Operation { OP_IDLE, OP_NAVIGATING, OP_MANIPULATING, OP_VIEWCONTROLING };

   static Property::Info   m_categoryDisplayInfo;
      static BoolProperty::Info m_bDisplayFrameRateInfo;

   Property                m_categoryDisplay;
      BoolProperty            m_bDisplayFrameRate;

protected:
   CBaseDoc               *m_pDocument;
   CGLBaseFrame           *m_pBaseFrame;
   LARGE_INTEGER           m_starttime;
   Operation               m_nCurrentOperation;
   SnipeNavigatorInstance *m_pNavigatorInstance;
   SnipeObjectList        *m_pDoPickExcludeList;
   CList<CGLViewControl *> m_ViewControlList;
   CGLViewControl         *m_pMouseOverViewControl;
   CGLViewControl         *m_pCurrentViewControl;
   PickHit                 m_mouseOverViewControlPick;
   PickHit                 m_currentViewControlPick;

public:
   static BOOL             m_bSinglePick;
   enum { SHADOWSIZE=4 };

   SnipeObjectType         m_nPickObjectType; // temporarily used during selection
   union {
      ULONG                m_nBaseViewFlags;
      struct {
         ULONG             m_bDoPick             : 1;
      };
   };

            CGLBaseView();
   virtual ~CGLBaseView();

   CommandUIPluginList *GetCommandUIPluginList(SnipeViewType nType);

   SnipeNavigatorInstance *GetNavigatorInstance() const { return m_pNavigatorInstance; } 

   SnipeObjectList *GetDoPickExcludeList() const { return m_pDoPickExcludeList; }

// Operations
   BOOL StartTimer();
   void EndTimer();

   Operation GetOperation() const { return m_nCurrentOperation; }
   BOOL SetOperation( Operation nOperation );

   CBaseDoc *GetDocument() const { return m_pDocument; }
   CGLBaseFrame *GetParentFrame() const { return m_pBaseFrame; }
   void SetParentFrame( CGLBaseFrame *pParentFrame) { m_pBaseFrame = pParentFrame; }

   CGLBaseDocFrame *FindDocFrame();

   int GetNumViewControls() { return m_ViewControlList.GetCount(); }
   void AddViewControl( CGLViewControl *pViewControl ) { m_ViewControlList.AddTail( pViewControl ); }
   void RemoveViewControl( CGLViewControl *pViewControl );
   CGLViewControl *HitTestViewControls( const CPoint &point, PickHit &pickHit );
   CList<CGLViewControl *> &GetViewControlList() { return m_ViewControlList; }

   void CountPickObjectType( UINT &nCount );

   SnipeObjectType GetCurrentSelectionSOT() const;
   BOOL SetCurrentSelectionSOT( SnipeObjectType nSOT );
   BOOL SetCurrentSelectionIndex( UINT nIndex );
   UINT GetNumSelectionSOTs() const;
   SnipeObjectType GetSelectionSOTAt( UINT nIndex ) const;

   SnipeManipulatorInstance *GetManipulatorInstance() const;
   void SetManipulatorInstance( SnipeManipulator *pManipulator );
   SnipeManipulatorList *GetAvailableManipulatorList();

   void HitTestManipulator( CPoint point, PickHit &pickHit ) const;
   void RefreshStatusBarInstructions(BOOL bForce=FALSE);

   void ZoomFitAll();
   void ZoomFitSelected();
   void DrawSelectedObjects();
   void DrawViewControls();

// Message Map
   afx_msg void OnViewZoomFitSelected();
   afx_msg void OnViewZoomFitAll();
   afx_msg void OnUpdateSplit( CCmdUI *pCmdUI );
   afx_msg void OnSplitBoth();
   afx_msg void OnSplitHorizontally();
   afx_msg void OnSplitVertically();
   afx_msg void OnUpdatePluginCommand( CCmdUI *pCmdUI );
   afx_msg BOOL OnPluginCommand( UINT nID );
   afx_msg void OnComponentComboChanged();
   afx_msg void OnUpdateComponentCombo(CCmdUI *pCmdUI);
   afx_msg void OnManipulatorComboChanged();
   afx_msg void OnUpdateManipulatorCombo(CCmdUI *pCmdUI);
   afx_msg void OnUpdateOutputControl( CCmdUI *pCmnUI );
   afx_msg void OnUpdateToggleMaximize( CCmdUI *pCmdUI );
   afx_msg void OnToggleMaximize();

// Overridables
   virt_base virtual void DoPick( const CRect &rcPick, PickHitArray &pickHitArray, BOOL *bOutSinglePick=NULL, SnipeObjectType kindof=SOT_Unknown, SnipeObjectList *pExcludeList=NULL ); // Use SOT_Known for Current Selection Type
   virt_base virtual float GetScreenPixelSize( float fDistance ) const { return 1.0f; }
   virt_base virtual void ViewToWorld( Vector &p, BOOL bRotateOnly=FALSE ) const {}
   virt_base virtual void WorldToView( Vector &p ) const {}
   virt_base virtual void ScreenToWorld( Vector &p, BOOL bRotateOnly=FALSE ) const {}
   virt_base virtual void WorldToScreen( Vector &p ) const {}
   virt_base virtual BOOL Project( Vector &p ) const { return FALSE; }
   virt_base virtual BOOL UnProject( Vector &p ) const { return FALSE; }
   virt_base virtual const Matrix44 &GetWorldToViewMatrix() const { return Identity44(); }
   virt_base virtual const Matrix44 &GetViewToWorldMatrix() const { return Identity44(); }
   virt_base virtual void DefaultZoomFit(SnipeObjectList *pList) {}; // Called by ZoomFitAll and ZoomFitSelected, use those functions instead
   virt_base virtual void DrawContents();
   virt_base virtual SnipeObjectTypeArray *GetSelectableObjectTypeArray() const;
   virt_base virtual CommandUIPlugin *GetCommandUIPlugin(UINT nID);
   virt_base virtual const CGLBaseView *IsKindOf( SnipeViewType nType ) const;
   virt_base virtual void ConvertSelectionToCurrentSelectionType( SnipeObjectType nSOT ) {}
   virt_base virtual SnipeNavigatorInstance *SetNavigatorInstance( SnipeNavigator *pNavigator );
   virt_base virtual BOOL OnSnipeIdle() { return FALSE; }
   virt_base virtual SnipeObject *GetObjectHead();

// GLControl overrides
   virtual void PostInit();
   virtual BOOL GetContextMenu( const CPoint &ptControl, CMenu &menu );
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual BOOL OnSetCursor( UINT message, CPoint point );
   virtual UINT GetToolTipText( CPoint point, String &strText ) const;
   virtual void OnActivate( ActivationMsg msg, CGLControl *pOther );
   virtual void OnMouseEnter();
   virtual void OnMouseLeave();

   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnLButtonDblClk( UINT nFlags, CPoint point );
   virtual BOOL OnLButtonUp( UINT nFlags, CPoint point );
   virtual BOOL OnMButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnMButtonDblClk( UINT nFlags, CPoint point );
   virtual BOOL OnMButtonUp( UINT nFlags, CPoint point );
   virtual BOOL OnRButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnRButtonDblClk( UINT nFlags, CPoint point );
   virtual BOOL OnRButtonUp( UINT nFlags, CPoint point );
   virtual BOOL OnMouseMove( UINT nFlags, CPoint point );
   virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

   virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   virtual BOOL OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

// CCmdTarget overrides
   virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// SnipeObject overrides
   virtual String GetTitle() const;
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};
