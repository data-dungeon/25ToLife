#include "VUCodeSetupHelper.h"

VUCodeSetupHelper::VUCodeSetupHelper( VUCodeManager &Manager) :
	d_CodeManager( Manager),
	p_Grouped( 0),
	p_NonGrouped( 0),
	p_NonGroupedLocked( 0)
{

}

		
void VUCodeSetupHelper::addCode( VUManagedCode &Code, bool bLocked)
{
linkCode **			ppAddList = bLocked ? (&p_NonGroupedLocked) : (&p_NonGrouped);

	*ppAddList = new linkCode( Code, *ppAddList);

}

void VUCodeSetupHelper::addGroupedCode( VUManagedCode &Code, unsigned int groupID, bool bLocked)
{
listGrouped *		pAddGroup = findGrouped( groupID);

	if( !pAddGroup)
	{
		pAddGroup = new listGrouped;

		pAddGroup->b_Locked = bLocked;
		pAddGroup->d_GroupID = groupID;
		pAddGroup->p_List = 0;
		
		p_Grouped = new linkGrouped( *pAddGroup, p_Grouped);
		
	}
	else
	{
		if( bLocked)
			pAddGroup->b_Locked = true;		
	}
	
	pAddGroup->p_List = new linkCode( Code, pAddGroup->p_List);
	
}

void VUCodeSetupHelper::setupVUCode( void )
{
unsigned int				vuSlotSize;
unsigned int				vuMaxSlotSize = 0;

linkCode *					pListWalker;
linkGrouped *				pLinkWalker;

	// Grouped Code
	
	vuSlotSize = 0;
	
	for( pLinkWalker = p_Grouped; pLinkWalker; pLinkWalker = pLinkWalker->next( ) )
	{
		listGrouped & 		Current = pLinkWalker->get( );
		
		vuSlotSize = findLargestVUSize( Current.p_List);
		
		if( Current.b_Locked)
		{
			VUManagedSlot * pSlot = d_CodeManager.createLockedSlot( vuSlotSize, 0, Current.d_GroupID);
			RENDER_ASSERT( pSlot);
						 
			setGroupAndSlot( Current.p_List, Current.d_GroupID, pSlot);
		}
		else
		{
			if( vuSlotSize > vuMaxSlotSize)
				vuMaxSlotSize = vuSlotSize;
				
			setGroupAndSlot( Current.p_List, Current.d_GroupID, 0);		
		}		
	}
	
	// Non-Grouped Locked
	
	for( pListWalker = p_NonGroupedLocked; pListWalker; pListWalker = pListWalker->next( ) )
	{
		VUManagedCode & Code = pListWalker->get( );
		
		VUManagedSlot * pSlot = d_CodeManager.createLockedSlot( Code.getMPG( ).instructions( ), Code.uniqueID( ), 0);
		RENDER_ASSERT( pSlot);

		Code.lockSlot( pSlot);
		Code.groupID( 0);
	}
	
	// Non-Grouped not Locked
	vuSlotSize = findLargestVUSize( p_NonGrouped);
	setGroupAndSlot( p_NonGrouped, 0, 0);
	
	if( vuSlotSize > vuMaxSlotSize)
		vuMaxSlotSize = vuSlotSize;
		
	d_CodeManager.partitionBySize( vuMaxSlotSize);
	d_CodeManager.partitionByCount( 1);
	
}

VUCodeSetupHelper::~VUCodeSetupHelper( void )
{

	freelinkCode( p_NonGrouped);
	freelinkCode( p_NonGroupedLocked);
	
	for( linkGrouped *pWalker = p_Grouped; pWalker; )
	{
		linkGrouped *pFree = pWalker;
		
		pWalker = pWalker->next( );
		
		freelinkCode( pFree->get( ).p_List);
		
		delete pFree;
	}
	
}


void VUCodeSetupHelper::freelinkCode( linkCode *pList)
{
	for( linkCode *pWalker = pList; pWalker; )
	{
		linkCode *pFree = pWalker;
		
		pWalker = pWalker->next( );
		
		delete pFree;
	}
	
}


unsigned int VUCodeSetupHelper::findLargestVUSize( linkCode *pList)
{
	unsigned int sizeMax = 0;
	
	for( ; pList; pList = pList->next( ) )
	{
		unsigned int sizeThis = pList->get( ).getMPG( ).instructions( );
		
		if( sizeThis > sizeMax)
			sizeMax = sizeThis;
			
	}
	
	return sizeMax;
}

void VUCodeSetupHelper::setGroupAndSlot( linkCode *pList, unsigned int groupID, VUManagedSlot *pSlot)
{
	for( linkCode *pWalker = pList; pWalker; pWalker = pWalker->next( ) )
	{
		VUManagedCode &Code = pWalker->get( );
		
		Code.lockSlot( pSlot);
		Code.groupID( groupID);
	}
	
}

VUCodeSetupHelper::listGrouped * VUCodeSetupHelper::findGrouped( unsigned int groupID)
{

	for( linkGrouped *pWalker = p_Grouped; pWalker; pWalker = pWalker->next( ) )
	{
		if( pWalker->get( ).d_GroupID == groupID)
			return &pWalker->get( );
			
	}
	
	return 0;
}


