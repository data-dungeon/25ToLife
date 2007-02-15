////////////////////////////////////////////////////////////////////////////
//
// CRC
//
// A class that implements the CRC-32 Cyclic Redundancy Check Algorithm
//
////////////////////////////////////////////////////////////////////////////

#ifndef CRC_H
#define CRC_H

class CRC
{
	unsigned long m_table[256];
	unsigned long m_value;
public:
	CRC(void);
	void Init(void); 
	void AnalyzeData(const void *pBuffer, unsigned long size);
	void AnalyzeData(unsigned long value);
	void AnalyzeData(int value);
	void AnalyzeData(unsigned char value);
	void AnalyzeData(float value);
	void AnalyzeData(const char *string);
	unsigned long GetResult(void);
private:
	void Analyze(const unsigned char *pBuffer, unsigned long size);
	unsigned long Reflect(unsigned long ref, char ch);
};

#include "CRC.hpp"

#endif

