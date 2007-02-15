////////////////////////////////////////////////////////////////////////////
//
// MultiSubObjectMaterialConverter
//
// Convert from using multisubobjectmaterials to using material refs for each face
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MultiSubObjectMaterialConverter.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/18/03    Time: 3:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * multi sub object material converter
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_MULTI_SUB_OBJECT_MATERIAL_CONVERTER_H
#define TUPPER3D_MULTI_SUB_OBJECT_MATERIAL_CONVERTER_H

#ifndef TUPPER3D_OBJECT_H
#include "object.h"
#endif

class TupNodePool;
class TupObjectPool;
class TupMaterialPool;

class TupMultiSubObjectMaterialConverter
{
	// Private Default constructor so no one can use it
	TupMultiSubObjectMaterialConverter(void);

	TupNodePool &m_tupNodePool;
	TupObjectPool &m_tupObjectPool;
	TupMaterialPool &m_tupMaterialPool;

public:
	TupMultiSubObjectMaterialConverter(TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupMaterialPool &tupMaterialPool);
	bool ConvertScene(void);
};

#endif
