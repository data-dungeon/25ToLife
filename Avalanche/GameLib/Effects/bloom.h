/////////////////////////////////////////////////////////////////////////////
// Blooms & glare
/////////////////////////////////////////////////////////////////////////////
#ifndef BLOOM_H
#define BLOOM_H

#include "EngineHelper/Singleton.h"
#include "camera/director.h"
#include "decal/quaddecal.h"
#include "container/dllist.h"

class CheapBlooms
{
public:
	CheapBlooms();
	~CheapBlooms();

	// Load the effects
	bool LoadEffects(const char *textureDBL);

	// load blooms from the world DBL
	bool DoWorldDBL(t_Handle worldHandle);

	// We talk about blooms/glares using a handle
	typedef void *Handle;

	// Add a bloom!
	Handle Add(int effectIndex,
					const Vector3 &center,
					float width, float height,
					const Vector3 &normal, const Vector3 &right, const Vector3 &up,
				 	const ts_bRGBA &color);

	// Hide the bloom
	void Hide(Handle handle, bool hide);
	bool Hidden(Handle handle);

	// Remove the halo once it is done
	bool Remove(Handle handle);

	// Draw
	void Update(CameraScene *scene = NULL, const Vector3 *sun = NULL);

private:
	// Our texture
	DecalSet					m_decalSet;

	// Stuff for the static halos in the world
	class Bloom
	{
	public:
		int					m_effect;

		Vector3 				m_center;

		float					m_halfWidth;
		float					m_halfHeight;

		Vector3 				m_normal;
		Vector3 				m_right;
		Vector3 				m_up;

	 	ts_bRGBA 			m_color;

		float					m_baseAlpha;

		bool					m_hidden;

		// Per frame
		float					m_cameraD;
		int operator<(const Bloom &b)
			{ return m_cameraD < b.m_cameraD ; }
		int operator>(const Bloom &b)
			{ return m_cameraD > b.m_cameraD ; }

		float					m_targetFactor;
		float					m_factor;

		bool					m_visible;

		Bloom					*prev, *next;
	};
	Pool<Bloom>				m_pool;

	// An effect is the combination of the bloom and the emitter
	class Effect
	{
	public:
		Effect(Pool<Bloom> &pool, DecalSet &decalSet, const DecalTextureLayer *layer, int layers);
		~Effect();

		// Initialize
		void Initialize(const char *varName);

		// Add
		void Add(Bloom *bloom)
			{ m_list.AddTail(bloom); }
		void Remove(Bloom *bloom)
			{ m_list.Unlink(bloom); m_pool.Delete(bloom); }

		// Update!
		void Update(Camera &camera, const Vector3 &sun, float deltaT);

	private:
		Pool<Bloom> 				&m_pool;

		int 						  	m_maxInstances;
		float						  	m_heightTweak;
		float						  	m_timeConstant;
		float							m_dotStart;
		float							m_dotEnd;
		float							m_alphaStart;
		float							m_alphaEnd;
		float							m_scaleMin;
		float							m_scaleMax;
		bool							m_useReflect;
		float							m_power;

		SimpleQuadDecalEmitter 	m_emitter;
		DoublyLinkedList<Bloom>	m_list;

		float ComputeFactor(Camera &camera, const Vector3 &sun, const Bloom &bloom);
	};
	DynList<Effect>				m_effect;
};

// There can be only one!
DefineSingleton(CheapBlooms)
#define g_bloom 	GetSingleton(CheapBlooms)

#endif
