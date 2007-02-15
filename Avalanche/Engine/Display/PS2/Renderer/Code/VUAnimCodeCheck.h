#ifndef __VU_CODE_CHECK_H
#define __VU_CODE_CHECK_H

#include "VUManagedCode.h"

#ifdef A_COMPILER_SN
#include <stdio.h>  // for printf().
#endif

typedef struct 
{
	unsigned int 	idx;
	char *			name;
} CodeCheckTransTable;

class VUCodeCheck
{
	const VUManagedCode &	d_Code;
	unsigned int *				p_OffsetList;
	unsigned int				d_OffsetListEntries;

public:
	VUCodeCheck( 
		const VUManagedCode & 	_Code,
		unsigned int *				_OffsetList,
		unsigned int				_OffsetListEntries
	) :
	d_Code( _Code),
	p_OffsetList( _OffsetList),
	d_OffsetListEntries( _OffsetListEntries)
	{
	
	}
	
	unsigned int 	offsetForEntry( unsigned int entry) const
	{
		RENDER_ASSERT( entry < d_OffsetListEntries);
		
		return d_Code.offsetInstructions( (void *) p_OffsetList[entry] );
	}
	
	bool 				validateAgainst( const VUCodeCheck &Other, CodeCheckTransTable *pTable = 0, unsigned int tableEntries = 0) const
	{
		bool			bProblem = false;
		
		if( Other.d_Code.sizeInstructions( ) > d_Code.sizeInstructions( ) )
			printf( "Code Size Larger %d vs %d\n", Other.d_Code.sizeInstructions( ), d_Code.sizeInstructions( ));
		
		if( Other.d_OffsetListEntries != d_OffsetListEntries )
		{
			printf( "Entry size doesn't match (%d vs %d)\n", d_OffsetListEntries, Other.d_OffsetListEntries);
			
			return true;
		}
		
		if( pTable  &&  (d_OffsetListEntries > tableEntries) )
		{
			printf( "Table Size Doesn't Match (Table = %d vs Entries = %d)\n", tableEntries, d_OffsetListEntries);

			return true; 
		}
		
		unsigned int 	checkCount = (tableEntries < Other.d_OffsetListEntries) ? tableEntries : d_OffsetListEntries;
		int 				sum = 0;
		
		for( unsigned int i = 0; i < checkCount; i++)
		{
			unsigned int j = pTable ? pTable[i].idx : i;
							
			unsigned int myCurrentOffset = offsetForEntry( j);
			unsigned int otherCurrentOffset = Other.offsetForEntry( j);
			
			if( myCurrentOffset != otherCurrentOffset)
			{
				int delta = myCurrentOffset - otherCurrentOffset;
				int change = delta - sum;
				
				if( change )
				{
					if( delta > 0 )
						printf( "Entry %d (%s) short by %d", j, pTable ? pTable[i].name : "unknown", delta);
					else if ( delta < 0 )
						printf( "Entry %d (%s) over by %d", j, pTable ? pTable[i].name : "unknown", -delta);
					else
						printf( "Entry %d (%s) aligned", j, pTable ? pTable[i].name : "unknown");
						
					if( change > 0)
						printf( ", add %d more\n", change); 
					else 
						printf( ", remove %d more\n", -change ); 
		
				}
						
				sum = delta;
				
				bProblem = true;
			}
			
		} 
		
		return bProblem;
	}
	
} ;
	
#endif
