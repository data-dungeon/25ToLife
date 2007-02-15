#pragma once

#include "PropertyEdge.h"

class TexCoords;

class UVEdge : public PropertyEdge
{

public:
   DECLARE_COMPONENT_CREATE(UVEdge, PropertyEdge)

   UVEdge();
   UVEdge( UINT nID );
   virtual ~UVEdge();

   UVEdge *GetNextUVEdge() { return (UVEdge*)GetNextPropertyEdge(); }
   void SetNextUVEdge( UVEdge *pNextUVEdge ) { SetNextPropertyEdge((PropertyEdge*)pNextUVEdge); }
   UVEdge **GetNextUVEdgeUplink() { return (UVEdge**)GetNextPropertyEdgeUplink(); }

   void SetFirstUV(UV *pUV) { return SetFirstPropertyVertex((PropertyVertex*)pUV); }
   void SetSecondUV(UV *pUV) { return SetSecondPropertyVertex((PropertyVertex*)pUV); }

   UV *GetFirstUV() const { return (UV*)GetFirstPropertyVertex(); }
   UV *GetSecondUV() const { return (UV*)GetSecondPropertyVertex(); }

   UV *GetSharedUV( UVEdge *pUVEdge ) const { return (UV*)GetSharedPropertyVertex((PropertyEdge*)pUVEdge); }
   UV *GetConnectedUV( UV *pNotThisUV ) { return (UV*)GetConnectedPropertyVertex((PropertyVertex*)pNotThisUV); }
   BOOL IsInEdge(UV *pUV) { return PropertyEdge::IsInEdge((PropertyVertex*)pUV); }

   BOOL IsAligned( UVPoly *pUVPoly ) const { return PropertyEdge::IsAligned((PropertyPoly *)pUVPoly); }
   UV *GetAlignedFirstUV( UVPoly *pUVPoly ) const { return (UV*)GetAlignedFirstPropertyVertex((PropertyPoly*)pUVPoly); }
   UV *GetAlignedSecondUV( UVPoly *pUVPoly ) const { return (UV*)GetAlignedSecondPropertyVertex((PropertyPoly*)pUVPoly); }

   UVPoly *GetFirstUVPoly() { return (UVPoly*)GetFirstPropertyPoly(); }
   void SetFirstUVPoly(UVPoly *pUVPoly) { SetFirstPropertyPoly((PropertyPoly*)pUVPoly); }
   UVPoly *GetSecondUVPoly() { return (UVPoly*)GetSecondPropertyPoly(); }
   void SetSecondUVPoly(UVPoly *pUVPoly) { SetFirstPropertyPoly((PropertyPoly*)pUVPoly); }
   UVPoly *GetUVPoly() { return (UVPoly*)GetPropertyPoly(); }
   UVPoly *GetConnectedUVPoly(UVPoly *pNotThis) { return (UVPoly*)GetConnectedPropertyPoly((PropertyPoly*)pNotThis); }

   TexCoords *GetTexCoords() { return (TexCoords*)GetPropertySet(); }

   static UVEdge *FindEdge( const UV *pUV0, const UV *pUV1 ) { return (UVEdge*)PropertyEdge::FindEdge((PropertyVertex*)pUV0,(PropertyVertex*)pUV1); }
   
   void GetVector(Vector2 &dir);

   BOOL AddUVPoly(UVPoly *pUVPoly) { return AddPropertyPoly((PropertyPoly*)pUVPoly); }
   void RemoveUVPoly(UVPoly *pUVPoly) { return RemovePropertyPoly((PropertyPoly*)pUVPoly); }
  
   Vector2 GetPosAlongEdge( float fPosition, UVPoly *pPoly );

// PropertyEdge overrides
   virtual void Draw( BOOL bDrawTess );
};
