////////////////////////////////////////////////////////////////////////////
//
// Camera
//
// Class for cameras
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: camera.h $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 5/14/03    Time: 4:15p
 * Updated in $/Avalanche/tools/Tupper3d
 * new camera info based on Maya instead of Max
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 1/16/02    Time: 3:44p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added constructors that use the parent as a parameter
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/28/01   Time: 4:35p
 * Updated in $/Avalanche/tools/Tupper3d
 * camera flags
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/20/01   Time: 11:10a
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/19/01   Time: 2:10p
 * Updated in $/Avalanche/tools/Tupper3d
 * Lights & Cameras
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_CAMERA_H
#define TUPPER3D_CAMERA_H

class TupObject;

class TupCamera
{
	// Private Default constructor so no one can use it
	TupCamera(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		FOCAL_LENGTH,
		CAMERA_SCALE,
		NEAR_CLIP_PLANE,
		FAR_CLIP_PLANE,
		HORIZONAL_FILM_APERTURE,
		VERTICAL_FILM_APERTURE,
		LENS_SQUEEZE_RATIO,
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum CameraFlags
	{
		FLAGS_ORTHOGRAPHIC											= 1<<0,
	};

	TupCamera(TupperwareAggregate *pObject);
	TupCamera(TupObject &objectBase);

	void BuildAtomCache(void);

// add methods

	TupperwareScalar *AddFocalLength(float value);
	TupperwareScalar *AddCameraScale(float value);
	TupperwareScalar *AddNearClipPlane(float value);
	TupperwareScalar *AddFarClipPlane(float value);
	TupperwareScalar *AddHorizontalFilmAperture(float value);
	TupperwareScalar *AddVerticalFilmAperture(float value);
	TupperwareScalar *AddLensSqueezeRatio(float value);
	TupperwareScalar *AddFlags(int value);

// get methods

	TupperwareScalar *GetFocalLength(float &value);
	TupperwareScalar *GetCameraScale(float &value);
	TupperwareScalar *GetNearClipPlane(float &value);
	TupperwareScalar *GetFarClipPlane(float &value);
	TupperwareScalar *GetHorizontalFilmAperture(float &value);
	TupperwareScalar *GetVerticalFilmAperture(float &value);
	TupperwareScalar *GetLensSqueezeRatio(float &value);
	TupperwareScalar *GetFlags(int &value);

private:
// add methods
	TupperwareScalar *AddFloatScalar(float value,TupCamera::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupCamera::ChildrenAtoms index);

// get methods
	TupperwareScalar *GetFloatScalar(float &value,TupCamera::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupCamera::ChildrenAtoms index);
};

#endif
