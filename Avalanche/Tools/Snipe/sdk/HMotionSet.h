#pragma once

#include "HHierObject.h"
#include "HMotionCurveDriver.h"

class HMotion;
class HFloatProperty;
class HMotionCurveDriver;


class SNIPEEXPORT HFloatMotionCurveDriverBiasObject : public HMotionCurveDriver
{
};

class SNIPEEXPORT HVector2MotionCurveDriverBiasObject : public HMotionCurveDriver
{
};

class SNIPEEXPORT HVectorMotionCurveDriverBiasObject : public HMotionCurveDriver
{
};

class SNIPEEXPORT HEulerMotionCurveDriverBiasObject : public HMotionCurveDriver
{
};

class SNIPEEXPORT HQuaternionMotionCurveDriverBiasObject : public HMotionCurveDriver
{
};

class SNIPEEXPORT HMotionChannelBiasObject : public HHierObject
{
public:
   HFloatProperty *GetTranslationBiasProperty();
   HFloatProperty *GetRotationBiasProperty();
   HFloatProperty *GetScaleBiasProperty();

   UINT GetNumMotionCurveDriverBiasObjects();
   HMotionCurveDriver *GetMotionCurveDriverBiasObjectAt( UINT nIndex );
};

class SNIPEEXPORT HMotionPlaceholder : public HHierObject
{
public:
   HMotion *GetMotion();
   UINT GetNumMotionChannelBiasObjects();
   HMotionChannelBiasObject *GetMotionChannelBiasObjectAt( UINT nIndex );
};

class SNIPEEXPORT HMotionSet : public HHierObject
{
public:
   HFloatProperty *GetTranslationToleranceProperty();
   HFloatProperty *GetRotationToleranceProperty();
   HFloatProperty *GetScaleToleranceProperty();

   UINT GetNumMotionPlaceholders();
   HMotionPlaceholder *GetMotionPlaceholderAt(UINT nIndex);
   HHierObject *AddMotion( HMotion *pMotion );

   UINT GetNumMotionChannelBiasObjects();
   HMotionChannelBiasObject *GetMotionChannelBiasObjectAt( UINT nIndex );
   HMotionChannelBiasObject *GetMotionChannelBiasObject( const String &strName );
   
};
