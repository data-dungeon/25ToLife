////////////////////////////////////////////////////////////////////////////
//
// AnnotateHelper
//
// Class to handle accessing data added in by the Annotate Filter
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_ANNOTATE_HELPER_H
#define TUPPERFILTERS_LIB_PLUGINS_ANNOTATE_HELPER_H

class TupConfigRoot;
class ImageManager;
class TupNodePool;
class TupObjectPool;
class TupMaterialPool;
class TupImagePool;
class TupMapPool;
class TupModifierPool;
class FaceCollisionAttributes;
class TupperwareAggregate;
class TupImage;
class Keeper;
class TupTriMesh;

class AnnotateHelper
{
public:
	struct FaceCollisionAttributes
	{
		bool m_collision;
		int m_surfaceType;
		int m_obbValue;
	};

	struct Color
	{
		unsigned char m_red;
		unsigned char m_green;
		unsigned char m_blue;
		unsigned char m_alpha;
	};

	struct ImageInfo
	{
		int m_width;
		int m_height;
		int m_formatFlags;

	};

	struct FaceRenderAttributes
	{
		int m_zbiasValue;
		bool m_bSetTwoSided;
		bool m_bNoZWrite;
		int m_twoSidedValue;
	};

	struct UsedEntry
	{
		TupArray<int> m_usedPartitionCounts;
		int m_totalUsedCount;
	};

	struct UsedMatsMapsAndImages
	{
		TupArray<UsedEntry> m_usedMaterials;
		TupArray<UsedEntry> m_usedMaps;
		TupArray<UsedEntry> m_usedImages;
	};

	static TupperwareAggregate *FindChildVertexColorMeshMap(TupObjectPool &tupObjectPool,int objectRef);
	static int FindChildVertexColorObject(TupObjectPool &tupObjectPool,int objectRef);
	static TupperwareAggregate *FindChildVertexAlphaMeshMap(TupObjectPool &tupObjectPool,int objectRef);
	static int FindChildVertexAlphaObject(TupObjectPool &tupObjectPool,int objectRef);
	static TupperwareAggregate *FindBaseMesh(TupObjectPool &tupObjectPool,int objectRef);
	static TupperwareAggregate *FindBasePath(TupObjectPool &tupObjectPool,int objectRef);
	static TupperwareAggregate *FindBaseXRef(TupObjectPool &tupObjectPool,int objectRef);
	static int FindBaseObjectRef(TupObjectPool &tupObjectPool,int objectRef);
	static int FindBaseMeshRef(TupObjectPool &tupObjectPool,int objectRef);
	static void FindModifiers(TupObjectPool &tupObjectPool,int objectRef,TupArray<int> &modifierReferenceArray,TupArray<TupperwareAggregate *> &modifierContextAggArray);
	static void BuildVertexColors(TupperwareAggregate *pVertexColorMapAgg,AnnotateHelper::Color **pColorArray,int **pColorIndices,int &numColorFaces);
	static TupperwareAggregate *FindFaceAttributes(TupModifierPool &tupModifierPool,TupObjectPool &tupObjectPool,int objectRef);
	static void BuildFaceRenderAttributes(TupConfigRoot &tupFaceTreeRoot,
											TupperwareAggregate *pFaceAttributesAgg,
											FaceRenderAttributes **pFaceAttributeArray,int **pFaceAttributeIndices,
											int &numAttributeFaces);
	static void GetImageInfo(TupImage &tupImage,int frameIndex,int mapIndex,ImageManager &imageManager,ImageInfo &imageInfo);

	static void CalculateUsedMatsMapsAndImages(TupTriMesh &tupTriMesh,
														TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,
														Keeper<int> &usedMaterials,Keeper<int> &usedMaps,Keeper<int> &usedImages);
	static void CalculateUsedMatsMapsAndImages(TupObjectPool &tupObjectPool,
												TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,
												UsedMatsMapsAndImages &usedMatsMapsAndImages,int numPartitions);
	static void CalculateUsedImages(int materialRef,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,Keeper<int> &usedImages);
	static void GetMapsFromMaterial(TupMaterialPool &tupMaterialPool,int materialRef,TupArray<int> &mapRefsArray);
	static void GetMapsFromMap(TupMapPool &tupMapPool,int mapRef,TupArray<int> &subMapRefsArray);
	static void GetImagesFromMap(TupMapPool &tupMapPool,int mapRef,TupArray<int> &imageRefsArray);

	static void CalculateUsedBitmapMapsFromMaterial(TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,int materialRef,Keeper<int> &usedBitmapMapRefs);
	static void CalculateUsedBitmapMapsFromMap(TupMapPool &tupMapPool,int mapRef,Keeper<int> &usedBitmapMapRefs);

	static void RemapMaps(TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupArray<int> &mapRefRemapArray);
	static void RemapMaterialMaps(TupMaterialPool &tupMaterialPool,int materialRef,TupArray<int> &mapRefRemapArray);
	static void RemapMapMaps(TupMapPool &tupMapPool,int mapRef,TupArray<int> &mapRefRemapArray);

	static int RemapMapChannels(TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,int materialRef,
		Keeper<int> &remappedMaterials, TupArray<int> &remappedMaterialRefs,
		Keeper<int> &remappedMaps, TupArray<int> &remappedMapRefs,
		Keeper<int> &imageRefs,TupArray<int> &mapChannels);
	static int RemapMapChannels(TupMapPool &tupMapPool,int mapRef,
		Keeper<int> &remappedMaps, TupArray<int> &remappedMapRefs,Keeper<int> &imageRefs,TupArray<int> &mapChannels);

	static int RemapMapChannels(TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,int materialRef,Keeper<int> &imageRefs,TupArray<int> mapChannels);
	static int RemapMapChannels(TupMapPool &tupMapPool,int mapRef,Keeper<int> &imageRefs,TupArray<int> mapChannels);

	static void RemapMaterials(TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupMaterialPool &tupMaterialPool,TupArray<int> &materialRefRemapArray);
	static unsigned long GetMaterialCRC(TupMaterialPool &tupMaterialPool,int materialRef);
	static unsigned long GetMapCRC(TupMapPool &tupMapPool,int mapRef);
	static bool CompareMaterials(TupMaterialPool &tupMaterialPool,int materialRef1,int materialRef2);
	static bool CompareMaps(TupMapPool &tupMapPool,int mapRef1,int mapRef2);

	static void CalculateUsedNodesObjectsAndModifiers(int nodeRef,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool,
																	  Keeper<int> &usedNodes,Keeper<int> &usedObjects,Keeper<int> &usedModifiers);
	static void CalculateUsedObjectsAndModifiers(int objectRef,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool,
																	  Keeper<int> &usedObjects,Keeper<int> &usedModifiers);
	static void FindAllParentDerivedObjects(int objectRef,TupObjectPool &tupObjectPool,Keeper<int> &parentObjects);

};

#endif
