#pragma once

#include "SnipeManipulator.h"
#include "NavigatorPlugin.h"

class SnipeNavigatorInstance;
class CGLBaseView;

class SnipeNavigator : public SnipeManipulator
{
public:
            SnipeNavigator( NavigatorPlugin *pNavigatorPlugin );
   virtual ~SnipeNavigator();

   SnipeNavigatorInstance *CreateInstance( CGLBaseView *pView );
};

class SnipeNavigatorInstance : public SnipeManipulatorInstance
{
public:
   SnipeNavigatorInstance( SnipeNavigator *pSnipeNavigator, void *pPluginInstanceData ) :
      SnipeManipulatorInstance( pSnipeNavigator, pPluginInstanceData ) {}

   BOOL SetViewAngle( CGL3DView *pView, ViewAngleIndex nIndex, BOOL bSnapToLastBirdsEye=TRUE );
   BOOL ZoomFit( CGLBaseView *pView, SnipeObjectList *pList );
};

