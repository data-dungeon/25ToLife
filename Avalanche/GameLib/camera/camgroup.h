///////////////////////////////////////////////////////////////////////////////
// The camera group allows a bunch of cameras to be linked together in a
// somewhat meaningful way.
///////////////////////////////////////////////////////////////////////////////
#ifndef CAMGROUP_H
#define CAMGROUP_H

#include "container/dllist.h"
#include "camera/camera.h"

class CameraGroup
{
public:
	// all cameras deleted on destrution
	CameraGroup(const char *name, bool loop = true);
	virtual ~CameraGroup() {}

	// Return the name
	const char *Name()
		{ return m_name; }

	// Add a camera to the group
	void AddHead(Camera *camera);
	void AddTail(Camera *camera);

	// Remove a camera from the group
	void Remove(Camera *camera);

	// Instead of a linear list, loop the list
	void Loop(bool loop)
		{ m_loop = loop; }
	bool Loop()
		{ return m_loop; }

	// Get the first/last camera
	Camera *First();
	Camera *Last();

	// Select a the previous/next camera
	Camera *Next(Camera *camera);
	Camera *Previous(Camera *camera);

	// Find a camera
	Camera *Find(const char *name);

	// Select a camera for this group
	bool Select(Camera *camera);
	bool Select(const char *cameraName);
	Camera &Selected();

	// How many cameras are there
	int Cameras();

	// Lock the selection
	bool Lock(bool lock);
	bool Locked();

	// Called once per frame so that the
	// group can do whatever is necessary
	void Update();

protected:
	char								m_name[32];
	CameraScene						*m_scene;

	DoublyLinkedList<Camera>	m_list;
	bool								m_loop;

	Camera							*m_selected;
	int								m_lock;	

public:
	// This is to ensure the camera has the scene
	void BindScene(CameraScene &scene);

	// These are called when we gain/lose focus on the stack
	virtual void Activate(Camera *lastCamera);
	virtual Camera *Deactivate();

	// We appear in a list in the camera director
	CameraGroup						*prev, *next;
};

#endif
