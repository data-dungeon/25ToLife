////////////////////////////////////////////////////////////////////////////
//
// Camera
//
// Class for cameras
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: camera.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 5/14/03    Time: 4:15p
 * Updated in $/Avalanche/tools/Tupper3d
 * new camera info based on Maya instead of Max
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 1/16/02    Time: 3:44p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added constructors that use the parent as a parameter
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

#include "Tupper3dPch.h"
#include "camera.h"
#include "object.h"

char *TupCamera::m_keyNames[TupCamera::NUM_CHILDREN_ATOMS] = 
{
	"FocalLength", // FOCAL_LENGTH
	"CameraScale", // CAMERA_SCALE
	"NearClipPlane", // NEAR_CLIP_PLANE
	"FarClipPlane", // FAR_CLIP_PLANE
	"HorizontalFilmAperture", // HORIZONAL_FILM_APERTURE
	"VerticalFilmAperture", // VERTICAL_FILM_APERTURE
	"LensSqueezeRatio", // LENS_SQUEEZE_RATIO
	"Flags", // FLAGS
};

TupCamera::TupCamera(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupCamera::TupCamera(TupObject &objectBase)
{
	m_pObject = objectBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupCamera::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

// add methods

TupperwareScalar *TupCamera::AddFocalLength(float value)
{
	return AddFloatScalar(value,TupCamera::FOCAL_LENGTH);
}

TupperwareScalar *TupCamera::AddCameraScale(float value)
{
	return AddFloatScalar(value,TupCamera::CAMERA_SCALE);
}

TupperwareScalar *TupCamera::AddNearClipPlane(float value)
{
	return AddFloatScalar(value,TupCamera::NEAR_CLIP_PLANE);
}

TupperwareScalar *TupCamera::AddFarClipPlane(float value)
{
	return AddFloatScalar(value,TupCamera::FAR_CLIP_PLANE);
}

TupperwareScalar *TupCamera::AddHorizontalFilmAperture(float value)
{
	return AddFloatScalar(value,TupCamera::HORIZONAL_FILM_APERTURE);
}

TupperwareScalar *TupCamera::AddVerticalFilmAperture(float value)
{
	return AddFloatScalar(value,TupCamera::VERTICAL_FILM_APERTURE);
}

TupperwareScalar *TupCamera::AddLensSqueezeRatio(float value)
{
	return AddFloatScalar(value,TupCamera::LENS_SQUEEZE_RATIO);
}

TupperwareScalar *TupCamera::AddFlags(int value)
{
	return AddIntScalar(value,TupCamera::FLAGS);
}

// get methods

TupperwareScalar *TupCamera::GetFocalLength(float &value)
{
	return GetFloatScalar(value,TupCamera::FOCAL_LENGTH);
}

TupperwareScalar *TupCamera::GetCameraScale(float &value)
{
	return GetFloatScalar(value,TupCamera::CAMERA_SCALE);
}

TupperwareScalar *TupCamera::GetNearClipPlane(float &value)
{
	return GetFloatScalar(value,TupCamera::NEAR_CLIP_PLANE);
}

TupperwareScalar *TupCamera::GetFarClipPlane(float &value)
{
	return GetFloatScalar(value,TupCamera::FAR_CLIP_PLANE);
}

TupperwareScalar *TupCamera::GetHorizontalFilmAperture(float &value)
{
	return GetFloatScalar(value,TupCamera::HORIZONAL_FILM_APERTURE);
}

TupperwareScalar *TupCamera::GetVerticalFilmAperture(float &value)
{
	return GetFloatScalar(value,TupCamera::VERTICAL_FILM_APERTURE);
}

TupperwareScalar *TupCamera::GetLensSqueezeRatio(float &value)
{
	return GetFloatScalar(value,TupCamera::LENS_SQUEEZE_RATIO);
}

TupperwareScalar *TupCamera::GetFlags(int &value)
{
	return GetIntScalar(value,TupCamera::FLAGS);
}

// private methods							

TupperwareScalar *TupCamera::AddFloatScalar(float value,TupCamera::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupCamera::GetFloatScalar(float &value,TupCamera::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *scalar = (TupperwareScalar *)pAtom;
		if (scalar->GetDataType()==TupperwareScalar::DATA_FLOAT)
		{
			value = scalar->GetAsFloat();
			return scalar;
		}
	}
	value = 0;
	return NULL;
}

TupperwareScalar *TupCamera::AddIntScalar(int value,TupCamera::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupCamera::GetIntScalar(int &value,TupCamera::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *scalar = (TupperwareScalar *)pAtom;
		if (scalar->GetDataType()==TupperwareScalar::DATA_INT)
		{
			value = scalar->GetAsInt();
			return scalar;
		}
	}
	value = 0;
	return NULL;
}

