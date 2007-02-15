////////////////////////////////////////////////////////////////////////////
//
// MaterialMaps.h
//
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MaterialMaps.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:49a
 * Updated in $/Avalanche/tools/TupperFilters/Stamp
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:29p
 * Updated in $/Avalanche/tools/TupperFilters/Stamp
 */

////////////////////////////////////////////////////////////////////////////

#ifndef MATERIAL_MAPS_H
#define MATERIAL_MAPS_H

class MaterialMapsManager
{
public:
	class ImageLayer
	{
	public:
		int m_imageRef;
		float m_opacity;
		int m_channel;
		int m_blendMode;

		bool operator==(const ImageLayer& other) const;
	};

	MaterialMapsManager(TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool);

	void StartMesh(int materialRef,TupTriMesh &tupTriMesh);
	int GetMaterialIndex (int materialIndex,const TupArray<ImageLayer> &imageLayers);
	int GetMaterialRef(void);
	void FinishMesh(void);
	int GetStartChannel(void);
	int ValidateMaterialIndex(int materialIndex);

private:

	int CreateMaterial(const ImageLayer &imageLayer);
	int CreateMap(const ImageLayer &imageLayer);
	int CreateMaterial(const TupArray<int> &materialStack);

	bool m_bLayersAdded; // indicate if we have added any layers or not
	bool m_bMeshProcessing; // indicate if we are between a StartMesh and a FinishMesh call

	TupMaterialPool &m_tupMaterialPool;
	TupMapPool &m_tupMapPool;

	int m_startChannel; // the first channel to use (uv) for the mesh

	Keeper<ImageLayer> m_imageLayers; // keeps track of all the unique image layers
	TupArray<int> m_imageLayerMaterialRefs; // associated material reference for each image layer

	Keeper<TupArray<int> > m_materialStacks; // keeps track of all stacks of materials for a set of image layers
	TupArray<int> m_materialStackRefs; // associated material refs for the combo materials for the stack

	TupArray<int> m_initialMeshMaterialRefs; // the material references used on the mesh initially
	Keeper<int> m_addedMeshMaterialRefs; // keeps track of the added material references used by the mesh
	
	int m_meshMaterialRef; // new material reference for the mesh
	TupString m_meshMaterialName; // name of new material

};

#endif
