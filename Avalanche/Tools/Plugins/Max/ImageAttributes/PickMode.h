//-----------------------------------------------------------------------------
//	File:		PickMode.h
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


#ifndef __PICKMODE__
#define __PICKMODE__



//-----------------------------------------------------------------------------
// Pre-declarations, etc.:



//-----------------------------------------------------------------------------
// Defines, enums, etc.:

enum PickButtonEnum
{
	BUTTON_ONE_TO_MANY,
	BUTTON_ONE_TO_MANY_REPLACE,
};



//-----------------------------------------------------------------------------
// ImgAttrPickNodeCallback - Class Declaration:

class ImgAttrPickNodeCallback : public PickNodeCallback 
{
public:		
	BOOL Filter(INode* node);
};



//-----------------------------------------------------------------------------
// ImgAttrPickModeCallback - Class Declaration:

class ImgAttrPickModeCallback : public PickModeCallback 
{
public:		
	ImgAttrPickModeCallback();
	~ImgAttrPickModeCallback();

	BOOL HitTest(IObjParam* ip, HWND hWnd, ViewExp* vpt, IPoint2 m, int flags);
	BOOL Pick(IObjParam* ip, ViewExp* vpt);
	
	void EnterMode(IObjParam* ip);
	void ExitMode(IObjParam* ip);

	PickNodeCallback* GetFilter();
	BOOL RightClick(IObjParam* ip, ViewExp* vpt);

	int m_buttonIndex;
};



//-----------------------------------------------------------------------------
// Expose the global objects - I know - Yuck!

extern ImgAttrPickNodeCallback thePickFilter;
extern ImgAttrPickModeCallback thePickMode;



#endif