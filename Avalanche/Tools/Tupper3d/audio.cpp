////////////////////////////////////////////////////////////////////////////
//
// Audio
//
// Class to handle an source clip
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: audio.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 6/17/03    Time: 2:57p
 * Updated in $/Avalanche/tools/Tupper3d
 * add audio clips
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "Audio.h"

char *TupAudio::m_keyNames[TupAudio::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"StartTime", // START_TIME
	"FileName", // FILE_NAME
	"Flags", // FLAGS
};

TupAudio::TupAudio(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupAudio::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupAudio::GetBaseAggregate( void )
{
	return( m_pObject );
}

TupperwareScalar *TupAudio::AddName(const char *name)
{
	return AddStringScalar(name,NAME);
}

TupperwareScalar *TupAudio::AddStartTime(float time)
{
	return AddFloatScalar(time,START_TIME);
}

TupperwareScalar *TupAudio::AddFileName(const char *filename)
{
	return AddStringScalar(filename,FILE_NAME);
}

TupperwareScalar *TupAudio::AddFlags(int flags)
{
	return AddIntScalar(flags,FLAGS);
}

// get methods

TupperwareScalar *TupAudio::GetName(char **name)
{
	return GetStringScalar(name,NAME);
}

TupperwareScalar *TupAudio::GetStartTime(float &time)
{
	return GetFloatScalar(time,START_TIME);
}

TupperwareScalar *TupAudio::GetFileName(char **filename)
{
	return GetStringScalar(filename,FILE_NAME);
}

TupperwareScalar *TupAudio::GetFlags(int &flags)
{
	return GetIntScalar(flags,FLAGS);
}

// private methods
TupperwareScalar *TupAudio::AddStringScalar(const char *pString,TupAudio::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *pScalar = m_pObject->AddScalarString(m_keyNames[index],NULL,pString);
	m_cachedAtoms[index] = (TupperwareAtom *)pScalar;
	return pScalar;
}

TupperwareScalar *TupAudio::GetStringScalar(char **pString,TupAudio::ChildrenAtoms index)
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

TupperwareScalar *TupAudio::AddIntScalar(int value,TupAudio::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupAudio::GetIntScalar(int &value,TupAudio::ChildrenAtoms index)
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

TupperwareScalar *TupAudio::AddFloatScalar(float value,TupAudio::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupAudio::GetFloatScalar(float &value,TupAudio::ChildrenAtoms index)
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

TupperwareAggregate *TupAudio::AddAggregate(TupAudio::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareAggregate *pAgg = m_pObject->AddAggregate(m_keyNames[index],NULL);
	m_cachedAtoms[index] = (TupperwareAtom *)pAgg;
	return pAgg;
}

TupperwareAggregate *TupAudio::GetAggregate(TupAudio::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
	{
		return (TupperwareAggregate *)pAtom;
	}
	return NULL;
}
