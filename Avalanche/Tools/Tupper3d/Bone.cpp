////////////////////////////////////////////////////////////////////////////
//
// Bone
//
// Class to handle bones
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: Bone.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/18/02    Time: 2:11p
 * Updated in $/Avalanche/tools/Tupper3d
 * Bone Export Complete
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "bone.h"
#include "object.h"

char *TupBone::m_keyNames[TupBone::NUM_CHILDREN_ATOMS] = 
{
	"Width", // WIDTH
	"Height", // HEIGHT
	"Taper", // TAPER
	"Length", // LENGTH
	"Flags", // FLAGS
	"SideFinsSize", // SIDE_FINS_SIZE
	"SideFinsStartTaper", // SIDE_FINS_START_TAPER
	"SideFinsEndTaper", // SIDE_FINS_END_TAPER
	"FrontFinSize", // FRONT_FIN_SIZE
	"FrontFinStartTaper", // FRONT_FIN_START_TAPER
	"FrontFinEndTaper", // FRONT_FIN_END_TAPER
	"BackFinSize", // BACK_FIN_SIZE
	"BackFinStartTaper", // BACK_FIN_START_TAPER
	"BackFinEndTaper", // BACK_FIN_END_TAPER
};

TupBone::TupBone(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupBone::TupBone(TupObject &objectBase)
{
	m_pObject = objectBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupBone::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupBone::GetBaseAggregate( void )
{
  return( m_pObject );
}

TupperwareScalar *TupBone::AddFlags(int flags)
{
	return AddIntScalar(flags,TupBone::FLAGS);
}

TupperwareScalar *TupBone::AddWidth(float value)
{
	return AddFloatScalar(value,TupBone::WIDTH);
}

TupperwareScalar *TupBone::AddHeight(float value)
{
	return AddFloatScalar(value,TupBone::HEIGHT);
}

TupperwareScalar *TupBone::AddTaper(float value)
{
	return AddFloatScalar(value,TupBone::TAPER);
}

TupperwareScalar *TupBone::AddLength(float value)
{
	return AddFloatScalar(value,TupBone::LENGTH);
}

TupperwareScalar *TupBone::AddSideFinsSize(float value)
{
	return AddFloatScalar(value,TupBone::SIDE_FINS_SIZE);
}

TupperwareScalar *TupBone::AddSideFinsStartTaper(float value)
{
	return AddFloatScalar(value,TupBone::SIDE_FINS_START_TAPER);
}

TupperwareScalar *TupBone::AddSideFinsEndTaper(float value)
{
	return AddFloatScalar(value,TupBone::SIDE_FINS_END_TAPER);
}

TupperwareScalar *TupBone::AddFrontFinSize(float value)
{
	return AddFloatScalar(value,TupBone::FRONT_FIN_SIZE);
}

TupperwareScalar *TupBone::AddFrontFinStartTaper(float value)
{
	return AddFloatScalar(value,TupBone::FRONT_FIN_START_TAPER);
}

TupperwareScalar *TupBone::AddFrontFinEndTaper(float value)
{
	return AddFloatScalar(value,TupBone::FRONT_FIN_END_TAPER);
}

TupperwareScalar *TupBone::AddBackFinSize(float value)
{
	return AddFloatScalar(value,TupBone::BACK_FIN_SIZE);
}

TupperwareScalar *TupBone::AddBackFinStartTaper(float value)
{
	return AddFloatScalar(value,TupBone::BACK_FIN_START_TAPER);
}

TupperwareScalar *TupBone::AddBackFinEndTaper(float value)
{
	return AddFloatScalar(value,TupBone::BACK_FIN_END_TAPER);
}


// get methods
TupperwareScalar *TupBone::GetFlags(int &flags)
{
	return GetIntScalar(flags,TupBone::FLAGS);
}

TupperwareScalar *TupBone::GetWidth(float &value)
{
	return GetFloatScalar(value,TupBone::WIDTH);
}

TupperwareScalar *TupBone::GetHeight(float &value)
{
	return GetFloatScalar(value,TupBone::HEIGHT);
}

TupperwareScalar *TupBone::GetTaper(float &value)
{
	return GetFloatScalar(value,TupBone::TAPER);
}

TupperwareScalar *TupBone::GetLength(float &value)
{
	return GetFloatScalar(value,TupBone::LENGTH);
}

TupperwareScalar *TupBone::GetSideFinsSize(float &value)
{
	return GetFloatScalar(value,TupBone::SIDE_FINS_SIZE);
}

TupperwareScalar *TupBone::GetSideFinsStartTaper(float &value)
{
	return GetFloatScalar(value,TupBone::SIDE_FINS_START_TAPER);
}

TupperwareScalar *TupBone::GetSideFinsEndTaper(float &value)
{
	return GetFloatScalar(value,TupBone::SIDE_FINS_END_TAPER);
}

TupperwareScalar *TupBone::GetFrontFinSize(float &value)
{
	return GetFloatScalar(value,TupBone::FRONT_FIN_SIZE);
}

TupperwareScalar *TupBone::GetFrontFinStartTaper(float &value)
{
	return GetFloatScalar(value,TupBone::FRONT_FIN_START_TAPER);
}

TupperwareScalar *TupBone::GetFrontFinEndTaper(float &value)
{
	return GetFloatScalar(value,TupBone::FRONT_FIN_END_TAPER);
}

TupperwareScalar *TupBone::GetBackFinSize(float &value)
{
	return GetFloatScalar(value,TupBone::BACK_FIN_SIZE);
}

TupperwareScalar *TupBone::GetBackFinStartTaper(float &value)
{
	return GetFloatScalar(value,TupBone::BACK_FIN_START_TAPER);
}

TupperwareScalar *TupBone::GetBackFinEndTaper(float &value)
{
	return GetFloatScalar(value,TupBone::BACK_FIN_END_TAPER);
}


// Private Methods
TupperwareScalar *TupBone::AddFloatScalar(float value,TupBone::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupBone::AddIntScalar(int value,TupBone::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupBone::GetFloatScalar(float &value,TupBone::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType()==TupperwareList::DATA_FLOAT)
		{
			value = pScalar->GetAsFloat();
			return pScalar;
		}
	}
	value = 0;
	return NULL;
}

TupperwareScalar *TupBone::GetIntScalar(int &value,TupBone::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType()==TupperwareList::DATA_INT)
		{
			value = pScalar->GetAsInt();
			return pScalar;
		}
	}
	value = 0;
	return NULL;
}
