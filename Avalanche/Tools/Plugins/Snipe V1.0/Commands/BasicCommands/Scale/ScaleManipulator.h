#pragma once

#include "../Transform/TransformManipulator.h"

// Plugin
class ScaleManipulatorPlugin : public TransformManipulatorPlugin
{
public:
   DECLARE_MANIPULATOR_PLUGINHELPER( ScaleManipulatorPlugin, TransformManipulatorPlugin )

// HManipulatorPluginHelper overrides
   virtual catch_msg ManipulatorInstance *CreateInstance();

// HCommandUIPluginHelper Overides
   virtual const char *GetTitleName() const;
   virtual UINT GetBitmapID() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
};

// Instance
class ScaleManipulator : public TransformManipulator
{
public:
   DECLARE_MANIPULATORINSTANCE( ScaleManipulator, TransformManipulator )

            ScaleManipulator( ScaleManipulatorPlugin *pPluginHelper );
   virtual ~ScaleManipulator();
};
