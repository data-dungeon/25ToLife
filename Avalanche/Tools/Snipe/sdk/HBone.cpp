#include "stdafx.h"
#include "HBone.h"
#include "Bone.h"

UINT HBone::GetNumVertices()
{
   return ((Bone *)this)->GetNumVertices();
}

POSITION HBone::GetVertexHeadPosition()
{
   return ((Bone *)this)->GetVertexHeadPosition();
}

HVertex *HBone::GetNextVertex( POSITION &pos )
{
   return (HVertex *)((Bone *)this)->GetNextVertex( pos );
}

void HBone::AddVertex( HVertex *pVertex )
{
   ((Bone *)this)->AddVertex( (Vertex *)pVertex );
}

BOOL HBone::RemoveVertex( HVertex *pVertex  )
{
   return ((Bone *)this)->RemoveVertex( (Vertex *)pVertex );
}

HVertex *HBone::FindVertex( UINT nID )
{
   return (HVertex *)((Bone *)this)->FindVertex( nID );
}

UINT HBoneContainer::GetNumTransformableObjects()
{
   return ((BoneContainer*)this)->GetNumTransformableObjects();
}

HTransformableObject *HBoneContainer::GetTransformableObjectAt(UINT nIndex)
{
   return (HTransformableObject*)((BoneContainer*)this)->GetTransformableObjectAt(nIndex);
}