///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "hud/HudPCH.h"

#include "hud/ipanel/icluster.h"
#include "hud/ipanel/isys.h"

///////////////////////////////////////////////////////////////////////////////
// To create the cluster give a list of allowed directions by 1 << direction
// |'d together
///////////////////////////////////////////////////////////////////////////////
InstrumentCluster::InstrumentCluster()
{
	m_origin.Clear();
	for (int d = 0; d < DIRECTIONS; d++)
		m_dir[d] = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InstrumentCluster::InstrumentCluster(
const Vector2 &origin,
int allow)
{
	Setup(origin, allow);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InstrumentCluster::~InstrumentCluster()
{
	for (int d = 0; d < DIRECTIONS; d++)
		delete m_dir[d];
}

///////////////////////////////////////////////////////////////////////////////
// Initialize evertything
///////////////////////////////////////////////////////////////////////////////
bool InstrumentCluster::Setup(
const Vector2 &origin,
int allow)
{
	bool okay = true;
	m_origin = origin;
	for (int d = 0; d < DIRECTIONS; d++)
	{
		if ((allow & (1 << d)) == 0)
			m_dir[d] = NULL;
		else
		{
			m_dir[d] = new DynList<Instrument>(4, 2, false);
			ASSERT(m_dir[d]);
			if (!m_dir[d])
				okay = false;
		}
	}

	return okay;
}

///////////////////////////////////////////////////////////////////////////////
// Add/remove an instrument
///////////////////////////////////////////////////////////////////////////////
bool InstrumentCluster::Add(
Direction allowMove,
Instrument *instrument)
{
	ASSERT(instrument);
	ASSERT(allowMove != DIRECTIONS);
	ASSERT(m_dir[allowMove]);

	// Bad direction?
	if (!instrument || !m_dir[allowMove])
		return false;

	m_dir[allowMove]->Add(instrument);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool InstrumentCluster::Remove(
Instrument *instrument)
{
	for (int d = 0; d < DIRECTIONS; d++)
	{
		if (m_dir[d])
		{
			int index = m_dir[d]->Find(instrument);
			if (index >= 0)
			{
				m_dir[d]->Delete(index);
				return true;
			}
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
// Update the positions (once per frame, does not call the instrument update)
///////////////////////////////////////////////////////////////////////////////
void InstrumentCluster::Update(
float dt)
{
	float minX, maxX;
	minX = maxX = m_origin.X();

	float minY, maxY;
	minY = maxY = m_origin.Y();

	const float centerX = m_origin.X();
	const float centerY = m_origin.Y();
	const Vector2 screenMin = g_instruments.TopLeft();
	const float screenMinX = screenMin.X();
	const float screenMinY = screenMin.Y();
	const Vector2 screenMax = g_instruments.BottomRight();
	const float screenMaxX = screenMax.X();
	const float screenMaxY = screenMax.Y();

	// Start with the center
	if (m_dir[CENTER])
	{
		for (int i = 0; i < m_dir[CENTER]->Count(); i++)
		{
			Instrument *instrument = m_dir[CENTER]->Get(i);
			ASSERT(instrument);

			// Get the extents
			const Vector2 extents = instrument->Extents();
			const float halfWidth = extents.X() * 0.5f;
			const float halfHeight = extents.Y() * 0.5f;

			// compute extents
			float iMinX = centerX - halfWidth;
			float iMaxX = centerX + halfWidth;
  			float iMinY = centerY - halfHeight;
			float iMaxY = centerY + halfHeight;

			// Clip to screen (not 100% robust)
			if (iMinX < screenMinX)
			{
				iMaxX += screenMinX - iMinX;
				iMinX = screenMinX;
			}
			if (iMaxX > screenMaxX)
			{
				iMinX = Math::Max(iMinX - (iMaxX - screenMaxX), screenMinX);
				iMaxX = screenMaxX;
			}
			if (iMinY < screenMinY)
			{
				iMaxY += screenMinY - iMinY;
				iMinY = screenMinY;
			}
			if (iMaxY > screenMaxY)
			{
				iMinY = Math::Max(iMinY - (iMaxY - screenMaxY), screenMinY);
				iMaxY = screenMaxY;
			}

			// Keep track of maxrect
			if (iMinX < minX)
				minX = iMinX;
			if (iMaxX > maxX)
				maxX = iMaxX;
			if (iMinY < minY)
				minY = iMinY;
			if (iMaxY > maxY)
				maxY = iMaxY;

			// Set it in place
			instrument->Place(iMinX, iMinY);
		}
	}

	// Now do each arm
	Update(m_dir[NORTH], Vector2(centerX, minY), EXTENT_IGNORE, EXTENT_NEGATIVE);
	Update(m_dir[NORTHEAST], Vector2(maxX, minY), EXTENT_POSITIVE, EXTENT_NEGATIVE);
	Update(m_dir[EAST], Vector2(maxX, centerY), EXTENT_POSITIVE, EXTENT_IGNORE);
	Update(m_dir[SOUTHEAST], Vector2(maxX, maxY), EXTENT_POSITIVE, EXTENT_POSITIVE);
	Update(m_dir[SOUTH], Vector2(centerX, maxY), EXTENT_IGNORE, EXTENT_POSITIVE);
	Update(m_dir[SOUTHWEST], Vector2(minX, maxY), EXTENT_NEGATIVE, EXTENT_POSITIVE);
	Update(m_dir[WEST], Vector2(minX, centerY), EXTENT_NEGATIVE, EXTENT_IGNORE);
	Update(m_dir[NORTHWEST], Vector2(minX, minY), EXTENT_NEGATIVE, EXTENT_NEGATIVE);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
inline void InstrumentCluster::Update(
DynList<Instrument> *dir,
Vector2 start,
UseExtent xExtent,
UseExtent yExtent)
{
	// Get out quick!
	if (!dir || dir->Count() == 0)
		return;

	float currentX = start.X();
	float currentY = start.Y();
	for (int i = 0; i < dir->Count(); i++)
	{
		Instrument *instrument = dir->Get(i);
		ASSERT(instrument);

		// Query the extents
		const Vector2 extent = instrument->Extents();

		// Compute the placement for this instrument
		const float xPlacement = ComputePlacement(xExtent, start.X(), currentX, extent.X());
		const float yPlacement = ComputePlacement(yExtent, start.Y(), currentY, extent.Y());
		instrument->Place(xPlacement, yPlacement);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
inline float InstrumentCluster::ComputePlacement(
UseExtent useExtent,
float center,
float &current,
float extent)
{
	if (useExtent == EXTENT_IGNORE)
		return center;
	else if (useExtent == EXTENT_POSITIVE)
	{
		const float placement = current;
		current += extent;
		return placement;
	}
	else //if (useExtent == EXTENT_NEGATIVE)
	{
		const float placement = current - extent;
		current -= extent;
		return placement;
	}
}

