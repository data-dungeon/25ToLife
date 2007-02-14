///////////////////////////////////////////////////////////////////////////////
// The 3rd person shooter cam
///////////////////////////////////////////////////////////////////////////////
#ifndef GHOSTCAM_H
#define GHOSTRCAM_H

#include "camera/camera.h"
#include "camera/world2view.h"
#include "camera/defproj.h"
#include "avatar/avcontrolmap.h"
#include "InterfacesTTL/ParticipantIF.h"

class CActor;
class GhostCamera;

// The parameters for this camera
struct GhostParams
{
	float					yHeight;
	float					followDistance;

	float					minFollowDistance;
	float					distanceTweak;
	float					followFilter;
	float 				minHither;

	float					normalFOV;

	float					hither;
	float					yon;
};

////////////////////////////////////////////////////////////////
// Our world to view controller
////////////////////////////////////////////////////////////////
class GhostWorldToView : public WorldToViewController
{
public:
	GhostWorldToView( AvatarControlMapper &mapper , GhostParams *&params );
	~GhostWorldToView() {}

	// Initialize (m_params are loaded)
	void Initialize();

	// Follow this actor
	void SetFollow(CActor *follow);
	CActor *GetFollow();

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
	void AimStartDir(Vector3 &start, Vector3 &dir);

	// Lock first person mode
	void FirstPersonLock(bool lock)
		{ m_firstPersonLock = lock; }
	bool FirstPersonLock()
		{ return m_firstPersonLock; }

	// Activate/deactivate
	void Activate();
	void Deactivate();

	void		setParticipantID( int ID ) { m_participantID = ID; }


private:
	void TargetNext(bool i_forceChange);
	void TargetPrev();

	// Who are we following?
	ActorHandle			m_ghost;

	// input
	AvatarControlMapper			&m_input;

	// What are the settings?
	GhostParams 		*&m_params;

	// What are our current camera values
	Vector3 				m_eye;
	Vector3 				m_focalPoint;
	Vector3 				m_up;
	float					m_followDistance;
	bool					m_firstPersonLock;

	uint8					m_participantID;
	bool					m_active;

	// Intersect with the world
	bool Intersect(Vector3CRef start, Vector3CRef end, float &d, bool terrainOnly);
};

////////////////////////////////////////////////////////////////
// Shooter projection controller
////////////////////////////////////////////////////////////////
class GhostProjection : public DefaultProjectionController
{
public:
	GhostProjection(GhostParams *&params);
	~GhostProjection() {}

  	// Update
	void Update(Camera &parent);

private:
	// What are the settings?
	GhostParams 		*&m_params;

	// Best hither and yon


	// Fustrum check
	float FrustumCollide(const Cartesian4x4 &viewToWorld, const Vector3 &eye, const Vector3 &dir, float fov, float hither);

	// Intersect with the world
	bool Intersect(Vector3CRef start, Vector3CRef end, Vector3 &intersection, bool terrainOnly);
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
class GhostCamera : public Camera
{
public:
	// Cameras must have at least a name ancmd w2v controller
	GhostCamera( const char *name , AvatarControlMapper &controlMapper );
	~GhostCamera();

	// Who do we follow
	void SetFollow(CActor *follow)
		{ m_ghostW2V.SetFollow(follow); }
	CActor *GetFollow()
		{ return m_ghostW2V.GetFollow(); }

	// Aiming state
	void SetFOVFactor(float FOVFactor, float filter);
	void RestoreFOV(float filter);

	// Get at the aiming parameters
	void AimStartDir(Vector3 &start, Vector3 &dir)
		{ m_ghostW2V.AimStartDir(start, dir); }

	void FirstPersonLock(bool lock)
		{ m_ghostW2V.FirstPersonLock(lock); }
	bool FirstPersonLock()
		{ return m_ghostW2V.FirstPersonLock(); }

	// Activate/deactivate
	void Activate();
	void Deactivate();
	bool Active()
		{ return m_active; }

	void HideShooter( bool hide );

	GhostProjection *getGhostProjection( void ) { return &m_ghostProj; }

	void setParticipantID( int ID ) { m_ghostW2V.setParticipantID( ID ); }

protected:
	// The controllers
	GhostWorldToView 			m_ghostW2V;
	GhostProjection 			m_ghostProj;

	// The parameters for this camera
	GhostParams 					*m_params;

	// Are we the active camera?
	bool								m_active;

	bool								m_inHead;
	bool								m_isWeaponHidden;
};

// We are going to offer a singleton for this
extern GhostCamera *g_ghostCamPtr;
inline GhostCamera &SafePtr(GhostCamera *ptr)
{
	ASSERT(ptr);
	return(*ptr);
}
#define g_ghostCam	SafePtr(g_ghostCamPtr)

#endif
