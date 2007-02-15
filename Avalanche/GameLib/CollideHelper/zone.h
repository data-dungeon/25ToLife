//***************************************************************************
// A generalized "zone" with helper implementations
//***************************************************************************
#ifndef ZONE
#define ZONE

#include "collide/xzrect.h"
#include "collide/obb.h"

class Zone
{
public:
	Zone(int useID) 
		{ id = useID; }
	virtual ~Zone() {}

	// What is our zoneID
	inline int ID()
		{ return id; }

	// Does this zone intersect this Rect?
	virtual XZRect Extents() = 0;

	// Does this zone intersect this rect?
	virtual bool Intersects(const XZRect &rect) = 0;

	// Does this zone contain this point?
	virtual bool Contains(Vector3CRef p) = 0;

	// Is this zone active?
	virtual bool Active() = 0;

	// Debugging draw call
	virtual void Draw() {}

protected:
	int				id;
};

class ZoneSphere : public Zone
{
public:
	ZoneSphere(int id, Vector3CRef position, float size);
	virtual ~ZoneSphere() {}

	// Does this zone intersect this Rect?
	XZRect Extents();

	// Does this zone intersect this rect?
	bool Intersects(const XZRect &rect);

	// Does this zone contain this point?
	bool Contains(Vector3CRef p);

	// Debugging draw call
	void Draw();

protected:
	Vector3		center;
	float			radius;

	inline bool EdgeIntersects(float x0, float z0, float x1, float z1);
	inline bool Inside(float x, float z);
};

class ZoneOBB : public Zone
{
public:
	ZoneOBB(int id, OBB &init);
	virtual ~ZoneOBB() {}

	// Does this zone intersect this Rect?
	XZRect Extents();

	// Does this zone intersect this rect?
	bool Intersects(const XZRect &rect);

	// Does this zone contain this point?
	bool Contains(Vector3CRef p);

	// Debugging draw call
	void Draw();

protected:
	OBBInstance obb;
};

#endif
