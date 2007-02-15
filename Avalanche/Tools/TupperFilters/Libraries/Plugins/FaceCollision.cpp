////////////////////////////////////////////////////////////////////////////
//
// FaceCollision
//
// Class to handle face collision data
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: FaceCollision.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:03a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/10/03    Time: 4:41p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * disabled bug
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/06/03    Time: 4:07p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * one material per mesh collision problem
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/25/02    Time: 11:33a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * new collision attribute system (using image names)
 */

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "PluginsPch.h"
#include "FaceCollision.h"
#include "MaterialImageRefs.h"
#include "AnnotateHelper.h"
#include "PluginLog.h"

#define BEGIN_BLOCK	"BEGIN OBJECTATTRIBUTES"
#define END_BLOCK	"END OBJECTATTRIBUTES"

bool FaceCollisionManager::Attributes::operator==(const FaceCollisionManager::Attributes& s) const
{
	return (m_bCollision==s.m_bCollision &&
			m_surfaceType==s.m_surfaceType &&
			m_obbValue==s.m_obbValue );
}

FaceCollisionManager::FaceCollisionManager(TupConfigRoot &tupNodeTreeRoot,TupConfigRoot &tupFaceTreeRoot,
										   TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,bool bTerrainMode) :
m_tupObjectPool(tupObjectPool),
m_tupModifierPool(tupModifierPool),
m_tupMaterialPool(tupMaterialPool),
m_tupMapPool(tupMapPool),
m_tupImagePool(tupImagePool),
m_tupNodeTreeRoot(tupNodeTreeRoot),
m_tupFaceTreeRoot(tupFaceTreeRoot),
m_bTerrainMode(bTerrainMode)
{
	BuildMaterialStageData();
}

static const int COLLISIONKEY = 0;
static const int DEFAULTCOLLISIONKEY = 1;
static const int SURFACETYPEKEY = 2;
static const int OBBVALUEKEY = 3;

void FaceCollisionManager::BuildFaceCollisionAttributes(TupNode &tupNode,TupArray<FaceCollisionManager::Attributes> &faceAttributeArray,TupArray<int> &faceAttributeIndices)
{
	char *pNodeAttributes;
	tupNode.GetUserProperties(&pNodeAttributes);
	TupKeyValue tupAttributes(pNodeAttributes,BEGIN_BLOCK,END_BLOCK);
	AttributeStage nodeStage;
	// set up the node stage
	BuildStageFromAttributes(nodeStage,m_tupNodeTreeRoot,tupAttributes);

	Keeper<Attributes> attributeKeeper;

	// if we ignore the mesh or am disabled
	if (!nodeStage.m_bUseMesh || nodeStage.m_bDisabled)
	{
		Attributes newAttribute;
		newAttribute.m_bCollision = !nodeStage.m_bDisabled;
		newAttribute.m_surfaceType =  nodeStage.m_surfaceType;
		newAttribute.m_obbValue = nodeStage.m_obbValue;
		faceAttributeIndices.SetSize(1);
		faceAttributeIndices[0] = attributeKeeper.Add(newAttribute);
	}
	else // look at the mesh
	{
		int materialRef;
		tupNode.GetMaterialReference(materialRef);
		TupArray<AttributeStage> materialStageArray;
		BuildNodeMaterialStageData(materialRef,materialStageArray);

		int objectRef;
		tupNode.GetObjectReference(objectRef);
		TupperwareAggregate *pFaceAttributesAgg = AnnotateHelper::FindFaceAttributes(m_tupModifierPool,m_tupObjectPool,objectRef);

		TupArray<AttributeStage> faceAttributeStageArray;
		char **pAttributeVerts = NULL;
		int numAttributeVerts = 0;
		int *pAttributeIndices = NULL;

		// if there are some face Attributes build some data for each unique attribute in the modifier
		if (pFaceAttributesAgg)
		{
			BuildFaceAttributeStageData(pFaceAttributesAgg,faceAttributeStageArray,&pAttributeVerts,numAttributeVerts,&pAttributeIndices);
		}

		TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(m_tupObjectPool,objectRef);
		assert(pBaseMeshAgg);
		TupTriMesh tupTriMesh(pBaseMeshAgg);

		int *pFaceMaterialIndices;
		int numFaces;
		tupTriMesh.GetFaceMaterialIndices(&pFaceMaterialIndices,numFaces);
		faceAttributeIndices.SetSize(numFaces);

		int faceIndex;
		for (faceIndex=0;faceIndex<numFaces;faceIndex++)
		{
			Attributes newAttribute;
			// if we have face attributes
			bool bSetAttribute = false;
			if (pFaceAttributesAgg)
			{
				AttributeStage &faceStage = faceAttributeStageArray[pAttributeIndices[faceIndex]];
				// if the buck stops here
				if (!faceStage.m_bUseMesh || faceStage.m_bDisabled)
				{
					newAttribute.m_bCollision = !faceStage.m_bDisabled;
					newAttribute.m_surfaceType =  faceStage.m_surfaceType;
					newAttribute.m_obbValue = faceStage.m_obbValue;
					faceAttributeIndices[faceIndex] = attributeKeeper.Add(newAttribute);
					bSetAttribute = true;
				}
			}
			if (!bSetAttribute)
			{
				AttributeStage &materialStage = materialStageArray[pFaceMaterialIndices[faceIndex]];
				// if the buck stops here
				if (!materialStage.m_bUseMesh)
				{
					newAttribute.m_bCollision = !materialStage.m_bDisabled;
					newAttribute.m_surfaceType =  materialStage.m_surfaceType;
					newAttribute.m_obbValue = materialStage.m_obbValue;
					faceAttributeIndices[faceIndex] = attributeKeeper.Add(newAttribute);
					bSetAttribute = true;
				}
				// if defaults are active on the material
				else if (materialStage.m_bDefaultCollision || materialStage.m_bDefaultDisabled)
				{
					newAttribute.m_bCollision = materialStage.m_bDefaultCollision;
					newAttribute.m_surfaceType =  materialStage.m_surfaceType;
					newAttribute.m_obbValue = materialStage.m_obbValue;
					faceAttributeIndices[faceIndex] = attributeKeeper.Add(newAttribute);
					bSetAttribute = true;
				}
			}
			// if still not set use the correct default from the face attributes
			if (!bSetAttribute)
			{
				if (pFaceAttributesAgg)
				{
					AttributeStage &faceStage = faceAttributeStageArray[pAttributeIndices[faceIndex]];
					// if defaults are active on the face
					if (faceStage.m_bDefaultCollision || faceStage.m_bDefaultDisabled)
					{
						newAttribute.m_bCollision = faceStage.m_bDefaultCollision;
						newAttribute.m_surfaceType =  faceStage.m_surfaceType;
						newAttribute.m_obbValue = faceStage.m_obbValue;
						faceAttributeIndices[faceIndex] = attributeKeeper.Add(newAttribute);
						bSetAttribute = true;
					}
				}
			}
			// if still not set use the correct default from the node attributes
			if (!bSetAttribute)
			{
				newAttribute.m_bCollision = nodeStage.m_bDefaultCollision;
				newAttribute.m_surfaceType =  nodeStage.m_surfaceType;
				newAttribute.m_obbValue = nodeStage.m_obbValue;
				faceAttributeIndices[faceIndex] = attributeKeeper.Add(newAttribute);
			}
		}
	}
	// copy the keeper array
	faceAttributeArray = attributeKeeper.GetArray();
}

void FaceCollisionManager::BuildFaceAttributeStageData(TupperwareAggregate *pFaceAttributesAgg,TupArray<AttributeStage> &faceStageArray,char ***pAttributeVerts,int &numAttributeVerts,int **pAttributeIndices)
{
	TupFaceAttributesModifierContext tupFaceAttributesModifierContext(pFaceAttributesAgg);
	int numAttributeFaces;
	tupFaceAttributesModifierContext.GetFaceAttributeVerts(pAttributeVerts,numAttributeVerts);
	tupFaceAttributesModifierContext.GetFaceAttributeIndices(pAttributeIndices,numAttributeFaces);
	assert(numAttributeVerts);
	faceStageArray.SetSize(numAttributeVerts);

	// go through each attribute and fill in each with proper data
	for (int attributesIndex=0;attributesIndex<numAttributeVerts;attributesIndex++)
	{
		AttributeStage &faceStage = faceStageArray[attributesIndex];
		TupKeyValue tupAttributes((*pAttributeVerts)[attributesIndex],NULL,NULL);
		BuildStageFromAttributes(faceStage,m_tupFaceTreeRoot,tupAttributes);
	}
}

void FaceCollisionManager::BuildStageFromAttributes(AttributeStage &stage,TupConfigRoot &tupConfigRoot,TupKeyValue &tupAttributes)
{
	// set defaults
	stage.m_bDefaultDisabled = false;
	stage.m_bDefaultCollision = false;
	stage.m_bDisabled = false;
	stage.m_bUseMesh = true;
	stage.m_surfaceType = 0;
	stage.m_obbValue = 0;

	TupKeyValueContext tupKeyValueContext;
	tupKeyValueContext.AddKey("CollisionType");
	tupKeyValueContext.AddKey("DefaultCollisionType");
	tupKeyValueContext.AddKey("SurfaceType");
	tupKeyValueContext.AddKey("OBBValue");
	tupConfigRoot.GetKeyValueContexts(tupAttributes,tupKeyValueContext,false);

	if (m_bTerrainMode)
	{
		// UseMesh
		if (!strcmp(tupKeyValueContext.GetValueAsString(COLLISIONKEY),"UseMesh"))
		{
			if (!strcmp(tupKeyValueContext.GetValueAsString(DEFAULTCOLLISIONKEY),"UseMesh"))
			{
				// leave defaults
			}
			else if (!strcmp(tupKeyValueContext.GetValueAsString(DEFAULTCOLLISIONKEY),"Disabled") ||
					 !strcmp(tupKeyValueContext.GetValueAsString(DEFAULTCOLLISIONKEY),"OBBOnly"))
			{
				stage.m_bDefaultDisabled = true;
			}
			else if (!strcmp(tupKeyValueContext.GetValueAsString(DEFAULTCOLLISIONKEY),"TerrainOnly") ||
					 !strcmp(tupKeyValueContext.GetValueAsString(DEFAULTCOLLISIONKEY),"TerrainAndOBB"))
			{
				stage.m_bDefaultCollision = true;
			}
		}
		else if (!strcmp(tupKeyValueContext.GetValueAsString(COLLISIONKEY),"Disabled") ||
			!strcmp(tupKeyValueContext.GetValueAsString(COLLISIONKEY),"OBBOnly"))
		{
			stage.m_bDisabled = true;
		}
		else if (!strcmp(tupKeyValueContext.GetValueAsString(COLLISIONKEY),"TerrainOnly") ||
				 !strcmp(tupKeyValueContext.GetValueAsString(COLLISIONKEY),"TerrainAndOBB"))
		{
			stage.m_bUseMesh = false;
		}
	}
	else
	{
		// UseMesh
		if (!strcmp(tupKeyValueContext.GetValueAsString(COLLISIONKEY),"UseMesh"))
		{
			if (!strcmp(tupKeyValueContext.GetValueAsString(DEFAULTCOLLISIONKEY),"UseMesh"))
			{
				// leave defaults
			}
			else if (!strcmp(tupKeyValueContext.GetValueAsString(DEFAULTCOLLISIONKEY),"Disabled") ||
					 !strcmp(tupKeyValueContext.GetValueAsString(DEFAULTCOLLISIONKEY),"TerrainOnly"))
			{
				stage.m_bDefaultDisabled = true;
			}
			else if (!strcmp(tupKeyValueContext.GetValueAsString(DEFAULTCOLLISIONKEY),"OBBOnly") ||
					 !strcmp(tupKeyValueContext.GetValueAsString(DEFAULTCOLLISIONKEY),"TerrainAndOBB"))
			{
				stage.m_bDefaultCollision = true;
			}
		}
		else if (!strcmp(tupKeyValueContext.GetValueAsString(COLLISIONKEY),"Disabled") ||
			!strcmp(tupKeyValueContext.GetValueAsString(COLLISIONKEY),"TerrainOnly"))
		{
			stage.m_bDisabled = true;
		}
		else if (!strcmp(tupKeyValueContext.GetValueAsString(COLLISIONKEY),"OBBOnly") ||
				 !strcmp(tupKeyValueContext.GetValueAsString(COLLISIONKEY),"TerrainAndOBB"))
		{
			stage.m_bUseMesh = false;
		}
	}
	stage.m_surfaceType = tupKeyValueContext.GetSelectionIndex(SURFACETYPEKEY);
	stage.m_obbValue = tupKeyValueContext.GetValueAsInt(OBBVALUEKEY);
}

void FaceCollisionManager::BuildNodeMaterialStageData(int materialRef,TupArray<AttributeStage> &materialStageArray)
{
	TupperwareAggregate *pMaterialAgg = NULL;
	if (materialRef!=-1)
	{
		pMaterialAgg = m_tupMaterialPool.GetMaterial(materialRef);
	}
	bool bBuilt = false;
	if (pMaterialAgg)
	{
		TupMaterial tupMaterial(pMaterialAgg);
		if (tupMaterial.GetMaterialType() == TupMaterial::MULTI_SUB_OBJECT_MATERIAL)
		{
			TupMultiSubObjectMaterial tupMultiSubObjectMaterial(tupMaterial);
			int numSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();
			if (numSubMaterials)
			{
				bBuilt = true;
				materialStageArray.SetSize(numSubMaterials);
				for (int subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
				{
					// set the defaults
					AttributeStage &stage = materialStageArray[subMaterialIndex];
					stage.m_bDefaultDisabled = false;
					stage.m_bDefaultCollision = false;
					stage.m_bDisabled = false;
					stage.m_bUseMesh = true;
					stage.m_surfaceType = 0;
					stage.m_obbValue = 0;

					TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(subMaterialIndex);
					if (!pSubMaterialAgg)
						continue;
					TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
					int subMaterialReference;
					tupSubMaterial.GetMaterialReference(subMaterialReference);
					if (subMaterialReference==-1)
						continue;
					// use the one from the sub material if present
					stage = m_materialStages[subMaterialReference];
				}
			}
		}
		else // if not a multisub material
		{
			bBuilt = true;
			materialStageArray.SetSize(1);
			materialStageArray[0] = m_materialStages[materialRef];
		}
	}

	if (!bBuilt) // no stage set up yet (use defaults)
	{
		materialStageArray.SetSize(1);
		AttributeStage &stage = materialStageArray[0];
		stage.m_bDefaultDisabled = false;
		stage.m_bDefaultCollision = false;
		stage.m_bDisabled = false;
		stage.m_bUseMesh = true;
		stage.m_surfaceType = 0;
		stage.m_obbValue = 0;
	}
}



void FaceCollisionManager::BuildMaterialStageData(void)
{
	TupArray<AttributeStage> imageStages;
	BuildImageAttributes(imageStages);

	MaterialImageRefsManager materialImageRefsManager(m_tupMaterialPool,m_tupMapPool);

	int numMaterials = m_tupMaterialPool.GetNumMaterials();
	m_materialStages.SetSize(numMaterials);
	int materialIndex;
	// start with them all set to default
	for (materialIndex=0;materialIndex<numMaterials;materialIndex++)
	{
		AttributeStage &stage = m_materialStages[materialIndex];
		stage.m_bDefaultDisabled = false;
		stage.m_bDefaultCollision = false;
		stage.m_bDisabled = false;
		stage.m_bUseMesh = true;
		stage.m_surfaceType = 0;
		stage.m_obbValue = 0;
	}

	if (!m_bTerrainMode) // if not terrain mode then no collision
		return;

	for (materialIndex=0;materialIndex<numMaterials;materialIndex++)
	{
		AttributeStage &materialStage = m_materialStages[materialIndex];

		// TODO use material stage stuff

		TupArray<MaterialImageRefsManager::MaterialImageData> imageRefsArray;
		materialImageRefsManager.BuildImageRefs(materialIndex,imageRefsArray);
		int numImages = imageRefsArray.GetSize();
		// start with the last image 
		bool bSetStage = false;
		int imageIndex;
		for (imageIndex=numImages-1;imageIndex>=0;imageIndex--)
		{
			MaterialImageRefsManager::MaterialImageData &imageData = imageRefsArray[imageIndex];
			int imageRef = imageData.m_imageRef;
			if (imageRef!=-1)
			{
				AttributeStage &imageStage = imageStages[imageRef];
				// if this image wants to set the collision
				if (!imageStage.m_bUseMesh)
				{
					materialStage = imageStage;
					bSetStage = true;
					break;
				}
			}
		}
		// if image stages did not set then try to set a default
		if (!bSetStage)
		{
			for (imageIndex=0;imageIndex<numImages;imageIndex++)
			{
				MaterialImageRefsManager::MaterialImageData &imageData = imageRefsArray[imageIndex];
				int imageRef = imageData.m_imageRef;
				if (imageRef!=-1)
				{
					AttributeStage &imageStage = imageStages[imageRef];
					// if this image wants to set the defaults
					if (imageStage.m_bDefaultCollision || imageStage.m_bDefaultDisabled)
					{
						materialStage = imageStage;
						bSetStage = true;
						break;
					}
				}
			}
		}
		// TODO use material stage default stuff
	}
}

void FaceCollisionManager::BuildImageAttributes(TupArray<FaceCollisionManager::AttributeStage> &imageStages)
{
	int numImages = m_tupImagePool.GetNumImages();
	imageStages.SetSize(numImages);
	int imageIndex;
	for (imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		AttributeStage &stage = imageStages[imageIndex];
		stage.m_bDefaultDisabled = false;
		stage.m_bDefaultCollision = false;
		stage.m_bDisabled = false;
		stage.m_bUseMesh = true;
		stage.m_surfaceType = 0;
		stage.m_obbValue = 0;
	}

	if (!m_bTerrainMode) // if not terrain mode then no collision
		return;

	// get names of surface types
	TupKeyValue pathToSurfaceTypes;
	pathToSurfaceTypes.AddKeyValue("CollisionType","TerrainOnly");
	TupKeySelection surfaceTypes;
	if (!m_tupNodeTreeRoot.GetKeySelections(pathToSurfaceTypes,"SurfaceType",surfaceTypes))
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_YELLOW|LogFlags::FONT_STYLE_BOLD,"Surface Types not found in Node Attribute Tree");
		return;
	}

	int numSurfaceTypes = surfaceTypes.GetNumSelections();
	if (numSurfaceTypes == 0)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_YELLOW|LogFlags::FONT_STYLE_BOLD,"No Surface types in list");
		return;
	}
	bool bSetStage = false;

	// TODO handle image attribute stuff

	for (imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		AttributeStage &stage = imageStages[imageIndex];
		TupperwareAggregate *pImageAgg = m_tupImagePool.GetImage(imageIndex);
		if (!pImageAgg)
			continue;

		TupImage tupImage(pImageAgg);
		char *pFilePathName;
		tupImage.GetFilePathName(&pFilePathName);
		TupString tupFileName(pFilePathName);
		int startPos = tupFileName.Find('[');
		if (startPos!=-1)
		{
			startPos++; // to next char
			int endPos = tupFileName.Find(']',startPos);
			if (endPos!=-1)
			{
				stage.m_bUseMesh = false; // this indicates we are doing something
				bSetStage = true;
				TupString typeString = tupFileName.Mid(startPos,endPos-startPos);
				for (int surfaceTypeIndex=0;surfaceTypeIndex<numSurfaceTypes;surfaceTypeIndex++)
				{
					const char *pSurfaceType = surfaceTypes.GetSelection(surfaceTypeIndex);
					if (!stricmp(pSurfaceType,typeString))
					{
						stage.m_surfaceType = surfaceTypeIndex; // set if string matches
						break;
					}
				}
			}
		}
	}
	// TODO add in the image attributes default stuff
}



