///////////////////////////////////////////////////////////////////////////////
// An actor orbit cam
///////////////////////////////////////////////////////////////////////////////
#include "camera/CameraPCH.h"
#include "GameHelper/ParamMgr.h"
#include "CollideHelper/SphereEntity.h"

extern	bool bSnapScreen;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CParam<CollidableControlCameraParams>::RegisterVariables()
{
   RegisterVariable(d_data.linearA, "linearA", 12.0, 0.0f, 100.0f);
   RegisterVariable(d_data.angularA, "angularA", 120.0, 0.0f, 360.0f);

   RegisterVariable(d_data.maxLinearV, "maxLinearV", 25.0, 0.0f, 100.0f);
   RegisterVariable(d_data.maxAngularV, "maxAngularV", 170.0, 0.0f, 360.0f);

   RegisterVariable(d_data.linearSlowdown, "linearSlowdown", 5.0, 0.0f, 100.0f);
   RegisterVariable(d_data.angularSlowdown, "angularSlowdown", 5.0, 0.0f, 100.0f);

   RegisterVariable(d_data.linearDampening, "linearDampening", 0.2f, 0.0f, 1.0f);
   RegisterVariable(d_data.angularDampening, "angularDampening", 0.2f, 0.0f, 1.0f);

   RegisterVariable(d_data.pitchClamp, "pitchClamp", 80.0f, 0.0f, 180.0f);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CollidableControlW2VC::CollidableControlW2VC(
DefaultControlMapper &mapper,
CollidableControlCameraParams *&params) :
m_input(mapper),
m_params(params)
{
	for (int i = 0; i < CONTROLS; i++)
		m_control[i].fixed = false;
	Reset();

	m_yaw = m_pitch = 0.0f;
	m_matrix.Identity();
	m_eye.Clear();
}

///////////////////////////////////////////////////////////////////////////////
// Update
///////////////////////////////////////////////////////////////////////////////
void CollidableControlW2VC::Update(
Camera &parent)
{
	CSphereEntity sphere;
	CCollisionList *list = NULL;
	Vector3 tempEye;

	float deltaT = g_timer.GetOSFrameSec();

	// Update the the quantities
	float deltaX = -UpdateLinearControl(m_control[STRAFE],
										 			m_input.GetAxis(DefaultControlMapper::AXIS_X1),
													deltaT);

	float deltaZ = UpdateLinearControl(m_control[FOREAFT],
													m_input.GetAxis(DefaultControlMapper::AXIS_Y1),
													deltaT);

	float deltaY = UpdateLinearControl(m_control[UPDOWN],
										 			ButtonPairToAxis(DefaultControlMapper::BUTTON_R2, DefaultControlMapper::BUTTON_R1),
													deltaT);

	float deltaYaw = UpdateAngularControl(m_control[YAW],
														m_input.GetAxis(DefaultControlMapper::AXIS_X2),
														deltaT);

	float deltaPitch = UpdateAngularControl(m_control[PITCH],
											  				m_input.GetAxis(DefaultControlMapper::AXIS_Y2),
															deltaT);

	// Now acutally update the matrix
	m_yaw += deltaYaw;
	m_pitch += deltaPitch;
	m_pitch = Math::Clamp(m_pitch, -m_params->pitchClamp, m_params->pitchClamp);

	m_matrix.SetYRotation(Math::Deg2Rad(m_yaw));
	m_matrix.CatXRotation(Math::Deg2Rad(m_pitch));

	// Factor in the chanage in translation
	Vector3 t(deltaX, deltaY, deltaZ);

	tempEye = m_eye + t / m_matrix;

	// check to see if the new camera position collides  with the world
	Vector3 cv( 0 , 0 , 0 );
	sphere.Sphere().Set( tempEye , 1 );
	list = g_collisionSystem.Test( &sphere );

	// Walk the events
	if( list != NULL )
	{
		CCollisionEvent	*event = list->GetHeadEvent();
		int					i;

		while( event ) 
		{
         CStageEntity *target = event->GetTargetEntity();
			i = 0;
			if( target->IsA( CTerrainEntity::ClassType() ) )
			{
				CTriangleListCollisionObject	*triangles = NULL;
				CCollisionIntersection			*intersection = event->GetHeadPair()->GetIntersection();
				switch(intersection->GetIntersectionType())
				{
					case CCollisionIntersection::SPHERE_TO_TERRAIN:
						triangles = &((CObbToTerrainIntersection*)intersection)->GetTriangleList();
					break;
				}
				Vector3 centroid;
				if( ( triangles != NULL ) && triangles->GetCentroid( 0 , centroid ) )
				{
					i++;
//					float minDistSq = (i_point - centroid).LengthSquared();
//					int index = 0;
//					int numTriangles = triangles->NumTriangles();
//					for (int i = 1; i < numTriangles; i++)
//					{
//						float distSq =
//								(i_point - triangles->UnsafeGetCentroid(i)).LengthSquared();
//						if (distSq < minDistSq)
//						{
//							minDistSq = distSq;
//							index = i;
//						}
//					}
//					o_targetType = triangles->UnsafeGetTriangle(index)->Material();
				}
			}
			event = event->GetNext();
		}
	}
	else
	{
		m_eye += t / m_matrix;
	}

	// ugly temp code to allow screenshots from freecam
	if (m_input.ButtonDown(DefaultControlMapper::BUTTON_START) && m_input.ButtonDown(DefaultControlMapper::BUTTON_L2))
		bSnapScreen = true;
}

///////////////////////////////////////////////////////////////////////////////
// Give back the all important info
///////////////////////////////////////////////////////////////////////////////
Vector3 CollidableControlW2VC::FocalPoint()
{
	return m_eye + m_matrix.GetColumn2();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector3 CollidableControlW2VC::Eye()
{
	return m_eye;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector3 CollidableControlW2VC::Up()
{
	return m_matrix.GetColumn1();
}

///////////////////////////////////////////////////////////////////////////////
// Reset everything
///////////////////////////////////////////////////////////////////////////////
void CollidableControlW2VC::Reset()
{
	for (int i = 0; i < CONTROLS; i++)
		m_control[i].v = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
// Setup the stuff
///////////////////////////////////////////////////////////////////////////////
void CollidableControlW2VC::Set(
const Vector3 &eye,
const Vector3 &focalPoint,
const Vector3 &up)
{
	Reset();

	m_eye = eye;

	Vector3 dir = focalPoint - eye;
	dir.SafeNormalize();

	// Figure out our initial pitch and yaw
	m_pitch = 0.0f;
	m_yaw = Math::Rad2Deg(-Math::ArcTan2(dir.X(), dir.Z()));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
float CollidableControlW2VC::UpdateLinearControl(
ControlInfo &info,
float input,
float deltaT)
{
	ASSERT(input >= -1.0f && input <= 1.0f);

	// Override the input
	if (info.fixed)
		input = info.fixedInput;

	// What is the acceleration?
	float a = m_params->linearA * input;

	// Slowing down
	if (!info.fixed && Math::Sign(a) != Math::Sign(info.v))
		a += info.v * -m_params->linearSlowdown;

	// Factor in dampnig
	a -= info.v * m_params->linearDampening;

	// compute the velocity
	info.v += a * deltaT;

	// Make sure we clamp
	info.v = Math::Clamp(info.v, -m_params->maxLinearV, m_params->maxLinearV);

	// Comptue the new position
	return info.v * deltaT;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
float CollidableControlW2VC::UpdateAngularControl(
ControlInfo &info,
float input,
float deltaT)
{
	ASSERT(input >= -1.0f && input <= 1.0f);

	// Override the input
	if (info.fixed)
		input = info.fixedInput;

	// What is the acceleration?
	float a = m_params->angularA * input;

	// Slowing down
	if (!info.fixed && Math::Sign(a) != Math::Sign(info.v))
		a += info.v * -m_params->angularSlowdown;

	// Factor in dampnig
	a -= info.v * m_params->angularDampening;

	// compute the velocity
	info.v += a * deltaT;

	// Make sure we clamp
	info.v = Math::Clamp(info.v, -m_params->maxAngularV, m_params->maxAngularV);

	// Comptue the new position
	return info.v * deltaT;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
float CollidableControlW2VC::ButtonPairToAxis(
DefaultControlMapper::DIGITALBUTTON down,
DefaultControlMapper::DIGITALBUTTON up)
{
	bool isDown = m_input.ButtonDown(down);
	bool isUp = m_input.ButtonDown(up);

	if (isDown == isUp)
		return 0.0f;
	else if (isDown)
		return -1.0f;
	else
		return 1.0f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CollidableControlCamera::CollidableControlCamera(
const char *name,
DefaultControlMapper &controlMapper,
ProjectionController *projectionController,
ViewportController *viewportController) :
Camera(name, &m_w2v, projectionController, viewportController),
m_mapper(controlMapper),
m_w2v(controlMapper, m_params)
{
	// Nothing to see here, move along
	SetEyePointFilter(0.01f);
	SetFocalPointFilter(0.01f);

	// Holy magic
	m_params = CParam<CollidableControlCameraParams>::GetParam(name, "var");
	ASSERT(m_params);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CollidableControlCamera::Activate()
{
	m_w2v.Set(m_eye, m_focalPoint, m_up);
}
