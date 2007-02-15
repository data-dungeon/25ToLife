/***********************************/
/*                                 */
/* Spawn.h                         */
/* Pigpen particle tool  4/5/02    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* simple effect spawning class    */
/*                                 */
/***********************************/

#ifndef __SPAWN_H
#define __SPAWN_H

/******************* includes ****************************/

/******************* defines *****************************/

/* file version. must be an integer. increment by 1 each time there is a version change in image serialization */

#define SPAWN_VERSION		0

/* flag bits for m_ulFlags. have to avoid conflict with engine particle flags enumerated in g_ParticleDB.h, so use */
/* "PART_" instead of "PARTICLE_" */

#define SPAWN_USE_DEFAULT_SURFACE	0x00000001	// use default collision surface spawn settings
#define SPAWN_EFFECT_ACTIVE			0x00000002	// "spawns" checkbox
#define SPAWN_END_OF_LIFE				0x00000004	// can edit spawn effect even if "particle collides" is off

/* values and masks for m_u32Bitfields (radio button bit fields) */

#define SPAWN_TYPE_SHIFT				0
#define SPAWN_TYPE_DEFAULT				(0x00000000 << SPAWN_TYPE_SHIFT)
#define SPAWN_TYPE_BOUNCE				(0x00000001 << SPAWN_TYPE_SHIFT)
#define SPAWN_TYPE_STICK				(0x00000002 << SPAWN_TYPE_SHIFT)
#define SPAWN_TYPE_CONTINUE			(0x00000003 << SPAWN_TYPE_SHIFT)
#define SPAWN_TYPE_MASK					(0x00000003 << SPAWN_TYPE_SHIFT)

/******************* structures **************************/

/******************* forward declarations ****************/

class CEffect;

/******************* class *******************************/

class CSpawn : public CObject
{
	/* give particle access to data members */

	friend class CParticle;

public:

	/* this enables easy serialization of the CObArray-- you just call array's Serialize function and all the */
	/* array records serialize themselves, in order-- slick */

	DECLARE_SERIAL(CSpawn)

// Constructor/destructor

	CSpawn();
	~CSpawn();

// Implementation

protected:

	/* keep serialization up-to-date! */

	int m_nEffectIndex;	// only used for serialization
	unsigned long m_ulVersion;
	unsigned long m_u32Flags;
	unsigned long m_u32Bitfields;
	float m_fBounceEnergy;

	/* not serialized */

	CEffect *m_pEffect;
	
	/* keep serialization up-to-date! */

public:

	/* simple routines */

	void SetEffect(CEffect *pEffect)				{ m_pEffect = pEffect; }
	CEffect *GetEffect()								{ return(m_pEffect); }
	void SetEffectIndex(int nIndex)				{ m_nEffectIndex = nIndex; }
	int GetEffectIndex()								{ return(m_nEffectIndex); }
	float GetBounceEnergy()							{ return(m_fBounceEnergy); }

	void SetFlags(unsigned long u32Flags)		{ m_u32Flags |= u32Flags; }
	void ClearFlags(unsigned long u32Flags)	{ m_u32Flags &= ~u32Flags; }
	bool UseDefault()									{ return((m_u32Flags & SPAWN_USE_DEFAULT_SURFACE) != 0); }
	bool Spawns()										{ return((m_u32Flags & SPAWN_EFFECT_ACTIVE) != 0); }
	bool EndOfLife()									{ return((m_u32Flags & SPAWN_END_OF_LIFE) != 0); }
	bool ValidForExport()							{ return((UseDefault() == false) && (Spawns()) && (GetEffect() != NULL)); }
	int Type()											{ return(m_u32Bitfields & SPAWN_TYPE_MASK); }

	/* routines defined in .cpp */

	/* override serialization-- let object save itself out */

	virtual void Serialize(CArchive& ar);

// operations

public:

};

#endif // __SPAWN_H
