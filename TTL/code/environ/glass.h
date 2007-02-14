///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifndef __GLASS_H
#define __GLASS_H

#include "Layers/LayersStruct.h"
#include "GameHelper/Message.h"
#include "CollideHelper/ObbNode.h"
#include "Effects/shatter.h"
#include "Effects/damageanim.h"

// Maximum number of bullets in a pane
#define MAX_BULLETS_PER_PANE	5

// Define this for bulletholes (they don't work so well)
//#define WANT_BULLETHOLES

// forwards
struct GlassParams;

class CGlass : public CProtagonist
{
public:
	CGlass();
	~CGlass();

	// Class
	static const CEntityType &ClassType()
		{ return(s_glassClass); }

	// overloaded funcs
	const char* GetClassName() const
		{ return "Glass"; }

	bool Initialize();
	void InitializeVisualProperties(void);

	void Reset();

#ifdef WANT_BULLETHOLES
	void EndUpdate(float i_deltaSec);
#endif

	void CollisionAccept( CStageEntity* i_entity, CCollisionEvent* i_event );

	uint8 Material(CCollisionPair *i_pair)
		{ return m_material; }

	// Network
	CNetActor *CreateNetActor();

	// What is our state?
	bool ShatterGlass();
	bool GlassShattered ()
		{ return m_shattered; }

protected:
	// Our collision object
	CObbNode						m_obbNode;

	// Our parameters
	GlassParams					*m_params;

	// Bullets associated with this pane
	struct BulletHole
	{
		Vector3		position;				// position
#ifdef WANT_BULLETHOLES
		t_Handle		handle;					// bullet handles for particle system
		Vector3		normal;					// normal
		Vector3		rotation;				// rotation
		float			scale;					// size
		float			scaleTarget;			// target size
#endif
	};
	BulletHole					m_hole[MAX_BULLETS_PER_PANE];
	int							m_holes;
#ifdef WANT_BULLETHOLES
	int							m_animatingHoles;
#endif

	// Our material (glass)
	uint8							m_material;
	Vector3 						m_normal;							// normal of this plane
	Vector3						m_corner[4];			 			// save the corners

	// Our health!
	float							m_fullHealth, m_health;

	// Shatter class pointer
	bool							m_shattered; 						// Have we shattered

	// message
	const char*					m_messageDie;						// shatter glass if message received

	// Glass Functions
	bool AddBulletToPane(Vector3CRef Position, float i_damage, CActor* i_sender);	// adds a bullethole to the render pipeline
	bool ThresholdReached();										// Shatter?

	// Wee
	enum Axis
	{
		AXIS_X,
		AXIS_Y,
		AXIS_Z
	};
	Axis MinimumAxis(const OBB &obb);
	void ComputeCorners(const OBB &obb, Axis minAxis, Vector3 corner[4]);
	void InflateSmallAxis(CObbNode &node, Axis minAxis, float obbDepth);
	void ComputeDimensions(const OBB &obb, Axis minAxis,
									Vector3 &right, Vector3 &up,
									float &width, float &height);

	// Animation support
	DamageAnimator m_damageAnimator;

	// Message handling
	REF_SEND_MESSAGE_HANDLER(MunitionImpact, CGlass) m_munitionImpactHandler;
	REF_SEND_MESSAGE_HANDLER(MunitionImpactFX, CGlass) m_munitionImpactFXHandler;
	REF_SEND_MESSAGE_HANDLER(CShatter, CGlass) m_shatterHandler;
	REF_SEND_MESSAGE_HANDLER(CDeath, CGlass) m_deathHandler;
	REF_BASIC_MESSAGE_HANDLER(CheckpointReset, MESSAGE_VALUE, CGlass) m_checkpointResetHandler;

private:
	// Class
	static const CEntityType s_glassClass;
};

#endif // __GLASS_H

