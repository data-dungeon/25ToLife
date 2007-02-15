#pragma once
#include "GL3DView.h"
#include "GLButtonControl.h"
#include "Component.h"

class CModelDoc;
class CGLModelFrame;
class TexCoords;
class Component;
class Vertex;
class Edge;
class Poly;

class CGLModelView : public CGL3DView
{
   DECLARE_BASEVIEW(CGLModelView, CGL3DView)
   enum { HEADERHEIGHT=15, BUTTONWIDTH=18 };

public:
   static TypeProperty::Info m_nShadingModeInfo;
   static TypeProperty::Info m_nDrawAsInfo;
   static BoolProperty::Info m_bDisplayControlHandlesInfo;
   static BoolProperty::Info m_bDisplayTexturesInfo;
   static BoolProperty::Info m_bDisplayStatsInfo;
   static Property::Info m_DiagnosticInfo;
      static BoolProperty::Info m_bDisplayInRetainInfo;
      static BoolProperty::Info m_bDisplayVertexIndicesInfo;
      static BoolProperty::Info m_bDisplayVertexNormalIndicesInfo;
      static BoolProperty::Info m_bDisplayVertexNormalsInfo;
      static BoolProperty::Info m_bDisplayEdgeIndicesInfo;
         static BoolProperty::Info m_bDisplayContinuityIDsInfo;
      static BoolProperty::Info m_bDisplayContinuityInfo;
      static BoolProperty::Info m_bDisplayFaceIndicesInfo;
      static BoolProperty::Info m_bDisplayFaceNormalsInfo;
      static BoolProperty::Info m_bDisplayFirstVertexInfo;
      static BoolProperty::Info m_bDisplayUVFaceIndicesInfo;
      static BoolProperty::Info m_bDisplayUVIndicesInfo;
      static BoolProperty::Info m_bDisplayUVEdgeIndicesInfo;
      static BoolProperty::Info m_bDisplayBoundingBoxInfo;
      static BoolProperty::Info m_bDisplayUVBoundaryInfo;
      static BoolProperty::Info m_bDisplayControlVerticesInfo;
      static BoolProperty::Info m_bDisplayHiddenEdgesInfo;

   TypeProperty m_nShadingMode; enum ShadingMode { SM_WIREFRAME, SM_SHADED, SM_SHADEDWIREFRAME };
   TypeProperty m_nDrawAs; enum DrawAs { DRAWAS_POLYMODEL, DRAWAS_PATCHMODEL };
   BoolProperty m_bDisplayControlHandles;
   BoolProperty m_bDisplayTextures;
   BoolProperty m_bDisplayStats;
   Property m_Diagnostic;
      BoolProperty m_bDisplayInRetain;
      BoolProperty m_bDisplayVertexIndices;
      BoolProperty m_bDisplayVertexNormalIndices;
      BoolProperty m_bDisplayVertexNormals;
      BoolProperty m_bDisplayEdgeIndices;
         BoolProperty m_bDisplayContinuityIDs;
      BoolProperty m_bDisplayContinuity;
      BoolProperty m_bDisplayFaceIndices;
      BoolProperty m_bDisplayFaceNormals;
      BoolProperty m_bDisplayFirstVertex;
      BoolProperty m_bDisplayUVFaceIndices;
      BoolProperty m_bDisplayUVIndices;
      BoolProperty m_bDisplayUVEdgeIndices;
      BoolProperty m_bDisplayBoundingBox;
      BoolProperty m_bDisplayUVBoundary;     
      BoolProperty m_bDisplayControlVertices;
      BoolProperty m_bDisplayHiddenEdges;

   // Header controls
   CGLButtonControl m_glWireFrameButton;
   CGLButtonControl m_glShadedButton;
   CGLButtonControl m_glShadedWireframeButton;
   CGLButtonControl m_glDisplayAsPolyModelButton;
   CGLButtonControl m_glDisplayAsPatchModelButton;

   int m_nMaxDrawAs;
   Component *m_pDoPickLimitingComponent;
   SnipeObjectType m_nDoPickLimitingComponentBaseType;

            CGLModelView();
   virtual ~CGLModelView();

   CModelDoc *GetDocument() const { return (CModelDoc *)CGLBaseView::GetDocument(); }
   CGLModelFrame *GetParentFrame() const { return (CGLModelFrame *)CGLBaseView::GetParentFrame(); }

   void DrawStatistics();

   BOOL IsDisplayWireframe() { return m_nShadingMode==SM_WIREFRAME || m_nShadingMode==SM_SHADEDWIREFRAME; }
   BOOL IsDisplayShaded() { return m_nShadingMode==SM_SHADED || m_nShadingMode==SM_SHADEDWIREFRAME; }

   void SetDrawAs( DrawAs nDrawAs ) { m_nDrawAs.StoreValue( nDrawAs ); }
   DrawAs GetDrawAs() { return (DrawAs)m_nDrawAs.GetValue(); }
   void SetMaxDrawAs( DrawAs nDrawAs ) { m_nMaxDrawAs = nDrawAs; }
   DrawAs GetMaxDrawAs() { return (DrawAs)m_nMaxDrawAs; }
   BOOL IsDrawAsPolyModel() { return GetDrawAs() == DRAWAS_POLYMODEL; }
   BOOL IsDrawAsPatchModel() { return GetDrawAs() == DRAWAS_PATCHMODEL; }

   BOOL IsDoPickLimitByComponent() const { return m_pDoPickLimitingComponent!=NULL; }
   Vertex *IsLimitDoPickByVertex() const { return m_nDoPickLimitingComponentBaseType==SOT_Vertex ? (Vertex *)m_pDoPickLimitingComponent : NULL; }
   Edge *IsLimitDoPickByEdge() const { return m_nDoPickLimitingComponentBaseType==SOT_Edge ? (Edge *)m_pDoPickLimitingComponent : NULL; }
   Poly *IsLimitDoPickByFace() const { return m_nDoPickLimitingComponentBaseType==SOT_Poly ? (Poly *)m_pDoPickLimitingComponent : NULL; }
   void SetDoPickLimitComponent( Component *pComponent ) ;

   afx_msg void OnUpdateDisplayShaded(CCmdUI *pCmdUI);
   afx_msg void OnDisplayShaded();
   afx_msg void OnUpdateDisplayWireframe(CCmdUI *pCmdUI);
   afx_msg void OnDisplayWireframe();
   afx_msg void OnUpdateDisplayShadedWireframe(CCmdUI *pCmdUI);
   afx_msg void OnDisplayShadedWireframe();
   afx_msg void OnUpdateDisplayTextures(CCmdUI *pCmdUI);
   afx_msg void OnDisplayTextures();

   afx_msg void OnDrawAsPolyModel();
   afx_msg void OnDrawAsPatchModel();

// HeaderView overrides
   virtual void UpdateHeader();

// CGLBaseView overrides
   virtual void DrawContents();
   virtual void DefaultZoomFit(SnipeObjectList *pList);
   virtual void ConvertSelectionToCurrentSelectionType( SnipeObjectType nSOT );
   virtual BOOL OnSnipeIdle();

// CGLControl overrides
   virtual void PostInit();
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};

inline void CGLModelView::SetDoPickLimitComponent( Component *pComponent ) 
{ 
   if (pComponent == NULL)
      m_nDoPickLimitingComponentBaseType = SOT_Unknown;
   else if (pComponent->IsKindOf(SOT_Vertex))
      m_nDoPickLimitingComponentBaseType = SOT_Vertex;
   else if (pComponent->IsKindOf(SOT_Edge))
      m_nDoPickLimitingComponentBaseType = SOT_Edge;
   else if (pComponent->IsKindOf(SOT_Poly))
      m_nDoPickLimitingComponentBaseType = SOT_Poly;
   else
   {
      ASSERT(FALSE);
      return;
   }

   m_pDoPickLimitingComponent = pComponent; 
}
