///////////////////////////////////////////////////////////////////////////////
// Encapsulate all the information about a scene
///////////////////////////////////////////////////////////////////////////////
#include "camera/CameraPCH.h"

///////////////////////////////////////////////////////////////////////////////
CameraScene::CameraScene(
Texture *realTimeTexture) :
m_groupList(true)
{
	// Clear it
	m_matrix.Identity();

	// Create the scene
	int sceneType = realTimeTexture ? SCENE_RENDER_INTO_TEXTURE : SCENE_DISPLAY;
	m_scene = SceneMgr::RegisterScene(&m_matrix,	sceneType, realTimeTexture);

	// Create a default group
	CameraGroup *defaultGroup = new CameraGroup("Default group");
	ASSERT(defaultGroup);
	AddGroup(defaultGroup);

	DefaultCamera *defaultCamera = new DefaultCamera;
	ASSERT(defaultCamera);

	// Select the default camera
	defaultGroup->AddHead(defaultCamera);
	defaultGroup->Select(defaultCamera);

	// Push the default camera
	PushGroup(defaultGroup);

	// Create the default controllers
	m_defaultProjection = new DefaultProjectionController;
	ASSERT(m_defaultProjection);

	m_defaultViewport = new DefaultViewportController;
	ASSERT(m_defaultViewport);

	// by default the master is defaulted
	m_masterProjection = m_defaultProjection;
	m_masterViewport = m_defaultViewport;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CameraScene::~CameraScene()
{
	// Kill the scene
	SceneMgr::KillScene(m_scene);

	// Clean up the default controllers
	delete m_defaultProjection;
	delete m_defaultViewport;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CameraScene::AddGroup(
CameraGroup *group)
{
	ASSERT(group);
	if (!group)
		return;

	// Bind them to us
	group->BindScene(*this);

	// Add them to our list
	m_groupList.AddTail(group);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CameraScene::RemoveGroup(
CameraGroup *group)
{
	m_groupList.Remove(group);
}

///////////////////////////////////////////////////////////////////////////////
// Find a group
///////////////////////////////////////////////////////////////////////////////
CameraGroup *CameraScene::FindGroup(
const char *groupName)
{
	CameraGroup *group = m_groupList.Head();
	while (group)
	{
		if (strcmpi(groupName, group->Name()) == 0)
			return group;

		group = group->next;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Select a camera for a group
///////////////////////////////////////////////////////////////////////////////
bool CameraScene::SelectCamera(
CameraGroup *group,
Camera *camera)
{
	if (!group)
		return false;
	if (!camera)
		return  false;

	group->Select(camera);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CameraScene::SelectCamera(
const char *groupName,
const char *cameraName)
{
	CameraGroup *group = FindGroup(groupName);
	if (!group)
		return false;

	return group->Select(cameraName);
}

///////////////////////////////////////////////////////////////////////////////
// Set the camera for a scene
///////////////////////////////////////////////////////////////////////////////
void CameraScene::PushGroup(
CameraGroup *cameraGroup)
{
	ASSERT(cameraGroup);
	if (!cameraGroup)
		return;

	// Prepare the camera that is losing focus
	Camera *lastCamera = NULL;

	// Deactivate the old camera group
	if (!m_stack.Empty())
		lastCamera = m_stack.Peek()->Deactivate();

	// Set the camera as active for this scene
	m_stack.Push(cameraGroup);

	// Make sure the camera knows about the scene
	cameraGroup->Activate(lastCamera);
}

///////////////////////////////////////////////////////////////////////////////
// Pop the cameragroup from the scene
///////////////////////////////////////////////////////////////////////////////
CameraGroup *CameraScene::PopGroup(
CameraGroup *group)
{
	// Anything to do
	if (m_stack.Empty())
		return NULL;

	// Are we on top
	if (m_stack.Peek() == group)
	{
		// Can't pop off default group
		ASSERT(m_stack.Items() > 1);

		// Get the group off, but keep the camera activation going
		CameraGroup *top = m_stack.Pop();
		Camera *lastCamera = top->Deactivate();
		m_stack.Peek()->Activate(lastCamera);
	}
	else // we are in the stack
	{
		// Find us
		for (int i = m_stack.Items() - 2; i > 0; i++)
		{
			// Kill us
			if (m_stack.Peek(i) == group)
			{	
				m_stack.Remove(i);
				break;	
			}
		}
	}

	return group;
}

///////////////////////////////////////////////////////////////////////////////
// What is the current camera group for the scene
///////////////////////////////////////////////////////////////////////////////
CameraGroup &CameraScene::CurrentGroup()
{
	// This should never happen (there is always the default cam)
	ASSERT(!m_stack.Empty());

	// Give them back the camera from the top
	return *m_stack.Peek();
}

///////////////////////////////////////////////////////////////////////////////
// What is the current camera for the scene
///////////////////////////////////////////////////////////////////////////////
Camera &CameraScene::CurrentCamera()
{
	return CurrentGroup().Selected();
}

///////////////////////////////////////////////////////////////////////////////
// Do we have a non-default camera installed?
///////////////////////////////////////////////////////////////////////////////
bool CameraScene::HasRealCamera()
{
	return (m_stack.Items() > 1);
}

///////////////////////////////////////////////////////////////////////////////
// Get the current controllers
///////////////////////////////////////////////////////////////////////////////
ProjectionController &CameraScene::GetCurrentProjection()
{
	if (!m_stack.Empty() && m_stack.Peek()->Selected().GetProjection())
		return *m_stack.Peek()->Selected().GetProjection();
	else
		return MasterProjection();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ViewportController &CameraScene::GetCurrentViewport()
{
	if (!m_stack.Empty() && m_stack.Peek()->Selected().GetViewport())
		return *m_stack.Peek()->Selected().GetViewport();
	else
		return MasterViewport();
}

///////////////////////////////////////////////////////////////////////////////
// Setup the Master projection and viewport controllers (set to NULL to go
// back to the default.
///////////////////////////////////////////////////////////////////////////////
void CameraScene::SetMasterProjection(
ProjectionController *controller)
{
	// Set it
	if (controller)
		m_masterProjection = controller;
	else
		m_masterProjection = m_defaultProjection;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CameraScene::SetMasterViewport(
ViewportController *controller)
{
	// Set it
	if (controller)
		m_masterViewport = controller;
	else
		m_masterViewport = m_defaultViewport;
}

///////////////////////////////////////////////////////////////////////////////
// Update once per frame
///////////////////////////////////////////////////////////////////////////////
void CameraScene::Update()
{
#ifndef UPDATEALLGROUPS
	// Get the current camera to update
	CurrentGroup().Selected().Update();
#else
	// Who updates here, everyone or just the top?
	CameraGroup *group = m_groupList.Head();
	while (group)
	{
		// Get the camera for this scene
		Camera &camera = group->Selected();

		// Do the work
		camera.Update();

		// next
		group = group->next;
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
// Post render update
///////////////////////////////////////////////////////////////////////////////
void CameraScene::PostRenderUpdate()
{
	// Get the current camera to update
	CurrentGroup().Selected().PostRenderUpdate();
}


