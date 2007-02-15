#include "renderassert.h"

#include "VUCodeManager.h"
#include "MasterList.h"

VUCodeManager::VUCodeManager( unsigned int _VUBase, unsigned int _VUSize) : 
	VUCodeMemManager( _VUBase, _VUSize)
{
	
}
	
	
VUManagedSlot * VUCodeManager::createSlot( VUManagedCode &Code, bool bLocked)
{
	VUManagedSlot * pSlot = _createSlot( Code.getMPG( ).instructions( ), Code.uniqueID( ), Code.groupID( ), bLocked);
	
	if( bLocked)
		Code.lockSlot( pSlot);	
		
	return pSlot;			
}


VUManagedSlot *VUCodeManager::findBestSlot( VUManagedCode &Code)
{
	VUManagedSlot *			pSlot = 0;
	
	pSlot = Code.lockSlot( );
	
	if( pSlot)
	{
		RENDER_ASSERT( pSlot->groupID( ) == Code.groupID( ) );
		return pSlot;
	}
	
	pSlot = findByUniqueID( Code.uniqueID( ) );
			
	if( !pSlot)
		pSlot = findAvailableSlot( Code.getMPG( ).instructions( ) );
	
	return pSlot;
}

		
bool VUCodeManager::loadManagedCode( VUManagedCode &Code, unsigned int &vuAddr, bool bActivate, bool bEndOfChain)
{
	VUDynamicMPG &				MPG = Code.getMPG( );
	VUManagedSlot *			pSlot = findBestSlot( Code);
	
	if( !pSlot)
	{	// for debugging
		findBestSlot( Code);
		RENDER_ASSERT( 0);
	}
	
	if( (pSlot->uniqueID( ) != Code.uniqueID( ))  ||  !pSlot->isLoaded( ) )
	{	
		bool bFlushList = false;
		
		if( pSlot->isActive( ))
		{
			flush( true, false);
			
			if( !MasterList::GetFlushed( ))
				bFlushList = true;
		}
		
		pSlot->groupID( Code.groupID( ) );
		pSlot->uniqueID( Code.uniqueID( ) );
		
		pSlot->Load( );
		
		MPG.addToDMA_MPG( pSlot->vuAddr( ), bFlushList );
		
		if( bFlushList)
			MasterList::SetFlushed( true);
	}
	
	pSlot->Chain( bActivate );
		
	if( bEndOfChain)
		swapActiveCode( );
	
	vuAddr = pSlot->vuAddr( );		
	return true;
}
		
void VUCodeManager::activateManagedCode( VUManagedCode &Code, unsigned int &vuAddr)
{
	VUDynamicMPG &				MPG = Code.getMPG( );
	VUManagedSlot *			pSlot = findBestSlot( Code);
	
	if( (pSlot->uniqueID( ) != Code.uniqueID( ))  ||  !pSlot->isLoaded( ) )
	{	
		pSlot->groupID( Code.groupID( ) );
		pSlot->uniqueID( Code.uniqueID( ) );
		
		pSlot->Load( );
	
		MPG.addToDMA_MPG( pSlot->vuAddr( ) );
	}
	
	vuAddr = pSlot->vuAddr( );		
}
