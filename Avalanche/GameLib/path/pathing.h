#ifndef PATHING_H
#define PATHING_H

#include "Math/Vector.h"
#include "path/Path.h"

extern ts_SplinePathTable* gbl_paths;

ts_SplinePath* GetSplinePath(const char* i_sPathName);

// ===========================================================================
//	CLASS: Line

class Line
{
	public:
		Vector3 a;
		Vector3 b;

	public:
		Line() {};
		Line(const Vector3& a, const Vector3& b);

		float ComputeDistanceFrom(const Vector3& point);
		float ComputeDistanceFromSegment(const Vector3& point);

	public:	// *** STATIC FUNCTIONS ***

		static float ComputeDistanceFrom(const Vector3& a, const Vector3& b, const Vector3& point);
		static float ComputeDistanceFromSegment(const Vector3& a, const Vector3& b, const Vector3& point);

		static Vector3 GetClosestPointOnLine(const Vector3& i_vEndPointA, const Vector3& i_vEndPointB, const Vector3& i_vPointInSpace, float* o_fT = 0);
		static Vector3 GetClosestPointOnSegment(const Vector3& i_vEndPointA, const Vector3& i_vEndPointB, const Vector3& i_vPointInSpace, float* o_fT = 0);
};


// ===========================================================================
//	CLASS: Plane

class Plane
{
	public:
		Vector3 n;	// normal
		Vector3 p;	// point on plane

	public:
		Plane() {};
		Plane(const Vector3& n, const Vector3& p);

		float GetDistanceFromPoint(const Vector3& point);

	public:	// *** STATIC FUNCTIONS ***

		static float GetDistanceFromPoint(const Vector3& n, const Vector3& p, const Vector3& point);
};

// ===========================================================================
//	CLASS: Spline

class Spline
{
	public:
		Vector3 a;	// end point
		Vector3 b;	// control point
		Vector3 c;	// control point
		Vector3 d;	// end point

	public:
		Spline() {};
		Spline(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d);

		void GetPointAndTangent(float t, Vector3* point, Vector3* tangent);

	public:	// *** STATIC FUNCTIONS ***

		static void GetPointAndTangent(float t, const Vector3& a, const Vector3& b,
			const Vector3& c, const Vector3& d, Vector3* point, Vector3* tangent);
};

// ===========================================================================
//	CLASS: Cone

class Cone
{
	public:
		Vector3	vApex;
		Vector3	vBase;
		float		fBaseRadius;

	public:
		Cone() {};
		Cone(const Vector3& i_vApex, const Vector3& i_vBase, float i_fBaseRadius);

		bool IsInside(const Vector3& i_vPointInSpace);
		bool IsInsideSection(const Vector3& i_vPointInSpace);

	public:	// *** STATIC FUNCTIONS ***

		static bool IsInside(const Vector3& i_vApex, const Vector3& i_vBase,
			float i_fBaseRadius, const Vector3& i_vPointInSpace);

		static bool IsInsideSection(const Vector3& i_vApex, const Vector3& i_vBase,
			float i_fBaseRadius, const Vector3& i_vPointInSpace);
};

// ===========================================================================
//	CLASS: PointPathFollower

class PointPathFollower
{
	public:
		PointPathFollower();
		~PointPathFollower();

	public:
		void							Initialize(const ts_SplinePath* dblPath, float defaultSpeed = 10.0f);
		void							Update(const Vector3& position);
		void							Reset();
		void							Draw();

		float							GetHeight() const;
		float							GetSpeed() const;
		const Vector3&				GetClosestPointOnPath() const;
		const Vector3&				GetDirection() const;
		const Vector3&				GetTangent() const;
		Vector3						GetTargetPoint() const;
		Vector3						GetPrevTargetPoint() const;
		Vector3						GetStartingPosition() const;
		Vector3						GetStartingDirection() const;
		int							GetLapsCompleted() const;
		void							SetSpeedAtPoint(int point, float speed);

		static void					DrawPaths();
		static void					DrawPaths(bool i_bDrawPaths);

	protected:
		void							UpdateTargetPoint(const Vector3& position);
		void							UpdateDirectionToTarget(const Vector3& position);
		void							UpdateHeight(const Vector3& position);
		void							UpdateTangent();

		void							TestForwardProgress(const Vector3& i_vPosition);
		void							TestBackwardProgress(const Vector3& i_vPosition);

		int							GetNextEndpointIdx() const;
		int							GetPrevEndpointIdx() const;

		Vector3						GetPointOnCurve(float t);
		Vector3						GetPointOnSplineSegment(int segmentIdx, float t);
		Vector3						GetPointOnLineSegment(int segmentIdx, float t);

		void							DrawSplineSegment(int segmentIdx, int subdivisions);

	protected:
		const ts_SplinePath*		d_dblPath;
		int							d_targetPointIdx;
		Vector3						d_directionToTarget;
		Vector3						d_vPointOnPath;
		Vector3						d_tangent;
		float							d_height;
		float							d_speed;
		int							d_pointsPassed;
		float*						d_speedAtEndpoint;
		int							d_endpointCount;

		static bool					s_bDrawPaths;
};

// ===========================================================================
//	IMPLEMENTATION: PointPathFollower
// ---------------------------------------------------------------------------
//	FUNCTION: GetHeight

inline float PointPathFollower::GetHeight() const
{
	return d_height;
}

// ---------------------------------------------------------------------------
//	FUNCTION: GetSpeed

inline float PointPathFollower::GetSpeed() const
{
	return d_speed;
}

// ---------------------------------------------------------------------------
//	FUNCTION: GetClosestPointOnPath

inline const Vector3& PointPathFollower::GetClosestPointOnPath() const
{
	return d_vPointOnPath;
}

// ---------------------------------------------------------------------------
//	FUNCTION: GetDirection

inline const Vector3& PointPathFollower::GetDirection() const
{
	return d_directionToTarget;
}

// ---------------------------------------------------------------------------
//	FUNCTION: GetTangent

inline const Vector3& PointPathFollower::GetTangent() const
{
	return d_tangent;
}

// ---------------------------------------------------------------------------
//	FUNCTION: GetTargetPoint

inline Vector3 PointPathFollower::GetTargetPoint() const
{
	Vector3 point = (Vector3 &)d_dblPath->pointList[d_targetPointIdx];
	return point;
}

// ---------------------------------------------------------------------------
//	FUNCTION: GetPrevTargetPoint

inline Vector3 PointPathFollower::GetPrevTargetPoint() const
{
	Vector3 point = (Vector3 &)d_dblPath->pointList[GetPrevEndpointIdx()];
	return point;
}

// ---------------------------------------------------------------------------
//	FUNCTION: GetStartingPosition

inline Vector3 PointPathFollower::GetStartingPosition() const
{
	return (Vector3 &)d_dblPath->pointList[0];
}

// ===========================================================================
//	CLASS: LinearPath
// ===========================================================================

class LinearPath
{
	public:
	
		 LinearPath();
		 LinearPath(int i_iPointCount);
		~LinearPath();

		void					Create(int i_iPointCount);
		void					Destroy();

	public:

		const Vector3&		operator[](int i_iPointIndex) const;
		Vector3&				operator[](int i_iPointIndex);

		void					Calibrate();
		float					GetPathLength() const;
		Vector3				GetPointOnPath(float i_fT) const;
		Vector3				GetClosestPointOnPath(const Vector3& i_vPoint, float* o_fT) const;

	private:

		void					Initialize();

	private:

		float					d_fPathLength;
		int					d_iPointCount;
		Vector3*				d_vPointList;
};

#endif
