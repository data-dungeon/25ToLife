#pragma once

#include "HCGLControl.h"

class HCommandUIPlugin;

class SNIPEEXPORT HCGLToolBarControl : public HCGLControl
{
public:
   BOOL AddButton( HCommandUIPlugin *pPlugin, int nPos=-1 );
};
