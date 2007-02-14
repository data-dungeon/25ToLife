///////////////////////////////////////////////////////////////////////////////
// The 3rd person shooter cam
///////////////////////////////////////////////////////////////////////////////
#ifndef SHOOTERCAM_H
#define SHOOTERCAM_H

#include "camera/camera.h"
#include "camera/world2view.h"
#include "camera/defproj.h"

class CActor;
class ShooterCamera;

// The parameters for this camera
struct ShooterParams
{
	float					yHeight;
	float					followDistance;

	float					minFollowDistance;
	float					distanceTweak;
	float					followFilter;
	float					firstPersonFilter;
	float 				minHither;

	float					normalFOV;

	float					hither;
	float					yon;
};

////////////////////////////////////////////////////////////////
// Our world to view controller
////////////////////////////////////////////////////////////////
class ShooterWorldToView : public WorldToViewController
{
public:
	ShooterWorldToView(ShooterParams *&params);
	~ShooterWorldToView() {}

	// Initialize (m_params are loaded)
	void Initialize();

	// Follow this actor
	void SetFollow(CActor *shooter)
		{ m_shooter = CActor::ToHandle(shooter); }
	CActor *GetFollow()
		{ return CActor::FromHandle(m_shooter); }

	// Override normal functionality and lock an eye & target
	void LockToTarget(const Vector3 &target);
	void UnlockTarget();
	bool TargetLocked()
		{ return m_targetLocked; }

  	// Update
	void Update(Camera &parent);

  // Eye position
	Vector3 Eye()
		{ return m_eye; }

	// Focal point
	Vector3 FocalPoint()
		{ return m_focalPoint; }

	// Up vector (give the actor's up)
	Vector3 Up()
		{ return m_up; }

	// Useful for targeting
	void AimOriginDir(Camera &parent, Vector3 &origin, Vector3 &dir);

	// Lock first person mode
	void FirstPersonLock(bool lock)
		{ m_firstPersonLock = lock; }
	bool FirstPersonLock()
		{ return m_firstPersonLock; }

	// Activate/deactivate
	void Activate();
	void Deactivate();

private:
	// Who are we following?
	ActorHandle			m_shooter;

	// What are the settings?
	ShooterParams 		*&m_params;

	// What are our current camera values
	Vector3 				m_eye;
	Vector3 				m_focalPoint;
	Vector3 				m_up;
	float					m_followDistance;

	bool					m_firstPersonLock;

	// Target locking
	Vector3				m_targetLock;
	bool					m_targetLocked;

	// Do the real update (if necessary)
	uint32 				m_lastUpdate;
	bool InternalUpdate(Camera &parent);

	// Intersect with the world
	bool Intersect(Vector3CRef start, Vector3CRef end, float &d, bool terrainOnly);
};

////////////////////////////////////////////////////////////////
// Shooter projection controller
////////////////////////////////////////////////////////////////
class ShooterProjection : public DefaultProjectionController
{
public:
	ShooterProjection(ShooterParams *&params);
	~ShooterProjection() {}

  	// Update
	void Update(Camera &parent);

private:
	// What are the settings?
	ShooterParams 		*&m_params;

	// Best hither and yon


	// Fustrum check
	float FrustumCollide(const Cartesian4x4 &viewToWorld, const Vector3 &eye, const Vector3 &dir, float fov, float hither);

	// Intersect with the world
	bool Intersect(Vector3CRef start, Vector3CRef end, Vector3 &intersection, bool terrainOnly);
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
class ShooterCamera : public Camera
{
public:
	// Cameras must have at least a name ancmd w2v controller
	ShooterCamera(const char *name);
	~ShooterCamera();

	// Who do we follow
	void SetFollow(CActor *shooter)
		{ m_shooterW2V.SetFollow(shooter); }
	CActor *GetFollow()
		{ return m_shooterW2V.GetFollow(); }

	// Aiming state
	void SetFOVFactor(float FOVFactor, float filter);
	void RestoreFOV(float filter);

	// Get at the aiming parameters
	void AimOriginDir(Vector3 &origin, Vector3 &dir)
		{ m_shooterW2V.AimOriginDir(*this, origin, dir); }

	void FirstPersonLock(bool lock)
		{ m_shooterW2V.FirstPersonLock(lock); }
	bool FirstPersonLock()
		{ return m_shooterW2V.FirstPersonLock(); }

	// Override normal functionality and lock an eye & target
	void LockToTarget(const Vector3 &target);
	void UnlockTarget();
	bool LockedToTarget();

	// Override camera update
	void Update();

	// Activate/deactivate
	void Activate();
	void Deactivate();
	bool Active()
		{ return m_active; }

	// maya control of shooter camera
	bool isInCameraControl() { return inCameraControl; }
	char *getControlData() { return controlData; }

	ShooterProjection *getShooterProjection( void ) { return &m_shooterProj; }

	void HideShooter( bool hide );

	REF_SEND_MESSAGE_HANDLER( EnterCameraControl , ShooterCamera ) d_enterCameraControlMessageHandler;
	REF_SEND_MESSAGE_HANDLER( ExitCameraControl , ShooterCamera ) d_exitCameraControlMessageHandler;
//	REF_SEND_MESSAGE_HANDLER( StartCinematic , ShooterCamera ) d_startCinematicMessageHandler;
//	REF_SEND_MESSAGE_HANDLER( StopCinematic , ShooterCamera ) d_stopCinematicMessageHandler;
	REF_BASIC_MESSAGE_HANDLER( StartCinematic , MESSAGE_VALUE , ShooterCamera ) d_startCinematicMessageHandler;
	REF_BASIC_MESSAGE_HANDLER( StopCinematic , MESSAGE_VALUE , ShooterCamera ) d_stopCinematicMessageHandler;
	//REF_BASIC_MESSAGE_HANDLER( ShowKill, MESSAGE_VALUE, AvatarHud ) d_killsHandler;

protected:
	// The controllers
	ShooterWorldToView 			m_shooterW2V;
	ShooterProjection 			m_shooterProj;

	// The parameters for this camera
	ShooterParams 					*m_params;

	// Are we the active camera?
	bool								m_active;

	char								*controlData;
	bool								inCameraControl;
	CMessageOwner					d_messageOwner;

	bool								m_hideShooter;

	float								m_targetLockSaveEyeFilter;
	float								m_targetLockSaveFocalFilter;

	bool								m_inHead;
	bool								m_isWeaponHidden;
};

// We are going to offer a singleton for this
extern ShooterCamera *g_shooterCamPtr;
inline ShooterCamera &SafePtr(ShooterCamera *ptr)
{
	ASSERT(ptr);
	return(*ptr);
}
#define g_shooterCam	SafePtr(g_shooterCamPtr)

#endif
