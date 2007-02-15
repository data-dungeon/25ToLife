#ifndef __MASTER_CODE_FACTORY_H
#define __MASTER_CODE_FACTORY_H

#include "VUCodeFactory.h"

namespace MasterCodeFactory
{
	extern VUCodeFactory		Factory;
	
	inline VUManagedCode &			createManagedCode( void *pCodeBegin, void *pCodeEnd)
	{
		return Factory.createCode( pCodeBegin, pCodeEnd);
	}
	
	inline VUManagedFunction &		createManagedFunction( void *pEntry, void *pCodeBegin, void *pCodeEnd)
	{
		return Factory.createFunction( pEntry, pCodeBegin, pCodeEnd);
	}
	
	inline VUManagedFunction &		createManagedFunction( void *pEntry)
	{
		return Factory.createFunction( pEntry);
	}
	
} 

#endif