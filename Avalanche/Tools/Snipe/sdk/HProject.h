#pragma once

#include "HHierObject.h"

class HImage;
class HImageContainer;
class HPolyModel;
class HCamera;
class HLight;
class HMotion;
class HScene;
class HMotionSet;

class SNIPEEXPORT HProject : public HHierObject
{
public:
   HImageContainer *GetImageContainer();
 
   void TimerStart(LARGE_INTEGER &starttime);
   float TimerStop(LARGE_INTEGER &starttime);

   UINT AddModel( HPolyModel *pModel );
   HPolyModel *AddPolyModel( const char *name = "untitled" );
   HCamera *AddCamera( const char *name = "untitled" );
   HLight *AddLight( const char *name = "untitled" );
   HMotion *AddMotion( const char *name = "untitled" );
   HSnipeObjectList *ImportMotions( StringList &strFileNames );
   HScene *AddScene( const char *name = "untitled" ); 
   HImage *AddImage( const char *name = "untitled" );
   HMotionSet *AddMotionSet( const char *name = "untitled" );
   HImage *AddImage( const String &strFileName );
};
