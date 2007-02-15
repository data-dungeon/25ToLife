//-----------------------------------------------------------------------------
//	File:		ApplyColorsCommunicator.cpp
//	Created:	November 18th, 2004
//				Copyright (c) 2002-2004, Avalanche Software Inc.
//				All Rights Reserved.
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "StdAfx.h"
#include "ApplyColorsCommunicator.h"
#include "FaceAttrController.h"
#include "CurrAttribController.h"
#include "CurrMeshAttribData.h"

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// ApplyColorsCommunicator::ApplyColorsCommunicator
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
ApplyColorsCommunicator::ApplyColorsCommunicator(FaceAttrController* pFaceAttrController)
{
	m_pFaceAttrController = pFaceAttrController;
}


//-----------------------------------------------------------------------------
// ApplyColorsCommunicator::~ApplyColorsCommunicator
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ApplyColorsCommunicator::~ApplyColorsCommunicator()
{
}

void ApplyColorsCommunicator::HandleApplyColor(void)
{
	if (m_pFaceAttrController != NULL)
	{
		// Tell the FaceAttrController to get the current attribute data:
		CurrAttribController* pCurrAttribController = m_pFaceAttrController->GetCurrAttribController();
		if (pCurrAttribController != NULL)
		{
			// Cycle through all the Maya objects, and through the selected faces of each, getting each key/val pair:
			int nNumMeshes = pCurrAttribController->GetSize();
			if (nNumMeshes==1)
			{
				CurrMeshAttribData* pCurrMeshAttribData = pCurrAttribController->GetCurrMeshAttribData(0);
				ColorSetManager *pColorSetManager = GetColorSetManager();
				int currentColorSetIndex = GetCurrentColorSetIndex();
				if (currentColorSetIndex >=0 && pColorSetManager && currentColorSetIndex<pColorSetManager->GetNumColorSets())
				{
					MString command = "polyColorBlindData -ncr 0 -ncg 0 -ncb 0 -ccr 0 -ccg 1 -ccb 1 ";
					const ColorSet &colorSet = pColorSetManager->GetColorSet(currentColorSetIndex);
					int numSets = pCurrMeshAttribData->GetNumAttribSets();
					for (int setIndex=0;setIndex<numSets;setIndex++)
					{
						const CurrAttribData &currAttribData = pCurrMeshAttribData->GetAttribSet(setIndex);
						int nNumAttribs = currAttribData.GetNumAttribs();
						TupArray<TupString> keyArray;
						TupArray<TupString> valueArray;
						for (int attribIndex = 0; attribIndex < nNumAttribs; attribIndex++)
						{
							TupString keyName(currAttribData.GetKeyAsString(attribIndex));
							TupString valueName(currAttribData.GetValueAsString(attribIndex));
							keyArray.Add(keyName);
							valueArray.Add(valueName);
						}
						ColorSet::ApplyColorValue colorValue = colorSet.GetColor(keyArray,valueArray);
						TupString commandToAdd;
						commandToAdd.Format("-id %d -num 1 -m 1 -dt \"%s\" -n \"%s\" -v \"%d\" -cr %f -cg %f -cb %f ",
							BLIND_DATA_ID,BLIND_DATA_FORMAT_NAME,BLIND_DATA_LONG_NAME,
							setIndex,colorValue.m_red,colorValue.m_green,colorValue.m_blue);
						command += MString(commandToAdd);
					}
					MDGModifier dgModifier;
					MGlobal::executeCommand(command);
					dgModifier.doIt();
					MGlobal::executeCommand("dgdirty -a");
					dgModifier.doIt();
				}
			}
		}
	}
}

void ApplyColorsCommunicator::HandleRemoveColor(void)
{
	MDGModifier dgModifier;
	MGlobal::executeCommand("polyColorBlindData -efc 0");
	dgModifier.doIt();
}

