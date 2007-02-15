// System includes //
#include <new.h>

// Gamelib includes //
#include "collide/Stack.h"

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/CapsuleToTerrain.h"
#include "geomUtil/LineToLine.h"

#if 0

// Engine include //
#include "Math/Vector.h"

// Gamelib includes //
#include "collide/StackArray.h"
#include "geomUtil/planefit.h"

#endif

// Valid data masks.
const uint CCapsuleToTerrainIntersection::INTERSECTION_TEST     = 0x01;

CCapsuleToTerrainIntersection::CCapsuleToTerrainIntersection(
		Capsule &i_capsule, CTerrainCollisionObject &i_terrain) :
	CCollisionIntersection(CAPSULE_TO_TERRAIN), d_capsule(i_capsule), 
	d_terrain(i_terrain), d_iterator(NULL)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CCapsuleToTerrainIntersection::CCapsuleToTerrainIntersection(Capsule &i_capsule,
		CTerrainCollisionObject &i_terrain, CCollisionStack &i_stack) :
	CCollisionIntersection(CAPSULE_TO_TERRAIN, i_stack), d_capsule(i_capsule),
	d_terrain(i_terrain), d_iterator(&i_stack)
{
	ClearTests();
}

/***************************************************************************/
/***************************************************************************/
CCapsuleToTerrainIntersection* CCapsuleToTerrainIntersection::New(
	Capsule &i_capsule, CTerrainCollisionObject &i_terrain, CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CCapsuleToTerrainIntersection* intersection =
			(CCapsuleToTerrainIntersection*)i_stack->Allocate(
			sizeof(CCapsuleToTerrainIntersection));

		if (intersection)
			new(intersection) CCapsuleToTerrainIntersection(i_capsule, i_terrain,
			*i_stack);

		return(intersection);
	}

	return(new CCapsuleToTerrainIntersection(i_capsule, i_terrain));
}

CCollisionIntersection* CCapsuleToTerrainIntersection::TestForIntersection(
	Capsule* i_capsule, CTerrainCollisionObject* i_terrain, CCollisionStack* i_stack)
{
	CCapsuleToTerrainIntersection* intersection = CCapsuleToTerrainIntersection::New(
		*i_capsule, *i_terrain, i_stack);
	if (intersection)
	{
		if (intersection->IntersectionTest())
			return(intersection);
		CCollisionStackItem::Delete(intersection);
	}
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
bool CCapsuleToTerrainIntersection::IntersectionTest(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
	{
		d_validFlags |= INTERSECTION_TEST;

		resolutionNormal.Clear();
		resolutionDistance = 0.0f;

		d_capsule.ComputeWorldExtents( d_iterator.Bounds() );
		if( d_iterator.Initialize(d_terrain) )
		{
			TerrainTriangle* triangle;
			while ((triangle = d_iterator.GetNextTriangle()) != NULL)
			{
				// check the capsule's bounds along the triangle's normal first
				float originDist, endDist, minDist;
				originDist = (d_capsule.Origin() - triangle->A()).Dot( triangle->FaceNormal() );
				endDist = (d_capsule.End() - triangle->A()).Dot( triangle->FaceNormal() );
				minDist = Math::Min( Math::Abs( originDist ), Math::Abs( endDist ) );
				if( !Math::SameSign( originDist, endDist ) || minDist < d_capsule.Radius() )
				{
					// now do a separating axis check, along the axis from one center to the other
					// going to require normalizing a vector, but we have a good chance of saving
					// all the extra math of a segment to triangle distance check
					Vector3 triCenter = (triangle->A() + triangle->B() + triangle->C()) * (1.0f / 3.0f);
					Vector3 d = d_capsule.Center() - triCenter;
					float r;
					bool reject = false;

					r = d.Length();
					if( !Math::Zero( r ) ) 
					{
						float r0, r1, test;
						d = d / r;
						r0 = (triangle->A() - triCenter).Dot( d );
						test = (triangle->B() - triCenter).Dot( d );
						if( test > r0 )
							r0 = test;
						test = (triangle->C() - triCenter).Dot( d );
						if( test > r0 )
							r0 = test;
						r1 = Math::Abs( (d_capsule.End() - d_capsule.Center()).Dot( d ) );
						r1 += d_capsule.Radius();

						if( r0 + r1 < r )
							reject = true;
					}

					if( !reject )
					{
						
						Vector3 pointOnLine, pointOnTriangle;
						float dSquared = SegmentToTriangleDistanceSquared( d_capsule.Origin(), d_capsule.Axis(),
							0.0f, d_capsule.AxisLength(), triangle->FaceNormal(), triangle->A(), 
							triangle->B() - triangle->A(), triangle->C() - triangle->A(),
							pointOnLine, pointOnTriangle );
						#ifdef DEBUG_POINTDUMP
						static int DumpPoints = 0;
						if( DumpPoints )
						{
							dbgPrint( "Vector3 A( %ff, %ff, %ff );\n", triangle->A().x(), triangle->A().y(), triangle->A().z() );
							dbgPrint( "Vector3 B( %ff, %ff, %ff );\n", triangle->B().x(), triangle->B().y(), triangle->B().z() );
							dbgPrint( "Vector3 C( %ff, %ff, %ff );\n", triangle->C().x(), triangle->C().y(), triangle->C().z() );
							dbgPrint( "Vector3 N( %ff, %ff, %ff );\n", triangle->FaceNormal().x(), triangle->FaceNormal().y(), triangle->FaceNormal().z() );
							dbgPrint( "Vector3 O( %ff, %ff, %ff );\n", d_capsule.Origin().x(), d_capsule.Origin().y(), d_capsule.Origin().z() );
							dbgPrint( "Vector3 E( %ff, %ff, %ff );\n", d_capsule.End().x(), d_capsule.End().y(), d_capsule.End().z() );
						}
						DrawUtility::Point( pointOnLine, DrawUtility::YELLOW, 0.1f );
						DrawUtility::Point( pointOnTriangle, DrawUtility::CYAN, 0.1f );
						DrawUtility::Line( pointOnLine, pointOnTriangle, DrawUtility::BLUE );
						#endif
						if( dSquared < d_capsule.Radius() * d_capsule.Radius() )
						{
							d_intersects = true;
							Vector3 displacementDir;
							float dist;
							
							// NOTE: this code is all probably quite a bit slower than it needs to be,
							// but I just don't have time right now to figure out a more efficient way
							// to do what needs to happen here.  I believe this is still faster
							// than what OBB to terrain had to do, though, so although its not as
							// fast as it could be, its still an overall win.
							if( Math::Zero( dSquared ) )
							{
								// axis intersects exactly on the surface, use the triangle's normal
								// as the displacement direction
								displacementDir = triangle->FaceNormal();
								dist = d_capsule.Radius();
							}
							else
							{
								displacementDir = pointOnLine - pointOnTriangle;
								dist = displacementDir.Length();

								if( !Math::Zero( dist ) )
									displacementDir = displacementDir / dist;
								dist = d_capsule.Radius() - dist;
							}

							// if the resolution vector we already have would perform this displacement,
							// don't add it.
							float RDotD = displacementDir.Dot( resolutionNormal );
							if( Math::Zero( resolutionDistance ) || (RDotD * dist) > resolutionDistance )
							{
								// our desired displacement goes further along
								// the current resolution normal than the old one does.
								// ignore the old one and just use this one
								resolutionNormal = displacementDir;
								resolutionDistance = dist;
							}
							else
							{
								if( RDotD > 0.0f )
								{
									// we need to remove the component of our desired displacement
									// that is in the same direciton as the old resolution normal
									displacementDir -= resolutionNormal * RDotD;
									dist *= (1.0f - RDotD);
									displacementDir.SafeNormalize();
								}

								resolutionNormal = resolutionNormal * resolutionDistance + displacementDir * dist;

								resolutionDistance = resolutionNormal.Length();
								if( !Math::Zero( resolutionDistance ) )
									resolutionNormal = resolutionNormal / resolutionDistance;
							}
						}
					}
				}
			}
		}
		ASSERT(!d_stack->IsBottomLocked());
	}
	return(d_intersects);
}

#ifdef DEBUG_STACK

#include <string.h>

/**********************************************************************/
/**********************************************************************/
const char* CCapsuleToTerrainIntersection::GetName(void)
{
	return("CapsuleToTerrain");
}

/***************************************************************************/
/***************************************************************************/
void CCapsuleToTerrainIntersection::AppendDescription(char* io_msg)
{
	//char msg[256];
	//sprintf(msg, " with %d Triangles", d_triangleList.NumTriangles());
	//strcat(io_msg, msg);
}

#endif
