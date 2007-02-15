#ifndef __BYTE_ALIGNER_H
#define __BYTE_ALIGNER_H

#include "OStreamAligner.h"

class ByteAligner : public OStreamAligner
{
	unsigned int		d_Alignment;
	unsigned char		d_AlignmentData;
		
public:
	ByteAligner( unsigned int defAlignment, unsigned char defData = 0) : d_Alignment( defAlignment), d_AlignmentData( defData) { }
	
	unsigned int				offset( streampos pos);
	virtual void				align( ostream &str);

} ;

#endif

