//-----------------------------------------------------------------------------
//	File:		PickMode.cpp
//	Created:	Mar. 12, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	This file contains the classes needed to handle the pick-mode
//			functionality for this plug-in.  The pick-mode functionality
//			allows the user to handle the picking of a node in the scene.
//
//	History:	3/12/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "ImageAttributesPCH.h"

#ifndef __IMAGEATTRIBUTES__H
#include "ImageAttributes.h"
#endif

#ifndef __PICKMODE__
#include "PickMode.h"
#endif



//-----------------------------------------------------------------------------
// Defines, globals, etc.:

ImgAttrPickNodeCallback thePickFilter;
ImgAttrPickModeCallback thePickMode;



//-----------------------------------------------------------------------------
// ImgAttrPickNodeCallback Class Definition:


//-----------------------------------------------------------------------------
// ImgAttrPickNodeCallback::Filter
// This method acts as a filter when picking objects in the world.  Using it,
// you can specify that things such as lights, cameras, etc. get filtered out.
// For now, we will allow all nodes to pass the filter.
//
// Input:
//  INode* node -- The node to inspect.
//
// Output: TRUE if the object passes the filter, FALSE otherwise.
//-----------------------------------------------------------------------------
BOOL ImgAttrPickNodeCallback::Filter(INode* node)
{
	return TRUE;
}



//-----------------------------------------------------------------------------
// ImgAttrPickModeCallback Class Definition:


//-----------------------------------------------------------------------------
// ImgAttrPickModeCallback::ImgAttrPickModeCallback
// This method is the constructor for this class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
ImgAttrPickModeCallback::ImgAttrPickModeCallback()
{
	m_buttonIndex = BUTTON_ONE_TO_MANY;
}


//-----------------------------------------------------------------------------
// ImgAttrPickModeCallback::~ImgAttrPickModeCallback
// This method is the default constructor for this class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
ImgAttrPickModeCallback::~ImgAttrPickModeCallback()
{
}


//-----------------------------------------------------------------------------
// ImgAttrPickModeCallback::HitTest
// This method is called by Max whenever the pick mode needs to do a hit test.
//
// Input:
//  IObjParam* ip -- An interface pointer available to call functions defined
//					 by Max.
//  HWND hWnd -- The window handle.
//  ViewExp* vpt -- An interface pointer that may be used to call methods
//					associated with the viewports.
//  IPoint2 m -- Point to check in screen coordinates.
//  int flags -- The flags for hit testing.
//
// Output: TRUE if something was hit, FALSE otherwise.
//-----------------------------------------------------------------------------
BOOL ImgAttrPickModeCallback::HitTest(IObjParam* ip, HWND hWnd, ViewExp* vpt, IPoint2 m, int flags)
{
	return ip->PickNode(hWnd, m, &thePickFilter) ? TRUE : FALSE;
}


//-----------------------------------------------------------------------------
// ImgAttrPickModeCallback::Pick
// This method is called by Max when the user picks something.
//
// Input:
//  IObjParam* ip -- An interface pointer available to call functions defined
//					 by Max.
//  ViewExp* vpt -- An interface pointer that may be used to call methods
//					associated with the viewports.  The vpt should have the
//					result of the hit test in it.
//
// Output: TRUE to end the pick mode, FALSE to stay in the pick mode. 
//-----------------------------------------------------------------------------
BOOL ImgAttrPickModeCallback::Pick(IObjParam* ip, ViewExp* vpt)
{
	BOOL bEndPickMode = TRUE;

	INode* pNode = vpt->GetClosestHit();
	if (pNode != NULL)
	{
		switch (m_buttonIndex)
		{
			case BUTTON_ONE_TO_MANY:
			{
//				if (theImageAttributes.CopyAttribsToSelSet(pNode) == false)
					bEndPickMode = FALSE;
				break;
			}
			case BUTTON_ONE_TO_MANY_REPLACE:
			{
//				if (theImageAttributes.CopyAttribsToSelSet(pNode, true) == false)
					bEndPickMode = FALSE;
				break;
			}
		}
	}

	return bEndPickMode;
}


//-----------------------------------------------------------------------------
// ImgAttrPickModeCallback::EnterMode
// This method is called by Max when the pick mode is entered.
//
// Input:
//  IObjParam* ip -- An interface pointer available to call functions defined
//					 by Max.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImgAttrPickModeCallback::EnterMode(IObjParam* ip)
{
	switch (m_buttonIndex)
	{
		case BUTTON_ONE_TO_MANY:
		{
			theImageAttributes.m_pOneToManyButton->SetCheck(TRUE);
			break;
		}
		case BUTTON_ONE_TO_MANY_REPLACE:
		{
			theImageAttributes.m_pOneToManyButtonReplace->SetCheck(TRUE);
			break;
		}
	}
}


//-----------------------------------------------------------------------------
// ImgAttrPickModeCallback::ExitMode
// This method is called by Max when the pick mode is exited from.
//
// Input:
//  IObjParam* ip -- An interface pointer available to call functions defined
//					 by Max.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImgAttrPickModeCallback::ExitMode(IObjParam* ip)
{
	switch (m_buttonIndex)
	{
		case BUTTON_ONE_TO_MANY:
		{
			if (theImageAttributes.bDialogUp == true)
			{
				theImageAttributes.m_pOneToManyButton->SetCheck(FALSE);
			}
			break;
		}
		case BUTTON_ONE_TO_MANY_REPLACE:
		{
			if (theImageAttributes.bDialogUp == true)
			{
				theImageAttributes.m_pOneToManyButtonReplace->SetCheck(FALSE);
			}
			break;
		}
	}
}


//-----------------------------------------------------------------------------
// ImgAttrPickModeCallback::GetFilter
// This method is called to get the appropriate filter object.
//
// Input: None.
//
// Output: PickNodeCallback* representing the filter.
//-----------------------------------------------------------------------------
PickNodeCallback* ImgAttrPickModeCallback::GetFilter()
{
	return &thePickFilter;
}


//-----------------------------------------------------------------------------
// ImgAttrPickModeCallback::RightClick
// This method is called by Max when the user right-clicks or presses ESC.
//
// Input:
//  IObjParam* ip -- An interface pointer available to call functions defined
//					 by Max.
//  ViewExp* vpt -- An interface pointer that may be used to call methods
//					associated with the viewports.  The vpt should have the
//					result of the hit test in it.
//
// Output: TRUE to end the pick mode, FALSE to stay in the pick mode. 
//-----------------------------------------------------------------------------
BOOL ImgAttrPickModeCallback::RightClick(IObjParam* ip, ViewExp* vpt) 
{
	return TRUE;
}
