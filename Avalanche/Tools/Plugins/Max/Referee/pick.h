////////////////////////////////////////////////////////////////////////////
//
// Pick
//
// handle picking of nodes in scene
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: pick.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/28/01   Time: 3:25p
 * Updated in $/Avalanche/tools/Plugins/Max/Referee
 * Collapse to Base
 * Collapse one level
 * Link to picked object
 * Link to picked base
 * Add a level
 */

#ifndef __PICK__H
#define __PICK__H

class RefereePickNodeCallback : public PickNodeCallback 
{
public:		
	BOOL Filter(INode *node);
};


class RefereePickModeCallback : public PickModeCallback 
{
public:		
	int m_buttonIndex;

	RefereePickModeCallback(void);

	BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);
	BOOL Pick(IObjParam *ip,ViewExp *vpt);
	
	void EnterMode(IObjParam *ip);
	void ExitMode(IObjParam *ip);

	PickNodeCallback *GetFilter();
	BOOL RightClick(IObjParam *ip,ViewExp *vpt);
};

enum PickButtonEnum
{
	LINKED_PICKED_OBJECT,
	LINKED_PICKED_BASE,
};

extern RefereePickNodeCallback thePickFilter;
extern RefereePickModeCallback thePickMode;



#endif