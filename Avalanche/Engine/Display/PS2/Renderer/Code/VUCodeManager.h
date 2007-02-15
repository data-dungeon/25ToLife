#ifndef __VU_CODE_MANAGER_H
#define __VU_CODE_MANAGER_H

#include "VUCodeMemManager.h"
#include "DMABlock.h"
#include "CompilerControl.h"

class VUCodeManager : public VUCodeMemManager
{
	VUManagedSlot *			findBestSlot( VUManagedCode &Code);
	
public:
	VUCodeManager(
		unsigned int			_VUBase,
		unsigned int			_VUSize
	);
	
	VUManagedSlot *		createSlot( VUManagedCode &Code, bool bLocked = false);

	bool						loadManagedCode( VUManagedCode &Code, unsigned int &vuAddr, bool bActivate = false, bool bEndOfChain = true) NAMED_SECTION("text.rend1");

	void  					activateManagedCode( VUManagedCode &Code, unsigned int &vuAddr);
	
} ;

#endif

