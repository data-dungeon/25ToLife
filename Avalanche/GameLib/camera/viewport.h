//////////////////////////////////////////////////////////////////////////
// The viuewport controller controls the final 2D output of the
// render. Screen coordinates are in floating point with the
// upper left hand of the device being 0,0 and lower right
// hand corner being 1,1
//////////////////////////////////////////////////////////////////////////
#ifndef VIEWPORT_H
#define VIEWPORT_H

class Camera;

class ViewportController
{
public:
	ViewportController() {}
	virtual ~ViewportController() {}

	// Update 
	virtual void Update(Camera &parent) {}

	// Set the upper left hand corner
	virtual float XOffset() = 0;
	virtual float YOffset() = 0;

	// Set the lower right hand corner
	virtual float Width() = 0;
	virtual float Height() = 0;

	// Post render
	virtual void PostRender() {}
		
protected:
	// Get at the device information
	int DeviceWidth();
	int DeviceHeight();
};

#endif
