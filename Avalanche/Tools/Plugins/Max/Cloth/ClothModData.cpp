#include "ClothPCH.h"


/****************************************************************************
 ClothConnection::GetConnectionType()

 Returns the type of connection

 Front to front = CT_FRONTFRONT
 Front to attached = CT_FRONTFRONT
 Front to back = CT_FRONTBACK
 Back to attached = CT_FRONTBACK
 Inactive to anything = CT_BAD
 Attached to attached = CT_BAD
 Back to back = CT_BAD
****************************************************************************/
ConnectionType ClothConnection::GetConnectionType(ClothModData* pModData)
{
	ClothPoint p1 = pModData->points[v1];
	ClothPoint p2 = pModData->points[v2];

	ConnectionType type;
	// front to front
	if (POINTSTATE_FRONT == p1.state && POINTSTATE_FRONT == p2.state)
		type = CT_FRONTFRONT;
	// front to attached (same as front to front)
	else if ((POINTSTATE_FRONT == p1.state && POINTSTATE_ATTACHED == p2.state) ||
				(POINTSTATE_ATTACHED == p1.state && POINTSTATE_FRONT == p2.state))
		type = CT_FRONTFRONT;
	// back to front
	else if ((POINTSTATE_FRONT == p1.state && POINTSTATE_BACK == p2.state) ||
				(POINTSTATE_BACK == p1.state && POINTSTATE_FRONT == p2.state))
		type = CT_FRONTBACK;
	// back to attached (same as back to front)
	else if ((POINTSTATE_ATTACHED == p1.state && POINTSTATE_BACK == p2.state) ||
				(POINTSTATE_BACK == p1.state && POINTSTATE_ATTACHED == p2.state))
		type = CT_FRONTBACK;
	else // assume an error condition
		type = CT_BAD;

	return type;
}


ClothModData::ClothModData()
{
	id = -1;
	isDragging = false;
}


ClothModData::ClothModData(int i)
{
	id = i;
	isDragging = false;
}


ClothModData::~ClothModData()
{
}


LocalModData*	ClothModData::Clone()
{
	ClothModData* pCmd = new ClothModData();
	pCmd->id = id;

	return pCmd;
}
