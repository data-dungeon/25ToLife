////////////////////////////////////////////////////////////////////////////
//
// CoordinateSystemConverter
//
// Handle the conversion from one coordinate system to another
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: CoordinateSystemConverter.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/11/03    Time: 4:06p
 * Updated in $/Avalanche/tools/Tupper3d
 * coordinate system converters
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "CoordinateSystemConverter.h"
#include "NodePool.h"
#include "ObjectPool.h"
#include "ModifierPool.h"
#include "ZupToYupConverter.h"

TupCoordinateSystemConverter::TupCoordinateSystemConverter(TupScene &tupScene,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool) :
	m_tupScene(tupScene),
	m_tupNodePool(tupNodePool),
	m_tupObjectPool(tupObjectPool),
	m_tupModifierPool(tupModifierPool)
{
}

bool TupCoordinateSystemConverter::SupportedConversion(TupScene::UnitFlags fromType,TupScene::UnitFlags toType)
{
	if (fromType==TupScene::UP_AXIS_Z && toType==TupScene::UP_AXIS_Y)
	{
		return true;
	}
	return false;
}

bool TupCoordinateSystemConverter::ConvertScene(TupScene::UnitFlags toType)
{
	int unitFlags;
	TupperwareScalar *pUnitsScalar = m_tupScene.GetUnitFlags(unitFlags);
	if (pUnitsScalar && ((unitFlags & TupScene::UP_AXIS_MASK) == TupScene::UP_AXIS_Y))
	{
		return false;
	}
	else if (toType == TupScene::UP_AXIS_Y)
	{
		return TupZupToYupConverter::ConvertScene(m_tupScene,m_tupNodePool,m_tupObjectPool,m_tupModifierPool);
	}
	return false;
}

