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
#ifdef DIRECTX_PC
#include "weapons/firearm.h"
#endif

// Our singleton
ShooterCamera *g_shooterCamPtr = NULL;

//#define SHOOTER_DEBUG // uncomment this to draw collsion lines and spheres from eye points to head
//#define COLLISION_DEBUG // uncomment this to start drawing collsion debug info and set up system to dectect ALL collision polys.

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CParam<ShooterParams>::RegisterVariables()
{
   RegisterVariable(d_data.yHeight, "height", 2.2f, 0.0f, 10.0f);

   RegisterVariable(d_data.followDistance, "followDistance", 1.0f, 0.0f, 20.0f);

   RegisterVariable(d_data.minFollowDistance, "minFollowDistance", 0.0f, 0.0f, 20.0f);
   RegisterVariable(d_data.distanceTweak, "distanceTweak", 0.025f, 0.0f, 1.0f);
   RegisterVariable(d_data.followFilter, "followFilter", 0.01f, 0.0f, 1.0f);
   RegisterVariable(d_data.firstPersonFilter, "firstPersonFilter", 0.0f, 0.0f, 1.0f);
   RegisterVariable(d_data.minHither, "minNearClip", 0.25f, 0.01f, 4.0f);

   RegisterVariable(d_data.normalFOV, "FOV", 80.0f, 10.0f, 120.0f);

   RegisterVariable(d_data.hither, "nearClip", 0.5f, 0.0f, 3000.0f);
   RegisterVariable(d_data.yon, "farClip", 300.0f, 0.0f, 3000.0f);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ShooterWorldToView::ShooterWorldToView(
ShooterParams *&params) :
m_params(params)
{
  	m_shooter = INVALID_OBJECT_HANDLE;

	m_eye.Clear();
	m_focalPoint.Set(0.0f, 1.0f, 0.0f);
	m_up.Set(0.0f, 1.0f, 0.0f);

	m_followDistance = 1.0f;

	m_firstPersonLock = false;

	m_targetLock.Clear();
	m_targetLocked = false;

	m_lastUpdate = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShooterWorldToView::Initialize()
{
	m_followDistance = m_params->followDistance;
}

///////////////////////////////////////////////////////////////////////////////
// Override normal functionality and lock an eye & target
///////////////////////////////////////////////////////////////////////////////
void ShooterWorldToView::LockToTarget(
const Vector3 &target)
{
	m_targetLock = target;
	m_targetLocked = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShooterWorldToView::UnlockTarget()
{
	ASSERT(m_targetLocked);
	m_targetLocked = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShooterWorldToView::Update(
Camera &parent)
{
	InternalUpdate(parent);
}

///////////////////////////////////////////////////////////////////////////////
// InternalUpdate
///////////////////////////////////////////////////////////////////////////////
float camTweak = .35f;
float sideTweak = .35f;
float minHeadDist = .75f;
float	rtweak = .5f;
float	utweak = .0f;

bool	lineDebug = false;

Vector3 tVec1;
Vector3 tVec2;
Vector3 tVec3;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static Vector3 lastPos;
bool ShooterWorldToView::InternalUpdate(
Camera &parent)
{
	CActor					*shooter = GetFollow();

	// Do we need to update?
	if (g_timer.GetFrameCount() == m_lastUpdate)
	{
		if (shooter)
		{
			float d = (shooter->GetBodyInWorld() - lastPos).LengthSquared();
			ASSERT(Math::Zero(d));
		}
		return false;
	}
	m_lastUpdate = g_timer.GetFrameCount();
	if (shooter)
		lastPos = shooter->GetBodyInWorld();

	// Work!
	bool						internalFPLock = false;
	float						dropR;
	Vector3					right;
	ShooterCamera			&sc = (ShooterCamera &)parent;

	// Nothing to do if we don't have a shooter
	if (!shooter || !m_params)
		return true;

	// Are we locked?
	if (m_targetLocked)
	{
		m_focalPoint = m_targetLock;
		m_up = shooter->GetBodyToWorld().GetRow1();
		return true;
	}

	// Get the interface
	ViewIF* viewIF = (ViewIF *)shooter->GetInterface(ViewIF::GetID());
	if (viewIF == NULL)
		return true;

	// Extract the data
	Vector3 dir;
	viewIF->GetOrientation(dir, m_up);
	right = shooter->GetBodyToWorld().GetRow0();

	// Compute the eye
	Vector3 realHead;
	viewIF->GetEyepoint(realHead);
	if (m_firstPersonLock)
	{
		const Vector3 bumpUp(0.0f, 0.15f, 0.0f);
		realHead += bumpUp;
	}
	//dbgPrint( "Eye X: %6.2f Y: %6.2f Z: %6.2f   Body X: %6.2f Y: %6.2f Z: %6.2f\n",
	//	realHead.x(), realHead.y(), realHead.z(),
	//	shooter->GetBodyInWorld().x(),
	//	shooter->GetBodyInWorld().y(),
	//	shooter->GetBodyInWorld().z() );

  	// Are we in too low of an area?
	Vector3 height(0.0f, 0.5f , 0.0f);
	float headRoom;
	if (Intersect(realHead, realHead + height, headRoom, false))
		height.Y(Math::Max(0.0f, headRoom - 1.5f * m_params->distanceTweak));

	Vector3 adjustedHead = realHead + height;
	m_eye = adjustedHead - m_params->followDistance * dir;

	// these values are used to keep the camera from going through the side walls.
	Vector3 lshoulder = m_eye - rtweak*right - utweak*m_up;
	Vector3 rshoulder = m_eye + rtweak*right - utweak*m_up;

	// Special case
	float followDistance, filter;
	bool filterOFF = false;

	if (m_firstPersonLock)
	{
		followDistance = 0.0f;
		filter = m_params->firstPersonFilter;
	}
	else
	{
		Vector3	oneup( 0.0f , camTweak , 0.0f );
		Vector3	side( 0.1f , 0.0 , 0.0f );
		float		shortestFollowDistance = 100000.0f;
		float		d, dr, dl;
		bool		b, br, bl;
		float		m;

#ifdef SHOOTER_DEBUG
		DrawUtility::ClearQueue();
		DrawUtility::DrawQueue();
		DrawUtility::QueueSphere( realHead , DrawUtility::RED , .1f );
		DrawUtility::QueueSphere( rshoulder , DrawUtility::RED , .1f );
		DrawUtility::QueueSphere( lshoulder , DrawUtility::RED , .1f );
		DrawUtility::QueueSphere( m_eye , DrawUtility::YELLOW , .1f );
		DrawUtility::QueueLine( realHead , rshoulder , DrawUtility::WHITE );
		DrawUtility::QueueLine( realHead , m_eye , DrawUtility::WHITE );
		DrawUtility::QueueLine( realHead , lshoulder , DrawUtility::WHITE );
#endif

		d = dl = dr = m_params->followDistance;
		lineDebug = true;
		b = Intersect( realHead , m_eye , d , true );
		lineDebug = false;
		br = Intersect( realHead , rshoulder , dr , true );
		bl = Intersect( realHead , lshoulder , dl , true );
		m = Math::Min( d , Math::Min( dr , dl ) );

		if(  b || br || bl )
		{
			shortestFollowDistance = Math::Min( shortestFollowDistance , Math::Max( 0.0f , m - sideTweak /*m_params->distanceTweak*/ ) );

			Vector3 tempEye = adjustedHead - shortestFollowDistance * dir;

			if( Intersect( realHead , tempEye + oneup , d , true ) )
			{
				float length = camTweak;
				float tl = ( tempEye + oneup - realHead ).Length();
				if( d < 0 )
					dropR = 0;
				else
					dropR = 1 - ( ( tl  - d ) / tl );
				float followDistanceTop = ( dropR * ( adjustedHead - m_eye ) ).Length();
				followDistanceTop = Math::Min( 0.0 , followDistanceTop - .25f );
				if( followDistanceTop < shortestFollowDistance )
				{
					float length = camTweak;

					adjustedHead = adjustedHead - ( 1 - dropR ) * ( adjustedHead - realHead );
					shortestFollowDistance = followDistanceTop;
					//filterOFF = true;
					//internalFPLock = true;
				}
			}
		}
		else if( Intersect( realHead , m_eye + oneup , d , true ) )
		{
			float tl = ( m_eye + oneup - realHead ).Length();
			float length = camTweak;
			if( d < 0 )
				dropR = 0;
			else
				dropR = 1 - ( ( tl - d ) / tl );

			adjustedHead = adjustedHead - ( 1 - dropR ) * ( adjustedHead - realHead );
			shortestFollowDistance = ( dropR * ( adjustedHead - m_eye ) ).Length();
			shortestFollowDistance = Math::Max( 0.0 , shortestFollowDistance - .25f );
			//filterOFF = true;
			//internalFPLock = true;
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

	// designer is requesting control
	if( sc.isInCameraControl() )
	{
		followDistance = 0.0;
	}

	// force first person
	//test code m_firstPersonLock = true;
	//test code followDistance = 0;

	// Filter our distance
	float deltaT = g_timer.GetFrameSec();
	m_followDistance = FOFilterImplicit(m_followDistance, followDistance, deltaT, filter);

	// adjust the eye
	m_eye = adjustedHead - m_followDistance * dir; // a quick guess for inhead check

	Vector3 testPos;
	if( m_firstPersonLock || ( internalFPLock && ((m_eye - realHead).Length() < minHeadDist) ) )
	{
		m_eye = realHead - m_followDistance * dir;
		m_focalPoint = realHead + dir;
		testPos = realHead;
	}
	else
	{
		m_eye = adjustedHead - m_followDistance * dir;
		m_focalPoint = adjustedHead + dir;
		testPos = adjustedHead;
	}

	tVec1 = m_eye;

#ifdef SHOOTER_DEBUG
	DrawUtility::QueueSphere( tVec1 , DrawUtility::GREEN , .1f );
#endif

	// hide the shooter if: the camera is close enough to penetrate the shooter, the shooter
	// is in first person, or the sniper scope is being used.
	//bool inHead = (m_eye - realHead).Length() < m_params->minFollowDistance;
	bool inHead = (m_eye - testPos).Length() < minHeadDist;

	sc.HideShooter( inHead );

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Useful for targeting
///////////////////////////////////////////////////////////////////////////////
void ShooterWorldToView::AimOriginDir(
Camera &parent,
Vector3 &origin,
Vector3 &dir)
{
	InternalUpdate(parent);
	origin = m_eye;
	dir = (m_focalPoint - m_eye).Normalized();
}

///////////////////////////////////////////////////////////////////////////////
// Activate/deactivate
///////////////////////////////////////////////////////////////////////////////
void ShooterWorldToView::Activate()
{
	g_lensFlare.SetIgnoreEntity(GetFollow());
}

///////////////////////////////////////////////////////////////////////////////
// Activate/deactivate
///////////////////////////////////////////////////////////////////////////////
void ShooterWorldToView::Deactivate()
{
	g_lensFlare.SetIgnoreEntity(NULL);
}

///////////////////////////////////////////////////////////////////////////////
// Intersect with the world
///////////////////////////////////////////////////////////////////////////////
bool ShooterWorldToView::Intersect(
Vector3CRef start,
Vector3CRef end,
float &d,
bool terrainOnly)
{
	CActor* shooter = GetFollow();
	CLineEntity line;
	float length = (end-start).Length();

	if( length == 0 )
		return false;

	//line.Line().SetOriginEnd(start, end);
	line.Line().SetOriginBasis( start, (end - start)/length );
	line.Line().SetMaxBoundary( length );
	line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_BOTH_SIDES);
//#ifdef COLLISION_DEBUG
//	line.Line().SetFindMode(CLineCollisionObject::FIND_ALL);
//#else
	line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
//#endif

	line.SetIgnoreEntity(shooter);
//	line.Category().Set( CAMERAQUERY_CATEGORY );
//	line.SetPositiveInclude( WORLD_CATEGORY | FIXED_CATEGORY );
	line.Category().Set( CAMERAQUERY_CATEGORY | QUERY_CATEGORY );
	line.SetPositiveInclude( ALL_CATEGORIES );
	line.SetPositiveExclude( TRANSPARENT_CATEGORY | NO_WEAPON_HIT_CATEGORY | PLAYER_CATEGORY | AI_CATEGORY | SUBSONIC_CATEGORY );
	// Do the test
	g_collisionSystem.ResetStack();
	CLineQuery query;
	if (!query.Test(line))
		return false;

//	CCollisionList *cl = g_collisionSystem.Test(&line);
//	if (cl == NULL)
//		return false;

#if 0
	// TEMP: find our d from a list of any collisions.
	float					td = 10000;
	float					df = 10000;
	CCollisionEvent	*ce = cl->GetHeadEvent();
	while( ce!=NULL )
	{
		CCollisionPair *collPair = ce->GetHeadPair();
		while( collPair!=NULL )
		{

			switch( collPair->GetIntersectionType() )
			{
				case CCollisionIntersection::LINE_TO_TERRAIN:
				{
					CLineToTerrainPoint* terrainPoint =	((CLineToTerrainIntersection*)collPair)->GetFoundPoint();
					if( terrainPoint!=NULL )
						df = terrainPoint->Frac();
					break;
				}
				case CCollisionIntersection::LINE_TO_OBB:
				{
					CLineToObbIntersection* obbPoint = (CLineToObbIntersection*)collPair;
					if( obbPoint!=NULL )
						df = ( obbPoint->ExitFrac()<obbPoint->EntryFrac() ) ? obbPoint->ExitFrac() : obbPoint->EntryFrac();
					break;
				 }
			}
			if( df < td )
				td = df;

			collPair = collPair->GetNext();
		}

		ce = ce->GetNext();
	}

	if( td == 10000 )
		d = 1.0;
	else
		d = td;

	d = d * length;
#else
	// Give them back the info
	d = query.d;
#endif

#ifdef SHOOTER_DEBUG
	DrawUtility::QueueSphere( query.intersection , DrawUtility::BLUE , .1f );
#endif

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ShooterProjection::ShooterProjection(
ShooterParams *&params) :
m_params(params)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShooterProjection::Update(
Camera &parent)
{
 	// Is the frustum even closer
	float newHither = FrustumCollide(parent.ViewToWorld(), parent.Eye(), parent.Dir(), FOV(), m_params->hither);
	SetHither(Math::Max(newHither, m_params->minHither), true);
	//SetHither(.05f, true); // this value is used so the camera doesn't clip through cars

	DefaultProjectionController::Update(parent);
}


///////////////////////////////////////////////////////////////////////////////
// Fustrum check
///////////////////////////////////////////////////////////////////////////////
float ShooterProjection::FrustumCollide(
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

	Vector3 testPoint[2];
	testPoint[0] = (frustumPoint[0] + frustumPoint[1]) * 0.5f;
	testPoint[1] = (frustumPoint[2] + frustumPoint[3]) * 0.5f;

	// Do the intersections
	float newHither = hither;

	for (int i = 0; i < 2; i++)
	{
		Vector3 intersection;
		if (Intersect(eye, testPoint[i], intersection, true))
		{
			float thisHither = Vector3::Dot(intersection - eye, dir);
			if (thisHither < newHither)
				newHither = thisHither;
		}
	}

#ifdef _DEBUG
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
bool ShooterProjection::Intersect(
Vector3CRef start,
Vector3CRef end,
Vector3 &intersection,
bool terrainOnly)
{
	CLineEntity line;
	line.Line().SetOriginEnd(start, end);
	line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_BOTH_SIDES);
	line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	line.Category().Set(CAMERAQUERY_CATEGORY);

	if (terrainOnly)
		line.SetPositiveInclude(WORLD_CATEGORY);
	else
		line.SetPositiveInclude(ALL_CATEGORIES);

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
ShooterCamera::ShooterCamera(
const char *name) :
Camera(name, &m_shooterW2V, &m_shooterProj),
m_shooterW2V(m_params),
m_shooterProj(m_params)
{
	// Doh!
	ASSERT(g_shooterCamPtr == NULL);
	g_shooterCamPtr = this;

	// Name the message owner.
	d_messageOwner.SetOwnerName("ShooterCamera", name);

	// Holy magic
	m_params = CParam<ShooterParams>::GetParam("shootercam", "var");

	// Filters ruin control
	SetEyePointFilter(0.0f);
	SetFocalPointFilter(0.0f);
	inCameraControl = false;

	if (m_params)
	{
		float yon = Math::Zero(m_params->yon) ? SceneSetup::ClipFar() : m_params->yon;
		m_shooterProj.SetHitherYon(m_params->hither, yon, true);
		m_shooterProj.SetFOV(m_params->normalFOV, true);
	}

	m_shooterW2V.Initialize();

	d_enterCameraControlMessageHandler.Initialize( "EnterCameraControl" , MESSAGE_HANDLE_IF_MASTER , d_messageOwner , this );
	d_exitCameraControlMessageHandler.Initialize( "ExitCameraControl" , MESSAGE_HANDLE_IF_MASTER , d_messageOwner , this );

	d_startCinematicMessageHandler.Initialize( "StartCinematic" , MESSAGE_HANDLE_IF_MASTER , d_messageOwner , this );
	d_stopCinematicMessageHandler.Initialize( "StopCinematic" , MESSAGE_HANDLE_IF_MASTER , d_messageOwner , this );

	m_active = false;

	m_inHead = false;
	m_isWeaponHidden = false;
}

///////////////////////////////////////////////////////////////////////////////
// Doh!
///////////////////////////////////////////////////////////////////////////////
ShooterCamera::~ShooterCamera()
{
	ASSERT(g_shooterCamPtr);
	g_shooterCamPtr = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Aiming state
///////////////////////////////////////////////////////////////////////////////
void ShooterCamera::SetFOVFactor(
float FOVFactor,
float filter)
{
	m_shooterProj.SetFOV(FOVFactor * m_params->normalFOV);
	m_shooterProj.SetFOVFilter(filter);
}

///////////////////////////////////////////////////////////////////////////////
// Aiming state
///////////////////////////////////////////////////////////////////////////////
void ShooterCamera::RestoreFOV(
float filter)
{
	m_shooterProj.SetFOV(m_params->normalFOV);
	m_shooterProj.SetFOVFilter(filter);
}

///////////////////////////////////////////////////////////////////////////////
// hide/unhide shooter
///////////////////////////////////////////////////////////////////////////////
void ShooterCamera::HideShooter( bool hide )
{
	CActor* shooter = m_shooterW2V.GetFollow();
	if (!shooter)
		return;

	g_halo.SetIgnoreEntity( hide ? shooter : NULL );

	// hide/unhide the actor using the body interface, so we don't stop
	// matrix updates which must keep happening so we don't have collision problems.
	SharedBody* body = (SharedBody*) shooter->Instance( 0 )->DynamicModel();
	ASSERT_PTR( body );
	body->SetDisableRender( hide );

	// hide the weapon instances, or unhide based on the weapon's hidden state
	if ( shooter->IsA( CCompActor::ClassType() ) )
	{
		CCompActor* ca = (CCompActor*) shooter;
		WeaponInventory* inv = (WeaponInventory*) ca->GetComponentByName( "WeaponInventory" );
		if( inv )
		{
			Weapon* weapon = inv->GetSelected();
			if( weapon )
			{
				if( hide )
				{
					weapon->HideLock( true );
				}
				else
				{
					weapon->HideLock( false );
				}
			}
		}
	}

//#ifdef DIRECTX_PC
//	// hide or move our weapons when in first person
//	if ( shooter->IsA( CCompActor::ClassType() ) )
//	{
//		CCompActor *ca = (CCompActor *)shooter;
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
// Override normal functionality and lock an eye & target
///////////////////////////////////////////////////////////////////////////////
void ShooterCamera::LockToTarget(const Vector3 &target)
{
	if (!m_shooterW2V.TargetLocked())
	{
		m_targetLockSaveEyeFilter = GetEyePointFilter();
		m_targetLockSaveFocalFilter = GetFocalPointFilter();
		SetEyePointFilter(0.5f);
		SetFocalPointFilter(0.5f);
	}
	m_shooterW2V.LockToTarget(target);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShooterCamera::UnlockTarget()
{
	if (m_shooterW2V.TargetLocked())
	{
		SetEyePointFilter(m_targetLockSaveEyeFilter);
		SetFocalPointFilter(m_targetLockSaveFocalFilter);
		m_shooterW2V.UnlockTarget();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ShooterCamera::LockedToTarget()
{
	return m_shooterW2V.TargetLocked();
}

///////////////////////////////////////////////////////////////////////////////
// Override camera update
///////////////////////////////////////////////////////////////////////////////
void ShooterCamera::Update()
{
	// Call the parent
	Camera::Update();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShooterCamera::Activate()
{
	m_active = true;
	m_shooterW2V.Activate();

	Camera::Activate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShooterCamera::Deactivate()
{
	Camera::Deactivate();

	m_active = false;
	m_shooterW2V.Deactivate();
	HideShooter(false);
}

void ShooterCamera::EnterCameraControl::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	d_ref->inCameraControl = true;
	d_ref->controlData = (char *)i_data;
}

void ShooterCamera::ExitCameraControl::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	d_ref->inCameraControl = false;
}

void ShooterCamera::StartCinematic::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	// save the visiblity state of the shooter.  The visibility of the shooter turned off when usning the
	// first person perspective and the sniper gun.  When a cutscene is triggered the player needs to become
	// visible in case one of the cutscene cameras focuses on the player.
	CActor* shooter = d_ref->m_shooterW2V.GetFollow();
	ASSERT_PTR(shooter);
	if (!shooter)
		return;
	SharedBody* body = (SharedBody*) shooter->Instance( 0 )->DynamicModel();
	ASSERT_PTR( body );
	d_ref->m_hideShooter = body->DisableRender();

	// show the shooter
	d_ref->HideShooter( false );
}

void ShooterCamera::StopCinematic::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	// resrote the visibility state of the shooter.
	d_ref->HideShooter( d_ref->m_hideShooter );
}

