///////////////////////////////////////////////////////////////////////////////
// A simpe load screen that works on all platforms
///////////////////////////////////////////////////////////////////////////////
#ifndef LSSPIN_H
#define LSSPIN_H

#include "enginehelper/loadscreen.h"

class LoadScreenSpin : public LoadScreenModule
{
public:
	// Setup the loadscreen dbl
	static bool SetModel(const char *dbl);

	// All roations are in degrees
	static void SetupCamera(float cameraD, const Vector3 &cameraRotation);
	static void SetupSpinRate(float spinRate);

	// The creator (pass this into the LoadScreen setup call
	static LoadScreenModule *Create();

	// o way to handle errors here
	LoadScreenSpin();
	virtual ~LoadScreenSpin();

	// We write these
	void Startup();
	void Render();

private:
	CInstance		m_instance;
	float				m_rotate;

	// The model we use
	static t_Handle 	s_dblHandle;
	static float	   s_cameraD;
	static Vector3		s_cameraRotation;
	static float		s_spinRate;
	static ts_bRGBA	s_clearColor;
};

#endif
