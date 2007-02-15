////////////////////////////////////////////////////////////////////////////
//
// qtfact
//
// A class for creating a loadable quadtree of obb's
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: qtfact.cpp $
 * 
 * *****************  Version 46  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:44a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 45  *****************
 * User: Bcollard     Date: 7/11/03    Time: 1:34p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Remove NEW_COLLISIONS #ifdef's.
 * 
 * *****************  Version 44  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:55p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * 
 * *****************  Version 43  *****************
 * User: Bcollard     Date: 3/28/03    Time: 2:44p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Integrated the Phase II collision system changes but enclosed them in
 * #ifdef's.
 * 
 * *****************  Version 42  *****************
 * User: Food         Date: 3/12/03    Time: 12:32p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * 
 * *****************  Version 41  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 11:48a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * update for streaming system
 * 
 * *****************  Version 40  *****************
 * User: Bcollard     Date: 3/05/03    Time: 3:52p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Start to use new collision interface.
 * 
 * *****************  Version 39  *****************
 * User: Food         Date: 1/14/03    Time: 4:04p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 *
 * *****************  Version 38  *****************
 * User: Adam Clayton Date: 10/11/02   Time: 2:51p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * output obbs generated in log
 *
 * *****************  Version 37  *****************
 * User: Adam Clayton Date: 10/04/02   Time: 8:23a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * inflate rect fixes
 *
 * *****************  Version 36  *****************
 * User: Adam Clayton Date: 9/25/02    Time: 3:35p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * collision vs render instances
 *
 * *****************  Version 35  *****************
 * User: Adam Clayton Date: 9/25/02    Time: 11:33a
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * new collision attribute system (using image names)
 *
 * *****************  Version 34  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 4:29p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Added support for attrib tree
 *
 * *****************  Version 33  *****************
 * User: Adam Clayton Date: 3/25/02    Time: 12:41p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * hybrid configuration
 *
 * *****************  Version 32  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 3:53p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * First Pass of Attributes
 *
 * *****************  Version 31  *****************
 * User: Adam Clayton Date: 3/18/02    Time: 12:33p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Abort checks within system
 *
 * *****************  Version 30  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 3:06p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * Memory Problems.
 * Simple Progress Bar Stuff
 *
 * *****************  Version 29  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 1:32p
 * Updated in $/Avalanche/tools/TupperFilters/OBBWorld
 * OBBWorld is now online... Just needs to get the node and face
 * attributes
 */

#include "Pch.h"

#include "qtfact.h"
#include "qtfactn.h"
#include "makeobb.h"
#include <collide\objectlayer.h>
#include <toolhelper\writepad.h>

/***************************************************************************/
/***************************************************************************/
OBBQuadTreeFactory::OBBQuadTreeFactory(
PartitionCollisionInstanceList &instanceList,
float minQuadrantSize,
const Vector3 &shrinkBy,
float minDimension) :
m_instanceList(instanceList)
{
	// pass on the min quadrant size
	OBBQuadTreeFactoryNode::SetMinQuadrantSize(minQuadrantSize);

	// Figure out the extents
	XZRect extents;
	extents.Clear();
	for (int i = 0; i < m_instanceList.Instances(); i++)
	{
		CollisionInstance *instance = m_instanceList.GetInstance(i);
		assert(instance);

		extents += instance->Rect();
	}

	// Alias the tupperware collision tree
	TupperwareCollisionTree &collisionTree = m_instanceList.CollisionTree();

	// Create the roots
	for (int layer = 0; layer < MAX_COLLISION_LAYERS; layer++)
	{
		// Anything to do with this layer?
		if (m_instanceList.InstancesInLayer(layer) == 0)
			m_pRoot[layer] = NULL;
		else
		{
			m_pRoot[layer] = OBBQuadTreeFactoryNode::CreateRoot(extents);
			assert(m_pRoot[layer]);
		}
	}

	// Process the data
	PluginLog::SetProgressText("Processing Instances");
	int totalOBBs = 0;
	for (i = 0; i < instanceList.Instances(); i++)
	{
		if (PluginLog::GetAbortFlag())
			throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));

		// Get the collision instance
		CollisionInstance *instance = m_instanceList.GetInstance(i);
		assert(instance);

		// Get the instance layer
		int layer = instance->Layer();

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


		PluginLog::SetProgressBar(0, instanceList.Instances() - 1, i);

		// Make an obb from the instance
		MakeOBB makeOBB(instance->Node(), baseMesh, vertexColorMapAgg, faceAttributeArray, faceAttributeIndices);
		int numObbs = makeOBB.Build(&shrinkBy, minDimension);

//		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Processing Instance %d of %d - %s - %d OBBS %d Triangles",
//																	i, instanceList.Instances(), instance->Name(), numObbs, numFaces);
		totalOBBs += numObbs;

		OBBStaticInstance *obb;
		while ((obb = makeOBB.Get()) != NULL)
		{
			// Make the obb rect for the obb
			XZRect obbRect;
			obb->ComputeWorldExtents(obbRect);

			// Add it into the tree
			assert(m_pRoot[layer]);
			m_pRoot[layer]->Add(obb, obbRect);
		}

		// Update the number of root obbs
		uint oldrootobbs = g_stats->rootObbs;
		g_stats->rootObbs = m_pRoot[layer]->OBBs();
		if (oldrootobbs != g_stats->rootObbs)
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Instance %s lies in root node", instance->Name());
	}

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Processed %d OBBS", totalOBBs);
}

/***************************************************************************/
/***************************************************************************/
OBBQuadTreeFactory::~OBBQuadTreeFactory(void)
{
	for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
		delete m_pRoot[i];
}

/***************************************************************************/
// Write the quad tree out
/***************************************************************************/
void OBBQuadTreeFactory::OutputOBBDatabase(const char *filename)
{
	// First get rid of the original file
	remove(filename);

	// Lets mock up a header
	ODBHeader hdr;
	memset(&hdr, '\0', sizeof(hdr));
	hdr.version = CURRENT_ODB_VERSION;

	// How much data is there to write?
	int pad[MAX_COLLISION_LAYERS];
	int totalSize = 0;
	for (int layer = 0; layer < MAX_COLLISION_LAYERS; layer++)
	{
		if (m_pRoot[layer])
		{
			int size = m_pRoot[layer]->DiskSize();
			pad[layer] = Align_ModN(size, 32);

			hdr.layerSize[layer] = size + pad[layer];
			totalSize += hdr.layerSize[layer];
		}
	}

	// No data?
	if (totalSize == 0)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_YELLOW|LogFlags::FONT_STYLE_BOLD,"Warning - No OBB Database to write for partition %d", m_instanceList.Partition());
		return;
	}

	// Create the file
	FILE *fh = fopen(filename, "wb");
	if (!fh)
		throw(PluginException(ERROR_FATAL,"OBBQuadTreeFactory - OutputOBBDatabase - Error Writing File %s", filename));

	// Write the header
	if (fwrite(&hdr, sizeof(hdr), 1, fh) != 1)
		throw(PluginException(ERROR_FATAL,"OBBQuadTreeFactory - OutputOBBDatabase - Error Writing Header %s", filename));

	// Write out all the nodes
	for (layer = 0; layer < MAX_COLLISION_LAYERS; layer++)
	{
		try
		{
			if (m_pRoot[layer])
			{
				m_pRoot[layer]->Write(fh);
				WritePad(fh, pad[layer]);
			}
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"OBBQuadTreeFactory - OutputOBBDatabase\r\n  %s", (const char *)(e)));
		}
	}

	// Done!
	fclose(fh);
}

/***************************************************************************/
// Do a temp report
/***************************************************************************/
void OBBQuadTreeFactory::OutputVisData(const char *filename)
{
	// what res do we generate at?
#ifdef _DEBUG
	const uint res = 1024 * 2;
#else
	const uint res = 1024;
#endif

	// Loop through all the roots
	for (int layer = 0; layer < MAX_COLLISION_LAYERS; layer++)
	{
		// Make the proper filename
		char realFilename[256];
		LayerizeFilename(filename, layer, realFilename);
		remove(realFilename);

		// Anything to do?
		if (!m_pRoot[layer])
			continue;

		// Create it
		VisTool vis(res, res, VisWHITE);

		// Setup the coordinate system (use half the parent rect)
		XZRect c = m_pRoot[layer]->Rect();
#define LOOSEQUADTREE
#ifdef LOOSEQUADTREE
		c.Inflate(c.Width() * -0.25f, c.Height() * -0.25f);
#endif
		vis.SetCoordinateSystem(c);

		// Draw all the instances
		for (int i = 0; i < m_instanceList.Instances(); i++)
		{
			// Get the collision instance
			CollisionInstance *instance = m_instanceList.GetInstance(i);
			assert(instance);

			if (instance->Layer() != layer)
				continue;

			// Build a rect
			const Vector3 &boundingBoxMin = instance->BoundingBoxMin();
			const Vector3 &boundingBoxMax = instance->BoundingBoxMax();
			XZRect rect(boundingBoxMin.x( ), boundingBoxMin.z( ), boundingBoxMax.x( ), boundingBoxMax.z( ) );

			// Draw the bb
			vis.Plot(rect, VisRED);
		}

	 	// Draw all the obbs
		m_pRoot[layer]->Report(vis);

		bool result = vis.Save(realFilename);
		if (!result)
			throw(PluginException(ERROR_FATAL,"OBBQuadTreeFactory - Visualize saving file %s", realFilename));
	}
}

/***************************************************************************/
// Convert a normal file name to have a layer prefix (0 does nothing)
// Returns a pointer to realFileName.
/***************************************************************************/
char *OBBQuadTreeFactory::LayerizeFilename(
const char *filename,
int layer,
char *realFilename)
{
	// Special case, keep layer 0 like the old way
	if (layer == 0)
	{
		strcpy(realFilename, filename);
		return(realFilename);
	}

	// save the start of string
	char *ret = realFilename;

	// Copy up until the dot
	while (*filename && *filename != '.')
		*realFilename++ = *filename++;

	// Insert the layer
	*realFilename++ = '_';
	if (layer > 0 && layer <= 9)
		*realFilename++ = '0' + layer;
	else
		*realFilename++ = 'A' + layer;

	// Finish the copy
	while (*filename)
		*realFilename++ = *filename++;
	*realFilename = '\0';

	return(ret);
}


