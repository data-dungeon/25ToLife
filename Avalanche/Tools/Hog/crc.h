/////////////////////////////////////////////////////////////////////////////
// Really simple class for computing a CRC
/////////////////////////////////////////////////////////////////////////////
#ifndef CRC_H
#define CRC_H

// The basic type
typedef unsigned int CRCValue;

// Simple function that does it all!
CRCValue ComputeCRC(void *data, unsigned int dataLength);

// CRC lookup table forward
class CRCLookupTable;

class CRCCompute
{
public:
	// Initialize with a key
	CRCCompute(CRCValue key = 0x18061968);

	// Add data into the computation
	void PutByte(unsigned char byte);
	void PutString(unsigned char *string, unsigned int length);

	// Call to get CRC value and reset
	CRCValue Done(void);

private:
	CRCValue 					_key;		// really 33-bit key, counting implicit 1 top-bit
	CRCValue						_register;
	static CRCLookupTable	_table;
};

// The lookup table
class CRCLookupTable
{
public:
	CRCLookupTable(void) : _key (0) {}
	void Init(CRCValue key);
	CRCValue operator [](unsigned i)
		{ return _table [i]; }

private:
	CRCValue _table [256];
	CRCValue _key;
};

#endif
