/////////////////////////////////////////////////////////////////////////////
// Simple shadow component
/////////////////////////////////////////////////////////////////////////////
#ifndef SHADOWCOMP_H
#define SHADOWCOMP_H

#include "Components/Component.h"
#include "Effects/dropshad.h"

class Shadow : public CActorComponent
{
public:
	// Pass in a list (terminated with a NULL string)
	// List must be static
	Shadow(CCompActor &actor);
	~Shadow();

	// Our interface support
	const char *ComponentName()
		{ return "Shadow"; }

	// The initialize
	void Initialize();

	// The update
	void RunAnimation();

private:
	// Our shadow!
	bool						m_hasProjectedShadow;

	// shadow handles
	DropShadow::Handle	m_dropShadow;
	t_Handle 				m_projectedShadow;
	ts_Light					m_fakeSun;

	// Center for drop shadow
	BoneID					m_boneID;
	Vector3					m_shadowCenter;
	float						m_radius;

	// Get the radii
	inline Vector3 GetRadii();

	// Bone stuff
	void UpdateBones();
	static Vector3 GetBoneInWorld(CActor &forWho, BoneID boneID);

	// Update the radius
	void UpdateRadius();
};

#endif
