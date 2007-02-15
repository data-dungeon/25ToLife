#ifndef __VU_CODE_FACTORY_H
#define __VU_CODE_FACTORY_H

#include "templateLink.h"

#include "VUManagedCode.h"
#include "VUManagedFunction.h"

class VUCodeFactory
{

	typedef link<VUManagedCode>		linkCode;
	typedef link<VUManagedFunction>	linkFunction;
	
	
	linkCode *					p_CodeList;
	linkFunction *				p_FunctionList;
	
	void							addCode( VUManagedCode &Code);
	void							addFunction( VUManagedFunction &Function);
	
	VUManagedCode * 			findExistingCode( const VUManagedCode &Other);
	
public:
	VUManagedCode &		createCode( void * pCodeBegin, void * pCodeEnd);

	VUManagedFunction &	createFunction( void * pEntry);
	VUManagedFunction &	createFunction( void * pEntry, void * pCodeBegin, void *pCodeEnd);

} ;		


#endif
			