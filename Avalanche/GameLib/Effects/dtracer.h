///////////////////////////////////////////////////////////////////////////////
// distortion tracers
///////////////////////////////////////////////////////////////////////////////
#ifndef DTRACER_H
#define DTRACER_H

#include "Effects/tracer.h"

// A tracer model and collection of instances
class DistortionTracer : public Tracer
{
public:
	~DistortionTracer();

private:
	// Prep textures
	void LockTextures();
	void UnlockTextures();

	// How many verts are in a strip?
	int m_stripVerts;
	bool CreateGeometry(int instances);

	// Create an instance
	void Create(int instanceIndex, const Vector3 &start, const Vector3 &end, const Vector3 &dir);

	// Update an instance for rendering
	bool Update(const Camera &camera, int instanceIndex, float now);

	// some utility stuff
	inline Vector2 UVFromWorld(const Camera &camera, const Vector3 &world);
	inline Vector2 UVClamp(const Vector2 &uv);
	static float s_uScale, s_vScale;

	// Do it!
	DistortionTracer(TracerManager &manager, TracerModel &model);
	friend class TracerManager;
};

#endif
