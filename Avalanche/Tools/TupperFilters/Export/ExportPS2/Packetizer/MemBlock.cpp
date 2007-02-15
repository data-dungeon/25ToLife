#include "ExportPch.h"
#include <assert.h>

#include "MemBlock.h"


void MemBlock::writePartial( ostream &str, unsigned int uiStart, unsigned int uiLength) const
{
	assert( (uiStart + uiLength) < d_Size);

	str.write( ((char *) p_Buffer) + uiStart, uiLength);

}