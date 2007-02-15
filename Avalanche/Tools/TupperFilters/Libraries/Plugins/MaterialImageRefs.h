////////////////////////////////////////////////////////////////////////////
//
// MaterialImageRefs
//
// Class to build up an array of images references by a material
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MaterialImageRefs.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 2:25p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * added bitmap map refs
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 1:07p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * material image refs added
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_MATERIAL_IMAGE_REFS_H
#define TUPPERFILTERS_LIB_PLUGINS_MATERIAL_IMAGE_REFS_H


class TupMaterialPool;
class TupMaterial;
class TupMapPool;

class MaterialImageRefsManager
{
	TupMaterialPool &m_tupMaterialPool;
	TupMapPool &m_tupMapPool;

	MaterialImageRefsManager(void); // this is not defined
public:
	class MaterialImageData
	{
	public:
		int m_imageRef;
		int m_bitmapMapRef; 
	};
	MaterialImageRefsManager(TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool);
	void BuildImageRefs(int materialReference,TupArray<MaterialImageData> &imageRefsArray);
private:
	void AddMaterial(int materialReference,TupArray<MaterialImageData> &imageRefsArray);
	void AddMap(int mapReference,TupArray<MaterialImageData> &imageRefsArray);
};

#endif