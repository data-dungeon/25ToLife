/////////////////////////////////////////////////////////////////////////////
// Laser sight implementation
/////////////////////////////////////////////////////////////////////////////
#ifndef LSIGHT_H
#define LSIGHT_H

#include "weapons/zoomsight.h"
#include "effects/lasersight.h"

class LaserSight;

class LaserSightCreator : public AimSightCreator
{
public:
	// Do the work
	LaserSightCreator();

	// Create
	AimSight *CreateInstance(const VarObjProperties<AimSight> &properties);

protected:
	AimSightProperties *NewProperties(int index);
};

// The properties
class LaserSightProperties : public ZoomSightProperties
{
public:
	LaserSightProperties(AimSightCreator &creator, int index);
	~LaserSightProperties();

	// For parsing the data
	void RegisterContext(const char *context);

	// the per-loop stuff
	void InitializeEvery();
	void TerminateEvery();

	// Max deviation (not in var file)
	float				m_maxAngle;
	float				m_filterAngle;
	float				m_maxChange;

	// sight info
	Vector4			m_laserColorStart, m_laserColorEnd;
	Vector4			m_laserColorMiss;
	Vector4			m_laserColorDot;
	float				m_laserRayRadius;
	float				m_laserDotRadius;
	float				m_laserRange;
	float				m_laserAimInflate;
	LaserRender		*m_laser;

  	// Utility...
  	static inline void SetColor(const Vector4 &v, ts_bRGBA &color)
  	{
  		color.R = (u8)Math::Clamp(v.X(), 0.0f, 255.0f);
  		color.G = (u8)Math::Clamp(v.Y(), 0.0f, 255.0f);
  		color.B = (u8)Math::Clamp(v.Z(), 0.0f, 255.0f);
  		color.A = (u8)Math::Clamp(v.W(), 0.0f, 255.0f);
  	}
};

class LaserSight : public ZoomSight
{
public:
	LaserSight(const LaserSightProperties &properties);
	~LaserSight();

	void Activate(Weapon &weapon);
	void Deactivate(Weapon &weapon);
	void Update(Weapon &weapon);

private:
	// Our specific props
	const LaserSightProperties &m_lsProps;

	// Our laser sight!
	LaserRender::Handle	m_laserHandle;

	// Our current direction
	Vector3					m_laserDir;
	bool						m_laserDirValid;
	void SetLaserDir(const Vector3 &dir)
		{ m_laserDir = dir; m_laserDirValid = true; }
	void ClearLaserDir()
		{ m_laserDirValid = false; }
	Vector3					m_lastPos;
};

#endif
