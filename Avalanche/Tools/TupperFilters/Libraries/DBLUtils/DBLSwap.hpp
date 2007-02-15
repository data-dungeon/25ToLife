////////////////////////////////////////////////////////////////////////////
//
// DBLSwap
//
// Class to handle the swapping endians of the DBL data
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DBLSwap.hpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 5:17p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Added library for handling the dbl file format (taken from Import
 * Export)
 */

inline bool DBLSwap::GetSwapEndianActive(void) { return m_swapEndianActive; }
inline void DBLSwap::SetSwapEndianActive(bool swapEndianActive) { m_swapEndianActive = swapEndianActive; }

inline void DBLSwap::SwapEndian2(void *pValue)
{
	*((u16 *) (pValue)) = (((*((u16 *) (pValue)) & 0x00ff) << 8) | 
						   ((*((u16 *) (pValue)) & 0xff00) >> 8));
}

inline void DBLSwap::SwapEndian4(void *pValue) 
{
	*((u32 *) (pValue)) = (((*((u32 *) (pValue)) & 0x000000ff) << 24) |
									 ((*((u32 *) (pValue)) & 0x0000ff00) << 8)  |	
									 ((*((u32 *) (pValue)) & 0x00ff0000) >> 8)  |	
									 ((*((u32 *) (pValue)) & 0xff000000) >> 24));
}

inline void DBLSwap::SwapEndian8(void *pValue)
{
	memcpy(m_bigEndian8Buff, pValue, 8);
	((u8 *) pValue)[0] = m_bigEndian8Buff[7];
	((u8 *) pValue)[1] = m_bigEndian8Buff[6];
	((u8 *) pValue)[2] = m_bigEndian8Buff[5];
	((u8 *) pValue)[3] = m_bigEndian8Buff[4];
	((u8 *) pValue)[4] = m_bigEndian8Buff[3];
	((u8 *) pValue)[5] = m_bigEndian8Buff[2];
	((u8 *) pValue)[6] = m_bigEndian8Buff[1];
	((u8 *) pValue)[7] = m_bigEndian8Buff[0];
}