///////////////////////////////////////////////////////////////////////////////
// A simple death camera
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CParam<DeathCamParams>::RegisterVariables()
{
   RegisterVariable(d_data.focalPointYTweak, "focalPointYTweak", 0.5f, 0.0f, 10.0f);
   RegisterVariable(d_data.hitTweak, "hitTweak", 0.15f, 0.0f, 10.0f);
   RegisterVariable(d_data.minD, "minD", 0.5f, 0.0f, 10.0f);
   RegisterVariable(d_data.distance, "distance", 0.5f, 0.0f, 10.0f);
   RegisterVariable(d_data.height, "height", 0.5f, 0.0f, 10.0f);
   RegisterVariable(d_data.rotationSpeed, "rotationSpeed", 0.5f, 0.0f, 10.0f);
	RegisterVariable(d_data.focalBone, "focalBone", "spine", sizeof(d_data.focalBone) - 1);
   RegisterVariable(d_data.resetSlowMo, "resetSlowMo", 3.0f, 0.0f, 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DeathWTVC::DeathWTVC(
DeathCamParams *&params) :
m_params(params)
{
	m_target = NULL;
	m_angle = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DeathWTVC::SetTarget(
CActor *target)
{
	m_target = target;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DeathWTVC::Update(
Camera &parent)
{
	if (!m_params || !m_target)
		return;

	if( !UseStaticDeathCam() )
	{
		// Update the angle
		m_angle += Math::Deg2Rad(m_params->rotationSpeed) * g_timer.GetFrameSec();

		// Make a vector out of the angle in xz
		float s, c;
		Math::SinCos(m_angle, s, c);
 		Vector3 dir(s, 0.0f, c);;

		// What is the up?
		m_up = m_target->GetBodyToWorld().GetRow1();

		// What is our focal point?
		m_focal = GetBoneInWorld(*m_target, m_params->focalBone) + Vector3(0.0f, m_params->focalPointYTweak, 0.0f);

		// Compute the eye!
		m_eye = m_focal + (m_params->height * m_up) + (m_params->distance * dir);
	}
	else
	{
		// What is the up?
		m_up = m_target->GetBodyToWorld().GetRow1();

		// What is our focal point?
		m_focal = GetBoneInWorld(*m_target, m_params->focalBone) + Vector3(0.0f, m_params->focalPointYTweak, 0.0f);

		// Compute the eye!
		m_eye = m_focal + (m_params->height * ( m_up + Vector3( .1f , 0 , 0 ) ) );// + (m_params->distance * Vector3( 0.0f , 1.0f , 0.0f ) );
	}

	// Do we have an intersection?
	float d;
	Vector3 intersection;
	if (Intersect(m_focal, m_eye, d, intersection))
	{
		m_eye = intersection;
		Vector3 dir = m_focal - intersection;
		float length = dir.Length();
		if (Math::Zero(length))
			m_eye = m_focal + m_up * m_params->minD;
		else
		{
			dir /= length;
			if (length < m_params->minD)
				m_eye = intersection + dir * m_params->minD;
			else
				m_eye = intersection + dir * m_params->hitTweak;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Intersect with the world
///////////////////////////////////////////////////////////////////////////////
bool DeathWTVC::Intersect(
Vector3CRef start,
Vector3CRef end,
float &d,
Vector3 &intersection)
{
	CLineEntity line;
	line.Line().SetOriginEnd(start, end);
	line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_BOTH_SIDES);
	line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	line.SetIgnoreEntity(m_target);
	line.SetPositiveInclude(WORLD_CATEGORY);

	// Do the test
	CLineQuery query;
	g_collisionSystem.ResetStack();
	if (!query.Test(line))
		return false;

	// What is the line length
	float length = (start - end).Length();

	// Give them back the info
	d = query.d * length;
	intersection = query.intersection;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector3 DeathWTVC::GetBoneInWorld(
CActor &forWho,
const char *boneName)
{
	Vector3 pos;

	if (!boneName || !boneName[0])
		pos = forWho.GetBodyInWorld();
	else
	{
		BoneID boneID;
		int ret = boneGetBoneID(forWho.Instance()->Body(), &boneID, (char *)boneName);
		if (!ret)
			pos = forWho.GetBodyInWorld();
		else
		{
			boneGetStartPosition(forWho.Instance()->Body(), boneID, &pos);
			pos = pos * forWho.Instance()->Matrix();
		}
	}

	return pos;
}

///////////////////////////////////////////////////////////////////////////////
// Cameras must have at least a name and w2v controller
///////////////////////////////////////////////////////////////////////////////
DeathCamera::DeathCamera(
const char *name,
ProjectionController *projectionC,
ViewportController *viewportC) :
Camera(name, &m_death, projectionC, viewportC),
m_death(m_params)
{
	// Holy magic
	m_params = CParam<DeathCamParams>::GetParam("deathcam", "var");

	// Reset the timer?
	m_resetSlowMo = -1.0f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DeathCamera::Activate()
{
	CSphereEntity	sphere;
	CCollisionList *list = NULL;
	int				i = 0;

	SetFocalPointFilter( .2f );
	SetEyePointFilter( .2f );

	m_death.ClearUseStaticDeathCam();

	// check to see if the new camera position collides  with the world
	Vector3 cv( 0 , 0 , 0 );
	sphere.Sphere().Set( m_target->GetBodyInWorld() + Vector3( 0 , 2.6f , 0 ) , 1.5f );
	sphere.SetPositiveInclude( TERRAIN_CATEGORY );

	list = g_collisionSystem.Test( &sphere );

	// Walk the events
	if( list != NULL )
	{
		CCollisionEvent *e = list->GetHeadEvent();
		while( e != NULL )
		{
         CStageEntity *target = e->GetTargetEntity();

			if( target->IsA( CTerrainEntity::ClassType() ) )
			{
				m_death.SetUseStaticDeathCam();
				break;
			}
			e = e->GetNext();
		}
	}

	m_clear = Display__GetClearMode();
	Display__SetClearMode(true);

	int camera = g_random.UpTo(2);
	m_resetSlowMo = -1.0f;
	if (camera == 0)
	{
		int speed = (int)CMasterTimer::ONE_FOURTH | 0x80000000;
		g_messageDispatcher.SendMessageToAll("TimeSpeed", CMessageDispatcher::IntToData(speed), INVALID_OBJECT_HANDLE);
		m_resetSlowMo = g_timer.GetEndSec() + m_params->resetSlowMo / 2.0f;
	}
	else if (camera == 1)
	{
		int speed = (int)CMasterTimer::ONE_HALF | 0x80000000;
		g_messageDispatcher.SendMessageToAll("TimeSpeed", CMessageDispatcher::IntToData(speed), INVALID_OBJECT_HANDLE);
		m_resetSlowMo = g_timer.GetEndSec() + m_params->resetSlowMo;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DeathCamera::Deactivate()
{
	Display__SetClearMode((m_clear & RS_COLOR_BUFFER_BIT));
	int speed = (int)CMasterTimer::ONE_X | 0x80000000;
	g_messageDispatcher.SendMessageToAll("TimeSpeed", CMessageDispatcher::IntToData(speed), INVALID_OBJECT_HANDLE);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DeathCamera::Update()
{
	// Call the parent
	Camera::Update();

	if (m_resetSlowMo >= 0.0f && g_timer.GetEndSec() >= m_resetSlowMo)	
	{
		int speed = (int)CMasterTimer::ONE_X | 0x80000000;
		g_messageDispatcher.SendMessageToAll("TimeSpeed", CMessageDispatcher::IntToData(speed), INVALID_OBJECT_HANDLE);
		m_resetSlowMo = -1.0f;
	}
	
}
