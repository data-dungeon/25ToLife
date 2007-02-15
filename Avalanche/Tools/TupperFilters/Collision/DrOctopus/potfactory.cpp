//***************************************************************************
// This class builds BSP trees for all the layers within a partition
//***************************************************************************
#include "Pch.h"

//***************************************************************************
//***************************************************************************
PartitionOctTreeFactory::PartitionOctTreeFactory(
PartitionCollisionInstanceList &instanceList,
const DrOctopusOptions &options) :
m_instanceList(instanceList),
m_options(options)
{
	// Clear all the bounding boxes to extremes so that the first triangle will be start of bounding
	for (int l = 0; l < MAX_COLLIDE_LAYERS; l++)
	{
		m_aabb[l].Set(Vector3(FLT_MAX,FLT_MAX,FLT_MAX),Vector3(FLT_MIN,FLT_MIN,FLT_MIN));
	}
	m_totalAABB.Set(Vector3(FLT_MAX,FLT_MAX,FLT_MAX),Vector3(FLT_MIN,FLT_MIN,FLT_MIN));

	// Build the soups
	BuildSoups();

	// Inflate the AABB's just a little
	m_totalAABB.Inflate(Vector3(1.0f, 1.0f, 1.0f));

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Bounding for partition %d, Min %f,%f,%f Max %f,%f,%f", m_instanceList.Partition(),
		m_totalAABB.minV.X(),m_totalAABB.minV.Y(),m_totalAABB.minV.Z(),
		m_totalAABB.maxV.X(),m_totalAABB.maxV.Y(),m_totalAABB.maxV.Z());

	// Now do the trees
	ProcessTrees(options);

	// Phew, it's Miller time
}

//***************************************************************************
//***************************************************************************
PartitionOctTreeFactory::~PartitionOctTreeFactory(void)
{
	for (int i = 0; i < MAX_COLLIDE_LAYERS; i++)
		delete m_tree[i];
}

//***************************************************************************
// Output the data
//***************************************************************************
void PartitionOctTreeFactory::OutputDatabase(
const char *filename)
{
	// Delete the old file
	remove(filename);

	// Setup a header
	CDBHeader hdr;
	memset(&hdr, '\0', sizeof(hdr));

	// Create the file
	FILE *fh = fopen(filename, "wb");
	if (!fh)
		throw(PluginException(ERROR_FATAL,"OutputCollisionDatabase - Cannot open file %s for writing", filename));

	// Write the header (just for space)
	if (fwrite(&hdr, sizeof(hdr), 1, fh) != 1)
		throw(PluginException(ERROR_FATAL,"OutputCollisionDatabase - Cannot write CDB header to %s", filename));

	// Write out all the databases
	int totalSize = 0;
	for (int i = 0; i < MAX_COLLIDE_LAYERS; i++)
	{
		if (m_tree[i])
		{
			// Write the database
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Writing Layer %d", i);
			int size = m_tree[i]->WriteDatabase(fh);

			// Make sure it is padded out
			int pad = Align_ModN(size, 32);
			WritePad(fh, pad);

			// increment total size
			hdr.layerSize[i] = size + pad;
			totalSize += hdr.layerSize[i];
		}
	}

	// Go back and fixup the header
	fseek(fh, 0, SEEK_SET);
	if (fwrite(&hdr, sizeof(hdr), 1, fh) != 1)
		throw(PluginException(ERROR_FATAL,"OutputCollisionDatabase - Cannot write header to %s", filename));

	// Done!
	fclose(fh);

	// No data?
	if (totalSize == 0)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_YELLOW|LogFlags::FONT_STYLE_BOLD,"Warning - No CDB Database for partition %d", m_instanceList.Partition());
		remove(filename);
	}
}

//***************************************************************************
//***************************************************************************
void PartitionOctTreeFactory::BuildSoups()
{
	// Alias the tupperware collision tree
	TupperwareCollisionTree &collisionTree = m_instanceList.CollisionTree();

	// Pump it full of data
	for (int i = 0; i < m_instanceList.Instances(); i++)
	{
		// Abort?
		if (PluginLog::GetAbortFlag())
			throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));

		// Get the collision instance
		CollisionInstance *instance = m_instanceList.GetInstance(i);
		assert(instance);

		// get base mesh (different than object if using derived objects)
		TupperwareAggregate *baseMeshAgg = AnnotateHelper::FindBaseMesh(*collisionTree.objectPool, instance->ObjectReference());
		TupTriMesh baseMesh(baseMeshAgg);
		int *faceFlags, numFaces;
		baseMesh.GetFaceFlags(&faceFlags, numFaces);

		// Get the face attribute array
		TupArray<FaceCollisionManager::Attributes> faceAttributeArray;
		TupArray<int> faceAttributeIndices;
		collisionTree.faceCollisionManager->BuildFaceCollisionAttributes(instance->Node(),
																								faceAttributeArray, faceAttributeIndices);

		// Access the vertex color stuff
		TupperwareAggregate *vertexColorMapAgg = AnnotateHelper::FindChildVertexColorMeshMap(*collisionTree.objectPool,
																															instance->ObjectReference());

		// Alias the soup
		assert(instance->Layer() >= 0 && instance->Layer() < MAX_COLLIDE_LAYERS);
		TriangleSoup &soup = m_triangleSoup[instance->Layer()];

		AABB &aabb = m_aabb[instance->Layer()];

		// Process the instance
	  	ProcessInstance(instance->Node(), baseMesh, vertexColorMapAgg,	faceAttributeArray, faceAttributeIndices, soup, aabb, m_totalAABB);
	}
}

//***************************************************************************
// Process an instance
//***************************************************************************
void PartitionOctTreeFactory::ProcessInstance(
TupNode &tupNode,
TupTriMesh &tupMesh,
TupperwareAggregate *vertexColorMapAgg,
TupArray<FaceCollisionManager::Attributes> &faceAttributeArray,
TupArray<int> &faceAttributeIndices,
TriangleSoup &soup,
AABB &layerAABB,
AABB &totalAABB)
{
	// Get the name, for some reason
	char *nodeName;
	tupNode.GetName(&nodeName);

	// Get the transform of this node
	TupperwareAggregate *transformAgg = tupNode.GetTransform();
	assert(transformAgg);
	TupTransform tupTransform(transformAgg);
	TupTransform::TransformTypes transformType = tupTransform.GetTransformType();
	assert(transformType == TupTransform::STATIC_MATRIX);
	TupStaticMatrix tupStaticMatrix(transformAgg);
	float *floatList;
	tupStaticMatrix.GetMatrix(&floatList);
	assert(floatList);
	Graphics4x4 &localToWorld = *(Graphics4x4 *)floatList;

	int numFaces;
	int numPositionVerts;
	Vector3 *positionVerts;
	int *positionIndices;
	tupMesh.GetPositionVerts((float **)&positionVerts, numPositionVerts);
	tupMesh.GetPositionIndices(&positionIndices, numFaces);

	// do normals
	int numNormalVerts;
	Vector3 *normalVerts;
	int *normalIndices;
	tupMesh.GetNormalVerts((float **)&normalVerts, numNormalVerts);
	tupMesh.GetNormalIndices(&normalIndices, numFaces);

	ts_bRGBA *colorArray = NULL;
	int *colorIndices = NULL;
	int numColorFaces;
	AnnotateHelper::BuildVertexColors(vertexColorMapAgg, (AnnotateHelper::Color **)&colorArray, &colorIndices, numColorFaces);

	int numAttributeFaces = faceAttributeIndices.GetSize();

	// if there is some collision within the mesh
	if (numAttributeFaces != 1 || faceAttributeArray[0].m_bCollision)
	{
		// Do the polys
		for (int faceIndex=0, faceOffset=0; faceIndex < numFaces; faceIndex++, faceOffset += 3)
		{
			int attributeIndex = (faceIndex < numAttributeFaces) ? faceIndex : 0;

			// check if this face has collision
			if (faceAttributeArray[faceAttributeIndices[attributeIndex]].m_bCollision)
			{
				// Transform it
				Vector3 mv[3];
				Vector3 v[3];
				Vector3 n[3];
				for (int pointIndex=0; pointIndex < 3; pointIndex++)
				{
					mv[pointIndex] = positionVerts[positionIndices[faceOffset + pointIndex]];
					v[pointIndex] = mv[pointIndex] * localToWorld;
					n[pointIndex] = normalVerts[normalIndices[faceOffset + pointIndex]].BasisTransform(localToWorld);
					layerAABB.ExpandByPoint(v[pointIndex]);
					totalAABB.ExpandByPoint(v[pointIndex]);
				}

				// Build the color list
				int colorIndex = (faceIndex < numColorFaces) ? faceOffset : 0;
				ts_bRGBA color[3];
				color[0] = colorArray[colorIndices[colorIndex]];
				color[1] = colorArray[colorIndices[colorIndex + 1]];
				color[2] = colorArray[colorIndices[colorIndex + 2]];

				// Add the triangle to the soup
				SoupTriangle *tri = soup.Add(v, n, color, faceAttributeArray[faceAttributeIndices[attributeIndex]].m_surfaceType, m_options.rewindTriangles);

				// Update global stats
				if (tri)
					++g_stats.triangles;
				else
					++g_stats.degenerates;
			}
		}
	}

	delete [] colorArray;
	delete [] colorIndices;
}

//***************************************************************************
//***************************************************************************
void PartitionOctTreeFactory::ProcessTrees(
const DrOctopusOptions &options)

{
 	// Create the PartitionOctTreeFactory tc's
	for (int l = 0; l < MAX_COLLIDE_LAYERS; l++)
	{
		if (m_triangleSoup[l].Triangles() == 0)
			m_tree[l] = NULL;
		else
		{
			// Inflate the AABB's just a little
			m_aabb[l].Inflate(Vector3(1.0f, 1.0f, 1.0f));

			// Create the PartitionOctTreeFactory
			m_tree[l] = new FactoryOctTree(m_triangleSoup[l], m_aabb[l], options);
			assert(m_tree[l]);
		}
	}
}
