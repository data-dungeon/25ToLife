/*
**
**  File:   ObbToTerrain.cpp
**  Date:   February 11, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between an OBB and terrain.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/ObbToTerrain.cpp $
**  $Revision: 23 $
**      $Date: 8/01/03 5:28p $
**    $Author: Bcollard $
**
*/

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/ObbToTerrain.h"

#if 0

// Engine include //
#include "Math/Vector.h"

// Gamelib includes //
#include "collide/StackArray.h"
#include "geomUtil/planefit.h"

#endif

// Valid data masks.
const uint CObbToTerrainIntersection::INTERSECTION_TEST     = 0x01;
const uint CObbToTerrainIntersection::TRIANGLE_NORMAL_SUM   = 0x02;
const uint CObbToTerrainIntersection::INTERSECTION_PLANE    = 0x04;
const uint CObbToTerrainIntersection::TRIANGLE_INTERSECTION = 0x08;
const uint CObbToTerrainIntersection::TRIANGLE_VERTEX       = 0x10;

/***************************************************************************/
/***************************************************************************/
CObbToTerrainIntersection::~CObbToTerrainIntersection()
{
	CCollisionStackItem::Delete(d_triangleVertex);
	CCollisionStackItem::Delete(d_triangleIntersection);
}

/***************************************************************************/
/***************************************************************************/
void CObbToTerrainIntersection::PerformIntersectionTest(void)
{
	d_validFlags |= INTERSECTION_TEST;

	CCollisionStackItem::Delete(d_triangleVertex);
	CCollisionStackItem::Delete(d_triangleIntersection);
	d_triangleIntersection = NULL;
	d_triangleVertex = NULL;

	d_iterator.Bounds() = d_obb;
	if (d_triangleList.Open(d_stack) && d_iterator.Initialize(d_terrain))
	{
		TerrainTriangle* triangle;
		Vector3 normalInObb, vertexAInObb, vertexBInObb, vertexCInObb;
		while ((triangle = d_iterator.GetNextTriangle()) != NULL)
		{
			if (d_obb.TestTriangle(triangle->FaceNormal(), triangle->A(),
					triangle->B(), triangle->C(), normalInObb, vertexAInObb,
					vertexBInObb, vertexCInObb))
			{
				d_triangleList.Add(triangle, normalInObb, vertexAInObb,
						vertexBInObb, vertexCInObb);
			}
		}
	}
	d_triangleList.Close();

	int count = d_triangleList.NumTriangles();
	if (count > 0)
	{
		d_intersects = true;
		count *= 3;

		d_triangleIntersection = CCollisionStackArray<Vector3>::New(d_stack);
		if (d_triangleIntersection != NULL)
			d_triangleIntersection->AllocateArray(2 * count);

		d_triangleVertex = CCollisionStackArray<Vector3>::New(d_stack);
		if (d_triangleVertex != NULL)
			d_triangleVertex->AllocateArray(count);
	}
	else
		d_intersects = false;

	ASSERT(!d_stack->IsBottomLocked());
}

/***************************************************************************/
/***************************************************************************/
Vector3CRef CObbToTerrainIntersection::TriangleNormalSum(void)
{
	if ((d_validFlags & TRIANGLE_NORMAL_SUM) == 0)
	{
		if ((d_validFlags & INTERSECTION_TEST) == 0)
			PerformIntersectionTest();
		PerformTriangleNormalSum();
	}

	return(d_normalSum);
}

/***************************************************************************/
/***************************************************************************/
void CObbToTerrainIntersection::PerformTriangleNormalSum(void)
{
	d_validFlags |= TRIANGLE_NORMAL_SUM;

	d_normalSum.Clear();
	int numEntries = d_triangleList.NumTriangles();
	for (int i = 0; i < numEntries; i++)
	{
		d_normalSum += d_triangleList.UnsafeGetTriangle(i)->FaceNormal();
	}
}

/***************************************************************************/
// Get the sum of the triangle normals that point in a general direction.
// To get expected results, i_direction should be a unit vector or
// i_scaledCosAngle should be the cosine of the allowed angle between the
// normal and i_direction scaled by the magnitude of i_direction.
/***************************************************************************/
Vector3 CObbToTerrainIntersection::TriangleNormalSum(Vector3CRef i_direction,
		float i_scaledCosAngle)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();

	Vector3 normalSum(0.0f, 0.0f, 0.0f);
	int numEntries = d_triangleList.NumTriangles();
	for (int i = 0; i < numEntries; i++)
	{
		TerrainTriangle* triangle = d_triangleList.UnsafeGetTriangle(i);
		if (i_direction.Dot(triangle->FaceNormal()) >= i_scaledCosAngle)
			normalSum += triangle->FaceNormal();
	}

	return(normalSum);
}

/***************************************************************************/
/***************************************************************************/
void CObbToTerrainIntersection::AddObbEdgeIntersections(CPlaneFit &io_planeFit)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();

	PerformAddObbEdgeIntersections(io_planeFit);
}

/***************************************************************************/
/***************************************************************************/
void CObbToTerrainIntersection::PerformAddObbEdgeIntersections(
		CPlaneFit &io_planeFit)
{
	for (int i = 0; i < d_triangleList.NumTriangles(); i++)
	{
		STriangleEntry* info = d_triangleList.UnsafeGetTriangleEntry(i);
		OBB::SObbPlaneIntersections intersections;
		if (d_obb.ComputeObbPlaneIntersections(info->d_transformedVertex[0],
				info->d_transformedNormal, intersections))
		{
			Vector3 absNormal = Vector3::Abs(info->d_transformedNormal);

			if ((absNormal.x() > absNormal.y()) && (absNormal.x() > absNormal.z()))
			{
				for (int j = 0; j < OBB::NUM_PATH; j++)
				{
					if (((intersections.d_master & (1 << j)) != 0) &&
							TriangleTest::PointInsideYZ(
							intersections.d_pointInObb[j], info->d_transformedVertex[0],
							info->d_transformedVertex[1], info->d_transformedVertex[2]))
					{
						io_planeFit.AddTransformPoint(intersections.d_pointInObb[j] *
								(*(d_obb.Matrix())) + (*d_obb.Center()));
					}
				}
			}
			else if ((absNormal.y() > absNormal.x()) && (absNormal.y() > absNormal.z()))
			{
				for (int j = 0; j < OBB::NUM_PATH; j++)
				{
					if (((intersections.d_master & (1 << j)) != 0) &&
							TriangleTest::PointInsideXZ(
							intersections.d_pointInObb[j], info->d_transformedVertex[0],
							info->d_transformedVertex[1], info->d_transformedVertex[2]))
					{
						io_planeFit.AddTransformPoint(intersections.d_pointInObb[j] *
								(*(d_obb.Matrix())) + (*d_obb.Center()));
					}
				}
			}
			else
			{
				for (int j = 0; j < OBB::NUM_PATH; j++)
				{
					if (((intersections.d_master & (1 << j)) != 0) &&
							TriangleTest::PointInsideXY(
							intersections.d_pointInObb[j], info->d_transformedVertex[0],
							info->d_transformedVertex[1], info->d_transformedVertex[2]))
					{
						io_planeFit.AddTransformPoint(intersections.d_pointInObb[j] *
								(*(d_obb.Matrix())) + (*d_obb.Center()));
					}
				}
			}
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void CObbToTerrainIntersection::ObbEdgeIntersectionPlaneFit(Vector3 &o_loc,
		Vector3 &o_normal)
{
	if ((d_validFlags & INTERSECTION_PLANE) == 0)
	{
		d_validFlags |= INTERSECTION_PLANE;

		if ((d_validFlags & INTERSECTION_TEST) == 0)
			PerformIntersectionTest();

		if (d_triangleList.NumTriangles() > 0)
		{
			if ((d_validFlags & TRIANGLE_NORMAL_SUM) == 0)
				PerformTriangleNormalSum();

			CPlaneFit planeFit;
			planeFit.TransformInit(d_normalSum);
			PerformAddObbEdgeIntersections(planeFit);

			d_planeResult = planeFit.TransformFit(d_planeLocation, d_planeNormal);
		}
		else
		{
			d_planeLocation = *(d_obb.Center());
			d_planeNormal.Set(0.0f, 1.0f, 0.0f);
		}
	}

	o_loc = d_planeLocation;
	o_normal = d_planeNormal;
}

/***************************************************************************/
/***************************************************************************/
bool CObbToTerrainIntersection::InitObbEdgeIntersectionTest(
		SObbEdgeTerrainState &o_state)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();

	if (d_triangleList.NumTriangles() <= 0)
		return(false);

	o_state.d_entry = 0;
	o_state.d_path = 0;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool CObbToTerrainIntersection::GetNextObbEdgeIntersection(
		SObbEdgeTerrainState &io_state, TerrainTriangle* &o_triangle,
		Vector3 &o_intersection)
{
	while (io_state.d_entry < d_triangleList.NumTriangles())
	{
		STriangleEntry* info =
				d_triangleList.UnsafeGetTriangleEntry(io_state.d_entry);
		if (io_state.d_path == 0)
		{
			if (d_obb.ComputeObbPlaneIntersections(info->d_transformedVertex[0],
					info->d_transformedNormal, io_state.d_intersections))
			{
				Vector3 absNormal = Vector3::Abs(info->d_transformedNormal);

				if (absNormal.x() > absNormal.y())
				{
					if (absNormal.x() > absNormal.z())
						io_state.d_plane = SObbEdgeTerrainState::PLANE_YZ;
					else
						io_state.d_plane = SObbEdgeTerrainState::PLANE_XY;
				}
				else if (absNormal.y() > absNormal.z())
					io_state.d_plane = SObbEdgeTerrainState::PLANE_XZ;
				else
					io_state.d_plane = SObbEdgeTerrainState::PLANE_XY;
			}
			else
				io_state.d_path = OBB::NUM_PATH;
		}

		while(io_state.d_path < OBB::NUM_PATH)
		{
			if ((io_state.d_intersections.d_master & (1 << io_state.d_path)) != 0)
			{
				bool hit;
				switch(io_state.d_plane)
				{
				 case SObbEdgeTerrainState::PLANE_YZ:
					hit = TriangleTest::PointInsideYZ(
							io_state.d_intersections.d_pointInObb[io_state.d_path],
							info->d_transformedVertex[0], info->d_transformedVertex[1],
							info->d_transformedVertex[2]);
					break;
				 case SObbEdgeTerrainState::PLANE_XZ:
					hit = TriangleTest::PointInsideXZ(
							io_state.d_intersections.d_pointInObb[io_state.d_path],
							info->d_transformedVertex[0], info->d_transformedVertex[1],
							info->d_transformedVertex[2]);
					break;
				 case SObbEdgeTerrainState::PLANE_XY:
				 default:
					hit = TriangleTest::PointInsideXY(
							io_state.d_intersections.d_pointInObb[io_state.d_path],
							info->d_transformedVertex[0], info->d_transformedVertex[1],
							info->d_transformedVertex[2]);
					break;
				}
				if (hit)
				{
					o_triangle = info->d_triangle;
					o_intersection = io_state.d_intersections.d_pointInObb[
							io_state.d_path] * (*(d_obb.Matrix())) + (*d_obb.Center());
					io_state.d_path++;
					return(true);
				}
			}
			io_state.d_path++;
		}
		io_state.d_entry++;
		io_state.d_path = 0;
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
CCollisionStackArray<Vector3>*
		CObbToTerrainIntersection::GetTriangleIntersections(
		Vector3CRef i_direction, float i_scaledCosAngle)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();

	if (!d_triangleList.BuildVertexList() || !d_triangleList.BuildEdgeList())
		return(NULL);

	CCollisionStackArray<Vector3>* triangleIntersection =
			CCollisionStackArray<Vector3>::New(d_stack);
	if (triangleIntersection == NULL)
		return(NULL);

	int numTriangles = d_triangleList.NumTriangles();
	int numEdges = d_triangleList.NumEdges();
	for (int i = 0; i < numEdges; i++)
	{
		bool tested = false;
		for (int j = 0; (j < numTriangles) && !tested; j++)
		{
			STriangleEntry* entry = d_triangleList.UnsafeGetTriangleEntry(j);
			for (int k = 0; k < 3; k++)
			{
				if (entry->d_edgeIndex[k] == i)
				{
					if (i_direction.Dot(entry->d_triangle->FaceNormal()) >=
							i_scaledCosAngle)
					{
						if (!TestAndAddTriangleIntersections(i, triangleIntersection))
							return(triangleIntersection);
						tested = true;
					}
					break;
				}
			}
		}
	}

	return(triangleIntersection);
}

/***************************************************************************/
/***************************************************************************/
void CObbToTerrainIntersection::BuildTriangleIntersections(void)
{
	d_validFlags |= TRIANGLE_INTERSECTION;

	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();

	if ((d_triangleIntersection == NULL) || !d_triangleList.BuildVertexList() ||
			!d_triangleList.BuildEdgeList())
		return;

	int numEdges = d_triangleList.NumEdges();
	for (int i = 0; i < numEdges; i++)
	{
		if (!TestAndAddTriangleIntersections(i, d_triangleIntersection))
			return;
	}
}

/***************************************************************************/
/***************************************************************************/
bool CObbToTerrainIntersection::TestAndAddTriangleIntersections(int i_edge,
		CCollisionStackArray<Vector3>* io_triangleIntersection)
{
	Vector3 start, end, delta;
	d_triangleList.UnsafeGetEdge(i_edge, start, end);
	delta = end - start;

	uint entryFace, exitFace;
	float entryFrac, exitFrac;
	if (d_obb.IntersectLineInWorld(start, delta, entryFace, entryFrac,
			exitFace, exitFrac))
	{
		if ((entryFrac >= 0.0f) && (entryFrac <= 1.0f))
		{
			Vector3* intersection = io_triangleIntersection->ExpandArray();
			if (intersection == NULL)
				return(false);
			*intersection = start + entryFrac * delta;
		}

		if ((exitFrac >= 0.0f) && (exitFrac <= 1.0f))
		{
			Vector3* intersection = io_triangleIntersection->ExpandArray();
			if (intersection == NULL)
				return(false);
			*intersection = start + exitFrac * delta;
		}
	}
	return(true);
}

/***************************************************************************/
/***************************************************************************/
void CObbToTerrainIntersection::BuildTriangleVertices(void)
{
	d_validFlags |= TRIANGLE_VERTEX;

	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();

	if ((d_triangleVertex == NULL) || !d_triangleList.BuildVertexList())
		return;

	int numVertices = d_triangleList.NumVertices();
	for (int i = 0; i < numVertices; i++)
	{
		Vector3 vertex = *(d_triangleList.UnsafeGetVertex(i));
		if (d_obb.PointInBox(vertex))
		{
			Vector3* entry = d_triangleVertex->ExpandArray();
			if (entry == NULL)
				return;
			*entry = vertex;
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void CObbToTerrainIntersection::DrawTerrain(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	d_triangleList.Draw();
}

/***************************************************************************/
/***************************************************************************/
void CObbToTerrainIntersection::DrawDebug(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();

	if (!d_triangleList.BuildVertexList() || !d_triangleList.BuildEdgeList())
		return;

	// Draw the obb.
	d_obb.Draw(DrawUtility::WHITE);

	// Draw the edges.
	int numEdges = d_triangleList.NumEdges();
	for (int i = 0; i < numEdges; i++)
	{
		Vector3 start, end;
		if (d_triangleList.UnsafeGetEdge(i, start, end))
		{
			uint entryFace, exitFace;
			float entryPoint, exitPoint;
			if (d_obb.IntersectLineInWorld(start, end - start, entryFace,
					entryPoint, exitFace, exitPoint) && (entryPoint <= 1.0f) &&
					(exitPoint >= 0.0f))
				DrawUtility::Line(start, end, DrawUtility::YELLOW);
			else
				DrawUtility::Line(start, end, DrawUtility::GREEN);
		}
	}

	// Draw the obb edge intersections.
	SObbEdgeTerrainState state;
	if (InitObbEdgeIntersectionTest(state))
	{
		TerrainTriangle* triangle;
		Vector3 intersection;
		while (GetNextObbEdgeIntersection(state, triangle, intersection))
		{
			DrawUtility::Sphere(intersection, DrawUtility::BLUE, 0.02f);
		}
	}

	// Draw the terrain edge intersections.
	CCollisionStackArray<Vector3>* triangleIntersection =
			GetTriangleIntersections();
	if (triangleIntersection != NULL)
	{
		int intersectionPoints = triangleIntersection->NumEntries();
		for (int i = 0; i < intersectionPoints; i++)
		{
			DrawUtility::Sphere(triangleIntersection->EntryRef(i),
					DrawUtility::BLUE, 0.02f);
		}
	}

	// Draw the plane.
	Vector3 point, normal;
	ObbEdgeIntersectionPlaneFit(point, normal);
	DrawUtility::NormalPlane(point, normal, DrawUtility::MAGENTA,
			DrawUtility::CYAN);

	Vector3 verifyPoint, verifyNormal;
	verifyNormal.Clear();
	verifyNormal += TriangleNormalSum();
	CPlaneFit planeFit;
	planeFit.TransformInit(verifyNormal);
	AddObbEdgeIntersections(planeFit);
	planeFit.TransformFit(verifyPoint, verifyNormal);
}

/***************************************************************************/
/***************************************************************************/
void CObbToTerrainIntersection::DrawDebugTerrain(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();

	if (!d_triangleList.BuildVertexList() || !d_triangleList.BuildEdgeList())
		return;

	// Draw the edges.
	int numEdges = d_triangleList.NumEdges();
	for (int i = 0; i < numEdges; i++)
	{
		Vector3 start, end;
		if (d_triangleList.UnsafeGetEdge(i, start, end))
		{
			uint entryFace, exitFace;
			float entryPoint, exitPoint;
			if (d_obb.IntersectLineInWorld(start, end - start, entryFace,
					entryPoint, exitFace, exitPoint) && (entryPoint <= 1.0f) &&
					(exitPoint >= 0.0f))
				DrawUtility::Line(start, end, DrawUtility::YELLOW);
			else
				DrawUtility::Line(start, end, DrawUtility::GREEN);
		}
	}
}

#ifdef DEBUG_STACK

#include <string.h>

/**********************************************************************/
/**********************************************************************/
const char* CObbToTerrainIntersection::GetName(void)
{
	return("ObbToTerrain");
}

/***************************************************************************/
/***************************************************************************/
void CObbToTerrainIntersection::AppendDescription(char* io_msg)
{
	char msg[256];
	sprintf(msg, " with %d Triangles", d_triangleList.NumTriangles());
	strcat(io_msg, msg);
}

#endif
