///////////////////////////////////////////////////////////////////////////////
// A simple death camera
///////////////////////////////////////////////////////////////////////////////
#ifndef DEATHCAM_H
#define DEATHCAM_H

#include "camera/util/flextarg.h"
#include "camera/camera.h"

struct DeathCamParams
{
	float 		focalPointYTweak;
	float			hitTweak;
	float			minD;
	float 		distance;
	float 		height;
	float 		rotationSpeed;
	char			focalBone[32];
	float			resetSlowMo;
};

class DeathWTVC : public WorldToViewController
{
public:
	DeathWTVC(DeathCamParams *&params);
	~DeathWTVC() {}

	// Set the target actor
	void SetTarget(CActor *actor);

  	// Update
	void Update(Camera &parent);

	// Eye position
	Vector3	Eye()
		{ return m_eye; }
	void		SetEye( const Vector3 &v ) { m_eye = v; }

	Vector3 Up()
		{ return m_up; }

	Vector3 FocalPoint()
		{ return m_focal; }

		void	SetUseStaticDeathCam() { m_useStaticDeathCam = true; }
		void	ClearUseStaticDeathCam() { m_useStaticDeathCam = false; }
		bool	UseStaticDeathCam() { return m_useStaticDeathCam; }

private:
	// Who are we following?
	CActor			*m_target;

	// What are the settings?
	DeathCamParams	*&m_params;

	// Our innsads
	Vector3 			m_eye;
	Vector3			m_up;
	Vector3			m_focal;

	float				m_angle;

	bool				m_useStaticDeathCam;

	// collision util
	bool Intersect(Vector3CRef start, Vector3CRef end,	float &d, Vector3 &intersection);
	Vector3 GetBoneInWorld(CActor &forWho, const char *boneName);
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
class DeathCamera : public Camera
{
public:
	// Cameras must have at least a name and w2v controller
	DeathCamera(const char *name,
					ProjectionController *projectionController = NULL,
					ViewportController *viewportController = NULL);

	// Set the parameters
	void SetTarget(CActor *target)
		{ m_death.SetTarget(target); m_target = target; }

	virtual void SetPosition( const Vector3& position ) { m_death.SetEye( (Vector3&)position ); }

	void Activate();
	void Deactivate();
	void Update();

protected:
	// Wee!
	DeathWTVC		m_death;

	// The parameters for this camera
	DeathCamParams	*m_params;

	// Clear mode
	u32				m_clear;

	// Our target
	CActor			*m_target;

	// Reset the timer?
	float				m_resetSlowMo;
};

#endif
