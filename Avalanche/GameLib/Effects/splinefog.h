/////////////////////////////////////////////////////////////////////////////
// Fog based on position along a spline.
/////////////////////////////////////////////////////////////////////////////
#ifndef SPLINEFOG_H
#define SPLINEFOG_H

#include "platform/BaseType.h"
#include "Display/Common/RenderState.h"

class SplinePath;

class SplineFog
{
	friend class SplineFogGen;

public:
	SplineFog(void);
	~SplineFog(void);

	// Initialize the system
	bool Initialize(SplinePath* path, const char* world_name);

	// Shut down everything
	void Terminate(void);

	// Called every frame to update the position at which the fog is being 
	// computed.
	void Update(const Vector3& position);

	// Get the fog start value in the range [0,1].
	float GetStart(void);

	// Get the fog end value in the range [0,1].
	float GetEnd(void);

	// Get the flare value
	float GetFlare(void)
		{ return(m_Flare); }

private:

	struct Fog
	{
		float start;
		float end;

		float flare;
		
		float red;
		float green;
		float blue;
		
		float intensity;
	};

private:

	SplinePath* m_Path;
	Fog*			m_Fogs;

	float			m_Start;
	float			m_End;
	
	float			m_Red;
	float			m_Green;
	float			m_Blue;

	float			m_Flare;
	float			m_Intensity;
};

// There can be only one!
extern SplineFog *g_splineFogPtr;
inline SplineFog &SafePtr(SplineFog *ptr)
{
	ASSERT(ptr);
	return(*ptr);
}
#define g_splineFog	SafePtr(g_splineFogPtr)

#endif	// SPLINEFOG_H
