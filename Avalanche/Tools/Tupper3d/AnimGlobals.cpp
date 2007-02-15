////////////////////////////////////////////////////////////////////////////
//
// AnimGlobals
//
// Handle animation global settings
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimGlobals.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 6/02/03    Time: 10:34a
 * Updated in $/Avalanche/tools/Tupper3d
 * animation globals
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "AnimGlobals.h"

char *TupAnimGlobals::m_keyNames[TupAnimGlobals::NUM_CHILDREN_ATOMS] = 
{
	"CurrentTime", // CURRENT_TIME
	"StartTime", // START_TIME
	"EndTime", // END_TIME
	"StartPlaybackTime", // START_PLAYBACK_TIME
	"EndPlaybackTime", // END_PLAYBACK_TIME
	"PlaybackBy", // PLAYBACK_BY
	"PlaybackSpeed", // PLAYBACK_SPEED
	"Flags", // FLAGS
};

TupAnimGlobals::TupAnimGlobals(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupAnimGlobals::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupAnimGlobals::GetBaseAggregate( void )
{
	return( m_pObject );
}

TupperwareScalar *TupAnimGlobals::AddCurrentTime(float time)
{
	return AddFloatScalar(time,CURRENT_TIME);
}

TupperwareScalar *TupAnimGlobals::AddStartTime(float time)
{
	return AddFloatScalar(time,START_TIME);
}

TupperwareScalar *TupAnimGlobals::AddEndTime(float time)
{
	return AddFloatScalar(time,END_TIME);
}

TupperwareScalar *TupAnimGlobals::AddStartPlaybackTime(float time)
{
	return AddFloatScalar(time,START_PLAYBACK_TIME);
}

TupperwareScalar *TupAnimGlobals::AddEndPlaybackTime(float time)
{
	return AddFloatScalar(time,END_PLAYBACK_TIME);
}

TupperwareScalar *TupAnimGlobals::AddPlaybackBy(float increment)
{
	return AddFloatScalar(increment,PLAYBACK_BY);
}

TupperwareScalar *TupAnimGlobals::AddPlaybackSpeed(float speed)
{
	return AddFloatScalar(speed,PLAYBACK_SPEED);
}

TupperwareScalar *TupAnimGlobals::AddFlags(int flags)
{
	return AddIntScalar(flags,FLAGS);
}


// get methods
TupperwareScalar *TupAnimGlobals::GetCurrentTime(float &time)
{
	return GetFloatScalar(time,CURRENT_TIME);
}

TupperwareScalar *TupAnimGlobals::GetStartTime(float &time)
{
	return GetFloatScalar(time,START_TIME);
}

TupperwareScalar *TupAnimGlobals::GetEndTime(float &time)
{
	return GetFloatScalar(time,END_TIME);
}

TupperwareScalar *TupAnimGlobals::GetStartPlaybackTime(float &time)
{
	return GetFloatScalar(time,START_PLAYBACK_TIME);
}

TupperwareScalar *TupAnimGlobals::GetEndPlaybackTime(float &time)
{
	return GetFloatScalar(time,END_PLAYBACK_TIME);
}

TupperwareScalar *TupAnimGlobals::GetPlaybackBy(float &increment)
{
	return GetFloatScalar(increment,PLAYBACK_BY);
}

TupperwareScalar *TupAnimGlobals::GetPlaybackSpeed(float &speed)
{
	return GetFloatScalar(speed,PLAYBACK_SPEED);
}

TupperwareScalar *TupAnimGlobals::GetFlags(int &flags)
{
	return GetIntScalar(flags,FLAGS);
}


// private methods
TupperwareScalar *TupAnimGlobals::AddIntScalar(int value,TupAnimGlobals::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupAnimGlobals::GetIntScalar(int &value,TupAnimGlobals::ChildrenAtoms index)
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

TupperwareScalar *TupAnimGlobals::AddFloatScalar(float value,TupAnimGlobals::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupAnimGlobals::GetFloatScalar(float &value,TupAnimGlobals::ChildrenAtoms index)
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
