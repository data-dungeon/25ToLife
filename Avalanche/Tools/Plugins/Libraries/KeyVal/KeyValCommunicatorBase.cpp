//-----------------------------------------------------------------------------
//	File:		KeyValCommunicatorBase.cpp
//	Created:	May 1, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The KeyValCommunicatorBase class acts as a base class for classes
//			that reside in seperate plugins that provide communication between
//			the plugin and the shared library.  The types of communication
//			necessary would be things such as redrawing the dynamic UI, purging
//			keys, remaping keys or values, etc.
//
//	History:	5/01/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "KeyValPCH.h"
#include "KeyValCommunicatorBase.h"
#include "KeyValPanelController.h"

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// KeyValCommunicatorBase::KeyValCommunicatorBase
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
KeyValCommunicatorBase::KeyValCommunicatorBase()
{
	m_pKeyValPanelController = NULL;
	m_pRegKeyName = NULL;
}


//-----------------------------------------------------------------------------
// KeyValCommunicatorBase::~KeyValCommunicatorBase
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
KeyValCommunicatorBase::~KeyValCommunicatorBase()
{
	if (m_pKeyValPanelController != NULL)
	{
		delete m_pKeyValPanelController;
		m_pKeyValPanelController = NULL;
		g_pTheKeyValPanelController = NULL;
	}
}


//-----------------------------------------------------------------------------
// KeyValCommunicatorBase::GetRegKeyName
// This method will return the m_pRegKeyName member data.
//
// Input: None
//
// Output: Pointer to a char representing the registry keyname to use. 
//-----------------------------------------------------------------------------
char* KeyValCommunicatorBase::GetRegKeyName()
{
	return m_pRegKeyName;
}


//-----------------------------------------------------------------------------
// KeyValCommunicatorBase::InitKeyValPanel
// This method will tell the keyvalpanelcontroller to initialize the keyval
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
void KeyValCommunicatorBase::InitKeyValPanel(ICustomRollupWindow* pRollupWindow, HINSTANCE hInstance, LPARAM lParam)
{
	m_pKeyValPanelController = new KeyValPanelController(this);

	// Set the global:
	g_pTheKeyValPanelController = m_pKeyValPanelController;

	m_pKeyValPanelController->InitKeyValPanel(pRollupWindow, hInstance, lParam);
}


//-----------------------------------------------------------------------------
// KeyValCommunicatorBase::IdCancelMessage
// This method will tell the keyvalpanelcontroller that the IDCANCEL message
// has been received.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValCommunicatorBase::IdCancelMessage()
{
	if (m_pKeyValPanelController != NULL)
		m_pKeyValPanelController->IdCancelMessage();
}


//-----------------------------------------------------------------------------
// KeyValCommunicatorBase::SetApplyBttnStates
// This method will tell the KeyValPanelController to set the apply button
// states for the key/val panel.
//
// Input:
//  bool bApplySelEnabled -- True if the Apply To Sel button is to be enabled.
//  bool bApplySceneEnabled -- True if the Apply To Scene button is to be
//							   enabled.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValCommunicatorBase::SetApplyBttnStates(bool bApplySelEnabled, bool bApplySceneEnabled)
{
	if (m_pKeyValPanelController != NULL)
		m_pKeyValPanelController->SetApplyBttnStates(bApplySelEnabled, bApplySceneEnabled);	
}


//-----------------------------------------------------------------------------
// KeyValCommunicatorBase::LoadKeyValPanelTree
// This method will tell the KeyValPanelController to load the key/vals in the
// key/val panel's tree.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValCommunicatorBase::LoadKeyValPanelTree()
{
	if (m_pKeyValPanelController != NULL)
		m_pKeyValPanelController->UpdateTreeViewAndKeeper();
}

//-----------------------------------------------------------------------------
// KeyValCommunicatorBase::ShowFindDialog
// This method will bring up the find dialog and set the key and value
//
// Input: const char *pKeyName
//			 const char *pValueName
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValCommunicatorBase::ShowFindDialog(const char *pKeyName, const char *pValueName)
{
	if (m_pKeyValPanelController != NULL)
		m_pKeyValPanelController->ShowFindDialog(pKeyName,pValueName);
}

//-----------------------------------------------------------------------------
// KeyValCommunicatorBase::ShowFindDialog
// This method will bring up the find dialog and set the key and value
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void KeyValCommunicatorBase::HandleFindButton(void)
{
	if (m_pKeyValPanelController != NULL)
		m_pKeyValPanelController->HandleFindButton();
}
