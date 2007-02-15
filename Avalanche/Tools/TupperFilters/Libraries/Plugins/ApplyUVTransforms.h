////////////////////////////////////////////////////////////////////////////
//
// ApplyUVTransforms
//
// Class to handle applying a uv transform to a set of uv coordinates
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ApplyUVTransforms.h $
 * 
 * *****************  Version 3  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:55p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/13/02    Time: 11:56a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * apply and build stuff
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_APPLY_UV_TRANSFORMS_H
#define TUPPERFILTERS_LIB_PLUGINS_APPLY_UV_TRANSFORMS_H

class UVTransform;
//class Vector3;

class ApplyUVTransform
{
public:
	static void Apply(const UVTransform &uvTransform,Vector3 &value);
};

#endif