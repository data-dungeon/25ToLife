#pragma once

#include "HManipulatorPluginHelper.h"
#include "Vector2.h"
#include "HBoolProperty.h"
#include "HTypeProperty.h"
#include "HPropertyArray.h"

class HCommandPlugin;
class HPickHitArray;

// Plugin
class SelectManipulatorPlugin : public HManipulatorPluginHelper
{
protected:
   HPropertyArray *m_pPropertyArray;

public:
   enum SelectionMethod { SM_SINGLE, SM_BOUND, SM_LASSO, SM_POLYLASSO, SM_PAINT, SM_NUMMETHODS };
   static HBoolProperty::HInfo *m_pMouseOverInfo;
   static HBoolProperty *m_pMouseOver;

   static HBoolProperty::HInfo *m_pRealTimeInfo;
   static HBoolProperty *m_pRealTime;

   static HBoolProperty::HInfo *m_pSelectObscuredInfo;
   static HBoolProperty *m_pSelectObscured;

   static HTypeProperty::HInfo::HNode *m_pMethodNodes;
   static HTypeProperty::HInfo *m_pMethodInfo;
   static HTypeProperty *m_pMethod;

   DECLARE_MANIPULATOR_PLUGINHELPER( SelectManipulatorPlugin, HManipulatorPluginHelper )
   HCommandPlugin *m_pSelectCommandPlugin;

            SelectManipulatorPlugin();
   virtual ~SelectManipulatorPlugin();

// HManipulatorPluginHelper overrides
   virtual catch_msg BOOL SupportSOT(SnipeObjectType nType);
   virtual catch_msg ManipulatorInstance *CreateInstance();

// HCommandUIPluginHelper Overrides
   virtual const char *GetTitleName() const;
   virtual UINT GetBitmapID() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual catch_msg void Init(HCommandPluginManager *pCommandManager);
   virtual catch_msg HPropertyArray *GetPropertyArray() const;
   virtual catch_msg BOOL OnObjectNotify(ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam);
};

// Instance
class SelectManipulator : public ManipulatorInstance
{
protected:
   KeyModifiers m_keymodifier;

   //Properties
   HPropertyArray *m_pPropertyArray;

public:
   DECLARE_MANIPULATORINSTANCE( SelectManipulator, ManipulatorInstance )

   HProperty::HInfo *m_pOptionsInfo;
   HProperty *m_pOptions;

   HCGLBaseView *m_pMarqueeView;
   CPoint m_origpoint;

   //Bound
	CRect m_rcMarquee;

   //Lasso 
   CPoint m_prevpoint;

   //PolyLasso
   enum { TOLERANCE=4 };
   
   //Lasso && PolyLasso
   CList<Vector2> m_pointlist;

            SelectManipulator( SelectManipulatorPlugin *pPluginHelper);
   virtual ~SelectManipulator(); 

   SelectManipulatorPlugin *GetSelectManipulatorPlugin() const { return (SelectManipulatorPlugin *)GetManipulatorPluginHelper(); }

   void Reset();
   BOOL BuildParameterList( HSelectionList *pSelectList, String &strOutParameters );// If Failed, strOutParameters has reason

   void Single_PostDraw(HCGLBaseView *pView);
   BOOL Single_OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   BOOL Single_OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   BOOL Single_OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   BOOL Single_OnApply( HCGLBaseView *pView );
   BOOL Single_DoSelect( HCGLBaseView *pView, BOOL bTemp, BOOL *bSingleSelect = NULL );

   void Bound_PostDraw(HCGLBaseView *pView);
   BOOL Bound_OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   BOOL Bound_OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   BOOL Bound_OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   BOOL Bound_OnApply( HCGLBaseView *pView );
   BOOL Bound_DoSelect( HCGLBaseView *pView, BOOL bTemp );

   void Lasso_PostDraw(HCGLBaseView *pView);
   BOOL Lasso_OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   BOOL Lasso_OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   BOOL Lasso_OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   BOOL Lasso_OnApply( HCGLBaseView *pView );

   void PolyLasso_PostDraw(HCGLBaseView *pView);
   BOOL PolyLasso_OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   BOOL PolyLasso_OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   BOOL PolyLasso_OnApply( HCGLBaseView *pView );

   void Paint_PostDraw(HCGLBaseView *pView);
   BOOL Paint_OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   BOOL Paint_OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   BOOL Paint_OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   BOOL Paint_OnApply( HCGLBaseView *pView );

   // Overridables
   virt_base virtual void OnSingleSelect( CPoint point, HCGLBaseView *pView ) {} // Notify that the an OnSelect Occur and bSinglePick was True from both DoPick and OnSelect
   virt_base virtual void DoPick( HCGLBaseView *pView, const CRect &rcRect, HPickHitArray *pPickHitArray, BOOL *bOutSinglePick=NULL );
   virt_base virtual BOOL OnSelect( HCGLBaseView *pView, HSnipeObjectList *pSelectList, BOOL bTemp, BOOL *bSingleSelect=NULL, HSnipeObject *pFocus=NULL ); // User selected something, no the command must be put on the stack

// ManipulatorInstance overrides
   virtual catch_msg const char *GetStatusbarInstructions( KeyModifiers keymodifier, MouseStates mousestate, HCGLBaseView *pView ) const;
   virtual catch_msg KeyModifiers GetKeyModifiers( HCGLBaseView *pView, MouseStates mousestate ) const;
   virtual catch_msg BOOL OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );

   virtual catch_msg HPropertyArray *GetPropertyArray() const;

   virtual catch_msg void PostDraw(HCGLBaseView *pView);
   virtual catch_msg BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnLButtonDblClk( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );

   virtual catch_msg BOOL OnApply( HCGLBaseView *pView );
   virtual catch_msg BOOL OnCancel( HCGLBaseView *pView );
};
