///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "hud/HudPCH.h"

#include "hud/ipanel/isys.h"
#include "hud/ipanel/ipanel.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InstrumentPanel::InstrumentPanel(
const char *panelName) :
m_groups(true)
{
	ASSERT(panelName);
	ASSERT(strlen(panelName) > 0 && strlen(panelName) < sizeof(m_name) - 1);
	strcpy(m_name, panelName);

	// No clusters yet
	for (int c = 0; c < CLUSTERS; c++)
		m_cluster[c] = NULL;

	m_transitionType = -1;
	m_transitionTime = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InstrumentPanel::~InstrumentPanel()
{
	for (int c = 0; c < CLUSTERS; c++)
 		delete m_cluster[c];

	// Group container cleans up instruments
}

///////////////////////////////////////////////////////////////////////////////
// Add an instrument (I own it now, you new, I delete)
///////////////////////////////////////////////////////////////////////////////
bool InstrumentPanel::Add(
Cluster cluster,
InstrumentCluster::Direction allowMove,
const char *groupName,
Instrument *instrument)
{
	ASSERT(cluster != CLUSTERS);
	ASSERT(groupName && *groupName);
	ASSERT(instrument);

	// Duh
	if (!instrument)
		return false;

	// Get our group
	InstrumentGroup *group = FindGroup(groupName);
	if (!group)
		group = CreateGroup(groupName);
	if (!group)
		return false;

	// Now add it to the cluster
	InstrumentCluster *instrumentCluster = GetCluster(cluster);
	if (!instrumentCluster)
		return false;

	// Add the intstrument to the group & cluster
	group->Add(instrument);
	return instrumentCluster->Add(allowMove, instrument);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool InstrumentPanel::Remove(
const char *groupName,
const char *instrumentName)
{
	// Get our group
	InstrumentGroup *group = FindGroup(groupName);
	if (!group)
		return false;

	// Find the instrument in question
	Instrument *instrument = group->Find(instrumentName);
	if (!instrument)
		return false;

	// Get rid it of it in the clusters (yuck this is slow)
	for (int c = 0; c < CLUSTERS; c++)
	{
		if (m_cluster[c] && m_cluster[c]->Remove(instrument))
			break;
	}

	// Kill it in the group
	group->Remove(instrument);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool InstrumentPanel::Show(
const char *groupName,
const char *instrumentName,
bool show)
{
	// Get our group
	InstrumentGroup *group = FindGroup(groupName);
	if (!group)
		return false;

	// Find the instrument in question
	Instrument *instrument = group->Find(instrumentName);
	if (!instrument)
		return false;

	return instrument->Show(show);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool InstrumentPanel::Activate(
const char *groupName,
const char *instrumentName,
bool activate)
{
	// Get our group
	InstrumentGroup *group = FindGroup(groupName);
	if (!group)
		return false;

	// Find the instrument in question
	Instrument *instrument = group->Find(instrumentName);
	if (!instrument)
		return false;

	return instrument->Activate(activate);
}

///////////////////////////////////////////////////////////////////////////////
// Group operatrions
///////////////////////////////////////////////////////////////////////////////
bool InstrumentPanel::RemoveGroup(
const char *groupName)
{
	// Get our group
	InstrumentGroup *group = FindGroup(groupName);
	if (!group)
		return false;

	// Kill it!
	m_groups.Remove(group);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool InstrumentPanel::ShowGroup(
const char *groupName,
bool show,
float transitionTime)
{
	// Get our group
	InstrumentGroup *group = FindGroup(groupName);
	if (!group)
		return false;

	// Kill it!
	return group->Show(show, transitionTime);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool InstrumentPanel::ActivateGroup(
const char *groupName,
bool activate)
{
	// Get our group
	InstrumentGroup *group = FindGroup(groupName);
	if (!group)
		return false;

	// Kill it!
	return group->Activate(activate);
}

///////////////////////////////////////////////////////////////////////////////
// All!
///////////////////////////////////////////////////////////////////////////////
bool InstrumentPanel::ShowAll(
bool show,
float transitionTime)
{
	InstrumentGroup *group = m_groups.Head();
	while (group)
	{
		group->Show(show, transitionTime);
		group = group->next;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// All!
///////////////////////////////////////////////////////////////////////////////
bool InstrumentPanel::ActivateAll(
bool activate)
{
	InstrumentGroup *group = m_groups.Head();
	while (group)
	{
		group->Activate(activate);
		group = group->next;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InstrumentPanel::SetDefaultTransition(
float transitionTime,
int transition)
{
	m_transitionTime = transitionTime;
	m_transitionType = transition;
}

///////////////////////////////////////////////////////////////////////////////
// Call this once per frame!
///////////////////////////////////////////////////////////////////////////////
void InstrumentPanel::Update(float dt)
{
	// First update the clusters
	for (int c = 0; c < CLUSTERS; c++)
	{
		if (m_cluster[c])
			m_cluster[c]->Update(dt);
	}

	// Now do the instruments
	InstrumentGroup *group = m_groups.Head();
	while (group)
	{
		group->Update(dt);
		group = group->next;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InstrumentGroup *InstrumentPanel::FindGroup(
const char *groupName)
{
	// Linear search
	InstrumentGroup *group = m_groups.Head();
	while (group)
	{
		if (strcmpi(group->Name(), groupName) == 0)
			return group;
		group = group->next;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InstrumentGroup *InstrumentPanel::CreateGroup(
const char *groupName)
{
	// Create it
	InstrumentGroup *group = new InstrumentGroup(groupName);
	ASSERT(group);
	if (group)
	{
		if (!Math::Zero(m_transitionTime) || m_transitionType >= 0)
			group->SetTransition(m_transitionTime, m_transitionType);
		m_groups.AddTail(group);
	}

	return group;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InstrumentCluster *InstrumentPanel::GetCluster(
Cluster cluster)
{
	ASSERT(cluster != CLUSTERS);
	if (m_cluster[cluster])
		return m_cluster[cluster];

	// Screen info
	const Vector2 center = g_instruments.ScreenSize() * 0.5f;
	const Vector2 min = g_instruments.TopLeft();
	const Vector2 max = g_instruments.BottomRight();

	// Figure out the origin and allow mask
	Vector2 origin;
	int allow = (1 << InstrumentCluster::CENTER);
	switch (cluster)
	{
		case TOPLEFT:
			allow |= ((1 << InstrumentCluster::EAST) | (1 << InstrumentCluster::SOUTHEAST) | (1 << InstrumentCluster::SOUTH));
			origin.Set(min.X(), min.Y());
			break;
		case TOPCENTER:
			allow |= ((1 << InstrumentCluster::EAST) | (1 << InstrumentCluster::SOUTHEAST) | (1 << InstrumentCluster::SOUTH) | (1 << InstrumentCluster::SOUTHWEST) | (1 << InstrumentCluster::WEST));
			origin.Set(center.X(), min.Y());
			break;
		case TOPRIGHT:
			allow |= ((1 << InstrumentCluster::SOUTH) | (1 << InstrumentCluster::SOUTHWEST) | (1 << InstrumentCluster::WEST));
			origin.Set(max.X(), min.Y());
			break;
		case LEFTCENTER:
			allow |= ((1 << InstrumentCluster::NORTH) | (1 << InstrumentCluster::NORTHEAST) | (1 << InstrumentCluster::EAST) | (1 << InstrumentCluster::SOUTHEAST) | (1 << InstrumentCluster::SOUTH));
			origin.Set(min.X(), center.Y());
			break;
		case CENTER:
			allow |= ((1 << InstrumentCluster::NORTH) | (1 << InstrumentCluster::NORTHEAST) | (1 << InstrumentCluster::EAST) | (1 << InstrumentCluster::SOUTHEAST) | (1 << InstrumentCluster::SOUTH) | (1 << InstrumentCluster::SOUTHWEST) | (1 << InstrumentCluster::WEST) | (1 << InstrumentCluster::NORTHWEST));
			origin.Set(center.X(), center.Y());
			break;
		case RIGHTCENTER:
			allow |= ((1 << InstrumentCluster::NORTH) | (1 << InstrumentCluster::SOUTH) | (1 << InstrumentCluster::SOUTHWEST) | (1 << InstrumentCluster::WEST) | (1 << InstrumentCluster::NORTHWEST));
			origin.Set(max.X(), center.Y());
			break;
		case BOTTOMLEFT:
			allow |= ((1 << InstrumentCluster::NORTH) | (1 << InstrumentCluster::NORTHEAST) | (1 << InstrumentCluster::EAST));
			origin.Set(min.X(), max.Y());
			break;
		case BOTTOMCENTER:
			allow |= ((1 << InstrumentCluster::NORTH) | (1 << InstrumentCluster::NORTHEAST) | (1 << InstrumentCluster::EAST) | (1 << InstrumentCluster::WEST) | (1 << InstrumentCluster::NORTHWEST));
			origin.Set(center.X(), max.Y());
			break;
		case BOTTOMRIGHT:
			allow |= ((1 << InstrumentCluster::NORTH) | (1 << InstrumentCluster::WEST) | (1 << InstrumentCluster::NORTHWEST));
			origin.Set(max.X(), max.Y());
			break;
	}

	m_cluster[cluster] = new InstrumentCluster(origin, allow);
	ASSERT(m_cluster[cluster]);
	return m_cluster[cluster];
}

