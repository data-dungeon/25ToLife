////////////////////////////////////////////////////////////////////////////
//
// AnnotateHelper
//
// Class to handle accessing data added in by the Annotate Filter
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "PluginsPch.h"
#include "AnnotateHelper.h"
#include "imageManager.h"
#include "CRC.h"

TupperwareAggregate *AnnotateHelper::FindChildVertexColorMeshMap(TupObjectPool &tupObjectPool,int objectRef)
{
	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectRef);
	assert(pObjectAgg);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		TupperwareAggregate *pVertexColorMeshMapAgg = pObjectAgg->FindAggregateByKeyAndName("Annotate_VertexColorMeshMap","0");
		// if we found one on me then return
		if (pVertexColorMeshMapAgg)
			return pVertexColorMeshMapAgg;
		// if not try the one I point to
		TupDerived tupDerived(pObjectAgg);
		int childObjectRef;
		tupDerived.GetObjectReference(childObjectRef);
		assert(childObjectRef>=0);
		return FindChildVertexColorMeshMap(tupObjectPool,childObjectRef);
	}
	else if (tupObject.GetObjectType()==TupObject::TRI_MESH)
	{
		TupTriMesh tupTriMesh(pObjectAgg);
		int numMeshMaps = tupTriMesh.GetNumMeshMaps();
		for (int meshMapIndex = 0;meshMapIndex<numMeshMaps;meshMapIndex++)
		{
			TupperwareAggregate *pMeshMapAgg = tupTriMesh.GetMeshMap(meshMapIndex);
			TupTriMeshMap tupTriMeshMap(pMeshMapAgg);
			if (tupTriMeshMap.GetMapChannel()==TupTriMeshMap::MAP_CHANNEL_VERTEX_COLOR)
			{
				return pMeshMapAgg; // return the aggregate to the mesh map itself
			}
		}
	}
	return NULL;
}

int AnnotateHelper::FindChildVertexColorObject(TupObjectPool &tupObjectPool,int objectRef)
{
	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectRef);
	assert(pObjectAgg);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		TupperwareAggregate *pVertexColorMeshMapAgg = pObjectAgg->FindAggregateByKeyAndName("Annotate_VertexColorMeshMap","0");
		// if we found one on me then return
		if (pVertexColorMeshMapAgg)
			return objectRef;
		// if not try the one I point to
		TupDerived tupDerived(pObjectAgg);
		int childObjectRef;
		tupDerived.GetObjectReference(childObjectRef);
		assert(childObjectRef>=0);
		return FindChildVertexColorObject(tupObjectPool,childObjectRef);
	}
	else if (tupObject.GetObjectType()==TupObject::TRI_MESH)
	{
		TupTriMesh tupTriMesh(pObjectAgg);
		int numMeshMaps = tupTriMesh.GetNumMeshMaps();
		for (int meshMapIndex = 0;meshMapIndex<numMeshMaps;meshMapIndex++)
		{
			TupperwareAggregate *pMeshMapAgg = tupTriMesh.GetMeshMap(meshMapIndex);
			TupTriMeshMap tupTriMeshMap(pMeshMapAgg);
			if (tupTriMeshMap.GetMapChannel()==TupTriMeshMap::MAP_CHANNEL_VERTEX_COLOR)
			{
				return objectRef; // return the object reference
			}
		}
	}
	return -1;
}

TupperwareAggregate *AnnotateHelper::FindChildVertexAlphaMeshMap(TupObjectPool &tupObjectPool,int objectRef)
{
	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectRef);
	assert(pObjectAgg);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		TupperwareAggregate *pVertexColorMeshMapAgg = pObjectAgg->FindAggregateByKeyAndName("Annotate_VertexAlphaMeshMap","-2");
		// if we found one on me then return
		if (pVertexColorMeshMapAgg)
			return pVertexColorMeshMapAgg;
		// if not try the one I point to
		TupDerived tupDerived(pObjectAgg);
		int childObjectRef;
		tupDerived.GetObjectReference(childObjectRef);
		assert(childObjectRef>=0);
		return FindChildVertexAlphaMeshMap(tupObjectPool,childObjectRef);
	}
	else if (tupObject.GetObjectType()==TupObject::TRI_MESH)
	{
		TupTriMesh tupTriMesh(pObjectAgg);
		int numMeshMaps = tupTriMesh.GetNumMeshMaps();
		for (int meshMapIndex = 0;meshMapIndex<numMeshMaps;meshMapIndex++)
		{
			TupperwareAggregate *pMeshMapAgg = tupTriMesh.GetMeshMap(meshMapIndex);
			TupTriMeshMap tupTriMeshMap(pMeshMapAgg);
			if (tupTriMeshMap.GetMapChannel()==TupTriMeshMap::MAP_CHANNEL_ALPHA)
			{
				return pMeshMapAgg; // return the aggregate to the mesh map itself
			}
		}
	}
	return NULL;
}

int AnnotateHelper::FindChildVertexAlphaObject(TupObjectPool &tupObjectPool,int objectRef)
{
	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectRef);
	assert(pObjectAgg);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		TupperwareAggregate *pVertexColorMeshMapAgg = pObjectAgg->FindAggregateByKeyAndName("Annotate_VertexAlphaMeshMap","-2");
		// if we found one on me then return
		if (pVertexColorMeshMapAgg)
			return objectRef;
		// if not try the one I point to
		TupDerived tupDerived(pObjectAgg);
		int childObjectRef;
		tupDerived.GetObjectReference(childObjectRef);
		assert(childObjectRef>=0);
		return FindChildVertexAlphaObject(tupObjectPool,childObjectRef);
	}
	else if (tupObject.GetObjectType()==TupObject::TRI_MESH)
	{
		TupTriMesh tupTriMesh(pObjectAgg);
		int numMeshMaps = tupTriMesh.GetNumMeshMaps();
		for (int meshMapIndex = 0;meshMapIndex<numMeshMaps;meshMapIndex++)
		{
			TupperwareAggregate *pMeshMapAgg = tupTriMesh.GetMeshMap(meshMapIndex);
			TupTriMeshMap tupTriMeshMap(pMeshMapAgg);
			if (tupTriMeshMap.GetMapChannel()==TupTriMeshMap::MAP_CHANNEL_ALPHA)
			{
				return objectRef; // return the object reference
			}
		}
	}
	return -1;
}

TupperwareAggregate *AnnotateHelper::FindBaseMesh(TupObjectPool &tupObjectPool,int objectRef)
{
	int baseRef = FindBaseObjectRef(tupObjectPool,objectRef);
	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(baseRef);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::TRI_MESH)
	{
		return pObjectAgg;
	}
	else
	{
		return NULL;
	}
}

TupperwareAggregate *AnnotateHelper::FindBasePath(TupObjectPool &tupObjectPool,int objectRef)
{
	int baseRef = FindBaseObjectRef(tupObjectPool,objectRef);
	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(baseRef);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::BEZIER_SHAPE ||
		 tupObject.GetObjectType()==TupObject::SPLINE_SHAPE)
	{
		return pObjectAgg;
	}
	else
	{
		return NULL;
	}
}

TupperwareAggregate *AnnotateHelper::FindBaseXRef(TupObjectPool &tupObjectPool,int objectRef)
{
	int baseRef = FindBaseObjectRef(tupObjectPool,objectRef);
	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(baseRef);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::XREF)
	{
		return pObjectAgg;
	}
	else
	{
		return NULL;
	}
}

int AnnotateHelper::FindBaseObjectRef(TupObjectPool &tupObjectPool,int objectRef)
{
	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectRef);
	assert(pObjectAgg);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		// try the one I point to
		TupDerived tupDerived(pObjectAgg);
		int childObjectRef;
		tupDerived.GetObjectReference(childObjectRef);
		assert(childObjectRef>=0);
		return FindBaseObjectRef(tupObjectPool,childObjectRef);
	}
	return objectRef;
}

int AnnotateHelper::FindBaseMeshRef(TupObjectPool &tupObjectPool,int objectRef)
{
	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectRef);
	assert(pObjectAgg);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		// try the one I point to
		TupDerived tupDerived(pObjectAgg);
		int childObjectRef;
		tupDerived.GetObjectReference(childObjectRef);
		assert(childObjectRef>=0);
		return FindBaseMeshRef(tupObjectPool,childObjectRef);
	}
	else if (tupObject.GetObjectType()==TupObject::TRI_MESH)
	{
		return objectRef;
	}
	return -1; // not a mesh at the end
}


// go through the derived objects and add information about the modifiers to the arrays
void AnnotateHelper::FindModifiers(TupObjectPool &tupObjectPool,int objectRef,TupArray<int> &modifierReferenceArray,TupArray<TupperwareAggregate *> &modifierContextAggArray)
{
	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectRef);
	assert(pObjectAgg);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		// try the one I point to
		TupDerived tupDerived(pObjectAgg);
		int numModifierSlots = tupDerived.GetNumModifierSlots();
		for (int modifierIndex=0;modifierIndex<numModifierSlots;modifierIndex++)
		{
			TupperwareAggregate *pTupDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
			if (pTupDerivedModifierAgg)
			{
				TupDerivedModifier tupDerivedModifier(pTupDerivedModifierAgg);
				int modifierReference;
				tupDerivedModifier.GetModifierReference(modifierReference);
				modifierReferenceArray.Add(modifierReference);
				TupperwareAggregate *pTupModifierContextAgg = tupDerivedModifier.GetModifierContext();
				modifierContextAggArray.Add(pTupModifierContextAgg);
			}
		}
		int childObjectRef;
		tupDerived.GetObjectReference(childObjectRef);
		assert(childObjectRef>=0);
		FindModifiers(tupObjectPool,childObjectRef,modifierReferenceArray,modifierContextAggArray);
	}
}



void AnnotateHelper::BuildVertexColors(TupperwareAggregate *pVertexColorMapAgg,AnnotateHelper::Color **pColorArray,int **pColorIndices,int &numColorFaces)
{
	bool built = false;
	if (pVertexColorMapAgg)
	{
		TupTriMeshMap tupTriMeshMap(pVertexColorMapAgg);
		Vector3 *pColorVerts;
		int *pMapColorIndices;
		int numColorVerts;
		tupTriMeshMap.GetMapVerts((float **)&pColorVerts,numColorVerts);
		tupTriMeshMap.GetMapIndices(&pMapColorIndices,numColorFaces); // set the number of faces we support

		if (numColorFaces)
		{
			built = true;
			assert(numColorVerts);
			AnnotateHelper::Color *pNewColorArray = new AnnotateHelper::Color[numColorVerts];
			*pColorArray = pNewColorArray;
			// create a new pool that is in the right format
			for (int colorVertIndex = 0;colorVertIndex<numColorVerts;colorVertIndex++)
			{
				pNewColorArray[colorVertIndex].m_red = (unsigned char)(pColorVerts[colorVertIndex].x()*255.0f);
				pNewColorArray[colorVertIndex].m_green = (unsigned char)(pColorVerts[colorVertIndex].y()*255.0f);
				pNewColorArray[colorVertIndex].m_blue = (unsigned char)(pColorVerts[colorVertIndex].z()*255.0f);
				pNewColorArray[colorVertIndex].m_alpha = 255;
			}
			// allocate the indices and copy
			*pColorIndices = new int[numColorFaces*3];
			memcpy(*pColorIndices,pMapColorIndices,sizeof(int)*numColorFaces*3);
		}
	}
	// if we didn't build the colors then build a simple one
	if (!built)
	{
		// allocate a single color
		*pColorArray = new AnnotateHelper::Color[1];
		// make it white
		(*pColorArray)[0].m_red = 255;
		(*pColorArray)[0].m_green = 255;
		(*pColorArray)[0].m_blue = 255;
		(*pColorArray)[0].m_alpha = 255;
		// allocate a indices for a single face
		*pColorIndices = new int[3];
		(*pColorIndices)[0] = 0;
		(*pColorIndices)[1] = 0;
		(*pColorIndices)[2] = 0;
		numColorFaces = 1; // set the number of faces we support to 1
	}
}

TupperwareAggregate *AnnotateHelper::FindFaceAttributes(TupModifierPool &tupModifierPool,TupObjectPool &tupObjectPool,int objectRef)
{
	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectRef);
	assert(pObjectAgg);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		TupDerived tupDerived(pObjectAgg);
		int totalModifierSlots = tupDerived.GetNumModifierSlots();
		// if there is any modifiers
		if (totalModifierSlots)
		{
			bool foundValidModifier = false;
			// loop through the modifiers and see if any deal with the vertex colors
			for (int modifierIndex=0;modifierIndex<totalModifierSlots;modifierIndex++)
			{
				TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
				if (pDerivedModifierAgg)
				{
					TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
					int modifierReference;
					tupDerivedModifier.GetModifierReference(modifierReference);
					TupperwareAggregate *pModifierAgg = tupModifierPool.GetModifier(modifierReference);
					TupModifier tupModifier(pModifierAgg);
					switch(tupModifier.GetModifierType())
					{
						case TupModifier::FACE_ATTRIBUTE_MODIFIER:
							{
								TupFaceAttributesModifier tupFaceAttributesModifier(pModifierAgg);
								int flags;
								tupFaceAttributesModifier.GetFlags(flags);
								if (flags&TupApplyVCModifier::FLAGS_ENABLED)
								{
									// return the modifier context (this holds the face data)
									return tupDerivedModifier.GetModifierContext();
								}
							}
							break;
					}
				}
			}
		}
		// we didn't find a modifier (try the next object)
		int childObjectRef;
		tupDerived.GetObjectReference(childObjectRef);
		assert(childObjectRef>=0);
		return FindFaceAttributes(tupModifierPool,tupObjectPool,childObjectRef);
	}
	return NULL;
}

void AnnotateHelper::BuildFaceRenderAttributes(TupConfigRoot &tupFaceTreeRoot,
											TupperwareAggregate *pFaceAttributesAgg,
											FaceRenderAttributes **pFaceAttributeArray,int **pFaceAttributeIndices,
											int &numAttributeFaces)
{
	bool built = false;

	// if there are some face Attributes
	if (pFaceAttributesAgg)
	{
		TupFaceAttributesModifierContext tupFaceAttributesModifierContext(pFaceAttributesAgg);

		char **pAttributeVerts;
		int numAttributeVerts;
		int *pAttributeIndices;
		tupFaceAttributesModifierContext.GetFaceAttributeVerts(&pAttributeVerts,numAttributeVerts);
		tupFaceAttributesModifierContext.GetFaceAttributeIndices(&pAttributeIndices,numAttributeFaces);
		if (numAttributeFaces)
		{
			built = true;
			assert(numAttributeVerts);
			*pFaceAttributeArray = new FaceRenderAttributes[numAttributeVerts];

			// go through each attribute and fill in each with proper data
			for (int attributesIndex=0;attributesIndex<numAttributeVerts;attributesIndex++)
			{
				FaceRenderAttributes &faceAttributes = (*pFaceAttributeArray)[attributesIndex];
				// set defaults
				faceAttributes.m_bSetTwoSided = false;
				faceAttributes.m_twoSidedValue = 0;
				faceAttributes.m_zbiasValue = 0;
				faceAttributes.m_bNoZWrite = false;

				TupKeyValue tupAttributes(pAttributeVerts[attributesIndex],NULL,NULL);
				TupKeyValueContext tupKeyValueContext;
				tupKeyValueContext.AddKey("ZBias");
				tupKeyValueContext.AddKey("TwoSidedFace");
				tupKeyValueContext.AddKey("NoZWrite");
				tupFaceTreeRoot.GetKeyValueContexts(tupAttributes,tupKeyValueContext,false);

				int zBiasKey = tupKeyValueContext.KeyExistsAt("ZBias");
				int twoSidedKey = tupKeyValueContext.KeyExistsAt("TwoSidedFace");
				int noZWriteKey = tupKeyValueContext.KeyExistsAt("NoZWrite");

				if (zBiasKey!=-1 && tupKeyValueContext.ValueValidAt(zBiasKey))
				{
					faceAttributes.m_zbiasValue = tupKeyValueContext.GetValueAsInt(zBiasKey);
				}
				if (twoSidedKey!=-1 && tupKeyValueContext.ValueValidAt(twoSidedKey))
				{
					if (!strcmp(tupKeyValueContext.GetValueAsString(twoSidedKey),"FromMaterial"))
					{
						faceAttributes.m_bSetTwoSided = false;
					}
					else if (!strcmp(tupKeyValueContext.GetValueAsString(twoSidedKey),"Disabled"))
					{
						faceAttributes.m_bSetTwoSided = true;
						faceAttributes.m_twoSidedValue = 0;
					}
					else if (!strcmp(tupKeyValueContext.GetValueAsString(twoSidedKey),"Enabled"))
					{
						faceAttributes.m_bSetTwoSided = true;
						faceAttributes.m_twoSidedValue = 1;
					}
				}
				if (noZWriteKey!=-1 && tupKeyValueContext.ValueValidAt(noZWriteKey))
				{
					faceAttributes.m_bNoZWrite = tupKeyValueContext.GetValueAsBool(noZWriteKey);
				}

			}
			// copy the indices to an allocated buffer
			*pFaceAttributeIndices = new int[numAttributeFaces];
			memcpy(*pFaceAttributeIndices,pAttributeIndices,sizeof(int)*numAttributeFaces);
		}
	}
	// if we didn't build one then build one with the defaults
	if (!built)
	{
		*pFaceAttributeArray = new FaceRenderAttributes[1];
		(*pFaceAttributeArray)[0].m_bSetTwoSided = false;
		(*pFaceAttributeArray)[0].m_twoSidedValue = 0;
		(*pFaceAttributeArray)[0].m_zbiasValue = 0;
		(*pFaceAttributeArray)[0].m_bNoZWrite = false;
		numAttributeFaces = 1;
		// one face index
		*pFaceAttributeIndices = new int[1];
		(*pFaceAttributeIndices)[0] = 0;
	}
}


void AnnotateHelper::GetImageInfo(TupImage &tupImage,int frameIndex,int mapIndex,ImageManager &imageManager,AnnotateHelper::ImageInfo &imageInfo)
{
	char *pImageFileName;
	// get filename for image
	tupImage.GetFilePathName(&pImageFileName);
	// get image data from image manager
	TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName);
	assert(pImageDataAgg);
	// get the first frame from the image data
	TupImageData tupImageData(pImageDataAgg);
	int numFrames = tupImageData.GetNumImageDataFrames();
	assert(numFrames>0);
	TupperwareAggregate *pImageDataFrame = tupImageData.GetImageDataFrame(frameIndex); // first frame only (for now)
	assert(pImageDataFrame);
	TupImageDataFrame tupImageDataFrame(pImageDataFrame);
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	// get first map from frame
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	assert(numMaps>0);
	TupperwareAggregate *pImageDataMap = tupImageDataFrame.GetImageDataMap(mapIndex);
	assert(pImageDataMap);
	TupImageDataMap tupImageDataMap(pImageDataMap);
	// get the bitmap data
	int width;
	tupImageDataMap.GetWidth(width);
	int height;
	tupImageDataMap.GetHeight(height);

	imageInfo.m_width = width;
	imageInfo.m_height = height;
	imageInfo.m_formatFlags = formatFlags;

}

// calculates which materials (not multi-sub object materials) were actually assigned to a 
// mesh or multi - sub object material
// NOTE: must be called after the annotate filter's OneMaterialPerMesh (pass1)
// the used keepers give a list of used references
void AnnotateHelper::CalculateUsedMatsMapsAndImages(TupTriMesh &tupTriMesh,
													TupMaterialPool &tupMaterialPool,
													TupMapPool &tupMapPool,
													TupImagePool &tupImagePool,
													Keeper<int> &usedMaterials,
													Keeper<int> &usedMaps,
													Keeper<int> &usedImages)
{
	int materialRef = tupTriMesh.GetBaseAggregate()->FindScalarByKey("Annotate_MaterialRef")->GetAsInt();
	if (materialRef!=-1)
	{
		int *pMaterialIndices;
		int numFaces,faceIndex;
		tupTriMesh.GetFaceMaterialIndices(&pMaterialIndices,numFaces);
		int *pFaceFlags;
		tupTriMesh.GetFaceFlags(&pFaceFlags,numFaces);

		TupperwareAggregate *pMaterialAgg = tupMaterialPool.GetMaterial(materialRef);
		TupMaterial tupMaterial(pMaterialAgg);
		if (tupMaterial.GetMaterialType() == TupMaterial::MULTI_SUB_OBJECT_MATERIAL)
		{
			usedMaterials.Add(materialRef);
			TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
			int numSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();
			if (numSubMaterials)
			{
				TupArray<int> subMaterialRefs;
				subMaterialRefs.SetSize(numSubMaterials);
				TupArray<int> usedSubMaterials;
				usedSubMaterials.SetSize(numSubMaterials);
				int subMaterialIndex;
				for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
				{
					usedSubMaterials[subMaterialIndex] = 0;

					TupperwareAggregate *pSubObjectMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(subMaterialIndex);
					TupMultiSubObjectMaterialSubMaterial tupMultiSubObjectMaterialSubMaterial(pSubObjectMaterialAgg);
					int subMaterialRef;
					tupMultiSubObjectMaterialSubMaterial.GetMaterialReference(subMaterialRef);
					subMaterialRefs[subMaterialIndex] = subMaterialRef;
				}
				// go through the faces and count how many used which material
				for (faceIndex=0;faceIndex<numFaces;faceIndex++)
				{
					usedSubMaterials[pMaterialIndices[faceIndex]]++; // mark how many faces used the sub material
				}
				for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
				{
					if (usedSubMaterials[subMaterialIndex] && subMaterialRefs[subMaterialIndex]!=-1)
					{
						usedMaterials.Add(subMaterialRefs[subMaterialIndex]);
						TupArray<int> mapRefsArray;
						// do the maps
						GetMapsFromMaterial(tupMaterialPool,subMaterialRefs[subMaterialIndex],mapRefsArray);
						int numMaps = mapRefsArray.GetSize();
						for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
						{
							usedMaps.Add(mapRefsArray[mapIndex]);
							TupArray<int> imageRefsArray;
							// do the images
							GetImagesFromMap(tupMapPool,mapRefsArray[mapIndex],imageRefsArray);
							int numImages = imageRefsArray.GetSize();
							for (int imageIndex=0;imageIndex<numImages;imageIndex++)
							{
								int imageRef = imageRefsArray[imageIndex];
								TupperwareAggregate *pImageAgg = tupImagePool.GetImage(imageRef);
								assert(pImageAgg);
								// remap to other image ref (duplicate image data)
								TupperwareScalar *pRemapImageRefScalar = pImageAgg->FindScalarByKey("Annotate_RemapImageRef");
								if (pRemapImageRefScalar)
								{
									imageRef = pRemapImageRefScalar->GetAsInt();
								}
								usedImages.Add(imageRef);
							}
						}
					}
				}
			}
		}
		else
		{
			usedMaterials.Add(materialRef);
			TupArray<int> mapRefsArray;
			// do the maps
			GetMapsFromMaterial(tupMaterialPool,materialRef,mapRefsArray);
			int numMaps = mapRefsArray.GetSize();
			for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
			{
				usedMaps.Add(mapRefsArray[mapIndex]);
				TupArray<int> imageRefsArray;
				// do the images
				GetImagesFromMap(tupMapPool,mapRefsArray[mapIndex],imageRefsArray);
				int numImages = imageRefsArray.GetSize();
				for (int imageIndex=0;imageIndex<numImages;imageIndex++)
				{
					int imageRef = imageRefsArray[imageIndex];
					TupperwareAggregate *pImageAgg = tupImagePool.GetImage(imageRef);
					assert(pImageAgg);
					// remap to other image ref (duplicate image data)
					TupperwareScalar *pRemapImageRefScalar = pImageAgg->FindScalarByKey("Annotate_RemapImageRef");
					if (pRemapImageRefScalar)
					{
						imageRef = pRemapImageRefScalar->GetAsInt();
					}
					usedImages.Add(imageRef);
				}
			}
		}
	}
}

// calculates which materials (not multi-sub object materials) were actually assigned to a 
// mesh or multi - sub object material
// NOTE: must be called after the annotate filter's OneMaterialPerMesh (pass1)
// each of the used arrays are giving a total amount of faces where they were used
void AnnotateHelper::CalculateUsedMatsMapsAndImages(TupObjectPool &tupObjectPool,
													TupMaterialPool &tupMaterialPool,
													TupMapPool &tupMapPool,
													TupImagePool &tupImagePool,
													AnnotateHelper::UsedMatsMapsAndImages &usedMatsMapsAndImages,
													int numPartitions)
{
	int numMaterials = tupMaterialPool.GetNumMaterials();
	usedMatsMapsAndImages.m_usedMaterials.SetSize(numMaterials);
	int numMaps = tupMapPool.GetNumMaps();
	usedMatsMapsAndImages.m_usedMaps.SetSize(numMaps);
	int numImages = tupImagePool.GetNumImages();
	usedMatsMapsAndImages.m_usedImages.SetSize(numImages);

	// clear all counters
	int materialIndex;
	for (materialIndex=0;materialIndex<numMaterials;materialIndex++)
	{
		usedMatsMapsAndImages.m_usedMaterials[materialIndex].m_usedPartitionCounts.SetSize(numPartitions);
		for (int setIndex=0;setIndex<numPartitions;setIndex++)
			usedMatsMapsAndImages.m_usedMaterials[materialIndex].m_usedPartitionCounts[setIndex] = 0;
	}
	int mapIndex;
	for (mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		usedMatsMapsAndImages.m_usedMaps[mapIndex].m_usedPartitionCounts.SetSize(numPartitions);
		for (int setIndex=0;setIndex<numPartitions;setIndex++)
			usedMatsMapsAndImages.m_usedMaps[mapIndex].m_usedPartitionCounts[setIndex] = 0;
	}
	int imageIndex;
	for (imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		usedMatsMapsAndImages.m_usedImages[imageIndex].m_usedPartitionCounts.SetSize(numPartitions);
		for (int setIndex=0;setIndex<numPartitions;setIndex++)
			usedMatsMapsAndImages.m_usedImages[imageIndex].m_usedPartitionCounts[setIndex] = 0;
	}

	int numObjects = tupObjectPool.GetNumObjects();
	int objectIndex;
	for (objectIndex=0;objectIndex<numObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectIndex);
		if (!pObjectAgg)
			continue;
		TupObject tupObject(pObjectAgg);
		// deal only with trimeshes
		if (tupObject.GetObjectType() == TupObject::TRI_MESH)
		{
			TupTriMesh tupTriMesh(pObjectAgg);

			int materialRef = pObjectAgg->FindScalarByKey("Annotate_MaterialRef")->GetAsInt();
			if (materialRef!=-1)
			{
				// render partition stuff
				TupperwareList *pRenderPartitionSOMStartsList = pObjectAgg->FindListByKey("Annotate_RenderPartitionSOMStarts");
				int *pRenderPartitionSOMStarts = NULL;
				bool bPartitionSOM = false;
				// the following loop handles rendered meshes and other meshes	(collision, emitters, etc)
				// the total used are for all used meshes (rendered and other) 
				// the used based on the visibility set is for rendered meshes only
				if (pRenderPartitionSOMStartsList)
				{
					pRenderPartitionSOMStarts = pRenderPartitionSOMStartsList->GetAsInt();
					bPartitionSOM = true;
				}
				for (int setIndex=0;setIndex<numPartitions;setIndex++)
				{
					// if we don't have any info for the render partition stuff and the partition is not the first
					if (!bPartitionSOM && setIndex>0)
						continue;

					int *pMaterialIndices;
					int numFaces,faceIndex;
					tupTriMesh.GetFaceMaterialIndices(&pMaterialIndices,numFaces);

					TupperwareAggregate *pMaterialAgg = tupMaterialPool.GetMaterial(materialRef);
					TupMaterial tupMaterial(pMaterialAgg);
					if (tupMaterial.GetMaterialType() == TupMaterial::MULTI_SUB_OBJECT_MATERIAL)
					{
						if (setIndex==0)
							usedMatsMapsAndImages.m_usedMaterials[materialRef].m_totalUsedCount+=numFaces;
						if (bPartitionSOM && pRenderPartitionSOMStarts[setIndex]!=-1)
							usedMatsMapsAndImages.m_usedMaterials[materialRef].m_usedPartitionCounts[setIndex]+=numFaces;

						TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
						int numSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();
						if (numSubMaterials)
						{
							TupArray<int> subMaterialRefs;
							subMaterialRefs.SetSize(numSubMaterials);
							TupArray<int> usedSubMaterials;
							usedSubMaterials.SetSize(numSubMaterials);
							int subMaterialIndex;
							for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
							{
								usedSubMaterials[subMaterialIndex] = 0;

								TupperwareAggregate *pSubObjectMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(subMaterialIndex);
								TupMultiSubObjectMaterialSubMaterial tupMultiSubObjectMaterialSubMaterial(pSubObjectMaterialAgg);
								int subMaterialRef;
								tupMultiSubObjectMaterialSubMaterial.GetMaterialReference(subMaterialRef);
								subMaterialRefs[subMaterialIndex] = subMaterialRef;
							}
							// go through the faces and count how many used which material
							for (faceIndex=0;faceIndex<numFaces;faceIndex++)
							{
								usedSubMaterials[pMaterialIndices[faceIndex]]++; // mark how many faces used the sub material
							}
							for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
							{
								if (usedSubMaterials[subMaterialIndex] && subMaterialRefs[subMaterialIndex]!=-1)
								{
									if (setIndex==0)
										usedMatsMapsAndImages.m_usedMaterials[subMaterialRefs[subMaterialIndex]].m_totalUsedCount+=usedSubMaterials[subMaterialIndex];
									if (bPartitionSOM && pRenderPartitionSOMStarts[setIndex]!=-1)
										usedMatsMapsAndImages.m_usedMaterials[subMaterialRefs[subMaterialIndex]].m_usedPartitionCounts[setIndex]+=usedSubMaterials[subMaterialIndex];

									TupArray<int> mapRefsArray;
									// do the maps
									GetMapsFromMaterial(tupMaterialPool,subMaterialRefs[subMaterialIndex],mapRefsArray);
									int numMaps = mapRefsArray.GetSize();
									for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
									{
										if (setIndex==0)
											usedMatsMapsAndImages.m_usedMaps[mapRefsArray[mapIndex]].m_totalUsedCount+=usedSubMaterials[subMaterialIndex];
										if (bPartitionSOM && pRenderPartitionSOMStarts[setIndex]!=-1)
											usedMatsMapsAndImages.m_usedMaps[mapRefsArray[mapIndex]].m_usedPartitionCounts[setIndex]+=usedSubMaterials[subMaterialIndex];
										TupArray<int> subMapRefsArray;
										GetMapsFromMap(tupMapPool,mapRefsArray[mapIndex],subMapRefsArray);
										for (int subMapIndex=0;subMapIndex<subMapRefsArray.GetSize();subMapIndex++)
										{
											if (setIndex==0)
												usedMatsMapsAndImages.m_usedMaps[subMapRefsArray[subMapIndex]].m_totalUsedCount+=usedSubMaterials[subMaterialIndex];
											if (bPartitionSOM && pRenderPartitionSOMStarts[setIndex]!=-1)
												usedMatsMapsAndImages.m_usedMaps[subMapRefsArray[subMapIndex]].m_usedPartitionCounts[setIndex]+=usedSubMaterials[subMaterialIndex];
										}
										TupArray<int> imageRefsArray;
										// do the images
										GetImagesFromMap(tupMapPool,mapRefsArray[mapIndex],imageRefsArray);
										int numImages = imageRefsArray.GetSize();
										for (int imageIndex=0;imageIndex<numImages;imageIndex++)
										{
											int imageRef = imageRefsArray[imageIndex];
											TupperwareAggregate *pImageAgg = tupImagePool.GetImage(imageRef);
											assert(pImageAgg);
											// remap to other image ref (duplicate image data)
											TupperwareScalar *pRemapImageRefScalar = pImageAgg->FindScalarByKey("Annotate_RemapImageRef");
											if (pRemapImageRefScalar)
											{
												imageRef = pRemapImageRefScalar->GetAsInt();
											}
											if (setIndex==0)
												usedMatsMapsAndImages.m_usedImages[imageRef].m_totalUsedCount+=usedSubMaterials[subMaterialIndex];
											if (bPartitionSOM && pRenderPartitionSOMStarts[setIndex]!=-1)
												usedMatsMapsAndImages.m_usedImages[imageRef].m_usedPartitionCounts[setIndex]+=usedSubMaterials[subMaterialIndex];
										}
									}
								}
							}
						}
					}
					else
					{
						if (setIndex==0)
							usedMatsMapsAndImages.m_usedMaterials[materialRef].m_totalUsedCount+=numFaces;
						if (bPartitionSOM && pRenderPartitionSOMStarts[setIndex]!=-1)
							usedMatsMapsAndImages.m_usedMaterials[materialRef].m_usedPartitionCounts[setIndex]+=numFaces;

						TupArray<int> mapRefsArray;
						// do the maps
						GetMapsFromMaterial(tupMaterialPool,materialRef,mapRefsArray);
						int numMaps = mapRefsArray.GetSize();
						for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
						{
							if (setIndex==0)
								usedMatsMapsAndImages.m_usedMaps[mapRefsArray[mapIndex]].m_totalUsedCount+=numFaces;
							if (bPartitionSOM && pRenderPartitionSOMStarts[setIndex]!=-1)
								usedMatsMapsAndImages.m_usedMaps[mapRefsArray[mapIndex]].m_usedPartitionCounts[setIndex]+=numFaces;

							TupArray<int> subMapRefsArray;
							GetMapsFromMap(tupMapPool,mapRefsArray[mapIndex],subMapRefsArray);
							for (int subMapIndex=0;subMapIndex<subMapRefsArray.GetSize();subMapIndex++)
							{
								if (setIndex==0)
									usedMatsMapsAndImages.m_usedMaps[subMapRefsArray[subMapIndex]].m_totalUsedCount+=numFaces;
								if (bPartitionSOM && pRenderPartitionSOMStarts[setIndex]!=-1)
									usedMatsMapsAndImages.m_usedMaps[subMapRefsArray[subMapIndex]].m_usedPartitionCounts[setIndex]+=numFaces;
							}
							TupArray<int> imageRefsArray;
							// do the images
							GetImagesFromMap(tupMapPool,mapRefsArray[mapIndex],imageRefsArray);
							int numImages = imageRefsArray.GetSize();
							for (int imageIndex=0;imageIndex<numImages;imageIndex++)
							{
								int imageRef = imageRefsArray[imageIndex];
								TupperwareAggregate *pImageAgg = tupImagePool.GetImage(imageRef);
								assert(pImageAgg);
								// remap to other image ref (duplicate image data)
								TupperwareScalar *pRemapImageRefScalar = pImageAgg->FindScalarByKey("Annotate_RemapImageRef");
								if (pRemapImageRefScalar)
								{
									imageRef = pRemapImageRefScalar->GetAsInt();
								}
								if (setIndex==0)
									usedMatsMapsAndImages.m_usedImages[imageRef].m_totalUsedCount+=numFaces;
								if (bPartitionSOM && pRenderPartitionSOMStarts[setIndex]!=-1)
									usedMatsMapsAndImages.m_usedImages[imageRef].m_usedPartitionCounts[setIndex]+=numFaces;
							}
						}
					}
				}
			}
		}
	}
}

// calculates which materials (not multi-sub object materials) were actually assigned to a 
// standard material
void AnnotateHelper::CalculateUsedImages(int materialRef,
													  TupMaterialPool &tupMaterialPool,
													  TupMapPool &tupMapPool,
													  TupImagePool &tupImagePool,
													  Keeper<int> &usedImages)
{
	TupArray<int> mapRefsArray;
	// do the maps
	GetMapsFromMaterial(tupMaterialPool,materialRef,mapRefsArray);
	int numMaps = mapRefsArray.GetSize();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupArray<int> imageRefsArray;
		// do the images
		GetImagesFromMap(tupMapPool,mapRefsArray[mapIndex],imageRefsArray);
		int numImages = imageRefsArray.GetSize();
		for (int imageIndex=0;imageIndex<numImages;imageIndex++)
		{
			int imageRef = imageRefsArray[imageIndex];
			TupperwareAggregate *pImageAgg = tupImagePool.GetImage(imageRef);
			assert(pImageAgg);
			// remap to other image ref (duplicate image data)
			TupperwareScalar *pRemapImageRefScalar = pImageAgg->FindScalarByKey("Annotate_RemapImageRef");
			if (pRemapImageRefScalar)
			{
				imageRef = pRemapImageRefScalar->GetAsInt();
			}
			usedImages.Add(imageRef);
		}
	}
}


void AnnotateHelper::GetMapsFromMaterial(TupMaterialPool &tupMaterialPool,int materialRef,TupArray<int> &mapRefsArray)
{
	TupperwareAggregate *pMaterialAgg = tupMaterialPool.GetMaterial(materialRef);
	TupMaterial tupMaterial(pMaterialAgg);
	TupMaterial::MaterialTypes materialType = tupMaterial.GetMaterialType();
	switch (materialType)
	{
		case TupMaterial::STANDARD_MATERIAL:
			{
				TupStandardMaterial tupStandardMaterial(pMaterialAgg);
				int numSubMaps = tupStandardMaterial.GetNumMaps();
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupStandardMaterial.GetMap(subMapIndex);
					TupStandardMaterialMap tupSubMap(pSubMapAgg);
					int subMapRef;
					tupSubMap.GetMapReference(subMapRef);
					if (subMapRef!=-1)
					{
						mapRefsArray.Add(subMapRef);
					}
				}
			}
			break;
		case TupMaterial::MULTI_SUB_OBJECT_MATERIAL:
			break;
		case TupMaterial::BLEND_MATERIAL:
			{
				TupBlendMaterial tupBlendMaterial(pMaterialAgg);
				int material1Ref;
				TupperwareScalar *pMaterial1RefScalar = tupBlendMaterial.GetMaterial1Reference(material1Ref);
				if (!pMaterial1RefScalar)
					material1Ref = -1;
				if (material1Ref!=-1)
				{
					GetMapsFromMaterial(tupMaterialPool,material1Ref,mapRefsArray);
				}

				int material2Ref;
				TupperwareScalar *pMaterial2RefScalar = tupBlendMaterial.GetMaterial2Reference(material2Ref);
				if (!pMaterial2RefScalar)
					material2Ref = -1;
				if (material2Ref!=-1)
				{
					GetMapsFromMaterial(tupMaterialPool,material2Ref,mapRefsArray);
				}
			}
			break;
		case TupMaterial::COMPOSITE_MATERIAL:
			{
				TupCompositeMaterial tupCompositeMaterial(pMaterialAgg);
				int baseMaterialRef;
				tupCompositeMaterial.GetBaseMaterialReference(baseMaterialRef);
				if (baseMaterialRef!=-1)
				{
					GetMapsFromMaterial(tupMaterialPool,baseMaterialRef,mapRefsArray);
				}
				int numSubMaterials = tupCompositeMaterial.GetNumSubMaterials();
				int subMaterialIndex;
				for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupCompositeMaterial.GetSubMaterial(subMaterialIndex);
					TupCompositeMaterialSubMaterial subMat(pSubMaterialAgg);
					int flags;
					subMat.GetFlags(flags);
					if (flags & TupCompositeMaterialSubMaterial::ENABLED)
					{
						int subMaterialRef;
						subMat.GetMaterialReference(subMaterialRef);
						if (subMaterialRef!=-1)
						{
							GetMapsFromMaterial(tupMaterialPool,subMaterialRef,mapRefsArray);
						}
					}
				}

			}
			break;
		case TupMaterial::DOUBLE_SIDED_MATERIAL:
			{
				TupDoubleSidedMaterial tupDoubleSidedMaterial(pMaterialAgg);
				int frontMaterialRef;
				TupperwareScalar *pFrontMaterialRefScalar = tupDoubleSidedMaterial.GetFrontMaterialReference(frontMaterialRef);
				if (pFrontMaterialRefScalar && frontMaterialRef!=-1)
				{
					GetMapsFromMaterial(tupMaterialPool,frontMaterialRef,mapRefsArray);
				}
 			}
			break;
	}
}

void AnnotateHelper::GetMapsFromMap(TupMapPool &tupMapPool,int mapRef,TupArray<int> &subMapRefsArray)
{
	TupperwareAggregate *pMapAgg = tupMapPool.GetMap(mapRef);
	TupMap tupMap(pMapAgg);
	switch(tupMap.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			break;
		case TupMap::COMPOSITE_MAP:
			{
				TupCompositeMap tupCompositeMap(pMapAgg);
				int numSubMaps = tupCompositeMap.GetNumSubMaps();
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupCompositeMap.GetSubMap(subMapIndex);
					TupCompositeMapSubMap tupSubMap(pSubMapAgg);
					int subMapRef;
					tupSubMap.GetMapReference(subMapRef);
					if (subMapRef!=-1)
					{
						subMapRefsArray.Add(subMapRef);
						GetMapsFromMap(tupMapPool,subMapRef,subMapRefsArray);
					}
				}
			}
			break;
		case TupMap::MIX_MAP:
			{
				TupMixMap tupMixMap(pMapAgg);

				int map1Ref;
				TupperwareScalar *pMap1RefScalar = tupMixMap.GetMap1Reference(map1Ref);
				if (!pMap1RefScalar)
					map1Ref = -1;
				int map2Ref;
				TupperwareScalar *pMap2RefScalar = tupMixMap.GetMap2Reference(map2Ref);
				if (!pMap2RefScalar)
					map2Ref = -1;
				int mixMapRef;
				TupperwareScalar *pMixMapRefScalar = tupMixMap.GetMixMapReference(mixMapRef);
				if (!pMixMapRefScalar)
					mixMapRef = -1;

				int flags;
				tupMixMap.GetFlags(flags);
				float mixAmount;
				tupMixMap.GetMixAmount(mixAmount);
				if (map1Ref!=-1 && (flags & TupMixMap::MAP_1_ENABLE))
				{
					subMapRefsArray.Add(map1Ref);
					GetMapsFromMap(tupMapPool,map1Ref,subMapRefsArray);
				}
				if (map2Ref!=-1 && (flags & TupMixMap::MAP_2_ENABLE))
				{
					if ((flags & TupMixMap::MIX_MAP_ENABLE) && mixMapRef!=-1)
					{
						subMapRefsArray.Add(mixMapRef);
						GetMapsFromMap(tupMapPool,mixMapRef,subMapRefsArray);
					}
					subMapRefsArray.Add(map2Ref);
					GetMapsFromMap(tupMapPool,map2Ref,subMapRefsArray);
				}
			}
			break;
	}
}

void AnnotateHelper::GetImagesFromMap(TupMapPool &tupMapPool,int mapRef,TupArray<int> &imageRefsArray)
{
	TupperwareAggregate *pMapAgg = tupMapPool.GetMap(mapRef);
	TupMap tupMap(pMapAgg);
	switch(tupMap.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			{
				TupBitmapMap tupBitmapMap(pMapAgg);
				int imageRef;
				tupBitmapMap.GetImageReference(imageRef);
				if (imageRef!=-1)
				{
					imageRefsArray.Add(imageRef);
				}
			}
			break;
		case TupMap::COMPOSITE_MAP:
			{
				TupCompositeMap tupCompositeMap(pMapAgg);
				int numSubMaps = tupCompositeMap.GetNumSubMaps();
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupCompositeMap.GetSubMap(subMapIndex);
					TupCompositeMapSubMap tupSubMap(pSubMapAgg);
					int subMapRef;
					tupSubMap.GetMapReference(subMapRef);
					if (subMapRef!=-1)
					{
						GetImagesFromMap(tupMapPool,subMapRef,imageRefsArray);
					}
				}
			}
			break;
		case TupMap::MIX_MAP:
			{
				TupMixMap tupMixMap(pMapAgg);

				int map1Ref;
				TupperwareScalar *pMap1RefScalar = tupMixMap.GetMap1Reference(map1Ref);
				if (!pMap1RefScalar)
					map1Ref = -1;
				int map2Ref;
				TupperwareScalar *pMap2RefScalar = tupMixMap.GetMap2Reference(map2Ref);
				if (!pMap2RefScalar)
					map2Ref = -1;
				int mixMapRef;
				TupperwareScalar *pMixMapRefScalar = tupMixMap.GetMixMapReference(mixMapRef);
				if (!pMixMapRefScalar)
					mixMapRef = -1;

				int flags;
				tupMixMap.GetFlags(flags);
				float mixAmount;
				tupMixMap.GetMixAmount(mixAmount);
				if (map1Ref!=-1 && (flags & TupMixMap::MAP_1_ENABLE))
				{
					GetImagesFromMap(tupMapPool,map1Ref,imageRefsArray);
				}
				if (map2Ref!=-1 && (flags & TupMixMap::MAP_2_ENABLE))
				{
					if ((flags & TupMixMap::MIX_MAP_ENABLE) && mixMapRef!=-1)
					{
						GetImagesFromMap(tupMapPool,mixMapRef,imageRefsArray);
					}
					GetImagesFromMap(tupMapPool,map2Ref,imageRefsArray);
				}
			}
			break;
	}
}

void AnnotateHelper::CalculateUsedBitmapMapsFromMaterial(TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,int materialRef,Keeper<int> &usedBitmapMapRefs)
{
	TupperwareAggregate *pMaterialAgg = tupMaterialPool.GetMaterial(materialRef);
	TupMaterial tupMaterial(pMaterialAgg);
	TupMaterial::MaterialTypes materialType = tupMaterial.GetMaterialType();
	switch (materialType)
	{
		case TupMaterial::STANDARD_MATERIAL:
			{
				TupStandardMaterial tupStandardMaterial(pMaterialAgg);
				int numSubMaps = tupStandardMaterial.GetNumMaps();
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupStandardMaterial.GetMap(subMapIndex);
					TupStandardMaterialMap tupSubMap(pSubMapAgg);
					int subMapRef;
					tupSubMap.GetMapReference(subMapRef);
					if (subMapRef!=-1)
					{
						CalculateUsedBitmapMapsFromMap(tupMapPool,subMapRef,usedBitmapMapRefs);
					}
				}
			}
			break;
		case TupMaterial::MULTI_SUB_OBJECT_MATERIAL:
			break;
		case TupMaterial::BLEND_MATERIAL:
			{
				TupBlendMaterial tupBlendMaterial(pMaterialAgg);
				int material1Ref;
				TupperwareScalar *pMaterial1RefScalar = tupBlendMaterial.GetMaterial1Reference(material1Ref);
				if (!pMaterial1RefScalar)
					material1Ref = -1;
				if (material1Ref!=-1)
				{
					CalculateUsedBitmapMapsFromMaterial(tupMaterialPool,tupMapPool,material1Ref,usedBitmapMapRefs);
				}

				int material2Ref;
				TupperwareScalar *pMaterial2RefScalar = tupBlendMaterial.GetMaterial2Reference(material2Ref);
				if (!pMaterial2RefScalar)
					material2Ref = -1;
				if (material2Ref!=-1)
				{
					CalculateUsedBitmapMapsFromMaterial(tupMaterialPool,tupMapPool,material2Ref,usedBitmapMapRefs);
				}
			}
			break;
		case TupMaterial::COMPOSITE_MATERIAL:
			{
				TupCompositeMaterial tupCompositeMaterial(pMaterialAgg);
				int baseMaterialRef;
				tupCompositeMaterial.GetBaseMaterialReference(baseMaterialRef);
				if (baseMaterialRef!=-1)
				{
					CalculateUsedBitmapMapsFromMaterial(tupMaterialPool,tupMapPool,baseMaterialRef,usedBitmapMapRefs);
				}
				int numSubMaterials = tupCompositeMaterial.GetNumSubMaterials();
				int subMaterialIndex;
				for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupCompositeMaterial.GetSubMaterial(subMaterialIndex);
					TupCompositeMaterialSubMaterial subMat(pSubMaterialAgg);
					int flags;
					subMat.GetFlags(flags);
					if (flags & TupCompositeMaterialSubMaterial::ENABLED)
					{
						int subMaterialRef;
						subMat.GetMaterialReference(subMaterialRef);
						if (subMaterialRef!=-1)
						{
							CalculateUsedBitmapMapsFromMaterial(tupMaterialPool,tupMapPool,subMaterialRef,usedBitmapMapRefs);
						}
					}
				}
			}
			break;
		case TupMaterial::DOUBLE_SIDED_MATERIAL:
			{
				TupDoubleSidedMaterial tupDoubleSidedMaterial(pMaterialAgg);
				int frontMaterialRef;
				TupperwareScalar *pFrontMaterialRefScalar = tupDoubleSidedMaterial.GetFrontMaterialReference(frontMaterialRef);
				if (pFrontMaterialRefScalar && frontMaterialRef!=-1)
				{
					CalculateUsedBitmapMapsFromMaterial(tupMaterialPool,tupMapPool,frontMaterialRef,usedBitmapMapRefs);
				}
			}
			break;
	}
}

void AnnotateHelper::CalculateUsedBitmapMapsFromMap(TupMapPool &tupMapPool,int mapRef,Keeper<int> &usedBitmapMapRefs)
{
	TupperwareAggregate *pMapAgg = tupMapPool.GetMap(mapRef);
	TupMap tupMap(pMapAgg);
	switch(tupMap.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			usedBitmapMapRefs.Add(mapRef);
			break;
		case TupMap::COMPOSITE_MAP:
			{
				TupCompositeMap tupCompositeMap(pMapAgg);
				int numSubMaps = tupCompositeMap.GetNumSubMaps();
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupCompositeMap.GetSubMap(subMapIndex);
					TupCompositeMapSubMap tupSubMap(pSubMapAgg);
					int subMapRef;
					tupSubMap.GetMapReference(subMapRef);
					if (subMapRef!=-1)
					{
						CalculateUsedBitmapMapsFromMap(tupMapPool,subMapRef,usedBitmapMapRefs);
					}
				}
			}
			break;
		case TupMap::MIX_MAP:
			{
				TupMixMap tupMixMap(pMapAgg);

				int map1Ref;
				TupperwareScalar *pMap1RefScalar = tupMixMap.GetMap1Reference(map1Ref);
				if (!pMap1RefScalar)
					map1Ref = -1;
				int map2Ref;
				TupperwareScalar *pMap2RefScalar = tupMixMap.GetMap2Reference(map2Ref);
				if (!pMap2RefScalar)
					map2Ref = -1;
				int mixMapRef;
				TupperwareScalar *pMixMapRefScalar = tupMixMap.GetMixMapReference(mixMapRef);
				if (!pMixMapRefScalar)
					mixMapRef = -1;

				int flags;
				tupMixMap.GetFlags(flags);
				float mixAmount;
				tupMixMap.GetMixAmount(mixAmount);
				if (map1Ref!=-1 && (flags & TupMixMap::MAP_1_ENABLE))
				{
					CalculateUsedBitmapMapsFromMap(tupMapPool,map1Ref,usedBitmapMapRefs);
				}
				if (map2Ref!=-1 && (flags & TupMixMap::MAP_2_ENABLE))
				{
					if ((flags & TupMixMap::MIX_MAP_ENABLE) && mixMapRef!=-1)
					{
						CalculateUsedBitmapMapsFromMap(tupMapPool,mixMapRef,usedBitmapMapRefs);
					}
					CalculateUsedBitmapMapsFromMap(tupMapPool,map2Ref,usedBitmapMapRefs);
				}
			}
			break;
	}
}

void AnnotateHelper::RemapMaps(TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupArray<int> &mapRefRemapArray)
{
	int numMaterials = tupMaterialPool.GetNumMaterials();
	int materialIndex;
	for (materialIndex=0;materialIndex<numMaterials;materialIndex++)
	{
		RemapMaterialMaps(tupMaterialPool,materialIndex,mapRefRemapArray);
	}
	int numMaps = tupMapPool.GetNumMaps();
	int mapIndex;
	for (mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		RemapMapMaps(tupMapPool,mapIndex,mapRefRemapArray);
	}
}

void AnnotateHelper::RemapMaterialMaps(TupMaterialPool &tupMaterialPool,int materialRef,TupArray<int> &mapRefRemapArray)
{
	if (materialRef==-1)
		return;
	TupperwareAggregate *pMaterialAgg = tupMaterialPool.GetMaterial(materialRef);
	if (!pMaterialAgg)
		return;
	TupMaterial tupMaterial(pMaterialAgg);
	switch(tupMaterial.GetMaterialType())
	{
		case TupMaterial::STANDARD_MATERIAL:
			{
				TupStandardMaterial tupStandardMaterial(pMaterialAgg);
				int numSubMaps = tupStandardMaterial.GetNumMaps();
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupStandardMaterial.GetMap(subMapIndex);
					TupStandardMaterialMap tupSubMap(pSubMapAgg);
					int subMapRef;
					TupperwareScalar *pSubMapRefScalar = tupSubMap.GetMapReference(subMapRef);
					if (subMapRef!=-1)
					{
						// if pointing to another map then set to new one
						if(mapRefRemapArray[subMapRef]!=-1)
							pSubMapRefScalar->SetAsInt(mapRefRemapArray[subMapRef]);
					}
				}
			}
			break;
		case TupMaterial::MULTI_SUB_OBJECT_MATERIAL:
			break;
		case TupMaterial::BLEND_MATERIAL:
			{
				TupBlendMaterial tupBlendMaterial(pMaterialAgg);
				int maskRef;
				TupperwareScalar *pMaskRefScalar = tupBlendMaterial.GetMaskReference(maskRef);
				if (!pMaskRefScalar)
					maskRef = -1;
				if (maskRef!=-1)
				{
					// if pointing to another map then set to new one
					if (mapRefRemapArray[maskRef]!=-1)
						pMaskRefScalar->SetAsInt(mapRefRemapArray[maskRef]);
				}
			}
			break;
		case TupMaterial::COMPOSITE_MATERIAL:
			break;
		case TupMaterial::DOUBLE_SIDED_MATERIAL:
			break;

	}
}

void AnnotateHelper::RemapMapMaps(TupMapPool &tupMapPool,int mapRef,TupArray<int> &mapRefRemapArray)
{
	if (mapRef==-1)
		return;
	TupperwareAggregate *pMapAgg = tupMapPool.GetMap(mapRef);
	if (!pMapAgg)
		return;
	TupMap tupMap(pMapAgg);
	switch(tupMap.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			break;
		case TupMap::COMPOSITE_MAP:
			{
				TupCompositeMap tupCompositeMap(pMapAgg);
				int numSubMaps = tupCompositeMap.GetNumSubMaps();
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupCompositeMap.GetSubMap(subMapIndex);
					TupCompositeMapSubMap tupSubMap(pSubMapAgg);
					int subMapRef;
					TupperwareScalar *pSubMapRefScalar = tupSubMap.GetMapReference(subMapRef);
					if (subMapRef!=-1)
					{
						// if pointing to another map then set to new one
						if(mapRefRemapArray[subMapRef]!=-1)
							pSubMapRefScalar->SetAsInt(mapRefRemapArray[subMapRef]);
					}
				}
			}
			break;
		case TupMap::MIX_MAP:
			{
				TupMixMap tupMixMap(pMapAgg);

				int map1Ref;
				TupperwareScalar *pMap1RefScalar = tupMixMap.GetMap1Reference(map1Ref);
				if (!pMap1RefScalar)
					map1Ref = -1;
				int map2Ref;
				TupperwareScalar *pMap2RefScalar = tupMixMap.GetMap2Reference(map2Ref);
				if (!pMap2RefScalar)
					map2Ref = -1;
				int mixMapRef;
				TupperwareScalar *pMixMapRefScalar = tupMixMap.GetMixMapReference(mixMapRef);
				if (!pMixMapRefScalar)
					mixMapRef = -1;

				if (map1Ref!=-1)
				{
					// if pointing to another map then set to new one
					if(mapRefRemapArray[map1Ref]!=-1)
						pMap1RefScalar->SetAsInt(mapRefRemapArray[map1Ref]);
				}
				if (map2Ref!=-1)
				{
					// if pointing to another map then set to new one
					if(mapRefRemapArray[map2Ref]!=-1)
						pMap2RefScalar->SetAsInt(mapRefRemapArray[map2Ref]);
				}
				if (mixMapRef!=-1)
				{
					// if pointing to another map then set to new one
					if(mapRefRemapArray[mixMapRef]!=-1)
						pMixMapRefScalar->SetAsInt(mapRefRemapArray[mixMapRef]);
				}
			}
			break;
	}
}

int AnnotateHelper::RemapMapChannels(TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,
												 int materialRef,
												 Keeper<int> &remappedMaterials, TupArray<int> &remappedMaterialRefs,
												 Keeper<int> &remappedMaps, TupArray<int> &remappedMapRefs,
												 Keeper<int> &imageRefs,TupArray<int> &mapChannels)
{
	if (materialRef==-1)
		return materialRef;
	TupperwareAggregate *pSourceMaterialAgg = tupMaterialPool.GetMaterial(materialRef);
	if (!pSourceMaterialAgg)
		return materialRef;

	int remappedMaterialIndex = remappedMaterials.GetIndex(materialRef);
	if (remappedMaterialIndex != -1)
		return remappedMaterialRefs[remappedMaterialIndex];

	remappedMaterials.Add(materialRef); // remember which material we made from the source

	int newMaterialRef = tupMaterialPool.AddMaterial();
	remappedMaterialRefs.Add(newMaterialRef);

	TupperwareAggregate *pNewMaterialAgg = tupMaterialPool.GetMaterial(newMaterialRef);
	pNewMaterialAgg->CopyChildren(pSourceMaterialAgg);

	TupMaterial tupMaterial(pNewMaterialAgg);
	switch(tupMaterial.GetMaterialType())
	{
		case TupMaterial::STANDARD_MATERIAL:
			{
				TupStandardMaterial tupStandardMaterial(pNewMaterialAgg);
				int numSubMaps = tupStandardMaterial.GetNumMaps();
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupStandardMaterial.GetMap(subMapIndex);
					TupStandardMaterialMap tupSubMap(pSubMapAgg);
					int subMapRef;
					TupperwareScalar *pSubMapRefScalar = tupSubMap.GetMapReference(subMapRef);
					if (subMapRef!=-1)
					{
						int newMapRef = RemapMapChannels(tupMapPool,subMapRef,remappedMaps,remappedMapRefs,imageRefs,mapChannels);
						pSubMapRefScalar->SetAsInt(newMapRef);
					}
				}
			}
			break;
		case TupMaterial::MULTI_SUB_OBJECT_MATERIAL:
			{
				TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pNewMaterialAgg);
				int numSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();
				if (numSubMaterials)
				{
					int subMaterialIndex;
					for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
					{
						TupperwareAggregate *pSubObjectMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(subMaterialIndex);
						TupMultiSubObjectMaterialSubMaterial tupMultiSubObjectMaterialSubMaterial(pSubObjectMaterialAgg);
						int subMaterialRef;
						TupperwareScalar *subMaterialRefScalar = tupMultiSubObjectMaterialSubMaterial.GetMaterialReference(subMaterialRef);
						if (subMaterialRef != -1)
						{
							int newMaterialRef = RemapMapChannels(tupMaterialPool,tupMapPool,subMaterialRef,
								remappedMaterials,remappedMaterialRefs,remappedMaps,remappedMapRefs,imageRefs,mapChannels);
							subMaterialRefScalar->SetAsInt(newMaterialRef);
						}
					}
				}
			}
			break;
		case TupMaterial::BLEND_MATERIAL:
			{
				TupBlendMaterial tupBlendMaterial(pNewMaterialAgg);
				int material1Ref;
				TupperwareScalar *pMaterial1RefScalar = tupBlendMaterial.GetMaterial1Reference(material1Ref);
				if (!pMaterial1RefScalar)
					material1Ref = -1;
				if (material1Ref!=-1)
				{
					int newMaterialRef = RemapMapChannels(tupMaterialPool,tupMapPool,material1Ref,
						remappedMaterials,remappedMaterialRefs,remappedMaps,remappedMapRefs,imageRefs,mapChannels);
					pMaterial1RefScalar->SetAsInt(newMaterialRef);
				}

				int material2Ref;
				TupperwareScalar *pMaterial2RefScalar = tupBlendMaterial.GetMaterial2Reference(material2Ref);
				if (!pMaterial2RefScalar)
					material2Ref = -1;
				if (material2Ref!=-1)
				{
					int newMaterialRef = RemapMapChannels(tupMaterialPool,tupMapPool,material2Ref,
						remappedMaterials,remappedMaterialRefs,remappedMaps,remappedMapRefs,imageRefs,mapChannels);
					pMaterial2RefScalar->SetAsInt(newMaterialRef);
				}
			}
			break;
		case TupMaterial::COMPOSITE_MATERIAL:
			{
				TupCompositeMaterial tupCompositeMaterial(pNewMaterialAgg);
				int baseMaterialRef;
				TupperwareScalar *pBaseMaterialRefScalar = tupCompositeMaterial.GetBaseMaterialReference(baseMaterialRef);
				if (baseMaterialRef!=-1)
				{
					int newMaterialRef = RemapMapChannels(tupMaterialPool,tupMapPool,baseMaterialRef,
						remappedMaterials,remappedMaterialRefs,remappedMaps,remappedMapRefs,imageRefs,mapChannels);
					pBaseMaterialRefScalar->SetAsInt(newMaterialRef);
				}
				int numSubMaterials = tupCompositeMaterial.GetNumSubMaterials();
				int subMaterialIndex;
				for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupCompositeMaterial.GetSubMaterial(subMaterialIndex);
					TupCompositeMaterialSubMaterial subMat(pSubMaterialAgg);
					int flags;
					subMat.GetFlags(flags);
					if (flags & TupCompositeMaterialSubMaterial::ENABLED)
					{
						int subMaterialRef;
						TupperwareScalar *pSubMaterialRefScalar = subMat.GetMaterialReference(subMaterialRef);
						if (subMaterialRef!=-1)
						{
							int newMaterialRef = RemapMapChannels(tupMaterialPool,tupMapPool,subMaterialRef,
								remappedMaterials,remappedMaterialRefs,remappedMaps,remappedMapRefs,imageRefs,mapChannels);
							pSubMaterialRefScalar->SetAsInt(newMaterialRef);
						}
					}
				}
			}
			break;
		case TupMaterial::DOUBLE_SIDED_MATERIAL:
			{
				TupDoubleSidedMaterial tupDoubleSidedMaterial(pNewMaterialAgg);
				int frontMaterialRef;
				TupperwareScalar *pFrontMaterialRefScalar = tupDoubleSidedMaterial.GetFrontMaterialReference(frontMaterialRef);
				if (pFrontMaterialRefScalar && frontMaterialRef!=-1)
				{
					int newMaterialRef = RemapMapChannels(tupMaterialPool,tupMapPool,frontMaterialRef,
						remappedMaterials,remappedMaterialRefs,remappedMaps,remappedMapRefs,imageRefs,mapChannels);
					pFrontMaterialRefScalar->SetAsInt(newMaterialRef);
				}
			}
			break;
	}
	return newMaterialRef;
}

int AnnotateHelper::RemapMapChannels(TupMapPool &tupMapPool,int mapRef,
												 Keeper<int> &remappedMaps, TupArray<int> &remappedMapRefs,
												 Keeper<int> &imageRefs,TupArray<int> &mapChannels)
{
	if (mapRef==-1)
		return mapRef;
	TupperwareAggregate *pSourceMapAgg = tupMapPool.GetMap(mapRef);
	if (!pSourceMapAgg)
		return mapRef;

	int remappedMapIndex = remappedMaps.GetIndex(mapRef);
	if (remappedMapIndex != -1)
		return remappedMapRefs[remappedMapIndex];

	remappedMaps.Add(mapRef); // remember which map we made from the source

	int newMapRef = tupMapPool.AddMap();
	remappedMapRefs.Add(newMapRef);

	TupperwareAggregate *pNewMapAgg = tupMapPool.GetMap(newMapRef);
	pNewMapAgg->CopyChildren(pSourceMapAgg);

	TupperwareAggregate *pMapAgg = tupMapPool.GetMap(mapRef);
	TupMap tupMap(pMapAgg);
	switch(tupMap.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			{
				TupBitmapMap tupBitmapMap(pNewMapAgg);
				int imageRef;
				tupBitmapMap.GetImageReference(imageRef);
				int keeperIndex = imageRefs.GetIndex(imageRef);
				if (keeperIndex != -1)
				{
					int mapChannel;
					TupperwareScalar *pMapChannelScalar = tupBitmapMap.GetMapChannel(mapChannel);
					pMapChannelScalar->SetAsInt(mapChannels[keeperIndex]);
				}
			}
			break;
		case TupMap::COMPOSITE_MAP:
			{
				TupCompositeMap tupCompositeMap(pNewMapAgg);
				int numSubMaps = tupCompositeMap.GetNumSubMaps();
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupCompositeMap.GetSubMap(subMapIndex);
					TupCompositeMapSubMap tupSubMap(pSubMapAgg);
					int subMapRef;
					TupperwareScalar *pMapReferenceScalar = tupSubMap.GetMapReference(subMapRef);
					if (subMapRef!=-1)
					{
						int newMapRef = RemapMapChannels(tupMapPool,subMapRef,remappedMaps,remappedMapRefs,imageRefs,mapChannels);
						pMapReferenceScalar->SetAsInt(newMapRef);
					}
				}
			}
			break;
		case TupMap::MIX_MAP:
			{
				TupMixMap tupMixMap(pNewMapAgg);

				int map1Ref;
				TupperwareScalar *pMap1RefScalar = tupMixMap.GetMap1Reference(map1Ref);
				if (!pMap1RefScalar)
					map1Ref = -1;
				int map2Ref;
				TupperwareScalar *pMap2RefScalar = tupMixMap.GetMap2Reference(map2Ref);
				if (!pMap2RefScalar)
					map2Ref = -1;
				int mixMapRef;
				TupperwareScalar *pMixMapRefScalar = tupMixMap.GetMixMapReference(mixMapRef);
				if (!pMixMapRefScalar)
					mixMapRef = -1;

				int flags;
				tupMixMap.GetFlags(flags);
				float mixAmount;
				tupMixMap.GetMixAmount(mixAmount);
				if (map1Ref!=-1 && (flags & TupMixMap::MAP_1_ENABLE))
				{
					int newMapRef = RemapMapChannels(tupMapPool,map1Ref,remappedMaps,remappedMapRefs,imageRefs,mapChannels);
					pMap1RefScalar->SetAsInt(newMapRef);
				}
				if (map2Ref!=-1 && (flags & TupMixMap::MAP_2_ENABLE))
				{
					if ((flags & TupMixMap::MIX_MAP_ENABLE) && mixMapRef!=-1)
					{
						int newMapRef = RemapMapChannels(tupMapPool,mixMapRef,remappedMaps,remappedMapRefs,imageRefs,mapChannels);
						pMixMapRefScalar->SetAsInt(newMapRef);
					}
					int newMapRef = RemapMapChannels(tupMapPool,map2Ref,remappedMaps,remappedMapRefs,imageRefs,mapChannels);
					pMap2RefScalar->SetAsInt(newMapRef);
				}
			}
			break;
	}
	return newMapRef;
}


void AnnotateHelper::RemapMaterials(TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupMaterialPool &tupMaterialPool,TupArray<int> &materialRefRemapArray)
{
	// remap the node material refs
	int numNodes = tupNodePool.GetNumNodes();
	int nodeIndex;
	for (nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeIndex);
		if (!pNodeAgg)
			continue;
		TupNode tupNode(pNodeAgg);
		int materialRef;
		TupperwareScalar *pMaterialRefScalar = tupNode.GetMaterialReference(materialRef);
		if (!pMaterialRefScalar)
			continue;

		if (materialRef!=-1)
		{
			// if pointing to another map then set to a new one
			if (materialRefRemapArray[materialRef]!=-1)
				pMaterialRefScalar->SetAsInt(materialRefRemapArray[materialRef]);
		}
	}

	// remap the object material refs
	int numObjects = tupObjectPool.GetNumObjects();
	int objectIndex;
	for (objectIndex=0;objectIndex<numObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectIndex);

		// if empty slot - continue
		if (!pObjectAgg)
			continue;

		// it needs to be a mesh
		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType() != TupObject::TRI_MESH)
			continue;

		TupperwareScalar *pMaterialRefScalar = pObjectAgg->FindScalarByKey("Annotate_MaterialRef");
		if (!pMaterialRefScalar)
			continue;

		int materialRef = pMaterialRefScalar->GetAsInt();
		// make sure it is using a material
		if (materialRef==-1)
			continue;

		// if pointing to another map then set to a new one
		if (materialRefRemapArray[materialRef]!=-1)
			pMaterialRefScalar->SetAsInt(materialRefRemapArray[materialRef]);
	}

	// remap all material refs within the material pools
	int numMaterials = tupMaterialPool.GetNumMaterials();
	int materialIndex;
	for (materialIndex=0;materialIndex<numMaterials;materialIndex++)
	{
		TupperwareAggregate *pMaterialAgg = tupMaterialPool.GetMaterial(materialIndex);
		if (!pMaterialAgg)
			continue;
		TupMaterial tupMaterial(pMaterialAgg);
		switch(tupMaterial.GetMaterialType())
		{
			case TupMaterial::STANDARD_MATERIAL:
				break;
			case TupMaterial::MULTI_SUB_OBJECT_MATERIAL:
				{
					TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
					int numSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();
					int subMaterialIndex;
					for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
					{
						TupperwareAggregate *pSubObjectMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(subMaterialIndex);
						TupMultiSubObjectMaterialSubMaterial tupMultiSubObjectMaterialSubMaterial(pSubObjectMaterialAgg);
						int subMaterialRef;
						TupperwareScalar *pSubMaterialRefScalar = tupMultiSubObjectMaterialSubMaterial.GetMaterialReference(subMaterialRef);
						if (subMaterialRef!=-1)
						{
							if (materialRefRemapArray[subMaterialRef]!=-1)
								pSubMaterialRefScalar->SetAsInt(materialRefRemapArray[subMaterialRef]);
						}
					}
				}
				break;
			case TupMaterial::BLEND_MATERIAL:
				{
					TupBlendMaterial tupBlendMaterial(pMaterialAgg);
					int material1Ref;
					TupperwareScalar *pMaterial1RefScalar = tupBlendMaterial.GetMaterial1Reference(material1Ref);
					if (!pMaterial1RefScalar)
						material1Ref = -1;
					if (material1Ref!=-1)
					{
						if (materialRefRemapArray[material1Ref]!=-1)
							pMaterial1RefScalar->SetAsInt(materialRefRemapArray[material1Ref]);
					}
					int material2Ref;
					TupperwareScalar *pMaterial2RefScalar = tupBlendMaterial.GetMaterial2Reference(material2Ref);
					if (!pMaterial2RefScalar)
						material2Ref = -1;
					if (material2Ref!=-1)
					{
						if (materialRefRemapArray[material2Ref]!=-1)
							pMaterial2RefScalar->SetAsInt(materialRefRemapArray[material2Ref]);
					}
				}
				break;
			case TupMaterial::COMPOSITE_MATERIAL:
				{
					TupCompositeMaterial tupCompositeMaterial(pMaterialAgg);
					int baseMaterialRef;
					TupperwareScalar *pBaseMaterialRefScalar = tupCompositeMaterial.GetBaseMaterialReference(baseMaterialRef);
					if (baseMaterialRef!=-1)
					{
						if (materialRefRemapArray[baseMaterialRef]!=-1)
							pBaseMaterialRefScalar->SetAsInt(materialRefRemapArray[baseMaterialRef]);
					}
					int numSubMaterials = tupCompositeMaterial.GetNumSubMaterials();
					int subMaterialIndex;
					for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
					{
						TupperwareAggregate *pSubMaterialAgg = tupCompositeMaterial.GetSubMaterial(subMaterialIndex);
						TupCompositeMaterialSubMaterial subMat(pSubMaterialAgg);
						int flags;
						subMat.GetFlags(flags);
						int subMaterialRef;
						TupperwareScalar *pSubMaterialRefScalar = subMat.GetMaterialReference(subMaterialRef);
						if (subMaterialRef!=-1)
						{
							if (materialRefRemapArray[subMaterialRef]!=-1)
								pSubMaterialRefScalar->SetAsInt(materialRefRemapArray[subMaterialRef]);
						}
					}
				}
				break;
			case TupMaterial::DOUBLE_SIDED_MATERIAL:
				{
					TupDoubleSidedMaterial tupDoubleSidedMaterial(pMaterialAgg);
					int frontMaterialRef;
					TupperwareScalar *pFrontMaterialRefScalar = tupDoubleSidedMaterial.GetFrontMaterialReference(frontMaterialRef);
					if (!pFrontMaterialRefScalar)
						frontMaterialRef = -1;
					if (frontMaterialRef!=-1)
					{
						if (materialRefRemapArray[frontMaterialRef]!=-1)
							pFrontMaterialRefScalar->SetAsInt(materialRefRemapArray[frontMaterialRef]);
					}
					int backMaterialRef;
					TupperwareScalar *pBackMaterialRefScalar = tupDoubleSidedMaterial.GetBackMaterialReference(backMaterialRef);
					if (!pBackMaterialRefScalar)
						backMaterialRef = -1;
					if (backMaterialRef!=-1)
					{
						if (materialRefRemapArray[backMaterialRef]!=-1)
							pBackMaterialRefScalar->SetAsInt(materialRefRemapArray[backMaterialRef]);
					}
				}
				break;
		}
	}

}

unsigned long AnnotateHelper::GetMaterialCRC(TupMaterialPool &tupMaterialPool,int materialRef)
{
	CRC CRCGenerator;
	char *pName;
	int intValue;
	float floatValue;
	float *pFloatList;

	assert(materialRef!=-1);
	TupperwareAggregate *pMaterialAgg = tupMaterialPool.GetMaterial(materialRef);
	assert(pMaterialAgg);
	TupMaterial tupMaterial(pMaterialAgg);

	switch(tupMaterial.GetMaterialType())
	{
		case TupMaterial::STANDARD_MATERIAL:
			{
				CRCGenerator.AnalyzeData((int)TupMaterial::STANDARD_MATERIAL);
				TupStandardMaterial tupStandardMaterial(pMaterialAgg);
//				tupStandardMaterial.GetName(&pName); CRCGenerator.AnalyzeData(pName);
				tupStandardMaterial.GetAmbientColor(&pFloatList); CRCGenerator.AnalyzeData(pFloatList,sizeof(float)*3);
				tupStandardMaterial.GetDiffuseColor(&pFloatList); CRCGenerator.AnalyzeData(pFloatList,sizeof(float)*3);
				tupStandardMaterial.GetSpecularColor(&pFloatList); CRCGenerator.AnalyzeData(pFloatList,sizeof(float)*3);
				tupStandardMaterial.GetFilterColor(&pFloatList); CRCGenerator.AnalyzeData(pFloatList,sizeof(float)*3);
				tupStandardMaterial.GetGlossiness(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetSpecularLevel(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetSpecularSoften(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetTransparency(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetWireSize(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetIndexOfRefraction(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetReflectionDimIntensity(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetReflectionDimMultiplier(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetOpacity(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetOpacityFalloff(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetTransparencyType(intValue); CRCGenerator.AnalyzeData(intValue);
				tupStandardMaterial.GetShadingTypeName(&pName); CRCGenerator.AnalyzeData(pName);
				tupStandardMaterial.GetSelfIllumination(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
				int numSubMaps = tupStandardMaterial.GetNumMaps(); CRCGenerator.AnalyzeData(numSubMaps);
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupStandardMaterial.GetMap(subMapIndex);
					TupStandardMaterialMap tupSubMap(pSubMapAgg);
					tupSubMap.GetMapName(&pName); CRCGenerator.AnalyzeData(pName);
					tupSubMap.GetMapReference(intValue); CRCGenerator.AnalyzeData(intValue);
					tupSubMap.GetAmount(floatValue); CRCGenerator.AnalyzeData(floatValue);
					tupSubMap.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
				}
			}
			break;
		case TupMaterial::MULTI_SUB_OBJECT_MATERIAL:
			{
				CRCGenerator.AnalyzeData((int)TupMaterial::MULTI_SUB_OBJECT_MATERIAL);
				TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
//				tupMultiSubObjectMaterial.GetName(&pName); CRCGenerator.AnalyzeData(pName);
				int numSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials(); CRCGenerator.AnalyzeData(numSubMaterials);
				int subMaterialIndex;
				for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
				{
					TupperwareAggregate *pSubObjectMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(subMaterialIndex);
					TupMultiSubObjectMaterialSubMaterial tupMultiSubObjectMaterialSubMaterial(pSubObjectMaterialAgg);
					tupMultiSubObjectMaterialSubMaterial.GetMaterialReference(intValue); CRCGenerator.AnalyzeData(intValue);
					tupMultiSubObjectMaterialSubMaterial.GetSlotName(&pName); CRCGenerator.AnalyzeData(pName);
					tupMultiSubObjectMaterialSubMaterial.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
				}
			}
			break;
		case TupMaterial::BLEND_MATERIAL:
			{
				CRCGenerator.AnalyzeData((int)TupMaterial::BLEND_MATERIAL);
				TupBlendMaterial tupBlendMaterial(pMaterialAgg);
//				tupBlendMaterial.GetName(&pName); CRCGenerator.AnalyzeData(pName);
				tupBlendMaterial.GetMaterial1Reference(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBlendMaterial.GetMaterial2Reference(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBlendMaterial.GetMaskReference(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBlendMaterial.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBlendMaterial.GetMixAmount(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBlendMaterial.GetUpperTransitionZone(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBlendMaterial.GetLowerTransitionZone(floatValue); CRCGenerator.AnalyzeData(floatValue);
			}
			break;
		case TupMaterial::COMPOSITE_MATERIAL:
			{
				CRCGenerator.AnalyzeData((int)TupMaterial::COMPOSITE_MATERIAL);
				TupCompositeMaterial tupCompositeMaterial(pMaterialAgg);
//				tupCompositeMaterial.GetName(&pName); CRCGenerator.AnalyzeData(pName);
				tupCompositeMaterial.GetBaseMaterialReference(intValue); CRCGenerator.AnalyzeData(intValue);
				int numSubMaterials = tupCompositeMaterial.GetNumSubMaterials(); CRCGenerator.AnalyzeData(numSubMaterials);
				int subMaterialIndex;
				for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupCompositeMaterial.GetSubMaterial(subMaterialIndex);
					TupCompositeMaterialSubMaterial subMat(pSubMaterialAgg);
					subMat.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
					subMat.GetMaterialReference(intValue); CRCGenerator.AnalyzeData(intValue);
					subMat.GetMixAmount(floatValue); CRCGenerator.AnalyzeData(floatValue);
				}
			}
			break;
		case TupMaterial::DOUBLE_SIDED_MATERIAL:
			{
				CRCGenerator.AnalyzeData((int)TupMaterial::DOUBLE_SIDED_MATERIAL);
				TupDoubleSidedMaterial tupDoubleSidedMaterial(pMaterialAgg);
//				tupDoubleSidedMaterial.GetName(&pName); CRCGenerator.AnalyzeData(pName);
				tupDoubleSidedMaterial.GetFrontMaterialReference(intValue);	CRCGenerator.AnalyzeData(intValue);
				tupDoubleSidedMaterial.GetBackMaterialReference(intValue); CRCGenerator.AnalyzeData(intValue);
				tupDoubleSidedMaterial.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
				tupDoubleSidedMaterial.GetTranslucency(floatValue); CRCGenerator.AnalyzeData(floatValue);
			}
			break;
	}
	return CRCGenerator.GetResult();
}

unsigned long AnnotateHelper::GetMapCRC(TupMapPool &tupMapPool,int mapRef)
{
	CRC CRCGenerator;
//	char *pName;
	int intValue;
	float floatValue;

	assert(mapRef!=-1);
	TupperwareAggregate *pMapAgg = tupMapPool.GetMap(mapRef);
	assert(pMapAgg);
	TupMap tupMap(pMapAgg);
	switch(tupMap.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			{
				CRCGenerator.AnalyzeData((int)TupMap::BITMAP_MAP);
				TupBitmapMap tupBitmapMap(pMapAgg);
//				tupBitmapMap.GetName(&pName); CRCGenerator.AnalyzeData(pName);
				tupBitmapMap.GetImageReference(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBitmapMap.GetMapping(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBitmapMap.GetMapChannel(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBitmapMap.GetUOffset(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetVOffset(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetUTiling(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetVTiling(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetMapAxis(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBitmapMap.GetUAngle(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetVAngle(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetWAngle(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetBlur(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetBlurOffset(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetNoiseAmount(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetNoiseLevels(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBitmapMap.GetNoiseSize(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetCroppingPlacementU(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetCroppingPlacementV(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetCroppingPlacementW(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetCroppingPlacementH(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetPlacementJitter(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetStartFrame(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBitmapMap.GetPlaybackRate(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetOutputAmount(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetOutputRGBOffset(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetOutputRGBLevel(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetOutputBumpAmount(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetBlendMode(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBitmapMap.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
				char *pUserProperties;
				tupBitmapMap.GetUserProperties(&pUserProperties); CRCGenerator.AnalyzeData(pUserProperties);
			}
			break;
		case TupMap::COMPOSITE_MAP:
			{
				CRCGenerator.AnalyzeData((int)TupMap::COMPOSITE_MAP);
				TupCompositeMap tupCompositeMap(pMapAgg);
//				tupCompositeMap.GetName(&pName); CRCGenerator.AnalyzeData(pName);
				int numSubMaps = tupCompositeMap.GetNumSubMaps(); CRCGenerator.AnalyzeData(numSubMaps);
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupCompositeMap.GetSubMap(subMapIndex);
					TupCompositeMapSubMap tupSubMap(pSubMapAgg);
					tupSubMap.GetMapReference(intValue); CRCGenerator.AnalyzeData(intValue);
					tupSubMap.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
				}
			}
			break;
		case TupMap::MIX_MAP:
			{
				CRCGenerator.AnalyzeData((int)TupMap::MIX_MAP);
				TupMixMap tupMixMap(pMapAgg);
//				tupMixMap.GetName(&pName); CRCGenerator.AnalyzeData(pName);
				tupMixMap.GetMap1Reference(intValue); CRCGenerator.AnalyzeData(intValue);
				tupMixMap.GetMap2Reference(intValue); CRCGenerator.AnalyzeData(intValue);
				tupMixMap.GetMixMapReference(intValue); CRCGenerator.AnalyzeData(intValue);
				tupMixMap.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
				tupMixMap.GetMixAmount(floatValue);	CRCGenerator.AnalyzeData(floatValue);

				tupMixMap.GetLowerTransitionZone(floatValue);	CRCGenerator.AnalyzeData(floatValue);
				tupMixMap.GetLowerTransitionZone(floatValue);	CRCGenerator.AnalyzeData(floatValue);
				tupMixMap.GetOutputAmount(floatValue);	CRCGenerator.AnalyzeData(floatValue);
				tupMixMap.GetOutputRGBOffset(floatValue);	CRCGenerator.AnalyzeData(floatValue);
				tupMixMap.GetOutputRGBLevel(floatValue);	CRCGenerator.AnalyzeData(floatValue);
				tupMixMap.GetOutputBumpAmount(floatValue);	CRCGenerator.AnalyzeData(floatValue);
			}
			break;
	}
	return CRCGenerator.GetResult();
}

static bool CompareFloats(float *pFloat1,float *pFloat2,int total)
{
	for (int index=0;index<total;index++)
	{
		if (pFloat1[index]!=pFloat2[index])
			return false;
	}
	return true;
}

bool AnnotateHelper::CompareMaterials(TupMaterialPool &tupMaterialPool,int materialRef1,int materialRef2)
{
	char *pName1,*pName2;
	int intValue1,intValue2;
	float floatValue1,floatValue2;
	float *pFloatList1,*pFloatList2;

	assert(materialRef1!=-1);
	assert(materialRef2!=-1);
	TupperwareAggregate *pMaterial1Agg = tupMaterialPool.GetMaterial(materialRef1);
	TupperwareAggregate *pMaterial2Agg = tupMaterialPool.GetMaterial(materialRef2);
	assert(pMaterial1Agg);
	assert(pMaterial2Agg);
	TupMaterial tupMaterial1(pMaterial1Agg);
	TupMaterial tupMaterial2(pMaterial1Agg);
	if (tupMaterial1.GetMaterialType()!=tupMaterial2.GetMaterialType()) return false;
	switch(tupMaterial1.GetMaterialType())
	{
		case TupMaterial::STANDARD_MATERIAL:
			{
				TupStandardMaterial tupStandardMaterial1(pMaterial1Agg);
				TupStandardMaterial tupStandardMaterial2(pMaterial2Agg);

//				tupStandardMaterial1.GetName(&pName1); tupStandardMaterial2.GetName(&pName2); if (strcmp(pName1,pName2)) return false;
				tupStandardMaterial1.GetAmbientColor(&pFloatList1); tupStandardMaterial2.GetAmbientColor(&pFloatList2);	if (!CompareFloats(pFloatList1,pFloatList2,3)) return false;
				tupStandardMaterial1.GetDiffuseColor(&pFloatList1); tupStandardMaterial2.GetDiffuseColor(&pFloatList2); if (!CompareFloats(pFloatList1,pFloatList2,3)) return false;
				tupStandardMaterial1.GetSpecularColor(&pFloatList1); tupStandardMaterial2.GetSpecularColor(&pFloatList2); if (!CompareFloats(pFloatList1,pFloatList2,3)) return false;
				tupStandardMaterial1.GetFilterColor(&pFloatList1); tupStandardMaterial2.GetFilterColor(&pFloatList2); if (!CompareFloats(pFloatList1,pFloatList2,3)) return false;
				tupStandardMaterial1.GetGlossiness(floatValue1); tupStandardMaterial2.GetGlossiness(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetSpecularLevel(floatValue1); tupStandardMaterial2.GetSpecularLevel(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetSpecularSoften(floatValue1); tupStandardMaterial2.GetSpecularSoften(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetTransparency(floatValue1); tupStandardMaterial2.GetTransparency(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetWireSize(floatValue1); tupStandardMaterial2.GetWireSize(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetIndexOfRefraction(floatValue1); tupStandardMaterial2.GetIndexOfRefraction(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetReflectionDimIntensity(floatValue1); tupStandardMaterial2.GetReflectionDimIntensity(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetReflectionDimMultiplier(floatValue1); tupStandardMaterial2.GetReflectionDimMultiplier(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetOpacity(floatValue1); tupStandardMaterial2.GetOpacity(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetOpacityFalloff(floatValue1); tupStandardMaterial2.GetOpacityFalloff(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetTransparencyType(intValue1); tupStandardMaterial2.GetTransparencyType(intValue2); if (intValue1!=intValue2) return false;
				tupStandardMaterial1.GetShadingTypeName(&pName1); tupStandardMaterial2.GetShadingTypeName(&pName2); if (strcmp(pName1,pName2)) return false;
				tupStandardMaterial1.GetSelfIllumination(floatValue1); tupStandardMaterial2.GetSelfIllumination(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetFlags(intValue1); tupStandardMaterial2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
				int numSubMaps1 = tupStandardMaterial1.GetNumMaps();
				int numSubMaps2 = tupStandardMaterial2.GetNumMaps();
				if (numSubMaps1!=numSubMaps2) return false;
				for (int subMapIndex=0;subMapIndex<numSubMaps1;subMapIndex++)
				{
					TupperwareAggregate *pSubMap1Agg = tupStandardMaterial1.GetMap(subMapIndex);
					TupperwareAggregate *pSubMap2Agg = tupStandardMaterial1.GetMap(subMapIndex);
					TupStandardMaterialMap tupSubMap1(pSubMap1Agg);
					TupStandardMaterialMap tupSubMap2(pSubMap2Agg);
					tupSubMap1.GetMapName(&pName1); tupSubMap2.GetMapName(&pName2); if (strcmp(pName1,pName2)) return false;
					tupSubMap1.GetMapReference(intValue1); tupSubMap2.GetMapReference(intValue2); if (intValue1!=intValue2) return false;
					tupSubMap1.GetAmount(floatValue1); tupSubMap2.GetAmount(floatValue2); if (floatValue1!=floatValue2) return false;
					tupSubMap1.GetFlags(intValue1); tupSubMap2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
				}
			}
			break;
		case TupMaterial::MULTI_SUB_OBJECT_MATERIAL:
			{
				TupMultiSubObjectMaterial tupMultiSubObjectMaterial1(pMaterial1Agg);
				TupMultiSubObjectMaterial tupMultiSubObjectMaterial2(pMaterial2Agg);
//				tupMultiSubObjectMaterial1.GetName(&pName1); tupMultiSubObjectMaterial2.GetName(&pName2); if (strcmp(pName1,pName2)) return false;
				int numSubMaterials1 = tupMultiSubObjectMaterial1.GetNumSubMaterials();
				int numSubMaterials2 = tupMultiSubObjectMaterial2.GetNumSubMaterials();
				int subMaterialIndex;
				if (numSubMaterials1!=numSubMaterials1) return false;
				for (subMaterialIndex=0;subMaterialIndex<numSubMaterials1;subMaterialIndex++)
				{
					TupperwareAggregate *pSubObjectMaterial1Agg = tupMultiSubObjectMaterial1.GetSubMaterial(subMaterialIndex);
					TupperwareAggregate *pSubObjectMaterial2Agg = tupMultiSubObjectMaterial2.GetSubMaterial(subMaterialIndex);
					TupMultiSubObjectMaterialSubMaterial tupMultiSubObjectMaterialSubMaterial1(pSubObjectMaterial1Agg);
					TupMultiSubObjectMaterialSubMaterial tupMultiSubObjectMaterialSubMaterial2(pSubObjectMaterial2Agg);
					tupMultiSubObjectMaterialSubMaterial1.GetMaterialReference(intValue1); tupMultiSubObjectMaterialSubMaterial2.GetMaterialReference(intValue2); if (intValue1!=intValue2) return false;
					tupMultiSubObjectMaterialSubMaterial1.GetSlotName(&pName1);	tupMultiSubObjectMaterialSubMaterial2.GetSlotName(&pName2); if (strcmp(pName1,pName2)) return false;
					tupMultiSubObjectMaterialSubMaterial1.GetFlags(intValue1); tupMultiSubObjectMaterialSubMaterial2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
				}
			}
			break;
		case TupMaterial::BLEND_MATERIAL:
			{
				TupBlendMaterial tupBlendMaterial1(pMaterial1Agg);
				TupBlendMaterial tupBlendMaterial2(pMaterial2Agg);
//				tupBlendMaterial1.GetName(&pName1); tupBlendMaterial2.GetName(&pName2); if (strcmp(pName1,pName2)) return false;
				tupBlendMaterial1.GetMaterial1Reference(intValue1); tupBlendMaterial2.GetMaterial1Reference(intValue2); if (intValue1!=intValue2) return false;
				tupBlendMaterial1.GetMaterial2Reference(intValue1); tupBlendMaterial2.GetMaterial2Reference(intValue2); if (intValue1!=intValue2) return false;
				tupBlendMaterial1.GetMaskReference(intValue1); tupBlendMaterial2.GetMaskReference(intValue2); if (intValue1!=intValue2) return false;
				tupBlendMaterial1.GetFlags(intValue1); tupBlendMaterial2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
				tupBlendMaterial1.GetMixAmount(floatValue1); tupBlendMaterial2.GetMixAmount(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBlendMaterial1.GetUpperTransitionZone(floatValue1); tupBlendMaterial2.GetUpperTransitionZone(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBlendMaterial1.GetLowerTransitionZone(floatValue1); tupBlendMaterial2.GetLowerTransitionZone(floatValue2); if (floatValue1!=floatValue2) return false;
			}
			break;
		case TupMaterial::COMPOSITE_MATERIAL:
			{
				TupCompositeMaterial tupCompositeMaterial1(pMaterial1Agg);
				TupCompositeMaterial tupCompositeMaterial2(pMaterial2Agg);
//				tupCompositeMaterial1.GetName(&pName1); tupCompositeMaterial2.GetName(&pName2); if (strcmp(pName1,pName2)) return false;
				tupCompositeMaterial1.GetBaseMaterialReference(intValue1); tupCompositeMaterial2.GetBaseMaterialReference(intValue2); if (intValue1!=intValue2) return false;
				int numSubMaterials1 = tupCompositeMaterial1.GetNumSubMaterials();
				int numSubMaterials2 = tupCompositeMaterial2.GetNumSubMaterials();
				if (numSubMaterials1!=numSubMaterials2)	return false;
				int subMaterialIndex;
				for (subMaterialIndex=0;subMaterialIndex<numSubMaterials1;subMaterialIndex++)
				{
					TupperwareAggregate *pSubMaterial1Agg = tupCompositeMaterial1.GetSubMaterial(subMaterialIndex);
					TupperwareAggregate *pSubMaterial2Agg = tupCompositeMaterial2.GetSubMaterial(subMaterialIndex);
					TupCompositeMaterialSubMaterial subMat1(pSubMaterial1Agg);
					TupCompositeMaterialSubMaterial subMat2(pSubMaterial2Agg);
					subMat1.GetFlags(intValue1); subMat2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
					subMat1.GetMaterialReference(intValue1); subMat2.GetMaterialReference(intValue2); if (intValue1!=intValue2) return false;
					subMat1.GetMixAmount(floatValue1); subMat2.GetMixAmount(floatValue2); if (floatValue1!=floatValue2) return false;
				}
			}
			break;
		case TupMaterial::DOUBLE_SIDED_MATERIAL:
			{
				TupDoubleSidedMaterial tupDoubleSidedMaterial1(pMaterial1Agg);
				TupDoubleSidedMaterial tupDoubleSidedMaterial2(pMaterial2Agg);
//				tupDoubleSidedMaterial1.GetName(&pName1); tupDoubleSidedMaterial2.GetName(&pName2); if (strcmp(pName1,pName2)) return false;
				tupDoubleSidedMaterial1.GetFrontMaterialReference(intValue1); tupDoubleSidedMaterial2.GetFrontMaterialReference(intValue2); if (intValue1!=intValue2) return false;
				tupDoubleSidedMaterial1.GetBackMaterialReference(intValue1); tupDoubleSidedMaterial2.GetBackMaterialReference(intValue2); if (intValue1!=intValue2) return false;
				tupDoubleSidedMaterial1.GetFlags(intValue1); tupDoubleSidedMaterial2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
				tupDoubleSidedMaterial1.GetTranslucency(floatValue1); tupDoubleSidedMaterial2.GetTranslucency(floatValue2); if (floatValue1!=floatValue2) return false;
			}
			break;
	}
	return true;
}

bool AnnotateHelper::CompareMaps(TupMapPool &tupMapPool,int mapRef1,int mapRef2)
{
//	char *pName1,*pName2;
	int intValue1,intValue2;
	float floatValue1,floatValue2;

	assert(mapRef1!=-1);
	assert(mapRef2!=-1);
	TupperwareAggregate *pMap1Agg = tupMapPool.GetMap(mapRef1);
	TupperwareAggregate *pMap2Agg = tupMapPool.GetMap(mapRef2);
	assert(pMap1Agg);
	assert(pMap2Agg);
	TupMap tupMap1(pMap1Agg);
	TupMap tupMap2(pMap2Agg);
	if (tupMap1.GetMapType()!=tupMap2.GetMapType()) return false;
	switch(tupMap1.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			{
				TupBitmapMap tupBitmapMap1(pMap1Agg);
				TupBitmapMap tupBitmapMap2(pMap2Agg);
//				tupBitmapMap1.GetName(&pName1); tupBitmapMap2.GetName(&pName2); if (strcmp(pName1,pName2)) return false;
				tupBitmapMap1.GetImageReference(intValue1); tupBitmapMap2.GetImageReference(intValue2); if (intValue1!=intValue2) return false;
				tupBitmapMap1.GetMapping(intValue1); tupBitmapMap2.GetMapping(intValue2); if (intValue1!=intValue2) return false;
				tupBitmapMap1.GetMapChannel(intValue1); tupBitmapMap2.GetMapChannel(intValue2); if (intValue1!=intValue2) return false;
				tupBitmapMap1.GetUOffset(floatValue1); tupBitmapMap2.GetUOffset(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetVOffset(floatValue1); tupBitmapMap2.GetVOffset(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetUTiling(floatValue1); tupBitmapMap2.GetUTiling(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetVTiling(floatValue1); tupBitmapMap2.GetVTiling(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetMapAxis(intValue1); tupBitmapMap2.GetMapAxis(intValue2); if (intValue1!=intValue2) return false;
				tupBitmapMap1.GetUAngle(floatValue1); tupBitmapMap2.GetUAngle(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetVAngle(floatValue1); tupBitmapMap2.GetVAngle(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetWAngle(floatValue1); tupBitmapMap2.GetWAngle(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetBlur(floatValue1); tupBitmapMap2.GetBlur(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetBlurOffset(floatValue1); tupBitmapMap2.GetBlurOffset(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetNoiseAmount(floatValue1); tupBitmapMap2.GetNoiseAmount(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetNoiseLevels(intValue1); tupBitmapMap2.GetNoiseLevels(intValue2); if (intValue1!=intValue2) return false;
				tupBitmapMap1.GetNoiseSize(floatValue1); tupBitmapMap2.GetNoiseSize(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetCroppingPlacementU(floatValue1); tupBitmapMap2.GetCroppingPlacementU(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetCroppingPlacementV(floatValue1); tupBitmapMap2.GetCroppingPlacementV(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetCroppingPlacementW(floatValue1); tupBitmapMap2.GetCroppingPlacementW(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetCroppingPlacementH(floatValue1); tupBitmapMap2.GetCroppingPlacementH(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetPlacementJitter(floatValue1); tupBitmapMap2.GetPlacementJitter(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetStartFrame(intValue1); tupBitmapMap2.GetStartFrame(intValue2); if (intValue1!=intValue2) return false;
				tupBitmapMap1.GetPlaybackRate(floatValue1); tupBitmapMap2.GetPlaybackRate(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetOutputAmount(floatValue1); tupBitmapMap2.GetOutputAmount(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetOutputRGBOffset(floatValue1); tupBitmapMap2.GetOutputRGBOffset(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetOutputRGBLevel(floatValue1); tupBitmapMap2.GetOutputRGBLevel(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetOutputBumpAmount(floatValue1); tupBitmapMap2.GetOutputBumpAmount(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetBlendMode(intValue1); tupBitmapMap2.GetBlendMode(intValue2); if (intValue1!=intValue2) return false;
				tupBitmapMap1.GetFlags(intValue1); tupBitmapMap2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
				char *pUserProperties1,*pUserProperties2;
				tupBitmapMap1.GetUserProperties(&pUserProperties1); tupBitmapMap2.GetUserProperties(&pUserProperties2); 
				// return false if they are different (taking into account NULL string pointers)
				if (pUserProperties1)
				{
					if (pUserProperties2)
					{
						if (strcmp(pUserProperties1,pUserProperties2))
							return false;
					}
					else
					{
						return false;
					}
				}
				else
				{
					if (pUserProperties2)
					{
						return false;
					}
				}
			}
			break;
		case TupMap::COMPOSITE_MAP:
			{
				TupCompositeMap tupCompositeMap1(pMap1Agg);
				TupCompositeMap tupCompositeMap2(pMap2Agg);
//				tupCompositeMap1.GetName(&pName1); tupCompositeMap2.GetName(&pName2); if (strcmp(pName1,pName2)) return false;
				int numSubMaps1 = tupCompositeMap1.GetNumSubMaps();
				int numSubMaps2 = tupCompositeMap2.GetNumSubMaps();
				if (numSubMaps1!=numSubMaps2) return false;
				for (int subMapIndex=0;subMapIndex<numSubMaps1;subMapIndex++)
				{
					TupperwareAggregate *pSubMap1Agg = tupCompositeMap1.GetSubMap(subMapIndex);
					TupperwareAggregate *pSubMap2Agg = tupCompositeMap2.GetSubMap(subMapIndex);
					TupCompositeMapSubMap tupSubMap1(pSubMap1Agg);
					TupCompositeMapSubMap tupSubMap2(pSubMap2Agg);
					tupSubMap1.GetMapReference(intValue1); tupSubMap2.GetMapReference(intValue2); if (intValue1!=intValue2) return false;
					tupSubMap1.GetFlags(intValue1); tupSubMap2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
				}
			}
			break;
		case TupMap::MIX_MAP:
			{
				TupMixMap tupMixMap1(pMap1Agg);
				TupMixMap tupMixMap2(pMap2Agg);
//				tupMixMap1.GetName(&pName1); tupMixMap1.GetName(&pName2); if (strcmp(pName1,pName2)) return false;
				tupMixMap1.GetMap1Reference(intValue1); tupMixMap2.GetMap1Reference(intValue2); if (intValue1!=intValue2) return false;
				tupMixMap1.GetMap2Reference(intValue1); tupMixMap2.GetMap2Reference(intValue2); if (intValue1!=intValue2) return false;
				tupMixMap1.GetMixMapReference(intValue1); tupMixMap2.GetMixMapReference(intValue2); if (intValue1!=intValue2) return false;
				tupMixMap1.GetFlags(intValue1); tupMixMap2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
				tupMixMap1.GetMixAmount(floatValue1); tupMixMap2.GetMixAmount(floatValue2); if (floatValue1!=floatValue2) return false;
				tupMixMap1.GetLowerTransitionZone(floatValue1); tupMixMap2.GetLowerTransitionZone(floatValue2); if (floatValue1!=floatValue2) return false;
				tupMixMap1.GetLowerTransitionZone(floatValue1); tupMixMap2.GetLowerTransitionZone(floatValue2); if (floatValue1!=floatValue2) return false;
				tupMixMap1.GetOutputAmount(floatValue1); tupMixMap2.GetOutputAmount(floatValue2); if (floatValue1!=floatValue2) return false;
				tupMixMap1.GetOutputRGBOffset(floatValue1); tupMixMap2.GetOutputRGBOffset(floatValue2); if (floatValue1!=floatValue2) return false;
				tupMixMap1.GetOutputRGBLevel(floatValue1); tupMixMap2.GetOutputRGBLevel(floatValue2); if (floatValue1!=floatValue2) return false;
				tupMixMap1.GetOutputBumpAmount(floatValue1); tupMixMap2.GetOutputBumpAmount(floatValue2); if (floatValue1!=floatValue2) return false;
			}
			break;
	}
	return true;
}

void AnnotateHelper::CalculateUsedNodesObjectsAndModifiers(int nodeRef,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool,
																			  Keeper<int> &usedNodes,Keeper<int> &usedObjects,Keeper<int> &usedModifiers)
{

	usedNodes.Add(nodeRef);

	TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeRef);
	TupNode tupNode(pNodeAgg);
	int objectRef;
	tupNode.GetObjectReference(objectRef);
	if (objectRef!=-1)
	{
		CalculateUsedObjectsAndModifiers(objectRef,tupObjectPool,tupModifierPool,usedObjects,usedModifiers);
	}
	int numChildNodes = tupNode.GetNumChildNodes();
	for (int childIndex=0;childIndex<numChildNodes;childIndex++)
	{
		int childRef;
		tupNode.GetChildNodeReference(childIndex,childRef);
		CalculateUsedNodesObjectsAndModifiers(childRef,tupNodePool,tupObjectPool,tupModifierPool,usedNodes,usedObjects,usedModifiers);
	}
}

void AnnotateHelper::CalculateUsedObjectsAndModifiers(int objectRef,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool,
																		Keeper<int> &usedObjects,Keeper<int> &usedModifiers)
{
	usedObjects.Add(objectRef);

	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectRef);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		// try the one I point to
		TupDerived tupDerived(pObjectAgg);
		int numModifierSlots = tupDerived.GetNumModifierSlots();
		for (int modifierIndex=0;modifierIndex<numModifierSlots;modifierIndex++)
		{
			TupperwareAggregate *pTupDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
			if (pTupDerivedModifierAgg)
			{
				TupDerivedModifier tupDerivedModifier(pTupDerivedModifierAgg);
				int modifierReference;
				tupDerivedModifier.GetModifierReference(modifierReference);
				usedModifiers.Add(modifierReference);
			}
		}
		int childObjectRef;
		tupDerived.GetObjectReference(childObjectRef);
		assert(childObjectRef>=0);
		CalculateUsedObjectsAndModifiers(childObjectRef,tupObjectPool,tupModifierPool,usedObjects,usedModifiers);
	}
}

void AnnotateHelper::FindAllParentDerivedObjects(int objectRef,TupObjectPool &tupObjectPool,Keeper<int> &parentObjects)
{
	Keeper<int> refsToCheckFor;
	refsToCheckFor.Add(objectRef);

	int numObjects = tupObjectPool.GetNumObjects();
	TupArray<int> childObjectRefs;
	childObjectRefs.SetSize(numObjects);
	int objectIndex;
	// loop through and set the child object refs
	for (objectIndex=0;objectIndex<numObjects;objectIndex++)
	{
		childObjectRefs[objectIndex] = -1; // start with null
		TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectIndex);
		if (!pObjectAgg)
			continue;
		TupObject tupObject(pObjectAgg);
		// deal only with derived objects
		if (tupObject.GetObjectType() == TupObject::DERIVED)
		{
			// try the one I point to
			TupDerived tupDerived(pObjectAgg);
			int childObjectRef;
			tupDerived.GetObjectReference(childObjectRef);
			assert(childObjectRef>=0);
			childObjectRefs[objectIndex] = childObjectRef;
		}
	}

	bool bAddedRefToCheck = true;
	// keep looping until none are added
	while (bAddedRefToCheck)
	{
		bAddedRefToCheck = false;
		// loop through all objects
		for (objectIndex=0;objectIndex<numObjects;objectIndex++)
		{
			// has child reference
			if (childObjectRefs[objectIndex]!=-1)
			{
				// if original reference (skip)
				if (objectIndex==objectRef)
					continue;

				// if checked before (skip)
				if (refsToCheckFor.GetIndex(objectIndex)!=-1)
					continue;

				// if child is a parent make me one too
				if (refsToCheckFor.GetIndex(childObjectRefs[objectIndex])!=-1)
				{
					parentObjects.Add(objectIndex); // new parent
					refsToCheckFor.Add(objectIndex); // new one to try
					bAddedRefToCheck = true;
				}
			}
		}
	}
}
