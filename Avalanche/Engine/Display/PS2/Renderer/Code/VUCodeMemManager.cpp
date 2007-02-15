#include "renderassert.h"
#include "VUCodeMemManager.h"

VUCodeMemManager::VUCodeMemManager( unsigned int _vuBase, unsigned int _vuSize) :
	d_VUBaseInit( _vuBase),
	d_VUSizeInit( _vuSize),
	d_VUBaseCurrent( _vuBase),
	d_VUSizeCurrent( _vuSize),
	p_SlotList( 0),
	p_LockedList( 0)
{
	
}

VUCodeMemManager::~VUCodeMemManager( void )
{
	releaseSlotList( p_SlotList);
	p_SlotList = 0;
	
	releaseSlotList( p_LockedList);
	p_LockedList = 0;
}

void VUCodeMemManager::flush( bool bFlushActive, bool bFlushChained )
{
	for( VUManagedSlot *pWalker = p_SlotList; pWalker; pWalker = pWalker->next( ) )
	{
		if( bFlushActive)
			pWalker->unActivate( );
			
		if( bFlushChained)
			pWalker->unChain( );
	}
}

void VUCodeMemManager::swapActiveCode( void )
{
	for( VUManagedSlot *pWalker = p_SlotList; pWalker; pWalker = pWalker->next( ) )
	{
		if( pWalker->isActive( ))
			pWalker->unActivate( );
		
		if( pWalker->inChain( ) )
			pWalker->swapChain( );
	}

}

VUManagedSlot *VUCodeMemManager::createLockedSlot( unsigned int vuSize, unsigned int uniqueID, unsigned int groupID)
{
	return _createSlot( vuSize, uniqueID, groupID, true);
}

VUManagedSlot *VUCodeMemManager::createUnlockedSlot( unsigned int vuSize, unsigned int uniqueID, unsigned int groupID)
{
	return _createSlot( vuSize, uniqueID, groupID, false);
}
	

void VUCodeMemManager::partitionBySize( unsigned int size)
{
	RENDER_ASSERT( size);
	RENDER_ASSERT( size < VUMemAvailable( ) );
	
	unsigned int count = VUMemAvailable( ) / size;

	for( unsigned int i = 0; i < count; i++)
		createUnlockedSlot( size);
}

void VUCodeMemManager::partitionByCount( unsigned int count)
{
	RENDER_ASSERT( count);

	unsigned int size = VUMemAvailable( ) / count;

	for( unsigned int i = 0; i < count; i++)
		createUnlockedSlot( size);
}

// this marks an area of vumem as unused and asserts if something in that area was locked.
void VUCodeMemManager::trashVUMemBlock( unsigned int start, unsigned int size )
{
	unsigned int end = start + size;
	unsigned int currStart;
	unsigned int currEnd;

	for( VUManagedSlot *pWalker = p_SlotList; pWalker; pWalker = pWalker->next( ) )
	{
		currStart = pWalker->vuAddr();
		currEnd = currStart + pWalker->vuSize();

		if(currStart <= end && currEnd >= start)
		{
			// need to flush this one...
			pWalker->unLoad();
		}
	}

	for( VUManagedSlot *pWalker = p_LockedList; pWalker; pWalker = pWalker->next( ) )
	{
		currStart = pWalker->vuAddr();
		currEnd = currStart + pWalker->vuSize();
		
		if(currStart <= end && currEnd >= start)
		{
			// need to flush this one...
			pWalker->unLoad();
		}
	}
}
	
// private methods

VUManagedSlot *VUCodeMemManager::_createSlot( unsigned int vuSize, unsigned int uniqueID, unsigned int groupID, bool bLocked)
{
	if( vuSize > d_VUSizeCurrent)
		return 0;

	VUManagedSlot *	pNewSlot = new VUManagedSlot( d_VUBaseCurrent, vuSize, uniqueID, groupID, false, bLocked);

	updateVUSizes( vuSize);

	if( bLocked)
	{
		pNewSlot->next( p_LockedList);
		p_LockedList = pNewSlot;
	} else
	{
		pNewSlot->next( p_SlotList);
		p_SlotList = pNewSlot;
	}

	return pNewSlot;
}


bool VUCodeMemManager::ValidateSlot( VUManagedSlot *pSlot)
{
	for( VUManagedSlot *pWalker = p_SlotList; pWalker; pWalker = pWalker->next( ))
	{
		if( pWalker == pSlot)
			return true;
	}

	for( VUManagedSlot *pWalker = p_LockedList; pWalker; pWalker = pWalker->next( ))
	{
		if( pWalker == pSlot)
			return true;
	}

	return false;
}

void VUCodeMemManager::releaseSlotList( VUManagedSlot *pList )
{
	for( VUManagedSlot *pWalker = pList; pWalker; )
	{
		VUManagedSlot * pRelease = pWalker;

		pWalker = pWalker->next( );

		delete pRelease;
	}
}
		
void VUCodeMemManager::updateVUSizes( unsigned int vuSize)
{
	d_VUBaseCurrent += vuSize;
	d_VUSizeCurrent -= vuSize;
}

VUManagedSlot * VUCodeMemManager::findAvailableSlot( unsigned int vuSize )
{
	VUManagedSlot * pSlot = findEmptySlot( vuSize );
	
	if( !pSlot)
		pSlot = findInactiveSlot( vuSize );
		
	if( !pSlot)
		pSlot = findActiveSlot( vuSize);
		
	return pSlot;
}

VUManagedSlot * VUCodeMemManager::findActiveSlot( unsigned int vuSize)
{
	for( VUManagedSlot *pWalker = p_SlotList; pWalker; pWalker = pWalker->next( ) )
	{
		if( !pWalker->inChain( )  &&  pWalker->isActive( )  &&  pWalker->vuSize( ) >= vuSize)
			return pWalker;
	}
				
	return 0;
}

VUManagedSlot * VUCodeMemManager::findEmptySlot( unsigned int vuSize)
{
	
	for( VUManagedSlot *pWalker = p_SlotList; pWalker; pWalker = pWalker->next( ) )
	{
		if( !pWalker->inChain( )  &&  !pWalker->isActive( )  &&  (pWalker->uniqueID( ) == 0)  &&  (pWalker->vuSize( ) >= vuSize) ) 
			return pWalker;
	}

	return 0;
}

VUManagedSlot * VUCodeMemManager::findInactiveSlot( unsigned int vuSize)
{
	for( VUManagedSlot *pWalker = p_SlotList; pWalker; pWalker = pWalker->next( ) )
	{
		if( !pWalker->inChain( )  &&  !pWalker->isActive( )  &&  (pWalker->vuSize( ) >= vuSize))
			return pWalker;
	}

	return 0;
}



VUManagedSlot * VUCodeMemManager::findByIDs( unsigned int groupID, unsigned int uniqueID)
{
	for( VUManagedSlot *pWalker = p_SlotList; pWalker; pWalker = pWalker->next( ) )
	{
		if( (pWalker->groupID( ) == groupID)  &&  (pWalker->uniqueID( ) == uniqueID))
			return pWalker;
	}

	return 0;
}

VUManagedSlot * VUCodeMemManager::findByGroupID( unsigned int groupID)
{
	for( VUManagedSlot *pWalker = p_SlotList; pWalker; pWalker = pWalker->next( ) )
	{
		if( !pWalker->inChain( )  &&  (pWalker->groupID( ) == groupID))
			return pWalker;
	}

	return 0;
}

VUManagedSlot * VUCodeMemManager::findByUniqueID( unsigned int uniqueID)
{
	for( VUManagedSlot *pWalker = p_SlotList; pWalker; pWalker = pWalker->next( ) )
	{
		if( pWalker->uniqueID( ) == uniqueID)
			return pWalker;
	}

	return 0;
}
