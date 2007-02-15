// ----------------------------------------------------------------------------
// File				-> helix.h
// Created			-> Oct 23, 2004
// Author			-> Dwain Skinner
// Description		-> Helix encryption interface
//
// Copyright (c) 2003 - 2004 by PJ Naughter
//

#ifndef _CHELIX_H
#define _CHELIX_H

typedef bool BooL;

#ifdef SN_TARGET_PS2
typedef unsigned char BYTE;
typedef unsigned int DWORD;
#define SetLastError( parm ) ((void)0)
#endif

#define DEFAULT_KEY "I'm a H4X0R b3c4us3 I l0ve c0ck"
#define MAC_SIZE_USED 0

class CHelixNonce
{
public:
	CHelixNonce										( );

	BYTE												m_Data[ 16 ];
};

// ----------------------------------------------------------------------------

class CHelixMAC
{
public:
	CHelixMAC										( );

	friend BooL operator ==						( const CHelixMAC& mac1, const CHelixMAC& mac2 );

	BYTE												m_Data[ 16 ];
};

// ----------------------------------------------------------------------------

class CHelix
{
public:
	CHelix											( );

	// --- interface
	BooL SetKey										( const BYTE* pbyKey, DWORD dwKeyLength );
	BooL Encrypt									( const BYTE* pbyPlainText, DWORD dwPlainTextSize, BYTE* pbyCipherText );
	BooL Decrypt									( const BYTE* pbyCipherText, DWORD dwCipherTextSize, BYTE* pbyPlainText );
	DWORD GetEncryptionSize						( const BYTE* pbyPlainText, DWORD dwPlainTextSize );
	DWORD GetEncryptionHeaderSize				( );

protected:
	inline void Block								( DWORD X0, DWORD P, DWORD X1 );
	void Start										( const CHelixNonce& nonce );
	void Finish										( DWORD lnm4, CHelixMAC& Mac );

	DWORD 											m_Z[ 5 ];
	DWORD 											m_K[ 8 ];
	DWORD 											m_X1[ 8 ];
	int   											m_dwKLen;
	int   											m_i8;

private:
	CHelixNonce										m_nonce;
	CHelixMAC										m_mac;
};



inline DWORD CHelix::GetEncryptionSize( const BYTE* pbyPlainText, DWORD dwPlainTextSize )
{
	return( dwPlainTextSize + MAC_SIZE_USED );
}

inline DWORD CHelix::GetEncryptionHeaderSize( )
{
	return( MAC_SIZE_USED );
}

extern CHelix g_helix;
#endif //_CHELIX_H
