///////////////////////////////////////////////////////////////////////////////
// Simple system for laying out an "Instrument Panel" i.e. HUD. Let's
// each instrument on a panel to be written and maintained separately.
///////////////////////////////////////////////////////////////////////////////
#ifndef IPANEL_H
#define IPANEL_H

#include "hud/ipanel/igroup.h"
#include "hud/ipanel/icluster.h"

///////////////////////////////////////
// A panel is a collection of instrument
// groups arranged into clusters.
///////////////////////////////////////
class InstrumentPanel
{
public:
	InstrumentPanel(const char *panelName);
	~InstrumentPanel();

	const char *Name()
		{ return m_name; }

	// Setup the transition for all the groups
	void SetDefaultTransition(float transitionTime, int transition = InstrumentGroup::TRANSITION_FADE);

	// The areas of the screen
	enum Cluster
	{
		TOPLEFT, TOPCENTER, TOPRIGHT,
		LEFTCENTER, CENTER, RIGHTCENTER,
		BOTTOMLEFT, BOTTOMCENTER, BOTTOMRIGHT,
		CLUSTERS
	};

	// Add an instrument (I own it now, you new, I delete)
	bool Add(Cluster cluster, InstrumentCluster::Direction allowMove, const char *groupName, Instrument *instrument);
	bool Remove(const char *groupName, const char *instrumentName);
	bool Show(const char *groupName, const char *instrumentName, bool show = true);
	bool Activate(const char *groupName, const char *instrumentName, bool activate = true);

	// Group operatrions
	bool RemoveGroup(const char *groupName);
	bool ShowGroup(const char *groupName, bool show = true, float transitionTime = -1.0f);
	bool ActivateGroup(const char *groupName, bool activate = true);

	// All!
	bool ShowAll(bool show = true, float transitionTime = -1.0f);
	bool ActivateAll(bool activate = true);

	// Call this once per frame!
	void Update(float dt);

private:
	// The panel name
	char											m_name[32];

	// The groups!
	DoublyLinkedList<InstrumentGroup>	m_groups;
	InstrumentGroup *FindGroup(const char *groupName);
	InstrumentGroup *CreateGroup(const char *groupName);

	// The clusters
	InstrumentCluster							*m_cluster[CLUSTERS];
	InstrumentCluster *GetCluster(Cluster cluster);

	// Transition support
	float											m_transitionTime;
	int											m_transitionType;

public:
	// We appear in a list
	InstrumentPanel		*prev, *next;
};

#endif
