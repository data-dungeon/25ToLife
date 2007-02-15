
#include "VUCodeFactory.h"
#include "VUCodeBlock.h"

VUManagedCode & VUCodeFactory::createCode( void *pBegin, void *pEnd)
{
	VUManagedCode * 		p_NewCode = new VUManagedCode( pBegin, pEnd);

	VUManagedCode *		p_Existing = findExistingCode( *p_NewCode);
	
	if( p_Existing)
	{
		delete p_NewCode;
		
		p_NewCode = p_Existing;
	}
	else
	{
		addCode( *p_NewCode);
	}
	
	return *p_NewCode;
}

void VUCodeFactory::addCode( VUManagedCode &Code)
{
	linkCode *					pNewCode = new linkCode( Code, p_CodeList);
	
	p_CodeList = pNewCode;
}

void VUCodeFactory::addFunction( VUManagedFunction &Function)
{
	linkFunction *				pNewFunction = new linkFunction( Function, p_FunctionList);
	
	p_FunctionList = pNewFunction;
}

VUManagedFunction & VUCodeFactory::createFunction( void *pEntry, void *pBegin, void *pEnd)
{
	RENDER_ASSERT( (pEntry >= pBegin)  &&  (pEntry < pEnd) );
	RENDER_ASSERT( VUCodeBlock::isValidInstructionOffset( pEntry, pBegin) );
	
	VUManagedCode &Code = createCode( pBegin, pEnd);
	
	VUManagedFunction *p_NewFunction = new VUManagedFunction( Code, Code.offsetInstructions( pEntry));

	addFunction( *p_NewFunction);
	
	return *p_NewFunction;
}

VUManagedFunction & VUCodeFactory::createFunction( void *pEntry)
{
	VUManagedFunction *	p_NewFunction = 0;
	
	for( linkCode *pWalker = p_CodeList; pWalker; pWalker = pWalker->next( ) )
	{
		VUManagedCode &Code = pWalker->get( );
		
		if( Code.inBlock( pEntry)  &&  Code.isValidEntry( pEntry))
		{
			p_NewFunction = new VUManagedFunction( Code, Code.offsetInstructions( pEntry) );
			
			addFunction( *p_NewFunction);
		}
	}
	
	RENDER_ASSERT( p_NewFunction);
	
	return *p_NewFunction;
}
	
VUManagedCode * VUCodeFactory::findExistingCode( const VUManagedCode &Other)
{
	for( linkCode *pWalker = p_CodeList; pWalker; pWalker = pWalker->next( ))
	{
		if( pWalker->get( ) == Other)
			return & pWalker->get( );			
	}
	
	return 0;
} 
