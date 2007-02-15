////////////////////////////////////////////////////////////////////////////
//
// BuildUVTransforms
//
// Class to build up data relating to the bitmapmap uv transforms
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: BuildUVTransforms.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 10/02/02   Time: 11:27a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Changed UV transform classes
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/20/02    Time: 12:55p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * added ps2 flag
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/18/02    Time: 3:55p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * added build for map
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/13/02    Time: 11:56a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * apply and build stuff
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_BUILD_UV_TRANSFORMS_H
#define TUPPERFILTERS_LIB_PLUGINS_BUILD_UV_TRANSFORMS_H

#include <Platform/BaseType.h>
#include <Math/Matrix.h>

class TupMaterialPool;
class TupMaterial;
class TupMapPool;
class TupImagePool;
class UVTransform;

class BuildUVTransformManager
{
	TupMaterialPool &m_tupMaterialPool;
	TupMapPool &m_tupMapPool;
	TupImagePool &m_tupImagePool;

	bool m_bPs2; // indicates if this needs to be built according to the ps2 platform

	BuildUVTransformManager(void); // this is not defined
public:
	BuildUVTransformManager(TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,bool bPs2);
	void BuildUVTransforms(int materialReference,TupArray<UVTransform> &uvTransformArray);
	void BuildUVTransform(int mapReference,UVTransform &uvTransform);
private:
	void AddMaterial(int materialReference,TupArray<UVTransform> &uvTransformArray);
	void AddMap(int mapReference,TupArray<UVTransform> &uvTransformArray);
};

#endif