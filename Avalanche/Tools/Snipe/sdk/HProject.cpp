#include "stdafx.h"
#include "..\Gui\Snipe.h"
#include "HProject.h"
#include "Project.h"

HImageContainer *HProject::GetImageContainer()
{
   return (HImageContainer *)&((Project *)this)->m_imagecontainer;
}

void HProject::TimerStart( LARGE_INTEGER &starttime )
{
   ::TimerStart(starttime);
}

float HProject::TimerStop( LARGE_INTEGER &starttime )
{
   return ::TimerStop(starttime);
}

UINT HProject::AddModel( HPolyModel *pModel )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((Project *)this)->AddModel( (PolyModel *)pModel );
}

HPolyModel *HProject::AddPolyModel( const char *name )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HPolyModel*)((Project *)this)->AddPolyModel(name);
}

HCamera *HProject::AddCamera( const char *name )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HCamera*)((Project *)this)->AddCamera(name);
}

HLight *HProject::AddLight( const char *name )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HLight*)((Project *)this)->AddLight(name);
}

HMotion *HProject::AddMotion( const char *name )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HMotion*)((Project *)this)->AddMotion(name);
}

HSnipeObjectList *HProject::ImportMotions( StringList &strFileNames )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HSnipeObjectList *)((Project *)this)->ImportMotions( strFileNames );
}

HScene *HProject::AddScene( const char *name )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HScene*)((Project *)this)->AddScene(name);
}

HMotionSet *HProject::AddMotionSet( const char *name )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HMotionSet*)((Project *)this)->AddMotionSet( name );
}

HImage *HProject::AddImage( const char *name )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HImage*)((Project *)this)->AddImage(name);
}

HImage *HProject::AddImage( const String &strFileName )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HImage*)((Project *)this)->AddImage( strFileName );
}
