/////////////////////////////////////////////////////////////////////////////
// Simple (but true 3d) objects
/////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"
#include "EngineHelper/timer.h"

// We are a singleton
WidgetFactory *g_widgetFactoryPtr = NULL;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
WidgetFactory::WidgetFactory() :
m_modelList(false)
{
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
WidgetFactory::~WidgetFactory()
{
	Terminate();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void WidgetFactory::CreateSingleton()
{
	if( g_widgetFactoryPtr )
		return;

	g_widgetFactoryPtr = new WidgetFactory;
	ASSERT(g_widgetFactoryPtr);

	g_widgetFactoryPtr->Initialize();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void WidgetFactory::DestroySingleton()
{
	if (!g_widgetFactoryPtr)
		return;
	delete g_widgetFactoryPtr;
	g_widgetFactoryPtr = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Initialize the system
/////////////////////////////////////////////////////////////////////////////
bool WidgetFactory::Initialize()
{
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Shut down everything
/////////////////////////////////////////////////////////////////////////////
void WidgetFactory::Terminate()
{
	// Delete all the models
	WidgetModel *model = m_modelList.Head();
	while (model)
	{
		m_modelList.Unlink(model);
		DestroyPool(model);
		memFree(model);
		model = m_modelList.Head();
	}
}

/////////////////////////////////////////////////////////////////////////////
// Call this every frame for updates
/////////////////////////////////////////////////////////////////////////////
void WidgetFactory::Update(
const Cartesian4x4 &camera)
{
	// We have two potential cameras
	Vector3 cameraPos;
	Vector3 cameraDir;

	// What is the camera position in world coordinates?
	Vector3 temp;
	temp = camera.GetTranslation();
	cameraPos = temp.InvBasisTransform(camera);
	cameraPos.Negate();

	//	What is the camera dir?
	cameraDir = camera.GetColumn2();
	cameraDir.Negate();

	// Update all the widgets
	float deltaT = g_timer.GetFrameSec();
	float	now = g_timer.GetEndSec();
	WidgetModel *model = m_modelList.Head();
	while (model)
	{
		// run the physics
		if (model->widget)
			RunPhysics(*model, now, deltaT);

		// next one!
		model = model->next;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Add a widget model into the system.
// Widget models should be in the XZ plane (y = 0)
// Failure is inidcated by a NULL return
/////////////////////////////////////////////////////////////////////////////
WidgetFactory::Handle WidgetFactory::LoadModel(
LoadSOM &loadSOM,
const char *pathFile,
const char *modelName,
int flags,
PhysicsModel physicsModel,
float ttl,
float fadeTime,
float heightTweak)
{
	// Are they dumb?
	ASSERT(modelName);
	ASSERT(pathFile);

	// Does it already exits?
	Handle modelHandle = GetModelHandle(modelName);
	if (modelHandle)
		return modelHandle;

	// First make sure we can even load this model
	ts_Geometry *geometry = loadSOM.Geometry(pathFile);
	if (!geometry)
		return NULL;

	// Create a structure for this model's data
	MEM_SET_ALLOC_NAME("Widget Model");
	WidgetModel *model = (WidgetModel *)memAlloc(sizeof(WidgetModel));
	if (!model)
		return NULL;
	memset(model, '\0', sizeof(WidgetModel));

	// Fill out the structure
	strncpy(model->name, modelName, sizeof(model->name) - 1);
	model->geometry = *geometry;

	// must have som!
	ASSERT(model->geometry.pSOM != NULL);

	// get the flags
	model->alphaSort = (flags & ALPHA_SORT) != 0;
	model->dynamicLight = (flags & DYNAMIC_LIGHT) != 0;

	// Aren't we clever?
	model->physicsModel = physicsModel;

	// Setup the props
	model->props.Defaults();

	// Add it to the list
	m_modelList.AddTail(model);

	// Fading parameters
	model->fadeTime = fadeTime;
	model->ttl = ttl;

	// height tweak
	model->heightTweak = heightTweak;

	// success!
	return model;
}

/////////////////////////////////////////////////////////////////////////////
// Retrieve a model handle
/////////////////////////////////////////////////////////////////////////////
WidgetFactory::Handle WidgetFactory::GetModelHandle(
const char *name)
{
	WidgetModel *model = m_modelList.Head();
	while (model)
	{
		// yes, no, maybe?
		if (strcmpi(model->name, name) == 0)
			return model;

		model = model->next;
	}

	// Could not find it
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Setup the the rigid body props for a model
/////////////////////////////////////////////////////////////////////////////
bool WidgetFactory::SetupProperties(
Handle forModel,
const RigidBodyProps &props)
{
	// Doh
	WidgetModel *model = (WidgetModel *)forModel;
	if (!model)
		return false;

	// Setup the properties
	model->props = props;
	model->props.Update();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WidgetFactory::HasPool(
Handle forModel)
{
	// Doh
	WidgetModel *model = (WidgetModel *)forModel;
	if (!model)
		return false;

	return (model->widget != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// Add a widget pool to the world
/////////////////////////////////////////////////////////////////////////////
bool WidgetFactory::CreatePool(
Handle forModel,
int maxItems)
{
	// Doh
	WidgetModel *model = (WidgetModel *)forModel;
	if (!model)
		return false;

	// Is it already here?
	if (model->widget)
		return false;

	// Allocate the array
	MEM_SET_ALLOC_NAME("Widgetpool");
	model->widget = (Widget *)memAlloc(sizeof(Widget) * maxItems);
	ASSERT(model->widget);
	if (!model->widget)
		return false;
	model->widgets = maxItems;

	// now initialize each one
	for (int i = 0; i < model->widgets; i++)
	{
		// Build up the instance stuff
		model->widget[i].instance.Reset();
		model->widget[i].instance.InitFromGeometry( model->geometry );
		model->widget[i].instance.SetName( "food-widget" );
		model->widget[i].instance.Matrix().Identity();

		// Default flags
		model->widget[i].instance.SetReceiveShadows( false );

		// lighting
		model->widget[i].instance.SetDynamicallyLit( model->dynamicLight );

		// alpha sort
		model->widget[i].instance.SetAlphaSort( model->alphaSort );

		// Make sure we call the constructor on the widget
		new ((void *)&model->widget[i].body) RigidBody;

		// Now setup the widget
		model->widget[i].body.Initialize(model->props, model->widget[i].instance.Matrix());

		// Duh
		model->widget[i].instance.UpdateBoundingVolumes();
	}

	// Add them all to the scene later
	ShowWidgets(model->widget, model->widgets, false);
	model->inScene = false;

	// The oldest one is the first one
	model->oldest = 0;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WidgetFactory::DestroyPool(
Handle forModel)
{
	// Doh
	WidgetModel *model = (WidgetModel *)forModel;
	if (!model)
		return false;

	// Is it already here?
	if (!model->widget)
		return false;

	// Remove them all from the scene
	RemoveFromScene(model->widget, model->widgets);

	for (int i = 0; i < model->widgets; i++)
		model->widget[i].instance.Destroy();

	// Get rid of everything
	memFree(model->widget);
	model->widget = NULL;
	model->widgets = 0;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Create a widget witht he parameters.
/////////////////////////////////////////////////////////////////////////////
bool WidgetFactory::CreateWidget(
Handle forModel,
Vector3CRef startPos,
const DirCos3x3 &o,
Vector3 *velocity,
Vector3 *angularImpulse)
{
	// Doh
	WidgetModel *model = (WidgetModel *)forModel;
	if (!model || !model->widget)
		return false;

	// Add them to the scene?
	if (!model->inScene)
	{
		AddToScene(model->widget, model->widgets);
		ShowWidgets(model->widget, model->widgets, false);
		model->inScene = true;
	}

	// First allocate the oldest
	Widget &widget = model->widget[model->oldest++];

	// Advance the pointer
	if (model->oldest == model->widgets)
		model->oldest = 0;

	// Setup the state
	widget.body.Place(startPos, o, velocity);

	// Make it spin
	if (angularImpulse)
		widget.body.ApplyAngularImpulse(*angularImpulse);

	// We are born!
	widget.born = g_timer.GetEndSec();

	// Draw us
	ResetAlpha(widget);
	ShowWidget(widget, true);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void WidgetFactory::ShowWidgets(
Widget *widgetList,
int widgets,
bool show)
{
	for (int i = 0; i < widgets; i++)
		ShowWidget(widgetList[i], show);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void WidgetFactory::AddToScene(
Widget *widgetList,
int widgets)
{
	for (int i = 0; i < widgets; i++)
		AddToScene(widgetList[i]);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void WidgetFactory::RemoveFromScene(
Widget *widgetList,
int widgets)
{
	for (int i = 0; i < widgets; i++)
		RemoveFromScene(widgetList[i]);
}

/////////////////////////////////////////////////////////////////////////////
// Run the physics for a pool!
/////////////////////////////////////////////////////////////////////////////
void WidgetFactory::RunPhysics(
WidgetModel &model,
float now,
float deltaT)
{
	// Update the physics for each widget
	int i;
	switch (model.physicsModel)
	{
		case SIMPLE:
			for (i = 0; i < model.widgets; i++)
			{
				if (!WidgetHidden(model.widget[i]))
					RunPhysicsSimple(model, model.widget[i], now, deltaT);
			}
			break;

		case STICK:
			for (i = 0; i < model.widgets; i++)
			{
				if (!WidgetHidden(model.widget[i]))
					RunPhysicsStick(model, model.widget[i], now, deltaT);
			}
			break;

		case BOUNCEANDFADE:
			for (i = 0; i < model.widgets; i++)
			{
				if (!WidgetHidden(model.widget[i]))
					RunPhysicsBounce(model, model.widget[i], now, deltaT);
			}
			break;

		case FULL:
			for (i = 0; i < model.widgets; i++)
			{
				if (!WidgetHidden(model.widget[i]))
					RunPhysicsFull(model, model.widget[i], now, deltaT);
			}
			break;

		default:
			// unknown model
			ASSERT(false);
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Compute the fade (return's false if dead)
/////////////////////////////////////////////////////////////////////////////
inline bool WidgetFactory::ComputeFade(
WidgetModel &model,
Widget &widget,
float now)
{
	// do we live for ever?
	if (Math::Zero(model.ttl))
		return true;

	// Time to die?
	float deathTime = widget.born + model.ttl + model.fadeTime;
	if (now > deathTime)
	{
		ShowWidget(widget, false);
		return false;
	}

	// time to fade?
	float fadeTime = widget.born + model.ttl;
	if (now > fadeTime)
	{
		float fade = 1.0f - Math::Clamp((now - fadeTime) / model.fadeTime);
		SetAlpha(widget, fade);
	}

	// we live
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Run the physics on a single widget!
/////////////////////////////////////////////////////////////////////////////
void WidgetFactory::RunPhysicsSimple(
WidgetModel &model,
Widget &widget,
float now,
float t)
{
	// Figure out our fade (returns false if dead)
	if (!ComputeFade(model, widget, now))
		return;

	// Alias the widget for chrissakes
	RigidBody &body = widget.body;
	body.ClearExternalForces();

	// Run a time step
	body.Run(t);

	// Update the bounding volumes
	widget.instance.UpdateBoundingVolumes();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void WidgetFactory::RunPhysicsBounce(
WidgetModel &model,
Widget &widget,
float now,
float t)
{
	// Figure out our fade (returns false if dead)
	if (!ComputeFade(model, widget, now))
		return;

	ASSERT(false);

	// Update the bounding volumes
	widget.instance.UpdateBoundingVolumes();
}

/////////////////////////////////////////////////////////////////////////////
// Run the physics on a single widget!
/////////////////////////////////////////////////////////////////////////////
void WidgetFactory::RunPhysicsStick(
WidgetModel &model,
Widget &widget,
float now,
float t)
{
	// Figure out our fade (returns false if dead)
	if (!ComputeFade(model, widget, now))
		return;

	// Alias the widget for chrissakes
	RigidBody &body = widget.body;
	body.ClearExternalForces();

	if (!body.AtRest())
	{
		// Do cheesy assed collision
		CLineEntity entity;
		entity.SetPositiveInclude(WORLD_CATEGORY);
		entity.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
		entity.Line().Origin() = body.Position();

		// Run a time step
		body.Run(t);

		// Collision?
		entity.Line().Basis() = body.Position() - entity.Line().Origin();
		g_collisionSystem.ResetStack();
		Vector3 intersection, normal;
		if ((g_collisionSystem.Test(&entity) != NULL) && CLineQuery::Contact(entity.Line(), intersection, normal))
		{
			// Alias our matrix
			Graphics4x4 &m = widget.instance.Matrix();

			// Build the matrix around the intersection
			Vector3 dir;
			dir = m.GetRow2();
			AlignToNormal(normal, dir, m);

			// Finally, poke in the translation
			intersection.y(intersection.y() + model.heightTweak);
			m.PokeTranslation(intersection);
			body.Reset(true);
		}
	}

	// Update the bounding volumes
	widget.instance.UpdateBoundingVolumes();
}

/////////////////////////////////////////////////////////////////////////////
// Run the physics on a single widget!
/////////////////////////////////////////////////////////////////////////////
void WidgetFactory::RunPhysicsFull(
WidgetModel &model,
Widget &widget,
float now,
float t)
{
	// Figure out our fade (returns false if dead)
	if (!ComputeFade(model, widget, now))
		return;

	// Alias the widget for chrissakes
	RigidBody &body = widget.body;
	body.ClearExternalForces();
	body.RunCollide(t);

	// Update the bounding volumes
	widget.instance.UpdateBoundingVolumes();
}

/////////////////////////////////////////////////////////////////////////////
// Align a matrix around the passed up, using dir as a reference.
// Assumes both are unit
/////////////////////////////////////////////////////////////////////////////
void WidgetFactory::AlignToNormal(
Vector3CRef up,
Vector3CRef dir,
Graphics4x4 &m)
{
	// compute the right
	Vector3 right = Vector3::Cross(up, dir);
	right.Normalize();

	// Re-fix the dir
	Vector3 fixDir = Vector3::Cross(right, up);

	// Set the matrix
	m.SetRow0(right);
	m.SetRow1(up);
	m.SetRow2(fixDir);
}
