#ifndef __VU_CODE_SETUP_HELPER_H
#define __VU_CODE_SETUP_HELPER_H

#include "templateLink.h"

#include "VUCodeManager.h"

class VUCodeSetupHelper
{
typedef link<VUManagedCode>	linkCode;

typedef struct
{
	bool					b_Locked;
	unsigned int		d_GroupID;
	linkCode *			p_List;
} listGrouped;

typedef link<listGrouped>	linkGrouped;

	VUCodeManager &			d_CodeManager;
	
	linkGrouped *				p_Grouped;
		
	linkCode *					p_NonGrouped;
	linkCode *					p_NonGroupedLocked;
	
	listGrouped *			findGrouped( unsigned int groupID);
	unsigned int			findLargestVUSize( linkCode *pList);
	
	void						setGroupAndSlot( linkCode *pList, unsigned int groupID, VUManagedSlot *pSlot);
	
	void						freelinkCode( linkCode *pList);
public:
	VUCodeSetupHelper( VUCodeManager &Manager);
	~VUCodeSetupHelper( void );
	
	
	void						addCode( VUManagedCode &Code, bool bLocked = false);
	void						addGroupedCode( VUManagedCode &Code, unsigned int groupID, bool bLocked = false);
	
	void						setupVUCode( void );
	
} ;

#endif
	
	