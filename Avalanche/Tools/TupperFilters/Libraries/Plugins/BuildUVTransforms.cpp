////////////////////////////////////////////////////////////////////////////
//
// BuildUVTransforms
//
// Class to build up data relating to the bitmapmap uv transforms
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "PluginsPch.h"
#include "BuildUVTransforms.h"
#include "UVTransforms.h"

// BuildUVTransformManager methods
BuildUVTransformManager::BuildUVTransformManager(TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,bool bPs2) :
	m_tupMaterialPool(tupMaterialPool),m_tupMapPool(tupMapPool),m_tupImagePool(tupImagePool),m_bPs2(bPs2)
{
}

void BuildUVTransformManager::BuildUVTransforms(int materialReference,TupArray<UVTransform> &uvTransformArray)
{
	uvTransformArray.RemoveAll();
	AddMaterial(materialReference,uvTransformArray);
	// make sure there is one here if we are the ps2 platform
	if (uvTransformArray.GetSize()==0 && m_bPs2)
	{
		UVTransform newTransform;
		uvTransformArray.Add(newTransform);
	}
}

void BuildUVTransformManager::AddMaterial(int materialReference,TupArray<UVTransform> &uvTransformArray)
{
	if (materialReference!=-1)
	{
		TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial(materialReference);
		if (pMaterialAgg)
		{
			TupMaterial tupMaterial(pMaterialAgg);
			switch (tupMaterial.GetMaterialType())
			{
				case TupMaterial::STANDARD_MATERIAL:
					{
						TupStandardMaterial tupStandardMaterial(tupMaterial);

						// look for diffuse color
						int totalMaps = tupStandardMaterial.GetNumMaps();
						int mapIndex;
						for(mapIndex=0;mapIndex<totalMaps;mapIndex++ )
						{
							TupperwareAggregate *pMapAgg = tupStandardMaterial.GetMap(mapIndex);
							TupStandardMaterialMap tupMaterialMap(pMapAgg);
							char *mapName;
							tupMaterialMap.GetMapName(&mapName);
							assert(mapName);
							if (!stricmp(mapName,"Diffuse Color"))
							{
								int mapRef;
								tupMaterialMap.GetMapReference(mapRef);
								AddMap(mapRef,uvTransformArray);
							}
						}
						// look for reflection
						for(mapIndex=0;mapIndex<totalMaps;mapIndex++ )
						{
							TupperwareAggregate *pMapAgg = tupStandardMaterial.GetMap(mapIndex);
							TupStandardMaterialMap tupMaterialMap(pMapAgg);
							char *mapName;
							tupMaterialMap.GetMapName(&mapName);
							assert(mapName);
							if (!stricmp(mapName,"Reflection"))
							{
								int mapRef;
								tupMaterialMap.GetMapReference(mapRef);
								AddMap(mapRef,uvTransformArray);
							}
						}
					}
					break;
				default:
//					assert(false);
					break;
			}
		}
	}
}

void BuildUVTransformManager::AddMap(int mapReference,TupArray<UVTransform> &uvTransformArray)
{
	assert(mapReference!=-1);
	// if we can't add any more then stop
	if (uvTransformArray.GetSize()==MAX_SOM_TEXTURES_PER_PATCH)
		return;

	TupperwareAggregate *pMapAgg = m_tupMapPool.GetMap(mapReference);
	TupMap tupMap(pMapAgg);
	switch (tupMap.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			{
				UVTransform newTransform;
				BuildUVTransform(mapReference,newTransform);
				uvTransformArray.Add(newTransform);
			}
			break;
		case TupMap::COMPOSITE_MAP:
			{
				TupCompositeMap tupCompositeMap(tupMap);
				int numSubMaps = tupCompositeMap.GetNumSubMaps();
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupCompositeMap.GetSubMap(subMapIndex);
					TupCompositeMapSubMap tupSubMap(pSubMapAgg);
					int mapRef;
					tupSubMap.GetMapReference(mapRef);
					AddMap(mapRef,uvTransformArray);
				}
			}
			break;
		default:
//			assert(false);
			break;
	}
}

void BuildUVTransformManager::BuildUVTransform(int mapReference,UVTransform &uvTransform)
{
	TupperwareAggregate *pMapAgg = m_tupMapPool.GetMap(mapReference);
	TupMap tupMap(pMapAgg);
	assert(tupMap.GetMapType()==TupMap::BITMAP_MAP);
	TupBitmapMap tupBitmapMap(tupMap);
	tupBitmapMap.GetMapChannel(uvTransform.m_mapChannel);

	int flags;
	tupBitmapMap.GetFlags(flags);
	if (flags & TupBitmapMap::TUP_U_WRAP)
	{
		uvTransform.m_flags |= UVTransform::FLAGS_U_WRAP;
	}
	if (flags & TupBitmapMap::TUP_V_WRAP)
	{
		uvTransform.m_flags |= UVTransform::FLAGS_V_WRAP;
	}
	if (flags & TupBitmapMap::ALPHA_SOURCE_IMAGE_ALPHA)
	{
		uvTransform.m_flags |= UVTransform::FLAGS_USE_IMAGE_ALPHA;
	}
	if (flags & TupBitmapMap::TUP_OUTPUT_INVERT)
	{
		uvTransform.m_flags |= UVTransform::FLAGS_INVERT_ALPHA;
	}
	switch (flags & TupBitmapMap::TUP_UV_TRANSFORM_TYPE_MASK)
	{
		case TupBitmapMap::TUP_UV_TRANSFORM_TYPE_MAX:
			uvTransform.m_flags |= UVTransform::FLAGS_UV_TRANSFORM_TYPE_MAX;
			break;
		case TupBitmapMap::TUP_UV_TRANSFORM_TYPE_MAYA:
			uvTransform.m_flags |= UVTransform::FLAGS_UV_TRANSFORM_TYPE_MAYA;
			break;
		case TupBitmapMap::TUP_UV_TRANSFORM_TYPE_HASH:
			uvTransform.m_flags |= UVTransform::FLAGS_UV_TRANSFORM_TYPE_HASH;
			break;
	}
	tupBitmapMap.GetUOffset(uvTransform.m_UOffset);
	tupBitmapMap.GetVOffset(uvTransform.m_VOffset);
	tupBitmapMap.GetUTiling(uvTransform.m_UTiling);
	tupBitmapMap.GetVTiling(uvTransform.m_VTiling);
	float UAngle,VAngle,WAngle;
	tupBitmapMap.GetUAngle(UAngle);
	tupBitmapMap.GetVAngle(VAngle);
	tupBitmapMap.GetWAngle(WAngle);
	uvTransform.m_rotationMatrix.Identity();
	uvTransform.m_rotationMatrix.CatYRotation(UAngle);
	uvTransform.m_rotationMatrix.CatXRotation(VAngle);
	uvTransform.m_rotationMatrix.CatZRotation(WAngle);
	int imageReference;
	tupBitmapMap.GetImageReference(imageReference);
	if (imageReference!=-1)
	{
		uvTransform.m_mapRef = mapReference;
		uvTransform.m_imageRef = imageReference;
		TupperwareAggregate *pImageAgg = m_tupImagePool.GetImage(imageReference);
		uvTransform.m_numFrames = pImageAgg->FindScalarByKey("Annotate_NumFrames")->GetAsInt();
		uvTransform.m_imageWidth = pImageAgg->FindScalarByKey("Annotate_ImageWidth")->GetAsInt();
		uvTransform.m_imageHeight = pImageAgg->FindScalarByKey("Annotate_ImageHeight")->GetAsInt();
		if (pImageAgg->FindScalarByKey("Annotate_ImageHasAlpha")->GetAsInt())
			uvTransform.m_flags |= UVTransform::FLAG_IMAGE_HAS_ALPHA;
		if (pImageAgg->FindScalarByKey("Annotate_ImageHas8BitAlpha")->GetAsInt())
			uvTransform.m_flags |= UVTransform::FLAG_IMAGE_HAS_8BIT_ALPHA;
		TupperwareScalar *pRotatedImageScalar = pImageAgg->FindScalarByKey("Annotate_ImageReversed");
		if (pRotatedImageScalar)
			uvTransform.m_flags |= UVTransform::FLAG_REVERSED_IMAGE;
	}
}
