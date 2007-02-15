//////////////////////////////////////////////////////////////////////////
// A simple default viewport controller
//////////////////////////////////////////////////////////////////////////
#ifndef DEFVIEWPORT_H
#define DEFVIEWPORT_H

#include "viewport.h"

class DefaultViewportController : public ViewportController
{
public:
	DefaultViewportController();
	virtual ~DefaultViewportController() {}

	// The upper left hand corner
	float XOffset();
	float YOffset();

	// The lower right hand corner
	float Width();
	float Height();

	// Setup the viewport
	void Setup(float x, float y, float width, float height);

protected:
	float			m_x, m_y;
	float			m_width, m_height;
};

#endif
