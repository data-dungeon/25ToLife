#pragma once

//#include "PatchModel.h"
//#include "Interface/MainInterface.h"
//#include "BezierKnot.h"
//#include "BezierSplineEdge.h"
//#include "BezierPatch.h"
//
//class BezierSplineModel : public PatchModel
//{
//public:
//   DECLARE_SNIPEOBJECT_CREATE( BezierSplineModel, PatchModel )
//
//   UINT *m_pConstrainedRTVertices;
//   UINT  m_nNumConstrainedRTVertices;
//
//            BezierSplineModel();
//   virtual ~BezierSplineModel();
//
//   BezierSplineModel *GetRootCache() { return (BezierSplineModel *)SceneObject::GetRootCache(); } // Note: returns this if it is the cache
//   BezierSplineModel *GetCache() { return (BezierSplineModel *)SceneObject::GetCache(); } // Note: returns this if it is the cache
//
//   BezierKnot *AddKnot() { return (BezierKnot *)AddVertex(); }
//   BezierKnot *AddKnot( UINT nID ) { return (BezierKnot *)AddVertex( nID ); }
//   BezierKnot *GetNextKnot( POSITION &pos ) { return (BezierKnot *)GetNextVertex( pos ); }
//   BezierKnot *FindKnot( UINT nID ) { return (BezierKnot *)FindVertex( nID ); }
//
//   void ConvertFromPolyModel(PolyModel *pPolyModel);
//
//   void DrawFaceKnotIndices(CGLModelView *pModelView);
//
//   void FreeConstrainedRTVertices();
//   void CreateConstrainedRTVertices();
//
//// PatchModel overrides
//   virtual void DrawControlVertices();
//
//// PolyModel overrides
//   virtual Vertex *CreateVertex( UINT nID=-1 ) const;
//   virtual Edge *CreateEdge( UINT nID=-1 ) const;
//   virtual Poly *CreatePoly( UINT nID=-1 ) const;
//   virtual Edge *AddEdgeBetween( Vertex *pV0, Vertex *pV1, BOOL *pOutReversedV1=NULL, Vertex *pPrev=VERTEX_FINDCONTINUOUS, Vertex *pNext=VERTEX_FINDCONTINUOUS );
//   virtual void DrawVertexIndices( CGLModelView *pModelView );
//   virtual void FreeRTVertices();
//   virtual void CreateRTVertices( BOOL bTessellated );
//   virtual void DrawRetainedMode( CGLModelView *pModelView );
//   virtual UVEdge *CreateUVEdge( UINT nID=-1 ) const;
//   virtual void BuildVertexNormals();
//
//// HierObject overrides
//   virtual void ConvertFrom(HierObject *pHierObject);
//
//// InstanceObject overrides
//   virtual void Draw( CGLBaseView *pBaseView );
//
//// SnipeObject overrides
//   virtual void OnEndObjectLoad();
//};
//
//#include "BezierSplineModel.hpp"
