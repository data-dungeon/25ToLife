////////////////////////////////////////////////////////////////////////////
//
// CRC
//
// A class that implements the CRC-32 Cyclic Redundancy Check Algorithm
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef __CRC__
#define __CRC__

class CRCTable
{
public:
	unsigned long m_table[256];
	CRCTable(void);
	unsigned long Reflect(unsigned long ref, char ch);
};

class CRC
{
	unsigned long m_value;
public:
	CRC(void);
	void Init(void); 
	void AnalyzeData(const void *pBuffer, unsigned long size);
	void AnalyzeData(bool value);
	void AnalyzeData(unsigned long value);
	void AnalyzeData(int value);
	void AnalyzeData(unsigned char value);
	void AnalyzeData(float value);
	void AnalyzeData(const char *string);
	unsigned long GetResult(void);
private:
	void Analyze(const unsigned char *pBuffer, unsigned long size);
};

#endif

