#ifndef __MEM_BLOCK_H
#define __MEM_BLOCK_H

#include <ostream>
using namespace std;

class MemBlock
{
	MemBlock( const MemBlock &defBlock) { };
	MemBlock &				operator =( const MemBlock &defBlock) { };

protected:

	unsigned int			d_Size;
	void *					p_Buffer;


public:
	MemBlock( void *defBuffer, unsigned int defSize) { p_Buffer = defBuffer; d_Size = defSize; }
	virtual ~MemBlock( ) { };

	unsigned int			size( void) const { return d_Size; }
	void *					buffer( void) const { return p_Buffer; }

	void						writePartial( ostream &str, unsigned int uiStart, unsigned int uiLength) const;
	void 						writeAll( ostream &str) const { writePartial( str, 0, d_Size); } 
} ;

inline ostream & operator <<( ostream &str, MemBlock &Block)
{
	Block.writeAll( str);
	
	return str;
}

#endif

	