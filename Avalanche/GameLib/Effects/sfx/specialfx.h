///////////////////////////////////////////////////////////////////////////////
// Generic module to manage special FX
///////////////////////////////////////////////////////////////////////////////
#ifndef SPECIALFX_H
#define SPECIALFX_H

#include "GameHelper/Sticky.h"
#include "GameHelper/Message.h"

class SpecialFXSystem;

// The base FX class has two modes of operation:
// A "TTL" mode where it goes away then the time to live timer expires.
// A non-TTL mode where the creator must subsequently free the SpecialFX
//
// In the case of a TTL SFX it is unsafe to maintain a pointer to the
// object between frames (as it may have expired and gone away). To safely
// retreive or test for the existence of a TTL SFX use FindByName.
class SpecialFX
{
public:
	// Creating without a ttl means you must destroy it
	SpecialFX(SpecialFXSystem &sfx, const char *name);

	// Auto destructs after ttl has expired
	SpecialFX(SpecialFXSystem &sfx, const char *name, float ttl);

	// Make sure all siblings go away too
	virtual ~SpecialFX();

	// give back the subclass
	virtual const char *Class() = 0;

	// Set the name
	void SetName(const char *name);

	// Return the name
	inline const char *Name() const
		{ return m_name; }

	// Reset the TTL
	virtual void ResetTTL(float newTTL, bool includeSiblings = true);

	// Handle moving stuff (which may mean nothing)
	virtual Vector3 GetPosition()
		{ return Vector3(0.0f, 0.0f, 0.0f); }
	virtual void MoveTo(const Vector3 &pos)
		{ if (m_sibling) m_sibling->MoveTo(pos); }

	// Add a sibling on this guy (deleted when destructor is called)
	virtual void AddSibling(SpecialFX *sibling);

	SpecialFX *GetFirstSibling()
		{ return m_sibling; }
	SpecialFX *GetNextSibling(SpecialFX *current)
		{ return current ? current->GetFirstSibling() : NULL; }

protected:
	SpecialFXSystem 	&m_sfx;
	char					m_name[32];
	float					m_born, m_ttl, m_die;

	// The sibling singly linked list
	SpecialFX			*m_sibling;

	// The system calls us!
	friend class SpecialFXSystem;

	// On create
	virtual void OnAdd(float birth);
	virtual void OnRemove() {}

	// Is it time to die?
	virtual bool TimeToDie(float now);
	virtual bool TTLBased()
		{ return (m_ttl > 0.0f); }
	virtual float Born()
		{ return m_born; }

	// Update
	virtual void Update(float now, float deltaT) = 0;

public:
	// We appear in a list
	SpecialFX	*prev, *next;
};

// A special FX with a sticky
class SpecialFXSticky : public SpecialFX
{
public:
	// We do nothing here
	SpecialFXSticky(SpecialFXSystem &sfx, const char *name) : SpecialFX(sfx, name) {}
	SpecialFXSticky(SpecialFXSystem &sfx, const char *name, float ttl) : SpecialFX(sfx, name, ttl) {}
	virtual ~SpecialFXSticky() {}

	// Access the sticky directly
	inline CSticky &Sticky()
		{ return m_sticky; }

	// Handle moving stuff
	virtual Vector3 GetPosition()
		{ return m_sticky.GetLocation(); }
	virtual void MoveTo(const Vector3 &pos);

protected:
	CSticky		m_sticky;
};

#endif
