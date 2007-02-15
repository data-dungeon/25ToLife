///////////////////////////////////////////////////////////////////////////////
// The controller cam
///////////////////////////////////////////////////////////////////////////////
#ifndef COLLIDABLECONTROLCAM_H
#define COLLIDABLECONTROLCAM_H

#include "camera/camera.h"
#include "camera/util/flextarg.h"
#include "platform/defcontrolmap.h"

// The settings for the camera
struct CollidableControlCameraParams
{
	float		linearA;			// Linear acceleration
	float		angularA;		// Angular accel in degrees

	float		maxLinearV;
	float		maxAngularV;

	float		linearSlowdown;
	float		angularSlowdown;

	float		linearDampening;
	float		angularDampening;

	float		pitchClamp;
};

class CollidableControlW2VC : public FlexTargetW2VC
{
public:
	CollidableControlW2VC(DefaultControlMapper &mapper, CollidableControlCameraParams *&params);
	~CollidableControlW2VC() {}

	// The controls we support
	enum Control
	{
		STRAFE,				// Stick1 X
		FOREAFT,				// Stick1 Y
		YAW,            	// Stick2 X
		PITCH,         	// Stick2 Y
		UPDOWN,				// R1/R2

		CONTROLS
	};

  	// Update
	void Update(Camera &parent);

	// Give back the all important info
	Vector3 FocalPoint();
	Vector3 Eye();
	Vector3 Up();

	// Reset everything
	void Reset();

	// Setup the stuff
	void Set(const Vector3 &eye, const Vector3 &focalPoint, const Vector3 &up);

	void SetEye( Vector3CRef eye ) { m_eye = eye; }

private:
	// The controller
	DefaultControlMapper			&m_input;

	CollidableControlCameraParams			*&m_params;

	// our rates
	struct ControlInfo
	{
		bool		fixed;			// Is this control enabled?
		float		fixedInput;		// Give it a fixed input
		float		v;					// current velocity
	};
	ControlInfo	m_control[CONTROLS];

	// Integrate a control
	float UpdateLinearControl(ControlInfo &info, float input, float deltaT);
	float UpdateAngularControl(ControlInfo &info, float input, float deltaT);
	float ButtonPairToAxis(DefaultControlMapper::DIGITALBUTTON down, DefaultControlMapper::DIGITALBUTTON up);

	// Our quantities
	Vector3 			m_eye;
	float				m_yaw, m_pitch;
	DirCos3x3		m_matrix;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
class CollidableControlCamera : public Camera
{
public:
	// Camedras must have at least a name ancmd w2v controller
	CollidableControlCamera(const char *name,
						DefaultControlMapper &controlMapper,
			  			ProjectionController *projectionController = NULL,
						ViewportController *viewportController = NULL);

	// Set the parameters
	void SetTarget(CActor *follow)
		{ m_w2v.SetTarget(follow); }
	void ClearTarget()
		{ m_w2v.ClearTarget(); }
		
	// Get it!
	void Activate();

	void SetEye( Vector3CRef eye ) { m_w2v.SetEye( eye ); }

protected:
	// Our controller
	DefaultControlMapper	&m_mapper;

	// Our params
	CollidableControlCameraParams	*m_params;

	// The W2V controller
	CollidableControlW2VC				m_w2v;
};

#endif
