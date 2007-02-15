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
#include "FaceAttrController.h"
#include "CurrAttribController.h"
#include "CurrMeshAttribData.h"

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// KeyValCommunicator::KeyValCommunicator
// This method is the default constructor for this class.
//
// Input:
//	FaceAttrController* pFaceAttrController -- Pointer back to the object
//											 attribute controller.
//	char* pRegKeyName -- The registry key name to use to store data for the
//						 keyval panel.
//
// Output: None
//-----------------------------------------------------------------------------
KeyValCommunicator::KeyValCommunicator(FaceAttrController* pFaceAttrController, char* pRegKeyName)
{
	m_pFaceAttrController = pFaceAttrController;
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

	if (m_pFaceAttrController != NULL)
	{
		// Tell the FaceAttrController to get the current attribute data:
		CurrAttribController* pCurrAttribController = m_pFaceAttrController->GetCurrAttribController();
		if (pCurrAttribController != NULL)
		{
			// Cycle through all the Maya objects, and through the selected faces of each, getting each key/val pair:
			int nNumMeshes = pCurrAttribController->GetSize();
			for (int nIndex = 0; nIndex < nNumMeshes; nIndex++)
			{
				CurrMeshAttribData* pCurrMeshAttribData = pCurrAttribController->GetCurrMeshAttribData(nIndex);
				int numSets = pCurrMeshAttribData->GetNumAttribSets();
				for (int setIndex=0;setIndex<numSets;setIndex++)
				{
					// is any selected faces using this set?
					if (pCurrMeshAttribData->SelectedFacesUsingSet(setIndex))
					{
						const CurrAttribData &currAttribData = pCurrMeshAttribData->GetAttribSet(setIndex);
						int nNumAttribs = currAttribData.GetNumAttribs();
						for (int attribIndex = 0; attribIndex < nNumAttribs; attribIndex++)
						{
							// Create a new KeyInfo to use:
							KeyInfo newKeyInfo;
							newKeyInfo.m_strKeyname = currAttribData.GetKeyAsString(attribIndex);
							int nKIIndex = keyKeeper.Add(newKeyInfo);
							// Create a new ValInfo to use:
							ValInfo newValInfo;
							newValInfo.m_strVal = currAttribData.GetValueAsString(attribIndex);
							keyKeeper[nKIIndex].m_valKeeper.Add(newValInfo);
						}
					}
				}
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
void KeyValCommunicator::HandleApplySelect(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	// Tell the FaceAttrController to handle this:
	if (m_pFaceAttrController != NULL)
		m_pFaceAttrController->HandleApplySelect(keyKeeper, bApplyToAll);
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
// Output: None.
//-----------------------------------------------------------------------------
void KeyValCommunicator::HandleApplyApply(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	// Tell the FaceAttrController to handle this:
	if (m_pFaceAttrController != NULL)
		m_pFaceAttrController->HandleApplyApply(keyKeeper, bApplyToAll);
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
// Output: None.
//-----------------------------------------------------------------------------
void KeyValCommunicator::HandleApplyPurge(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	// Tell the FaceAttrController to handle this:
	if (m_pFaceAttrController != NULL)
		m_pFaceAttrController->HandleApplyPurge(keyKeeper, bApplyToAll);
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
// Output: None.
//-----------------------------------------------------------------------------
void KeyValCommunicator::HandleApplyRemap(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	// Tell the FaceAttrController to handle this:
	if (m_pFaceAttrController != NULL)
		m_pFaceAttrController->HandleApplyRemap(keyKeeper, bApplyToAll);
}
