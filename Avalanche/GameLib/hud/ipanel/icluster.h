///////////////////////////////////////////////////////////////////////////////
// Simple system for laying out an "Instrument Panel" i.e. HUD. Let's
// each instrument on a panel to be written and maintained separately.
///////////////////////////////////////////////////////////////////////////////
#ifndef ICLUSTER_H
#define ICLUSTER_H

#include "container\dynlist.h"

class Instrument;

///////////////////////////////////////
///////////////////////////////////////
class InstrumentCluster
{
public:
	enum Direction
	{
		CENTER = 0x0,
		NORTH,
		NORTHEAST,
		EAST,
		SOUTHEAST,
		SOUTH,
		SOUTHWEST,
		WEST,
		NORTHWEST,
		DIRECTIONS
	};

	// To create the cluster give a list of allowed directions by 1 << direction |'d together
	InstrumentCluster();
	InstrumentCluster(const Vector2 &origin, int allow = 0xff);
	~InstrumentCluster();

	// Initialize evertything
	bool Setup(const Vector2 &origin, int allow = 0xff);

	// Add/remove an instrument
	bool Add(Direction allowMove, Instrument *instrument);
	bool Remove(Instrument *instrument);

	// Update the positions (once per frame, does not call the instrument update)
	void Update(float dt);

private:
	// The root position of the cluster
	Vector2						m_origin;

	// The array of allowed directions
	DynList<Instrument>		*m_dir[DIRECTIONS];

	enum UseExtent
	{
		EXTENT_NEGATIVE,
		EXTENT_IGNORE,
		EXTENT_POSITIVE
	};
	inline void Update(DynList<Instrument> *dir, Vector2 start, UseExtent xExtent, UseExtent yExtent);
	inline float ComputePlacement(UseExtent useExtent, float center, float &current, float extent);
};

#endif
