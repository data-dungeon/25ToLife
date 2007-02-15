#include "stdafx.h"
#include "SnipeManipulator.h"
#include "ManipulatorPlugin.h"
#include "SnipePlugin.h"
#include "GUI/GLBaseView.h"
#include "GUI/BaseDoc.h"
#include "GUI/MainFrm.h"

//SnipeManipulatorList
SnipeManipulator *SnipeManipulatorList::Find( UINT nID ) const
{
   for (POSITION pos = GetHeadPosition(); pos; ) {
      SnipeManipulator *pManipulator = GetNext( pos );
      if (pManipulator->m_pPlugin->GetCmdID() == nID)
         return pManipulator;
   }
   return NULL;
}

//SnipeManipulator
SnipeManipulator::SnipeManipulator(ManipulatorPlugin *pManipulatorPlugin) :
 m_pPlugin(pManipulatorPlugin)
{
   ASSERT(pManipulatorPlugin);
}

SnipeManipulator::~SnipeManipulator()
{
}

// Callbacks
SnipeManipulatorInstance *SnipeManipulator::CreateInstance()
{
   try {
      if (m_pPlugin->GetCreateInstanceCallback()) {
         void *pPluginInstanceData = m_pPlugin->GetCreateInstanceCallback()( m_pPlugin->GetPluginData() );
         if (!pPluginInstanceData)
            return NULL;
         return SNEW SnipeManipulatorInstance( this, pPluginInstanceData );
      }
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("CreateInstance", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("CreateInstance", "Unknown") << "\n";
   }
   return NULL;
}

BOOL SnipeManipulator::SupportSOT( SnipeObjectType nType )
{
   try {
      if (m_pPlugin->GetSupportSOTCallback())
         return m_pPlugin->GetSupportSOTCallback()(m_pPlugin->GetPluginData(), nType);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("SupportSOT", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("SupportSOT", "Unknown") << "\n";
   }
   return FALSE;
}

BOOL SnipeManipulator::IsComboInvalid()
{
   return m_pPlugin->CreateIcon();
}

///////////////////////////////////////////////////////////////////////
// Instance
///////////////////////////////////////////////////////////////////////
SnipeManipulatorInstance::SnipeManipulatorInstance(SnipeManipulator *pSnipeManipulator, void *pPluginInstanceData)
{
   m_pSnipeManipulator = pSnipeManipulator;
   m_pPlugin = pSnipeManipulator->m_pPlugin;
   m_pPluginInstanceData = pPluginInstanceData;
   SetPropertyArray(GetPropertyArrayCallback());
}

void SnipeManipulatorInstance::DrawTreeItem( CGLControl *pItem, float x/*=0.0f*/ )
{
   m_pSnipeManipulator->m_pPlugin->GetImageList()->Draw( x, 0.0f );

   glColor3f( 0.1f, 0.1f, 0.1f );
   pItem->DrawText( x + m_pSnipeManipulator->m_pPlugin->GetImageList()->GetWidthEach() + 2, 12, GetTitle());
}

const char *SnipeManipulatorInstance::GetClassName() const { return m_pPlugin->CommandUI::GetClassName(); }
String SnipeManipulatorInstance::GetMatchName() const { return m_pPlugin->GetMatchName(); }
String SnipeManipulatorInstance::GetTitle() const { return m_pPlugin->GetTitle(); }

void SnipeManipulatorInstance::SetPropertyArray( PropertyArray *pPropertyArray )
{
   m_pPropertyArray = pPropertyArray;

   if (pPropertyArray)
   {
      for (int i=0; i<pPropertyArray->GetCount(); i++)
      {
         Property *pProperty = pPropertyArray->GetAt( i );
         if (pProperty && pProperty->m_pParent==NULL) // this allows the property to be both in the Manipulator (Button) and ManipulatorInstance (Manipulator Properties), but the Manipulator will get all Notifications, not the instance
            pProperty->m_pParent = this;
      }
   }
}

// Callbacks
void SnipeManipulatorInstance::DestroyCallback()
{
   try {
      if (m_pPlugin->GetDestroyInstanceCallback())
         m_pPlugin->GetDestroyInstanceCallback()(m_pPluginInstanceData);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("DestroyInstance", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("DestroyInstance", "Unknown") << "\n";
   }
}

PropertyArray *SnipeManipulatorInstance::GetPropertyArrayCallback()
{
   try {
      if (m_pPlugin->GetGetPropertyArrayInstanceCallback())
         return (PropertyArray *)m_pPlugin->GetGetPropertyArrayInstanceCallback()(m_pPluginInstanceData);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("GetPropertyArrayInstance", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("GetPropertyArrayInstance", "Unknown") << "\n";
   }

   return NULL;
}

BOOL SnipeManipulatorInstance::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   try {
      if (m_pPlugin->GetOnObjectNotifyInstanceCallback())
         m_pPlugin->GetOnObjectNotifyInstanceCallback()(m_pPluginInstanceData, msg, wParam, lParam);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("OnObjectNotifyInstance", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("OnObjectNotifyInstance", "Unknown") << "\n";
   }

   return SnipeObject::OnObjectNotify( msg, wParam, lParam );
}

void SnipeManipulatorInstance::DrawPick(const CGLBaseView *pView)
{
   try {
      if (m_pPlugin->GetDrawPickCallback())
         m_pPlugin->GetDrawPickCallback()(m_pPluginInstanceData, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("DrawPick", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("DrawPick", "Unknown") << "\n";
   }
}

void SnipeManipulatorInstance::PreDraw(CGLBaseView *pView)
{
   try {
      if (m_pPlugin->GetPreDrawCallback())
         m_pPlugin->GetPreDrawCallback()(m_pPluginInstanceData, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("PreDraw", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("PreDraw", "Unknown") << "\n";
   }
}

void SnipeManipulatorInstance::PostDraw(CGLBaseView *pView)
{
   try {
      if (m_pPlugin->GetPostDrawCallback())
         m_pPlugin->GetPostDrawCallback()(m_pPluginInstanceData, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("PostDraw", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("PostDraw", "Unknown") << "\n";
   }
}

const char *SnipeManipulatorInstance::GetStatusbarInstructions( KeyModifiers keymodifier, MouseStates mousestate, CGLBaseView *pView )
{
   try {
      if (m_pPlugin->GetGetStatusbarInstructionsCallback())
         return m_pPlugin->GetGetStatusbarInstructionsCallback()( m_pPluginInstanceData, keymodifier, mousestate, pView );
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("GetStatusbarInstructions", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("GetStatusbarInstructions", "Unknown") << "\n";
   }
   return NULL;
}

KeyModifiers SnipeManipulatorInstance::GetKeyModifiers( CGLBaseView *pView, MouseStates mousestate )
{
   try {
      if (m_pPlugin->GetGetKeyModifiersCallback())
         return m_pPlugin->GetGetKeyModifiersCallback()( m_pPluginInstanceData, pView, mousestate );
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("GetKeyModifiers", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("GetKeyModifiers", "Unknown") << "\n";
   }
   return 0;
}


BOOL SnipeManipulatorInstance::OnLButtonDown(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView)
{
   try {             
      if (m_pPlugin->GetOnLButtonDownCallback())
          return m_pPlugin->GetOnLButtonDownCallback()(m_pPluginInstanceData, keymodifier, mousestate, point, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("OnLButtonDown", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("OnLButtonDown", "Unknown") << "\n";
   }
   return FALSE;
}

BOOL SnipeManipulatorInstance::OnLButtonDblClk(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView)
{
   try {             
      if (m_pPlugin->GetOnLButtonDblClkCallback())
          return m_pPlugin->GetOnLButtonDblClkCallback()(m_pPluginInstanceData, keymodifier, mousestate, point, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("OnLButtonDblClk", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("OnLButtonDblClk", "Unknown") << "\n";
   }
   return FALSE;
}

BOOL SnipeManipulatorInstance::OnLButtonUp(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView)
{
   try {
      if (m_pPlugin->GetOnLButtonUpCallback())
         return m_pPlugin->GetOnLButtonUpCallback()(m_pPluginInstanceData, keymodifier, mousestate, point, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("OnLButtonUp", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("OnLButtonUp", "Unknown") << "\n";
   }
   return FALSE;
}

BOOL SnipeManipulatorInstance::OnRButtonDown(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView)
{
   try {
      if (m_pPlugin->GetOnRButtonDownCallback())
         return m_pPlugin->GetOnRButtonDownCallback()(m_pPluginInstanceData, keymodifier, mousestate, point, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("OnRButtonDown", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("OnRButtonDown", "Unknown") << "\n";
   }
   return FALSE;
}

BOOL SnipeManipulatorInstance::OnRButtonDblClk(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView)
{
   try {             
      if (m_pPlugin->GetOnRButtonDblClkCallback())
          return m_pPlugin->GetOnRButtonDblClkCallback()(m_pPluginInstanceData, keymodifier, mousestate, point, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("OnRButtonDblClk", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("OnRButtonDblClk", "Unknown") << "\n";
   }
   return FALSE;
}

BOOL SnipeManipulatorInstance::OnRButtonUp(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView)
{
   try {
      if (m_pPlugin->GetOnRButtonUpCallback())
         return m_pPlugin->GetOnRButtonUpCallback()(m_pPluginInstanceData, keymodifier, mousestate, point, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("OnRButtonUp", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("OnRButtonUp", "Unknown") << "\n";
   }
   return FALSE;
}

BOOL SnipeManipulatorInstance::OnMButtonDown(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView)
{
   try {
      if (m_pPlugin->GetOnMButtonDownCallback())
         return m_pPlugin->GetOnMButtonDownCallback()(m_pPluginInstanceData, keymodifier, mousestate, point, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("OnMButtonDown", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("OnMButtonDown", "Unknown") << "\n";
   }
   return FALSE;
}

BOOL SnipeManipulatorInstance::OnMButtonDblClk(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView)
{
   try {             
      if (m_pPlugin->GetOnMButtonDblClkCallback())
          return m_pPlugin->GetOnMButtonDblClkCallback()(m_pPluginInstanceData, keymodifier, mousestate, point, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("OnMButtonDblClk", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("OnMButtonDblClk", "Unknown") << "\n";
   }
   return FALSE;
}

BOOL SnipeManipulatorInstance::OnMButtonUp(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView)
{
   try {
      if (m_pPlugin->GetOnMButtonUpCallback())
         return m_pPlugin->GetOnMButtonUpCallback()(m_pPluginInstanceData, keymodifier, mousestate, point, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("OnMButtonUp", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("OnMButtonUp", "Unknown") << "\n";
   }
   return FALSE;
}

BOOL SnipeManipulatorInstance::OnMouseMove(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView)
{
   try {
      if (m_pPlugin->GetOnMouseMoveCallback())
         return m_pPlugin->GetOnMouseMoveCallback()(m_pPluginInstanceData, keymodifier, mousestate, point, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("OnMouseMove", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("OnMouseMove", "Unknown") << "\n";
   }
   return FALSE;
}

BOOL SnipeManipulatorInstance::OnMouseWheel(KeyModifiers keymodifier, MouseStates mousestate, short zDelta, CPoint point, CGLBaseView *pView)
{
   try {
      if (m_pPlugin->GetOnMouseWheelCallback())
         return m_pPlugin->GetOnMouseWheelCallback()(m_pPluginInstanceData, keymodifier, mousestate, zDelta, point, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("OnMouseWheel", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("OnMouseWheel", "Unknown") << "\n";
   }
   return FALSE;
}

BOOL SnipeManipulatorInstance::OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView )
{
   try {
      if (m_pPlugin->GetOnSetCursorCallback())
         return m_pPlugin->GetOnSetCursorCallback()(m_pPluginInstanceData, keymodifier, mousestate, point, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("OnSetCursor", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("OnSetCursor", "Unknown") << "\n";
   }
   return FALSE;
}

UINT SnipeManipulatorInstance::GetToolTipText( CPoint point, String &strText, const CGLBaseView *pView )
{
  try {
      if (m_pPlugin->GetGetToolTipTextCallback())
         return m_pPlugin->GetGetToolTipTextCallback()(m_pPluginInstanceData, point, strText, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("GetToolTipText", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("GetToolTipText", "Unknown") << "\n";
   }
   return -1;
}

BOOL SnipeManipulatorInstance::OnApply( CGLBaseView *pView )
{
   try {
      return m_pPlugin->GetOnApplyCallback()(m_pPluginInstanceData, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("OnApply", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("OnApply", "Unknown") << "\n";
   }
   return FALSE;
}

BOOL SnipeManipulatorInstance::OnCancel( CGLBaseView *pView )
{
   try {
      BOOL rvalue = m_pPlugin->GetOnCancelCallback()(m_pPluginInstanceData, pView);
      pView->GetDocument()->GetCommandObject()->GetCommandList()->UndoTempCommand();
      return rvalue;
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("OnCancel", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("OnCancel", "Unknown") << "\n";
   }
   return FALSE;
}

BOOL SnipeManipulatorInstance::OnArrowKeyDown(UINT nChar, UINT nRepCnt, CGLBaseView *pView)
{
   try {
      if (m_pPlugin->GetOnArrowKeyDownCallback())
         return m_pPlugin->GetOnArrowKeyDownCallback()(m_pPluginInstanceData, nChar, nRepCnt, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("OnArrowKeyDown", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("OnArrowKeyDown", "Unknown") << "\n";
   }
   return FALSE;
}

BOOL SnipeManipulatorInstance::OnArrowKeyUp(UINT nChar, UINT nRepCnt, CGLBaseView *pView)
{
   try {
      if (m_pPlugin->GetOnArrowKeyUpCallback())
         return m_pPlugin->GetOnArrowKeyUpCallback()(m_pPluginInstanceData, nChar, nRepCnt, pView);
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("OnArrowKeyUp", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("OnArrowKeyUp", "Unknown") << "\n";
   }
   return FALSE;
}
