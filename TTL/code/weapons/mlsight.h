/////////////////////////////////////////////////////////////////////////////
// Maglite sight implementation
/////////////////////////////////////////////////////////////////////////////
#ifndef MLSIGHT_H
#define MLSIGHT_H

#include "weapons/zoomsight.h"
#include "effects/lasersight.h"

class MagliteSight;

class MagliteSightCreator : public AimSightCreator
{
public:
	// Do the work
	MagliteSightCreator();

	// Create
	AimSight *CreateInstance(const VarObjProperties<AimSight> &properties);

protected:
	AimSightProperties *NewProperties(int index);
};

// The properties
class MagliteSightProperties : public ZoomSightProperties
{
public:
	MagliteSightProperties(AimSightCreator &creator, int index);
	~MagliteSightProperties();

	// For parsing the data
	void RegisterContext(const char *context);

	// Setup our stuff
	void PostLoadSetup();

	// sight info
	Vector4			m_magliteColorStart, m_magliteColorEnd;
	float				m_magliteRadiusStart, m_magliteRadiusEnd;
	float				m_magliteRange, m_magliteLightD;
	float				m_magliteIntensity, m_magliteAttenuation;
	LaserRender		*m_maglite;

  	// Utility...
  	static inline void SetColor(const Vector4 &v, ts_bRGBA &color)
  	{
  		color.R = (u8)Math::Clamp(v.X(), 0.0f, 255.0f);
  		color.G = (u8)Math::Clamp(v.Y(), 0.0f, 255.0f);
  		color.B = (u8)Math::Clamp(v.Z(), 0.0f, 255.0f);
  		color.A = (u8)Math::Clamp(v.W(), 0.0f, 255.0f);
  	}
};

class MagliteSight : public ZoomSight
{
public:
	MagliteSight(const MagliteSightProperties &properties);
	~MagliteSight();

	void Deactivate(Weapon &weapon);
	void Update(Weapon &weapon);

	// Are we on?
	bool DynamicLightOn()
		{ return (m_light && m_light->isLightEnabled()); }

private:
	// Our specific props
	const MagliteSightProperties &m_mlProps;

	// Our laser sight!
	LaserRender::Handle	m_magliteHandle;
	CLightRecord			*m_light;
};

#endif
