////////////////////////////////////////////////////////////////////////////
//
// CRC
//
// A class that implements the CRC-32 Cyclic Redundancy Check Algorithm
//
////////////////////////////////////////////////////////////////////////////

#include "CRC.h"

inline CRC::CRC(void)
{
   // This is the official polynomial used by CRC-32 
   // in PKZip, WinZip and Ethernet. 
   unsigned long ulPolynomial = 0x04C11DB7;

   // 256 values representing ASCII character codes.
   for (int i = 0; i <= 0xFF; i++)
   {
      m_table[i] = Reflect(i, 8) << 24;
      for (int j = 0; j < 8; j++)
         m_table[i] = (m_table[i] << 1) ^ (m_table[i] & (1 << 31) ? ulPolynomial : 0);
      m_table[i] = Reflect(m_table[i], 32);
   }
   Init();
}

// call if you need to generate more than one value through the life of the object
// constructor does this too
inline void CRC::Init(void)
{
   m_value = 0xFFFFFFFF;
}

inline void CRC::AnalyzeData(const void *pBuffer, unsigned long size)
{
   // check the validity of the data
   if (pBuffer && size)
   {
      Analyze((const unsigned char *)pBuffer, size);
   }
}

inline void CRC::AnalyzeData(unsigned long value)
{
   Analyze((const unsigned char *)&value,4);
}

inline void CRC::AnalyzeData(int value)
{
   Analyze((const unsigned char *)&value,4);
}

inline void CRC::AnalyzeData(unsigned char value)
{
   Analyze((const unsigned char *)&value,1);
}

inline void CRC::AnalyzeData(float value)
{
   Analyze((const unsigned char *)&value,4);
}

inline void CRC::AnalyzeData(const char *string)
{
   if (string)
   {
      int len = (int)strlen(string);
      Analyze((const unsigned char *)string,len);
   }
}


inline unsigned long CRC::GetResult(void)
{
   return m_value ^ 0xFFFFFFFF;
}

inline unsigned long CRC::Reflect(unsigned long ref, char ch)
{
   unsigned long value = 0;

   // Swap bit 0 for bit 7
   // bit 1 for bit 6, etc.
   for(int i = 1; i < (ch + 1); i++)
   {
      if (ref & 1)
         value |= 1 << (ch - i);
      ref >>= 1;
   }
   return value;
}

inline void CRC::Analyze(const unsigned char *pBuffer, unsigned long size)
{
   // calculate the CRC
   const unsigned char *pbyte = pBuffer;

   while (size--)
      m_value = (m_value >> 8) ^ m_table[(m_value & 0xFF) ^ *pbyte++];
}



