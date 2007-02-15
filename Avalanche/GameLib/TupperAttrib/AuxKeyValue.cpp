////////////////////////////////////////////////////////////////////////////
//
// AuxKeyValue
//
// Class to deal with the auxiliary key value pairs
//
////////////////////////////////////////////////////////////////////////////

#include "TupperAttribPch.h"
#include "AuxKeyValue.h"
#include "KeyValueContext.h"



// TupAuxKeyValue::Entry
TupAuxKeyValue::Entry::Entry(void)
{
}

TupAuxKeyValue::Entry::Entry(const char *pEntryName,const char *pEntryValues) :
	m_entryName(pEntryName),m_keyValue(pEntryValues)
{
}

TupAuxKeyValue::Entry::Entry(const TupAuxKeyValue::Entry& src) :
	m_entryName(src.m_entryName),
	m_keyValue(src.m_keyValue)
{
}

const TupAuxKeyValue::Entry& TupAuxKeyValue::Entry::operator=(const TupAuxKeyValue::Entry& src)
{
	m_entryName = src.m_entryName;
	m_keyValue = src.m_keyValue;
	return *this;
}


// TupAuxKeyValue
TupAuxKeyValue::TupAuxKeyValue(void) : m_maxEntryNameLength(0),m_bCaseSensitiveEntryNames(true)
{
}

void TupAuxKeyValue::LoadEntries(TupperwareAggregate *pAgg)
{
	m_uniqueEntryNames.RemoveAll();
	m_entries.RemoveAll();
	for (TupperwareIterator iter = pAgg->FindByKey("Entry"); !iter.Finished(); iter.Next()) 
	{
		// make sure the child is a scalar and has a name
		TupperwareAtom *pEntryAtom = iter.GetAsAtom();
		if (!pEntryAtom || !pEntryAtom->GetName() || pEntryAtom->GetAtomType() != TupperwareAtom::ATOM_SCALAR)
			continue;

		TupperwareScalar *pEntryScalar = iter.GetAsScalar();
		if (pEntryScalar->GetDataType() != TupperwareScalar::DATA_STRING)
			continue;

		const char *pStringData = pEntryScalar->GetAsString();

		TupString entryName = pEntryScalar->GetName();
		TupString entryNameKeeper = GetEntryKeeperName(entryName);
		int oldKeeperSize = m_uniqueEntryNames.GetSize();
		int keeperIndex = m_uniqueEntryNames.Add(entryNameKeeper);
		// we need to add a new entry
		if (keeperIndex==oldKeeperSize)
		{
			Entry newEntry(entryName,pEntryScalar->GetAsString());
			m_entries.Add(newEntry);
		}
		// we have a duplicate (merge keys in this case)
		else
		{
			Entry &entry = m_entries[keeperIndex];
			TupKeyValue keyValue(pEntryScalar->GetAsString());
			entry.m_keyValue.MergeKeyValues(keyValue);
		}
	}
}

void TupAuxKeyValue::SaveEntries(TupperwareAggregate *pAgg)
{
	int numEntries = m_entries.GetSize();
	for (int entryIndex=0;entryIndex<numEntries;entryIndex++)
	{
		Entry &entry = m_entries[entryIndex];
		TupString keyValString = "\n";
		int totalAttribs = entry.m_keyValue.GetNumAttribs();
		for (int attIndex = 0;attIndex<totalAttribs;attIndex++)
		{
			keyValString+="\t";
			keyValString+= entry.m_keyValue.GetKeyAsString(attIndex);
			keyValString+= "=";
			keyValString+= entry.m_keyValue.GetValueAsString(attIndex);
			keyValString+= "\n";
		}
		TupString string = entry.m_keyValue.BuildString(NULL,NULL);
		pAgg->AddScalarString("Entry",entry.m_entryName,keyValString);
	}
}

int TupAuxKeyValue::GetMaxEntryNameLength(void) const
{
	return m_maxEntryNameLength;
}

void TupAuxKeyValue::SetMaxEntryNameLength(int value)
{
	m_maxEntryNameLength = value;
}

bool TupAuxKeyValue::IsEntryNamesCaseSensitive(void) const
{
	return m_bCaseSensitiveEntryNames;
}

void TupAuxKeyValue::EntryNamesCaseSensitive(bool bValue)
{
	m_bCaseSensitiveEntryNames = bValue;
}

bool TupAuxKeyValue::MergeToKeyValues(TupKeyValue &keyValue,const char *pEntryName,bool bReplace)
{
	assert(pEntryName);
	TupString entryName(pEntryName);
	TupString keeperName = GetEntryKeeperName(entryName);
	int keeperIndex = m_uniqueEntryNames.GetIndex(keeperName);
	if (keeperIndex!=-1)
	{
		return keyValue.MergeKeyValues(m_entries[keeperIndex].m_keyValue,bReplace);
	}
	return false;
}

void TupAuxKeyValue::AddFromContext(const TupKeyValue &keyValue,const TupKeyValueContext &keyValueContext,const char *pEntryName)
{
	assert(pEntryName);
	TupString entryName(pEntryName);
	TupString keeperName = GetEntryKeeperName(entryName);
	int oldKeeperSize = m_uniqueEntryNames.GetSize();
	int keeperIndex = m_uniqueEntryNames.Add(keeperName);
	// if not in keeper set one up
	if (keeperIndex==oldKeeperSize)
	{
		Entry newEntry;
		newEntry.m_entryName = pEntryName;
		m_entries.Add(newEntry);
	}

	Entry &entry = m_entries[keeperIndex];

	int numAttribs = keyValueContext.GetNumAttribs();
	for (int attribIndex=0;attribIndex<numAttribs;attribIndex++)
	{
		TupString keyString = keyValueContext.GetKeyAsString(attribIndex);
		TupString valueString = keyValueContext.GetValueAsString(attribIndex);
		// see if this key-value is a default or explicitly set
		bool bDefaultValue = true;
		// see if key is used in the explicitly set key-values
		int keyInKeyValueIndex = keyValue.KeyExistsAt(keyString);
		// if in the list and the value matches
		if (keyInKeyValueIndex!=-1 && !strcmp(keyValue.GetValueAsString(keyInKeyValueIndex),valueString))
		{
			bDefaultValue = false;
		}
		// add keyvalue to entry (replace if explicitly set)
		entry.m_keyValue.AddKeyValue(keyString,valueString,!bDefaultValue);
	}
}

bool TupAuxKeyValue::GetKeyValues(const char *pEntryName,TupKeyValue &keyValue)
{
	TupString entryName(pEntryName);
	TupString keeperName = GetEntryKeeperName(entryName);
	int keeperIndex = m_uniqueEntryNames.GetIndex(keeperName);
	if (keeperIndex!=-1)
	{
		keyValue = m_entries[keeperIndex].m_keyValue;
		return true;
	}
 	return false;
}

TupString TupAuxKeyValue::GetEntryKeeperName(const TupString &name)  const
{
	TupString tempName;
	if (m_maxEntryNameLength)
	{
		tempName = name.Right(m_maxEntryNameLength);
	}
	else
	{
		tempName = name;
	}
	if (m_bCaseSensitiveEntryNames)
	{
		tempName.MakeLower();
	}
	return tempName;
}


