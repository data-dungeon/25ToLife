///////////////////////////////////////////////////////////////////////////////
// The camera scene encapsulates the scene of cameras for the scene.
///////////////////////////////////////////////////////////////////////////////
#ifndef CAMERASTACK_H
#define CAMERASTACK_H

// If I put the comment "includes" here would you kill me?
#include "container/sstack.h"
#include "camera/camgroup.h"
#include "camera/defcam.h"

// Go heavy on the forwards
class ProjectionController;
class ViewportController;
class DefaultProjectionController;
class DefaultViewportController;
class Camera;
class Texture;
class CScene;

class CameraScene
{
public:
	CameraScene(Texture *realtimeTexture = NULL);
	~CameraScene();

	// Get at the information
	inline CScene* Scene() const
		{ return m_scene; }
	inline Cartesian4x4 &CameraMatrix()
		{ return m_matrix; }
	inline void UpdateCameraMatrix(Cartesian4x4 &m)
		{ m_matrix = m; }

	// You must work in groups
	void AddGroup(CameraGroup *group);
	void RemoveGroup(CameraGroup *group);
	CameraGroup *FindGroup(const char *groupName);

	// Select a camera for a group
	bool SelectCamera(CameraGroup *group, Camera *camera);
	bool SelectCamera(const char *groupName, const char *cameraName);

	// Set the camera for a scene
	void PushGroup(CameraGroup *camera);
	void PushGroup(const char *groupName)
		{ PushGroup(FindGroup(groupName)); }

	// Pop the camera for the scene
	CameraGroup *PopGroup(CameraGroup *camera);
	CameraGroup *PopGroup(const char *groupName)
		{ return PopGroup(FindGroup(groupName)); }

	// What is the current group for this scene
	CameraGroup &CurrentGroup();

	// What is the current camera for the scene
	Camera &CurrentCamera();

	// Do we have a non-default camera installed?
	bool HasRealCamera();

	// Get the current controllers
	ProjectionController &GetCurrentProjection();
	ViewportController &GetCurrentViewport();

	// Get the master projection and viewport
	inline ProjectionController &MasterProjection()
		{ return *m_masterProjection; }
	inline ViewportController &MasterViewport()
		{ return *m_masterViewport; }

	// Set the master controllers
	void SetMasterProjection(ProjectionController *controller);
	void SetMasterViewport(ViewportController *controller);

	// Update once per frame
	void Update();
	void PostRenderUpdate();

private:
	CScene*								m_scene;					// The scene manager handle
	Cartesian4x4 						m_matrix;				// The camera matrix

	// The group management stuff
	DoublyLinkedList<CameraGroup>	m_groupList;
	DynamicStack<CameraGroup *> 	m_stack;					// The stack of groups

	// The controllers for this scene
	ProjectionController 			*m_masterProjection;
	ViewportController 	  			*m_masterViewport;

	// Default controllers
	DefaultProjectionController 	*m_defaultProjection;
	DefaultViewportController 		*m_defaultViewport;

public:
	// We appear in a list
	CameraScene							*prev, *next;
};

#endif
