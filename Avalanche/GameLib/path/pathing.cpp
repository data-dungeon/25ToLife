/******************* includes ****************************/

#include "geomUtil/GeomUtilPCH.h"





// ===========================================================================
//	IMPLEMENTATION: Global Functions
// ---------------------------------------------------------------------------
//	FUNCTION: GetSplinePath

ts_SplinePath* GetSplinePath(const char* i_sPathName)
{
	ts_SplinePath* path = 0;
	
	for (int i = 0; i < gbl_paths->pathCount; i++)
	{
		if (!stricmp(i_sPathName, gbl_paths->pathList[i]->name))
		{
			path = gbl_paths->pathList[i];
			break;
		}
	}

	return path;
}

// ===========================================================================
//	IMPLEMENTATION: Line
// ---------------------------------------------------------------------------
//	CONSTRUCTOR

Line::Line(const Vector3& a, const Vector3& b)
{
	this->a = a;
	this->b = b;
}

// ---------------------------------------------------------------------------
//	FUNCTION: ComputeDistanceFrom

float Line::ComputeDistanceFrom(const Vector3& point)
{
	return ComputeDistanceFrom(a, b, point);
}

// ---------------------------------------------------------------------------
//	FUNCTION: ComputeDistanceFromSegment

float Line::ComputeDistanceFromSegment(const Vector3& point)
{
	return ComputeDistanceFromSegment(a, b, point);
}

// ---------------------------------------------------------------------------
//	STATIC FUNCTION: ComputeDistanceFrom

float Line::ComputeDistanceFrom(const Vector3& a, const Vector3& b, const Vector3& point)
{
	Vector3 ab = b - a;
	Vector3 ac = point - a;

	float t = Vector3::Dot(ac, ab) / ab.LengthSquared();

	Vector3 c = (ab * t) + a;
	Vector3 cp = point - c;

	return cp.Length();
}

// ---------------------------------------------------------------------------
//	STATIC FUNCTION: ComputeDistanceFromSegment

float Line::ComputeDistanceFromSegment(const Vector3& a, const Vector3& b, const Vector3& point)
{
	Vector3 ab = b - a;
	Vector3 ac = point - a;

	float t = Vector3::Dot(ac, ab) / ab.LengthSquared();

	Vector3 c;
	
	if (t >= 1.0f)
		c = b;

	else if (t <= 0.0f)
		c = a;
		
	else	
		c = (ab * t) + a;

	Vector3 cp = point - c;

	return cp.Length();
}

// ---------------------------------------------------------------------------
//	STATIC FUNCTION: GetClosestPointOnLine

Vector3 Line::GetClosestPointOnLine(const Vector3& i_vEndPointA, const Vector3& i_vEndPointB, const Vector3& i_vPointInSpace, float* o_fT)
{
	Vector3 vAB = i_vEndPointB - i_vEndPointA;
	Vector3 vAC = i_vPointInSpace - i_vEndPointA;

	float fT = Vector3::Dot(vAC, vAB) / vAB.LengthSquared();

	Vector3 vPointOnLine = (vAB * fT) + i_vEndPointA;

	if (o_fT)
		*o_fT = fT;

	return vPointOnLine;
}

// ---------------------------------------------------------------------------
//	STATIC FUNCTION: GetClosestPointOnSegment

Vector3 Line::GetClosestPointOnSegment(const Vector3& i_vEndPointA, const Vector3& i_vEndPointB, const Vector3& i_vPointInSpace, float* o_fT)
{
	Vector3 vAB = i_vEndPointB - i_vEndPointA;
	Vector3 vAC = i_vPointInSpace - i_vEndPointA;

	float fT = Vector3::Dot(vAC, vAB) / vAB.LengthSquared();

	Vector3 vPointOnLine;
	
	if (fT >= 1.0f)
	{
		fT = 1.0f;
		vPointOnLine = i_vEndPointB;
	}

	else if (fT <= 0.0f)
	{
		fT = 0.0f;
		vPointOnLine = i_vEndPointA;
	}

	else
		vPointOnLine = (vAB * fT) + i_vEndPointA;

	if (o_fT)
		*o_fT	= fT;

	return vPointOnLine;
}



// ===========================================================================
//	IMPLEMENTATION: Plane
// ---------------------------------------------------------------------------
//	CONSTRUCTOR

Plane::Plane(const Vector3& n, const Vector3& p)
{
	this->n = n;
	this->p = p;
}

// ---------------------------------------------------------------------------
//	FUNCTION: GetDistanceFromPoint

float Plane::GetDistanceFromPoint(const Vector3& point)
{
	return GetDistanceFromPoint(n, p, point);
}

// ---------------------------------------------------------------------------
//	STATIC FUNCTION: GetDistanceFromPoint

float Plane::GetDistanceFromPoint(const Vector3& n, const Vector3& p, const Vector3& point)
{
	Vector3 a = p;
	Vector3 b = p + n;

	Vector3 ab = b - a;
	Vector3 ac = point - a;

	float length = Vector3::Dot(ac, ab);

	return length;
}

// ===========================================================================
//	IMPLEMENTATION: Spline
// ---------------------------------------------------------------------------
//	CONSTRUCTOR

Spline::Spline(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d)
{
	this->a = a;
	this->b = b;
	this->c = c;
	this->d = d;
}

// ---------------------------------------------------------------------------
//	FUNCTION: GetPointAndTangent

void Spline::GetPointAndTangent(float t, Vector3* point, Vector3* tangent)
{
	GetPointAndTangent(t, a, b, c, d, point, tangent);
}

// ---------------------------------------------------------------------------
//	STATIC FUNCTION: GetPointAndTangent

void Spline::GetPointAndTangent(float t, const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d, Vector3* point, Vector3* tangent)
{
	if (t < 0.0f)
		t = 0.0f;

	else if (t > 1.0f)
		t = 1.0f;

	Vector3 e, f, g, h, i, j;

	e = (b - a) * t + a;
	f = (c - b) * t + b;
	g = (d - c) * t + c;

	h = (f - e) * t + e;
	i = (g - f) * t + f;

	j = i - h;

	if (tangent)
		*tangent = j.Normalized();

	if (point)
		*point = j * t + h;
}

// ===========================================================================
//	IMPLEMENTATION: Cone
// ---------------------------------------------------------------------------
//	CONSTRUCTOR

Cone::Cone(const Vector3& i_vApex, const Vector3& i_vBase, float i_fBaseRadius)
{
	vApex			= i_vApex;
	vBase			= i_vBase;
	fBaseRadius	= i_fBaseRadius;
}

// ---------------------------------------------------------------------------
//	FUNCTION: IsInside

bool Cone::IsInside(const Vector3& i_vPointInSpace)
{
	return IsInside(vApex, vBase, fBaseRadius, i_vPointInSpace);
}

// ---------------------------------------------------------------------------
//	STATIC FUNCTION: IsInside

bool Cone::IsInside(const Vector3& i_vApex, const Vector3& i_vBase, float i_fBaseRadius,
	const Vector3& i_vPointInSpace)
{
	Vector3 vPointOnAxis;
	float fT;

	vPointOnAxis = Line::GetClosestPointOnLine(i_vApex, i_vBase, i_vPointInSpace, &fT);

	float fDistanceFromAxis = (i_vPointInSpace - vPointOnAxis).Length();
	float fRadiusAtT = i_fBaseRadius * fT;

	if (fDistanceFromAxis < fRadiusAtT)
		return true;

	return false;
}

// ===========================================================================
//	IMPLEMENTATION: PointPathFollower

bool PointPathFollower::s_bDrawPaths = false;

// ---------------------------------------------------------------------------
//	CONSTRUCTOR

PointPathFollower::PointPathFollower()
{
	d_dblPath			= 0;
	d_speedAtEndpoint	= 0;
	d_endpointCount	= 0;

	Reset();
}

// ---------------------------------------------------------------------------
//	DESTRUCTOR

PointPathFollower::~PointPathFollower()
{
	delete d_speedAtEndpoint;
}

// ---------------------------------------------------------------------------
//	FUNCTION: Initialize

void PointPathFollower::Initialize(const ts_SplinePath* dblPath, float defaultSpeed)
{
	ASSERT(dblPath);

	d_dblPath = dblPath;
	d_endpointCount = (dblPath->pointCount - 1) / 3 + 1;

	delete d_speedAtEndpoint;
	d_speedAtEndpoint = new float[d_endpointCount];
	ASSERT(d_speedAtEndpoint);

	for (int i = 0; i < d_endpointCount; i++)
		d_speedAtEndpoint[i] = defaultSpeed;

	Reset();
}

// ---------------------------------------------------------------------------
//	FUNCTION: Update

void PointPathFollower::Update(const Vector3& position)
{
	ASSERT(d_dblPath);

	UpdateTargetPoint(position);
	UpdateDirectionToTarget(position);
	UpdateHeight(position);
//	UpdateTangent();

	if (s_bDrawPaths)
		Draw();
}

// ---------------------------------------------------------------------------
//	FUNCTION: Reset

void PointPathFollower::Reset()
{
	d_targetPointIdx	= 3;
	d_height				= 0.0f;
	d_pointsPassed		= 0;

	d_directionToTarget.Set(0.0f, 0.0f, 1.0f);
}

// ---------------------------------------------------------------------------
//	FUNCTION: Draw

void PointPathFollower::Draw()
{
/*
	int iPointIdx = GetNextEndpointIdx();

	Vector3 vPoint, vDirection;
	
	vPoint = (Vector3&)d_dblPath->pointList[iPointIdx];
	vDirection = (Vector3&)d_dblPath->pointList[iPointIdx + 1];
	vDirection -= vPoint;

	DrawUtility::Normal(vPoint, vDirection);
*/
	int prevTargetPointIdx = GetPrevEndpointIdx();
	int nextTargetPointIdx = GetNextEndpointIdx();

	DrawSplineSegment(d_targetPointIdx, 16);
	DrawSplineSegment(nextTargetPointIdx, 16);
}

// ---------------------------------------------------------------------------
//	PROTECTED FUNCTION: UpdateTargetPoint

void PointPathFollower::UpdateTargetPoint(const Vector3& position)
{
	TestForwardProgress(position);
	TestBackwardProgress(position);
	return;

	/*
	int prevTargetPointIdx = GetPrevEndpointIdx();

	Vector3 prevTargetPoint = d_dblPath->pointList[prevTargetPointIdx];
	Vector3 currTargetPoint = d_dblPath->pointList[d_targetPointIdx];

	Vector3 toPrevTarget = prevTargetPoint;
	toPrevTarget -= currTargetPoint;

	Vector3 toPosition = position;
	toPosition -= currTargetPoint;

	if (Vector3::Dot(toPrevTarget, toPosition) <= 0.0f)
	{
		d_targetPointIdx = GetNextEndpointIdx();
		d_pointsPassed++;
		return;
	}

	Vector3 toCurrTarget = currTargetPoint;
	toCurrTarget -= prevTargetPoint;

	toPosition = position;
	toPosition -= prevTargetPoint;

	if (Vector3::Dot(toCurrTarget, toPosition) < 0.0f)
	{
		d_targetPointIdx = prevTargetPointIdx;
		d_pointsPassed--;
	}
	*/
}

// ---------------------------------------------------------------------------
//	PROTECTED FUNCTION: UpdateDirectionToTarget

void PointPathFollower::UpdateDirectionToTarget(const Vector3& position)
{
	d_directionToTarget = d_dblPath->pointList[d_targetPointIdx];
	d_directionToTarget -= position;
	d_directionToTarget.Normalize();
}

// ---------------------------------------------------------------------------
//	PROTECTED FUNCTION: UpdateHeight

void PointPathFollower::UpdateHeight(const Vector3& position)
{
	// compute path height.

	Plane a, b;

	a.p = d_dblPath->pointList[d_targetPointIdx - 3];
	a.n = d_dblPath->pointList[d_targetPointIdx - 2];
	a.n = (a.n - a.p).Normalized();

	b.p = d_dblPath->pointList[d_targetPointIdx];
	b.n = d_dblPath->pointList[d_targetPointIdx - 1];
	b.n = (b.n - b.p).Normalized();

	float distanceFromA = a.GetDistanceFromPoint(position);
	float distanceFromB = b.GetDistanceFromPoint(position);

	float t = distanceFromA / (distanceFromA + distanceFromB);

//	Vector3 p;// = GetPointOnSplineSegment(d_targetPointIdx, t);

	Spline::GetPointAndTangent
	(
		t,
		d_dblPath->pointList[d_targetPointIdx - 3],
		d_dblPath->pointList[d_targetPointIdx - 2],
		d_dblPath->pointList[d_targetPointIdx - 1],
		d_dblPath->pointList[d_targetPointIdx - 0],
		&d_vPointOnPath,
		&d_tangent
	);

	d_height = d_vPointOnPath.y;

	// compute path speed.

	int prevEndpointIdx = GetPrevEndpointIdx();

	float prevEndpointSpeed = d_speedAtEndpoint[GetPrevEndpointIdx() / 3];
	float nextEndpointSpeed = d_speedAtEndpoint[d_targetPointIdx / 3];

	d_speed = (nextEndpointSpeed - prevEndpointSpeed) * t + prevEndpointSpeed;

/*
	int prevTargetPointIdx = GetPrevEndpointIdx();

	Vector3 a, b;

	a = d_dblPath->pointList[prevTargetPointIdx];
	b = d_dblPath->pointList[d_targetPointIdx];

	Vector3 ab = b - a;
	Vector3 ac = position - a;

	float t = Vector3::Dot(ac, ab) / ab.LengthSquared();

	Vector3 p;

	if (t <= 0.0f)
		p = a;

	else if (t >= 1.0f)
		p = b;

	else
//		p = (ab * t) + a;
		p = GetPointOnSplineSegment(d_targetPointIdx, t);

	d_height = p.y;
*/
}

// ---------------------------------------------------------------------------
//	PROTECTED FUNCTION: UpdateTangent

void PointPathFollower::UpdateTangent()
{
	Vector3 a, b;

	a = d_dblPath->pointList[GetPrevEndpointIdx()];
	b = d_dblPath->pointList[d_targetPointIdx];

	d_tangent = (b - a).Normalized();
}

// ---------------------------------------------------------------------------
//	PROTECTED FUNCTION: TestForwardProgress

void PointPathFollower::TestForwardProgress(const Vector3& i_vPosition)
{
	Vector3 vEndPoint = (Vector3&)d_dblPath->pointList[d_targetPointIdx];

	Vector3 vTangent;
	{
		vTangent = (Vector3&)d_dblPath->pointList[d_targetPointIdx - 1];
		vTangent -= vEndPoint;
	}

	Vector3 vP = i_vPosition - vEndPoint;

	if (Vector3::Dot(vTangent, vP) < 0.0f)
	{
		d_targetPointIdx = GetNextEndpointIdx();
		d_pointsPassed++;
		TestForwardProgress(i_vPosition);
	}
}

// ---------------------------------------------------------------------------
//	PROTECTED FUNCTION: TestBackwardProgress

void PointPathFollower::TestBackwardProgress(const Vector3& i_vPosition)
{
	Vector3 vEndPoint = (Vector3&)d_dblPath->pointList[GetPrevEndpointIdx()];

	Vector3 vTangent;
	{
		vTangent = (Vector3&)d_dblPath->pointList[d_targetPointIdx - 2];
		vTangent -= vEndPoint;
	}

	Vector3 vP = i_vPosition - vEndPoint;

	if (Vector3::Dot(vTangent, vP) < 0.0f)
	{
		d_targetPointIdx = GetPrevEndpointIdx();
		d_pointsPassed--;
		TestBackwardProgress(i_vPosition);
	}
}

// ---------------------------------------------------------------------------
//	PROTECTED FUNCTION: GetNextEndpointIdx

int PointPathFollower::GetNextEndpointIdx() const
{
	int nextEndpointIdx = d_targetPointIdx + 3;

	if (nextEndpointIdx > d_dblPath->pointCount)
		nextEndpointIdx = 3;

	return nextEndpointIdx;
}

// ---------------------------------------------------------------------------
//	PROTECTED FUNCTION: GetPrevEndpointIdx

int PointPathFollower::GetPrevEndpointIdx() const
{
	int prevEndpointIdx = d_targetPointIdx - 3;

	if (prevEndpointIdx < 3)
		prevEndpointIdx = d_dblPath->pointCount - 1;

	return prevEndpointIdx;
}

// ---------------------------------------------------------------------------
//	PROTECTED FUNCTION: GetPointOnCurve

Vector3 PointPathFollower::GetPointOnCurve(float t)
{
	return GetPointOnSplineSegment(d_targetPointIdx, t);
}

// ---------------------------------------------------------------------------
//	PROTECTED FUNCTION: GetPointOnSplineSegment

Vector3 PointPathFollower::GetPointOnSplineSegment(int segmentIdx, float t)
{
	Vector3 point;

	Spline::GetPointAndTangent(t, 
										d_dblPath->pointList[segmentIdx - 3], 
										d_dblPath->pointList[segmentIdx - 2],
										d_dblPath->pointList[segmentIdx - 1], 
										d_dblPath->pointList[segmentIdx - 0], 
										&point, 
										0);

	return point;
}

// ---------------------------------------------------------------------------
//	PROTECTED FUNCTION: GetPointOnLineSegment

Vector3 PointPathFollower::GetPointOnLineSegment(int segmentIdx, float t)
{
	Vector3 a = d_dblPath->pointList[segmentIdx - 3];
	Vector3 b = d_dblPath->pointList[segmentIdx];

	Vector3 ab = b - a;
	Vector3 p;

	if (t <= 0.0f)
		p = a;

	else if (t >= 1.0f)
		p = b;

	else
		p = (ab * t) + a;

	return p;
}

// ---------------------------------------------------------------------------
//	PROTECTED FUNCTION: DrawSplineSegment

void PointPathFollower::DrawSplineSegment(int segmentIdx, int subdivisions)
{
	Vector3 a, b;

	float step = 1.0f / subdivisions;
	float t = 0.0f;

	a = d_dblPath->pointList[segmentIdx - 3];

	for (int i = 1; i < subdivisions; i++)
	{
		t += step;
		b = GetPointOnSplineSegment(segmentIdx, t);
		DrawUtility::Line(a, b, DrawUtility::YELLOW);
		a = b;
	}

	b = d_dblPath->pointList[segmentIdx];
	DrawUtility::Line(a, b, DrawUtility::YELLOW);
}

// ---------------------------------------------------------------------------
//	FUNCTION: GetStartingDirection

Vector3 PointPathFollower::GetStartingDirection() const
{
	Vector3 direction = d_dblPath->pointList[1];
	direction -= d_dblPath->pointList[0];
	direction.Normalize();

	return direction;
}

// ---------------------------------------------------------------------------
//	FUNCTION: GetLapsCompleted

int PointPathFollower::GetLapsCompleted() const
{
	int pointsInLap = (d_dblPath->pointCount - 1) / 3;
	int lapsCompleted = d_pointsPassed / pointsInLap;

	return lapsCompleted;
}

// ---------------------------------------------------------------------------
//	FUNCTION: SetSpeedAtPoint

void PointPathFollower::SetSpeedAtPoint(int point, float speed)
{
	if (point < 0 || point >= d_endpointCount)
		return;

	ASSERT(speed >= 1.0f); // speed must be greater than 1 meter/sec.

	d_speedAtEndpoint[point] = speed;
}

// ===========================================================================
//	IMPLEMENTATION: LinearPath
// ===========================================================================
// ---------------------------------------------------------------------------
//	CONSTRUCTOR

LinearPath::LinearPath()
{
	Initialize();
}

// ---------------------------------------------------------------------------
//	CONSTRUCTOR

LinearPath::LinearPath(int i_iPointCount)
{
	Create(i_iPointCount);
}

// ---------------------------------------------------------------------------
//	DESTRUCTOR

LinearPath::~LinearPath()
{
	Destroy();
}

// ---------------------------------------------------------------------------
//	FUNCTION: Create
//
//	Object is not valid unless Create()'d.

void LinearPath::Create(int i_iPointCount)
{
	ASSERT(i_iPointCount >= 2);

	Destroy();

	d_iPointCount = i_iPointCount;
	d_vPointList = new Vector3[i_iPointCount];

	ASSERT(d_vPointList); // memory allocation failed!
}

// ---------------------------------------------------------------------------
//	FUNCTION: Destroy
//
//	Object clean-up done here; destructor should call this function.

void LinearPath::Destroy()
{
	delete[] d_vPointList;
	Initialize();
}

// ---------------------------------------------------------------------------
//	(PRIVATE) FUNCTION: Initialize

void LinearPath::Initialize()
{
	d_iPointCount = 0;
	d_vPointList = 0;
}

// ---------------------------------------------------------------------------
//	OPERATOR [] const

const Vector3& LinearPath::operator[](int i_iPointIndex) const
{
	ASSERT( (i_iPointIndex >= 0) && (i_iPointIndex < d_iPointCount) ); // i_iPointIndex is out-of-range!
	return d_vPointList[i_iPointIndex];
}

// ---------------------------------------------------------------------------
//	OPERATOR []

Vector3& LinearPath::operator[](int i_iPointIndex)
{
	ASSERT( (i_iPointIndex >= 0) && (i_iPointIndex < d_iPointCount) ); // i_iPointIndex is out-of-range!
	return d_vPointList[i_iPointIndex];
}

// ---------------------------------------------------------------------------
//	FUNCTION: Calibrate
//
//	After the path points have been set, this function must be called in order
//	to insure the normalized 0.0 to 1.0 input produces the correct point on
//	the path.

void LinearPath::Calibrate()
{
	ASSERT(d_iPointCount >= 2); // this object has not been Create()'d!

	d_fPathLength = 0.0f;

	for (int i = 1; i < d_iPointCount; i++)
	{
		Vector3 v = d_vPointList[i];
		v -= d_vPointList[i - 1];
		
		float fSegmentLength = v.Length();
		ASSERT(fSegmentLength > 0.0f); // these 2 consecutive points on the path are the same point!

		d_fPathLength += v.Length();
	}
}

// ---------------------------------------------------------------------------
//	FUNCTION: GetPathLength

float LinearPath::GetPathLength() const
{
	return d_fPathLength;
}

// ---------------------------------------------------------------------------
//	FUNCTION: GetPointOnPath
//
//	i_fT is a normalized value between 0.0 (beginning of path) and 1.0 (end of path).

Vector3 LinearPath::GetPointOnPath(float i_fT) const
{
	ASSERT( (i_fT >= 0.0f) && (i_fT <= 1.0f) ); // i_fT is out-of-range!

	ASSERT(d_iPointCount >= 2);	// this object has not been Create()'d!
	ASSERT(d_fPathLength > 0.0f);	// this object has not been Calibrate()'d!

	Vector3 vPointOnPath = d_vPointList[0];

	if (i_fT == 1.0f)
		return d_vPointList[d_iPointCount - 1];

	float fLengthAtT = d_fPathLength * i_fT;
	float fLength = 0.0f;

	for (int i = 1; i < d_iPointCount; i++)
	{
		Vector3 v = d_vPointList[i];
		v -= d_vPointList[i - 1];

		float fSegmentLength = v.Length();
		fLength += fSegmentLength;

		if (fLength > fLengthAtT)
		{
			float fT = (fLength - fLengthAtT) / fSegmentLength;
			fT = 1.0f - fT;

			vPointOnPath = v;
			vPointOnPath *= fT;
			vPointOnPath += d_vPointList[i];

			break;
		}
	}

	return vPointOnPath;
}

// ---------------------------------------------------------------------------
//	FUNCTION: GetClosestPointOnPath

Vector3 LinearPath::GetClosestPointOnPath(const Vector3& i_vPointInSpace, float* o_fT) const
{
	ASSERT(d_iPointCount >= 2);	// this object has not been Create()'d!
	ASSERT(d_fPathLength > 0.0f);	// this object has not been Calibrate()'d!

	if (o_fT)
		*o_fT = 0.0f;

	float fDistanceFromPath, fPathT;
	Vector3 vPointOnPath;
	
	vPointOnPath = Line::GetClosestPointOnSegment(d_vPointList[0], d_vPointList[1], i_vPointInSpace, &fPathT);
	fDistanceFromPath = Distance(i_vPointInSpace, vPointOnPath) + 1.0f;

	float fLength = 0.0f;

	for (int i = 1; i < d_iPointCount; i++)
	{
		float fSegmentLength;
		
		if (o_fT)
			fSegmentLength = Distance(d_vPointList[i - 1], d_vPointList[i]);

		float fDistanceFromSegment, fSegmentT;
		Vector3 vPointOnSegment;

		 vPointOnSegment = Line::GetClosestPointOnSegment(d_vPointList[i - 1], d_vPointList[i], i_vPointInSpace, &fSegmentT);
		 fDistanceFromSegment = Distance(i_vPointInSpace, vPointOnSegment);

		if (fDistanceFromSegment < fDistanceFromPath)
		{
			fDistanceFromPath = fDistanceFromSegment;
			vPointOnPath = vPointOnSegment;
		
			if (o_fT)
			{	
				*o_fT = (fSegmentT * fSegmentLength) + fLength;
				*o_fT /= d_fPathLength;
			}
		}

		fLength += fSegmentLength;
	}

	return vPointOnPath;
}

// ---------------------------------------------------------------------------
//	STATIC FUNCTION: DrawPaths
//
//	This function toggles the drawing of paths on or off.

void PointPathFollower::DrawPaths()
{
	DrawPaths(!s_bDrawPaths);
}

// ---------------------------------------------------------------------------
//	STATIC FUNCTION: DrawPaths
//
//	This function explicitly turns on the drawing on or off.

void PointPathFollower::DrawPaths(bool i_bDrawPaths)
{
	s_bDrawPaths = i_bDrawPaths;
}
