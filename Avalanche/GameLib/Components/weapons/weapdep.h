/////////////////////////////////////////////////////////////////////////////
// Auto dependencies for weapon lib
// I'm sure there is a clever ass way to do this with a template,
// and I tried, but I'm in a hurry.
/////////////////////////////////////////////////////////////////////////////
#ifndef WEAPONDEP_H
#define WEAPONDEP_H

class WeaponDependency
{
public:
	WeaponDependency()
		{ m_next = m_head; m_head = this; }

	// you write these!
	virtual void InitializeOnce() = 0;
	virtual void TerminateOnce() = 0;

	virtual void InitializeEvery() {}
	virtual void TerminateEvery() {}

	// Do you want this before all other inits?
	enum Phase
	{
		PREINIT,
		POSTINIT
	};
	virtual Phase WantPhase() = 0;

	// Do the work
	static void InitializeAllOnce(Phase phase);
	static void TerminateAllOnce(Phase phase);

	static void InitializeAllEvery(Phase phase);
	static void TerminateAllEvery(Phase phase);

private:
	WeaponDependency	  		*m_next;
	static WeaponDependency *m_head;
};

#endif
