////////////////////////////////////////////////////////////////////////////
//
// AnimSourceClip
//
// Class to handle an source clip
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimSourceClip.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "AnimSourceClip.h"

char *TupAnimSourceClip::m_keyNames[TupAnimSourceClip::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"Channels", // CHANNELS
	"StartTime", // START_TIME
	"Duration", // DURATION
	"UserProperties", // USER_PROPERTIES
	"Flags", // FLAGS
};

TupAnimSourceClip::TupAnimSourceClip(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupAnimSourceClip::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupAnimSourceClip::GetBaseAggregate( void )
{
	return( m_pObject );
}

TupperwareScalar *TupAnimSourceClip::AddName(const char *name)
{
	return AddStringScalar(name,NAME);
}

TupperwareAggregate *TupAnimSourceClip::AddChannels(void)
{
	return AddAggregate(CHANNELS);
}

TupperwareScalar *TupAnimSourceClip::AddStartTime(float time)
{
	return AddFloatScalar(time,START_TIME);
}

TupperwareScalar *TupAnimSourceClip::AddDuration(float duration)
{
	return AddFloatScalar(duration,DURATION);
}

TupperwareScalar *TupAnimSourceClip::AddUserProperties(const char *userPropertyString)
{
	return AddStringScalar(userPropertyString,USER_PROPERTIES);
}

TupperwareScalar *TupAnimSourceClip::AddFlags(int flags)
{
	return AddIntScalar(flags,FLAGS);
}

// get methods

TupperwareScalar *TupAnimSourceClip::GetName(char **name)
{
	return GetStringScalar(name,NAME);
}

TupperwareAggregate *TupAnimSourceClip::GetChannels(void)
{
	return GetAggregate(CHANNELS);
}

TupperwareScalar *TupAnimSourceClip::GetStartTime(float &time)
{
	return GetFloatScalar(time,START_TIME);
}

TupperwareScalar *TupAnimSourceClip::GetDuration(float &duration)
{
	return GetFloatScalar(duration,DURATION);
}

TupperwareScalar *TupAnimSourceClip::GetUserProperties(char **userPropertyString)
{
	return GetStringScalar(userPropertyString,USER_PROPERTIES);
}

TupperwareScalar *TupAnimSourceClip::GetFlags(int &flags)
{
	return GetIntScalar(flags,FLAGS);
}

// private methods
TupperwareScalar *TupAnimSourceClip::AddStringScalar(const char *pString,TupAnimSourceClip::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *pScalar = m_pObject->AddScalarString(m_keyNames[index],NULL,pString);
	m_cachedAtoms[index] = (TupperwareAtom *)pScalar;
	return pScalar;
}

TupperwareScalar *TupAnimSourceClip::GetStringScalar(char **pString,TupAnimSourceClip::ChildrenAtoms index)
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

TupperwareScalar *TupAnimSourceClip::AddIntScalar(int value,TupAnimSourceClip::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupAnimSourceClip::GetIntScalar(int &value,TupAnimSourceClip::ChildrenAtoms index)
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

TupperwareScalar *TupAnimSourceClip::AddFloatScalar(float value,TupAnimSourceClip::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupAnimSourceClip::GetFloatScalar(float &value,TupAnimSourceClip::ChildrenAtoms index)
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

TupperwareAggregate *TupAnimSourceClip::AddAggregate(TupAnimSourceClip::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareAggregate *pAgg = m_pObject->AddAggregate(m_keyNames[index],NULL);
	m_cachedAtoms[index] = (TupperwareAtom *)pAgg;
	return pAgg;
}

TupperwareAggregate *TupAnimSourceClip::GetAggregate(TupAnimSourceClip::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
	{
		return (TupperwareAggregate *)pAtom;
	}
	return NULL;
}
