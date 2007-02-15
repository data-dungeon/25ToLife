/***************************************************************************/
// VAG stuff
/***************************************************************************/
#ifndef VAG_H
#define VAG_H

// VAG header structure
struct VAGHeader
{
	char 	ID[4];
	int 	version;
	char 	reserved1[4];
	int 	dataSize;
	int 	sampleFreq;
	char	reserved2[12];
	char 	name[16];
};

#endif
