////////////////////////////////////////////////////////////////////////////
//
// Pick
//
// handle picking of nodes in scene
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: pick.cpp $
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

#include "Referee.h"
#include "pick.h"

RefereePickNodeCallback thePickFilter;
RefereePickModeCallback thePickMode;

BOOL RefereePickNodeCallback::Filter(INode *node)
{
	ObjectState os = node->EvalWorldState(theReferee.m_pInterface->GetTime());
	if ((os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID &&
		os.obj->IsRenderable()) || os.obj->SuperClassID()==SHAPE_CLASS_ID) return TRUE;
	else return FALSE;
}

RefereePickModeCallback::RefereePickModeCallback(void)
{
	m_buttonIndex = 0;
}


BOOL RefereePickModeCallback::HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags)
{
	return ip->PickNode(hWnd,m,&thePickFilter)?TRUE:FALSE;
}

BOOL RefereePickModeCallback::Pick(IObjParam *ip,ViewExp *vpt)
{
	INode *node = vpt->GetClosestHit();
	if (node) 
	{
		switch (m_buttonIndex)
		{
			case LINKED_PICKED_OBJECT:
				theReferee.LinkToPickedObject(node);
				break;
			case LINKED_PICKED_BASE:
				theReferee.LinkToPickedBase(node);
				break;
		}
	}
	return TRUE;
}
	
void RefereePickModeCallback::EnterMode(IObjParam *ip)
{
	switch (m_buttonIndex)
	{
		case LINKED_PICKED_OBJECT:
			theReferee.m_linkedPickedObjectButton->SetCheck(TRUE);
			break;
		case LINKED_PICKED_BASE:
			theReferee.m_linkedPickedBaseButton->SetCheck(TRUE);
			break;
	}
}

void RefereePickModeCallback::ExitMode(IObjParam *ip)
{
	switch (m_buttonIndex)
	{
		case LINKED_PICKED_OBJECT:
			theReferee.m_linkedPickedObjectButton->SetCheck(FALSE);
			break;
		case LINKED_PICKED_BASE:
			theReferee.m_linkedPickedBaseButton->SetCheck(FALSE);
			break;
	}
}

PickNodeCallback *RefereePickModeCallback::GetFilter()
{
	return &thePickFilter;
}

BOOL RefereePickModeCallback::RightClick(IObjParam *ip,ViewExp *vpt) 
{
	return TRUE;
}


