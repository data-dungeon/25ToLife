///////////////////////////////////////////////////////////////////////////////
// Attach point managment
//
// This is kind of a "helper" class that should eventually built in to the
// SOM/Instance. But for now, you have to manage it yourself and hook it
// up using dbl handle.
///////////////////////////////////////////////////////////////////////////////
#ifndef ATTACHP_H
#define ATTACHP_H

#include "game/animation/bone.h"

// Straight form the DB_ATTACH_POINTS record
struct ts_AttachPoint;

// Wee! abstraction good!
// < 0 means invalid
typedef int AttachPointID;
#define INVALID_ATTACH_POINT	((AttachPointID)-1)

class AttachPoints
{
public:
	AttachPoints();
	~AttachPoints();

	// Initialize from the dbl (returns the number of points)
	// If you don't specifiy a actor/body parent bones are never used
	int Bind(t_Handle fromDBL, ts_Body *body = NULL);

	// This works, but is not the "best" way to do this
	int BindSlowBoatToChina(ts_Geometry &geometry, ts_Body *body = NULL);

	// Find a named attach point (returns an attach point ID)
	// < 0 means invalid
	AttachPointID Find(const char *pointName) const;

	// Get the attach point matrix directly ignoring the parent relationship
	const Graphics4x4 &PeekMatrix(AttachPointID point) const;

	// Get the attach point matrix ignoring the parent relationship (will multiply through by parent if necessary)
	Graphics4x4 ComputeMatrix(AttachPointID point) const;

	static void FixupChunk(void *data, int dataSize);

private:
	// Our body! (can be NULL)
	ts_Body				*m_body;

	// The array!
	ts_AttachPoint		*m_point;
	int					m_points;
	bool GetPointsFromChunk(t_Handle dbl);

	// the bone ID's (if bound)
	BoneID				*m_boneID;
};

#endif
