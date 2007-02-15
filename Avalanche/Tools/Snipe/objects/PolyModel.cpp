#include "stdafx.h"
#include "PatchModel.h"
#include "Project.h"
#include "Bone.h"
#include "Selection.h"
#include <gl/gl.h>
#include "UV.h"
#include "../Utility/BoundingBox.h"
#include "GUI/GLModelView.h"
#include "GUI/SnipeDocTemplate.h"
#include "GUI/Snipe.h"
#include "Interface/Implement.h"
#include "Edge.h"
#include "RTVertex.h"
#include "../Utility/IntIntHash.h"
#include "../Utility/IntHash.h"
#include "resource.h"
#include "Rotoscope.h"
#include "UVPoly.h"
#include "ColorVertexSet.h"
#include "ColorPoly.h"

FileInfoProperty::Info PolyModel::m_fileinfoinfo( "File Information", "FileInfo", "Untitled.snipemodel", "Model File (*.snipemodel)|*.snipemodel||", "Model" );
Property::Info PolyModel::m_statisticsinfo( "Statistics", "Statistics" );
   StatisticProperty::Info PolyModel::m_nVerticesStatInfo( "Vertices", "Vertices" );
   StatisticProperty::Info PolyModel::m_nVertexNormalsStatInfo( "Vertex Normals", "VertexNormals" );
   StatisticProperty::Info PolyModel::m_nEdgesStatInfo( "Edges", "Edges" );
   StatisticProperty::Info PolyModel::m_nFacesStatInfo( "Faces", "Faces" );
   StatisticProperty::Info PolyModel::m_nSurfacesStatInfo( "Surfaces", "Surfaces" );
   StatisticProperty::Info PolyModel::m_nBonesStatInfo( "Bones", "Bones" );
   StatisticProperty::Info PolyModel::m_nVertexArraySizeStatInfo( "GL Vertex Array Size", "GLVertexArraySize" );
Property::Info PolyModel::m_surfaceinfo( "Base Surface", "BaseSurface" );

RGBFloat PolyModel::m_fBoneColors[16] = {
   RGBFloat(1.0f, 0.0f, 0.0f),//Red
   RGBFloat(0.0f, 1.0f, 0.0f),//Green
   RGBFloat(0.0f, 0.0f, 1.0f),//Blue
   RGBFloat(1.0f, 1.0f, 0.0f),//Yellow
   RGBFloat(1.0f, 0.0f, 1.0f),//Purple
   RGBFloat(1.0f, 0.5f, 0.0f),//Orange
 
   RGBFloat(1.0f, 0.7f, 0.7f),//LtRed
   RGBFloat(0.7f, 1.0f, 0.7f),//LtGreen
   RGBFloat(0.7f, 0.7f, 1.0f),//LtBlue
   RGBFloat(1.0f, 0.8f, 0.5f),//LtOrange
   RGBFloat(1.0f, 1.0f, 0.5f),//LtYellow

   RGBFloat(0.5f, 0.0f, 0.0f),//DkRed
   RGBFloat(0.0f, 0.5f, 0.0f),//DkGreen
   RGBFloat(0.0f, 0.0f, 0.5f),//DkBlue
   RGBFloat(0.7f, 0.3f, 0.5f),//DkOrange
};

IMPLEMENT_SNIPEOBJECT_CREATE( PolyModel )
IMPLEMENT_COMMANDOBJECT( PolyModel )

PolyModel *BasicProperty<PolyModel *>::m_prevvalue;

String GetNumVerticesStat( void *pData )
{
   PolyModel *pModel = (PolyModel *)pData;
   return pModel->GetNumVertices();
}

String GetNumVertexNormalsStat( void *pData )
{
   PolyModel *pModel = (PolyModel *)pData;
   return pModel->GetNumVertexNormals();
}

String GetNumEdgesStat( void *pData )
{
   PolyModel *pModel = (PolyModel *)pData;
   return pModel->GetNumEdges();
}

String GetNumFacesStat( void *pData )
{
   PolyModel *pModel = (PolyModel *)pData;
   return pModel->GetNumPolys();
}

String GetNumSurfacesStat( void *pData )
{
   PatchModel *pModel = (PatchModel *)pData;
   return pModel->GetNumCompositedSurfaces();
}

String GetNumBonesStat( void *pData )
{
   PolyModel *pModel = (PolyModel *)pData;
   return pModel->GetNumBones();
}

String GetVertexArraySizeStat( void *pData )
{
   PolyModel *pModel = (PolyModel *)pData;
   return (String)(pModel->GetNumRTVertices() * sizeof(RTVertex)) + " bytes";
}

PolyModel::PolyModel()
{
   m_vertexlist.SetOwner(this);
   m_vertexnormallist.SetOwner(this);
   m_edgelist.SetOwner(this);
   m_polylist.SetOwner(this);

   m_fileinfo.Init( &m_fileinfoinfo, this );
   m_statistics.Init( &m_statisticsinfo, this );
      m_nVerticesStat.Init( &m_nVerticesStatInfo, &m_statistics, ::GetNumVerticesStat, this );
      m_nVertexNormalsStat.Init( &m_nVertexNormalsStatInfo, &m_statistics, ::GetNumVertexNormalsStat, this );
      m_nEdgesStat.Init( &m_nEdgesStatInfo, &m_statistics, ::GetNumEdgesStat, this );
      m_nFacesStat.Init( &m_nFacesStatInfo, &m_statistics, ::GetNumFacesStat, this );
      m_nSurfacesStat.Init( &m_nSurfacesStatInfo, &m_statistics, ::GetNumSurfacesStat, this );
      m_nBonesStat.Init( &m_nBonesStatInfo, &m_statistics, ::GetNumBonesStat, this );
      m_nVertexArraySizeStat.Init( &m_nVertexArraySizeStatInfo, &m_statistics, ::GetVertexArraySizeStat, this );
   m_surface.Init( &m_surfaceinfo, this );
   for (int i=1; i<m_DefaultSurface.GetPropertyArray()->GetCount(); i++)
   {
      Property *pSurfaceProperty = m_DefaultSurface.GetPropertyArray()->GetAt( i );
      pSurfaceProperty->SetSet( TRUE );
      pSurfaceProperty->SetParent( NULL );
      m_surface.AddProperty( pSurfaceProperty );
   }
   m_DefaultSurface.SetParent( this );

   m_bRebuildBufferObjects = TRUE;
   m_pTexCoordsContainer = NULL;
   m_pBoneContainer = NULL;
   m_pSurfaceContainer = NULL;
   m_pRotoscopeContainer = NULL;

   m_glCallList = 0;
   m_glVertexIndiceCallList = 0;
   m_glFaceIndiceCallList = 0;
   m_glEdgeIndiceCallList = 0;

   m_pRTVertices = NULL;
   m_nNumRTVertices = 0;
   m_pVisibleVertices = NULL;
   m_nNumVisibleVertices = 0;
   m_pLockedVertices = NULL;
   m_nNumLockedVertices = 0;

   m_pRTEdges = NULL;
   m_nNumRTEdges = 0;
   m_nNumHardEdges = 0;
   m_nNumBorderEdges = 0;

   m_nPolyModelFlags = 0;
   SetCompositedSurfacesDirty( TRUE );

   m_bBoundingSphereDirty = TRUE;
   m_nNumBones = 0;

   m_pCurrentInstance = NULL;
   m_fDrawOpacity = 1.0f;

   m_pColorVertexSetContainer = NULL;
}

PolyModel::~PolyModel()
{
   if (m_pRTVertices)
      FreeRTVertices();

   for (POSITION pos=GetHeadCompositedSurfacePosition(); pos; )
   {
      CompositedSurface *pCompositedSurface = GetNextCompositedSurface( pos );
      delete pCompositedSurface;
   }
}

void PolyModel::PreDestruct()
{
   m_commandlist.Empty();
   SceneObject::PreDestruct();
}

UINT PolyModel::GetDefaultMenuItemID()
{
   return ID_OBJECT_EDIT;
}

CSnipeDocTemplate *PolyModel::GetDocTemplate()
{
   return GetApp().GetModelDocTemplate();
}

Vertex *PolyModel::CreateVertex( UINT nID/*=-1*/ ) const
{
   return Vertex::CreateWithID( nID );
}

Edge *PolyModel::CreateEdge( UINT nID/*=-1*/ ) const
{
   return Edge::CreateWithID( nID );
}

Poly *PolyModel::CreatePoly( UINT nID/*=-1*/ ) const
{
   return Poly::CreateWithID( nID );
}

RGBFloat PolyModel::NewBoneColor()
{
   return m_fBoneColors[GetNumBones() % 16];
}

BoneContainer *PolyModel::CreateBoneContainer()
{
   if (!m_pBoneContainer)
   {
      m_pBoneContainer = BoneContainer::Create();
      AddChildAtTail( m_pBoneContainer );
   }
   return m_pBoneContainer;
}

void PolyModel::AddBone( TransformableObject *pBone, TransformableObject *pParent )
{
   ASSERT(pBone);

   CreateBoneContainer();

   HierObject *pHierParent = pParent;
   if (pParent == this)
      pHierParent = m_pBoneContainer;

   pHierParent->AddChildAtTail( pBone );

   pBone->GetBoneColor()->StoreValue(NewBoneColor());
}

TransformableObject *PolyModel::FindBone( const char *name )
{
   BoneContainer *pBoneContainer = GetBoneContainer();
   for (UINT i=0; i<pBoneContainer->GetNumTransformableObjects(); i++)
   {
      TransformableObject *pChild = pBoneContainer->GetTransformableObjectAt( i );
      TransformableObject *pTransformableObject = FindBone(name,pChild);
      if (pTransformableObject)
         return pTransformableObject;
   }
   return NULL;
}

TransformableObject *PolyModel::FindBone( const char *name, TransformableObject *pTransformableObject )
{
   if (pTransformableObject->IsBone())
   {
      if (stricmp(name,pTransformableObject->GetMatchName().Get())==0)
         return pTransformableObject;
   }
 
   for (UINT i=0; i<pTransformableObject->GetNumTransformableObjects(); i++)
   {
      TransformableObject *pChild = pTransformableObject->GetTransformableObjectAt( i );
      TransformableObject *pTransformableObject = FindBone(name,pChild);
      if (pTransformableObject)
         return pTransformableObject;
   }
   return NULL;
}

// HierObject overrides
SimpleComponentList *PolyModel::FindComponentList( const String &strComponentType )
{
   if (strComponentType == "vertex")
      return (SimpleComponentList *)&m_vertexlist;
   else if (strComponentType == "face")
      return (SimpleComponentList *)&m_polylist;
   else if (strComponentType == "edge")
      return (SimpleComponentList *)&m_edgelist;

   return SceneObject::FindComponentList(strComponentType);
}

void PolyModel::DirtyDependentFaces()
{
   for (POSITION pos=m_polylist.GetHeadPosition(); pos; )
   {
      Poly *pPoly = m_polylist.GetNext( pos );
      pPoly->SetDirty();
   }
}

void PolyModel::DirtyDependentEdges()
{
   for (POSITION pos=m_edgelist.GetHeadPosition(); pos; )
   {
      Edge *pEdge = m_edgelist.GetNext( pos );
      pEdge->SetDirty();
   }
}

BoundingBox PolyModel::CalculateBoundingBox(CoordinateSpace nSpace, CGLModelView *pView/*=NULL*/)
{
   BoundingBox bbox;
   Vector position;

   for (POSITION pos=GetHeadVertexPosition(); pos; )
   {
      Vertex *pVertex = GetNextVertex( pos );

      if (!pVertex->IsVisible())
         continue;

      if (nSpace == Space_Object)
         position = pVertex->GetPos();
      else if (nSpace == Space_World)
         position = pVertex->GetWorldPos(this);
      else if (nSpace == Space_View)
         position = pVertex->GetViewPos(this, pView);

      bbox.Push( position );
   }
   return bbox;
}

TexCoordsContainer *PolyModel::CreateTexCoordsContainer()
{
   if (!m_pTexCoordsContainer)
   {
      m_pTexCoordsContainer = TexCoordsContainer::Create();
      AddChildAtTail(m_pTexCoordsContainer);
   }
   return m_pTexCoordsContainer;
}

TexCoords *PolyModel::AddTexCoords(const char *name/*=NULL*/)
{
   CreateTexCoordsContainer();
   TexCoords *pTexCoords = TexCoords::Create(); 
   m_pTexCoordsContainer->AddChildAtTail( pTexCoords );
   if (name)
      pTexCoords->SetName(name);
   return pTexCoords;
}

ColorVertexSetContainer *PolyModel::CreateColorVertexSetContainer()
{
   if (!m_pColorVertexSetContainer)
   {
      m_pColorVertexSetContainer = ColorVertexSetContainer::Create();
      AddChildAtTail(m_pColorVertexSetContainer);
   }
   return m_pColorVertexSetContainer;
}

ColorVertexSet *PolyModel::AddColorVertexSet(const char *name/*=NULL*/)
{
   CreateColorVertexSetContainer();
   ColorVertexSet *pColorVertexSet = ColorVertexSet::Create(); 
   m_pColorVertexSetContainer->AddChildAtTail( pColorVertexSet );
   if (name)
      pColorVertexSet->SetName(name);
   return pColorVertexSet;
}

Edge *PolyModel::AddEdgeBetween( Vertex *pV0, Vertex *pV1, Edge **pOutReversedEdge/*=NULL*/, Edge *pPrevEdge/*=EDGE_FINDCONTINUOUS*/, Edge *pNextEdge/*=EDGE_FINDCONTINUOUS*/)
{
   if (pOutReversedEdge)
      *pOutReversedEdge = NULL;

   return AddEdge( pV0, pV1 );
}

Edge::DeleteInfo *PolyModel::AddEdge( Vertex *pV0, Vertex *pV1, Edge *pPrevEdge, Edge *pNextEdge )
{
   if (Edge::FindEdge( pV0, pV1 )) // Don't let adjacent vertices in a poly add an edge
      return NULL;

   Poly *pPoly = Poly::FindPoly( pV0, pV1 );

   Edge::DeleteInfo *pInfo;

   if (pPoly)
      pInfo = pPoly->SplitPoly( pV0, pV1, pPrevEdge, pNextEdge );
   else 
   {// Dangles
      Edge *pReversedEdge = NULL;
      Edge *pEdge = AddEdgeBetween( pV0, pV1, &pReversedEdge, pPrevEdge, pNextEdge  );

      pInfo = SNEW Edge::DeleteInfo( pEdge );
      pInfo->SetReversedEdge(pReversedEdge);
   }
   return pInfo;
}

int PolyModel::AddEdges( SnipeObjectList *pVertices, Edge::DeleteInfo **&pOutEdgeDeleteInfos, Edge *pFirstPrevEdge/*=EDGE_TERMINATE*/, Edge *pLastNextEdge/*=EDGE_TERMINATE*/ )
{
   ASSERT (pVertices );

   if (pVertices->GetCount() < 2 )
      return 0;

   int nArraySize = pVertices->GetCount()-1;
   pOutEdgeDeleteInfos = SNEW Edge::DeleteInfo *[nArraySize];

   Vertex *pV0 = NULL;
   Vertex *pV1 = NULL;
   Edge *pPrevEdge = pFirstPrevEdge;
   Edge *pNextEdge = EDGE_TERMINATE;

   int nAdded = 0;
   for (POSITION pos = pVertices->GetHeadPosition(); pos; )
   {
      pV0 = pV1;
      pV1 = (Vertex *)pVertices->GetNext( pos );

      if (!pV0)
         continue;

      if (!pos) // If pos is NULL it is the last vertex to add, lets use the passed in value to determine if the edge should be terminated or continuous
         pNextEdge = pLastNextEdge;

      Edge *pEdge = Edge::FindEdge( pV0, pV1 );
      if (pEdge) // Don't let adjacent vertices in a poly add an edge
      {
         pPrevEdge = pEdge;
         continue;
      }

      Poly *pPoly = Poly::FindPoly( pV0, pV1 );

      if (pPoly)
         pOutEdgeDeleteInfos[nAdded] = pPoly->SplitPoly( pV0, pV1, pPrevEdge, pNextEdge );
      else 
      {// Dangles
         Edge *pReversedEdge = NULL;

         Edge *pEdge = AddEdgeBetween( pV0, pV1, &pReversedEdge, pPrevEdge, pNextEdge  );
         ASSERT(pEdge);

         pOutEdgeDeleteInfos[nAdded] = SNEW Edge::DeleteInfo( pEdge );
         pOutEdgeDeleteInfos[nAdded]->SetReversedEdge(pReversedEdge);

         Vertex *pV0Mirror = (Vertex *)pV0->GetMirrorObject();
         Vertex *pV1Mirror = (Vertex *)pV1->GetMirrorObject();
         Edge *pMirrorPrevious = pPrevEdge;
         Edge *pMirrorNext = pNextEdge;
         if (pMirrorPrevious != EDGE_TERMINATE && pMirrorPrevious != EDGE_FINDCONTINUOUS)
            pMirrorPrevious = (Edge *)pMirrorPrevious->GetMirrorObject();
         if (pMirrorNext != EDGE_TERMINATE && pMirrorNext != EDGE_FINDCONTINUOUS)
            pMirrorNext = (Edge *)pMirrorNext->GetMirrorObject();

         if (pV0Mirror && pV1Mirror)
         {
            Edge *pMirrorEdge = AddEdgeBetween( pV0Mirror, pV1Mirror, &pReversedEdge, pMirrorPrevious, pMirrorNext  );
            if (pMirrorEdge)
            {
               pEdge->SetMirrorObject(pMirrorEdge);
               pOutEdgeDeleteInfos[nAdded]->m_pMirrorDeleteInfo = SNEW Edge::DeleteInfo( pMirrorEdge );
               ((Edge::DeleteInfo *)pOutEdgeDeleteInfos[nAdded]->m_pMirrorDeleteInfo)->SetReversedEdge(pReversedEdge);
            }
         }
      }

      if (!pOutEdgeDeleteInfos[nAdded])
         continue;

      pPrevEdge = (Edge *)pOutEdgeDeleteInfos[nAdded]->GetSnipeObject();
      nAdded++;
   }

   return nAdded;
}

Component *PolyModel::CreateComponent( SnipeObjectType nSOT )
{
   switch (nSOT)
   {
      case SOT_Vertex:
         return CreateVertex();
      case SOT_Edge:
         return CreateEdge();
      case SOT_Poly:
         return CreatePoly();
      case SOT_VertexNormal:
         return VertexNormal::Create();
   }

   return SceneObject::CreateComponent( nSOT );
}

void PolyModel::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   SnipeObjectHeader header( this, bWriteHeader, bs );

   m_vertexlist.Save( bs, "Vertices" );
   m_edgelist.Save( bs, "Edges" );
   m_polylist.Save( bs, "Faces" );

   SceneObject::Save( bs );
}

BOOL PolyModel::ParseData( BaseStream &bs, const DataHeader &dataheader )
{
   if (dataheader.m_strKey == "Vertices")
   {
#ifdef _DEBUG
      OutputWindowTimer timer( "Load Vertices" );
#endif // _DEBUG
      return m_vertexlist.Load( bs );
   }
   else if (dataheader.m_strKey == "Edges")
   {
#ifdef _DEBUG
      OutputWindowTimer timer( "Load Edges" );
#endif // _DEBUG
      return m_edgelist.Load( bs );
   }
   else if (dataheader.m_strKey == "Faces")
   {
#ifdef _DEBUG
      OutputWindowTimer timer( "Load Faces" );
#endif // _DEBUG
      return m_polylist.Load( bs );
   }
   else
      return SceneObject::ParseData( bs, dataheader );
}

void PolyModel::BuildEdges()
{
   Poly *pPoly = m_polylist.GetHead();
   if (!pPoly || pPoly->GetEdgeAt(0))
      return;

#ifdef _DEBUG
   OutputWindowTimer timer( "Build Edges From Polys" );
#endif // _DEBUG

   for (POSITION pos=m_polylist.GetHeadPosition(); pos; )
   {
      Poly *pPoly = m_polylist.GetNext( pos );
      for (UINT i=0; i<pPoly->GetNumSides(); i++)
         pPoly->GetVertexAt(i)->Reference();
   }

   for (POSITION pos=m_polylist.GetHeadPosition(); pos; )
   {
      Poly *pPoly = m_polylist.GetNext( pos );
      BOOL bResult = pPoly->BuildEdges();

      for (PropertyPoly *pPropertyPoly = pPoly->GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly = pPropertyPoly->GetNextPropertyPoly())
         pPropertyPoly->BuildPropertyEdges();

      if (!bResult)
         pPoly->Isolate();
   }

   for (POSITION pos=m_polylist.GetHeadPosition(); pos; )
   {
      Poly *pPoly = m_polylist.GetNext( pos );
      for (UINT i=0; i<pPoly->GetNumSides(); i++)
         pPoly->GetVertexAt(i)->DeReference();
   }
}

void PolyModel::BuildVertexNormals()
{
   if (!m_vertexnormallist.IsEmpty())
      return;

#ifdef _DEBUG
   OutputWindowTimer timer( "Build Vertex Normals" );
#endif // _DEBUG

   for (POSITION pos=GetHeadVertexPosition(); pos; )
   {
      Vertex *pVertex = GetNextVertex( pos );
      VertexNormal *pVertexNormal = AddVertexNormal();
      for (UINT i=0; i<pVertex->GetNumSurroundingPolys(); i++)
      {
         Poly *pPoly = pVertex->GetSurroundingPolyAt( i );
         int nVertexIndex = pPoly->FindVertexFaceRelativeIndex( pVertex );
         pPoly->SetVertexNormalAt( nVertexIndex, pVertexNormal );
      }
   }

#ifdef _DEBUG
   for (POSITION pos=m_polylist.GetHeadPosition(); pos; )
   {
      Poly *pPoly = m_polylist.GetNext( pos );
      for (UINT i=0; i<pPoly->GetNumSides(); i++)
         ASSERT( pPoly->GetVertexNormalAt( i, FALSE ));
   }
#endif // _DEBUG
}

Edge *PolyModel::FindMatchingManifoldEdge(Edge *pEdge,Poly *pPoly)
{
   Vertex *pFirstVertex = pEdge->GetFirstVertex();
   Vertex *pSecondVertex = pEdge->GetSecondVertex();
   for (POSITION pos=GetHeadVertexPosition(); pos; )
   {
      Vertex *pVertex = GetNextVertex( pos );
      if ((pVertex->GetPos()==pFirstVertex->GetPos())&&(pVertex!=pFirstVertex))
      {
         for (UINT i = 0; i < pVertex->GetNumSurroundingEdges(); i++)
         {
            Edge *pSurroundingEdge = pVertex->GetSurroundingEdgeAt(i);
            Vertex *pSurroundingVertex = pSurroundingEdge->GetSecondVertex();
            if (((pSurroundingEdge->GetFirstVertex()->GetPos()==pSecondVertex->GetPos())||
               (pSurroundingEdge->GetSecondVertex()->GetPos()==pSecondVertex->GetPos()))&&(pSurroundingVertex!=pSecondVertex))
            {
               if (pSurroundingEdge->HasPoly(pPoly))
                  return pSurroundingEdge;
            }
         }
      }
   }
   return NULL;
}

BOOL PolyModel::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VISIBILITYCHANGED:
      case NOTIFYOBJ_LOCKEDCHANGED:
      {
         SnipeObject *pObject = (SnipeObject *)lParam;
         if (IsCache() && pObject->IsKindOf(SOT_Component))
            FreeRTVertices();
         break;
      }
      case NOTIFYOBJ_ADDEDCHILD:
      {
         SnipeObject *pChild = (SnipeObject *)lParam;
         if (pChild->GetSOT() == SOT_BoneContainer)
            m_pBoneContainer = (BoneContainer *)pChild;
         else if (pChild->GetSOT() == SOT_SurfaceContainer)
            m_pSurfaceContainer = (SurfaceContainer *)pChild;
         else if (pChild->GetSOT() == SOT_TexCoordsContainer)
            m_pTexCoordsContainer = (TexCoordsContainer *)pChild;

         if (IsCache() && pChild->GetSOT() != SOT_VertexNormal)
            FreeRTVertices();
         break;
      }
      case NOTIFYOBJ_REMOVEDCHILD:
      {
         SnipeObject *pChild = (SnipeObject *)lParam;
         if (pChild == GetBoneContainer())
            m_pBoneContainer = NULL;
         else if (pChild == GetSurfaceContainer())
            m_pSurfaceContainer = NULL;
         else if (pChild == GetTexCoordsContainer())
            m_pTexCoordsContainer = NULL;

         if (IsCache() && pChild->GetSOT() != SOT_VertexNormal)
            FreeRTVertices();
         break;
      }
      case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if (pProperty->GetParent() == &m_surface)
            m_DefaultSurface.DirtyCompositedSurfaces();
         break;
      }
      case NOTIFYOBJ_ADDEDDESCENDANT:
      case NOTIFYOBJ_REMOVEDDESCENDANT:
      {
         if (IsCache())
            FreeRTVertices();
         break;
      }
 
   }

   return SceneObject::OnObjectNotify( msg, wParam, lParam );
}

RotoscopeContainer *PolyModel::CreateRotoscopeContainer()
{
   if (!m_pRotoscopeContainer)
   {
      m_pRotoscopeContainer = RotoscopeContainer::Create();
      AddChildAtTail(m_pRotoscopeContainer);
   }
   return m_pRotoscopeContainer;
}

BOOL PolyModel::AddRotoscope( Rotoscope *pRotoscope )
{
   CreateRotoscopeContainer()->AddChildAtTail( pRotoscope );
   return TRUE;
}

SurfaceContainer *PolyModel::CreateSurfaceContainer()
{
   if (!m_pSurfaceContainer)
   {
      m_pSurfaceContainer = SurfaceContainer::Create();
      AddChildAtTail(m_pSurfaceContainer);
   }
   return m_pSurfaceContainer;
}

Surface *PolyModel::AddSurface( const char *name/*=NULL*/, int nIndex/*=-1*/, Surface::DeleteInfo **pDeleteInfo/*=NULL*/ )
{
   SurfaceContainer *pSurfaceContainer = CreateSurfaceContainer();
   Surface *pSurface = Surface::Create();
   pSurfaceContainer->InsertChildAt( nIndex, pSurface );
   if (name)
      pSurface->SetName(name);

   if (pDeleteInfo)
      *pDeleteInfo = (Surface::DeleteInfo *)pSurface->AllocDeleteInfo();

   return pSurface;
}

void PolyModel::RemoveSurface(Surface *pSurface)
{
   SurfaceContainer *pSurfaceContainer = GetSurfaceContainer();
   pSurfaceContainer->RemoveSurface(pSurface);
}

CompositedSurface *PolyModel::FindCompositedSurface( Poly *pPoly )
{
   static Surface *pSurfaces[256];
   static SurfaceArray surfacearray( pSurfaces, 0 );

   pSurfaces[0] = &m_DefaultSurface;
   surfacearray.m_nNumSurfaces = 1;

   if (m_pSurfaceContainer)
   {
      for (UINT i=0; i<m_pSurfaceContainer->GetNumSurfaces(); i++)
      {
         Surface *pSurface = m_pSurfaceContainer->GetSurfaceAt( i );
         if (pSurface->FindFace( pPoly ) != -1)
            pSurfaces[ surfacearray.m_nNumSurfaces++ ] = pSurface;
      }
   }

   SurfaceArray *pSurfaceArray = &surfacearray;
   CompositedSurface *pCompositedSurface;
   if (!m_compositedsurfaces.Lookup( pSurfaceArray, pCompositedSurface ))
   {
      pCompositedSurface = SNEW CompositedSurface( this );
      pCompositedSurface->m_SurfaceArray = surfacearray;
      pSurfaceArray = &pCompositedSurface->m_SurfaceArray;
      m_compositedsurfaces.SetAt( pSurfaceArray, pCompositedSurface );
   }

   return pCompositedSurface;
}


void PolyModel::SetVertexNormals(const CArray<UINT> &faceindicies,const CArray<Vector> &vertexnormals)
{
   // unfortunately there is no guarantee that the order stored in the internal face list will match the
   // order stored in the vertexnormals, so we need the faceindicies array to figure out the order

   // build face id to pointer hashtable
   IntHash facehash;
   for (POSITION pos = GetHeadFacePosition(); pos;)
   {
       Poly *pPoly = GetNextFace(pos);
       facehash.AddKey((int)pPoly->GetID(),(int)pPoly);
   }

   // build hashtable to look up vertex normal from the vertex and the poly pointers
   IntIntHash vnhash;

   int nValue;
   int nCount = 0;
   for (UINT nFaceIndex = 0; nFaceIndex < (UINT)faceindicies.GetSize(); nFaceIndex++)
   {
      if (facehash.FindKey((int)faceindicies[nFaceIndex],(int)nValue))
      {
         Poly *pPoly = (Poly*)nValue;
         for (UINT i = 0; i < pPoly->GetNumSides(); i++)
         {
            Vertex *pVertex = pPoly->GetVertexAt(i);
            vnhash.AddKey((int)pVertex,(int)pPoly,nCount++);
         }        
      }
   }

   Vector dir;
   Vertex *pVertex;
   VertexNormal *pVertexNormal;
   CMap <Vector,const Vector &,VertexNormal*,VertexNormal*> vnmap;

   for (POSITION pos = GetHeadVertexPosition(); pos;)
   {
      pVertex = GetNextVertex(pos);

      vnmap.RemoveAll();

      for (UINT i = 0; i < pVertex->GetNumSurroundingPolys(); i++)
      {
         Poly *pSurroundingPoly = pVertex->GetSurroundingPolyAt(i);
         int nIndex;
         if (vnhash.FindKey((int)pVertex,(int)pSurroundingPoly,nIndex))
         {
            dir = vertexnormals[nIndex];
            if (!vnmap.Lookup(dir,pVertexNormal))
            {
               pVertexNormal = AddVertexNormal();
               vnmap.SetAt(dir,pVertexNormal);
            }
            int nFaceRelativeIndex = pSurroundingPoly->FindVertexFaceRelativeIndex(pVertex);
            pSurroundingPoly->SetVertexNormalAt(nFaceRelativeIndex,pVertexNormal);
         }
      }
   }
}

BOOL PolyModel::GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList )
{
   if (pPointerProperty != &m_pSceneObjectCache)
      return TRUE;

   Project *pProject = GetMainInterface()->GetProject();
   if (!pProject)
   {
      ASSERT( FALSE );
      return TRUE;
   }

   ModelContainer *pModelContainer = &pProject->m_modelcontainer;
   for (UINT i=0; i<pModelContainer->GetNumModels(); i++)
   {
      PolyModel *pModel = pModelContainer->GetModelAt( i );
      pSnipeObjectList->AddTail( pModel );
   }

   return TRUE;
}

void PolyModel::FreeRTVertices()
{
   ASSERT( IsCache() );

   if (m_pRTVertices)
   {
      for (POSITION pos=GetHeadCompositedSurfacePosition(); pos; )
      {
         CompositedSurface *pCompositedSurface = GetNextCompositedSurface( pos );
         pCompositedSurface->FreeRTQuads();
      }

      if (m_pRTEdges)
      {
         delete [] m_pRTEdges;
         m_pRTEdges = NULL;
         m_nNumRTEdges = 0;
         m_nNumHardEdges = 0;
         m_nNumBorderEdges = 0;
      }

      for (POSITION pos=GetHeadVertexPosition(); pos; )
      {
         Vertex *pVertex = GetNextVertex( pos );
         pVertex->SetRTVertexHead( NULL );
      }

      delete [] m_pRTVertices;
      m_pRTVertices = NULL;
      m_nNumRTVertices = 0;

      delete [] m_pVisibleVertices;
      m_pVisibleVertices = NULL;
      m_nNumVisibleVertices = 0;

      delete [] m_pLockedVertices;
      m_pLockedVertices = NULL;
      m_nNumLockedVertices = 0;
   }
}

// Randy you may want to reevaluate this stuff. You are using a VertexData in a map. That may not be necessary anymore.
void PolyModel::CreateRTVertices( BOOL bTessellated )
{
   ASSERT( IsCache() );

   FreeRTVertices();

   UINT nNumFaces = GetNumPolys();
   UINT nMapSize = GetPrimeLargerThan( nNumFaces*4 );
   CRTVertexMap rtvertexmap;
   rtvertexmap.InitHashTable( nMapSize );
   CMap<UINT, UINT, Vertex *, Vertex *> inttovertexmap;
   inttovertexmap.InitHashTable( nMapSize );
   RTVertexMapEntry rtvertexmapentry;
   UINT nRTVertexIndex = 0;

   for (POSITION pos=GetHeadFacePosition(); pos; )
   {
      Poly *pFace = GetNextFace( pos );

      UVPoly *pUVPoly = (UVPoly*)pFace->FindLastPropertyPoly(SOT_TexCoords);
      ColorPoly *pColorPoly = (ColorPoly*)pFace->FindLastPropertyPoly(SOT_ColorVertexSet);

      UINT nNumSides = pFace->GetNumSides();
      for (UINT nFaceSide=0; nFaceSide<nNumSides; nFaceSide++)
      {
         Vertex *pVertex = pFace->GetVertexAt( nFaceSide );
         rtvertexmapentry.SetVertex( pVertex );
         rtvertexmapentry.SetPos( pVertex->GetPos() );

         VertexNormal *pVertexNormal = pFace->GetVertexNormalAt( nFaceSide );
         rtvertexmapentry.SetNormal( pVertexNormal->GetDir() );

         if (pColorPoly)
         {
            ColorVertex *pColorVertex = pColorPoly->GetColorVertexAt( nFaceSide );
            rtvertexmapentry.SetColor( pColorVertex->GetColor() );
         }
         else
            rtvertexmapentry.SetColor( 0.0f );

         if (pUVPoly)
         {
            UV *pUV = pUVPoly->GetUVAt( nFaceSide );
            rtvertexmapentry.SetUV( pUV->GetPos() );
         }
         else
            rtvertexmapentry.SetUV( 0.0f );

         UINT nIndex;
         if (!rtvertexmap.Lookup( rtvertexmapentry, nIndex ))
         {
            nIndex = nRTVertexIndex++;
            rtvertexmap.SetAt( rtvertexmapentry, nIndex );
            inttovertexmap.SetAt( nIndex, pVertex );

            if (pVertex->IsDraw())
            {
               if (pVertex->IsLocked())
                  m_nNumLockedVertices++;
               else
                  m_nNumVisibleVertices++;
            }
         }

         pFace->SetRTVertexAt( nFaceSide, nIndex );
      }
   }

   // add dangles
   for (POSITION pos=GetHeadVertexPosition(); pos; )
   {
      Vertex *pVertex = GetNextVertex( pos );

      if (pVertex->GetNumSurroundingPolys() == 0)
      {
         rtvertexmapentry.SetVertex( pVertex );
         rtvertexmapentry.SetPos( pVertex->GetPos() );
         rtvertexmapentry.SetNormal( 0.0f );
         rtvertexmapentry.SetUV( 0.0f );
         rtvertexmapentry.SetColor( 0.0f );
         UINT nIndex;
         if (!rtvertexmap.Lookup( rtvertexmapentry, nIndex ))
         {
            nIndex = nRTVertexIndex++;
            rtvertexmap.SetAt( rtvertexmapentry, nIndex );
            inttovertexmap.SetAt( nIndex, pVertex );

            if (pVertex->IsDraw())
            {
               if (pVertex->IsLocked())
                  m_nNumLockedVertices++;
               else
                  m_nNumVisibleVertices++;
            }
         }
      }
   }

   // transfer from map to member array
   m_nNumRTVertices = rtvertexmap.GetCount();
   if (!m_nNumRTVertices)
      return;

   m_pVisibleVertices = SNEW UINT[ m_nNumVisibleVertices ];
   m_pLockedVertices = SNEW UINT[ m_nNumLockedVertices ];

   CMap<Vertex *, Vertex *, RTVertex *, RTVertex *> lastrtvertexmap;
   lastrtvertexmap.InitHashTable( nMapSize );
   m_pRTVertices = SNEW RTVertex[ m_nNumRTVertices ];

   UINT nVisibleVertex=0, nLockedVertex=0;
   for (const CRTVertexMap::CPair *pPair=rtvertexmap.PGetFirstAssoc(); pPair; pPair=rtvertexmap.PGetNextAssoc( pPair ))
   {
      UINT nIndex = pPair->value;
      RTVertex *pRTVertex = &m_pRTVertices[ nIndex ];
      *pRTVertex = pPair->key;

      Vertex *pVertex;
      inttovertexmap.Lookup( nIndex, pVertex );

      if (pVertex->IsDraw())
      {
         if (pVertex->IsLocked())
            m_pLockedVertices[ nLockedVertex++ ] = nIndex;
         else
            m_pVisibleVertices[ nVisibleVertex++ ] = nIndex;
      }

      RTVertex *pPrevRTVertex;
      if (lastrtvertexmap.Lookup( pVertex, pPrevRTVertex ))
         pPrevRTVertex->SetNextWithSameVertex( pRTVertex );
      else
         pVertex->SetRTVertexHead( pRTVertex );
      lastrtvertexmap.SetAt( pVertex, pRTVertex );
   }

   for (POSITION pos=GetHeadEdgePosition(); pos; )
   {
      Edge *pEdge = GetNextEdge( pos );
      if (!pEdge->IsDraw())
         continue;

      m_nNumRTEdges++;

      if (pEdge->IsBorderEdge())
         m_nNumBorderEdges++;
      else if (!pEdge->IsSoftEdge())
         m_nNumHardEdges++;
   }

   m_pRTEdges = SNEW RTEdge[ m_nNumRTEdges ];

   UINT nRTSoftEdgeIndex = 0;
   UINT nRTBorderEdgeIndex = m_nNumRTEdges - m_nNumBorderEdges;
   UINT nRTHardEdgeIndex = nRTBorderEdgeIndex - m_nNumHardEdges;
   for (POSITION pos=GetHeadEdgePosition(); pos; )
   {
      Edge *pEdge = GetNextEdge( pos );
      UINT nRTEdgeIndex;
      if (!pEdge->IsDraw())
         continue;

      if (pEdge->IsBorderEdge())
         nRTEdgeIndex = nRTBorderEdgeIndex++;
      else if (!pEdge->IsSoftEdge())
         nRTEdgeIndex = nRTHardEdgeIndex++;
      else
         nRTEdgeIndex = nRTSoftEdgeIndex++;

      RTEdge *pRTEdge = &m_pRTEdges[ nRTEdgeIndex ];

      Vertex *pVertex0 = pEdge->GetFirstVertex();
      RTVertex *pRTVertex0;
      BOOL rValue = lastrtvertexmap.Lookup( pVertex0, pRTVertex0 );
      ASSERT( rValue );

      Vertex *pVertex1 = pEdge->GetSecondVertex();
      RTVertex *pRTVertex1;
      rValue = lastrtvertexmap.Lookup( pVertex1, pRTVertex1 );
      ASSERT( rValue );

      UINT nIndex0 = pRTVertex0 - m_pRTVertices;
      UINT nIndex1 = pRTVertex1 - m_pRTVertices;
      pRTEdge->SetVertexIndices( nIndex0, nIndex1 );
   }
}

void PolyModel::PositionVertices( PolyModel *pModel, BOOL bTessellated )
{
   PolyModel *pCache = GetRootCache();
   if (!pCache->m_pRTVertices)
      pCache->CreateRTVertices( bTessellated );

   if (m_pBoneContainer)
   {
      for (UINT i=0; i< m_pBoneContainer->GetNumTransformableObjects(); i++)
      {
         TransformableObject *pTransformableObject = m_pBoneContainer->GetTransformableObjectAt( i );
         pTransformableObject->PositionVertices( pModel, bTessellated );
      }
   }
}

void PolyModel::ConvertFrom(HierObject *pHierObject)
{
   if (pHierObject->IsKindOf(SOT_PatchModel))
      ConvertFromPatchModel((PatchModel*)pHierObject);
}

void AssignVerticesToBone(Bone *pBone,Bone *pOtherBone,CMap<PatchVertex*,PatchVertex*,Vertex*,Vertex*> &vertexmap)
{
   if ((!pBone)||(!pOtherBone))
      return;

   Vertex *pVertex;
   if (pBone->GetSOT() == SOT_Bone)
   {
      pBone->RemoveAllVertices();
      for (POSITION pos = pOtherBone->GetVertexHeadPosition(); pos;)
      {
         PatchVertex *pPatchVertex = (PatchVertex*)pOtherBone->GetNextVertex(pos);
         if (vertexmap.Lookup(pPatchVertex,pVertex))
         {
            pBone->AddVertex(pVertex);
         }
      }
   }
   for (int nChildIndex = 0; nChildIndex < pOtherBone->GetNumChildren(); nChildIndex++)
   {
      HierObject *pOtherChild = (HierObject*)pOtherBone->GetChildAt(nChildIndex);
      HierObject *pChild = (HierObject*)pBone->GetChildAt(nChildIndex);
      AssignVerticesToBone((Bone*)pChild,(Bone*)pOtherChild,vertexmap);
   }
}

void PolyModel::ConvertFromPatchModel(PatchModel *pPatchModel)
{
   CMap<PatchVertex*,PatchVertex*,Vertex*,Vertex*> vertexmap;
   CMap<VertexNormal*,VertexNormal*,VertexNormal*,VertexNormal*> vertexnormalmap;

   for ( POSITION pos = pPatchModel->GetHeadVertexNormalPosition(); pos; )
   {
      VertexNormal *pVertexNormal = pPatchModel->GetNextVertexNormal(pos);
      vertexnormalmap.SetAt(pVertexNormal,AddVertexNormal(pVertexNormal->GetDir()));
   }

   // build polys
   Patch *pPatch;
   Vertex *pVertex;
   VertexNormal *pVertexNormal;

   for ( POSITION pos = pPatchModel->GetHeadFacePosition(); pos; )
   {
      pPatch = pPatchModel->GetNextFace(pos);

      Poly *pPoly = AddPoly(pPatch->GetID());

      pPoly->SetNumSides(pPatch->GetNumSides());

      for (UINT i = 0; i < pPatch->GetNumSides(); i++)
      {
         PatchVertex *pPatchVertex = (PatchVertex*)pPatch->GetVertexAt(i);
         if (!vertexmap.Lookup(pPatchVertex,pVertex))
         {
            pVertex = AddVertex(pPatchVertex->GetPos());
            vertexmap.SetAt(pPatchVertex,pVertex);
         }
         pPoly->SetVertexAt(i,pVertex);

         if (!vertexnormalmap.Lookup(pPatch->GetVertexNormalAt(i),pVertexNormal))
         {
            ASSERT(FALSE);
         }
         pPoly->SetVertexNormalAt(i,pVertexNormal);
      }
   }


   // copy surfaces and things
   MemoryStream ms;

   pPatchModel->MarkSaveDependents();
   pPatchModel->SceneObject::Save( ms );

   for (HierObject *pParent = (HierObject *)pPatchModel->GetParent(); pParent; pParent = (HierObject *)pParent->GetParent())
      pParent->m_bSaveDependent = FALSE;

   ms.Seek( 0, BaseStream::ST_BEGINNING );
   Load( ms );

   GetMainInterface()->GetProject()->NotifyEndProjectLoad();

   // fix vertex bone assignments
   BoneContainer *pPolyModelBoneContainer = pPatchModel->GetBoneContainer();
   BoneContainer *pBoneContainer = GetBoneContainer();

   AssignVerticesToBone((Bone*)pBoneContainer,(Bone*)pPolyModelBoneContainer,vertexmap);

   SelectionContainer *pPolyModelSelectionContainer = pPatchModel->GetSelectionContainer();
   SelectionContainer *pSelectionContainer = GetSelectionContainer();

   MainInterface *pMainInterface = GetMainInterface();

   // fix vertex selections
   if ((pPolyModelSelectionContainer)&&(pSelectionContainer))
   {
      for (int nSelectionIndex = 0; nSelectionIndex < pPolyModelSelectionContainer->GetNumChildren(); nSelectionIndex++)
      {
         Selection *pOtherSelection = (Selection*)pPolyModelSelectionContainer->GetChildAt(nSelectionIndex);
         Selection *pSelection = (Selection*)pSelectionContainer->GetChildAt(nSelectionIndex);
         pSelection->RemoveAll();
         StringList outerrorlist;
         SnipeObjectList *pSnipeObjectList = pMainInterface->DescriptionToObjects(pOtherSelection->GetSelectionDescription(),outerrorlist);
         for (POSITION pos = pSnipeObjectList->GetHeadPosition(); pos;)
         {
            SnipeObject *pSnipeObject = pSnipeObjectList->GetNext(pos);
            if (pSnipeObject->GetSOT()==SOT_Vertex)
               if (vertexmap.Lookup((PatchVertex*)pSnipeObject,pVertex))
                  pSelection->AddObject(pVertex);
         }
         delete pSnipeObjectList;
      }
   }
}

void PolyModel::OnEndProjectLoad()
{
   BuildEdges();
   BuildVertexNormals();
   Compact();
}

void PolyModel::Compact()
{
   for ( POSITION pos = GetHeadVertexPosition(); pos; )
   {
      Vertex *pVertex = GetNextVertex(pos);
      pVertex->Compact();
   }

   CMap<PropertySet*,PropertySet*,BOOL,BOOL> propertySets;
   PropertySet *pPropertySet;
   for (POSITION pos = GetHeadFacePosition(); pos; )
   {
      Poly *pPoly = GetNextFace( pos );
      for (PropertyPoly *pPropertyPoly = pPoly->GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly = pPropertyPoly->GetNextPropertyPoly())
      {
         pPropertySet = pPropertyPoly->GetPropertySet();
         propertySets.SetAt(pPropertySet,TRUE);
      }
   }

   BOOL bValue;
   for (pos = propertySets.GetStartPosition(); pos; )
   {
      propertySets.GetNextAssoc(pos,pPropertySet,bValue);
      for (POSITION pos = pPropertySet->GetHeadPropertyVertexPosition(); pos;)
      {
         PropertyVertex *pPropertyVertex = pPropertySet->GetNextPropertyVertex(pos);
         pPropertyVertex->Compact();
      }
   }
}

BaseStream &operator << ( BaseStream &bs, const PolyModel *&value )
{
   return operator << ( bs, (const SnipeObject * &) value);
}

BaseStream &operator >> ( BaseStream &bs, PolyModel *&value )
{
   return operator >> (bs, (SnipeObject *&) value );
}
