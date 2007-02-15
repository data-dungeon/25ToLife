////////////////////////////////////////////////////////////////////////////
//
// TupperwareReader
//
// A reader of tupperware files.
//
////////////////////////////////////////////////////////////////////////////

// TupperwareParser
inline void TupperwareParser::ResetTokenStr() 
{
	m_pTokenStr[0] = 0;
	m_nTokenStrPos = 0;
}

inline void TupperwareParser::AddBlockToTokenStr(const char *pBlock, int len) 
{
	if (m_nTokenStrPos + len + 1 >= m_nTokenStrLen) 
	{
		int new_len = m_nTokenStrLen * 4;
		while (new_len < m_nTokenStrPos + len) 
		{
			new_len *= 4;
		}

		char *new_str = new char[new_len];
		memcpy(new_str, m_pTokenStr, m_nTokenStrPos);
		m_nTokenStrLen = new_len;
		delete[] m_pTokenStr;
		m_pTokenStr = new_str;
	}

	memcpy(&m_pTokenStr[m_nTokenStrPos], pBlock, len);
	m_nTokenStrPos += len;
	m_pTokenStr[m_nTokenStrPos] = 0;
}


inline void TupperwareParser::AddToTokenStr(char c) 
{
	AddBlockToTokenStr(&c, 1);
}


inline int TupperwareParser::NextBlock(char *block, int len) 
{
	if (m_nBufferPos != m_nBufferLength)
	{
		if (m_nBufferLength - m_nBufferPos > len) 
		{
			memcpy(block, &m_pBuffer[m_nBufferPos], len);
#ifdef TRACK_LINE_NUMBERS
			while(len)
			{
				char data = m_pBuffer[m_nBufferPos];
				if (data == '\r')
				{
					// if lone carriage return
					if (m_nBufferPos+1!=m_nBufferLength && m_pBuffer[m_nBufferPos+1]!='\n')
					{
						m_nBufferLineNum++;
						m_nBufferCharPos=0;
					}
					else
					{
						m_nBufferCharPos++;
					}
				}
				else if (data == '\n')
				{
					m_nBufferLineNum++;
					m_nBufferCharPos=0;
				}
				else
				{
					m_nBufferCharPos++;
				}
				m_nBufferPos++;
				len--;
			}
#else
			m_nBufferPos += len;
#endif
			return len;
		} 
		else 
		{
			int ret = m_nBufferLength - m_nBufferPos;
			memcpy(block, &m_pBuffer[m_nBufferPos], ret);
#ifdef TRACK_LINE_NUMBERS
			len = ret;
			while(len)
			{
				char data = m_pBuffer[m_nBufferPos];
				if (data == '\r')
				{
					// if lone carriage return
					if (m_nBufferPos+1!=m_nBufferLength && m_pBuffer[m_nBufferPos+1]!='\n')
					{
						m_nBufferLineNum++;
						m_nBufferCharPos=0;
					}
					else
					{
						m_nBufferCharPos++;
					}
				}
				else if (data == '\n')
				{
					m_nBufferLineNum++;
					m_nBufferCharPos=0;
				}
				else
				{
					m_nBufferCharPos++;
				}
				m_nBufferPos++;
				len--;
			}
#else
			m_nBufferPos = m_nBufferLength;
#endif
			return ret;
		}
	}
	else 
	{
		return 0;
	}
}

inline char TupperwareParser::NextChar() 
{
	if (m_nBufferPos != m_nBufferLength)
	{
#ifdef TRACK_LINE_NUMBERS
		char data = m_pBuffer[m_nBufferPos];
		if (data == '\r')
		{
			// if lone carriage return
			if (m_nBufferPos+1!=m_nBufferLength && m_pBuffer[m_nBufferPos+1]!='\n')
			{
				m_nBufferLineNum++;
				m_nBufferCharPos=0;
			}
			else
			{
				m_nBufferCharPos++;
			}
		}
		else if (data == '\n')
		{
			m_nBufferLineNum++;
			m_nBufferCharPos=0;
		}
		else
		{
			m_nBufferCharPos++;
		}
		m_nBufferPos++;
		return data;
#else
		return m_pBuffer[m_nBufferPos++];
#endif
	}
	else
	{
		return NextBufferNextChar();
	}
}

inline void TupperwareParser::AdjustBlockBack(int len) 
{
	assert(m_nBufferPos - len >= 0);
#ifdef TRACK_LINE_NUMBERS
	while(len)
	{
		m_nBufferPos--;
		len--;
		char data = m_pBuffer[m_nBufferPos];
		if (data == '\r')
		{
			// if lone carriage return
			if (m_nBufferPos+1!=m_nBufferLength && m_pBuffer[m_nBufferPos+1]!='\n')
			{
				m_nBufferLineNum++;
			}
		}
		else if (data == '\n')
		{
			m_nBufferLineNum++;
		}
	}
#else
	m_nBufferPos -= len;
#endif
}
