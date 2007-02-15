//-----------------------------------------------------------------------------
//	File:		ApplyColorsCommunicatorBase.cpp
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes:

#include "ApplyColorsPCH.h"
#include "ApplyColorsCommunicatorBase.h"
#include "ApplyColorsPanelController.h"
#include "ApplyColorsRegistry.h"
#include "ColorSetManager.h"

// Registry defines:
#define APPLY_COLORS_COLOR_SET "ColorSet"

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// ApplyColorsCommunicatorBase::ApplyColorsCommunicatorBase
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ApplyColorsCommunicatorBase::ApplyColorsCommunicatorBase()
{
	m_pApplyColorsPanelController = NULL;
	m_pColorSetManager = NULL;
}


//-----------------------------------------------------------------------------
// ApplyColorsCommunicatorBase::~ApplyColorsCommunicatorBase
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ApplyColorsCommunicatorBase::~ApplyColorsCommunicatorBase()
{
	if (m_pApplyColorsPanelController != NULL)
	{
		delete m_pApplyColorsPanelController;
		m_pApplyColorsPanelController = NULL;
		g_pTheApplyColorsPanelController = NULL;
	}
	if (m_pColorSetManager)
	{
		delete m_pColorSetManager;
		m_pColorSetManager = NULL;
	}
}

//-----------------------------------------------------------------------------
// ApplyColorsCommunicatorBase::InitApplyColorsPanel
// This method will tell the ApplyColorsPanelController to initialize the apply colors
// panel.
//
// Input:
//	ICustomRollupWindow* pRollupWindow -- The rollup window to append the panel to.
//  HINSTANCE hInstance -- The HINSTANCE of the plugin to use.
//  LPARAM lParam -- The LPARAM to use.  This should be the parent HWND cast as
//					 an LPARAM.
//	int nCategory -- The category to use for the keyval panel order.
//
// Output: None.
//-----------------------------------------------------------------------------
void ApplyColorsCommunicatorBase::InitApplyColorsPanel(ICustomRollupWindow* pRollupWindow, HINSTANCE hInstance, LPARAM lParam)
{
	m_pApplyColorsPanelController = new ApplyColorsPanelController(this);

	// Set the global:
	g_pTheApplyColorsPanelController = m_pApplyColorsPanelController;

	m_pApplyColorsPanelController->InitApplyColorsPanel(pRollupWindow, hInstance, lParam);
	m_pApplyColorsPanelController->UpdateTreeViewAndCombo();
}


//-----------------------------------------------------------------------------
// ApplyColorsCommunicatorBase::IdCancelMessage
// This method will tell the ApplyColorsPanelController that the IDCANCEL message
// has been received.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ApplyColorsCommunicatorBase::IdCancelMessage()
{
	SaveColorSetInfoToRegistry();
	if (m_pApplyColorsPanelController != NULL)
		m_pApplyColorsPanelController->IdCancelMessage();
}


//-----------------------------------------------------------------------------
// ApplyColorsCommunicatorBase::SaveColorSetInfoToRegistry
// This method save out the registry information for the current color sets
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ApplyColorsCommunicatorBase::SaveColorSetInfoToRegistry(void) const
{
	ApplyColorsRegistry kvReg(m_registryKeyName);
	TupString colorSetName = "null";
	if (m_currentColorSetIndex>=0 && m_pColorSetManager != NULL && m_currentColorSetIndex<m_pColorSetManager->GetNumColorSets())
	{
		colorSetName = m_pColorSetManager->GetColorSet(m_currentColorSetIndex).GetName();
	}
	kvReg.Set(APPLY_COLORS_COLOR_SET, (const char *) colorSetName);
}

//-----------------------------------------------------------------------------
// ApplyColorsCommunicatorBase::LoadColorSetInfoFromRegistry
// This method load the registry information for the current color sets
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ApplyColorsCommunicatorBase::LoadColorSetInfoFromRegistry(void)
{
	m_currentColorSetIndex = -1; // default
	ApplyColorsRegistry kvReg(m_registryKeyName);
	char colorSetName[80];
	if (kvReg.Get(APPLY_COLORS_COLOR_SET, colorSetName,80) == TRUE)
	{
		for (int colorSetIndex = 0;colorSetIndex<m_pColorSetManager->GetNumColorSets();colorSetIndex++)
		{
			if (strcmp(colorSetName,m_pColorSetManager->GetColorSet(colorSetIndex).GetName())==0)
			{
				m_currentColorSetIndex = colorSetIndex;
				break;
			}
		}
	}
	if (m_currentColorSetIndex == -1 && m_pColorSetManager->GetNumColorSets()>0)
	{
		m_currentColorSetIndex = 0; // set to first if there is a set to use
	}
}

//-----------------------------------------------------------------------------
// ApplyColorsCommunicatorBase::SetApplyButtonState
// This method will tell the ApplyColorsPanelController to set the apply button
// states for the key/val panel.
//
// Input:
//  bool bEnabled -- True if the button is to be enabled.
//
// Output: None.
//-----------------------------------------------------------------------------
void ApplyColorsCommunicatorBase::SetApplyButtonState(bool bEnabled)
{
	if (m_pApplyColorsPanelController != NULL)
		m_pApplyColorsPanelController->SetApplyButtonState(bEnabled);
}

//-----------------------------------------------------------------------------
// ApplyColorsCommunicatorBase::SetRemoveColorButtonState
// This method will tell the ApplyColorsPanelController to set the remove colors button
// states for the key/val panel.
//
// Input:
//  bool bEnabled -- True if the button is to be enabled.
//
// Output: None.
//-----------------------------------------------------------------------------
void ApplyColorsCommunicatorBase::SetRemoveColorButtonState(bool bEnabled)
{
	if (m_pApplyColorsPanelController != NULL)
		m_pApplyColorsPanelController->SetRemoveColorButtonState(bEnabled);
}

void InitializeColorSets(TupperwareAggregate *pRootAgg);

//-----------------------------------------------------------------------------
// ApplyColorsCommunicatorBase::InitializeColorSets
// This method will tell the ApplyColorsPanelController to load the key/vals in the
// key/val panel's tree.
//
// Input: const char *pRegistryKeyName -- registry path name to use for this set
//			 TupperwareAggregate *pRootAgg -- tup tree to parse for building color sets
//
// Output: None.
//-----------------------------------------------------------------------------
void ApplyColorsCommunicatorBase::InitializeColorSets(const char *pRegistryKeyName,TupperwareAggregate *pRootAgg)
{
	if (m_registryKeyName.GetLength() && m_pColorSetManager!=NULL)
	{
		SaveColorSetInfoToRegistry();
	}

	if (m_pColorSetManager==NULL)
	{
		m_pColorSetManager = new ColorSetManager;
	}

	m_pColorSetManager->Init(pRootAgg);
	m_registryKeyName = pRegistryKeyName;
	LoadColorSetInfoFromRegistry();
	if (m_pApplyColorsPanelController != NULL)
		m_pApplyColorsPanelController->UpdateTreeViewAndCombo();
}

//-----------------------------------------------------------------------------
// ApplyColorsCommunicatorBase::GetColorSetManager
// This method return the color set manager
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
ColorSetManager *ApplyColorsCommunicatorBase::GetColorSetManager(void)
{
	return m_pColorSetManager;
}
