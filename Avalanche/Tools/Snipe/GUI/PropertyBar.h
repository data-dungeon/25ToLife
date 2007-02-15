#pragma once

#include <afxpriv.h>
#include "Snipe.h"
#include "ObjectExplorerBar.h"
#include "GLBrowseEditControl.h"
#include "GLButtonControl.h"
#include "GLTabControl.h"
#include "GLListControl.h"
#include "GLComboControl.h"
#include "..\Properties\Property.h"

#define PADDING   5
#define EDITGAP   2
#define EDITWIDTH 70

class Property;

class CGLPropertyTreeControl : public CGLObjectTreeControl
{
   DECLARE_MESSAGE_MAP()
public:
   static GLImageList m_glImageListBackground;
   static GLImageList m_glImageListExpand;

   enum { ITEMHEIGHT=20, VALUECOLUMNWIDTH=(2*PADDING)+(3*EDITWIDTH)+(2*EDITGAP) };

            CGLPropertyTreeControl();
   virtual ~CGLPropertyTreeControl();

// Operations
   CGLTreeItem *InsertItem( Property *pProperty, CGLTreeItem *pParent=NULL, CGLTreeItem *pInsertAfter=GLTI_LAST )
   {
      return CGLObjectTreeControl::InsertItem( (LPARAM)pProperty, pParent, pInsertAfter );
   }
   void InsertObjectBranch( SnipeObject *pObject, CGLTreeItem *pParentItem=NULL );

   afx_msg void OnUpdateOutputControl( CCmdUI *pCmnUI );

   void SetObjectHead( SnipeObject *pObjectHead );

// CGLControl overrides
   virtual void PostInit();
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};

class CGLPropertyTreeItem;

class CGLPropertyParameterControl : public CGLControl
{
public:
// Operations
   CGLPropertyTreeItem *GetTreeItem() { return (CGLPropertyTreeItem *)m_pParentControl; }

// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
};

class CGLPropertyValueControl : public CGLControl
{
public:
// Operations
   CGLPropertyTreeItem *GetTreeItem() { return (CGLPropertyTreeItem *)m_pParentControl; }

// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};

class CGLPropertyTreeItem : public CGLObjectTreeItem
{
public:
   static GLImageList m_glImageListGroupBox;
//   m_nID is a (InstanceObject *)
   CGLPropertyParameterControl m_glParameterControl;
   CGLPropertyValueControl     m_glValueControl;

            CGLPropertyTreeItem( CGLTreeControl *pTreeControl );
   virtual ~CGLPropertyTreeItem();

   Property *GetProperty() const { return (Property *)m_nID; }

// CGLTreeItem overrides
   virtual int GetExpandedMargin() { return 5; }
   virtual BOOL IsExpandable() { return GetProperty()->GetPropertyArray()->GetSize(); }

// CGLControl overrides
   virtual BOOL IsThisVisible() const;
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};

class CGLObjectListControl : public CGLListControl
{
public:
// CGLListControl overrides
   virtual CGLListItem *AllocItem( CGLListControl *pListControl, LPARAM lParam );
};

class CGLObjectListItem : public CGLListItem
{
public:
   SnipeObject *GetObject() { return (SnipeObject *)m_nID; }

// CGLListItem overrides
   virt_base virtual int GetItemHeight() { return GetObject()->GetItemHeight(); }

// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};

class CGLObjectComboListControl : public CGLObjectListControl
{
public:
// CGLListControl overrides
   virtual CGLListItem *AllocItem( CGLListControl *pListControl, LPARAM lParam );
};

class CGLObjectCombo : public CGLComboControl, public SnipeObject
{
public:
            CGLObjectCombo();
   virtual ~CGLObjectCombo();

   int AddObject( SnipeObject *pObject );

// CGLComboControl
   virtual CGLListControl *AllocListControl();
   virtual void OnAddItem( int nIndex );
   virtual void OnDeleteItem( int nIndex );

// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual String GetMatchName() const { return "CGLObjectCombo"; }
   virtual const char *GetClassName() const { return "CGLObjectCombo"; }
};

class CGLPropertyBar : public CGLBorderedControl
{
   DECLARE_DYNCREATE(CGLPropertyBar);
public:
   enum { COMBOHEIGHT=18 };
   CGLComboControl        *m_pglCombo;
   CGLPropertyTreeControl  m_glTree;

            CGLPropertyBar();
	virtual ~CGLPropertyBar();

// Operations
   void SetObject( SnipeObject *pObject );

// Overridables
   virt_base virtual void FillObjectCombo();

// CGLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );

// CCmdTarget overrides
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
};

class CGLNavigatorPropertyBar : public CGLPropertyBar
{
public:
// CGLPropertyBar overrides
   virtual void FillObjectCombo();

// CGLControl overrides
   virtual void PostInit();
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};

class CGLPropertyFrame : public CGLBorderedControl
{
public:
   enum { TABHEIGHT=20 };

   CGLTabControl            m_glTab;
     CGLPropertyBar           m_glObjectPropertyBar;
     CGLPropertyBar           m_glViewPropertyBar;
     CGLPropertyBar           m_glManipulatorPropertyBar;
     CGLNavigatorPropertyBar  m_glNavigatorPropertyBar;

            CGLPropertyFrame();
	virtual ~CGLPropertyFrame();

// Operations

// CGLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
};

template< typename TYPE >
class CGLBasicPropertyEdit : public CGLEditControlBase, public SnipeObject
{
private:
   static GLImageList m_glImageList;

   BasicProperty<TYPE> *m_pProperty;
   int                  m_nIndex;

public:
   CGLBasicPropertyEdit( BasicProperty<TYPE> *pProperty, int nIndex=0 )
   {
      m_bCenterText = TRUE;
      m_pProperty = NULL;
      m_nIndex = nIndex;
      SetProperty( pProperty );
   }
   ~CGLBasicPropertyEdit()
   {
      SetProperty( NULL );
   }

   void SetProperty( BasicProperty<TYPE> *pProperty )
   {
      if (m_pProperty)
         m_pProperty->RemoveNotifyNode( this );

      m_pProperty = pProperty;

      if (m_pProperty)
         m_pProperty->AddNotifyNode( NOTIFYOBJ_VALUECHANGED, this );

      Invalidate();
   }

// CGLEditControl overrides
   virtual void StoreText( const String &cstrText )
   {
      if (m_pProperty)
      {
         String strText = cstrText;
         m_pProperty->StoreValueIndexFromString( strText, m_nIndex );
      }
      //GetApp().DrawInvalidControls();
   }
   virtual void GetText( String &strValue ) const
   {
      if (m_pProperty)
      {
         m_pProperty->GetValueIndexString( strValue, m_nIndex );
         strValue.Replace( "{n}", "\x0D\x0A" );
      }
   }
   virtual GLImageList *GetImageList() const
   {
      return &m_glImageList;
   }
   virtual void AdjustEditRect( CRect &rcEdit ) const
   {
      rcEdit.DeflateRect( 14, 2, 14, 1 );
   }
   virtual void OnScrub( int nPixels, BOOL bStoreUndo = FALSE )
   {
      if (m_pProperty)
      {
         m_pProperty->OnScrub( nPixels, m_nIndex, bStoreUndo );
         GetApp().DrawInvalidControls();
      }
   }
   virtual void OnEndScrub() 
   {
      if (m_pProperty)
         DoStoreValueCommandCommitTemp( m_pProperty );
   }
   virtual COLORREF GetBkColor() const { return RGB(251,251,248); }
   virtual RGBAFloat GetTextColor()
   {
      if (m_pProperty && !m_pProperty->IsSet())
         return RGBAFloat( 0.0f, 0.0f, 0.0f, 0.3f );
      else
         return CGLEditControlBase::GetTextColor();
   }

// CGLControl overrides
   virtual void PostInit()
   {
      CGLEditControlBase::PostInit();

      if (m_glImageList.IsEmpty())
         m_glImageList.Create( 147/*IDB_NUMERICALEDIT*/, CRect(CPoint(10,2),CSize(46,12)), 66 );
   }

   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point )
   {
      int nSpin = 0;

      CRect rcDecrement( 0,0,13,m_szControl.cy );
      if (rcDecrement.PtInRect( point ))
         nSpin = -1;

      CRect rcIncrement( m_szControl.cx-13,0,m_szControl.cx,m_szControl.cy );
      if (rcIncrement.PtInRect( point ))
         nSpin = 1;

      if (nSpin) {
         OnScrub( nSpin );

         MSG msg;
         msg.message = 0;
          
         DWORD nLastTicks = GetTickCount();
         DWORD nTicksNeeded = 300;
         float fAccelerator = 0.0f;

         while (msg.message != WM_LBUTTONUP)	{
		      if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			      TranslateMessage(&msg);
			      DispatchMessage(&msg);
			      continue;
		      }

		      if (GetTickCount() - nLastTicks > nTicksNeeded) {
               OnScrub( nSpin );
               ((CWnd *)m_pGLRenderWnd)->SendMessage( WM_IDLEUPDATECMDUI, (WPARAM)TRUE);// for CLI to update
			      nTicksNeeded = 30.0f - fAccelerator;
               if (fAccelerator < 29.0f)
                  fAccelerator += 0.1f;
			      nLastTicks = GetTickCount();
		      }
         }
      }
      return CGLEditControlBase::OnLButtonDown( nFlags, point );
   }
   virtual BOOL OnLButtonUp( UINT nFlags, CPoint point )
   {
      if (GetMainInterface()->GetCurrentCommandList())
         GetMainInterface()->GetCurrentCommandList()->CommitTempCommand();

      return CGLEditControlBase::OnLButtonUp( nFlags, point );
   }

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
   {
      switch (msg) {
         case NOTIFYOBJ_VALUECHANGED:
            Invalidate();
            break;
      }
      return SnipeObject::OnObjectNotify( msg, wParam, lParam );
   }
   virtual const char *GetClassName() const { return "CGLBasicPropertyEdit"; }
   virtual String GetMatchName() const { return "CGLBasicPropertyEdit"; }
};

class CGLStringPropertyEdit : public CGLBasicPropertyEdit<String>
{
public:
   CGLStringPropertyEdit( StringProperty *pProperty, int nIndex=0 )
      : CGLBasicPropertyEdit<String>( pProperty, nIndex )
   {
      m_bCenterText = FALSE;
      m_bReadOnly = pProperty->IsReadOnly();
   }

// CGLEditControlBase Overrides
   virtual void AdjustEditRect( CRect &rcEdit ) const;

// CGLControl Overrides
   virtual void PostInit();
};

class CGLFileNamePropertyBrowseEdit : public CGLBrowseEditControl, public SnipeObject
{
private:
   FileNameProperty *m_pProperty;

public:
   CGLFileNamePropertyBrowseEdit( FileNameProperty *pProperty )
   {
      m_pProperty = pProperty;
      SetReadOnly( pProperty->IsReadOnly() );
      m_pProperty->AddNotifyNode( NOTIFYOBJ_VALUECHANGED, this );
   }
   ~CGLFileNamePropertyBrowseEdit()
   {
      m_pProperty->RemoveNotifyNode( this );
   }

// CGLControl overrides
   virtual void PostInit()
   {
      CGLBrowseEditControl::PostInit();
      SetText( m_pProperty->GetValue(), FALSE );
      SetFilters( m_pProperty->GetFilters() );
   }

   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
   {
      switch (msg) {
         case NOTIFYCTRL_EN_CHANGED: 
         {
            String strText;
            GetText(strText);
            m_pProperty->StoreValueIndexFromString( strText, 0 );
            return TRUE;
         }
      }
      return CGLBrowseEditControl::OnControlNotify( msg, wParam, lParam );
   }

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
   {
      switch (msg) {
         case NOTIFYOBJ_VALUECHANGED:
            SetText( m_pProperty->GetValue(), FALSE );
            break;
      }
      return SnipeObject::OnObjectNotify( msg, wParam, lParam );
   }
   virtual const char *GetClassName() const { return "CGLFileNamePropertyBrowseEdit"; }
   virtual String GetMatchName() const { return "CGLFileNamePropertyBrowseEdit"; }
};

template< typename TYPE >
class CGLBasicPropertySlider : public CGLSliderControl, public SnipeObject
{
private:
   BasicProperty<TYPE> *m_pProperty;

public:
   CGLBasicPropertySlider( BasicProperty<TYPE> *pProperty ) :
      CGLSliderControl( pProperty->GetFactoredValue(), pProperty->GetPropertyInfo()->m_minvalue * pProperty->GetFactor(), pProperty->GetPropertyInfo()->m_maxvalue * pProperty->GetFactor() )
   {
      m_pProperty = pProperty;
      m_pProperty->AddNotifyNode( NOTIFYOBJ_VALUECHANGED, this );
   }
   ~CGLBasicPropertySlider()
   {
      m_pProperty->RemoveNotifyNode( this );
   }

// CGLSliderControl overrides
   virtual int SetPos( int nPos )
   {
      nPos = min( GetMax(), max( GetMin(), nPos ));

      if (GetPos() == nPos)
         return nPos;

      int rvalue = GetPos();
      String strValue = (String)nPos;
      DoStoreValueCommand( m_pProperty, strValue, TAGGEDTIME, TRUE );
      GetApp().DrawInvalidControls();
      return rvalue;
   }

   virtual void OnEndScrub()
   {
      DoStoreValueCommandCommitTemp( m_pProperty );
   }

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
   {
      switch (msg) {
         case NOTIFYOBJ_VALUECHANGED:
            CGLSliderControl::SetPos( m_pProperty->GetFactoredValue() );
            break;
      }
      return SnipeObject::OnObjectNotify( msg, wParam, lParam );
   }
   virtual const char *GetClassName() const { return "CGLBasicPropertySlider"; };
   virtual String GetMatchName() const { return "CGLBasicPropertySlider"; };
};

class CGLBoolPropertyCheckBox : public CGLButtonControl, public SnipeObject
{
private:
   BoolProperty *m_pProperty;

public:
   CGLBoolPropertyCheckBox( BoolProperty *pProperty ) :
      CGLButtonControl( TRUE, pProperty->GetValue() )
   {
      m_pProperty = pProperty;
      m_pProperty->AddNotifyNode( NOTIFYOBJ_VALUECHANGED, this );
   }
   ~CGLBoolPropertyCheckBox()
   {
      m_pProperty->RemoveNotifyNode( this );
   }

// CGLButtonControl overrides
   virtual BOOL SetChecked( BOOL bChecked )
   {
      BOOL rvalue = GetChecked();
      DoStoreValueCommand( m_pProperty, bChecked?"ON":"OFF" );
      GetApp().DrawInvalidControls();
      return rvalue;
   }

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
   {
      switch (msg) {
         case NOTIFYOBJ_VALUECHANGED:
            CGLButtonControl::SetChecked( m_pProperty->GetValue() );
            break;
      }
      return SnipeObject::OnObjectNotify( msg, wParam, lParam );
   }
   virtual const char *GetClassName() const { return "CGLBoolPropertyCheckBox"; }
   virtual String GetMatchName() const { return "CGLBoolPropertyCheckBox"; }
};

class CGLTypePropertyCombo : public CGLComboControl, public SnipeObject
{
private:
   TypeProperty *m_pProperty;

public:
   CGLTypePropertyCombo( TypeProperty *pProperty ) :
      CGLComboControl()
   {
      m_pProperty = pProperty;

      for (int i=0; i<m_pProperty->GetPropertyInfo()->GetNodeCount(); i++)
      {
         TypeProperty::Info::Node *pNode=&m_pProperty->GetPropertyInfo()->GetNodeArray()[ i ];
         AddString( pNode->GetName().Get(), (LPARAM)pNode );
      }
      CGLComboControl::SetCurSel( m_pProperty->GetValue());

      m_pProperty->AddNotifyNode( NOTIFYOBJ_VALUECHANGED, this );
   }
   ~CGLTypePropertyCombo()
   {
      m_pProperty->RemoveNotifyNode( this );
   }

// CGLComboControl overrides
   virtual void SetCurSel( int nIndex, BOOL bNotify=TRUE )
   {
      String strValue = m_pProperty->GetPropertyInfo()->GetNodeArray()[ nIndex ].GetMatchName();
      DoStoreValueCommand( m_pProperty, strValue );
   }
   virtual void GetText( String &strValue ) const;

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
   {
      switch (msg) {
         case NOTIFYOBJ_VALUECHANGED:
            CGLComboControl::SetCurSel( m_pProperty->GetValue() );
            break;
      }
      return SnipeObject::OnObjectNotify( msg, wParam, lParam );
   }
   virtual const char *GetClassName() const { return "CGLTypePropertyCombo"; }
   virtual String GetMatchName() const { return "CGLTypePropertyCombo"; }
};

class CGLPointerPropertyCombo : public CGLObjectCombo
{
   DECLARE_MESSAGE_MAP()
private:
   PointerProperty<SnipeObject *> *m_pPointerProperty;

public:
   CGLPointerPropertyCombo( PointerProperty<SnipeObject *> *pPointerProperty );
   virtual ~CGLPointerPropertyCombo();

   afx_msg void OnUpdateSelectObject( CCmdUI *pCmdUI );
   afx_msg void OnSelectObject();

// CGLComboControl overrides
   virtual void SetCurSel( int nIndex, BOOL bNotify=TRUE );
   virtual void GetText( String &strValue ) const;

// CGLControl overrides
   virtual UINT GetContextMenuID( const CPoint &point, UINT &nDefaultItemID );

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual const char *GetClassName() const { return "CGLPointerPropertyCombo"; }
   virtual String GetMatchName() const { return "CGLPointerPropertyCombo"; }
};

class CGLColorPropertyColorControl : public CGLColorControl, public SnipeObject
{
private:
   ColorProperty *m_pProperty;

public:
   CGLColorPropertyColorControl( ColorProperty *pProperty ) :
      CGLColorControl( pProperty->GetValue() )
   {
      m_pProperty = pProperty;
      m_pProperty->AddNotifyNode( NOTIFYOBJ_VALUECHANGED, this );
   }
   ~CGLColorPropertyColorControl()
   {
      m_pProperty->RemoveNotifyNode( this );
   }

// CGLColorControl overrides
   virtual RGBFloat SetColor( const RGBFloat &rgb )
   {
      RGBFloat rvalue = GetColor();

      String strValue;
      CONVERTTOSTRING( rgb, strValue );
      DoStoreValueCommand( m_pProperty, strValue );    

      GetApp().DrawInvalidControls();
      return rvalue;
   }
   virtual void FillColorChip( const CRect &rcFill );

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
   {
      switch (msg) {
         case NOTIFYOBJ_VALUECHANGED:
            CGLColorControl::SetColor( m_pProperty->GetValue() );
            break;
      }
      return SnipeObject::OnObjectNotify( msg, wParam, lParam );
   }
   virtual const char *GetClassName() const { return "CGLColorPropertyColorControl"; }
   virtual String GetMatchName() const { return "CGLColorPropertyColorControl"; }
};

template< class TYPE >
void PointerProperty<TYPE>::CreatePropertyControls( CGLControl *pParent )
{
   CGLObjectCombo *pCombo = SNEW CGLPointerPropertyCombo( (PointerProperty<SnipeObject *> *)this );

   pCombo->Init( pParent, NULL, CPoint(PADDING,1), CSize( CGLPropertyTreeControl::VALUECOLUMNWIDTH - (2*PADDING), 18 ) );

   BasicProperty<TYPE>::CreatePropertyControls( pParent );
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
