////////////////////////////////////////////////////////////////////////////
//
// CRC
//
// A class that implements the CRC-32 Cyclic Redundancy Check Algorithm
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: CRC.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/27/02    Time: 12:23p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * more analyze functions
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/05/02    Time: 4:23p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * CRC class for Remove Duplicated Images
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_CRC_H
#define TUPPERFILTERS_LIB_PLUGINS_CRC_H

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

#endif

