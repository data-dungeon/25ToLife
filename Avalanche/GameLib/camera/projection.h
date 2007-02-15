///////////////////////////////////////////////////////////////////////////////
// The projection controller gives information on how the projection
// matrix should be constructed.
///////////////////////////////////////////////////////////////////////////////
#ifndef PROJECTION_H
#define PROJECTION_H

class Camera;

//////////////////////////////////////////////////////////////////////////
// The projection controller controls the projection matrix parameters.
// These parameters include:
// 	- hither and yon
// 	- FOV
// 	- Aspect ratio
//////////////////////////////////////////////////////////////////////////
class ProjectionController
{
public:
	ProjectionController() {}
	virtual ~ProjectionController() {}

	// Update 
	virtual void Update(Camera &parent) {}

	// Control hither and yon
	virtual float Hither() = 0;
	virtual float Yon() = 0;

	// Return an FOV (0...180) degrees (non-inclusive)
	virtual float FOV() = 0;

	// Return the aspect ratio
	virtual float AspectRatio() = 0;
};

#endif
