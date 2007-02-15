#pragma once
#include "HCGLHeaderView.h"

class HCamera;
class SNIPEEXPORT HCGL2DView : public HCGLHeaderView
{
public:

   HCamera *GetCurrentCamera();
};