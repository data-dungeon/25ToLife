//-----------------------------------------------------------------------------
//	File:		KeyValCommunicator.cpp
//	Created:	May 1, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The KeyValCommunicator class derives from KeyValCommunicatorBase
//			class in the KeyVal shared library.  This class will handle all
//			communication between the plugin and the shared library.  The types
//			of communication necessary would be things such as redrawing the
//			dynamic UI, purging keys, remaping keys or values, etc.
//
//	History:	5/01/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "StdAfx.h"
#include "KeyValCommunicator.h"
#include "MotionAttrController.h"
#include "CurrAttribController.h"
#include "CurrAttribData.h"

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// KeyValCommunicator::KeyValCommunicator
// This method is the default constructor for this class.
//
// Input:
//	MotionAttrController* pMotionAttrController -- Pointer back to the object
//											 attribute controller.
//	char* pRegKeyName -- The registry key name to use to store data for the
//						 keyval panel.
//
// Output: None
//-----------------------------------------------------------------------------
KeyValCommunicator::KeyValCommunicator(MotionAttrController* pMotionAttrController, char* pRegKeyName)
{
	m_pMotionAttrController = pMotionAttrController;
	m_pRegKeyName = pRegKeyName;
}


//-----------------------------------------------------------------------------
// KeyValCommunicator::~KeyValCommunicator
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
KeyValCommunicator::~KeyValCommunicator()
{
}


//-----------------------------------------------------------------------------
// KeyValCommunicator::LoadKeyValPanelData
// This method will take the incoming keeper structure, empty it out, and load
// it up with the current attribute data.
//
// Input:
//  Keeper<KeyInfo>& keyKeeper -- The keeper structure to load up.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValCommunicator::LoadKeyValPanelData(Keeper<KeyInfo>& keyKeeper)
{
	// Remove any previous items in the keeper:
	keyKeeper.RemoveAll();

	if (m_pMotionAttrController != NULL)
	{
		// Tell the MotionAttrController to get the current attribute data:
		CurrAttribController* pCurrAttribController = m_pMotionAttrController->GetCurrAttribController();
		if (pCurrAttribController != NULL)
		{
			ConfigRoot *pConfigRoot = m_pMotionAttrController->GetConfigRoot();
			// Cycle through all the objects, getting each key/val pair:
			int nNumObjects = pCurrAttribController->GetSize();
			for (int nIndex = 0; nIndex < nNumObjects; nIndex++)
			{
				CurrAttribData* pCurrAttribData = pCurrAttribController->GetCurrAttribData(nIndex);
				AddKeyInfoData(pCurrAttribController,pCurrAttribData,keyKeeper);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// KeyValCommunicator::AddKeyInfoData
// This method will add the key/value data for the current object to the key keeper
//
// Input:
//  Keeper<KeyInfo>& keyKeeper -- The keeper structure to load up.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValCommunicator::AddKeyInfoData(CurrAttribController *pCurrAttribController,CurrAttribData* pCurrAttribData,Keeper<KeyInfo>& keyKeeper)
{
	ConfigRoot *pConfigRoot = m_pMotionAttrController->GetConfigRoot();

	KeyValueContext keyValueContext;
	KeyValue keyValue(pCurrAttribData->GetKeyValuePairs());
	pConfigRoot->GetKeyValueContexts(pCurrAttribController,keyValue,keyValueContext,true);

	int numMountedKeys = keyValue.GetNumAttribs();
	int numContextKeys = keyValueContext.GetNumAttribs();

	TupArray<bool> usedMountedKeys;
	usedMountedKeys.SetSize(numMountedKeys);

	int keyIndex;

	// keep track of which mounted keys were used in the context
	for (keyIndex = 0; keyIndex < numMountedKeys; keyIndex++)
	{
		usedMountedKeys[keyIndex] = false;
	}

	for (int contextKeyIndex = 0; contextKeyIndex < numContextKeys; contextKeyIndex++)
	{
		// Create a new KeyInfo to use:
		KeyInfo newKeyInfo;
		newKeyInfo.m_strKeyname = keyValueContext.GetKeyAsString(contextKeyIndex);
		int nKIIndex = keyKeeper.Add(newKeyInfo);

		const ConfigBase *pConfigBase = keyValueContext.GetControlPtr(contextKeyIndex);
		if (pConfigBase)
		{
			if (pConfigBase->GetControlType() == ConfigBase::ControlType::CHECK_LIST)
			{
				TupString contextValueString = keyValueContext.GetValueAsString(contextKeyIndex);
				Keeper<TupString> contextSubValues;
				m_pMotionAttrController->GetCheckListBoxValues(contextValueString,contextSubValues);
				int subIndex;
				for (subIndex = 0; subIndex < contextSubValues.GetSize(); subIndex++)
				{
					ValInfo newValInfo;
					newValInfo.m_strVal = contextSubValues[subIndex];
					int valInfoIndex = keyKeeper[nKIIndex].m_valKeeper.Add(newValInfo);
					keyKeeper[nKIIndex].m_valKeeper[valInfoIndex].m_activeCount++;
					keyKeeper[nKIIndex].m_activeCount++; // active key
				}
				int mountedKeyValueIndex = keyValueContext.GetKeyValueIndex(contextKeyIndex);
				if (mountedKeyValueIndex != -1)
				{
					usedMountedKeys[mountedKeyValueIndex] = true;
					TupString mountedValueString = keyValue.GetValueAsString(mountedKeyValueIndex);
					Keeper<TupString> mountedSubValues;
					m_pMotionAttrController->GetCheckListBoxValues(mountedValueString,mountedSubValues);

					for (subIndex = 0; subIndex < mountedSubValues.GetSize(); subIndex++)
					{
						// any value that was not part of the context is added as an unused value
						if (contextSubValues.GetIndex(mountedSubValues[subIndex])==-1)
						{
							ValInfo newValInfo;
							newValInfo.m_strVal = mountedSubValues[subIndex];
							int valInfoIndex = keyKeeper[nKIIndex].m_valKeeper.Add(newValInfo);
							keyKeeper[nKIIndex].m_valKeeper[valInfoIndex].m_unusedCount++;
							keyKeeper[nKIIndex].m_unusedCount++; // unused key
						}
					}
				}
			}
			else
			{
				// Create a new ValInfo to use:
				ValInfo newValInfo;
				newValInfo.m_strVal = keyValueContext.GetValueAsString(contextKeyIndex);
				int valInfoIndex = keyKeeper[nKIIndex].m_valKeeper.Add(newValInfo);
				int mountedKeyValueIndex = keyValueContext.GetKeyValueIndex(contextKeyIndex);
				if (mountedKeyValueIndex != -1)
				{
					usedMountedKeys[mountedKeyValueIndex] = true;
					if (keyValueContext.IsDefaultValue(contextKeyIndex))
					{
						keyKeeper[nKIIndex].m_defaultCount++; // default key
						keyKeeper[nKIIndex].m_valKeeper[valInfoIndex].m_defaultCount++;
					}
					else
					{
						keyKeeper[nKIIndex].m_activeCount++; // active key
						keyKeeper[nKIIndex].m_valKeeper[valInfoIndex].m_activeCount++;
					}
				}
				else
				{
					keyKeeper[nKIIndex].m_defaultCount++; // default key
					keyKeeper[nKIIndex].m_valKeeper[valInfoIndex].m_defaultCount++;
				}
			}
		}
	}

	// any unused keys are also added
	for (keyIndex = 0; keyIndex < numMountedKeys; keyIndex++)
	{
		if (!usedMountedKeys[keyIndex])
		{
			KeyInfo newKeyInfo;
			newKeyInfo.m_strKeyname = keyValue.GetKeyAsString(keyIndex);
			int nKIIndex = keyKeeper.Add(newKeyInfo);

			if (m_pMotionAttrController->IsCheckListKey(newKeyInfo.m_strKeyname))
			{
				TupString valueString = keyValue.GetValueAsString(keyIndex);
				Keeper<TupString> subValues;
				m_pMotionAttrController->GetCheckListBoxValues(valueString,subValues);
				for (int subIndex = 0; subIndex < subValues.GetSize(); subIndex++)
				{
					ValInfo newValInfo;
					newValInfo.m_strVal = subValues[subIndex];
					int valInfoIndex = keyKeeper[nKIIndex].m_valKeeper.Add(newValInfo);
					keyKeeper[nKIIndex].m_valKeeper[valInfoIndex].m_unusedCount++;
					keyKeeper[nKIIndex].m_unusedCount++; // unused key
				}
			}
			else
			{
				// Create a new ValInfo to use:
				ValInfo newValInfo;
				newValInfo.m_strVal = keyValue.GetValueAsString(keyIndex);
				int valInfoIndex = keyKeeper[nKIIndex].m_valKeeper.Add(newValInfo);
				keyKeeper[nKIIndex].m_valKeeper[valInfoIndex].m_unusedCount++;
				keyKeeper[nKIIndex].m_unusedCount++; // unused key
			}
		}
	}
}

//-----------------------------------------------------------------------------
// KeyValCommunicator::HandleApplySelect
// This method will be called to select nodes based on the keys and vals
// checked in the incoming keykeeper.  The boolean passed in specifies whether
// to select nodes from the selection set, or the whole scene.
//
// Input:
//  Keeper<KeyInfo>& keyKeeper -- The keeper structure that contains the keys
//								  and vals that are checked.
//	bool bApplyToAll -- Pass in false to select nodes from the selection set
//						only.  Pass in true to select nodes from the whole
//						scene.
//
// Output: None.
//-----------------------------------------------------------------------------
int KeyValCommunicator::HandleApplySelect(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	int totalSelected = 0;
	// Tell the MotionAttrController to handle this:
	if (m_pMotionAttrController != NULL)
		totalSelected = m_pMotionAttrController->HandleApplySelect(keyKeeper, bApplyToAll);
	return totalSelected;
}

//-----------------------------------------------------------------------------
// KeyValCommunicator::HandleApplyApply
// This method will be called to apply specific vals for the keys selected in
// the incoming keeper, to nodes in either the scene or selection set,
// depending on the boolean passed in.
//
// Input:
//  Keeper<KeyInfo>& keyKeeper -- The keeper structure that contains the keys
//								  and vals that are checked.
//	bool bApplyToAll -- Pass in false to apply to nodes from the selection set
//						only.  Pass in true to apply to nodes from the whole
//						scene.
//
// Output: int -- total number applied nodes
//-----------------------------------------------------------------------------
int KeyValCommunicator::HandleApplyApply(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	int totalApplied = 0;
	// Tell the MotionAttrController to handle this:
	if (m_pMotionAttrController != NULL)
		totalApplied = m_pMotionAttrController->HandleApplyApply(keyKeeper, bApplyToAll);
	return totalApplied;
}


//-----------------------------------------------------------------------------
// KeyValCommunicator::HandleApplyPurge
// This method will be called to purge keys and vals (specified in the incoming
// keykeeper) from nodes in either the selection set, or from the whole scene.
//
// Input:
//  Keeper<KeyInfo>& keyKeeper -- The keeper structure that contains the keys
//								  and vals that are marked for purging.
//	bool bApplyToAll -- Pass in false to apply to nodes from the selection set
//						only.  Pass in true to apply to nodes from the whole
//						scene.
//
// Output: int -- total number purged nodes
//-----------------------------------------------------------------------------
int KeyValCommunicator::HandleApplyPurge(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	int totalPurged = 0;
	// Tell the MotionAttrController to handle this:
	if (m_pMotionAttrController != NULL)
		totalPurged = m_pMotionAttrController->HandleApplyPurge(keyKeeper, bApplyToAll);
	return totalPurged;
}


//-----------------------------------------------------------------------------
// KeyValCommunicator::HandleApplyRemap
// This method will be called to remap keys and vals (specified in the incoming
// keykeeper) within nodes in either the selection set, or from the whole scene.
//
// Input:
//  Keeper<KeyInfo>& keyKeeper -- The keeper structure that contains the keys
//								  and vals that are marked for remapping.
//	bool bApplyToAll -- Pass in false to apply to nodes from the selection set
//						only.  Pass in true to apply to nodes from the whole
//						scene.
//
// Output: int -- total number remapped nodes
//-----------------------------------------------------------------------------
int KeyValCommunicator::HandleApplyRemap(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	int totalRemapped = 0;
	// Tell the MotionAttrController to handle this:
	if (m_pMotionAttrController != NULL)
		totalRemapped = m_pMotionAttrController->HandleApplyRemap(keyKeeper, bApplyToAll);
	return totalRemapped;
}

//-----------------------------------------------------------------------------
// KeyValCommunicator::HandleFind
// This method will be select nodes that match the parameters
//
// Input:
// const KeyValueFind & -- criteria to search for
// bool bCurrentSelection -- true = search through selection false = search entire scene
// bool bAppendSelection -- if true found items are appended to current selection
//
// Output: int -- total number found nodes
//-----------------------------------------------------------------------------
int KeyValCommunicator::HandleFind(const KeyValueFind &keyValueFind, bool bCurrentSelection, bool bAppendSelection)
{
	int totalFound = 0;
	if (m_pMotionAttrController != NULL)
		totalFound = m_pMotionAttrController->HandleFind(keyValueFind,bCurrentSelection,bAppendSelection);
	return totalFound;
}


bool KeyValCommunicator::IsCheckListKey(const TupString &keyString)
{
	if (m_pMotionAttrController != NULL)
		return m_pMotionAttrController->IsCheckListKey(keyString);
	return false;
}

//------------------------------------------------------------------------
// KeyValCommunicator::UpdateStatsText
// This method get the sub values from a check list box value
//
// Input:
//  const TupString &statsLine -- text to show
//
// Output: none
//------------------------------------------------------------------------
void KeyValCommunicator::UpdateStatsText(const TupString &statsLine)
{
	if (m_pMotionAttrController != NULL)
		return m_pMotionAttrController->UpdateStatsText(statsLine);

}
