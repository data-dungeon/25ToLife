////////////////////////////////////////////////////////////////////////////
//
// CoordinateSystemConverter
//
// Handle the conversion from one coordinate system to another
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: CoordinateSystemConverter.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/11/03    Time: 4:06p
 * Updated in $/Avalanche/tools/Tupper3d
 * coordinate system converters
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_COORDINATE_SYSTEM_CONVERTER_H
#define TUPPER3D_COORDINATE_SYSTEM_CONVERTER_H

#ifndef TUPPER3D_SCENE_H
#include "scene.h"
#endif

class TupNodePool;
class TupObjectPool;
class TupModifierPool;

class TupCoordinateSystemConverter
{
	// Private Default constructor so no one can use it
	TupCoordinateSystemConverter(void);

	TupScene &m_tupScene;
	TupNodePool &m_tupNodePool;
	TupObjectPool &m_tupObjectPool;
	TupModifierPool &m_tupModifierPool;

public:
	TupCoordinateSystemConverter(TupScene &tupScene,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool);
	bool SupportedConversion(TupScene::UnitFlags fromType,TupScene::UnitFlags toType);
	bool ConvertScene(TupScene::UnitFlags toType);
};

#endif
