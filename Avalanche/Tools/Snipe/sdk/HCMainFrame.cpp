//

#include "stdafx.h"
#include "HCMainFrame.h"
#include "../GUI/MainFrm.h"

HCMainFrame *GetHCMainFrame()
{
   return (HCMainFrame *)GetMainFrame();
}

HCGLToolBarControl *HCMainFrame::FindToolBar( const String &strName ) const
{
   return (HCGLToolBarControl *)((CMainFrame *)this)->FindToolBar( strName );
}

// This will return an already existing toolbar if available
HCGLToolBarControl *HCMainFrame::AddToolBar( const String &strName )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   
   return (HCGLToolBarControl *) ((CMainFrame *)this)->AddToolBar( strName );
}

GLuint HCMainFrame::CreateCallListFromGeoResource( UINT nResourceID )
{
   return ((CMainFrame *)this)->CreateCallListFromGeoResource( nResourceID );
}
