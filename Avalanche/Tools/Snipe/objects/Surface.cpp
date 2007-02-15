#include "stdafx.h"
#include "Surface.h"
#include "Properties/UVMapSurfaceDriver.h"
#include "TexCoords.h"
#include "PolyModel.h"
#include "GUI/MainFrm.h"
#include "GUI/GL3DView.h"
#include "resource.h"

/////////////////////////////////////////////////////
// Surface

static TypeProperty::Info::Node f_blendmethodinfos[] = {
   TypeProperty::Info::Node( "Replace", "Replace" ),
   TypeProperty::Info::Node( "Add", "Add" ),
   TypeProperty::Info::Node( "Subtract", "Subtract" ),
   TypeProperty::Info::Node( "Multiply", "Multiply" ),
};

static TypeProperty::Info::Node f_cullfacemodeinfos[] = {
   TypeProperty::Info::Node( "None", "None" ),
   TypeProperty::Info::Node( "Front", "Front" ),
   TypeProperty::Info::Node( "Back", "Back" ),
};

TypeProperty::Info Surface::m_nBlendMethodInfo( "Blend Method", "BlendMethod", f_blendmethodinfos, sizeof(f_blendmethodinfos)/sizeof(TypeProperty::Info::Node), Surface::BM_REPLACE );
   FloatProperty::Info Surface::m_fBlendFactorInfo( "Factor", "Factor", 1.0f, 0.0f, 1.0f, 100.0f, FloatProperty::Info::FPT_PERCENT );
ColorProperty::Info Surface::m_cAmbientColorInfo( "Ambient", "Ambient", RGBFloat(0.0f), RGBFloat(-MAX_FLOAT), RGBFloat(MAX_FLOAT) );
ColorProperty::Info Surface::m_cDiffuseColorInfo( "Diffuse", "Diffuse", RGBFloat(0.5f), RGBFloat(-MAX_FLOAT), RGBFloat(MAX_FLOAT) );
ColorProperty::Info Surface::m_cSpecularColorInfo( "Specular", "Specular", RGBFloat(0.0f), RGBFloat(-MAX_FLOAT), RGBFloat(MAX_FLOAT) );
ColorProperty::Info Surface::m_cEmissionColorInfo( "Emission", "Emission", RGBFloat(0.0f), RGBFloat(-MAX_FLOAT), RGBFloat(MAX_FLOAT) );
FloatProperty::Info Surface::m_fTransparencyInfo( "Transparency", "Transparency", 0.0f, 0.0f, 1.0f, 100.0f, FloatProperty::Info::FPT_PERCENT );
FloatProperty::Info Surface::m_fShininessInfo( "Shininess", "Shininess", 20.0f, 0.0f, 128.0f );
Vector2Property::Info Surface::m_vNormalOffsetInfo( "Normal Offset", "NormalOffset", 0.0f, -MAX_FLOAT, MAX_FLOAT, 1.0f, Vector2Property::Info::FPT_ANGLE );
TypeProperty::Info Surface::m_nCullFaceModeInfo( "Cull Faces", "CullFaces", f_cullfacemodeinfos, sizeof(f_cullfacemodeinfos)/sizeof(TypeProperty::Info::Node), Surface::CFM_BACK );

GLuint Surface::m_glSurfaceIconCallList;

IMPLEMENT_SNIPEOBJECT_CREATE( Surface )

Surface::Surface()
{
   m_nBlendMethod.Init( &m_nBlendMethodInfo, this );
      m_fBlendFactor.Init( &m_fBlendFactorInfo, &m_nBlendMethod );
   m_cAmbientColor.Init( &m_cAmbientColorInfo, this );
   m_cDiffuseColor.Init( &m_cDiffuseColorInfo, this );
   m_cSpecularColor.Init( &m_cSpecularColorInfo, this );
   m_cEmissionColor.Init( &m_cEmissionColorInfo, this );
   m_fTransparency.Init( &m_fTransparencyInfo, this );
   m_fShininess.Init( &m_fShininessInfo, this );
   m_vNormalOffset.Init( &m_vNormalOffsetInfo, this );
   m_nCullFaceMode.Init( &m_nCullFaceModeInfo, this);

   for (UINT i=1; i<GetNumProperties(); i++)
      m_propertyarray[i]->SetSet( FALSE );

   if (!m_glSurfaceIconCallList)
   {
      AFX_MANAGE_STATE( AfxGetAppModuleState())
      m_glSurfaceIconCallList = GetMainFrame()->CreateCallListFromGeoResource( IDR_SPHERE_GEO );
   }
}

Surface::~Surface()
{
}

SurfaceDriver *Surface::GetSurfaceDriver(Property *pProperty)
{
   for (UINT nSurfaceDriverIndex = 0; nSurfaceDriverIndex < GetNumSurfaceDrivers(); nSurfaceDriverIndex++)
   {
      SurfaceDriver *pSurfaceDriver = (SurfaceDriver *)GetSurfaceDriverAt(nSurfaceDriverIndex);
      if (pSurfaceDriver->GetProperty() == pProperty)
         return pSurfaceDriver;
   }
   return NULL;
}

void Surface::Bind()
{
   //glPushAttrib( GL_LIGHTING_BIT | GL_POLYGON_BIT );

   GLfloat params[4];
   params[3] = 1.0f - (m_fTransparency / 100.0f);

   params[0] = m_cAmbientColor.GetValue().GetRed(); params[1] = m_cAmbientColor.GetValue().GetGreen(); params[2] = m_cAmbientColor.GetValue().GetBlue();
   glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, params );

   params[0] = m_cDiffuseColor.GetValue().GetRed(), params[1] = m_cDiffuseColor.GetValue().GetGreen(), params[2] = m_cDiffuseColor.GetValue().GetBlue();
   glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, params );

   params[0] = m_cSpecularColor.GetValue().GetRed(); params[1] = m_cSpecularColor.GetValue().GetGreen(); params[2] = m_cSpecularColor.GetValue().GetBlue();
   glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, params );

   //params[0] = m_cEmissionColor.GetValue().GetRed(); params[1] = m_cEmissionColor.GetValue().GetGreen(); params[2] = m_cEmissionColor.GetValue().GetBlue();
   //glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, params );

   glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, m_fShininess );

   switch (m_nCullFaceMode)
   {
      case CFM_NONE:
         glDisable(GL_CULL_FACE);
         break;
      case CFM_FRONT:
         glCullFace( GL_FRONT );
         glEnable( GL_CULL_FACE );
         break;
      case CFM_BACK:
         glCullFace( GL_BACK );
         glEnable( GL_CULL_FACE );
         break;
   }
}

void Surface::UnBind()
{
   //glPopAttrib();
}

int Surface::AddSurfaceDriver( SurfaceDriver *pSurfaceDriver, Property *pProperty )
{
   ASSERT( pSurfaceDriver );

   int nIndex = AddChildAtTail( (HierObject*)pSurfaceDriver );
   pSurfaceDriver->m_pProperty.StoreValue( pProperty );
   DestroyCompositedSurfaces();
   return nIndex;
}

void Surface::DrawFacesBase( const Matrix44 &viewmatrix, SurfaceDriver *pSurfaceDriver/*=NULL*/ )
{
   for (UINT i=0; i<GetNumFaces(); i++)
   {
      Poly *pPoly = GetFaceAt(i);
      if (!pPoly->IsDraw())
         continue;

      pPoly->Poly::Draw( viewmatrix, FALSE, pSurfaceDriver ); // go straight to the poly version
   }
}

void Surface::DrawFaces( const Matrix44 &viewmatrix )
{
   Bind();

   DrawFacesBase( viewmatrix );

   for (UINT nSurfaceDriverIndex=0; nSurfaceDriverIndex<GetNumSurfaceDrivers(); nSurfaceDriverIndex++)
   {
      SurfaceDriver *pSurfaceDriver = GetSurfaceDriverAt( nSurfaceDriverIndex );
      if (pSurfaceDriver->m_pProperty!=&m_cDiffuseColor || !pSurfaceDriver->IsValid())
         continue;

      pSurfaceDriver->Bind();
         DrawFacesBase( viewmatrix, pSurfaceDriver );
      pSurfaceDriver->UnBind();
   }

   UnBind();
}

void Surface::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   SnipeObjectHeader header( this, bWriteHeader, bs );

   DataHeader dataheader( bs, "Faces" );

   UINT nNumFaces = GetNumFaces();
   bs << mar << nNumFaces << nl;

   for (UINT nIndex=0; nIndex<nNumFaces; nIndex++)
   {
      Poly *pPoly = GetFaceAt( nIndex );
      if (pPoly)
         bs << mar << pPoly->GetID() << nl;
   }

   HierObject::Save( bs );
}

BOOL SurfaceContainer::Load( BaseStream &bs )
{
#ifdef _DEBUG
   OutputWindowTimer timer( "Load Surfaces" );
#endif // _DEBUG

   return ContainerObject::Load( bs );
}

BOOL Surface::ParseData( BaseStream &bs, const DataHeader &dataheader )
{
   if (dataheader.m_strKey == "Faces")
   {
      bs.ReadMargin();

      UINT nNumFaces;
      bs >> nNumFaces;

      PolyModel *pPolyModel = (PolyModel *)GetParent()->GetParent();
      for (UINT nIndex=0; nIndex<nNumFaces; nIndex++)
      {
         bs.ReadMargin();

         UINT nPolyID;
         bs >> nPolyID;
         Poly *pPoly = pPolyModel->FindPoly( nPolyID );

         if (pPoly)
            AddFace( pPoly );
         else
            g_outputWindow << "<W>Warning: Face(" << nPolyID << ") not found.  Removing from surface " << GetTitle() << ".\n";
      }
      return TRUE;
   }
   else
      return HierObject::ParseData( bs, dataheader );
}

/////////////////////////////////////////////////////
// SurfaceContainer

IMPLEMENT_SNIPEOBJECT_CREATE( SurfaceContainer )

SurfaceContainer::SurfaceContainer()
{
   m_strName = "Surfaces";
}

SurfaceContainer::~SurfaceContainer()
{
}

/////////////////////////////////////////////////////
// SurfaceDriver

UINT Surface::AddFace( Poly *pFace )
{
   UINT nIndex = m_facearray.Add( pFace );
   pFace->AddSurface( this );
   return nIndex;
}

BOOL Surface::RemoveFace( Poly *pFace, BOOL bRemoveSurfaceFromFaceToo/*=TRUE*/ )
{
   UINT nIndex = FindFace( pFace );
   if (nIndex == -1)
      return FALSE;
   m_facearray.RemoveAt( nIndex );
   
   // don't remove the surface from the face for undo reasons
   if (bRemoveSurfaceFromFaceToo)
      pFace->RemoveSurface( this );
   
   pFace->SetCompositedSurface( NULL );
   GetModel()->SetCompositedSurfacesDirty( TRUE );
   
   return TRUE;
}

BOOL Surface::RemoveAllFaces()
{
   if (GetNumFaces())
   {
      for (UINT i=0; i<GetNumFaces(); i++)
      {
         Poly *pFace = GetFaceAt( i );
         pFace->RemoveSurface( this );
         pFace->SetCompositedSurface( NULL );
      }
      m_facearray.RemoveAll();
      GetModel()->SetCompositedSurfacesDirty( TRUE );
   }
   return TRUE;
}

BOOL Surface::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg) {
      case NOTIFYOBJ_REMOVEDFROMPROJECT:
         DestroyCompositedSurfaces();
         for (UINT i=0; i<GetNumFaces(); i++)
         {
            Poly *pPoly = GetFaceAt(i);
            pPoly->RemoveSurface( this );
         }
         break;
      case NOTIFYOBJ_ADDEDTOPROJECT:
         for (UINT i=0; i<GetNumFaces(); i++)
         {
            Poly *pPoly = GetFaceAt(i);
            pPoly->AddSurface( this );
         }
         break;
      case NOTIFYOBJ_VALUECHANGED:
         NotifyNodes( NOTIFYOBJ_ICONCHANGED, 0, (LPARAM)this );
         DirtyCompositedSurfaces();
         break;
   }
   return HierObject::OnObjectNotify( msg, wParam, lParam );
}

int Surface::GetItemHeight()
{
   static int nItemHeight = min(128, max(16, GetApp().GetProfileInt( "General", "Surface Icon Size", 48 )));
   return nItemHeight;
}

BoundingBox Surface::GetBoundingBox() const
{
   BoundingBox box;

   for (UINT i=0; i<GetNumFaces(); i++)
   {
      Poly *pPoly = GetFaceAt( i );

      pPoly->PushBounds( box );
   }

   return box;
}

void Surface::DrawTreeItem( CGLControl *pItem, float x/*=0.0f*/ )
{
   glClear( GL_DEPTH_BUFFER_BIT );
   //if (m_bounds.IsEmpty())
      m_bounds = GetBoundingBox();
   float fLargestDimension = m_bounds.GetLargestDimension() * 1.2f;

   GLint viewport[4];
   glGetIntegerv( GL_VIEWPORT, viewport );
   CRect rcViewport = pItem->GetViewportRect();
   CRect rcClient;
   pItem->m_pGLRenderWnd->GetClientRect( &rcClient );
   float fIconSize = min(GetItemHeight()-2, pItem->m_szControl.cy);
   glViewport( rcViewport.left+x, rcClient.bottom-rcViewport.top-fIconSize-1, fIconSize, fIconSize );
      glMatrixMode( GL_PROJECTION );
      glPushMatrix();
         Vector vCenter = m_bounds.CalculateCenterPoint();
         Vector vEye( 0.0f, 0.0f, fLargestDimension );

         float fRotation = 40.0f;
         float fAngle = -20.0f;
         CGLBaseView *pBaseView = GetMainInterface()->GetCurrentView();
         if (pBaseView && pBaseView->IsKindOf( SVT_CGLModelView ))
         {
            fRotation = ((CGL2DView *)pBaseView)->GetCamera()->m_transform.m_vRotate.GetValue().y;
            fAngle = ((CGL2DView *)pBaseView)->GetCamera()->m_transform.m_vRotate.GetValue().x;
            fAngle = min(89.9f, max( -89.9f, fAngle ));
         }
         Matrix44 matrix, temp;
         matrix.RotationMatrix( fRotation*(float)DEGTORAD, 0.0f, 1.0f, 0.0f );
         temp.RotationMatrix( fAngle*(float)DEGTORAD, 1.0f, 0.0f, 0.0f );
         matrix *= temp;
         vEye *= matrix;
         vEye += vCenter;
         float fNear = fLargestDimension * 0.25f;
         float fFar = fLargestDimension * 2.0f;
         glLoadIdentity();
         gluPerspective( 45.0f, 1.0f, fNear, fFar );

         glMatrixMode( GL_MODELVIEW );
         glPushMatrix();
            glLoadIdentity();

            glPushAttrib( GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_POLYGON_BIT );
               glEnable( GL_DEPTH_TEST );
               glEnable( GL_LIGHTING );
               glEnable( GL_LIGHT0 );
               GLfloat params[4] = { 0, 0, 1, 0 };
               glLightfv( GL_LIGHT0, GL_POSITION, params );
               params[0] = params[1] = params[2] = params[3] = 1.0f;
               glLightfv( GL_LIGHT0, GL_AMBIENT, params );
               glLightfv( GL_LIGHT0, GL_DIFFUSE, params );

               gluLookAt( vEye.x, vEye.y, vEye.z, vCenter.x, vCenter.y, vCenter.z, 0, 1, 0 );
#if 1
               GLdouble glViewMatrix[16];
               glGetDoublev( GL_MODELVIEW_MATRIX, glViewMatrix );
               Matrix44 viewmatrix( glViewMatrix, TRUE );
               DrawFaces( viewmatrix );
#else
               Bind();
               glCallList( m_glSurfaceIconCallList );
               glEnable( GL_POLYGON_SMOOTH );
               glCallList( m_glSurfaceIconCallList );

               for (UINT nSurfaceDriverIndex = 0; nSurfaceDriverIndex < GetNumSurfaceDrivers(); nSurfaceDriverIndex++)
               {
                  SurfaceDriver *pSurfaceDriver = GetSurfaceDriverAt( nSurfaceDriverIndex );
                  if (!pSurfaceDriver->IsValid() || pSurfaceDriver->m_pProperty.GetValue()!=&m_cDiffuseColor)
                     continue;
                  pSurfaceDriver->Bind( this );
                     glEnable( GL_CULL_FACE );
                     glCullFace( GL_BACK );
                     glTexCoord2f( 0.5f, 0.5f );
                     glCallList( m_glSurfaceIconCallList );
                     glEnable( GL_POLYGON_SMOOTH );
                     glCallList( m_glSurfaceIconCallList );
                  pSurfaceDriver->UnBind();
               }
               UnBind();
#endif
            glPopAttrib();
         glPopMatrix();
         glMatrixMode( GL_PROJECTION );
      glPopMatrix();
   glViewport( viewport[0], viewport[1], viewport[2], viewport[3] );

   //static GLImageList glImageListBackground;
   //if (glImageListBackground.IsEmpty())
   //   glImageListBackground.Create( IDB_GROUPBOX, CRect(10,10,20,20) );
   //glImageListBackground.DrawDiced( CRect( CPoint(x+1, 1), CSize(fIconSize, fIconSize) ) );

   glColor3f( 0.1f, 0.1f, 0.1f );
   pItem->DrawText( x + fIconSize + 4, 12, GetTitle() );

   String strBlendMethod, strBlendFactor;
   m_nBlendMethod.GetValueString( strBlendMethod );
   m_fBlendFactor.GetValueString( strBlendFactor );
   glColor3f( 0.4f, 0.4f, 0.4f );
   pItem->DrawText( x + fIconSize + 4 + 8, 12 + 14, strBlendMethod + " : " + strBlendFactor );
}

void Surface::DirtyCompositedSurfaces()
{
   PolyModel *pPolyModel = GetModel();
   if (!pPolyModel)
      return;

   for (POSITION pos=pPolyModel->GetHeadCompositedSurfacePosition(); pos; )
   {
      CompositedSurface *pCompositedSurface = pPolyModel->GetNextCompositedSurface( pos );
      for (UINT i=0; i<pCompositedSurface->GetNumSurfaces(); i++)
      {
         Surface *pSurface = pCompositedSurface->GetSurfaceAt( i );
         if (pSurface == this)
         {
            pCompositedSurface->SetDirty();
            break;
         }
      }
   }
}

void Surface::DestroyCompositedSurfaces()
{
   PolyModel *pPolyModel = GetModel();
   if (!pPolyModel)
      return;

   for (POSITION pos=pPolyModel->GetHeadCompositedSurfacePosition(); pos; )
   {
      CompositedSurface *pCompositedSurface = pPolyModel->GetNextCompositedSurface( pos );
      for (UINT i=0; i<pCompositedSurface->GetNumSurfaces(); i++)
      {
         Surface *pSurface = pCompositedSurface->GetSurfaceAt( i );
         if (pSurface == this)
         {
            pPolyModel->RemoveCompositedSurface( pCompositedSurface );
            delete pCompositedSurface;
            break;
         }
      }
   }

   pPolyModel->SetCompositedSurfacesDirty( TRUE );
}

template<>
UINT AFXAPI HashKey( SurfaceArray *pKey )
{
   UINT nKey = pKey->m_nNumSurfaces;

   for (UINT i=0; i<pKey->m_nNumSurfaces; i++)
      nKey += HashKey( pKey->m_pSurfaces[i] );

   return nKey;
}

//template<class TYPE, class ARG_TYPE>
//template<> BOOL AFXAPI CompareElements<Surface *, Surface *>( SurfaceArray **pElement1, SurfaceArray **pElement2)
//{
//   return *pElement1 == *pElement2;
//}

//template<class TYPE, class ARG_TYPE>
//BOOL AFXAPI CompareElements(const TYPE* pElement1, const ARG_TYPE* pElement2)
//{
//   ASSERT(AfxIsValidAddress(pElement1, sizeof(TYPE), FALSE));
//   ASSERT(AfxIsValidAddress(pElement2, sizeof(ARG_TYPE), FALSE));
//
//   return *pElement1 == *pElement2;
//}
