#pragma once

#include "../Transform/TransformManipulator.h"
#include "MouseOn.h"

// Plugin
class RotateManipulatorPlugin : public TransformManipulatorPlugin
{
public:
   DECLARE_MANIPULATOR_PLUGINHELPER( RotateManipulatorPlugin, TransformManipulatorPlugin )

// HManipulatorPluginHelper overrides
   virtual catch_msg ManipulatorInstance *CreateInstance();

// HCommandUIPluginHelper Overides
   virtual const char *GetTitleName() const;
   virtual UINT GetBitmapID() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
};

//Instance
class RotateManipulator : public TransformManipulator
{
public:
   DECLARE_MANIPULATORINSTANCE( RotateManipulator, TransformManipulator )

            RotateManipulator( RotateManipulatorPlugin *pPluginHelper );
   virtual ~RotateManipulator();
};
