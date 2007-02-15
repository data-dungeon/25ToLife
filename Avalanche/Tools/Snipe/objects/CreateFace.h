#pragma once

class Poly;

class DeleteFaceInfo
{
public:
   Poly *m_pFace;
   DeleteFaceInfo *m_pMirrorDeleteFaceInfo;

   DeleteFaceInfo( Poly *pFace );
   ~DeleteFaceInfo();

   void Delete();
   void UnDelete();
};

