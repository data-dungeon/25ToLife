/*
inline void TransformableObject::GetExclusiveTransformMatrix( Matrix44 &matrix )
{
   matrix.Identity();
   for (HierObject *pHObject = GetParent(); pHObject; pHObject = pHObject->GetParent())
   {
      Matrix44 *pLocalMatrix = pHObject->GetLocalTransformationMatrix();
      if (pLocalMatrix)
         matrix *= *pLocalMatrix;
   }
}

inline void TransformableObject::GetInclusiveTransformMatrix( Matrix44 &matrix )
{
   matrix.Identity();

   for (HierObject *pHObject = this; pHObject; pHObject = pHObject->GetParent()) {
      Matrix44 *pLocalMatrix = pHObject->GetLocalTransformationMatrix();
      if (pLocalMatrix)
         matrix *= *pLocalMatrix;
   }
}
*/