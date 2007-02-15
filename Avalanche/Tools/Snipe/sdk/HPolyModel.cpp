#include "stdafx.h"
#include "HPolyModel.h"
#include "PolyModel.h"
#include "ColorVertexSet.h"

UINT HPolyModel::GetNumVertices()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((PolyModel *)this)->GetNumVertices();
}

POSITION HPolyModel::GetHeadVertexPosition()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((PolyModel *)this)->GetHeadVertexPosition();
}

HVertex *HPolyModel::GetNextVertex( POSITION &pos )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HVertex *)((PolyModel *)this)->GetNextVertex( pos );
}

HVertex *HPolyModel::AddVertex()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HVertex *)((PolyModel *)this)->AddVertex();
}

HVertex *HPolyModel::AddVertex( UINT nID )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HVertex *)((PolyModel *)this)->AddVertex( nID );
}

HVertex *HPolyModel::AddVertex( const Vector &pos, HVertex::HDeleteInfo **pOutVertexDeleteInfo/*=NULL*/, BOOL bMirrorIfInMirrorMode/*=FALSE*/ )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HVertex *)((PolyModel *)this)->AddVertex( pos, (Vertex::DeleteInfo **) pOutVertexDeleteInfo, bMirrorIfInMirrorMode );
}

HVertex *HPolyModel::FindVertex( UINT nID )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HVertex *)((PolyModel *)this)->FindVertex( nID );
}

void HPolyModel::BuildVertexNormals()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((PolyModel *)this)->BuildVertexNormals();
}

UINT HPolyModel::GetNumVertexNormals()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((PolyModel *)this)->GetNumVertexNormals();
}

POSITION HPolyModel::GetHeadVertexNormalPosition()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((PolyModel *)this)->GetHeadVertexNormalPosition();
}

HVertexNormal *HPolyModel::GetNextVertexNormal( POSITION &pos )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HVertexNormal *)((PolyModel *)this)->GetNextVertexNormal( pos );
}

HVertexNormal *HPolyModel::AddVertexNormal()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HVertexNormal *)((PolyModel *)this)->AddVertexNormal();  
}

void HPolyModel::BuildEdges()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((PolyModel *)this)->BuildEdges();
}

HEdge *HPolyModel::AddEdge()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HEdge *)((PolyModel *)this)->AddEdge();
}

UINT HPolyModel::GetNumEdges()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((PolyModel *)this)->GetNumEdges();
}

POSITION HPolyModel::GetHeadEdgePosition()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((PolyModel *)this)->GetHeadEdgePosition();
}

HEdge *HPolyModel::GetNextEdge( POSITION &pos )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HEdge *)((PolyModel *)this)->GetNextEdge( pos );
}

int HPolyModel::AddEdges( HSnipeObjectList *pVertices, HEdge::HDeleteInfo **&pEdgeDeleteInfos, HEdge *pPrevEdge/*=HEDGE_TERMINATE*/, HEdge *pNextEdge/*=HEDGE_TERMINATE*/ )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((PolyModel *)this)->AddEdges( (SnipeObjectList *)pVertices, (Edge::DeleteInfo **&)pEdgeDeleteInfos, (Edge *)pPrevEdge, (Edge *)pNextEdge );
}

HEdge::HDeleteInfo *HPolyModel::AddEdge( HVertex *pV0, HVertex *pV1, HEdge *pPrevEdge/*=HEDGE_FINDCONTINUOUS*/, HEdge *pNextEdge/*=HEDGE_FINDCONTINUOUS*/ )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HEdge::HDeleteInfo *)((PolyModel *)this)->AddEdge( (Vertex *)pV0, (Vertex*)pV1, (Edge *)pPrevEdge, (Edge *)pNextEdge );
}

UINT HPolyModel::GetNumPolys()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((PolyModel *)this)->GetNumPolys();
}

POSITION HPolyModel::GetHeadFacePosition()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((PolyModel *)this)->GetHeadFacePosition();
}

HPoly *HPolyModel::GetNextFace( POSITION &pos )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HPoly *)((PolyModel *)this)->GetNextFace( pos );
}

HPoly *HPolyModel::AddPoly()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HPoly *)((PolyModel *)this)->AddPoly();
}

void HPolyModel::RemovePoly(HPoly *pPoly)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((PolyModel*)this)->RemovePoly((Poly*)pPoly);
}

void HPolyModel::BuildPolys()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
//   ((PolyModel *)this)->BuildPolys();
}

HTexCoords *HPolyModel::AddTexCoords(const char *name/*=NULL*/)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HTexCoords*)((PolyModel *)this)->AddTexCoords(name);
}

HColorVertexSet *HPolyModel::AddColorVertexSet(const char *name/*=NULL*/)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HColorVertexSet*)((PolyModel *)this)->AddColorVertexSet(name);
}

HBoneContainer *HPolyModel::GetBoneContainer()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HBoneContainer *)((PolyModel *)this)->GetBoneContainer();
}

void HPolyModel::AddBone( HTransformableObject *pBone, HTransformableObject *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((PolyModel *)this)->AddBone( (TransformableObject *)pBone, (TransformableObject *)pParent );
}

HTexCoordsContainer *HPolyModel::GetTexCoordsContainer()
{
   return (HTexCoordsContainer*)((PolyModel*)this)->GetTexCoordsContainer();
}

void HPolyModel::SetVertexNormals(const CArray<UINT> &faceindicies,const CArray<Vector> &vertexnormals)
{
   ((PolyModel *)this)->SetVertexNormals(faceindicies,vertexnormals);
}

HSurfaceContainer *HPolyModel::GetSurfaceContainer()
{
   return (HSurfaceContainer *)((PolyModel *)this)->GetSurfaceContainer();
}

HSurface *HPolyModel::AddSurface( const char *name/*=NULL*/, int nIndex/*=-1*/, HSurface::HDeleteInfo **pDeleteInfo/*=NULL*/ )
{
   return (HSurface *)((PolyModel *)this)->AddSurface( name, nIndex, (Surface::DeleteInfo **)pDeleteInfo );
}

void HPolyModel::RemoveSurface( HSurface *pSurface )
{
   ((PolyModel *)this)->RemoveSurface( (Surface *)pSurface );
}
