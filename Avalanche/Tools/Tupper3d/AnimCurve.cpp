////////////////////////////////////////////////////////////////////////////
//
// AnimCurve
//
// Class to handle animation curves
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimCurve.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/16/03    Time: 1:50p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/14/03    Time: 10:53a
 * Updated in $/Avalanche/tools/Tupper3d
 * start of anim curve data
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "AnimCurve.h"

char *TupAnimCurve::m_keyNames[TupAnimCurve::NUM_CHILDREN_ATOMS] = 
{
	"StartTime", // START_TIME
	"EndTime", // END_TIME
	"InputValues", // INPUT_VALUES
	"OutputValues", // OUTPUT_VALUES
	"Flags", // FLAGS
	"KeyFlags", // KEY_FLAGS
	"InTangentAngles", //IN_TANGENT_ANGLES
	"OutTangentAngles", // OUT_TANGENT_ANGLES
	"InTangentWeights", // IN_TANGENT_WEIGHTS
	"OutTangentWeights", // OUT_TANGENT_WEIGHTS
	"InTangentX", //IN_TANGENT_X
	"OutTangentX", // OUT_TANGENT_X
	"InTangentY", // IN_TANGENT_Y
	"OutTangentY", // OUT_TANGENT_Y
};

TupAnimCurve::TupAnimCurve(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupAnimCurve::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupAnimCurve::GetBaseAggregate( void )
{
  return( m_pObject );
}

TupperwareScalar *TupAnimCurve::AddStartTime(float value)
{
	return AddFloatScalar(value,TupAnimCurve::START_TIME);
}

TupperwareScalar *TupAnimCurve::AddEndTime(float value)
{
	return AddFloatScalar(value,TupAnimCurve::END_TIME);
}

TupperwareScalar *TupAnimCurve::AddFlags(int value)
{
	return AddIntScalar(value,TupAnimCurve::FLAGS);
}

TupperwareList *TupAnimCurve::AddInputValues(const float *pValues,int numValues)
{
	return AddFloatList(pValues,numValues,TupAnimCurve::INPUT_VALUES);
}

TupperwareList *TupAnimCurve::AddOutputValues(const float *pValues,int numValues)
{
	return AddFloatList(pValues,numValues,TupAnimCurve::OUTPUT_VALUES);
}

TupperwareList *TupAnimCurve::AddKeyFlags(const int *pValues,int numValues)
{
	return AddIntList(pValues,numValues,TupAnimCurve::KEY_FLAGS);
}

TupperwareList *TupAnimCurve::AddInTangentAngles(const float *pValues,int numValues)
{
	return AddFloatList(pValues,numValues,TupAnimCurve::IN_TANGENT_ANGLES);
}

TupperwareList *TupAnimCurve::AddInTangentWeights(const float *pValues,int numValues)
{
	return AddFloatList(pValues,numValues,TupAnimCurve::IN_TANGENT_WEIGHTS);
}

TupperwareList *TupAnimCurve::AddOutTangentAngles(const float *pValues,int numValues)
{
	return AddFloatList(pValues,numValues,TupAnimCurve::OUT_TANGENT_ANGLES);
}

TupperwareList *TupAnimCurve::AddOutTangentWeights(const float *pValues,int numValues)
{
	return AddFloatList(pValues,numValues,TupAnimCurve::OUT_TANGENT_WEIGHTS);
}

TupperwareList *TupAnimCurve::AddInTangentX(const float *pValues,int numValues)
{
	return AddFloatList(pValues,numValues,TupAnimCurve::IN_TANGENT_X);
}

TupperwareList *TupAnimCurve::AddInTangentY(const float *pValues,int numValues)
{
	return AddFloatList(pValues,numValues,TupAnimCurve::IN_TANGENT_Y);
}

TupperwareList *TupAnimCurve::AddOutTangentX(const float *pValues,int numValues)
{
	return AddFloatList(pValues,numValues,TupAnimCurve::OUT_TANGENT_X);
}

TupperwareList *TupAnimCurve::AddOutTangentY(const float *pValues,int numValues)
{
	return AddFloatList(pValues,numValues,TupAnimCurve::OUT_TANGENT_Y);
}


TupperwareScalar *TupAnimCurve::GetStartTime(float &value)
{
	return GetFloatScalar(value,TupAnimCurve::START_TIME);
}

TupperwareScalar *TupAnimCurve::GetEndTime(float &value)
{
	return GetFloatScalar(value,TupAnimCurve::END_TIME);
}

TupperwareScalar *TupAnimCurve::GetFlags(int &value)
{
	return GetIntScalar(value,TupAnimCurve::FLAGS);
}

TupperwareList *TupAnimCurve::GetInputValues(float **pValues,int &numValues)
{
	return GetFloatList(pValues,numValues,TupAnimCurve::INPUT_VALUES);
}

TupperwareList *TupAnimCurve::GetOutputValues(float **pValues,int &numValues)
{
	return GetFloatList(pValues,numValues,TupAnimCurve::OUTPUT_VALUES);
}

TupperwareList *TupAnimCurve::GetKeyFlags(int **pValues,int &numValues)
{
	return GetIntList(pValues,numValues,TupAnimCurve::KEY_FLAGS);
}

TupperwareList *TupAnimCurve::GetInTangentAngles(float **pValues,int &numValues)
{
	return GetFloatList(pValues,numValues,TupAnimCurve::IN_TANGENT_ANGLES);
}

TupperwareList *TupAnimCurve::GetInTangentWeights(float **pValues,int &numValues)
{
	return GetFloatList(pValues,numValues,TupAnimCurve::IN_TANGENT_WEIGHTS);
}

TupperwareList *TupAnimCurve::GetOutTangentAngles(float **pValues,int &numValues)
{
	return GetFloatList(pValues,numValues,TupAnimCurve::OUT_TANGENT_ANGLES);
}

TupperwareList *TupAnimCurve::GetOutTangentWeights(float **pValues,int &numValues)
{
	return GetFloatList(pValues,numValues,TupAnimCurve::OUT_TANGENT_WEIGHTS);
}

TupperwareList *TupAnimCurve::GetInTangentX(float **pValues,int &numValues)
{
	return GetFloatList(pValues,numValues,TupAnimCurve::IN_TANGENT_X);
}

TupperwareList *TupAnimCurve::GetInTangentY(float **pValues,int &numValues)
{
	return GetFloatList(pValues,numValues,TupAnimCurve::IN_TANGENT_Y);
}

TupperwareList *TupAnimCurve::GetOutTangentX(float **pValues,int &numValues)
{
	return GetFloatList(pValues,numValues,TupAnimCurve::OUT_TANGENT_X);
}

TupperwareList *TupAnimCurve::GetOutTangentY(float **pValues,int &numValues)
{
	return GetFloatList(pValues,numValues,TupAnimCurve::OUT_TANGENT_Y);
}


// Private Methods
TupperwareList *TupAnimCurve::AddFloatList(const float *pFloats,int numFloats,TupAnimCurve::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupAnimCurve::AddIntList(const int *pInts,int numInts,TupAnimCurve::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListInt(m_keyNames[index],NULL,pInts,numInts);
	m_cachedAtoms[index] = list;
	return list;
}

TupperwareList *TupAnimCurve::GetFloatList(float **pFloats,int &numFloats,TupAnimCurve::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
	{
		TupperwareList *pList = (TupperwareList *)pAtom;
		if (pList->GetDataType()==TupperwareList::DATA_FLOAT)
		{
			*pFloats = pList->GetAsFloat();
			numFloats = pList->GetLength();
			return pList;
		}
	}
	*pFloats = NULL;
	numFloats = 0;
	return NULL;
}

TupperwareList *TupAnimCurve::GetIntList(int **pInts,int &numInts,TupAnimCurve::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
	{
		TupperwareList *pList = (TupperwareList *)pAtom;
		if (pList->GetDataType()==TupperwareList::DATA_INT)
		{
			*pInts = pList->GetAsInt();
			numInts = pList->GetLength();
			return pList;
		}
	}
	*pInts = NULL;
	numInts = 0;
	return NULL;
}

TupperwareScalar *TupAnimCurve::AddFloatScalar(float value,TupAnimCurve::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupAnimCurve::GetFloatScalar(float &value,TupAnimCurve::ChildrenAtoms index)
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

TupperwareScalar *TupAnimCurve::AddIntScalar(int value,TupAnimCurve::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupAnimCurve::GetIntScalar(int &value,TupAnimCurve::ChildrenAtoms index)
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

TupperwareList *TupAnimCurve::AddStringList(const char **pStrings,int numStrings,TupAnimCurve::ChildrenAtoms index)
{
   TupperwareAtom *pAtom = m_cachedAtoms[index];
   // delete if there
   if (pAtom)
      m_pObject->DeleteAtom(pAtom);
   TupperwareList *list = m_pObject->AddListString(m_keyNames[index],NULL,pStrings,numStrings);
   m_cachedAtoms[index] = (TupperwareAtom *)list;
   return list;
}

TupperwareList *TupAnimCurve::AddStringList(const TupArray<TupString> &strings,TupAnimCurve::ChildrenAtoms index)
{
   TupperwareAtom *pAtom = m_cachedAtoms[index];
   // delete if there
   if (pAtom)
      m_pObject->DeleteAtom(pAtom);
   TupperwareList *list = m_pObject->AddListString(m_keyNames[index],NULL,strings);
   m_cachedAtoms[index] = (TupperwareAtom *)list;
   return list;
}

TupperwareList *TupAnimCurve::GetStringList(char ***pStrings,int &numStrings,TupAnimCurve::ChildrenAtoms index)
{
   TupperwareAtom *pAtom = m_cachedAtoms[index];
   if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
   {
      TupperwareList *pList = (TupperwareList *)pAtom;
      if (pList->GetDataType()==TupperwareList::DATA_STRING)
      {
         numStrings = pList->GetLength();
         *pStrings = pList->GetAsString();
         return pList;
      }
   }
   // if we could not get the data then return a null pointer and init the count to 0
   *pStrings = NULL;
   numStrings = 0;
   return NULL;
}

TupperwareList *TupAnimCurve::GetStringList(TupArray<TupString> &strings,TupAnimCurve::ChildrenAtoms index)
{
   TupperwareAtom *pAtom = m_cachedAtoms[index];
   if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
   {
      TupperwareList *pList = (TupperwareList *)pAtom;
      if (pList->GetDataType()==TupperwareList::DATA_STRING)
      {
         pList->GetAsString(strings);
         return pList;
      }
   }
   // if we could not get the data then return a null pointer and init the count to 0
   strings.RemoveAll();
   return NULL;
}

TupperwareList *TupAnimCurve::AddOutputValues(const char **pStrings,int numValues)
{
   return AddStringList(pStrings,numValues,TupAnimCurve::OUTPUT_VALUES);
}

TupperwareList *TupAnimCurve::AddOutputValues(const TupArray<TupString> &strings)
{
   return AddStringList(strings,TupAnimCurve::OUTPUT_VALUES);
}

TupperwareList *TupAnimCurve::GetOutputValues(char ***pStrings,int &numValues)
{
   return GetStringList(pStrings,numValues,TupAnimCurve::OUTPUT_VALUES);
}

TupperwareList *TupAnimCurve::GetOutputValues(TupArray<TupString> &strings)
{
   return GetStringList(strings,TupAnimCurve::OUTPUT_VALUES);
}