///////////////////////////////////////////////////////////////////////////////
// Fast coplanar check
//
// - Builds AABB
// - Collects terrain triangles
// - Tests each edge
///////////////////////////////////////////////////////////////////////////////
#include "CollideHelper/CollideHelperPCH.h"

#include "CollideHelper/LineEntity.h"
#include "CollideHelper/aabbEntity.h"
#include "CollideHelper/ccoplanar.h"

#if defined(FOOD) && defined(_DEBUG)
//#define COPLANAR_DEBUG
#endif

//#define USE_SELECTION_ENTITY
#ifdef USE_SELECTION_ENTITY
#include "CollideHelper/SelectionEntity.h"
#endif


///////////////////////////////////////////////////////////////////////////////
// Are these coplanar on the terrain in the world?
///////////////////////////////////////////////////////////////////////////////
bool CheckCoplanar::Check(
const Vector3 corner[4],
Vector3CRef normal,
float start,
float end,
bool enforceMaterial,
bool cheap)
{
	// Figure out some stuff
	const Vector3 startP = normal * start;
	const Vector3 endP = normal * end;

	// Make an AABB with this info
	CAabbEntity test;
	AABB &aabb = test.Aabb().GetAABB();
	aabb.Set(corner[0] + startP, corner[0] + startP);
	aabb.ExpandByPoint(corner[0] + endP);
	aabb.ExpandByPoint(corner[1] + startP);
	aabb.ExpandByPoint(corner[1] + endP);
	aabb.ExpandByPoint(corner[2] + startP);
	aabb.ExpandByPoint(corner[2] + endP);
	aabb.ExpandByPoint(corner[3] + startP);
	aabb.ExpandByPoint(corner[3] + endP);
	const Vector3 inflate(0.005f, 0.005f, 0.005f);
	aabb.Inflate(inflate);

#if defined(COPLANAR_DEBUG)
	DrawUtility::QueueBox(aabb.minV, aabb.maxV, DrawUtility::YELLOW, 2.0f);
#endif

	// Setup the object
	test.SetPositiveInclude(WORLD_CATEGORY);

#ifndef USE_SELECTION_ENTITY

	// Reset the stack
	CCollisionStack &stack = g_collisionSystem.GetStack();
	stack.Reset();

	// Query the database
	CCollisionList *collisionList = g_collisionSystem.Test(&test);
	if (!collisionList)
		return false;
	ASSERT(!collisionList->Empty());

	// Lock down the stack
	CCollisionStackLock stackLock;
	stack.Lock(&stackLock);

	// Setup the line test object
	const float length = end - start;

	CLineCollisionObject line[4];
	int l;
	for (l = 0; l < 4; l++)
	{
		line[l].SetDetectionAbility(CLineCollisionObject::DETECT_BOTH_SIDES);
		line[l].SetFindMode(CLineCollisionObject::FIND_CLOSEST);
		line[l].SetOriginBasis(corner[l] + startP, normal);
		line[l].SetMaxBoundary(length);
		line[l].InitializeFindMode();

#if defined(COPLANAR_DEBUG)
  		DrawUtility::QueueLine(corner[l] + startP, corner[l] + endP, DrawUtility::CYAN, 5.0f);
#endif
	}

	// Get the first pair
	CCollisionEvent *event = collisionList->GetHeadEvent();
	while (event)
	{
		CCollisionPair *pair = event->GetHeadPair();
		ASSERT(pair);

		CCollisionIntersection::ECollisionIntersectionType intersectionType = pair->GetIntersectionType();
		ASSERT(intersectionType == CCollisionIntersection::AABB_TO_TERRAIN || intersectionType == CCollisionIntersection::AABB_TO_OBB);

		// Is this the terrain?
		CCollisionObject *targetObject;
		if (intersectionType == CCollisionIntersection::AABB_TO_TERRAIN)
			targetObject = &((CAabbToTerrainIntersection *)pair->GetIntersection())->GetTriangleList();
		else // This is the obb
			targetObject = pair->GetTargetObject();
		ASSERT(targetObject);

		while (pair)
		{
			// Intersect 4 tiny lines
			for (l = 0; l < 4; l++)
				CCollisionPair::TestObjectPair(&line[l], targetObject, &stack);

			pair = pair->GetNext();
		}

		// Next!
		event = event->GetNext();
	}

	// Finish the test
	Vector3 intersection[4];
	u8 material;
	for (l = 0; l < 4; l++)
	{
		line[l].TerminateFindMode();
		CCollisionIntersection *collisionIntersection = line[l].Modifier();
		if (!collisionIntersection)
			return false;

		// What is the intersection point
		intersection[l] = line[l].ComputePoint(line[l].FoundPoint());

#if defined(COPLANAR_DEBUG)
  		DrawUtility::QueuePoint(intersection[l], DrawUtility::RED, 0.01f, 5.0f);
#endif

		// Same material?
		if (enforceMaterial)
		{
			u8 hitMaterial;
			switch (collisionIntersection->GetIntersectionType())
			{
				case CCollisionIntersection::LINE_TO_TERRAIN:
					hitMaterial = ((CLineToTerrainIntersection *)collisionIntersection)->GetFoundPoint()->Material();
					break;

				case CCollisionIntersection::LINE_TO_OBB:
					hitMaterial = ((OBBStaticInstance &)((CLineToObbIntersection *)collisionIntersection)->Obb()).Material();
					break;

				default:
					ASSERT(false);
					hitMaterial = 0xff;
					break;
			}

			if (l == 0)
				material = hitMaterial;
			else if (material != hitMaterial)
				return false;
		}
	}

#else // USE_SELECTION_ENTITY

	// Select everything touching the aabb.
	CSelectionEntity selection;
	if (!selection.Select(&test, CStageEntity::USE_SIMPLE, false))
		return false;

	CLineEntity lineEntity;
	CLineCollisionObject& line = lineEntity.Line();
	line.SetDetectionAbility(CLineCollisionObject::DETECT_BOTH_SIDES);
	line.SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	line.SetMaxBoundary(end - start);
	Vector3 intersection[4];
	u8 material;
	int l;
	for (l = 0; l < 4; l++)
	{
#if defined(COPLANAR_DEBUG)
  		DrawUtility::QueueLine(corner[l] + startP, corner[l] + endP, DrawUtility::CYAN, 5.0f);
#endif

		line.SetOriginBasis(corner[l] + startP, normal);
		lineEntity.BeginQuery();
		CCollisionEvent::TestEntityPair(&lineEntity, &selection, &g_collisionSystem.GetStack());
		lineEntity.EndQuery();

		CCollisionIntersection *collisionIntersection = line.Modifier();
		if (!collisionIntersection)
			return false;

		// What is the intersection point
		intersection[l] = line.ComputePoint(line.FoundPoint());

#if defined(COPLANAR_DEBUG)
		DrawUtility::QueuePoint(intersection[l], DrawUtility::RED, 0.01f, 5.0f);
#endif

		// Same material?
		if (enforceMaterial)
		{
			u8 hitMaterial;
			switch (collisionIntersection->GetIntersectionType())
			{
				case CCollisionIntersection::LINE_TO_TERRAIN:
					hitMaterial = ((CLineToTerrainIntersection *)collisionIntersection)->GetFoundPoint()->Material();
					break;

				case CCollisionIntersection::LINE_TO_OBB:
					hitMaterial = ((OBBStaticInstance &)((CLineToObbIntersection *)collisionIntersection)->Obb()).Material();
					break;

				default:
					ASSERT(false);
					hitMaterial = 0xff;
					break;
			}

			if (l == 0)
				material = hitMaterial;
			else if (material != hitMaterial)
				return false;
		}
	}

#endif // USE_SELECTION_ENTITY

	// We are done!
	if (cheap)
		return true;

	// There are a bunch of ways to do this
	//Vector3 newNormal = Vector3::Cross(corner[2] - corner[1], corner[0] - corner[1]);
	for (int t = 1; t < 4; t++)
	{
		float dot = Vector3::Dot(intersection[t] - intersection[0], normal);
		if (!Math::Zero(dot, 0.001f))
			return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Are these coplanar on the terrain in the world?
///////////////////////////////////////////////////////////////////////////////
#ifdef PS2
bool CheckCoplanar::Check(
const Vector3Packed corner[4],
Vector3CRef normal,
float start,
float end,
bool enforceMaterial,
bool cheap)
{
	// Figure out some stuff
	const Vector3 startP = normal * start;
	const Vector3 endP = normal * end;

	// Make an AABB with this info
	CAabbEntity test;
	AABB &aabb = test.Aabb().GetAABB();
	aabb.Set(corner[0] + startP, corner[0] + startP);
	aabb.ExpandByPoint(corner[0] + endP);
	aabb.ExpandByPoint(corner[1] + startP);
	aabb.ExpandByPoint(corner[1] + endP);
	aabb.ExpandByPoint(corner[2] + startP);
	aabb.ExpandByPoint(corner[2] + endP);
	aabb.ExpandByPoint(corner[3] + startP);
	aabb.ExpandByPoint(corner[3] + endP);
	const Vector3 inflate(0.005f, 0.005f, 0.005f);
	aabb.Inflate(inflate);

#if defined(COPLANAR_DEBUG)
	DrawUtility::QueueBox(aabb.minV, aabb.maxV, DrawUtility::YELLOW, 2.0f);
#endif

	// Setup the object
	test.SetPositiveInclude(WORLD_CATEGORY);

	// Reset the stack
	CCollisionStack &stack = g_collisionSystem.GetStack();
	stack.Reset();

	// Query the database
	CCollisionList *collisionList = g_collisionSystem.Test(&test);
	if (!collisionList)
		return false;
	ASSERT(!collisionList->Empty());

	// Lock down the stack
	CCollisionStackLock stackLock;
	stack.Lock(&stackLock);

	// Setup the line test object
	const float length = end - start;

	CLineCollisionObject line[4];
	int l;
	for (l = 0; l < 4; l++)
	{
		line[l].SetDetectionAbility(CLineCollisionObject::DETECT_BOTH_SIDES);
		line[l].SetFindMode(CLineCollisionObject::FIND_CLOSEST);
		line[l].SetOriginBasis(corner[l] + startP, normal);
		line[l].SetMaxBoundary(length);
		line[l].InitializeFindMode();

#if defined(COPLANAR_DEBUG)
  		DrawUtility::QueueLine(corner[l] + startP, corner[l] + endP, DrawUtility::CYAN, 5.0f);
#endif
	}

	// Get the first pair
	CCollisionEvent *event = collisionList->GetHeadEvent();
	while (event)
	{
		CCollisionPair *pair = event->GetHeadPair();
		ASSERT(pair);

		CCollisionIntersection::ECollisionIntersectionType intersectionType = pair->GetIntersectionType();
		ASSERT(intersectionType == CCollisionIntersection::AABB_TO_TERRAIN || intersectionType == CCollisionIntersection::AABB_TO_OBB);

		// Is this the terrain?
		CCollisionObject *targetObject;
		if (intersectionType == CCollisionIntersection::AABB_TO_TERRAIN)
			targetObject = &((CAabbToTerrainIntersection *)pair->GetIntersection())->GetTriangleList();
		else // This is the obb
			targetObject = pair->GetTargetObject();
		ASSERT(targetObject);

		while (pair)
		{
			// Intersect 4 tiny lines
			for (l = 0; l < 4; l++)
				CCollisionPair::TestObjectPair(&line[l], targetObject, &stack);

			pair = pair->GetNext();
		}

		// Next!
		event = event->GetNext();
	}

	// Finish the test
	Vector3 intersection[4];
	u8 material;
	for (l = 0; l < 4; l++)
	{
		line[l].TerminateFindMode();
		CCollisionIntersection *collisionIntersection = line[l].Modifier();
		if (!collisionIntersection)
			return false;

		// What is the intersection point
		intersection[l] = line[l].ComputePoint(line[l].FoundPoint());

#if defined(COPLANAR_DEBUG)
  		DrawUtility::QueuePoint(intersection[l], DrawUtility::RED, 0.01f, 5.0f);
#endif

		// Same material?
		if (enforceMaterial)
		{
			u8 hitMaterial;
			switch (collisionIntersection->GetIntersectionType())
			{
				case CCollisionIntersection::LINE_TO_TERRAIN:
					hitMaterial = ((CLineToTerrainIntersection *)collisionIntersection)->GetFoundPoint()->Material();
					break;

				case CCollisionIntersection::LINE_TO_OBB:
					hitMaterial = ((OBBStaticInstance &)((CLineToObbIntersection *)collisionIntersection)->Obb()).Material();
					break;

				default:
					ASSERT(false);
					hitMaterial = 0xff;
					break;
			}

			if (l == 0)
				material = hitMaterial;
			else if (material != hitMaterial)
				return false;
		}
	}

	// We are done!
	if (cheap)
		return true;

	// There are a bunch of ways to do this
	//Vector3 newNormal = Vector3::Cross(corner[2] - corner[1], corner[0] - corner[1]);
	for (int t = 1; t < 4; t++)
	{
		float dot = Vector3::Dot(intersection[t] - intersection[0], normal);
		if (!Math::Zero(dot, 0.001f))
			return false;
	}

	return true;
}
#endif





















