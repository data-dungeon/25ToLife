///////////////////////////////////////////////////////////////////////////+//
// Instance support without an actor!
/////////////////////////////////////////////////////////////////////////////
#ifndef SIMPLEINST_H
#define SIMPLEINST_H

#include "platform/BaseType.h"
#include "Math/Matrix.h"
#include "GameHelper/loadsom.h"
#include "container/dllist.h"

class SimpleInstance
{
public:
	SimpleInstance();
	~SimpleInstance();

	enum Flags
	{
		ALPHA_SORT = 0x01,
		DYNAMIC_LIGHT = 0x02,
		NO_ZWRITE = 0x04,
		ADDITIVE_BLEND = 0x08
	};

	// Load the model
	bool Create(const char *pathFile, int flags);

	// Destroy this guy
	void Destroy();

	// Show/hide this instance
	void Show(bool show);
	bool Hidden();

	// Place the instnace
	void Place(const Graphics4x4 &place);

	// Where is it?
	const Graphics4x4 &Matrix()
		{ return m_instance.Matrix(); }

	// Manipulate alpha
	float GetConstantAlpha()
		{ return m_instance.Alpha(); }
	void SetConstantAlpha(float newAlpha)
		{ return m_instance.SetAlpha( newAlpha ); }

	// Render bucket time!
	void SetRenderBucket(int renderBucket)
		{ m_instance.SetRenderBucket(renderBucket); }
	
	// Make this stuff available
	CInstance &Instance()
		{ return m_instance; }

private:
	// our data
	CInstance			m_instance;

	// Add and remove to the scene
	bool AddToScene();
	void RemoveFromScene();
};

#endif
