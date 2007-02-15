///////////////////////////////////////////////////////////////////////////+//
/////////////////////////////////////////////////////////////////////////////
#ifndef WIDGETWORLD_H
#define WIDGETWORLD_H

#include "platform/BaseType.h"
#include "Math/Matrix.h"
#include "GameHelper/loadsom.h"
#include "container/dllist.h"
#include "Effects/simplerb.h"

class WidgetFactory
{
public:
	WidgetFactory();
	~WidgetFactory();

	// Manage the singleton
	static void CreateSingleton();
	static void DestroySingleton();

	// Initialize the system
	bool Initialize();

	// Shut down everything
	void Terminate();

	// Call this every frame for updates
	void Update(const Cartesian4x4 &camera);

	// Get a model handle
	typedef void *Handle;

	enum Flags
	{
		ALPHA_SORT = 0x01,
		DYNAMIC_LIGHT = 0x02
	};

	enum PhysicsModel
	{
		SIMPLE = 0, 		// No collisions or anything
		STICK, 				// stick to first collision
		BOUNCEANDFADE,		// Bounce off first collision, then fade
		FULL        		// Full model
	};

	// Add a into the system.
	// Failure is inidcated by a false return
	Handle LoadModel(LoadSOM &loadSOM, const char *pathFile,	const char *modelName,
							int flags = 0,
							PhysicsModel physicsModel = SIMPLE,
							float ttl = 0.0f, float fadeTime = 0.0f,
							float heightTweak = 0.05f);

	// Retrieve a model handle
	Handle GetModelHandle(const char *name);

	// Setup the the rigid body props for a model
	bool SetupProperties(Handle forModel, const RigidBodyProps &props);

	// Create a pool for a model type
	bool HasPool(Handle forModel);
	bool CreatePool(Handle forModel, int maxItems);
	bool DestroyPool(Handle forModel);

	// Create a widget witht he parameters. (null quantities are left alone)
	bool CreateWidget(Handle forModel,
							Vector3CRef startPos, const DirCos3x3 &o,
							Vector3 *velocity = NULL, Vector3 *angularImpulse = NULL);

private:
	// A structure to merge all the widget dataparts
	struct Widget
	{
		CInstance			instance;
		RigidBody			body;
		float					born;
		char					pad[16 - ((sizeof(CInstance) + sizeof(float) + sizeof(RigidBody)) & 0xf)];
	};

	// The total information about a widget
	struct WidgetModel
	{
		// What is the name of this widget
		char					name[64];

		// The geometry pointer for this widget
		ts_Geometry			geometry;

		// Our flags
		bool					alphaSort;
		bool					dynamicLight;
		bool					vertexLight;

		// our physics model
		PhysicsModel		physicsModel;

		// How much do we tweak this bad boy up?
		float					heightTweak;

		// What is our TTL?
		float					ttl;
		float					fadeTime;

		// Our properties for this type of widget
		RigidBodyProps		props;

		// The pool of widgets
		bool					inScene; // God I hate the SCENESTUPIDMANGLER
		Widget				*widget;
		int					widgets;
		int					oldest;

		// For the doubly linked list
		WidgetModel			*prev, *next;
	};

	// Our basic info per model
	DoublyLinkedList<WidgetModel>	m_modelList;

	// Hide/show widgets
	inline void ShowWidget(Widget &widget, bool show);
	inline bool WidgetHidden(Widget &widget);
	void ShowWidgets(Widget *widgetList, int widgets, bool show);

	// Add and remove widgets from the scene
	inline void AddToScene(Widget &widget);
	inline void RemoveFromScene(Widget &widget);
	void AddToScene(Widget *widgetList, int widgets);
	void RemoveFromScene(Widget *widgetList, int widgets);

	// Run the physics
	void RunPhysics(WidgetModel &model, float now, float deltaT);

	// Compute the fade (return's false if dead)
	inline void SetAlpha(Widget &widget, float alpha);
	inline void ResetAlpha(Widget &widget);
	inline bool ComputeFade(WidgetModel &model, Widget &widget, float now);

	// The models
	void RunPhysicsSimple(WidgetModel &model, Widget &widget, float now, float t);
	void RunPhysicsStick(WidgetModel &model, Widget &widget, float now, float t);
	void RunPhysicsBounce(WidgetModel &model, Widget &widget, float now, float t);
	void RunPhysicsFull(WidgetModel &model, Widget &widget, float now, float t);

	// Align a matrix around the passed up, using dir as a reference.  Assumes both are unit
	static void AlignToNormal(Vector3CRef up, Vector3CRef dir, Graphics4x4 &m);
};

// Include the inlines
#include "Effects/widget.hpp"

// There can be only one!
extern WidgetFactory *g_widgetFactoryPtr;
inline WidgetFactory &SafePtr(WidgetFactory *ptr)
{
	ASSERT(ptr);
	return(*ptr);
}
#define g_widgetFactory SafePtr(g_widgetFactoryPtr)

#endif
