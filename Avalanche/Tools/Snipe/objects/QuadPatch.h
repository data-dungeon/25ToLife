#pragma once

#include "..\Utility\DynamicMatrix.h"

class QuadPatch
{
public:
   Vector m_cvs[16];
   Vector m_vncvs[16];
   DynamicMatrix m_vmat, m_vnmat;
 
public:
   QuadPatch();
   ~QuadPatch();

   void     SetCVs(Vector *pCVs);
};

#include "QuadPatch.hpp"