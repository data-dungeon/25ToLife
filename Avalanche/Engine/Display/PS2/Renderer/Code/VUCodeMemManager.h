#ifndef __VU_CODE_MEM_MANAGER_H
#define __VU_CODE_MEM_MANAGER_H

#include "VUManagedSlot.h"
#include "VUManagedCode.h"
#include "CompilerControl.h"

class VUCodeMemManager
{
	unsigned int			d_VUBaseInit, d_VUSizeInit;
	unsigned int			d_VUBaseCurrent, d_VUSizeCurrent;

	VUManagedSlot *		p_SlotList;
	VUManagedSlot *		p_LockedList;

	void						updateVUSizes( unsigned int vuSize);

protected:
	VUManagedSlot * 		findEmptySlot( unsigned int vuSize);
	VUManagedSlot * 		findInactiveSlot( unsigned int vuSize);
	VUManagedSlot *		findActiveSlot( unsigned int vuSize);

	bool						ValidateSlot( VUManagedSlot *pSlot);
	void						releaseSlotList( VUManagedSlot *pList );
	
	VUManagedSlot *		_createSlot( unsigned int vuSize, unsigned int groupID, unsigned int uniqueID, bool bLocked);
	
public:
	VUCodeMemManager(
		unsigned int			_VUBase,
		unsigned int			_VUSize
	) ;

	~VUCodeMemManager( void );
	
	void						flush( bool bFlushActive, bool bFlushChained );
	
	void						swapActiveCode( void )  NAMED_SECTION("text.rend1");

	unsigned int			VUMemAvailable( void ) { return d_VUSizeCurrent; }
	unsigned int			VUMemAllocated( void ) { return d_VUSizeInit - d_VUSizeCurrent; }

	// this marks an area of vumem as unused and asserts if something in that area was locked.
	void						trashVUMemBlock( unsigned int size, unsigned int count );

	void						partitionBySize( unsigned int size);
	void						partitionByCount( unsigned int count);
	
	VUManagedSlot *		createLockedSlot( unsigned int vuSize, unsigned int uniqueID = 0, unsigned int groupID = 0);
	VUManagedSlot *		createUnlockedSlot( unsigned int vuSize, unsigned int uniqueID = 0, unsigned int groupID = 0);
	
	VUManagedSlot * 		findAvailableSlot( unsigned int vuSize);

	VUManagedSlot * 		findByIDs( unsigned int groupID, unsigned int uniqueID);
	VUManagedSlot * 		findByGroupID( unsigned int groupID);
	VUManagedSlot * 		findByUniqueID( unsigned int uniqueID) NAMED_SECTION("text.rend1");
} ;

#endif
