////////////////////////////////////////////////////////////////////////////
//
// BitmapMap
//
// Class to handle the bitmap map
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: bitmapmap.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 6/27/03    Time: 2:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * map attributes export
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 1/29/03    Time: 12:09p
 * Updated in $/Avalanche/tools/Tupper3d
 * added blend mode
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
 * User: Adam Clayton Date: 12/17/01   Time: 11:06a
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:27p
 * Updated in $/Avalanche/tools/Tupper3d
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "bitmapmap.h"
#include "map.h"

char *TupBitmapMap::m_keyNames[TupBitmapMap::NUM_CHILDREN_ATOMS] = 
{
		"Name", // NAME
		"ImageReference", // IMAGE_REFERENCE
		"Mapping", // MAPPING
		"MapChannel", // MAP_CHANNEL
		"UOffset", // U_OFFSET
		"VOffset", // V_OFFSET
		"UTiling", // U_TILING
		"VTiling", // V_TILING
		"MapAxis", // MAP_AXIS
		"UAngle", // U_ANGLE
		"VAngle", // V_ANGLE
		"WAngle", // W_ANGLE
		"Blur", // BLUR
		"BlueOffset", // BLUR_OFFSET
		"NoiseAmount", // NOISE_AMOUNT
		"NoiseLevels", // NOISE_LEVELS
		"NoiseSize", // NOISE_SIZE
		"CroppingPlacementU", // CROPPING_PLACEMENT_U
		"CroppingPlacementV", // CROPPING_PLACEMENT_V
		"CroppingPlacementW", // CROPPING_PLACEMENT_W
		"CroppingPlacementH", // CROPPING_PLACEMENT_H
		"PlacementJitter", // PLACEMENT_JITTER
		"StartFrame", // START_FRAME
		"PlaybackRate", // PLAYBACK_RATE
		"OutputAmount", // OUTPUT_AMOUNT
		"OutputRGBOffset", // OUTPUT_RGB_OFFSET
		"OutputRGBLevel", // OUTPUT_RGB_LEVEL
		"OutputBumpAmount", // OUTPUT_BUMP_AMOUNT
		"BlendMode", // BLEND_MODE
		"Flags", // FLAGS
		"UserProperties", // USER_PROPERTIES
};

TupBitmapMap::TupBitmapMap(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupBitmapMap::TupBitmapMap(TupMap &mapBase)
{
	m_pObject = mapBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupBitmapMap::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareScalar *TupBitmapMap::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupBitmapMap::AddImageReference(int value)
{
	return AddIntScalar(value,TupBitmapMap::IMAGE_REFERENCE);
}

TupperwareScalar *TupBitmapMap::AddMapping(int value)
{
	return AddIntScalar(value,TupBitmapMap::MAPPING);
}

TupperwareScalar *TupBitmapMap::AddMapChannel(int value)
{
	return AddIntScalar(value,TupBitmapMap::MAP_CHANNEL);
}

TupperwareScalar *TupBitmapMap::AddUOffset(float value)
{
	return AddFloatScalar(value,TupBitmapMap::U_OFFSET);
}

TupperwareScalar *TupBitmapMap::AddVOffset(float value)
{
	return AddFloatScalar(value,TupBitmapMap::V_OFFSET);
}

TupperwareScalar *TupBitmapMap::AddUTiling(float value)
{
	return AddFloatScalar(value,TupBitmapMap::U_TILING);
}

TupperwareScalar *TupBitmapMap::AddVTiling(float value)
{
	return AddFloatScalar(value,TupBitmapMap::V_TILING);
}

TupperwareScalar *TupBitmapMap::AddMapAxis(int value)
{
	return AddIntScalar(value,TupBitmapMap::MAP_AXIS);
}

TupperwareScalar *TupBitmapMap::AddUAngle(float value)
{
	return AddFloatScalar(value,TupBitmapMap::U_ANGLE);
}

TupperwareScalar *TupBitmapMap::AddVAngle(float value)
{
	return AddFloatScalar(value,TupBitmapMap::V_ANGLE);
}

TupperwareScalar *TupBitmapMap::AddWAngle(float value)
{
	return AddFloatScalar(value,TupBitmapMap::W_ANGLE);
}

TupperwareScalar *TupBitmapMap::AddBlur(float value)
{
	return AddFloatScalar(value,TupBitmapMap::BLUR);
}

TupperwareScalar *TupBitmapMap::AddBlurOffset(float value)
{
	return AddFloatScalar(value,TupBitmapMap::BLUR_OFFSET);
}

TupperwareScalar *TupBitmapMap::AddNoiseAmount(float value)
{
	return AddFloatScalar(value,TupBitmapMap::NOISE_AMOUNT);
}

TupperwareScalar *TupBitmapMap::AddNoiseLevels(int value)
{
	return AddIntScalar(value,TupBitmapMap::NOISE_LEVELS);
}

TupperwareScalar *TupBitmapMap::AddNoiseSize(float value)
{
	return AddFloatScalar(value,TupBitmapMap::NOISE_SIZE);
}

TupperwareScalar *TupBitmapMap::AddCroppingPlacementU(float value)
{
	return AddFloatScalar(value,TupBitmapMap::CROPPING_PLACEMENT_U);
}

TupperwareScalar *TupBitmapMap::AddCroppingPlacementV(float value)
{
	return AddFloatScalar(value,TupBitmapMap::CROPPING_PLACEMENT_V);
}

TupperwareScalar *TupBitmapMap::AddCroppingPlacementW(float value)
{
	return AddFloatScalar(value,TupBitmapMap::CROPPING_PLACEMENT_W);
}

TupperwareScalar *TupBitmapMap::AddCroppingPlacementH(float value)
{
	return AddFloatScalar(value,TupBitmapMap::CROPPING_PLACEMENT_H);
}

TupperwareScalar *TupBitmapMap::AddPlacementJitter(float value)
{
	return AddFloatScalar(value,TupBitmapMap::PLACEMENT_JITTER);
}

TupperwareScalar *TupBitmapMap::AddStartFrame(int value)
{
	return AddIntScalar(value,TupBitmapMap::START_FRAME);
}

TupperwareScalar *TupBitmapMap::AddPlaybackRate(float value)
{
	return AddFloatScalar(value,TupBitmapMap::PLAYBACK_RATE);
}

TupperwareScalar *TupBitmapMap::AddOutputAmount(float value)
{
	return AddFloatScalar(value,TupBitmapMap::OUTPUT_AMOUNT);
}

TupperwareScalar *TupBitmapMap::AddOutputRGBOffset(float value)
{
	return AddFloatScalar(value,TupBitmapMap::OUTPUT_RGB_OFFSET);
}

TupperwareScalar *TupBitmapMap::AddOutputRGBLevel(float value)
{
	return AddFloatScalar(value,TupBitmapMap::OUTPUT_RGB_LEVEL);
}

TupperwareScalar *TupBitmapMap::AddOutputBumpAmount(float value)
{
	return AddFloatScalar(value,TupBitmapMap::OUTPUT_BUMP_AMOUNT);
}

TupperwareScalar *TupBitmapMap::AddBlendMode(int value)
{
	return AddIntScalar(value,TupBitmapMap::BLEND_MODE);
}

TupperwareScalar *TupBitmapMap::AddFlags(int value)
{
	return AddIntScalar(value,TupBitmapMap::FLAGS);
}

TupperwareScalar *TupBitmapMap::AddUserProperties(const char *userPropertyString)
{
	TupperwareAtom *pAtom = m_cachedAtoms[USER_PROPERTIES];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[USER_PROPERTIES],NULL,userPropertyString);
	m_cachedAtoms[USER_PROPERTIES] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupBitmapMap::GetName(char **name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType() == TupperwareScalar::DATA_STRING)
		{
			*name = pScalar->GetAsString();
			return pScalar;
		}
	}
	*name = NULL;
	return NULL;
}

TupperwareScalar *TupBitmapMap::GetImageReference(int &value)
{
	return GetIntScalar(value,TupBitmapMap::IMAGE_REFERENCE);
}

TupperwareScalar *TupBitmapMap::GetMapping(int &value)
{
	return GetIntScalar(value,TupBitmapMap::MAPPING);
}

TupperwareScalar *TupBitmapMap::GetMapChannel(int &value)
{
	return GetIntScalar(value,TupBitmapMap::MAP_CHANNEL);
}

TupperwareScalar *TupBitmapMap::GetUOffset(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::U_OFFSET);
}

TupperwareScalar *TupBitmapMap::GetVOffset(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::V_OFFSET);
}

TupperwareScalar *TupBitmapMap::GetUTiling(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::U_TILING);
}

TupperwareScalar *TupBitmapMap::GetVTiling(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::V_TILING);
}

TupperwareScalar *TupBitmapMap::GetMapAxis(int &value)
{
	return GetIntScalar(value,TupBitmapMap::MAP_AXIS);
}

TupperwareScalar *TupBitmapMap::GetUAngle(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::U_ANGLE);
}

TupperwareScalar *TupBitmapMap::GetVAngle(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::V_ANGLE);
}

TupperwareScalar *TupBitmapMap::GetWAngle(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::W_ANGLE);
}

TupperwareScalar *TupBitmapMap::GetBlur(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::BLUR);
}

TupperwareScalar *TupBitmapMap::GetBlurOffset(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::BLUR_OFFSET);
}

TupperwareScalar *TupBitmapMap::GetNoiseAmount(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::NOISE_AMOUNT);
}

TupperwareScalar *TupBitmapMap::GetNoiseLevels(int &value)
{
	return GetIntScalar(value,TupBitmapMap::NOISE_LEVELS);
}

TupperwareScalar *TupBitmapMap::GetNoiseSize(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::NOISE_SIZE);
}

TupperwareScalar *TupBitmapMap::GetCroppingPlacementU(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::CROPPING_PLACEMENT_U);
}

TupperwareScalar *TupBitmapMap::GetCroppingPlacementV(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::CROPPING_PLACEMENT_V);
}

TupperwareScalar *TupBitmapMap::GetCroppingPlacementW(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::CROPPING_PLACEMENT_W);
}

TupperwareScalar *TupBitmapMap::GetCroppingPlacementH(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::CROPPING_PLACEMENT_H);
}

TupperwareScalar *TupBitmapMap::GetPlacementJitter(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::PLACEMENT_JITTER);
}

TupperwareScalar *TupBitmapMap::GetStartFrame(int &value)
{
	return GetIntScalar(value,TupBitmapMap::START_FRAME);
}

TupperwareScalar *TupBitmapMap::GetPlaybackRate(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::PLAYBACK_RATE);
}

TupperwareScalar *TupBitmapMap::GetOutputAmount(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::OUTPUT_AMOUNT);
}

TupperwareScalar *TupBitmapMap::GetOutputRGBOffset(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::OUTPUT_RGB_OFFSET);
}

TupperwareScalar *TupBitmapMap::GetOutputRGBLevel(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::OUTPUT_RGB_LEVEL);
}

TupperwareScalar *TupBitmapMap::GetOutputBumpAmount(float &value)
{
	return GetFloatScalar(value,TupBitmapMap::OUTPUT_BUMP_AMOUNT);
}

TupperwareScalar *TupBitmapMap::GetBlendMode(int &value)
{
	return GetIntScalar(value,TupBitmapMap::BLEND_MODE);
}

TupperwareScalar *TupBitmapMap::GetFlags(int &value)
{
	return GetIntScalar(value,TupBitmapMap::FLAGS);
}

TupperwareScalar *TupBitmapMap::GetUserProperties(char **userPropertyString)
{
	TupperwareAtom *pAtom = m_cachedAtoms[USER_PROPERTIES];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType() == TupperwareScalar::DATA_STRING)
		{
			*userPropertyString = pScalar->GetAsString();
			return pScalar;
		}
	}
	*userPropertyString = NULL;
	return NULL;
}

// Private Methods

TupperwareScalar *TupBitmapMap::AddFloatScalar(float value,TupBitmapMap::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupBitmapMap::GetFloatScalar(float &value,TupBitmapMap::ChildrenAtoms index)
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

TupperwareScalar *TupBitmapMap::AddIntScalar(int value,TupBitmapMap::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupBitmapMap::GetIntScalar(int &value,TupBitmapMap::ChildrenAtoms index)
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
