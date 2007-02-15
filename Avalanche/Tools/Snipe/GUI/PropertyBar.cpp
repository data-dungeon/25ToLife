// PropertiesPane.cpp : implementation file
//

#include "stdafx.h"
#include "Snipe.h"
#include "PropertyBar.h"
#include "HierObject.h"
#include "GLEditControl.h"
#include "../Plugin/CommandPluginManager.h"
#include "OutputView.h"
#include "GLManipulatorCombo.h"
#include "resource.h"

GLImageList CGLPropertyTreeControl::m_glImageListBackground;
GLImageList CGLPropertyTreeControl::m_glImageListExpand;
GLImageList CGLPropertyTreeItem::m_glImageListGroupBox;

GLImageList CGLBasicPropertyEdit<int>::m_glImageList;
GLImageList CGLBasicPropertyEdit<Time>::m_glImageList;
GLImageList CGLBasicPropertyEdit<float>::m_glImageList;
GLImageList CGLBasicPropertyEdit<Vector2>::m_glImageList;
GLImageList CGLBasicPropertyEdit<Vector>::m_glImageList;
GLImageList CGLBasicPropertyEdit<String>::m_glImageList;

CGLPropertyFrame::CGLPropertyFrame() :
   m_glTab( TRUE, TRUE )
{
}

CGLPropertyFrame::~CGLPropertyFrame()
{
}

void CGLPropertyFrame::PostInit()
{
   CGLControl::PostInit();

   m_glObjectPropertyBar.m_nID = IDR_OBJECTPROPERTIES;
   m_glViewPropertyBar.m_nID = IDR_VIEWPROPERTIES;
   m_glManipulatorPropertyBar.m_nID = IDR_MANIPULATORPROPERTIES;
   m_glNavigatorPropertyBar.m_nID = IDR_NAVIGATORPROPERTIES;

   m_glTab.Init( this, NULL, CPoint(0,0), CSize(0,TABHEIGHT) );

   m_glTab.AddTab( "Object", (LPARAM)&m_glObjectPropertyBar );
   m_glTab.AddTab( "View", (LPARAM)&m_glViewPropertyBar );
   m_glTab.AddTab( "Manipulator", (LPARAM)&m_glManipulatorPropertyBar );
   m_glTab.AddTab( "Navigator", (LPARAM)&m_glNavigatorPropertyBar );
}

void CGLPropertyFrame::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLControl::Move( ptControl, szControl, justification );

   m_glTab.Move( CPoint(2,2), CSize( m_szControl.cx-4, m_szControl.cy-4 ));
}

// Property Command
void DoStoreValueCommandCommitTemp( Property *pProperty )
{
   SnipeObject *pCommandObject = pProperty->GetCommandObject();
   if (pCommandObject)
   {
      CommandList *pCommandList = pCommandObject->GetCommandList();
      pCommandList->CommitTempCommand();
   }
}

void DoStoreValueCommand( Property *pProperty, const String &strValue, const Time &time/*=TAGGEDTIME*/, BOOL bTemp/*=FALSE*/ )
{
   static CommandPlugin *pSetValueCommandPlugin = GetMainInterface()->GetCommandPluginManager()->FindCommandByCLIName( "SnipeBasicCommands", "StoreValue" );
   ASSERT(pSetValueCommandPlugin);

   SnipeObject *pCommandObject = pProperty->GetCommandObject();
   if (pCommandObject && pProperty->IsUndoable())
   {
      GetMainInterface()->SetCurrentWorkingObject( pCommandObject );

      MemoryStream ms;
      ms.Open(BaseStream::BS_OUTPUT); // Faster than using strings

      if (!time.IsTagged())
         ms << "-t " <<time.ToString( 30.0f, Time::SMPTE  ) << ' ';

      if (GetMainInterface()->ObjectToDescription( pProperty, ms ))
      {
         CommandList *pCommandList = pCommandObject->GetCommandList();
         GetMainInterface()->SetCurrentCommandObject( pCommandObject );
         ms << '=';
         if (!strValue.IsEmpty())
            ms << strValue;
         ms << '\0';

         String strParameters = (char *)ms.GetBuffer();
         StringList strResults;

         pCommandList->Do( pSetValueCommandPlugin, strParameters, strResults, bTemp );
      }
   }
   else
      pProperty->StoreValueFromString( strValue, time, !bTemp );
}

/////////////////////////////////////////////////////////////////////////////
// CGLPropertyBar

IMPLEMENT_DYNCREATE(CGLPropertyBar, CGLControl);

CGLPropertyBar::CGLPropertyBar()
{
   m_pglCombo = NULL;
}

CGLPropertyBar::~CGLPropertyBar()
{
   if (m_pglCombo)
      delete m_pglCombo;
}

void CGLPropertyBar::PostInit()
{
   CGLControl::PostInit();

   if (!m_pglCombo)
      m_pglCombo = SNEW CGLObjectCombo;
   m_pglCombo->Init( this );
   m_pglCombo->SetWindowsLike();
   m_pglCombo->m_bFlatLook = TRUE;

   m_glTree.Init( this );
   m_glTree.MakeActive();
}

void CGLPropertyBar::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLControl::Move( ptControl, szControl, justification );

   m_pglCombo->Move( CPoint(2,2), CSize(m_szControl.cx-4,COMBOHEIGHT));
   m_glTree.Move( CPoint(2,2+COMBOHEIGHT), CSize(m_szControl.cx-4,m_szControl.cy-COMBOHEIGHT-4));
}

void CGLPropertyBar::OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler )
{
	CGLControlCmdUI state(this);
   state.DoUpdate(pTarget, bDisableIfNoHndler);
}

void CGLPropertyBar::SetObject( SnipeObject *pObject )
{
   if (pObject == m_glTree.GetObjectHead())
      return;

   m_glTree.SetObjectHead( pObject );
}

void CGLPropertyBar::FillObjectCombo()
{
   CGLObjectCombo *pCombo = (CGLObjectCombo *)m_pglCombo;

   pCombo->ResetContent();

   SnipeObject *pObject = m_glTree.GetObjectHead();
   if (pObject)
   {
      int nIndex = pCombo->AddObject( pObject );
      pCombo->SetCurSel( nIndex );
   }
}

/////////////////////////////////////////////////////////////////////////////
// CGLNavigatorPropertyBar

void CGLNavigatorPropertyBar::PostInit()
{
   if (!m_pglCombo)
      m_pglCombo = SNEW CGLManipulatorCombo;
   m_pglCombo->m_bEnabled = TRUE;

   CGLPropertyBar::PostInit();
}

void CGLNavigatorPropertyBar::FillObjectCombo()
{
   CGLManipulatorCombo *pCombo = (CGLManipulatorCombo *)m_pglCombo;
   pCombo->SetAvailableManipulators( &GetMainInterface()->GetSnipeNavigatorManager()->m_alllist );
}

BOOL CGLNavigatorPropertyBar::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYCTRL_CB_CHANGED:
      {
         if (m_pglCombo == (CGLManipulatorCombo *)lParam)
         {
            GetMainInterface()->SetCurrentNavigator( (SnipeNavigator *)((CGLManipulatorCombo *)m_pglCombo)->GetManipulator() );
            GetApp().DrawInvalidControls();
            return TRUE;
         }
      }
      break;
   }

   return CGLPropertyBar::OnControlNotify( msg, wParam, lParam );
}

///////////////////
BEGIN_MESSAGE_MAP(CGLPropertyTreeControl, CGLObjectTreeControl)
   ON_UPDATE_COMMAND_UI(ID_OUTPUTCONTROL, OnUpdateOutputControl)
END_MESSAGE_MAP()

CGLPropertyTreeControl::CGLPropertyTreeControl()
{
}

CGLPropertyTreeControl::~CGLPropertyTreeControl()
{
}

void CGLPropertyTreeControl::PostInit()
{
   CGLObjectTreeControl::PostInit();

   if (m_glImageListBackground.IsEmpty()) {
      m_glImageListBackground.Create( IDB_BACKGROUND );
      m_glImageListExpand.Create( IDB_PROPERTYEXPAND, 7 );
   }
}

void CGLPropertyTreeControl::SetObjectHead( SnipeObject *pObjectHead )
{
   if (pObjectHead && pObjectHead->IsProperty())
      pObjectHead = ((Property *)pObjectHead)->GetObject();

   if (m_pObjectHead)
      m_pObjectHead->RemoveNotifyNode( this );

   CGLTreeControl::Select( NULL );

   m_pObjectHead = pObjectHead;

   DeleteAllItems();

   if (m_pObjectHead) 
   {
      InsertObjectBranch( m_pObjectHead );
      m_pObjectHead->AddNotifyNode( NOTIFYOBJ_DELETE, this );
   }

   m_bNeedsRecalcLayout = TRUE;
   Invalidate();
}

void CGLPropertyTreeControl::InsertObjectBranch( SnipeObject *pObject, CGLTreeItem *pParentItem/*=NULL*/ )
{
   PropertyArray *pPropertyArray = pObject->GetPropertyArray();
   if (pPropertyArray)
   {
      for (int i=0; i<pPropertyArray->GetCount(); i++)
      {
         Property *pProperty = pPropertyArray->GetAt(i);
         CGLPropertyTreeItem *pItem = (CGLPropertyTreeItem *)InsertItem( pProperty, pParentItem );
         pItem->m_bOdd = i % 2;
         InsertObjectBranch( pProperty, pItem );
      }
   }
}

void CGLPropertyTreeControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   if (m_bNeedsRecalcLayout)
      Move( m_ptControl, GetControlSize() );

   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

//   glClearColor( 0.84f , 0.86f, 0.75f, 1.0f );
//   glClear( GL_DEPTH_BUFFER_BIT );

#if 0
   m_glImageListBackground.Draw( CRect(CPoint(0,0), m_szControl), FALSE, TRUE );
#else
   glColor4f( 0.0f, 0.0f, 0.f, 0.2f );
   glBegin( GL_QUADS );
      glVertex2f( 0.0f, 0.0f );
      glVertex2f( m_szControl.cx, 0.0f );
      glVertex2f( m_szControl.cx, m_szControl.cy );
      glVertex2f( 0.0f, m_szControl.cy );
   glEnd();
#endif
   CGLControl::Draw( nDrawFlags );

   int nWidth = m_szControl.cx - VALUECOLUMNWIDTH;
   glColor4f( 0.0f, 0.0f, 0.0f, 0.3f );
   glBegin( GL_LINES );
      glVertex2f( nWidth+0.5f, 0 );
      glVertex2f( nWidth+0.5f, m_szControl.cy );
   glEnd();

   DrawFocusRing();
   DrawShadow();
}

void CGLPropertyTreeControl::OnUpdateOutputControl( CCmdUI *pCmdUI )
{
   // Don't want default implementation
   CGLOutputControlCmdUI *pOutputControlCmdUI = (CGLOutputControlCmdUI *)pCmdUI;

   if (m_pObjectHead==NULL)
      return;

   SnipeObject *pCommandObject;

   if (m_pObjectHead->IsCommandObject())
      pCommandObject = m_pObjectHead;
   else
      pCommandObject = m_pObjectHead->GetCommandObject();

   if ( pCommandObject )
   {
      pOutputControlCmdUI->SetCurrentWorkingObject( pCommandObject );
      pOutputControlCmdUI->SetCurrentCommandObject( pCommandObject );
   }
}

//////////////////////////////////
// CGLPropertyParameterControl

void CGLPropertyParameterControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   if (GetTreeItem()->CGLTreeItem::IsSelected())
   {
      RGBAFloat rgba( 1.0f, 1.0f, 0.5f, 0.8f );
      RGBAFloat c[4] = { rgba, rgba, rgba*0.9f, rgba*0.9f };
      DrawGradient( c, CRect( 1, 1, m_szControl.cx-1, m_szControl.cy-1 ));
   }

   float x = 3.0f;
   if (GetTreeItem()->GetProperty()->HasVisibleProperty()) {
      CGLPropertyTreeControl::m_glImageListExpand.Draw( x, m_szControl.cy/2 - CGLPropertyTreeControl::m_glImageListExpand.GetHeight()/2, GetTreeItem()->IsExpanded() );
      x += CGLPropertyTreeControl::m_glImageListExpand.GetWidthEach() + 3.0f;
   }

   SnipeObject *pObj = GetTreeItem()->GetObject();
   if (pObj)
      pObj->DrawTreeItem( this, x );

   CGLControl::Draw( nDrawFlags );
}

BOOL CGLPropertyParameterControl::OnLButtonDown( UINT nFlags, CPoint point )
{
   return GetTreeItem()->OnLButtonDown( nFlags, point );
}

//////////////////////////////////
// CGLPropertyValueControl

void CGLPropertyValueControl::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);
   if (nDrawFlags & DRAW_PARENT && m_pParentControl)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already
   CGLControl::Draw( nDrawFlags );
}

//////////////////////////////////
// CGLPropertyTreeItem

CGLPropertyTreeItem::CGLPropertyTreeItem( CGLTreeControl *pTreeControl )
 : CGLObjectTreeItem( pTreeControl )
{
}

CGLPropertyTreeItem::~CGLPropertyTreeItem()
{
}

BOOL CGLPropertyTreeItem::IsThisVisible() const
{
   return CGLControl::IsThisVisible() && GetProperty()->IsVisible();
}

void CGLPropertyTreeItem::PostInit()
{
   CGLObjectTreeItem::PostInit();

   if (m_glImageListGroupBox.IsEmpty())
      m_glImageListGroupBox.Create( IDB_GROUPBOX, CRect( 10, 10, 20, 20 ) );

   m_glParameterControl.Init( this );

   m_glValueControl.Init( this );
   GetProperty()->CreatePropertyControls( &m_glValueControl );
}

void CGLPropertyTreeItem::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLObjectTreeItem::Move( ptControl, szControl, justification );

   m_glParameterControl.Move( CPoint(0, 0), CSize(m_szControl.cx - CGLPropertyTreeControl::VALUECOLUMNWIDTH, GetItemHeight()));
   m_glValueControl.Move( CPoint(m_szControl.cx - CGLPropertyTreeControl::VALUECOLUMNWIDTH, 0), CSize(CGLPropertyTreeControl::VALUECOLUMNWIDTH, GetItemHeight()));
}

void CGLPropertyTreeItem::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (m_bOutsideScissorArea)
      return;

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

   BOOL bHasVisibleChild = GetProperty()->HasVisibleProperty();

   if (IsExpanded() && bHasVisibleChild)
      m_glImageListGroupBox.DrawDiced( CRect( 4.0f, GetItemHeight()-1, m_szControl.cx - 4.0f, m_szControl.cy-4 ) );

   if (!m_pParentControl || m_pParentControl==GetTreeControl())
   {
      if (m_bOdd)
      {
         glColor4f( 0.0f, 0.0f, 0.0f, 0.06f );
         glBegin( GL_QUADS );
            glVertex2f( 0.0f, 0.0f );
            glVertex2f( m_szControl.cx, 0.0f );
            glVertex2f( m_szControl.cx, m_szControl.cy );
            glVertex2f( 0.0f, m_szControl.cy );
         glEnd();
      }

      glColor4f( 0.0f, 0.0f, 0.0f, 0.15f );
      glBegin( GL_LINES );
         glVertex2f( 0, m_szControl.cy );
         glVertex2f( m_szControl.cx, m_szControl.cy );
      glEnd();
   }

   CGLControl::Draw( nDrawFlags );
}

///////////////////

CGLObjectTreeItem *Property::AllocTreeItem( CGLTreeControl *pTreeControl )
{
   return SNEW CGLPropertyTreeItem( pTreeControl );
}

void Property::DrawTreeItem( CGLControl *pControl, float x/*=0.0f*/ )
{
   glColor3f( 0.1f, 0.1f, 0.1f );
   pControl->DrawText( x, 13, GetTitle());
}

void Property::CreatePropertyControls( CGLControl *pParent )
{
//   CGLButtonControl *pBtn = SNEW CGLButtonControl;
//   pBtn->Init( pParent->m_pGLRenderWnd, CPoint(19,4), CSize( 17, 13 ), -1, pParent, CGLControl::JUST_RIGHT );
}

void BoolProperty::CreatePropertyControls( CGLControl *pParent )
{
   CGLBoolPropertyCheckBox *pBtn = SNEW CGLBoolPropertyCheckBox( this );
   pBtn->Init( pParent, NULL, CPoint(PADDING,1), CSize( 16, 17 ) );

   Property::CreatePropertyControls( pParent );
}

void IntProperty::CreatePropertyControls( CGLControl *pParent )
{
   CGLBasicPropertyEdit<int> *pEdit = SNEW CGLBasicPropertyEdit<int>( this );
   pEdit->Init( pParent, NULL, CPoint(PADDING,2), CSize( EDITWIDTH, 16 ) );

   CGLBasicPropertySlider<int> *pSlider = SNEW CGLBasicPropertySlider<int>( this );
   pSlider->Init( pParent, NULL, CPoint(PADDING+EDITWIDTH+EDITGAP,2), CSize( CGLPropertyTreeControl::VALUECOLUMNWIDTH-(2*PADDING)-(EDITWIDTH+2), 16 ) );

   Property::CreatePropertyControls( pParent );
}

void StatisticProperty::CreatePropertyControls( CGLControl *pParent )
{
//   CGLBasicPropertyEdit<int> *pEdit = SNEW CGLBasicPropertyEdit<int>( this );
//   pEdit->Init( pParent, NULL, CPoint(PADDING,2), CSize( EDITWIDTH, 16 ) );

   Property::CreatePropertyControls( pParent );
}

void TypeProperty::CreatePropertyControls( CGLControl *pParent )
{
   CGLTypePropertyCombo *pCombo = SNEW CGLTypePropertyCombo( this );
   pCombo->Init( pParent, NULL, CPoint(PADDING,1), CSize( CGLPropertyTreeControl::VALUECOLUMNWIDTH - (2*PADDING), 18 ) );

   Property::CreatePropertyControls( pParent );
}

void FloatProperty::CreatePropertyControls( CGLControl *pParent )
{
   CGLBasicPropertyEdit<float> *pEdit = SNEW CGLBasicPropertyEdit<float>( this );
   pEdit->Init( pParent, NULL, CPoint(PADDING,2), CSize( EDITWIDTH, 16 ) );

   if (GetPropertyInfo()->m_minvalue != -MAX_FLOAT && GetPropertyInfo()->m_maxvalue != MAX_FLOAT)
   {
      CGLBasicPropertySlider<float> *pSlider = SNEW CGLBasicPropertySlider<float>( this );
      pSlider->Init( pParent, NULL, CPoint(PADDING+EDITWIDTH+EDITGAP,2), CSize( CGLPropertyTreeControl::VALUECOLUMNWIDTH-(2*PADDING)-(EDITWIDTH+EDITGAP), 16 ) );
   }

   Property::CreatePropertyControls( pParent );
}

void TimeProperty::CreatePropertyControls( CGLControl *pParent )
{
   CGLBasicPropertyEdit<Time> *pEdit = SNEW CGLBasicPropertyEdit<Time>( this );
   pEdit->Init( pParent, NULL, CPoint(PADDING,2), CSize( EDITWIDTH, 16 ) );

   Property::CreatePropertyControls( pParent );
}

void Vector2Property::CreatePropertyControls( CGLControl *pParent )
{
   CGLBasicPropertyEdit<Vector2> *pEdit = SNEW CGLBasicPropertyEdit<Vector2>( this, 0 );
   pEdit->Init( pParent, NULL, CPoint(PADDING,2), CSize( EDITWIDTH, 16 ) );

   pEdit = SNEW CGLBasicPropertyEdit<Vector2>( this, 1 );
   pEdit->Init( pParent, NULL, CPoint(PADDING+EDITWIDTH+EDITGAP,2), CSize( EDITWIDTH, 16 ) );

   Property::CreatePropertyControls( pParent );
}

void VectorProperty::CreatePropertyControls( CGLControl *pParent )
{
   CGLBasicPropertyEdit<Vector> *pEdit = SNEW CGLBasicPropertyEdit<Vector>( this, 0 );
   pEdit->Init( pParent, NULL, CPoint(PADDING,2), CSize( EDITWIDTH, 16 ) );

   pEdit = SNEW CGLBasicPropertyEdit<Vector>( this, 1 );
   pEdit->Init( pParent, NULL, CPoint(PADDING+EDITWIDTH+EDITGAP,2), CSize( EDITWIDTH, 16 ) );

   pEdit = SNEW CGLBasicPropertyEdit<Vector>( this, 2 );
   pEdit->Init( pParent, NULL, CPoint(PADDING+(EDITWIDTH+EDITGAP)*2,2), CSize( EDITWIDTH, 16 ) );

   Property::CreatePropertyControls( pParent );
}

void ColorProperty::CreatePropertyControls( CGLControl *pParent )
{
   CGLColorPropertyColorControl *pColor = SNEW CGLColorPropertyColorControl( this );
   pColor->Init( pParent, NULL, CPoint(PADDING,1), CSize( EDITWIDTH, 18 ) );

   Property::CreatePropertyControls( pParent );
}

void StringProperty::CreatePropertyControls( CGLControl *pParent )
{
   CGLStringPropertyEdit *pEdit = SNEW CGLStringPropertyEdit( this );
   pEdit->m_bKillFocusOnEnter = FALSE;
   pEdit->m_bMultiLine = TRUE;
   pEdit->Init( pParent, NULL, CPoint(PADDING,1), CSize( CGLPropertyTreeControl::VALUECOLUMNWIDTH-(2*PADDING), GetItemHeight()-2 ) );

   BasicProperty<String>::CreatePropertyControls( pParent );
}

void StringProperty::ReadValue( BaseStream &bs )
{
   String strValue;
   bs.ReadLine( strValue );
   SetValue( strValue );
}

void StringProperty::StoreValueFromString( const String &cstrValue, const Time &time/*=GetTime()*/, BOOL bStoreUndo/*=TRUE*/ )
{
   String strValue = cstrValue;
   strValue.Replace( "\x0D\x0A", "{n}" );
   strValue.Replace( "\n", "{n}" );

   BasicProperty<String>::StoreValueFromString( strValue, time, bStoreUndo );
}

int StringProperty::GetItemHeight()
{
   if (IsMultiLine())
      return min(5, GetCurrentValue().GetLineCount()) * 13 + 4 + 2;
   else
      return BasicProperty<String>::GetItemHeight();
}

void FileNameProperty::CreatePropertyControls( CGLControl *pParent )
{
   CGLFileNamePropertyBrowseEdit *pBrowseEdit = SNEW CGLFileNamePropertyBrowseEdit( this );
   pBrowseEdit->Init( pParent, NULL, CPoint(PADDING,1), CSize( CGLPropertyTreeControl::VALUECOLUMNWIDTH - (2*PADDING), 18 ) );

   Property::CreatePropertyControls( pParent );
}

void FileNameProperty::ReadValue( BaseStream &bs )
{
   String strRelative;
   bs.ReadLine( strRelative );
   SetValue( GetAbsoluteFileName(strRelative) );
}

BOOL FileNameProperty::WriteValue( BaseStream &bs ) 
{ 
   bs << GetRelativeFileName( GetValue() ); 
   return TRUE;
}

// CGLObjectListControl

CGLListItem *CGLObjectComboListControl::AllocItem( CGLListControl *pListControl, LPARAM lParam )
{
   CGLComboControl::ComboEntry *pComboEntry = (CGLComboControl::ComboEntry *)lParam;

   CGLListItem *pListItem = SNEW CGLObjectListItem;
   pListItem->m_nID = (UINT)pComboEntry->m_lParam;

   return pListItem;
}

CGLListItem *CGLObjectListControl::AllocItem( CGLListControl *pListControl, LPARAM lParam )
{
   CGLListItem *pListItem = SNEW CGLObjectListItem;
   pListItem->m_nID = (UINT)lParam;

   return pListItem;
}

// CGLObjectListItem

void CGLObjectListItem::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   if (nDrawFlags & DRAW_PARENT)
      m_pParentControl->Draw( DRAW_PARENT ); // this control has transparency, so have our parent draw first if it hasn't already

//   if (m_nIndex % 2)
//   {
//      glColor4f( 0.0f, 0.0f, 0.0f, 0.06f );
//      glBegin( GL_QUADS );
//      glVertex2f( 0.0f, 0.0f );
//      glVertex2f( m_szControl.cx, 0.0f );
//      glVertex2f( m_szControl.cx, m_szControl.cy );
//      glVertex2f( 0.0f, m_szControl.cy );
//      glEnd();
//   }

   if (IsSelected() || IsDropHilited())
   {
      RGBAFloat rgba( 1.0f, 1.0f, 0.5f, 0.8f );
      RGBAFloat c[4] = { rgba, rgba, rgba*0.9f, rgba*0.9f };
      DrawGradient( c, CRect( 0, 0, m_szControl.cx, GetItemHeight() ));
   }

   float x = MARGIN;
   SnipeObject *pSnipeObject = GetObject();
   pSnipeObject->DrawTreeItem( this, 5.0f );

   CGLControl::Draw( nDrawFlags ); // Skip CGLListItem
}

// CGLObjectCombo

CGLObjectCombo::CGLObjectCombo()
{
   m_bFlatLook = FALSE;
}

CGLObjectCombo::~CGLObjectCombo()
{
   for (int i=0; i<m_entries.GetCount(); i++)
      OnDeleteItem( i );
}

CGLListControl *CGLObjectCombo::AllocListControl()
{
   return SNEW CGLObjectComboListControl;
}

int CGLObjectCombo::AddObject( SnipeObject *pObject )
{
   ASSERT(pObject);

   return AddString( pObject->GetTitle(), (LPARAM)pObject );
}

void CGLObjectCombo::OnAddItem( int nIndex )
{
   SnipeObject *pObject = (SnipeObject *)GetItemData( nIndex );
   
   ASSERT(pObject);

   pObject->AddNotifyNode( NOTIFYOBJ_DELETE | NOTIFYOBJ_ICONCHANGED, this );
}

void CGLObjectCombo::OnDeleteItem( int nIndex )
{
   SnipeObject *pObject = (SnipeObject *)GetItemData( nIndex );
   
   ASSERT(pObject);

   pObject->RemoveNotifyNode( this );
}

BOOL CGLObjectCombo::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg) {
      case NOTIFYOBJ_DELETE:
      {
         int nIndex = Find(lParam);
         ASSERT(nIndex != -1);
         DeleteString( nIndex );
         break;
      }
      case NOTIFYOBJ_ICONCHANGED:
      {
         if (GetItemData( GetCurSel()) == lParam)
            Invalidate();
         break;
      }
   }
   return SnipeObject::OnObjectNotify( msg, wParam, lParam );
}

void CGLObjectCombo::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   GLDrawPrepper prep(this);

   int nOldIndex = m_nIndex;
   m_nIndex = -1;
   CGLComboControl::Draw( nDrawFlags );
   m_nIndex = nOldIndex;

   if (m_nIndex>=0 && m_nIndex<m_entries.GetCount())
   {
      HierObject *pObject = (HierObject *)m_entries[GetCurSel()].m_lParam;
      pObject->DrawTreeItem( this, m_bFlatLook ? 3.0f : 5.0f );
   }
}

///////////////////////////////////////
// CGLStringPropertyEdit

void CGLStringPropertyEdit::AdjustEditRect( CRect &rcEdit ) const
{
   rcEdit.DeflateRect( 2, 2, 2, 2 );
   CGLEditControlBase::AdjustEditRect( rcEdit );
}

void CGLStringPropertyEdit::PostInit()
{
   if (GetImageList()->IsEmpty())
      GetImageList()->Create( IDB_EDITBOX, CRect( 3, 3, 4, 4 ), 7 );

   CGLBasicPropertyEdit<String>::PostInit();
}

///////////////////////////////////////
// CGLPointerPropertyCombo

BEGIN_MESSAGE_MAP(CGLPointerPropertyCombo, CGLComboControl)
   ON_COMMAND(ID_POINTERPROPERTY_SELECTOBJECT, OnSelectObject)
   ON_UPDATE_COMMAND_UI(ID_POINTERPROPERTY_SELECTOBJECT, OnUpdateSelectObject)
END_MESSAGE_MAP()

CGLPointerPropertyCombo::CGLPointerPropertyCombo( PointerProperty<SnipeObject *> *pPointerProperty ) :
CGLObjectCombo()
{
   m_pPointerProperty = pPointerProperty;

   SnipeObjectList possibleobjects;
   if (m_pPointerProperty->GetPossiblePointerPropertyChoices( m_pPointerProperty, &possibleobjects ))
   {
      for (POSITION pos=possibleobjects.GetHeadPosition(); pos; )
      {
         SnipeObject *pObject = possibleobjects.GetNext( pos );
         int nIndex = AddObject( pObject );
         if (pObject == m_pPointerProperty->GetValue())
            CGLObjectCombo::SetCurSel( nIndex );
      }
   }

   m_pPointerProperty->AddNotifyNode( NOTIFYOBJ_VALUECHANGED, this );
}

CGLPointerPropertyCombo::~CGLPointerPropertyCombo()
{
   m_pPointerProperty->RemoveNotifyNode( this );
}

void CGLPointerPropertyCombo::OnUpdateSelectObject( CCmdUI *pCmdUI )
{
   SnipeObject *pSnipeObject = m_pPointerProperty->GetValue();

   if (pSnipeObject)
   {
      pCmdUI->Enable( TRUE );
      pCmdUI->SetText( String("Select " + pSnipeObject->GetTitle()).Get() );
   }
   else if (pCmdUI->m_pMenu)
      pCmdUI->m_pMenu->DeleteMenu( pCmdUI->m_nIndex, MF_BYPOSITION );
}

void CGLPointerPropertyCombo::OnSelectObject()
{
   SnipeObject *pSnipeObject = m_pPointerProperty->GetValue();
   if (pSnipeObject)
      pSnipeObject->Select();
}

void CGLPointerPropertyCombo::SetCurSel( int nIndex, BOOL bNotify/*=TRUE*/ )
{
   SnipeObject *pSnipeObject = (SnipeObject *)GetItemData( nIndex );
   String strValue;
   SnipeObject *pOldWorkingObject = GetMainInterface()->SetCurrentWorkingObject( m_pPointerProperty->GetObject() );
   if (GetMainInterface()->ObjectToDescription( pSnipeObject, strValue ))
      DoStoreValueCommand( m_pPointerProperty, strValue );
   GetMainInterface()->SetCurrentWorkingObject( pOldWorkingObject );
}

void CGLPointerPropertyCombo::GetText( String &strValue ) const
{
   if (m_pPointerProperty->IsSet())
      CGLObjectCombo::GetText( strValue );
   else
   {
      static String strNotSet = String::GetResourceString( IDS_NOTSET );
      strValue = strNotSet;
   }
}

UINT CGLPointerPropertyCombo::GetContextMenuID( const CPoint &point, UINT &nDefaultItemID )
{
   return IDR_POINTERPROPERTYCONTROL;
}

BOOL CGLPointerPropertyCombo::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
   case NOTIFYOBJ_VALUECHANGED:
      {
         for (INT_PTR i=0; i<m_entries.GetCount(); i++)
         {
            if (m_pPointerProperty->GetValue() == (SnipeObject *)GetItemData( i ))
            {
               CGLObjectCombo::SetCurSel( i );
               break;
            }
         }
         break;
      }
   }
   return SnipeObject::OnObjectNotify( msg, wParam, lParam );
}

void CGLTypePropertyCombo::GetText( String &strValue ) const
{
   if (m_pProperty->IsSet())
      CGLComboControl::GetText( strValue );
   else
   {
      static String strNotSet = String::GetResourceString( IDS_NOTSET );
      strValue = strNotSet;
   }
}

void CGLColorPropertyColorControl::FillColorChip( const CRect &rcFill )
{
   if (m_pProperty->IsSet())
      CGLColorControl::FillColorChip( rcFill );
   else
   {
      static String strNotSet = String::GetResourceString(IDS_NOTSET);
      static CSize szText = GetTextExtent( strNotSet );
      glColor3f( 0.0f, 0.0f, 0.0f );
      DrawText( rcFill.left + (rcFill.Width() - szText.cx) / 2, rcFill.top+11, strNotSet );
   }
}
