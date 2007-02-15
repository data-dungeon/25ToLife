#include "stdafx.h"
#include "Bone.h"
#include "Vertex.h"
#include "GUI/ObjectExplorerBar.h"
#include "GUI/GLModelView.h"
#include "GUI/ModelDoc.h"
#include "GUI/MainFrm.h"
#include "resource.h"
#include "PolyModel.h"

/////////////////////////////////////////////////////
// Bone

IMPLEMENT_SNIPEOBJECT_CREATE( Bone )

GLuint Bone::m_glCallList;

Bone::Bone()
{
   m_bVisible.SetVisible( TRUE );

   if (!m_glCallList)
      m_glCallList = GetMainFrame()->CreateCallListFromGeoResource( IDR_BONE_GEO );
}

Bone::~Bone()
{
}

void Bone::AddVertex( Vertex *pVertex, BOOL bAddToVertexBoneList/*=TRUE*/ )
{
   ASSERT(pVertex);
   
   m_vertexmap.SetAt( pVertex->GetID(), pVertex );
   if (bAddToVertexBoneList)
      pVertex->AddBone(this);
}

BOOL Bone::RemoveVertex( Vertex *pVertex, BOOL bRemoveFromVertexBoneList/*=TRUE*/ )
{
   ASSERT(pVertex);

   if (bRemoveFromVertexBoneList)
      pVertex->RemoveBone(this);

   return m_vertexmap.RemoveKey( pVertex->GetID() );
}

Vector Bone::GetPos()
{
   //TODO
   return Vector(0);
}

void Bone::Draw( CGLBaseView *pBaseView, RGBAFloat &color, BOOL bDrawBone/*=TRUE*/, BOOL bDrawEnd/*=TRUE*/ )
{
   // set the view matrix
   glPushMatrix();

   Matrix44 matrix = pBaseView->GetWorldToViewMatrix();
   matrix *= GetObjectToWorldMatrix();
   matrix *= GetBoneToObjectMatrix();
   matrix *= Scalef(m_fLength.GetValue(),m_fLength.GetValue(),m_fLength.GetValue());
   GLfloat glmatrix[16];
   matrix.GetFloatMatrix( glmatrix, true );
   glLoadMatrixf( glmatrix );

   if (bDrawBone)
   {
      glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha()*0.3f );

      glCullFace( GL_BACK );
      BOOL bOldLighting = glSet( GL_LIGHTING, TRUE );
      BOOL bOldColorMaterial = glSet( GL_COLOR_MATERIAL, TRUE );

      glCallList( m_glCallList );

      glDisable(GL_LIGHTING);
      glPolygonMode( GL_FRONT, GL_LINE );
      glLineWidth(1.0);
      glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );
      glCallList( m_glCallList );

      glPolygonMode( GL_FRONT, GL_FILL );
      glSet( GL_COLOR_MATERIAL, bOldColorMaterial );
      glSet( GL_LIGHTING, bOldLighting );
   }

   if (bDrawEnd)
   {
      // get bone endpoint position
      Vector endpoint(0,0,1);
      endpoint *= matrix;
      endpoint *= pBaseView->GetViewToWorldMatrix();

      glLineWidth(2.0);
      glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );
      if (pBaseView->m_bDoPick && IsKindOf( pBaseView->m_nPickObjectType ))
         ((CGL3DView*)pBaseView)->Draw3DCircleFacingScreen(endpoint,6.0f,8,TRUE);
      else
         ((CGL3DView*)pBaseView)->Draw3DCircleFacingScreen(endpoint,6.0f,8,FALSE);
   }

   glPopMatrix();
}

void Bone::Draw( CGLBaseView *pBaseView )
{
   if (pBaseView->GetCurrentSelectionSOT() != SOT_Bone)
      return; // to prevent traversing the entire bone hierarchy

   BOOL bDrawBone = TRUE;
   BOOL bDrawEnd = TRUE;

   if (m_bVisible)
   {
      if (pBaseView->m_bDoPick && IsKindOf( pBaseView->m_nPickObjectType ))
      {
         glLoadName( (GLuint)this );
         switch(SnipeObject::GetCurrentPickElement())
         {
            case -1:
            case 1:
            {
               bDrawEnd = FALSE;
               bDrawBone = TRUE;
               break;
            }
            case 0:
            {
               bDrawEnd = TRUE;
               bDrawBone = FALSE;
               break;
            }
         }
      }

      RGBAFloat color( m_cBoneColor, 0.5f );

      BOOL bOldDepthTest = glSet( GL_DEPTH_TEST, FALSE );
      Draw( pBaseView, color, bDrawBone, bDrawEnd );
      glSet( GL_DEPTH_TEST, bOldDepthTest );

      //color.SetAlpha( 1.0f );
      //Draw( pBaseView, color, bDrawBone, bDrawEnd );

      if (pBaseView->m_bDoPick && IsKindOf( pBaseView->m_nPickObjectType ))
         glLoadName( NULL );
   }

   TransformableObject::Draw( pBaseView );
}

void Bone::DrawFocused( CGLBaseView *pBaseView )
{
   RGBAFloat color;
   if (IsLocked())
      color = g_focuscolorlocked;
   else
      color = g_focuscolor;

   CGLModelView *pModelView = (CGLModelView *)pBaseView;

   BOOL bOldDepthTest = glSet( GL_DEPTH_TEST, FALSE );
   color.SetAlpha( pModelView->GetDocument()->m_fPulse*0.5f );
   Draw( pBaseView, color );
   glSet( GL_DEPTH_TEST, bOldDepthTest );

   color.SetAlpha( pModelView->GetDocument()->m_fPulse );
   Draw( pBaseView, color );
}

void Bone::DrawSelected( CGLBaseView *pBaseView )
{
   RGBAFloat color;
   if (IsLocked())
      color = g_selectedcolorlocked;
   else
      color = g_selectedcolor;

   BOOL bOldDepthTest = glSet( GL_DEPTH_TEST, FALSE );
   color.SetAlpha( 0.5f );
   Draw( pBaseView, color );
   glSet( GL_DEPTH_TEST, bOldDepthTest );

   color.SetAlpha( 1.0f );
   Draw( pBaseView, color );
}

void Bone::DrawMirrored( CGLBaseView *pBaseView )
{
   RGBAFloat color;
   if (IsLocked())
      color = g_mirrorcolorlocked;
   else
      color = g_mirrorcolor;

   BOOL bOldDepthTest = glSet( GL_DEPTH_TEST, FALSE );
   color.SetAlpha( 0.5f );
   Draw( pBaseView, color );
   glSet( GL_DEPTH_TEST, bOldDepthTest );

   color.SetAlpha( 1.0f );
   Draw( pBaseView, color );
}

void Bone::DrawTreeItem( CGLControl *pItem, float x/*=0.0f*/ )
{
   RGBFloat cTint = m_cBoneColor;
   RGBFloat cOldTint = CGLObjectTreeControl::m_glImageListTreeIcons.SetTint( cTint );
      TransformableObject::DrawTreeItem( pItem, x );
   CGLObjectTreeControl::m_glImageListTreeIcons.SetTint( cOldTint );
}

BOOL Bone::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_REMOVEDFROMPROJECT:
      {
         for (POSITION pos = GetVertexHeadPosition(); pos; )
         {
            Vertex *pVertex = GetNextVertex( pos );
            pVertex->RemoveBone( this );
         }
         PolyModel *pPolyModel = FindModel();
         pPolyModel->SetBoneCount(pPolyModel->GetNumBones()-1);
         break;
      }
      case NOTIFYOBJ_ADDEDTOPROJECT:
      {
         for (POSITION pos = GetVertexHeadPosition(); pos; )
         {
            Vertex *pVertex = GetNextVertex( pos );
            pVertex->RemoveBone( this );
         }
         PolyModel *pPolyModel = FindModel();
         pPolyModel->SetBoneCount(pPolyModel->GetNumBones()+1);
         break;
      }
   }

   return TransformableObject::OnObjectNotify( msg, wParam, lParam );
}

void Bone::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   SnipeObjectHeader header( this, bWriteHeader, bs );

   if (GetNumVertices())
   {
      DataHeader header( bs, "Vertices" );

      SnipeObjectList vertexlist;
      for (POSITION pos=GetVertexHeadPosition(); pos; )
      {
         Vertex *pVertex = GetNextVertex( pos );
         vertexlist.AddTail( pVertex );
      }

      SnipeObject *pOldWorkingObject = GetMainInterface()->SetCurrentWorkingObject( GetCommandObject() );
      String strDescription;
      GetMainInterface()->ObjectsToDescription( &vertexlist, strDescription );
      bs << mar << strDescription << nl;
      GetMainInterface()->SetCurrentWorkingObject( pOldWorkingObject );
   }

   TransformableObject::Save( bs );
}

BOOL Bone::ParseData( BaseStream &bs, const DataHeader &dataheader )
{
   if (dataheader.m_strKey == "Vertices")
   {
      bs.ReadMargin();

      String strDescription;
      bs.ReadLine( strDescription );

      SnipeObject *pOldWorkingObject = GetMainInterface()->SetCurrentWorkingObject( GetCommandObject() );
      StringList strErrorList;
      SnipeObjectList *pVertexList = GetMainInterface()->DescriptionToObjects( strDescription, strErrorList );
      GetMainInterface()->SetCurrentWorkingObject( pOldWorkingObject );

      if (pVertexList)
      {
         for (POSITION pos=pVertexList->GetHeadPosition(); pos; )
         {
            Vertex *pVertex = (Vertex *)pVertexList->GetNext( pos )->IsKindOf( SOT_Vertex );
            if (pVertex)
               AddVertex( pVertex );
         }

         delete pVertexList;
      }
   }

   return TransformableObject::ParseData( bs, dataheader );
}

void Bone::PositionVertices( PolyModel *pModel, BOOL bTessellated )
{
   Bone *pCache = GetRootCache();
   Matrix44 objecttoworldmatrix = GetObjectToWorldMatrix();
   if (objecttoworldmatrix != pCache->m_prevobjecttoworld)
   {
      pCache->m_prevobjecttoworld = objecttoworldmatrix;

      for (POSITION pos=pCache->GetVertexHeadPosition(); pos; )
      {
         Vertex *pVertex = pCache->GetNextVertex( pos );
         Vector vPos = pVertex->GetPos();
         vPos *= objecttoworldmatrix;
         pVertex->SetRTVertexPos( vPos );
      }
   }

   for (int i=0; i<GetNumChildren(); i++)
   {
      TransformableObject *pChild = (TransformableObject *)GetChildAt( i );
      pChild->PositionVertices( pModel, bTessellated );
   }
}

PolyModel *Bone::FindModel()
{
   SnipeObject *pSnipeObject = this;
   // find bone container
   while(pSnipeObject->GetSOT()!=SOT_BoneContainer)
   {
      pSnipeObject = pSnipeObject->GetParent();
      if (!pSnipeObject)
         return NULL;
   }
   pSnipeObject = pSnipeObject->GetParent();//one up from bonecontainer
   if (pSnipeObject->IsKindOf(SOT_PolyModel))
   {
      return (PolyModel*)pSnipeObject;
   }
   return NULL;
}

/////////////////////////////////////////////////////
// BoneContainer

IMPLEMENT_SNIPEOBJECT_CREATE( BoneContainer )

BoneContainer::BoneContainer()
{
   m_strName = "Bones";
}

BoneContainer::~BoneContainer()
{
}

void BoneContainer::Draw( CGLBaseView *pView )
{
   if (pView->GetCurrentSelectionSOT() == SOT_Bone)
      return ContainerObject::Draw( pView );
   else
      return; // to prevent traversing the entire bone hierarchy
}

