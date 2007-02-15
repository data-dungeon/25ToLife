///////////////////////////////////////////////////////////////////////////////
// The cammera director maintains a scene of cameras for each scene. Each
// scene also has default projection and viewport controllers.
//
// No memory management is done on cameras in this class, you must do your own
// memory management on camera objects.
///////////////////////////////////////////////////////////////////////////////
#ifndef DIRECTOR_H
#define DIRECTOR_H

// If I put the comment "includes" here would you kill me?
#include "container/pool.h"
#include "container/dllist.h"
#include "container/sstack.h"

#include "camera/camscene.h"
#include "camera/world2view.h"
#include "camera/projection.h"
#include "camera/viewport.h"

// Go heavy on the forwards
class Camera;

class CameraDirector
{
public:
	~CameraDirector();

	// Intialize everything
	static void Initialize();

	// Shut it down
	static void Terminate();

	// Create a scene (look at the scene manager for details)
	CameraScene *CreateScene(Texture *realtimeTexture = NULL);
	void DestroyScene(CameraScene *scene = NULL);

	// Set the default scene
	void SetDefaultScene(CameraScene *scene);
	CameraScene *GetDefaultScene();

	// Access the camera matrix
	Cartesian4x4 &GetCameraMatrix(CameraScene *scene = NULL);

	// Set the camera for a scene
	void PushGroup(CameraGroup *cameraGroup, CameraScene *scene = NULL);
	void PushGroup(const char *groupName, CameraScene *scene = NULL);

	// Pop the camera for the scene
	CameraGroup *PopGroup(CameraGroup *cameraGroup, CameraScene *scene = NULL);
	CameraGroup *PopGroup(const char *groupName, CameraScene *scene = NULL);

	// What is the current camera for the scene
	Camera &CurrentCamera(CameraScene *scene = NULL);

	// Is there a non-default camera installed?
	bool HasRealCamera(CameraScene *scene = NULL);

	// Get the current projection/viewport controllers
	ProjectionController &GetCurrentProjection(CameraScene *scene = NULL);
	ViewportController &GetCurrentViewport(CameraScene *scene = NULL);

	// Setup the Master projection and viewport controllers
	void SetMasterProjection(ProjectionController *controller, CameraScene *scene = NULL);
	ProjectionController &GetMasterProjection(CameraScene *scene = NULL);
	void SetMasterViewport(ViewportController *controller, CameraScene *scene = NULL);
	ViewportController &GetMasterViewport(CameraScene *scene = NULL);

	// Per frame update
	void Update();

	// Render right after the main render
	void PostRenderUpdate();

	// Camera group support (any group you give to the director
	// will be deleted on destruction)
	void AddGroup(CameraGroup *group, CameraScene *scene = NULL);
	void RemoveGroup(CameraGroup *group, CameraScene *scene = NULL);

	// Find a group
	CameraGroup *FindGroup(const char *groupName, CameraScene *scene = NULL);

	// This is the majik call
	bool SelectCamera(const char *groupName, const char *cameraName, CameraScene *scene = NULL);

private:
	// Intialize creates us
	CameraDirector();

	// The scene description stuff
	Pool<CameraScene>				  	m_scenePool;
	DoublyLinkedList<CameraScene>	m_sceneList;

	// The default scene
	CameraScene						  *m_defaultScene;

	// Handy utilties
	CameraScene *UseScene(CameraScene *scene);
};

///////////////////////////////////////////////////////////////////////////////
// camera system singleton
///////////////////////////////////////////////////////////////////////////////
extern CameraDirector *g_cameraDirectorPtr;
inline CameraDirector &SafePtr(CameraDirector *ptr)
{
	ASSERT(ptr);
	return(*ptr);
}
#define g_cameraDirector SafePtr(g_cameraDirectorPtr)

#endif
