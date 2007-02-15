#pragma once

#include "HProperty.h"
#include "HEdge.h"

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class HPatch;
class HPatchModel;
class HPatchVertex;
class HFloatProperty;

class SNIPEEXPORT HVertexHandle : public HProperty
{
public:
   HFloatProperty *GetMagnitudeProperty();
   HFloatProperty *GetAlphaProperty();
   HFloatProperty *GetGammaProperty();

   Vector GetTangent() const;
};

class SNIPEEXPORT HPatchEdge : public HEdge
{
public:
   class SNIPEEXPORT HRedirectInfo
   {
   public:
      void Undo();
      void Redo();

      void operator delete (void *pVoid);
   };

   HPatchModel *GetModel();

   HPatchVertex *GetPrevContVertex() const;
   HPatchVertex *GetNextContVertex() const;

   HPatchEdge *GetPrevContEdge( BOOL bLoop );
   void SetPrevContEdge( HPatchEdge *pEdge );
   HPatchEdge *GetNextContEdge( BOOL bLoop );
   void SetNextContEdge( HPatchEdge *pEdge );

   HVertexHandle *GetFirstVertexHandle( BOOL bBuildTangentIfDirty=TRUE );
   HVertexHandle *GetSecondVertexHandle( BOOL bBuildTangentIfDirty=TRUE );

   BOOL IsVertexConstrained( HVertex *pVertex ) const;

   HRedirectInfo *Redirect( HPatchVertex *pTermVertex, String *pOutError=NULL );
   HRedirectInfo *Redirect( HPatchEdge *pToEdge, String *pOutError=NULL );
};