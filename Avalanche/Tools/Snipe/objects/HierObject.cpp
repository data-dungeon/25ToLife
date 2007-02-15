#include "stdafx.h"
#include "PlaceholderObject.h"
#include "GUI/BaseDoc.h"
#include "GUI/Snipe.h"
#include "GUI/ModelDoc.h"
#include "GUI/SnipeDocTemplate.h"
#include "GUI/ObjectExplorerBar.h"
#include "GUI/GLSchematicView.h"

IMPLEMENT_SNIPEOBJECT_PURE( HierObject )

StringProperty::Info HierObject::m_strNotesInfo( "Notes", "Notes", "", TRUE );

HierObject::HierObject( )
{
   m_pDocument = NULL;
   m_bSchematicPositionsDirty = TRUE;

   m_strNotes.Init( &m_strNotesInfo, this );
}

HierObject::~HierObject(void)
{
 //  DestroyChildren();
}

void HierObject::PostConstruct()
{
   ParentableObject::PostConstruct();

   SetSelectionContainer( NULL );

   CommandList *pCommandList = GetCommandList();
   if (pCommandList)
      pCommandList->SetCommandObject(this);
}

void HierObject::PreDestruct()
{
   ParentableObject::PreDestruct();
   if (GetParent())
      ((HierObject *)GetParent())->RemoveChild( this );
   DestroyChildren();
}

void HierObject::DestroyChildren()
{
   for (int i=m_childarray.GetCount()-1; i>=0; i--)
   {
      SnipeObject *pChild = m_childarray[i];
      pChild->Destroy();
   }
}

String HierObject::GetTitle() const
{
   return GetMatchName();
}

String HierObject::GetMatchName() const
{
   if (IsCache() || m_bUserNamed)
      return m_strName;
//   else if (m_nInstanceNumber) // this doesn't work for bones.  might have to do something for root level objects
//      return GetCache()->GetMatchName() + m_nInstanceNumber;
   else
      return GetCache()->GetMatchName();
}

String HierObject::GetFileName()
{
   return m_strName;
}

BOOL HierObject::HasVisibleChild() const
{
   for (int i=0; i<m_childarray.GetCount(); i++)
      if (m_childarray[i]->IsVisible())
         return TRUE;

   return FALSE;
}

void HierObject::NotifyDescendants( ObjectNotificationMsg msg, WPARAM wParam/*=0*/, LPARAM lParam/*=0*/ )
{
   OnObjectNotify( msg, wParam, lParam );

   for (int i=0; i<m_childarray.GetCount(); i++)
   {
      HierObject *pChild = m_childarray[i];
      pChild->NotifyDescendants( msg, wParam, (LPARAM)pChild );
   }
}

void HierObject::NotifyObjectForEachDescendant( HierObject *pNotifyObject, ObjectNotificationMsg nMsg, WPARAM wParam )
{
   pNotifyObject->OnObjectNotify( nMsg, wParam, (LPARAM)this );

   for (int i=0; i<GetNumChildren(); i++)
   {
      HierObject *pChild = m_childarray[i];
      pChild->NotifyObjectForEachDescendant( pNotifyObject, nMsg, wParam );
   }
}

BOOL HierObject::Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError )
{
   DeleteInfo *&pHierObjectDeleteInfo = (HierObject::DeleteInfo *&)pDeleteInfo;

   if (pHierObjectDeleteInfo==NULL)
      pHierObjectDeleteInfo = SNEW DeleteInfo( this );

   if (GetParent())
   {
      pHierObjectDeleteInfo->m_pParent = GetParent();
      pHierObjectDeleteInfo->m_nChildIndex = ((HierObject *)GetParent())->RemoveChild( this );
   }

   return ParentableObject::Delete( pDeleteInfo, strOutError );
}

BOOL HierObject::UnDelete( SnipeObject::DeleteInfo *pDeleteInfo )
{
   if (!ParentableObject::UnDelete( pDeleteInfo ))
      return FALSE;

   DeleteInfo *pHierObjectDeleteInfo = (HierObject::DeleteInfo *)pDeleteInfo;
   if (pHierObjectDeleteInfo->m_pParent)
      ((HierObject *)pHierObjectDeleteInfo->m_pParent)->InsertChildAt( pHierObjectDeleteInfo->m_nChildIndex, this );

   //if (m_bSelectedAtDisconnect)
   //{
   //   SelectionList *pList = GetMainInterface()->GetCurrentSelectionList();
   //   pList->Add( this );
   //}
   return TRUE;
}

int HierObject::AddChildAtTail( HierObject *pChild )
{
   pChild->m_pParent = this;
   int nIndex = m_childarray.Add( pChild );
   pChild->m_bInList = TRUE;
   pChild->Reference();

   OnObjectNotify( NOTIFYOBJ_ADDEDCHILD, 0, (LPARAM) pChild );

   for (HierObject *pNotifyObject=this; pNotifyObject; pNotifyObject=(HierObject *) pNotifyObject->GetParent())
      pChild->NotifyObjectForEachDescendant( pNotifyObject, NOTIFYOBJ_ADDEDDESCENDANT, 0 );
   
   pChild->NotifyDescendants( NOTIFYOBJ_ADDEDTOPROJECT, 0, (LPARAM)pChild);

   pChild->m_strName = pChild->GetUniqueName( pChild->m_strName );

   return nIndex;
}

void HierObject::InsertChildAt( int nIndex, HierObject *pChild )
{
   if (nIndex == -1)
   {
      AddChildAtTail( pChild );
      return;
   }

   pChild->m_pParent = this;
   pChild->m_bInList = TRUE;

   m_childarray.InsertAt( nIndex, pChild );
   pChild->Reference();

   OnObjectNotify( NOTIFYOBJ_ADDEDCHILD, 0, (LPARAM) pChild );

   for (HierObject *pNotifyObject=this; pNotifyObject; pNotifyObject= (HierObject *)pNotifyObject->GetParent())
      pChild->NotifyObjectForEachDescendant( pNotifyObject, NOTIFYOBJ_ADDEDDESCENDANT, 0 );

   pChild->NotifyDescendants( NOTIFYOBJ_ADDEDTOPROJECT, 0, (LPARAM)pChild);

   pChild->m_strName = pChild->GetUniqueName( pChild->m_strName );
}

int HierObject::FindChildIndex( HierObject *pChild )
{
   for (int i=0; i<m_childarray.GetCount(); i++)
      if (m_childarray[i] == pChild)
         return i;
   return -1;
}

UINT HierObject::RemoveChild( HierObject *pChild )
{
   UINT nIndex = FindChildIndex( pChild );
   ASSERT(nIndex != -1);

   m_childarray.RemoveAt( nIndex );
   pChild->m_bInList = FALSE;

   OnObjectNotify( NOTIFYOBJ_REMOVEDCHILD, 0, (LPARAM) pChild );
   for (HierObject *pNotifyObject=this; pNotifyObject; pNotifyObject=(HierObject *) pNotifyObject->GetParent())
      pChild->NotifyObjectForEachDescendant( pNotifyObject, NOTIFYOBJ_REMOVEDDESCENDANT, 0 );

   if (IsInProject())
      pChild->NotifyDescendants( NOTIFYOBJ_REMOVEDFROMPROJECT, 0, (LPARAM)pChild);

   if (pChild->m_pDocument)
      pChild->m_pDocument->OnCloseDocument();

   pChild->m_pParent = NULL;
   pChild->DeReference();

   return nIndex;
}

PlaceholderObject *HierObject::AddPlaceholderObject( const String &strFileText )
{
   PlaceholderObject *pPlaceholderObject = PlaceholderObject::Create();
   pPlaceholderObject->SetFileText( strFileText );
   AddChildAtTail( pPlaceholderObject );
   return pPlaceholderObject;
}

void HierObject::CountDescendants( CArray<int> &totaldescendantcountarray, int nLevel )
{
   totaldescendantcountarray[nLevel]++;

   if (totaldescendantcountarray.GetSize() <= nLevel+1)
      totaldescendantcountarray.SetAtGrow( nLevel+1, 0 );
   for (int i=0; i<m_childarray.GetCount(); i++)
      m_childarray[i]->CountDescendants( totaldescendantcountarray, nLevel+1 );
}

void HierObject::SetSchematicPosition( CArray<int> &descendantcountarray, CArray<int> &totaldescendantcountarray, int nLevel )
{
   float fRatio = (float)descendantcountarray[nLevel] / (float)totaldescendantcountarray[nLevel];
   float fHeight = totaldescendantcountarray[nLevel] * 30.0f;
   float fTop = fHeight / 2.0f;
   m_v2SchematicPos.Set( nLevel*150.0f, fTop - fRatio * fHeight );

   descendantcountarray[nLevel]++;

   if (descendantcountarray.GetSize() <= nLevel+1)
      descendantcountarray.SetAtGrow( nLevel+1, 0 );
   for (int i=0; i<m_childarray.GetCount(); i++)
      m_childarray[i]->SetSchematicPosition( descendantcountarray, totaldescendantcountarray, nLevel+1 );

   m_bSchematicPositionsDirty = FALSE;
}

void HierObject::ComputeSchematicPositions()
{
   CArray<int> totaldescendantcountarray;
   int nLevel = 1;
   totaldescendantcountarray.SetAtGrow( nLevel, 0 );
   for (int i=0; i<m_childarray.GetCount(); i++)
      m_childarray[i]->CountDescendants( totaldescendantcountarray, nLevel );

   m_v2SchematicPos.Set( 0.0f, 0.0f );

   CArray<int> descendantcountarray;
   descendantcountarray.SetAtGrow( nLevel, 0 );
   for (int i=0; i<m_childarray.GetCount(); i++)
      m_childarray[i]->SetSchematicPosition( descendantcountarray, totaldescendantcountarray, nLevel );

   m_bSchematicPositionsDirty = FALSE;
}

void HierObject::DrawSchematic( CGLSchematicView *pView )
{
   if (m_bSchematicPositionsDirty)
      ComputeSchematicPositions();

   for (int i=0; i<m_childarray.GetCount(); i++)
      m_childarray[i]->DrawSchematic( pView );

   if (pView->m_bDoPick)
      glLoadName( (GLuint)this );

   float sh = 5.0f * pView->GetScreenPixelSize( 0.0f );
   float x1 = m_v2SchematicPos.x;
   float x2 = m_v2SchematicPos.x + sh + 100.0f;
   float y1 = m_v2SchematicPos.y;
   float y2 = m_v2SchematicPos.y - sh - 20.0f;
   float fIntensity = 0.5f;

   if (!pView->m_bDoPick)
   {
      // SPLINE TO PARENT
      if (m_pParent)
      {
         float px = ((HierObject *)m_pParent)->m_v2SchematicPos.x + 100.0f;
         float py = ((HierObject *)m_pParent)->m_v2SchematicPos.y;
         Vector2 p[4];
         p[0].Set( px, py - 10.0f );
         p[1].Set( x1 - px, 0.0f );
         p[2].Set( x1 - px, 0.0f );
         p[3].Set( x1, y1 - 10.0f );
         glColor3f( 0,0,0 );
         glLineWidth( 2.0f );
         pView->DrawSpline( p );
         glLineWidth( 1.0f );
      }

      // SHADOW
      glBegin( GL_QUADS );
      // top
      glColor4f( 0.0f, 0.0f ,0.0f, 0.0f );
      glVertex2f( x1, y1 );
      glVertex2f( x2, y1 );
      glColor4f( 0.0f, 0.0f ,0.0f, fIntensity );
      glVertex2f( x2-sh, y1-sh );
      glVertex2f( x1+sh, y1-sh );
      //right
      glColor4f( 0.0f, 0.0f ,0.0f, 0.0f );
      glVertex2f( x2, y1 );
      glVertex2f( x2, y2 );
      glColor4f( 0.0f, 0.0f ,0.0f, fIntensity );
      glVertex2f( x2-sh, y2+sh );
      glVertex2f( x2-sh, y1-sh );
      // bottom
      glColor4f( 0.0f, 0.0f ,0.0f, 0.0f );
      glVertex2f( x2, y2 );
      glVertex2f( x1, y2 );
      glColor4f( 0.0f, 0.0f ,0.0f, fIntensity );
      glVertex2f( x1+sh, y2+sh );
      glVertex2f( x2-sh, y2+sh );
      // left
      glColor4f( 0.0f, 0.0f ,0.0f, 0.0f );
      glVertex2f( x1, y2 );
      glVertex2f( x1, y1 );
      glColor4f( 0.0f, 0.0f ,0.0f, fIntensity );
      glVertex2f( x1+sh, y1-sh );
      glVertex2f( x1+sh, y2+sh );
      glEnd();
   }

   // BACKGROUND
   x2 -= sh;
   y2 += sh;

   float fTop = 0.9f;
   float fBottom = fTop * 0.5f;
   float r = 1.0f, g = 1.0f, b = 1.0f;
   if (IsSelected())
      b = 0.0f;
   glBegin( GL_QUADS );
   glColor3f( fTop*r, fTop*g, fTop*b );
   glVertex3f( x1, y1, -1.0f );
   glVertex3f( x2, y1, -1.0f );
   glColor3f( fBottom*r, fBottom*g, fBottom*b );
   glVertex3f( x2, y2, -1.0f );
   glVertex3f( x1, y2, -1.0f );
   glEnd();

   if (!pView->m_bDoPick)
   {
      // OUTLINE
      glColor3f( 0,0,0 );
      glBegin( GL_LINE_LOOP );
      glVertex3f( x1, y1, -1.0f );
      glVertex3f( x2, y1, -1.0f );
      glVertex3f( x2, y2, -1.0f );
      glVertex3f( x1, y2, -1.0f );
      glEnd();

      // ICON
      //Vector sc1( x1, y1, 0.0 );
      //pView->WorldToScreen( sc1 );
      //DrawTreeIcon( pView, sc1.x );

      // NAME
      String strName = GetTitle();
      if (strName.Length())
      {
         Vector sc1( x1, y1, 0.0 );
         Vector sc2( x2, y2, 0.0 );
         pView->WorldToScreen( sc1 );
         pView->WorldToScreen( sc2 );
         float params[4];
         glGetFloatv( GL_SCISSOR_BOX, params );
         pView->SetClip( CRect( fast_round(sc1.x), fast_round(sc1.y), fast_round(sc2.x), fast_round(sc2.y)));
         glColor3f( 0,0,0 );
         pView->DrawText( sc1.x+3.0f, sc1.y+13.0f, strName );
         glScissor( params[0], params[1], params[2], params[3] );
      }
   }
}

void HierObject::CountPickObjectType( CGLBaseView *pView, UINT &nCount )
{
   ParentableObject::CountPickObjectType( pView, nCount );

   for (int nChildIndex=0; nChildIndex<GetNumChildren(); nChildIndex++)
      GetChildAt( nChildIndex )->CountPickObjectType( pView, nCount );
}

void HierObject::Draw( CGLBaseView *pView )
{
   for (int nChildIndex=0; nChildIndex<GetNumChildren(); nChildIndex++)
   {
      SnipeObject *pSnipeObject = GetChildAt( nChildIndex );
      pSnipeObject->Draw( pView );
   }
}

void HierObject::SetName( const String &strName )
{
   if (m_strName == strName)
      return;

   m_strName = GetUniqueName( strName );
   m_bUserNamed = TRUE;
}

void HierObject::MarkSaveDependents()
{
   if (m_bSaveDependent)
      return;

   ParentableObject::MarkSaveDependents();

   for ( int i=0; i<m_childarray.GetCount(); i++ )
      m_childarray[i]->MarkSaveDependents();
}

SnipeObject *HierObject::CreateChild( const SnipeObjectInfo *pInfo )
{
   HierObject *pObject = (HierObject *)ParentableObject::CreateChild( pInfo );
   AddChildAtTail( pObject );
   return pObject;
}

void HierObject::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   SnipeObjectHeader header( this, bWriteHeader, bs );

   ParentableObject::Save( bs ); // This will save your properties

   for ( int i=0; i<m_childarray.GetCount(); i++ ) {
      HierObject *pChild = m_childarray[i];
      if (pChild->m_bSaveDependent)
         pChild->Save( bs, TRUE );
   }
}

BOOL HierObject::ParseData( BaseStream &bs, const DataHeader &dataheader )
{
   if (dataheader.m_strKey == "Name")
   {
      SetName( dataheader.m_strValue );
      return TRUE;
   }
   else
      return ParentableObject::ParseData( bs, dataheader );
}

BOOL HierObject::ConvertInfo::Convert()
{
   m_pFrom->Reference();
   m_pTo->Reference();
   m_name = m_pFrom->GetMatchName();
   String strerror;
   if (!m_pFrom->Delete((SnipeObject::DeleteInfo *&)m_pFromDeleteInfo,strerror))
      return FALSE;
   if (!m_pTo->Delete((SnipeObject::DeleteInfo *&)m_pToDeleteInfo,strerror))
      return FALSE;
   m_pToDeleteInfo->UnDelete();
   m_pTo->SetName(m_name);
   return TRUE;
}

void HierObject::ConvertInfo::ReConvert()
{
   m_pToDeleteInfo->UnDelete();
   m_pFromDeleteInfo->Delete();
   m_pTo->SetName(m_name);
}

void HierObject::ConvertInfo::UnConvert()
{
   m_pFromDeleteInfo->UnDelete();
   m_pToDeleteInfo->Delete();
   m_pFrom->SetName(m_name);
}

HierObject::ConvertInfo *HierObject::ConvertTo( SnipeObjectType nSOT )
{
   SnipeObjectInfo *pSnipeObjectInfo = SnipeObjectInfo::GetBySOT( nSOT );
   ASSERT(pSnipeObjectInfo);

   // create and add object to tree
   HierObject *pNewHierObject = (HierObject*)pSnipeObjectInfo->CreateObject();
   HierObject *pParent = (HierObject *)GetParent();
   int nChildIndex = pParent->FindChildIndex( this );
   ASSERT(nChildIndex >=0);
   pParent->InsertChildAt(nChildIndex,pNewHierObject);

   pNewHierObject->ConvertFrom( this );

   pNewHierObject->SetName("Converted");

   ConvertInfo *pConvertInfo = SNEW ConvertInfo(this,pNewHierObject);

   if (!pConvertInfo->Convert())
      return NULL;
  
/*
   CSnipeApp *pSnipeApp = (CSnipeApp*)AfxGetApp();

   for (POSITION pos = pSnipeApp->GetDocList().GetHeadPosition(); pos;)
   {
      CBaseDoc *pBaseDoc = (CBaseDoc*)pSnipeApp->GetDocList().GetNext(pos);
      SnipeDocType nType = pBaseDoc->GetDocType();
      if (pBaseDoc->GetDocType() == SDT_CModelDoc)
      {
         CModelDoc *pModelDoc = (CModelDoc*)pBaseDoc;

         if (pModelDoc->GetObjectHead()==this)
         {
            pModelDoc->SetObjectHead(pNewHierObject);
         }
      }
   }
*/

   return pConvertInfo;
}

void HierObject::OnBeginDrag( CPoint point, CGLObjectTreeItem *pDragTreeItem )
{
   if (!IsDraggable())
      return;
   SnipeObject::OnBeginDrag( point, pDragTreeItem );
}

void HierObject::OnDoDrag( CPoint point, CGLObjectTreeItem *pDragTreeItem, CGLObjectTreeItem *pHitTreeItem )
{
   if (!IsDraggable())
      return;
   SnipeObject::OnDoDrag( point, pDragTreeItem, pHitTreeItem ); 
}

void HierObject::OnEndDrag( CPoint point, CGLObjectTreeItem *pDragTreeItem, CGLObjectTreeItem *pHitTreeItem )
{
   if (!IsDraggable())
      return;
   SnipeObject::OnEndDrag( point, pDragTreeItem, pHitTreeItem );
   if (!pHitTreeItem)
      return;
   SnipeObject *pDragSnipeObject = (SnipeObject*)pDragTreeItem->GetObject();
   SnipeObject *pHitSnipeObject = (SnipeObject*)pHitTreeItem->GetObject();
   if (!pDragSnipeObject->IsKindOf(SOT_HierObject))
      return;
   if (!pHitSnipeObject->IsKindOf(SOT_HierObject))
      return;
   HierObject *pDragHierObject = (HierObject*)pDragSnipeObject;
   HierObject *pHitHierObject = (HierObject*)pHitSnipeObject;

   pDragHierObject->OnParent(pHitHierObject);
}

HierObject::ParentInfo *HierObject::Parent( HierObject *pHierObject )
{
   HierObject::ParentInfo *pParentInfo = SNEW ParentInfo();

   HierObject *pParent = (HierObject*)GetParent();
  
   pParentInfo->m_pChild = this;
   pParentInfo->m_pNewParent = pHierObject;

   if (pParent)
   {
      Reference();
      pParentInfo->m_pPrevParent = pParent;
      pParentInfo->m_nPrevChildIndex = pParent->RemoveChild( this );
   }

   pParentInfo->m_nNewChildIndex = pHierObject->AddChildAtTail(this);

   if (pParent)
      DeReference();

   return pParentInfo;
}

void HierObject::ParentInfo::ReParent()
{
   m_pChild->Reference();
   m_pPrevParent->RemoveChild(m_pChild);
   m_pNewParent->InsertChildAt( m_nNewChildIndex, m_pChild );
   m_pChild->DeReference();
}

void HierObject::ParentInfo::UnParent()
{
   m_pChild->Reference();
   m_pNewParent->RemoveChild(m_pChild);
   m_pPrevParent->InsertChildAt( m_nPrevChildIndex, m_pChild );
   m_pChild->DeReference();
}
