///////////////////////////////////////////////////////////////////////////////
// Camera system rewrite, by jeff
//
// The camera now is implemented with three distinct componenents to control
// the three "traditional computer graphics" quanties: worldToView, projection,
// and viewport.
///////////////////////////////////////////////////////////////////////////////
#ifndef CAMERA_H
#define CAMERA_H

#include "container/dllist.h"
#include "container/pool.h"
#include "camera/projection.h"

// fowards
class WorldToViewController;
class ViewportController;
class CameraScene;
class CameraGroup;
class AABB;

////////////////////////////////////////////////////////////////
// The camera takes at least a name and a worldToView controller
// If other controllers are not installed, the camerasystem
// controllers are used.
//
// No memory management is done on the controllers, if you
// new them, you delete them. The camera will not delete them.
//
// So, in other words, do not install a viewport/projection
// controller unless you want very specific control of what is
// going on.
////////////////////////////////////////////////////////////////
class Camera
{
public:
	// Cameras must have at least a name and w2v controller
	Camera(const char *name,
				WorldToViewController *w2vController = NULL,
				ProjectionController *projectionController = NULL,
				ViewportController *viewportController = NULL);
	virtual ~Camera();

	// Get at the name
	const char *Name()
		{ return m_name; }

	// Set the group
	CameraGroup *GetGroup()
		{ return m_group;	}
	void SetGroup(CameraGroup *group)
		{ m_group = group;}

	// Filtering has a threshold for where a "hard" jump will
	// occur. This number (in meters)
	void SetNoFilterDistance(float noFilter)
		{ m_noFilterDistanceSquared = noFilter * noFilter; }

	// Control filtering (0 for time constant removes filter)
	void SetEyePointFilter(float timeConstant)
		{ m_eyeTimeConstant = timeConstant;}
	float GetEyePointFilter()
		{ return m_eyeTimeConstant;}

	// The focal point filter
	void SetFocalPointFilter(float timeConstant)
		{ m_focalTimeConstant = timeConstant; }
	float GetFocalPointFilter()
		{ return m_focalTimeConstant; }

	// Setup some shake stuff (duration 0.0f == forever)
	void SetEyePointShake(float amplitude, float frequency, float duration, bool useNoise = true, bool noFilter = true);
	void ClearEyePointShake();
	bool EyePointShaking()
		{ return !ShakeDone(m_shakeEye); }

	void SetFocalPointShake(float amplitude, float frequency, float duration, bool useNoise = true, bool noFilter = true);
	void ClearFocalPointShake();
	bool FocalPointShaking()
		{ return !ShakeDone(m_shakeEye); }

	// The do-it-yourself shaker (return false if there is no change)
	typedef bool (*ShakeCallback)(const Vector3 &eye, const Vector3 &focal, float deltaT, Vector3 &newEye, Vector3 &newFocal, float &tc);

	// Install a shake callback, returns the existing one (which you should also call...)
	void InstallShakeCallback(ShakeCallback callback, void *context = NULL);
	bool UninstallShakeCallback(ShakeCallback callback, void *context = NULL);
	
	// Update the camera
	virtual void Update();

	// Render right after the main render
	virtual void PostRenderUpdate();

	////////////////////////////////////////////////////////////
	// Access view quantities in world coordinates (these are
	// valid after a call to Update. All vectors are unit length
	// and orthonormal
	////////////////////////////////////////////////////////////
	Vector3CRef Eye() const
		{ return m_eye; }
	Vector3CRef FocalPoint() const
		{ return m_focalPoint; }
	Vector3CRef Dir() const
		{ return m_dir; }
	Vector3CRef Up() const
		{ return m_up; }
	Vector3CRef Right() const
		{ return m_right; }

	///////////////////////////////////
	// Get the projection parameters
	///////////////////////////////////
	float Hither()
		{ return GetProjectionController().Hither(); }
	float Yon() 
		{ return GetProjectionController().Yon(); }
	float FOV() 
		{ return GetProjectionController().FOV(); }
	float AspectRatio()
		{ return GetProjectionController().AspectRatio(); }

	////////////////////////////////////
	// Access the world to view matrix
	////////////////////////////////////
	const Cartesian4x4 &WorldToView() const
		{ return m_worldToView; }

	////////////////////////////////////
	// Access the view to world matrix
	////////////////////////////////////
	const Cartesian4x4 &ViewToWorld() const
		{ return m_viewToWorld; }

	///////////////////////////////////////////////
	// Put a point in world space into camera space
	///////////////////////////////////////////////
	const Vector4 ToView(Vector3CRef p) const
		{ return p.Transform3(m_worldToView); }

	///////////////////////////////////////////////
	// Put a point in view space into world space
	///////////////////////////////////////////////
	const Vector4 FromView(Vector3CRef p) const
		{ return p.Transform3(m_viewToWorld); }

	// Get the world to screen matrix
	const Matrix4x4 &WorldToScreen()
		{ return m_worldToScreen; }

	// Transform from world to screen using the fustrum
	// Returns true if the the point is on-screen, false if clipped
	bool ToScreen(Vector3CRef world, Vector2 &screen, float &w) const;

	// Transform using a clip factor (making the cliping region larger/smaller than the screen)
	// Returns true if the the point is on-screen, false if clipped
	bool ToScreen(Vector3CRef world, Vector2 &screen, float &w, const Vector2 &clipFactor) const;

	// Transform from world to screen, no clipping
	Vector2 ToScreen(Vector3CRef world) const;

	// Return the state of this object within the current camera view
	enum FRUSTUMSTATE
	{
		OUTSIDE,		// Object is completely outside of frustum
		INSIDE,		// Object is completely inside of frustum
		BORDER      // Object is intersected by frustum plane
	};

	// Simple object tests against the fustrum
	FRUSTUMSTATE Test(Vector3CRef point);
	FRUSTUMSTATE Test(Vector3CRef center, float radius);
	FRUSTUMSTATE Test(const AABB &aabb);

	// Get at the controllers
	ProjectionController *GetProjection()
		{ return m_projectionController; }
	ViewportController *GetViewport()
		{ return	m_viewportController; }
	
protected:
	// Who owns us?
	CameraScene					*m_scene;
	char							m_name[32];
	CameraGroup					*m_group;

	// Our controllers go here
	WorldToViewController	*m_worldToViewController;
	ProjectionController		*m_projectionController;
	ViewportController		*m_viewportController;

	// Access the controllers
	ProjectionController &GetProjectionController();
	ViewportController &GetViewportController();

	// The matrices we have created
	Cartesian4x4				m_worldToView;
	Cartesian4x4				m_viewToWorld;
	Matrix4x4					m_projection;
	Matrix4x4					m_worldToScreen; // m_worldToView * m_projection

	// These are derivable, but let's keep them here for clarity
	Vector3						m_eye;			// The eye position, in world coordinates
	Vector3						m_focalPoint;	// The focal point
	Vector3						m_dir;			// The direction vector
	Vector3						m_up;				// The VALID up vector
	Vector3						m_right;	  		// The right vector

	// Stuff for filtering
	float							m_eyeTimeConstant;
	float							m_focalTimeConstant;
	float							m_noFilterDistanceSquared;
	
	// Stuff for shaking
	struct Shake
	{
		bool		active;
		float		amplitude;
		float		frequency;
		Vector3	seed;
		float		doneTime;
		bool		noise;
		bool		disableFilter;
	};
	Shake							m_shakeEye, m_shakeFocal;
	void SetupShake(Shake &shake, float amplitude, float frequency, float duration, bool useNoise, bool noFilter);
	void ClearShake(Shake &shake);
	bool ShakeDone(Shake &shake);
	Vector3 ComputeShake(Shake &shake);
	bool NoFilter(Shake &shake);

	// Shake callback stuff
	struct Callback
	{
		ShakeCallback	callback;
		void				*context;
		
		Callback			*prev, *next;
	};
	DoublyLinkedList<Callback>	m_shakeCallbacks;	
	Pool<Callback>					m_shakeCallbackPool;
	// The do-it-yourself shaker (return false if there is no change)
	void EvaluateShakeCallbacks(const Vector3 &eye, const Vector3 &focal, float deltaT, Vector3 &newEye, Vector3 &newFocal);

	// Do the actual work
	void BuildCamera();
	void BuildProjection();
	void BuildViewport();

	// Apply things to the engine
	void ApplyCamera();
	void ApplyProjection();
	void ApplyViewport();

	// Utiltities for the matrix building
	bool Paralell(Vector3CRef a, Vector3CRef b)
		{ return Math::Zero(Vector3::Cross(a, b).LengthSquared()); }

public:
	// This is to ensure the camera has the scene
	void BindScene(CameraScene &scene);

	// These are called when we gain/lose current camera status
	virtual void Activate() {}
	virtual void Deactivate() {}

	// Used to inherit
	Camera &operator=(const Camera &c);

	// We probably appear in a list for a camera group
	Camera		*prev, *next;
};

#endif
