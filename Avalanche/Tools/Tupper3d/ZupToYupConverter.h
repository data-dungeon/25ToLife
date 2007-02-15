////////////////////////////////////////////////////////////////////////////
//
// ZupToYupConverter
//
// Handle the conversion from a zup coordinate system to yup
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ZupToYupConverter.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/11/03    Time: 4:06p
 * Updated in $/Avalanche/tools/Tupper3d
 * coordinate system converters
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_ZUP_TO_YUP_CONVERTER_H
#define TUPPER3D_ZUP_TO_YUP_CONVERTER_H

class TupScene;
class TupNodePool;
class TupObjectPool;
class TupModifierPool;
class TupVertex3;

class TupZupToYupConverter
{
public:
	static bool ConvertScene(TupScene &tupScene,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool);
	
private:
	TupScene &m_tupScene;
	TupNodePool &m_tupNodePool;
	TupObjectPool &m_tupObjectPool;
	TupModifierPool &m_tupModifierPool;
	
	TupZupToYupConverter(TupScene &tupScene,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool);

	bool TransformScene(void);
	bool TransformObject(TupperwareAggregate *pObjectAgg);
	bool TransformModifier(TupperwareAggregate *pDerivedModifierAgg);
	bool TransformNode(TupperwareAggregate *pNodeAgg);
};

#endif
