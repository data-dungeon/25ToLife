/***************************************************************************/
// Triangle cluster
/***************************************************************************/

/******************* includes ****************************/

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/tcluster.h"

/***************************************************************************/
// Initialize (we are loaded from disk, so we have to fixup pointers)
/***************************************************************************/
void TriangleCluster::DiskFixup(void)
{
	uint baseAddr = (uint)this;

#ifdef GCN
	EndianSwap( );
#endif

	// None of these should be NULL (they all point to the end of the structure
	// if there is no data
	ASSERT(vertex != NULL);
	ASSERT(triangle != NULL);
#ifdef WANTTERRAINEDGES
	ASSERT(edge != NULL);
#endif

	vertex = (Vector3Packed *)(baseAddr + (uint)vertex);
	triangle = (TerrainTriangle *)(baseAddr + (uint)triangle);
#ifdef WANTTERRAINEDGES
	edge = (TerrainEdge *)(baseAddr + (uint)edge);
#endif

#ifdef GCN
	u32		iIndex;

	for( iIndex = 0; iIndex < vertices; iIndex++)
	{
		vertex[iIndex].EndianSwap( );
	}
	for( iIndex = 0; iIndex < triangles; iIndex++)
	{
		triangle[iIndex].EndianSwap( );
	}
#ifdef WANTTERRAINEDGES
	for( iIndex = 0; iIndex < edges; iIndex++)
	{
		edge[iIndex].EndianSwap( );
	}
#endif
#endif
}

/***************************************************************************/
// Reset the test counter in all the triangles to 0
/***************************************************************************/
void TriangleCluster::ResetTestID(void)
{
	for (uint t = 0; t < triangles; t++)
		triangle[t].ResetTestID();
}

/***************************************************************************/
// Validate the cluster
/***************************************************************************/
#include "cmdcon/cmdcon.h"
void TriangleCluster::Validate(SSuspectTriangle* o_suspectList,
		u32 &io_numSuspects, u32 i_maxSuspects)
{
	for (u32 i = 0; i < triangles; i++)
	{
		if ((i % 1000) == 0)
		{
			g_console.Echo("Testing triangle %d of %d triangles (%d suspects).\n",
					i, triangles, io_numSuspects);
		}

		u32 reason = 0x0;

		Vector3 edge1 = triangle[i].V2(vertex) - triangle[i].V1(vertex);
		Vector3 edge2 = triangle[i].V3(vertex) - triangle[i].V1(vertex);
		if (triangle[i].FaceNormal().Dot(edge1.Cross(edge2)) < 0.0f)
			reason |= SUSPECT_RIGHT_HANDED;

		if (!Math::Equal(triangle[i].FaceNormal().Length(), 1.0f))
			reason |= SUSPECT_UNIT_NORMAL;

		reason |= TestNeighbor(i, triangle[i].V1(), triangle[i].V2()) <<
				NEIGHBOR1_SHIFT;

		reason |= TestNeighbor(i, triangle[i].V2(), triangle[i].V3()) <<
				NEIGHBOR2_SHIFT;

		reason |= TestNeighbor(i, triangle[i].V3(), triangle[i].V1()) <<
				NEIGHBOR3_SHIFT;

		if ((reason != 0x0) && (io_numSuspects < i_maxSuspects))
		{
			o_suspectList[io_numSuspects].d_triangle = &triangle[i];
			o_suspectList[io_numSuspects].d_vertexTable = vertex;
			o_suspectList[io_numSuspects].d_reason = reason;
			io_numSuspects++;
		}
	}
}

/***************************************************************************/
// Find a triangle that contains a specific edge
/***************************************************************************/
u32 TriangleCluster::TestNeighbor(u32 i_tester, u32 i_v1, u32 i_v2)
{
	u32 reason = 0x0;
	bool foundNeighbor = false;

	for (u32 i = 0; i < triangles; i++)
	{
		if (i == i_tester)
			continue;
		if (((triangle[i].V1() == i_v1) && (triangle[i].V2() == i_v2)) ||
				((triangle[i].V2() == i_v1) && (triangle[i].V1() == i_v2)) ||
				((triangle[i].V2() == i_v1) && (triangle[i].V3() == i_v2)) ||
				((triangle[i].V3() == i_v1) && (triangle[i].V2() == i_v2)) ||
				((triangle[i].V3() == i_v1) && (triangle[i].V1() == i_v2)) ||
				((triangle[i].V1() == i_v1) && (triangle[i].V3() == i_v2)))
		{
			if (triangle[i_tester].FaceNormal().Dot(triangle[i].FaceNormal()) <
					-0.8f)
				reason |= SUSPECT_CORNER;
			if (foundNeighbor)
				reason |= SUSPECT_MULTI;
			foundNeighbor = true;
		}
	}
	return(reason);
}
