#pragma once

#include "HCFrameWnd.h"

class HCGLToolBarControl;

class SNIPEEXPORT HCMainFrame : public HCFrameWnd
{
public:
   HCGLToolBarControl *FindToolBar(const String &strName) const;

   // This will return an already existing toolbar if available
   HCGLToolBarControl *AddToolBar( const String &strName );

   GLuint CreateCallListFromGeoResource( UINT nResourceID );
};

SNIPEEXPORT HCMainFrame *GetHCMainFrame();