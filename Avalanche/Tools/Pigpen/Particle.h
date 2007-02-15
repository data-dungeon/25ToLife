/***********************************/
/*                                 */
/* Particle.h                      */
/* Pigpen particle tool  12/10/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle class                  */
/*                                 */
/***********************************/

#ifndef __PARTICLE_H
#define __PARTICLE_H

/******************* includes ****************************/

#include "Effect.h"
#include "Emitter.h"
#include "Graphic.h"
#include "Envelope.h"
#include "Spawn.h"

/* ImportExport includes */

/* game engine includes */

#include <Game/Managers/ParticleDB.h>

/******************* defines *****************************/

/* file version. must be an integer. increment by 1 each time there is a version change in particle serialization */

#define PARTICLE_VERSION		16

/* max size of particle name. name is treated as a CString for the most part, but there are times when it */
/* it's useful to be able to save off the string somewhere else for copying purposes */

#define PARTICLE_MAX_NAME_LENGTH		100 

/* flag bits for m_u32Flags. have to avoid conflict with engine particle flags enumerated in g_ParticleDB.h, so use */
/* "PART_" instead of "PARTICLE_" */

#define PART_ASSOCIATED							0x00000001
#define PART_SELECTED							0x00000002	// for serialization
#define OBSOLETE_PART_XMIRROR								0x00000004	// obsolete-- intended to support building a larger image out of a 1/2 or 1/4 image, never implemented
#define OBSOLETE_PART_YMIRROR								0x00000008	// obsolete-- intended to support building a larger image out of a 1/2 or 1/4 image, never implemented
#define PART_MAINTAIN_ASPECT_RATIO			0x00000010
#define ENVELOPE_HIDE_X_SIZE					0x00000020
#define ENVELOPE_HIDE_Y_SIZE					0x00000040
#define ENVELOPE_HIDE_RED						0x00000080
#define ENVELOPE_HIDE_GREEN					0x00000100
#define ENVELOPE_HIDE_BLUE						0x00000200
#define ENVELOPE_HIDE_ALPHA					0x00000400
#define ENVELOPE_HIDE_ALL						(ENVELOPE_HIDE_X_SIZE | ENVELOPE_HIDE_Y_SIZE | ENVELOPE_HIDE_RED | ENVELOPE_HIDE_GREEN | ENVELOPE_HIDE_BLUE | ENVELOPE_HIDE_ALPHA)
#define PART_BULLET								0x00000800
#define PART_STANDARD_GRAVITY					0x00001000
#define PART_INIT_ROT_RANDOM					0x00002000
#define PART_PERPENDICULAR_TO_VELOCITY		0x00004000	// must be a free orient particle
#define OBSOLETE_PART_COLLIDES							0x00008000	// enabler for collision with world/actors/water, not exported
#define PART_FLOATS_ON_WATER					0x00010000
#define PART_ALIGN_TO_VELOCITY				0x00020000
#define PART_LIGHTED								0x00040000
#define PART_NO_DISTORTION						0x00080000	// particle is unaffected by distortions (like wind)
#define PART_ZBUFFER_WRITE						0x00100000	// write to the z-buffer (for opaque or mostly opaque images)
#define PART_NO_VIEW_CULL						0x00200000	// particle does not die once outside of view
#define PART_DIE_AT_END_OF_LEASH				0x00400000	// particle dies once it gets a certain distance from creation position
#define PART_FORCEFIELDS_AFFECT				0x00800000	// particle is affected by forcefield distortions
#define PART_ADDITIVE_RENDER					0x01000000	// use additive blend function when rendering
#define PART_PIGGYBACK_EMITTER				0x02000000	// an emitter piggybacks on particle
#define PART_RANDOM_POS_PLANE					0x04000000	// if random position non-zero, generate in emitter space xz plane
#define PART_NEW_ASSOCIATE						0x08000000
/*** always add to end of list-- don't reorder-- flags are serialized ***/

/* flag bits for m_u32RandomFlags. somehow these values got all fubar'd, but there's lots of bits left */

#define PART_FPS_CONT_RANDOM							0x00000001
#define PART_RANDOM_SIZE_MAINTAIN_ASPECT_RATIO	0x00000008
#define PART_RANDOM_START_FRAME						0x00004000
#define PART_ORBIT_INIT_ROT_RANDOM					0x00008000

/* flag bits for m_u32CollideFlags. these got taken out of the m_u32Flags set */

#define PART_COLLIDES							0x00000001	// enabler for collision with world/actors/water, not exported
#define PART_COLLIDES_WORLD					0x00000002
#define PART_COLLIDES_ACTORS					0x00000004
#define PART_COLLIDES_WATER					0x00000008

/* ned says: it looks to me like i implemented these next 2 wrong (or just lazily). there should be a flags word per */
/* collision surface type that has these flags, as well as the DIE/BOUNCE/STICK/CONTINUE flags in the bitfields. maybe */
/* they could go into the spawn records? i don't remember if there is a spawn record per collision surface type. i */
/* found this when i was looking at how to add a CREATE_WATER_SPRITE flag, which logically only belongs with a water */
/* collision surface, which should only be available if COLLIDES_WATER is checked. it's a bit awkward, because */
/* checking COLLIDES_WATER shouldn't enable the other collision surface types, only water. but how do you populate */
/* combo box at that point? just add the entries that are available based on the COLLIDES_??? flags? for now i'm just */
/* going to have a single CREATE_WATER_SPRITE flag */

#define PART_COLLIDE_STOP_ROTATING			0x00000010	// particle stops rotating when it collides
#define PART_COLLIDE_STOP_ANIMATING			0x00000020	// particle stops animating when it collides

#define PART_COLLIDE_CREATE_WATER_SPRITE	0x00000040	// create a water sprite on tyler's water when it collides

/* old collide flag values when they were in m_u32Flags, for old version fixup */

#define OLD_PART_COLLIDES						0x00008000
#define OLD_PART_COLLIDES_WORLD				0x20000000
#define OLD_PART_COLLIDES_ACTORS				0x40000000
#define OLD_PART_COLLIDES_WATER				0x80000000
#define OLD_PART_COLLIDE_STOP_ROTATING		0x08000000
#define OLD_PART_COLLIDE_STOP_ANIMATING	0x10000000

/* values and masks for m_u32Bitfields (radio button bit fields) */

#define PART_ORIENT_SHIFT				0
#define PART_ORIENT_SCREEN				(0x00000000 << PART_ORIENT_SHIFT)
#define PART_ORIENT_FREE				(0x00000001 << PART_ORIENT_SHIFT)
#define PART_ORIENT_GROUND				(0x00000002 << PART_ORIENT_SHIFT)
#define PART_ORIENT_WALL				(0x00000003 << PART_ORIENT_SHIFT)
#define PART_ORIENT_MASK				(0x00000003 << PART_ORIENT_SHIFT)

#define PART_ANIM_SHIFT					(PART_ORIENT_SHIFT + 2)	// skip over orient mask
#define PART_ANIM_LOOP					(0x00000000 << PART_ANIM_SHIFT)
#define PART_ANIM_PING_PONG			(0x00000001 << PART_ANIM_SHIFT)
#define PART_ANIM_ONE_SHOT				(0x00000002 << PART_ANIM_SHIFT)
#define PART_ANIM_MASK					(0x00000003 << PART_ANIM_SHIFT)

#define PART_COLOR_SHIFT				(PART_ANIM_SHIFT + 2)	// skip over anim mask
#define PART_COLOR_PAIR					(0x00000000 << PART_COLOR_SHIFT)
#define PART_COLOR_RGB					(0x00000001 << PART_COLOR_SHIFT)
#define PART_COLOR_MASK					(0x00000001 << PART_COLOR_SHIFT)

#define PART_COLLIDE_SHIFT				(PART_COLOR_SHIFT + 1)	// skip over color mask
#define PART_COLLIDE_DIE				(0x00000000 << PART_COLLIDE_SHIFT)
#define PART_COLLIDE_BOUNCE			(0x00000001 << PART_COLLIDE_SHIFT)
#define PART_COLLIDE_STICK				(0x00000002 << PART_COLLIDE_SHIFT)
#define PART_COLLIDE_CONTINUE			(0x00000003 << PART_COLLIDE_SHIFT)
#define PART_COLLIDE_MASK				(0x00000003 << PART_COLLIDE_SHIFT)

#define PART_EJECT_SHIFT				(PART_COLLIDE_SHIFT + 2)	// skip over collide mask
#define PART_EJECT_PARTICLE			(0x00000000 << PART_EJECT_SHIFT)
#define PART_EJECT_DEFAULT				(0x00000001 << PART_EJECT_SHIFT)
#define PART_EJECT_MASK					(0x00000001 << PART_EJECT_SHIFT)

#define PART_ORBIT_PLANE_SHIFT		(PART_EJECT_SHIFT + 1)	// skip over eject mask
#define PART_ORBIT_SCREEN				(0x00000000 << PART_ORBIT_PLANE_SHIFT)
#define PART_ORBIT_GROUND				(0x00000001 << PART_ORBIT_PLANE_SHIFT)
#define PART_ORBIT_ALIGN_TO_VEL		(0x00000002 << PART_ORBIT_PLANE_SHIFT)
#define PART_ORBIT_PLANE_MASK			(0x00000003 << PART_ORBIT_PLANE_SHIFT)

/* special hard-wired graphics. leave value even if it becomes obsolete */

#define PARTICLE_HARDWIRED_GRAPHIC_NONE						0
#define PARTICLE_HARDWIRED_GRAPHIC_FRONT_BUFFER				1
#define PARTICLE_HARDWIRED_GRAPHIC_BACK_BUFFER				2
#define PARTICLE_HARDWIRED_GRAPHIC_COUNT						3

/* current number of hard-wired graphics supported. may be less than number defined if we end up eliminating any */

#define PARTICLE_HARDWIRED_GRAPHIC_NONE_IN_LIST				0
#define PARTICLE_HARDWIRED_GRAPHIC_FRONT_BUFFER_IN_LIST	1
#define PARTICLE_HARDWIRED_GRAPHIC_BACK_BUFFER_IN_LIST	2
#define PARTICLE_HARDWIRED_GRAPHIC_LIST_COUNT				3

/******************* structures **************************/

/******************* forward declarations ****************/

class CImage;
class CSpawn;

/******************* class *******************************/

class CParticle : public CObject
{
public:

	/* this enables easy serialization of the CObArray-- you just call array's Serialize function and all the */
	/* particle records serialize themselves, in order-- slick */

	DECLARE_SERIAL(CParticle)

// Constructor/destructor

	CParticle();
	~CParticle();

// Implementation

protected:

	/* keep serialization up-to-date! */

	CString m_Name;
	unsigned long m_ulVersion;
	unsigned long m_u32Flags;
	unsigned long m_u32RandomFlags;	// one-shot vs continuous randomness for application of randomness value
	unsigned long m_u32CollideFlags;	// collision-related flags
	unsigned long m_u32Bitfields;		// for radio button groups-- can be more than one bit wide
	int m_nGraphicIndex;					// used for serialization pointer-to-index conversion
	int m_nHardwiredGraphic;			// if not a valid graphic image, may be a special graphic like front buffer
	int m_nPiggybackEmitterIndex;		// used for serialization pointer-to-index conversion
	float m_fXSize;						// x-size
	float m_fXSizeRandomPct;			// x-size randomness
	float m_fYSize;						// y-size
	float m_fYSizeRandomPct;			// x-size randomness
	float m_fLifetime;					// lifetime (in seconds)
	float m_fLifetimeRandomPct;		// lifetime randomness
	float m_fLeash;						// leash distance (meters) for die at end of leash-- distance from creation position
	float m_fLeashRandomPct;			// leash distance randomness

	float m_fXPos;							// initial position (for bullets that need to come out of barrel of gun)
	float m_fYPos;
	float m_fZPos;
	float m_fPosRandom;					// position randomness (radius of sphere)
	float m_fXVel;							// initial velocity
	float m_fYVel;
	float m_fZVel;
	float m_fVelRandomPct;				// randomness of initial velocity magnitude (in percent)
	float m_fVelCone1;					// size of random velocity cone in degrees (min degrees)
	float m_fVelCone2;					// size of random velocity cone in degrees (max degrees)
	float m_fGravity;						// gravity
	float m_fAccel;						// acceleration (can be negative for friction/air resistance)
	float m_fXRotVel;						// initial rotation velocity
	float m_fYRotVel;
	float m_fZRotVel;
	float m_fRotVelRandomPct;			// randomness of initial rotation velocity (in percent)
	float m_fRotAccel;					// rotation acceleration (can be negative for friction/air resistance)
	float m_fCollideBounceEnergy;		// bounce/continue energy when particle collides
	float m_fZBias;						// rendering adjustment to avoid z-buffer clipping
	
	/* exported as integers */

	float m_fFPS;							// animation speed
	float m_fFPSRandomPct;				// animation speed randomness

	/* envelopes */

	CEnvelope m_Envelope[MAX_ENVELOPES];	// envelopes for particle behavior control
	int m_nCurrentEnvelope;						// current envelope being edited in envelope dialog

	/* colors for color pair color control */

	COLORREF	m_Color[2];

	/* orbit */

	float m_fOrbitRadius;
	float m_fOrbitRadiusRandomPct;
	float m_fOrbitRadiusVel;
	float m_fOrbitRadiusVelRandomPct;
	float m_fOrbitRotVel;
	float m_fOrbitRotVelRandomPct;

	/* list of spawn effect definitions, one per surface type defined in doc */

	CObArray m_Spawn;
	CSpawn *m_pCurrentSpawn;					// current working spawn record in collide page

//xxx this becomes a run-time malloc'd array of pDoc->GetSurfaceCount() entries, with flag settings
//for each entry (e.g. default/bounce/stick/continue, bounce/continue energy, active flag. then at serialization
//we save out one entry per active entry, with surface name instead of index. then we match them back up
//again at load

	/* obsolete, but kept for compatibility */

	float m_fObsoleteSpawnBounceEnergy;		// now one per surface type
	float m_fObsoletePreviewRate;

	/* not serialized */

	CEmitter *m_pPiggybackEmitter;	// for piggyback emitters
	CGraphic *m_pGraphic;				// graphic that particle uses, set at construction or serialization loading

	int m_nTempIndex;						// used for export
	int m_nExportStatus;					// used for export
	int m_nExportDataSize;				// used for preview
	u32 m_u32TestFlags;					// used for export error-checking
	void *m_pEngineData;					// for real-time update of particle data during particle preview
	char m_NameString[PARTICLE_MAX_NAME_LENGTH];	// for record copying purposes
	int m_nUsedCount;						// for use analysis
	unsigned long m_ulLoadVersion;	// version number from serialization load, for possible fix-ups

	/* keep serialization up-to-date! */

public:

	/* simple routines */

	CString& GetName(void)								{ return(m_Name); }
	void SetName(char *pName)							{ m_Name = pName; }
	bool IsBullet(void)									{ return((m_u32Flags & PART_BULLET) != 0); }
	bool IsMaintainAspectRatio(void)					{ return((m_u32Flags & PART_MAINTAIN_ASPECT_RATIO) != 0); }
	bool IsAlignToVelocity(void)						{ return((m_u32Flags & PART_ALIGN_TO_VELOCITY) != 0); }
	void SetGraphic(CGraphic *pGraphic)				{ m_pGraphic = pGraphic; }
	CGraphic *GetGraphic(void)							{ return(m_pGraphic); }
	void SetGraphicIndex(int nIndex)					{ m_nGraphicIndex = nIndex; }
	int GetGraphicIndex(void)							{ return(m_nGraphicIndex); }
	void SetHardwiredGraphic(int nValue)			{ ASSERT(nValue < PARTICLE_HARDWIRED_GRAPHIC_COUNT); m_nHardwiredGraphic = nValue; }
	int GetHardwiredGraphic(void)						{ return(m_nHardwiredGraphic); }
	bool IsDistortionGraphic()							{ return((m_pGraphic == NULL) && ((GetHardwiredGraphic() == PARTICLE_HARDWIRED_GRAPHIC_FRONT_BUFFER) || (GetHardwiredGraphic() == PARTICLE_HARDWIRED_GRAPHIC_BACK_BUFFER))); }
	int GetHeight(void)									{ if (m_pGraphic) return(m_pGraphic->GetHeight()); else return(1); }
	int GetWidth(void)									{ if (m_pGraphic) return(m_pGraphic->GetWidth()); else return(1); }
	bool MaintainAspectRatio(void)					{ return((m_u32Flags & PART_MAINTAIN_ASPECT_RATIO) != 0); }
	void SetFPS(float fFPS)								{ m_fFPS = fFPS; }
	void SetXSize(float fXSize)						{ m_fXSize = fXSize; }
	void SetYSize(float fYSize)						{ m_fYSize = fYSize; }
	void SetMaintainAspectRatio(bool bOn)			{ if (bOn) m_u32Flags |= PART_MAINTAIN_ASPECT_RATIO; else m_u32Flags &= ~PART_MAINTAIN_ASPECT_RATIO; }
	void SetTempIndex(int nIndex)						{ m_nTempIndex = nIndex; }
	int GetTempIndex(void)								{ return(m_nTempIndex); }
	void SetExportStatus(int nStatus)				{ m_nExportStatus = nStatus; }
	int GetExportStatus(void)							{ return(m_nExportStatus); }
	void SetExportDataSize(int nSize)				{ m_nExportDataSize = nSize; }
	int GetExportDataSize(void)						{ return(m_nExportDataSize); }
	void SetSelected(bool bSelected)					{ if (bSelected) m_u32Flags |= PART_SELECTED; else m_u32Flags &= ~PART_SELECTED; }
	bool GetSelected(void)								{ return((m_u32Flags & PART_SELECTED) != 0); }
	CEnvelope *GetCurrentEnvelope(void)				{ return(&m_Envelope[m_nCurrentEnvelope]); }
	void SetCurrentEnvelopeIndex(int nIndex)		{ m_nCurrentEnvelope = nIndex; }
	int GetCurrentEnvelopeIndex(void)				{ return(m_nCurrentEnvelope); }
	CEnvelope *GetEnvelope(int nIndex)				{ return(&m_Envelope[nIndex]); }
	void SetOrient(u32 u32FlagBit)					{ ASSERT(u32FlagBit & PART_ORIENT_MASK); m_u32Bitfields &= ~PART_ORIENT_MASK; m_u32Bitfields |= u32FlagBit;}
	int GetOrient(void)									{ return(m_u32Bitfields & PART_ORIENT_MASK); }
	int GetColorControl(void)							{ return(m_u32Bitfields & PART_COLOR_MASK); }
	void SetEngineData(void *pData)					{ m_pEngineData = pData; }
	void *GetEngineData(void)							{ return(m_pEngineData); }
	void SetColor(int nIndex, COLORREF Color)		{ m_Color[nIndex] = Color; }
	COLORREF GetColor(int nIndex)						{ return(m_Color[nIndex]); }
	bool Collides(void)									{ return(IsCollideFlagSet(PART_COLLIDES)); }
	bool CollidesWithWorld(void)						{ return(IsCollideFlagSet(PART_COLLIDES_WORLD)); }
	bool CollidesWithActors(void)						{ return(IsCollideFlagSet(PART_COLLIDES_ACTORS)); }
	bool CollidesWithWater(void)						{ return(IsCollideFlagSet(PART_COLLIDES_WATER)); }
	bool CollidesValid(void)							{ return(Collides() && (CollidesWithWorld() || CollidesWithActors() || CollidesWithWater())); }
	bool HasPiggybackEmitter(void)					{ return((m_u32Flags & PART_PIGGYBACK_EMITTER) != 0); }
	CObArray *GetSpawnArray()							{ return(&m_Spawn); }
	CSpawn *GetSpawn(int nIndex)						{ return((CSpawn *) m_Spawn.GetAt(nIndex)); }
	void SetSpawn(int nIndex, CSpawn *pSpawn)		{ m_Spawn.SetAt(nIndex, pSpawn); }
	int GetSpawnCount()									{ return(m_Spawn.GetSize()); }
	CSpawn *GetCurrentSpawn()							{ return(m_pCurrentSpawn); }
	void SetCurrentSpawn(int nIndex)					{ m_pCurrentSpawn = GetSpawn(nIndex); }
	void ClearCurrentSpawn()							{ m_pCurrentSpawn = NULL; }
	void AddNewSpawn()									{ m_Spawn.Add(new CSpawn); }
	void DeleteSpawn(int nIndex)						{ m_Spawn.RemoveAt(nIndex); }
	int GetCollideType(void)							{ return(m_u32Bitfields & PART_COLLIDE_MASK); }
	void SetUsedCount(int nCount)						{ m_nUsedCount = nCount; }
	int GetUsedCount(void)								{ return(m_nUsedCount); }
	void IncUsedCount(void)								{ m_nUsedCount++; }
	bool CheckVelCone1(void)							{ if (m_fVelCone1 <= m_fVelCone2) return(FALSE); m_fVelCone1 = m_fVelCone2; return(TRUE); }
	bool CheckVelCone2(void)							{ if (m_fVelCone2 >= m_fVelCone1) return(FALSE); m_fVelCone2 = m_fVelCone1; return(TRUE); }
	void SetFlag(u32 u32FlagBit)						{ m_u32Flags |= u32FlagBit; }
	void ClearFlag(u32 u32FlagBit)					{ m_u32Flags &= ~u32FlagBit; }
	bool IsFlagSet(u32 u32FlagBit)					{ return((m_u32Flags & u32FlagBit) == u32FlagBit); }	// can be more than one bit passed, will check for all bits set
	void SetCollideFlag(u32 u32FlagBit)				{ m_u32CollideFlags |= u32FlagBit; }
	void ClearCollideFlag(u32 u32FlagBit)			{ m_u32CollideFlags &= ~u32FlagBit; }
	bool IsCollideFlagSet(u32 u32FlagBit)			{ return((m_u32CollideFlags & u32FlagBit) == u32FlagBit); }	// can be more than one bit passed, will check for all bits set
	unsigned long GetLoadVersion(void)				{ return(m_ulLoadVersion); }
	void SetPiggybackEmitter(CEmitter *pEmitter)	{ m_pPiggybackEmitter = pEmitter; }
	CEmitter *GetPiggybackEmitter(void)				{ return(m_pPiggybackEmitter); }
	void SetPiggybackEmitterIndex(int nIndex)		{ m_nPiggybackEmitterIndex = nIndex; }
	int GetPiggybackEmitterIndex(void)				{ return(m_nPiggybackEmitterIndex); }
	
	/* routines defined in .cpp */

	CString& GetGraphicName(void);
	int FillExportBuffer(char *pBuffer, int nMaxSize, bool bPreview);
	bool ExportSpawnEffects(ts_ParticleSource *pParticleSource, bool bPreview);
	bool ExportSpawnEffect(ts_SpawnEffect *pSpawnEffect, CSpawn *pSpawn);
	void AddFlag(u32 u32Flags, ts_ParticleSource *pParticleSource, u32 u32InternalFlag, u32 u32ExportFlag);
	void AddBitfield(ts_ParticleSource *pParticleSource, u32 u32InternalMask, u32 u32InternalShift, u32 u32ExportMask, u32 u32ExportShift);
	int AnalyzeForExport(bool bPreview);
	bool EnvelopeHidden(int nIndex);
	void PrepareForMemcpy(void);
	void RestoreAfterMemcpy(void);
	CImage *GetImage(bool *pbShowAlphaMask);
	void LoadFixup(void);

	/* association testing */

	bool IsAssociated()									{ return((m_u32Flags & PART_ASSOCIATED) != 0); }
	bool IsNewAssociate()								{ return((m_u32Flags & PART_NEW_ASSOCIATE) != 0); }
	void ClearAssociateFlags()							{ m_u32Flags &= ~(PART_ASSOCIATED | PART_NEW_ASSOCIATE); }
	void ClearNewAssociateFlag()						{ m_u32Flags &= ~PART_NEW_ASSOCIATE; }
	void SetAssociateFlags()							{ m_u32Flags |= (PART_ASSOCIATED | PART_NEW_ASSOCIATE); }
	int TestAssociate(CObject *pTestObject, int nType);

	/* override serialization-- let object save itself out */

	virtual void Serialize(CArchive& ar);

	/* static function for telling particle construction how many spawn records to create-- can't pass */
	/* surface count into constructor because of automatic construction via CObArray serialization */

	static int nSurfaceCount;
	static int nPredefinedSurfaceCount;
	static int nEndOfLifeSurfaceCount;
	static int nDefaultCollisionSurfaceIndex;
	static void SetSurfaceCount(int nCount)						{ nSurfaceCount = nCount; }
	static int GetSurfaceCount()										{ return(nSurfaceCount); }
	static void SetPredefinedSurfaceCount(int nCount)			{ nPredefinedSurfaceCount = nCount; }
	static void SetEndOfLifeSurfaceCount(int nCount)			{ nEndOfLifeSurfaceCount = nCount; }
	static void SetDefaultCollisionSurfaceIndex(int nIndex)	{ nDefaultCollisionSurfaceIndex = nIndex; }
	static int *pnSurfaceTranslation;
	static void SetSurfaceTranslation(int *pnTranslation)		{ pnSurfaceTranslation = pnTranslation; }

	/* for more generic dialog processing, give access to addresses of data fields. data field id's are used */
	/* by dialog control lists */

	enum {	// because the enum is inside the class, it needs to be referenced as CParticle::DATA_XXX
		DATA_FLAGS = 0,
		DATA_RANDOM_FLAGS,
		DATA_COLLIDE_FLAGS,
		DATA_BITFIELDS,

		/* display page */

		DATA_FPS,
		DATA_FPS_RANDOM,
		DATA_ZBIAS,

		/* size page */

		DATA_XSIZE,
		DATA_XSIZE_RANDOM,
		DATA_YSIZE,
		DATA_YSIZE_RANDOM,
		DATA_ORBIT_RADIUS,
		DATA_ORBIT_RADIUS_RANDOM,
		DATA_ORBIT_RADIUS_VEL,
		DATA_ORBIT_RADIUS_VEL_RANDOM,
		DATA_ORBIT_ROTVEL,
		DATA_ORBIT_ROTVEL_RANDOM,

		/* movement page */

		DATA_XPOS,
		DATA_YPOS,
		DATA_ZPOS,
		DATA_POS_RANDOM,
		DATA_XVEL,
		DATA_YVEL,
		DATA_ZVEL,
		DATA_VEL_RANDOM,
		DATA_VEL_CONE1,
		DATA_VEL_CONE2,
		DATA_GRAVITY,
		DATA_ACCEL,
		DATA_XROT_VEL,
		DATA_YROT_VEL,
		DATA_ZROT_VEL,
		DATA_ROT_VEL_RANDOM,
		DATA_ROT_ACCEL,

		/* envelope dialog */

		DATA_ENVELOPE_BITFIELDS,
		DATA_CURRENT_ENVELOPE,

		/* preview dialog */

		DATA_PREVIEW_RATE,

		/* collision dialog */

		DATA_COLLIDE_BOUNCE,
		DATA_SPAWN_BOUNCE,
		DATA_SPAWN_FLAGS,
		DATA_SPAWN_BITFIELDS,
		
		/* lifetime dialog */

		DATA_LIFE,
		DATA_LIFE_RANDOM,
		DATA_LEASH,
		DATA_LEASH_RANDOM,

		// update GetDataPointer, below, as you add more
	};

	void *GetDataPointer(int nDataID)
	{
		switch(nDataID)
		{
		case DATA_FLAGS:
			return(&m_u32Flags);
		case DATA_RANDOM_FLAGS:
			return(&m_u32RandomFlags);
		case DATA_COLLIDE_FLAGS:
			return(&m_u32CollideFlags);
		case DATA_BITFIELDS:
			return(&m_u32Bitfields);

		/* display page */

		case DATA_FPS:
			return(&m_fFPS);
		case DATA_FPS_RANDOM:
			return(&m_fFPSRandomPct);
		case DATA_ZBIAS:
			return(&m_fZBias);

		/* size page */

		case DATA_XSIZE:
			return(&m_fXSize);
		case DATA_XSIZE_RANDOM:
			return(&m_fXSizeRandomPct);
		case DATA_YSIZE:
			return(&m_fYSize);
		case DATA_YSIZE_RANDOM:
			return(&m_fYSizeRandomPct);
		case DATA_ORBIT_RADIUS:
			return(&m_fOrbitRadius);
		case DATA_ORBIT_RADIUS_RANDOM:
			return(&m_fOrbitRadiusRandomPct);
		case DATA_ORBIT_RADIUS_VEL:
			return(&m_fOrbitRadiusVel);
		case DATA_ORBIT_RADIUS_VEL_RANDOM:
			return(&m_fOrbitRadiusVelRandomPct);
		case DATA_ORBIT_ROTVEL:
			return(&m_fOrbitRotVel);
		case DATA_ORBIT_ROTVEL_RANDOM:
			return(&m_fOrbitRotVelRandomPct);

		/* movement page */

		case DATA_XPOS:
			return(&m_fXPos);
		case DATA_YPOS:
			return(&m_fYPos);
		case DATA_ZPOS:
			return(&m_fZPos);
		case DATA_POS_RANDOM:
			return(&m_fPosRandom);
		case DATA_XVEL:
			return(&m_fXVel);
		case DATA_YVEL:
			return(&m_fYVel);
		case DATA_ZVEL:
			return(&m_fZVel);
		case DATA_VEL_RANDOM:
			return(&m_fVelRandomPct);
		case DATA_VEL_CONE1:
			return(&m_fVelCone1);
		case DATA_VEL_CONE2:
			return(&m_fVelCone2);
		case DATA_GRAVITY:
			return(&m_fGravity);
		case DATA_ACCEL:
			return(&m_fAccel);
		case DATA_XROT_VEL:
			return(&m_fXRotVel);
		case DATA_YROT_VEL:
			return(&m_fYRotVel);
		case DATA_ZROT_VEL:
			return(&m_fZRotVel);
		case DATA_ROT_VEL_RANDOM:
			return(&m_fRotVelRandomPct);
		case DATA_ROT_ACCEL:
			return(&m_fRotAccel);

		/* envelope page */

		case DATA_ENVELOPE_BITFIELDS:
			return(&m_Envelope[m_nCurrentEnvelope].m_ulBitfields);
		case DATA_CURRENT_ENVELOPE:
			return(&m_nCurrentEnvelope);

		/* collision dialog */

		case DATA_COLLIDE_BOUNCE:
			return(&m_fCollideBounceEnergy);
		case DATA_SPAWN_BOUNCE:
			return(&m_pCurrentSpawn->m_fBounceEnergy);
		case DATA_SPAWN_FLAGS:
			return(&m_pCurrentSpawn->m_u32Flags);
		case DATA_SPAWN_BITFIELDS:
			return(&m_pCurrentSpawn->m_u32Bitfields);

		/* lifetime dialog */

		case DATA_LIFE:
			return(&m_fLifetime);
		case DATA_LIFE_RANDOM:
			return(&m_fLifetimeRandomPct);
		case DATA_LEASH:
			return(&m_fLeash);
		case DATA_LEASH_RANDOM:
			return(&m_fLeashRandomPct);
		}
		ASSERT(FALSE);
		return(NULL);
	}

// operations

public:
	
	/* overload = operator to make copying particles easy. it is NOT ok to do a memcpy because the class contains CStrings as data */

	CParticle &operator=( CParticle & );

};

#endif // __PARTICLE_H
