////////////////////////////////////////////////////////////////////////////
//
// VertexAlpha
//
// Class to handle vertex alpha issues with multi texturing
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "MultiTexPch.h"
#include "VertexAlpha.h"

VertexAlphaManager::VertexAlphaManager(TupImagePool &tupImagePool,ImageManager &imageManager,TupTriMesh &tupTriMesh) :
	m_tupImagePool(tupImagePool),m_imageManager(imageManager),m_tupTriMesh(tupTriMesh)
{
	// set up the mesh alpha array to be 3 floats per face (one for each point)
	tupTriMesh.GetFaceMaterialIndices(&m_pFaceMaterialIndices,m_numFaces);
	m_numFacePoints = (m_numFaces<<1)+m_numFaces;
	m_meshAlphaArray.SetSize(m_numFacePoints);
	for (int facePointIndex=0;facePointIndex<m_numFacePoints;facePointIndex++)
	{
		m_meshAlphaArray[facePointIndex] = 1.0f; // initialize to opaque
	}
}

void VertexAlphaManager::ApplyVertexAlpha(const TupArray<MaterialRefAlpha> &materialRefAlphaArray)
{
	int numMaterialRefEntries = materialRefAlphaArray.GetSize();
	// goes through each entry that deals with a specific material index
	for (int materialRefEntryIndex=0;materialRefEntryIndex<numMaterialRefEntries;materialRefEntryIndex++)
	{
		const MaterialRefAlpha &materialRefEntry = materialRefAlphaArray[materialRefEntryIndex];
		int numApplyAlphaEntries = materialRefEntry.m_alphaEntries.GetSize();
		// goes through each entry that deals with an application of alpha to the faces
		for (int applyAlphaEntryIndex = 0;applyAlphaEntryIndex<numApplyAlphaEntries;applyAlphaEntryIndex++)
		{
			int facePointIndex;
			const ApplyAlphaEntry &applyAlphaEntry = materialRefEntry.m_alphaEntries[applyAlphaEntryIndex];
			// apply the constant alpha if needed
			if (applyAlphaEntry.m_constantAlpha!=1.0f)
			{
				int faceIndex;
				for (faceIndex=0,facePointIndex=0;faceIndex<m_numFaces;faceIndex++,facePointIndex+=3)
				{
					// make sure we are using the desired material
					if (m_pFaceMaterialIndices[faceIndex] == materialRefEntry.m_faceMaterialIndex)
					{
						for (int pointIndex=0;pointIndex<3;pointIndex++)
						{
							// multiply by alpha
							m_meshAlphaArray[facePointIndex+pointIndex] *= applyAlphaEntry.m_constantAlpha; 
						}
					}
				}
			}
			// if there is an image to deal with
			if (applyAlphaEntry.m_uvTransform.m_imageRef!=-1)
			{
				int totalImages = m_tupImagePool.GetNumImages();
				assert(applyAlphaEntry.m_uvTransform.m_imageRef<totalImages);
				TupperwareAggregate *pImageAgg = m_tupImagePool.GetImage(applyAlphaEntry.m_uvTransform.m_imageRef);
				TupImage tupImage(pImageAgg);
				char *pImageFileName;
				// get filename for image
				tupImage.GetFilePathName(&pImageFileName);
				// get image data from image manager
				TupperwareAggregate *pImageDataAgg = m_imageManager.GetImageData(pImageFileName);
				assert(pImageDataAgg);
				// get the first frame from the image data
				TupImageData tupImageData(pImageDataAgg);
				int numFrames = tupImageData.GetNumImageDataFrames();
				assert(numFrames>0);
				TupperwareAggregate *pImageDataFrame = tupImageData.GetImageDataFrame(0); // first frame only
				assert(pImageDataFrame);
				TupImageDataFrame tupImageDataFrame(pImageDataFrame);
				bool bClampU = (applyAlphaEntry.m_uvTransform.m_flags & UVTransform::FLAGS_U_WRAP) ? false : true;
				bool bClampV = (applyAlphaEntry.m_uvTransform.m_flags & UVTransform::FLAGS_V_WRAP) ? false : true;

				int *pUVIndices = NULL;
				int tempValue;
				Vector3 *pUVVerts = NULL;
				int uvVerts;

				// set up the uv channel pointers
				for (int meshMapIndex = 0; meshMapIndex < m_tupTriMesh.GetNumMeshMaps(); meshMapIndex++ )
				{
					TupTriMeshMap  tupTriMeshMap( m_tupTriMesh.GetMeshMap( meshMapIndex ) );
					if ( tupTriMeshMap.GetMapChannel() == applyAlphaEntry.m_uvTransform.m_mapChannel)
					{
						tupTriMeshMap.GetMapVerts((float **) &pUVVerts, uvVerts );
						tupTriMeshMap.GetMapIndices( &pUVIndices, tempValue );
						break;
					}
				}

				int faceIndex;
				for (faceIndex=0,facePointIndex=0;faceIndex<m_numFaces;faceIndex++,facePointIndex+=3)
				{
					// make sure we are using the desired material
					if (m_pFaceMaterialIndices[faceIndex] == materialRefEntry.m_faceMaterialIndex)
					{
						for (int pointIndex=0;pointIndex<3;pointIndex++)
						{
							// multiply by alpha
							Vector3 tempVert;
							if (pUVVerts)
							{
								int vertexIndex = pUVIndices[facePointIndex+pointIndex];
								tempVert = pUVVerts[vertexIndex];
							}
							else
							{
								tempVert.Set(0.0f,0.0f,0.0f); // if channel does not exist then use 0
							}
							ApplyUVTransform::Apply(applyAlphaEntry.m_uvTransform,tempVert);
							u32 imageColor = ImageUtilities::GetPixelColor(tupImageDataFrame,0,tempVert.x( ),tempVert.y( ),bClampU,bClampV);
							float imageAlpha;
							if (applyAlphaEntry.m_uvTransform.m_flags & UVTransform::FLAGS_USE_IMAGE_ALPHA)
							{
								// grab alpha component
								imageAlpha = ((float)((imageColor>>24)&0xff))/255.0f;
							}
							else
							{
								// get intensity of the rgb portion
								u32 imageRed = imageColor&0xff;
								u32 imageGreen = (imageColor>>8)&0xff;
								u32 imageBlue = (imageColor>>16)&0xff;

								imageAlpha = ((float)(imageRed+imageGreen+imageBlue))/(255.0f + 255.0f + 255.0f);
							}
							// if the alpha needs to be inverted
							if (applyAlphaEntry.m_uvTransform.m_flags & UVTransform::FLAGS_INVERT_ALPHA)
							{
								imageAlpha = 1.0f-imageAlpha;
							}
							m_meshAlphaArray[facePointIndex+pointIndex] *= imageAlpha; 
						}
					}
				}
			}
		}
	}
}

void VertexAlphaManager::AddAlphaToMesh(void)
{
	// keeper for keepering track the unique alpha values
	Keeper<float> newVertexAlphaVerts;
	// make the first one opaque
	float opaqueAlpha = 1.0f;
	newVertexAlphaVerts.Add(opaqueAlpha);

	int totalFacePoints = m_meshAlphaArray.GetSize();
	int facePointIndex;
	TupArray<int> newVertexAlphaIndices;
	newVertexAlphaIndices.SetSize(totalFacePoints);
	for (facePointIndex=0;facePointIndex<totalFacePoints;facePointIndex++)
	{
		newVertexAlphaIndices[facePointIndex] = newVertexAlphaVerts.Add(m_meshAlphaArray[facePointIndex]);
	}
	m_tupTriMesh.GetBaseAggregate()->AddListFloat("MultiTex_VertexAlphaVerts",NULL,newVertexAlphaVerts.GetArray());
	m_tupTriMesh.GetBaseAggregate()->AddListInt("MultiTex_VertexAlphaIndices",NULL,newVertexAlphaIndices);
}
