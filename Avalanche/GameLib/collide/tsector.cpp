/***************************************************************************/
// A terrain sector contains all the vertex and connectivity data for a
// sector of the terrain
/***************************************************************************/

/******************* includes ****************************/

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/tsector.h"

// Gamelib includes //
#include "collide/tcluster.h"

/***************************************************************************/
/***************************************************************************/
void TerrainSector::GetVerticalExtent(TriangleCluster* i_cluster,
		float &o_min, float &o_max) const
{
	if (triangles == 0)
		return;

	// Alias this
	Vector3Packed *vertex = i_cluster->VertexArray();

	o_min = i_cluster->Triangle(triangleIndex[0]).V1(vertex).y();
	o_max = o_min;
	for (uint t = 0; t < triangles; t++)
	{
		// alias the triangle
		TerrainTriangle &triangle = i_cluster->Triangle(triangleIndex[t]);

		float vertexY = triangle.V1(vertex).y();
		if (vertexY < o_min)
			o_min = vertexY;
		else if (vertexY > o_max)
			o_max = vertexY;

		vertexY = triangle.V2(vertex).y();
		if (vertexY < o_min)
			o_min = vertexY;
		else if (vertexY > o_max)
			o_max = vertexY;

		vertexY = triangle.V3(vertex).y();
		if (vertexY < o_min)
			o_min = vertexY;
		else if (vertexY > o_max)
			o_max = vertexY;
	}
}

