#ifndef __LINE_TO_LINE_H_INCLUDED__
#define __LINE_TO_LINE_H_INCLUDED__

class OBB;

float PointToLine_TOnly( Vector3CRef origin, Vector3CRef direction, Vector3CRef point );
float PointToLineDistanceSquared( Vector3CRef origin, Vector3CRef direction, 
	Vector3CRef point, float &o_t, Vector3 &o_p );
float PointToSegmentDistanceSquared( Vector3CRef origin, Vector3CRef direction, float start, float end,
	Vector3CRef point, float &o_t, Vector3 &o_p );
float PointToSegmentDistanceSquared( Vector3CRef origin, Vector3CRef direction, float start, float end,
	Vector3CRef point );

void LineToLine_TOnly( Vector3CRef origin1, Vector3CRef direction1,
	Vector3CRef origin2, Vector3CRef direction2, float &o_t1, float &o_t2 );
float LineToLineDistanceSquared( Vector3CRef origin1, Vector3CRef direction1,
	Vector3CRef origin2, Vector3CRef direction2, float &o_t1, float &o_t2,
	Vector3 &o_p1, Vector3 &o_p2 );
float SegmentToSegmentDistanceSquared( Vector3CRef origin1, Vector3CRef direction1, float start1, float end1,
	Vector3CRef origin2, Vector3CRef direction2, float start2, float end2 );
float SegmentToSegmentDistanceSquared( Vector3CRef origin1, Vector3CRef direction1, float start1, float end1,
	Vector3CRef origin2, Vector3CRef direction2, float start2, float end2,
	float &o_t1, float &o_t2, Vector3 &o_p1, Vector3 &o_p2 );

float SegmentToTriangleDistanceSquared( Vector3CRef LOrigin, Vector3CRef LDirection, float LStart, float LEnd,
	Vector3CRef TNormal, Vector3CRef TOrigin, Vector3CRef E0, Vector3CRef E1, Vector3 &o_LP, Vector3 &o_TP );

	typedef struct tag_SegmentToRectangleInfo
{
	float dSquared;
	float LT;
	float RT1;
	float RT2;
	Vector3 PointOnLine;
} SegmentToRectangleInfo;

float PointToOBBDistanceSquared( Vector3CRef point, const OBB& obb, Vector3 &o_BP );
float SegmentToOBBDistanceSquared( Vector3CRef LOrigin, Vector3CRef LDirection,
	float LStart, float LEnd, const OBB &obb, Vector3 &o_LP, Vector3 &o_BP );

bool LineToPlaneIntersection( Vector3CRef LOrigin, Vector3CRef LDirection, 
	Vector3CRef PNormal, Vector3CRef POrigin, float &o_t, float &o_LNRatio );

void SegmentToRectangleDistanceSquared( Vector3CRef LOrigin, Vector3CRef LDirection,
	float LStart, float LEnd, Vector3CRef RNormal, Vector3CRef ROrigin, Vector3CRef RDirection1, 
	float RStart1, float REnd1, Vector3CRef RDirection2, float RStart2, float REnd2, 
	SegmentToRectangleInfo &o_info );

float CapsuleToRectangleMaxPenetration( Vector3CRef LOrigin, Vector3CRef LDirection, float radius,
	float LStart, float LEnd, Vector3CRef RNormal, Vector3CRef ROrigin, Vector3CRef RDirection1, 
	float RStart1, float REnd1, Vector3CRef RDirection2, float RStart2, float REnd2, 
	SegmentToRectangleInfo &o_info );

#endif