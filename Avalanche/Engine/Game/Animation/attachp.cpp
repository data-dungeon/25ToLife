///////////////////////////////////////////////////////////////////////////////
// Attach point managment
///////////////////////////////////////////////////////////////////////////////
#include "Game/GamePCH.h"

#define MAX_ATTACHPOINTNAME_LEN 16

// Straight form the DB_ATTACH_POINTS record
struct ts_AttachPoint
{
	Graphics4x4 m_matrix;
	char 			m_cAttachPointName[MAX_ATTACHPOINTNAME_LEN];
	char 			m_cParentBoneName[MAX_BONENAME_LEN];
	char 			pad[16];
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AttachPoints::AttachPoints()
{
	m_body = NULL;
	m_point = NULL;
	m_points = 0;
	m_boneID = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AttachPoints::~AttachPoints()
{
	delete [] m_boneID;
}

///////////////////////////////////////////////////////////////////////////////
// Initialize from the dbl (returns the number of points)
// If you don't specifiy a actor/body parent bones are never used
///////////////////////////////////////////////////////////////////////////////
int AttachPoints::Bind(
t_Handle fromDBL,
ts_Body *body)
{
	// Save off the body
	m_body = body;

	// Get the points
	GetPointsFromChunk(fromDBL);

	// Create the boneID array
	if (!m_body || !m_points)
	{
		delete [] m_boneID;
		m_boneID = NULL;
	}
	else
	{
		// Make a paralell array of parent bone id's
		m_boneID = new BoneID[m_points];
		ASSERT(m_boneID);
		if (!m_boneID)
			return 0;

		// Compute the bone ID's
		for (int p = 0; p < m_points; p++)
		{
			// Assume nothing
			m_boneID[p] = INVALID_BONEID;
	 		int ret = boneGetBoneID(m_body, &m_boneID[p], (char *)m_point[p].m_cParentBoneName);
	 		ASSERT(ret > 0);
		}
	}

	return m_points;
}

///////////////////////////////////////////////////////////////////////////////
// This works, but is not the "best" way to do this
///////////////////////////////////////////////////////////////////////////////
int AttachPoints::BindSlowBoatToChina(ts_Geometry &geometry, ts_Body *body)
{
	t_Handle dblHandle = dblGetHandleFromGeometry(&geometry);
	return Bind(dblHandle, body);
}

///////////////////////////////////////////////////////////////////////////////
// Find a named attach point (returns an attach point ID)
// < 0 means invalid
///////////////////////////////////////////////////////////////////////////////
AttachPointID AttachPoints::Find(
const char *pointName) const
{
	if (!pointName || !*pointName)
		return INVALID_ATTACH_POINT;

	// Linear search, hopefully they don't call this much
	for (int p = 0; p < m_points; p++)
	{
		if (strncmp(m_point[p].m_cAttachPointName, pointName, sizeof(m_point[p].m_cAttachPointName) - 1) == 0)
			return (AttachPointID)p;
	}

	// Couldn't find it
	return INVALID_ATTACH_POINT;
}

///////////////////////////////////////////////////////////////////////////////
// Get the attach point matrix directly ignoring the parent relationship
///////////////////////////////////////////////////////////////////////////////
const Graphics4x4 &AttachPoints::PeekMatrix(
AttachPointID point) const
{
	// Doh
	ASSERT(point == INVALID_ATTACH_POINT || (point >= 0 && point < m_points));
	if (point == INVALID_ATTACH_POINT || point < 0 || point >= m_points)
	{
		static Graphics4x4 i;
		i.Identity();
		return i;
	}

	return m_point[point].m_matrix;
}

///////////////////////////////////////////////////////////////////////////////
// Get the attach point matrix ignoring the parent relationship
// (will multiply through by parent if necessary)
///////////////////////////////////////////////////////////////////////////////
Graphics4x4 AttachPoints::ComputeMatrix(
AttachPointID point) const
{
	// Doh
	ASSERT(point == INVALID_ATTACH_POINT || (point >= 0 && point < m_points));
	if (point == INVALID_ATTACH_POINT || point < 0 || point >= m_points)
	{
		Graphics4x4 i;
		i.Identity();
		return i;
	}

	// Is there a parent?
	if (!m_boneID || m_boneID[point] == INVALID_BONEID)
		return m_point[point].m_matrix;

	// The heavy duty!
	ASSERT(m_body);
	Graphics4x4 parent;
 	boneGetCurrentBoneToModel(m_body, m_boneID[point], &parent);
	return m_point[point].m_matrix * parent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool AttachPoints::GetPointsFromChunk(
t_Handle dbl)
{
	// Weeee!
	int size = 0;
	m_point = (ts_AttachPoint *)dblGetChunkData(DB_ATTACH_POINTS, DEFAULT_CHUNK_KEY_STRING, dbl, &size);
	m_points = size / sizeof(ts_AttachPoint);

	return (m_point != NULL);
}
