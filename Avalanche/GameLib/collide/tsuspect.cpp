/***************************************************************************/
// Triangle suspect support
/***************************************************************************/
#include "collide/CollidePCH.h"
#include "collide/tsuspect.h"
#include "cmdcon/cmdcon.h"

/***************************************************************************/
/***************************************************************************/
TriangleSuspect::TriangleSuspect()
{
	triangle = NULL;
	triangles = 0;
}

/***************************************************************************/
/***************************************************************************/
void TriangleSuspect::Initialize(
TerrainTriangle *i_triList, 
int i_triangles)
{
	triangle = i_triList;
	triangles = i_triangles;
}

/***************************************************************************/
// Validate the cluster
/***************************************************************************/
void TriangleSuspect::Validate(STriangle* o_suspectList,
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

		Vector3 edge1 = triangle[i].B() - triangle[i].A();
		Vector3 edge2 = triangle[i].C() - triangle[i].A();
		if (triangle[i].FaceNormal().Dot(edge1.Cross(edge2)) < 0.0f)
			reason |= SUSPECT_RIGHT_HANDED;

		if (!Math::Equal(triangle[i].FaceNormal().Length(), 1.0f))
			reason |= SUSPECT_UNIT_NORMAL;

		reason |= TestNeighbor(i, triangle[i].A(), triangle[i].B()) <<
				NEIGHBOR1_SHIFT;

		reason |= TestNeighbor(i, triangle[i].B(), triangle[i].C()) <<
				NEIGHBOR2_SHIFT;

		reason |= TestNeighbor(i, triangle[i].C(), triangle[i].A()) <<
				NEIGHBOR3_SHIFT;

		if ((reason != 0x0) && (io_numSuspects < i_maxSuspects))
		{
			o_suspectList[io_numSuspects].d_triangle = &triangle[i];
			o_suspectList[io_numSuspects].d_reason = reason;
			io_numSuspects++;
		}
	}
}

/***************************************************************************/
// Find a triangle that contains a specific edge
/***************************************************************************/
u32 TriangleSuspect::TestNeighbor(u32 i_tester, const Vector3 &i_v1,
		const Vector3 &i_v2)
{
	u32 reason = 0x0;
	bool foundNeighbor = false;

	for (u32 i = 0; i < triangles; i++)
	{
		if (i == i_tester)
			continue;

		bool aV1Equal = Equal(triangle[i].A(), i_v1);
		bool bV1Equal = Equal(triangle[i].B(), i_v1);
		bool cV1Equal = Equal(triangle[i].C(), i_v1);

		bool aV2Equal = Equal(triangle[i].A(), i_v2);
		bool bV2Equal = Equal(triangle[i].B(), i_v2);
		bool cV2Equal = Equal(triangle[i].C(), i_v2);

		if ((aV1Equal && bV2Equal) || (bV1Equal && aV2Equal) || (bV1Equal && cV2Equal) ||
				(cV1Equal && bV2Equal) || (cV1Equal && aV2Equal) || (aV1Equal && cV2Equal))
		{
			if (triangle[i_tester].FaceNormal().Dot(triangle[i].FaceNormal()) < -0.8f)
				reason |= SUSPECT_CORNER;
			if (foundNeighbor)
				reason |= SUSPECT_MULTI;
			foundNeighbor = true;
		}
	}

	return reason;
}

