/////////////////////////////////////////////////////////////////////////////
// Quick tagging class
/////////////////////////////////////////////////////////////////////////////
#ifndef QUICK_TAG_H
#define QUICK_TAG_H

#include "Components/Component.h"
#include "Effects/decal/fadedecal.h"

#define MAX_QUICKTAGS  5

class QuickTag : public CActorComponent
{
public:
	QuickTag(CCompActor &actor);
	~QuickTag();

	// Our interface support
	const char *ComponentName()
		{ return "QuickTag"; }

	// The initialize
	void Initialize();

	// Tag!
	bool Tag(const Vector3 &eye, const Vector3 &dir);

private:
	// Our tags
	Tizag		*m_tag[MAX_QUICKTAGS];
	int 		m_index;
	uint     m_nextTagTime;

	// Helpers
	bool LegalMaterial(u8 materal);
	bool WillFit(const Vector3 &pos, const Vector3 &normal, const Vector3 &up);
	bool Apply(const CustomTagInfo &info, bool master = false);

	// Message Handlers
	REF_SEND_MESSAGE_HANDLER(ApplyTag, QuickTag) m_applyTagHandler;
	REF_BASIC_MESSAGE_HANDLER(CheckpointReset, MESSAGE_VALUE, QuickTag) m_checkpointReset;
};

#endif
