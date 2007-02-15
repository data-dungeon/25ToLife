#pragma once

#include "HTransformableObject.h"
#include "HContainerObject.h"

class HVertex;

class SNIPEEXPORT HBone : public HTransformableObject
{
public:
   UINT GetNumVertices();
   POSITION GetVertexHeadPosition();
   HVertex *GetNextVertex( POSITION &pos );
   void AddVertex( HVertex *pVertex );
   HVertex *FindVertex( UINT nID );
   BOOL RemoveVertex( HVertex *pVertex );
};

class SNIPEEXPORT HBoneContainer : public HContainerObject
{
public:
   UINT GetNumTransformableObjects();
   HTransformableObject *GetTransformableObjectAt( UINT nIndex );
};
