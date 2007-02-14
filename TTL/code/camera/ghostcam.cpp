///////////////////////////////////////////////////////////////////////////////
// The third person shooter cam
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "Math/sfilter.h"
#include "GameHelper/SharedBody.h"
#include "Interfaces/ViewIF.h"
#include "Effects/scenesetup.h"
#include "Effects/halo.h"
#include "Effects/lensflare.h"
#include "main/TTLConstants.h"

// Our singleton
GhostCamera *g_ghostCamPtr = NULL;

#define GHOST_UP .15f

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CParam<GhostParams>::RegisterVariables()
{
   RegisterVariable(d_data.yHeight, "height", 2.2f, 0.0f, 10.0f);

   RegisterVariable(d_data.followDistance, "followDistance", 1.0f, 0.0f, 20.0f);

   RegisterVariable(d_data.minFollowDistance, "minFollowDistance", 0.0f, 0.0f, 20.0f);
   RegisterVariable(d_data.distanceTweak, "distanceTweak", 0.025f, 0.0f, 1.0f);
   RegisterVariable(d_data.followFilter, "followFilter", 0.01f, 0.0f, 1.0f);
   RegisterVariable(d_data.minHither, "minNearClip", 0.25f, 0.01f, 4.0f);

   RegisterVariable(d_data.normalFOV, "FOV", 80.0f, 10.0f, 120.0f);

   RegisterVariable(d_data.hither, "nearClip", 0.5f, 0.0f, 3000.0f);
   RegisterVariable(d_data.yon, "farClip", 300.0f, 0.0f, 3000.0f);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GhostWorldToView::GhostWorldToView( AvatarControlMapper &mapper , GhostParams *&params ) :
m_params(params),
m_input(mapper)
{
	m_ghost = INVALID_OBJECT_HANDLE;

	m_eye.Clear();
	m_focalPoint.Set(0.0f, 1.0f, 0.0f);
	m_up.Set(0.0f, 1.0f, 0.0f);

	m_followDistance = 1.0f;
	m_firstPersonLock = false;

	m_participantID = INVALID_PARTICIPANT;
	m_active = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GhostWorldToView::Initialize()
{
	m_followDistance = m_params->followDistance;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GhostWorldToView::SetFollow(CActor *follow)
{
	ActorHandle newGhost = CActor::ToHandle(follow);
	if (newGhost != m_ghost)
	{
		if (m_active)
		{
			g_lensFlare.SetIgnoreEntity(follow);
			CActor* oldTarget = CActor::FromHandle(m_ghost);
			if (oldTarget != NULL)
				oldTarget->Hide(false);
			g_halo.SetIgnoreEntity(NULL);
		}
		m_ghost = newGhost;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CActor *GhostWorldToView::GetFollow()
{
	return CActor::FromHandle(m_ghost);
}

///////////////////////////////////////////////////////////////////////////////
//
// Change to the next target. The order of precedence is to cycle through
// living teammates, then dead teammates, then living non-teammates, then
// dead non-teammates. If all of those fail, there is no one to look at.
//
///////////////////////////////////////////////////////////////////////////////
void GhostWorldToView::TargetNext(bool i_forceChange)
{
	// Get the status of the current target.
	uint8 currTargetID;
	bool currTargetAlive;
	CParticipantIF* partIF = CParticipantIF::GetInterface(m_ghost);
	if (partIF != NULL)
	{
		currTargetID = partIF->GetParticipantId();
		currTargetAlive = partIF->IsAliveOrDying();
	}
	else
	{
		currTargetID = INVALID_PARTICIPANT;
		currTargetAlive = false;
	}

	// If a change is not required and the current target is valid and alive,
	// return, even if he is not a teammate. Just changing because some better
	// target has appeared without any change in the current target could be
	// confusing.
	if (!i_forceChange && currTargetAlive)
		return;

	// See if he is a teammate.
	uint8 currTeammateID = g_referee.GetTeamingModel().AreTeammates(currTargetID,
			m_participantID, true) ? currTargetID : INVALID_PARTICIPANT;

	// Find the next teammate.
	uint8 nextTargetID = g_referee.GetTeamingModel().GetNextTeammate(
			m_participantID, currTeammateID, true, true, false);

	// Find the next living teammate.
	uint8 targetID = nextTargetID;
	uint8 firstTargetID = INVALID_PARTICIPANT;
	while((targetID != INVALID_PARTICIPANT) && (targetID != currTeammateID) &&
			(targetID != firstTargetID))
	{
		if (firstTargetID == INVALID_PARTICIPANT)
			firstTargetID = targetID;
		partIF = CParticipantIF::GetInterface(g_referee.GetActor(targetID));
		if (partIF && partIF->IsAliveOrDying())
		{
			SetFollow(g_referee.GetActor(targetID));
			return;
		}
		targetID = g_referee.GetTeamingModel().GetNextTeammate(
				m_participantID, targetID, true, true, false);
	}

	// No other living teammate was found. If the current target is alive and a
	// teammate, keep it as the target.
	if (currTargetAlive && (currTeammateID != INVALID_PARTICIPANT))
		return;

	// No one on the team is alive. If a change is not required and the current
	// target is valid, return, even he is not a teammate. If the only available
	// teammate is dead, switching from any target to a dead target with no
	// user action could be confusing.
	if (!i_forceChange && (currTargetID != INVALID_PARTICIPANT))
		return;

	// If there is some other (dead) teammate.
	if (nextTargetID != INVALID_PARTICIPANT)
	{
		SetFollow(g_referee.GetActor(nextTargetID));
		return;
	}

	// No other (dead) teammate was found. If the current target is a teammate,
	// keep it as the target.
	if (currTeammateID != INVALID_PARTICIPANT)
		return;

	// If a change is not required and the current target is valid, return,
	// even if he is dead. Just changing from a dead target to a living one
	// without any action by the user could be confusing.
	if (!i_forceChange && (currTargetID != INVALID_PARTICIPANT))
		return;

	// There are no team members which means that the current target must be
	// invalid or a non-teammate. Try to find a living, non-ghost, player.
	firstTargetID = INVALID_PARTICIPANT;
	nextTargetID = INVALID_PARTICIPANT;
	targetID = g_referee.GetTeamingModel().GetNextNonTeammate(
			m_participantID, currTargetID, true, true, false);
	while((targetID != INVALID_PARTICIPANT) && (targetID != currTargetID) &&
			(targetID != firstTargetID))
	{
		if (firstTargetID == INVALID_PARTICIPANT)
			firstTargetID = targetID;
		if (g_referee.IsPlayer(targetID))
		{
			if (nextTargetID == INVALID_PARTICIPANT)
				nextTargetID = targetID;
			partIF = CParticipantIF::GetInterface(g_referee.GetActor(targetID));
			if (partIF && partIF->IsAliveOrDying())
			{
				SetFollow(g_referee.GetActor(targetID));
				return;
			}
		}
		targetID = g_referee.GetTeamingModel().GetNextNonTeammate(
				m_participantID, targetID, true, true, false);
	}

	// No other living non-teammate was found. If the current target is alive,
	// keep it as the target.
	if (currTargetAlive)
		return;

	// There are no living non-teammates. If a change is required and there is
	// some other dead non-teammate, switch to him; otherwise, leave things as
	// they are.
	if (i_forceChange && (nextTargetID != INVALID_PARTICIPANT))
		SetFollow(g_referee.GetActor(nextTargetID));
}

///////////////////////////////////////////////////////////////////////////////
//
// Change to the prev target. The order of precedence is to cycle through
// living teammates, then dead teammates, then living non-teammates, then
// dead non-teammates. If all of those fail, there is no one to look at.
//
///////////////////////////////////////////////////////////////////////////////
void GhostWorldToView::TargetPrev(void)
{
	// Get the status of the current target.
	uint8 currTargetID;
	bool currTargetAlive;
	CParticipantIF* partIF = CParticipantIF::GetInterface(m_ghost);
	if (partIF != NULL)
	{
		currTargetID = partIF->GetParticipantId();
		currTargetAlive = partIF->IsAliveOrDying();
	}
	else
	{
		currTargetID = INVALID_PARTICIPANT;
		currTargetAlive = false;
	}

	// See if he is a teammate.
	uint8 currTeammateID = g_referee.GetTeamingModel().AreTeammates(currTargetID,
			m_participantID, true) ? currTargetID : INVALID_PARTICIPANT;

	// Find the prev teammate.
	uint8 prevTargetID = g_referee.GetTeamingModel().GetPrevTeammate(
			m_participantID, currTeammateID, true, true, false);

	// Find the prev living teammate.
	uint8 targetID = prevTargetID;
	uint8 firstTargetID = INVALID_PARTICIPANT;
	while((targetID != INVALID_PARTICIPANT) && (targetID != currTeammateID) &&
			(targetID != firstTargetID))
	{
		if (firstTargetID == INVALID_PARTICIPANT)
			firstTargetID = targetID;
		partIF = CParticipantIF::GetInterface(g_referee.GetActor(targetID));
		if (partIF && partIF->IsAliveOrDying())
		{
			SetFollow(g_referee.GetActor(targetID));
			return;
		}
		targetID = g_referee.GetTeamingModel().GetPrevTeammate(
				m_participantID, targetID, true, true, false);
	}

	// No other living teammate was found. If the current target is alive and a
	// teammate, keep it as the target.
	if (currTargetAlive && (currTeammateID != INVALID_PARTICIPANT))
		return;

	// If there is some other (dead) teammate.
	if (prevTargetID != INVALID_PARTICIPANT)
	{
		SetFollow(g_referee.GetActor(prevTargetID));
		return;
	}

	// No other (dead) teammate was found. If the current target is a teammate,
	// keep it as the target.
	if (currTeammateID != INVALID_PARTICIPANT)
		return;

	// There are no team members which means that the current target must be
	// invalid or a non-teammate. Try to find a living, non-ghost, player.
	firstTargetID = INVALID_PARTICIPANT;
	prevTargetID = INVALID_PARTICIPANT;
	targetID = g_referee.GetTeamingModel().GetPrevNonTeammate(
			m_participantID, currTargetID, true, true, false);
	while((targetID != INVALID_PARTICIPANT) && (targetID != currTargetID) &&
			(targetID != firstTargetID))
	{
		if (firstTargetID == INVALID_PARTICIPANT)
			firstTargetID = targetID;
		if (g_referee.IsPlayer(targetID))
		{
			if (prevTargetID == INVALID_PARTICIPANT)
				prevTargetID = targetID;
			partIF = CParticipantIF::GetInterface(g_referee.GetActor(targetID));
			if (partIF && partIF->IsAliveOrDying())
			{
				SetFollow(g_referee.GetActor(targetID));
				return;
			}
		}
		targetID = g_referee.GetTeamingModel().GetPrevNonTeammate(
				m_participantID, targetID, true, true, false);
	}

	// No other living non-teammate was found. If the current target is alive,
	// keep it as the target.
	if (currTargetAlive)
		return;

	// There are no living non-teammates. If there is some other dead
	// non-teammate, switch to him; otherwise, leave things as they are.
	if (prevTargetID != INVALID_PARTICIPANT)
		SetFollow(g_referee.GetActor(prevTargetID));
}

#define GCAM_TWEAK .35f
#define GSIDE_TWEAK .35f
#define GMIN_HEAD_DIST .5f
#define GR_TWEAK .5f
#define GU_TWEAK .0f

///////////////////////////////////////////////////////////////////////////////
// Update
///////////////////////////////////////////////////////////////////////////////
void GhostWorldToView::Update(Camera &parent)
{
	// Change targets if desired or necessary.
	if (m_input.ButtonPressed(AvatarControlMapper::CROUCH))
		TargetPrev();
	else
		TargetNext(m_input.ButtonPressed(AvatarControlMapper::CHANGEWEAPON));

	CActor* ghost = GetFollow();
	GhostCamera			&gc = (GhostCamera &)parent;

	// Get the view interface (will be NULL if there is no target).
	ViewIF* viewIF = ViewIF::GetInterface(m_ghost);

	// If there is no view interface, leave the camera where it is.
	if (viewIF == NULL)
		return;

	// Extract the data
	Vector3 dir;
	viewIF->GetOrientation(dir, m_up);
	Vector3					right = ghost->GetBodyToWorld().GetRow0();

	// Compute the eye
	Vector3 realHead;
	viewIF->GetEyepoint(realHead);

  	// Are we in too low of an area?
	Vector3 height(0.0f, 0.5f , 0.0f);
	float headRoom;
	if (Intersect(realHead, realHead + height, headRoom, false))
		height.Y(Math::Max(0.0f, headRoom - 1.5f * m_params->distanceTweak));

	Vector3 adjustedHead = realHead + height;
	m_eye = adjustedHead - m_params->followDistance * dir;

	// these values are used to keep the camera from going through the side walls.
	Vector3 lshoulder = m_eye - GR_TWEAK*right - GU_TWEAK*m_up;
	Vector3 rshoulder = m_eye + GR_TWEAK*right - GU_TWEAK*m_up;

	// Special case
	float followDistance, filter;
	bool filterOFF = false;
	float						dropR;

	{
		Vector3	oneup( 0.0f , GCAM_TWEAK , 0.0f );
		Vector3	side( 0.1f , 0.0 , 0.0f );
		float		shortestFollowDistance = 100000.0f;
		float		d, dr, dl;
		bool		b, br, bl;
		float		m;

		d = dl = dr = m_params->followDistance;
		b = Intersect( realHead , m_eye , d , true );
		br = Intersect( realHead , rshoulder , dr , true );
		bl = Intersect( realHead , lshoulder , dl , true );
		m = Math::Min( d , Math::Min( dr , dl ) );

		if(  b || br || bl )
		{
			shortestFollowDistance = Math::Min( shortestFollowDistance , Math::Max( 0.0f , m - GSIDE_TWEAK /*m_params->distanceTweak*/ ) );

			Vector3 tempEye = adjustedHead - shortestFollowDistance * dir;

			if( Intersect( realHead , tempEye + oneup , d , true ) )
			{
				float length = GCAM_TWEAK;
				float tl = ( tempEye + oneup - realHead ).Length();
				if( d < 0 )
					dropR = 0;
				else
					dropR = 1 - ( ( tl  - d ) / tl );
				float followDistanceTop = ( dropR * ( adjustedHead - m_eye ) ).Length();
				followDistanceTop = Math::Min( 0.0 , followDistanceTop - .25f );
				if( followDistanceTop < shortestFollowDistance )
				{
					float length = GCAM_TWEAK;

					adjustedHead = adjustedHead - ( 1 - dropR ) * ( adjustedHead - realHead );
					shortestFollowDistance = followDistanceTop;
				}
			}
		}
		else if( Intersect( realHead , m_eye + oneup , d , true ) )
		{
			float tl = ( m_eye + oneup - realHead ).Length();
			float length = GCAM_TWEAK;
			if( d < 0 )
				dropR = 0;
			else
				dropR = 1 - ( ( tl - d ) / tl );

			adjustedHead = adjustedHead - ( 1 - dropR ) * ( adjustedHead - realHead );
			shortestFollowDistance = ( dropR * ( adjustedHead - m_eye ) ).Length();
			shortestFollowDistance = Math::Max( 0.0 , shortestFollowDistance - .25f );
		}
		else
		{
			shortestFollowDistance = m_params->followDistance;
		}

		followDistance = shortestFollowDistance;

		if ( followDistance < m_followDistance || filterOFF )
			filter = 0.0f;
		else
			filter = m_params->followFilter;

		// Special hack
		if (followDistance < m_params->minFollowDistance)
		{
			followDistance = 0.0f;
			filter = m_params->followFilter;
		}
	}

	// Cheesey
	if (m_followDistance < m_params->minFollowDistance)
		filter /= 2.0f;

	// Filter our distance
	float deltaT = g_timer.GetFrameSec();
	m_followDistance = FOFilterImplicit(m_followDistance, followDistance, deltaT, filter);

	// adjust the eye
	m_eye = adjustedHead - m_followDistance * dir; // a quick guess for inhead check

	if( m_firstPersonLock || ((m_eye - realHead).Length() < GMIN_HEAD_DIST) )
	{
		m_eye = realHead - m_followDistance * dir;
		m_focalPoint = realHead + dir;
	}
	else
	{
		m_eye = adjustedHead - m_followDistance * dir;
		m_focalPoint = adjustedHead + dir;
	}

	// hide the shooter if: the camera is close enough to penetrate the shooter, the shooter
	// is in first person, or the sniper scope is being used.
	//bool inHead = (m_eye - realHead).Length() < m_params->minFollowDistance;
	bool inHead = (m_eye - realHead).Length() < GMIN_HEAD_DIST;

	gc.HideShooter( inHead );
}

///////////////////////////////////////////////////////////////////////////////
// Useful for targeting
///////////////////////////////////////////////////////////////////////////////
void GhostWorldToView::AimStartDir(
Vector3 &eye,
Vector3 &dir)
{
	// Get the interface
	ViewIF* viewIF = ViewIF::GetInterface(m_ghost);

	// Nothing to do if we don't have a shooter or it doesn't have a view
	// interface.
	if (viewIF == NULL)
	{
		eye.Clear();
		dir.Set(0.0f, 0.0f, 1.0f);
		return;
	}

	// Extract the data
	Vector3 up;
	viewIF->GetOrientation(dir, up);

	// Compute the eye
	Vector3 realHead;
	viewIF->GetEyepoint(realHead);
	Vector3 adjustedHead = realHead + Vector3(0.0f, m_params->yHeight, 0.0f);
	eye = adjustedHead - m_params->followDistance * dir;

	// Special case
	float followDistance;
	if (m_firstPersonLock)
		followDistance = 0.0f;
	else
		followDistance = m_params->followDistance;

	// Readjust the eye
	if (m_firstPersonLock)
		eye = realHead - m_followDistance * dir;
	else
		eye = adjustedHead - m_followDistance * dir;
}

///////////////////////////////////////////////////////////////////////////////
// Activate/deactivate
///////////////////////////////////////////////////////////////////////////////
void GhostWorldToView::Activate()
{
	m_active = true;
	g_lensFlare.SetIgnoreEntity(CActor::FromHandle(m_ghost));
}

///////////////////////////////////////////////////////////////////////////////
// Activate/deactivate
///////////////////////////////////////////////////////////////////////////////
void GhostWorldToView::Deactivate()
{
	if (m_active)
	{
		m_active = false;
		g_lensFlare.SetIgnoreEntity(NULL);
		CActor* target = CActor::FromHandle(m_ghost);
		// probaby should check if we hid them...
		if (target != NULL)
			target->Hide(false);
		g_halo.SetIgnoreEntity(NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Intersect with the world
///////////////////////////////////////////////////////////////////////////////
bool GhostWorldToView::Intersect(
Vector3CRef start,
Vector3CRef end,
float &d,
bool terrainOnly)
{
	CActor* ghost = GetFollow();
	CLineEntity line;
	line.Line().SetOriginEnd(start, end);
	line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_BOTH_SIDES);
	line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);

	line.SetIgnoreEntity(ghost);
	line.Category().Set( CAMERAQUERY_CATEGORY );
	line.SetPositiveInclude( WORLD_CATEGORY | FIXED_CATEGORY );

	// Do the test
	g_collisionSystem.ResetStack();
	CLineQuery query;
	if (!query.Test(line))
		return false;

   // What is the line length
	float length = (start - end).Length();

	// Give them back the info
	d = query.d * length;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GhostProjection::GhostProjection(
GhostParams *&params) :
m_params(params)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GhostProjection::Update(
Camera &parent)
{
 	// Is the frustum even closer
	float newHither = FrustumCollide(parent.ViewToWorld(), parent.Eye(), parent.Dir(), FOV(), m_params->hither);
	SetHither(Math::Max(newHither, m_params->minHither), true);

	DefaultProjectionController::Update(parent);
}


///////////////////////////////////////////////////////////////////////////////
// Fustrum check
///////////////////////////////////////////////////////////////////////////////
float GhostProjection::FrustumCollide(
const Cartesian4x4 &viewToWorld,
const Vector3 &eye,
const Vector3 &dir,
float fov,
float hither)
{
	// Do we need to adjust the front clipping plane?
	float halfFOVRad = Math::Deg2Rad(fov) * 0.5f;
	float halfHeight = Math::Tan(halfFOVRad) * hither;
	float halfWidth = halfHeight * AspectRatio();

	Vector3 frustumPoint[4];
	frustumPoint[0] = Vector3(halfWidth, halfHeight, -hither) * viewToWorld;
	frustumPoint[1] = Vector3(halfWidth, -halfHeight, -hither) * viewToWorld;
	frustumPoint[2] = Vector3(-halfWidth, -halfHeight, -hither) * viewToWorld;
	frustumPoint[3] = Vector3(-halfWidth, halfHeight, -hither) * viewToWorld;

	// Do the intersections
	float newHither = hither;

	for (int i = 0; i < 4; i++)
	{
		Vector3 intersection;
		if (Intersect(eye, frustumPoint[i], intersection, true))
		{
			float thisHither = Vector3::Dot(intersection - eye, dir);
			if (thisHither < newHither)
				newHither = thisHither;
		}
	}

#if 0
	if (newHither < m_params->hither)
	{
		if (newHither < m_params->minHither)
			TRACE("Oh shit, penetration may occur (hither %f).\n", m_params->minHither);
		else
			TRACE("Setting hither to %f.\n", newHither);
	}
#endif

	return newHither;
}

///////////////////////////////////////////////////////////////////////////////
// Intersect with the world
///////////////////////////////////////////////////////////////////////////////
bool GhostProjection::Intersect(
Vector3CRef start,
Vector3CRef end,
Vector3 &intersection,
bool terrainOnly)
{
	CLineEntity line;
	line.Line().SetOriginEnd(start, end);
	line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_BOTH_SIDES);
	line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	if (terrainOnly)
		line.SetPositiveInclude(WORLD_CATEGORY);

	// Do the test
	CLineQuery query;
	g_collisionSystem.ResetStack();
	if (!query.Test(line))
		return false;

	// What is the line length
	float length = (start - end).Length();

	// Give them back the info
	intersection = query.intersection;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Cameras must have at least a name ancmd w2v controller
///////////////////////////////////////////////////////////////////////////////
GhostCamera::GhostCamera( const char *name , AvatarControlMapper &controlMapper ) :
Camera(name, &m_ghostW2V, &m_ghostProj),
m_ghostW2V( controlMapper , m_params),
m_ghostProj(m_params)
{
	// Doh!
	ASSERT(g_ghostCamPtr == NULL);
	g_ghostCamPtr = this;

	// Holy magic (will return non-NULL)
	m_params = CParam<GhostParams>::GetParam("ghostcam", "var");

	// Filters ruin control
	SetEyePointFilter(0.0f);
	SetFocalPointFilter(0.0f);

	float yon = Math::Zero(m_params->yon) ? SceneSetup::ClipFar() : m_params->yon;
	m_ghostProj.SetHitherYon(m_params->hither, yon, true);
	m_ghostProj.SetFOV(m_params->normalFOV, true);

	m_ghostW2V.Initialize();

	m_inHead = false;
	m_isWeaponHidden = false;

	m_active = false;
}

///////////////////////////////////////////////////////////////////////////////
// Doh!
///////////////////////////////////////////////////////////////////////////////
GhostCamera::~GhostCamera()
{
	ASSERT(g_ghostCamPtr);
	g_ghostCamPtr = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Aiming state
///////////////////////////////////////////////////////////////////////////////
void GhostCamera::SetFOVFactor(
float FOVFactor,
float filter)
{
	m_ghostProj.SetFOV(FOVFactor * m_params->normalFOV);
	m_ghostProj.SetFOVFilter(filter);
}

///////////////////////////////////////////////////////////////////////////////
// hide/unhide shooter
///////////////////////////////////////////////////////////////////////////////
void GhostCamera::HideShooter( bool hide )
{
	CActor* ghost = m_ghostW2V.GetFollow();
	if (!ghost)
		return;

	g_halo.SetIgnoreEntity( hide ? ghost : NULL );

	// hide/unhide the actor using the body interface, so we don't stop
	// matrix updates which must keep happening so we don't have collision problems.
	SharedBody* body = (SharedBody*) ghost->Instance( 0 )->DynamicModel();
	ASSERT_PTR( body );
	body->SetDisableRender( hide );

	// hide the weapon instances, or unhide based on the weapon's hidden state
	if ( ghost->IsA( CCompActor::ClassType() ) )
	{
		CCompActor* ca = (CCompActor*) ghost;
		WeaponInventory* inv = (WeaponInventory*) ca->GetComponentByName( "WeaponInventory" );
		if( inv )
		{
			Weapon* weapon = inv->GetSelected();
			if( weapon )
			{
				if( hide )
				{
					if( !m_inHead )
						m_isWeaponHidden = weapon->IsHidden();
					weapon->Hide();
				}
				else
				{
					if( !m_isWeaponHidden )
					{
						m_isWeaponHidden = false;
						weapon->Show();
					}
				}
			}
		}
	}

//#ifdef DIRECTX_PC
//	// hide or move our weapons when in first person
//	if ( ghost->IsA( CCompActor::ClassType() ) )
//	{
//		CCompActor *ca = (CCompActor *)ghost;
//		WeaponInventory *inv = (WeaponInventory *)ca->GetComponentByName( "WeaponInventory" );
//		Weapon *weapon = inv->GetSelected();
//		Firearm *firearm = NULL;
//		if( weapon )
//			firearm = (Firearm *)weapon->GetInterface( "firearm" );
//		if( hide )
//		{
//			if ( firearm ) firearm->Hide();
//		}
//		else
//		{
//			if ( firearm ) firearm->Show();
//		}
//	}
//#endif

	m_inHead = hide;
}

///////////////////////////////////////////////////////////////////////////////
// Aiming state
///////////////////////////////////////////////////////////////////////////////
void GhostCamera::RestoreFOV(
float filter)
{
	m_ghostProj.SetFOV(m_params->normalFOV);
	m_ghostProj.SetFOVFilter(filter);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GhostCamera::Activate()
{
	m_active = true;
	m_ghostW2V.Activate();

	Camera::Activate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GhostCamera::Deactivate()
{
	Camera::Deactivate();

	m_active = false;
	m_ghostW2V.Deactivate();
}

