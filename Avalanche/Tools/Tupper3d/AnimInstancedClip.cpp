////////////////////////////////////////////////////////////////////////////
//
// AnimInstancedClip
//
// Class to handle an instanced clip
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimInstancedClip.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 6/06/03    Time: 4:06p
 * Updated in $/Avalanche/tools/Tupper3d
 * instanced clip curves
 * camera lookat
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "AnimInstancedClip.h"

char *TupAnimInstancedClip::m_keyNames[TupAnimInstancedClip::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"SourceClipReference", // SOURCE_CLIP_REFERENCE
	"StartTime", // START_TIME
	"Cycle", // CYCLE
	"Weight", // WEIGHT
	"Scale", // SCALE
	"UserProperties", // USER_PROPERTIES
	"AnimatedChannels", // ANIMATED_CHANNELS
	"Flags", // FLAGS
};

TupAnimInstancedClip::TupAnimInstancedClip(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupAnimInstancedClip::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupAnimInstancedClip::GetBaseAggregate( void )
{
	return( m_pObject );
}

TupperwareScalar *TupAnimInstancedClip::AddName(const char *name)
{
	return AddStringScalar(name,NAME);
}

TupperwareScalar *TupAnimInstancedClip::AddSourceClipReference(int ref)
{
	return AddIntScalar(ref,SOURCE_CLIP_REFERENCE);
}

TupperwareScalar *TupAnimInstancedClip::AddStartTime(float time)
{
	return AddFloatScalar(time,START_TIME);
}

TupperwareScalar *TupAnimInstancedClip::AddCycle(float cycle)
{
	return AddFloatScalar(cycle,CYCLE);
}

TupperwareScalar *TupAnimInstancedClip::AddWeight(float weight)
{
	return AddFloatScalar(weight,WEIGHT);
}

TupperwareScalar *TupAnimInstancedClip::AddScale(float scale)
{
	return AddFloatScalar(scale,SCALE);
}

TupperwareScalar *TupAnimInstancedClip::AddUserProperties(const char *userPropertyString)
{
	return AddStringScalar(userPropertyString,USER_PROPERTIES);
}

TupperwareScalar *TupAnimInstancedClip::AddFlags(int flags)
{
	return AddIntScalar(flags,FLAGS);
}

TupperwareAggregate *TupAnimInstancedClip::AddAnimagedChannels(void)
{
	return AddAggregate(ANIMATED_CHANNELS);
}

// get methods

TupperwareScalar *TupAnimInstancedClip::GetName(char **name)
{
	return GetStringScalar(name,NAME);
}

TupperwareScalar *TupAnimInstancedClip::GetSourceClipReference(int &ref)
{
	return GetIntScalar(ref,SOURCE_CLIP_REFERENCE);
}

TupperwareScalar *TupAnimInstancedClip::GetStartTime(float &time)
{
	return GetFloatScalar(time,START_TIME);
}

TupperwareScalar *TupAnimInstancedClip::GetCycle(float &cycle)
{
	return GetFloatScalar(cycle,CYCLE);
}

TupperwareScalar *TupAnimInstancedClip::GetWeight(float &weight)
{
	return GetFloatScalar(weight,WEIGHT);
}

TupperwareScalar *TupAnimInstancedClip::GetScale(float &scale)
{
	return GetFloatScalar(scale,SCALE);
}

TupperwareScalar *TupAnimInstancedClip::GetUserProperties(char **userPropertyString)
{
	return GetStringScalar(userPropertyString,USER_PROPERTIES);
}

TupperwareScalar *TupAnimInstancedClip::GetFlags(int &flags)
{
	return GetIntScalar(flags,FLAGS);
}

TupperwareAggregate *TupAnimInstancedClip::GetAnimagedChannels(void)
{
	return GetAggregate(ANIMATED_CHANNELS);
}


// private methods
TupperwareScalar *TupAnimInstancedClip::AddStringScalar(const char *pString,TupAnimInstancedClip::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *pScalar = m_pObject->AddScalarString(m_keyNames[index],NULL,pString);
	m_cachedAtoms[index] = (TupperwareAtom *)pScalar;
	return pScalar;
}

TupperwareScalar *TupAnimInstancedClip::GetStringScalar(char **pString,TupAnimInstancedClip::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *) pAtom;
		if (pScalar->GetDataType() == TupperwareScalar::DATA_STRING)
		{
			*pString = pScalar->GetAsString();
			return pScalar;
		}
	}
	*pString = NULL;
	return NULL;
}

TupperwareScalar *TupAnimInstancedClip::AddIntScalar(int value,TupAnimInstancedClip::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupAnimInstancedClip::GetIntScalar(int &value,TupAnimInstancedClip::ChildrenAtoms index)
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

TupperwareScalar *TupAnimInstancedClip::AddFloatScalar(float value,TupAnimInstancedClip::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupAnimInstancedClip::GetFloatScalar(float &value,TupAnimInstancedClip::ChildrenAtoms index)
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
	value = 0.0f;
	return NULL;
}

TupperwareAggregate *TupAnimInstancedClip::GetAggregate(TupAnimInstancedClip::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
	{
		return (TupperwareAggregate *)pAtom;
	}
	return NULL;
}

TupperwareAggregate *TupAnimInstancedClip::AddAggregate(TupAnimInstancedClip::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareAggregate *pAgg = m_pObject->AddAggregate(m_keyNames[index],NULL);
	m_cachedAtoms[index] = (TupperwareAtom *)pAgg;
	return pAgg;
}
