// ----------------------------------------------------------------------------
// File			-> Helix.cpp
// Created		-> Oct 23, 2004
// Author		-> Dwain Skinner
// Description	-> Helix Encryption algorithm
//
// Copyright (c) 2003 - 2004 by PJ Naughter. All rights reserved
//

// ------------------------------- includes -----------------------------------


#ifdef WIN32
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#ifndef _XBOX
#include <windows.h>
#endif
#include <tchar.h>
#include <stdlib.h>
#endif

#include "Helix.h"

#ifndef ASSERTS
#define ASSERTS(exp, str) ((void)0)
#endif

// ------------------------------- statics ------------------------------------
DWORD Helix_Decrypt_Masks[] = { 0x0, 0xFF, 0xFFFF, 0xFFFFFF };

#ifdef SN_TARGET_PS2
inline DWORD _rotl( unsigned long bits, unsigned long rotCount )
{
	register unsigned long upperBits;

	asm __volatile__("
		dsllv		%0, %0, %2
		dsrl32	%1, %0, 0
		or			%0, %0, %1

		" : "+r" (bits), "+r" (upperBits) : "r" (rotCount) );

	return( ( DWORD )bits );
}
#else

//Ensure we generate intrinsics for "_rotl" calls
#pragma intrinsic(_rotl)
#endif

// --- our singleton
CHelix g_helix;


// ----------------------------------------------------------------------------

CHelixNonce::CHelixNonce()
{
	memset( &m_Data, 0, sizeof( m_Data ) );
}

// ----------------------------------------------------------------------------

CHelixMAC::CHelixMAC()
{
	memset( &m_Data, 0, sizeof( m_Data ) );
}

// ----------------------------------------------------------------------------

BooL operator==(const CHelixMAC& mac1, const CHelixMAC& mac2)
{
	BooL bSuccess = TRUE;
	for( int i = 0; ( i < MAC_SIZE_USED ) && bSuccess; i++ )
	{
		bSuccess = ( mac1.m_Data[ i ] == mac2.m_Data[ i ] );

	}
	return bSuccess;
}


CHelix::CHelix()
{
  memset(&m_Z, 0, sizeof(m_Z));
  memset(&m_K, 0, sizeof(m_K));
  memset(&m_X1, 0, sizeof(m_X1));
  m_dwKLen = 0;
  m_i8 = 0;

  //By default set the key to be an empty string
  SetKey( ( BYTE * )&DEFAULT_KEY, sizeof( DEFAULT_KEY ) - 1 );
}

BooL CHelix::SetKey(const BYTE* pbyKey, DWORD dwKeyLength)
{
  //Validate our parameters
  if ((pbyKey != NULL) && (dwKeyLength > 32))
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
  if ((pbyKey == NULL) && (dwKeyLength))
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  //Hive away in a member variable
  m_dwKLen = dwKeyLength;

  //Form the key (represented as a DWORD array internally)
  BYTE key[32];
  memset(&key, 0, sizeof(key));
  memcpy(&key, pbyKey, m_dwKLen);
  DWORD* pdwKey = (DWORD*) &key;
  int i;
  for (i=0; i<8; i++)
    m_K[i] = pdwKey[i];

  //Perform the key mixing
  for (i=0; i<8; i++)
  {
    m_Z[0] = m_K[0];
    m_Z[1] = m_K[1];
    m_Z[2] = m_K[2];
    m_Z[3] = m_K[3];
    m_Z[4] = m_dwKLen + 64;
    Block(0, 0, 0);
    DWORD K0 = m_K[0];
    DWORD K1 = m_K[1];
    DWORD K2 = m_K[2];
    DWORD K3 = m_K[3];
    m_K[0] = m_K[4] ^ m_Z[0];
    m_K[1] = m_K[5] ^ m_Z[1];
    m_K[2] = m_K[6] ^ m_Z[2];
    m_K[3] = m_K[7] ^ m_Z[3];
    m_K[4] = K0;
    m_K[5] = K1;
    m_K[6] = K2;
    m_K[7] = K3;
  }

  return TRUE;
}

void CHelix::Start(const CHelixNonce& nonce)
{
  //convert nonce to a array of 4 DWORD's
  DWORD N1[8];
  DWORD* pdwN1 = (DWORD*) &nonce.m_Data;
  int i;
  for (i=0; i<4; i++)
    N1[i] = pdwN1[i];

  //Extend the nonce to an array of 8 DWORD's
  for (i=0; i<4; i++)
    N1[i+4] = (i - N1[i]);

  //Make array of X1 values
  for (i=0; i<8; i++)
    m_X1[i] = m_K[(i+4)%8] + N1[i] + (((i%4)==1)*4*m_dwKLen);

  //Initialize state and run first 8 blocks for nonce mixing.
  m_Z[0] = m_K[3] ^ N1[0];
  m_Z[1] = m_K[4] ^ N1[1];
  m_Z[2] = m_K[5] ^ N1[2];
  m_Z[3] = m_K[6] ^ N1[3];
  m_Z[4] = m_K[7];
  m_i8 = 0;
  int locali;
  for (i=0; i<8; i++)
  {
    locali = m_i8 & 0x7;
    Block(m_K[locali], 0, m_X1[locali] + m_i8);
    ++m_i8;
  }
}

void CHelix::Finish(DWORD lnm4, CHelixMAC& Mac)
{
  //Tweak the internal state as specified
  m_Z[0] ^= 0x912d94f1;

  int locali;
  //Apply 8 block function with len(P) mod 4 as plaintext
  int i;
  for (i=0; i<8; i++)
  {
    locali = m_i8 & 0x7;
    Block(m_K[locali], lnm4, m_X1[locali] + m_i8);
    ++m_i8;
  }

  //And generate the tag
  DWORD* pdwMac = (DWORD*) &Mac.m_Data;
  for (i=0; i<3; i++)
  {
    pdwMac[i] = m_Z[0];
    locali = m_i8 & 0x7;
    Block(m_K[locali], lnm4, m_X1[locali] + m_i8);
    ++m_i8;
  }
  pdwMac[3] = m_Z[0];

  //Cleanup
  memset(&m_Z, 0, sizeof(m_Z));
  memset(&m_X1, 0, sizeof(m_X1));
  m_i8 = 0;
}

void CHelix::Block(DWORD X0, DWORD P, DWORD X1)
{
  //Prod the compiler to use the registers by using local variables with the "register" keyword
  register DWORD Z0 = m_Z[0];
  register DWORD Z1 = m_Z[1];
  register DWORD Z2 = m_Z[2];
  register DWORD Z3 = m_Z[3];
  register DWORD Z4 = m_Z[4];

  Z0 += Z3; 
  Z3 = _rotl(Z3, 15);
  Z1 += Z4; 
  Z4 = _rotl(Z4, 25);
  Z2 ^= Z0; 
  Z0 = _rotl(Z0, 9);
  Z3 ^= Z1; 
  Z1 = _rotl(Z1, 10);
  Z4 += Z2; 
  Z2 = _rotl(Z2, 17);

  Z0 ^= (Z3 + X0); 
  Z3 = _rotl(Z3, 30);
  Z1 ^= Z4; 
  Z4 = _rotl(Z4, 13);
  Z2 += Z0; 
  Z0 = _rotl(Z0, 20);        
  Z3 += Z1; 
  Z1 = _rotl(Z1, 11);
  Z4 ^= Z2; 
  Z2 = _rotl(Z2, 5);
  
  Z0 += (Z3 ^ P); 
  Z3 = _rotl(Z3, 15);
  Z1 += Z4; 
  Z4 = _rotl(Z4, 25);
  Z2 ^= Z0; 
  Z0 = _rotl(Z0, 9);
  Z3 ^= Z1; 
  Z1 = _rotl(Z1, 10);
  Z4 += Z2; 
  Z2 = _rotl(Z2, 17);
  
  Z0 ^= (Z3 + X1); 
  Z3 = _rotl(Z3, 30);
  Z1 ^= Z4; 
  Z4 = _rotl(Z4, 13);
  Z2 += Z0; 
  Z0 = _rotl(Z0, 20);
  Z3 += Z1; 
  Z1 = _rotl(Z1, 11);
  Z4 ^= Z2; 
  Z2 = _rotl(Z2, 5); 

  //Move the local variable values back to the member variables
  m_Z[0] = Z0;
  m_Z[1] = Z1;
  m_Z[2] = Z2;
  m_Z[3] = Z3;
  m_Z[4] = Z4;
}

BooL CHelix::Encrypt(const BYTE* pbyPlainText, DWORD dwPlainTextSize, BYTE* pbyCipherText )
{
	//Validate our parameters
	if ((pbyPlainText == NULL) || (dwPlainTextSize == 0) || (pbyCipherText == NULL))
	{
		ASSERTS( false, "invalid parameters" );
		return FALSE;
	}

	DWORD dwLoops = dwPlainTextSize / 4;
	DWORD dwExtraBytes = (dwPlainTextSize % 4);
	DWORD* pdwPlainText = (DWORD*) pbyPlainText;  
	DWORD* pdwCipherText = (DWORD*) pbyCipherText;

	//Encrypt the plaintext "pByPlainText" with the nonce "nonce".
	//Returns a ciphertext "pByCipherText" and the authentication tag "Mac"
	Start( m_nonce );
	DWORD dwPlainText;
	int locali;
	DWORD i;
	for (i=0; i<dwLoops; i++)
	{
		dwPlainText = pdwPlainText[i];
		pdwCipherText[i] = dwPlainText ^ m_Z[0];
		locali = m_i8 & 0x7;
		Block(m_K[locali], dwPlainText, m_X1[locali] + m_i8);
		++m_i8;
	}
	if (dwExtraBytes)
	{
		dwPlainText = 0;
		BYTE* pbyTempPlainText = (BYTE*) &dwPlainText;
		BYTE* pbyExtraPlainText = (BYTE*) &pdwPlainText[dwLoops];

		for (i=0; i<dwExtraBytes; i++)
			pbyTempPlainText[i] = pbyExtraPlainText[i];

		DWORD dwTempCipherText = dwPlainText ^ m_Z[0];
		locali = m_i8 & 0x7;
		Block(m_K[locali], dwPlainText, m_X1[locali] + m_i8);
		++m_i8;

		BYTE* pbyTempCipherText = (BYTE*) &dwTempCipherText;
		BYTE* pbyExtraCipherText = (BYTE*) &pdwCipherText[dwLoops];
		for (i=0; i<dwExtraBytes; i++)
			pbyExtraCipherText[i] = pbyTempCipherText[i];
	}

	//Compute the authentication tag
	Finish( dwPlainTextSize%4, m_mac );
	
	// --- tack on the authentication code
	for( int i = 0; i < MAC_SIZE_USED; i++ )
	{
		pbyCipherText[ dwPlainTextSize + i ] = m_mac.m_Data[ i ];
	}
	return TRUE;
}

BooL CHelix::Decrypt( const BYTE* pbyCipherText, DWORD dwCipherTextSize, BYTE* pbyPlainText )
{
	//Validate our parameters
	if ((pbyCipherText == NULL) || (dwCipherTextSize == 0) || (pbyPlainText == NULL))
	{
		ASSERTS( false, "invalid parameters" );
		return FALSE;
	}

	// --- adjust the size of the encrypted data to not include the authentication code
	dwCipherTextSize -= MAC_SIZE_USED;

	DWORD dwLoops = dwCipherTextSize / 4;
	DWORD dwExtraBytes = (dwCipherTextSize % 4);
	DWORD* pdwPlainText = (DWORD*) pbyPlainText;  
	DWORD* pdwCipherText = (DWORD*) pbyCipherText;

	//Decrypt the plaintext "pByCipherText" with the nonce "nonce".
	//Returns FALSE if the Authentication fails, or TRUE if authentication succeeds and the plaintext is returned in "pbyPlainText"
	Start( m_nonce );
	int locali;
	DWORD i;
	for (i=0; i<dwLoops; i++)
	{
		pdwPlainText[i] = pdwCipherText[i] ^ m_Z[0];
		locali = m_i8 & 0x7;
		Block(m_K[locali], pdwPlainText[i], m_X1[locali] + m_i8);
		++m_i8;
	}

	if (dwExtraBytes)
	{
		DWORD dwTempCipherText = 0;
		BYTE* pbyTempCipherText = (BYTE*) &dwTempCipherText;
		BYTE* pbyExtraCipherText = (BYTE*) &pdwCipherText[dwLoops];

		for (i=0; i<dwExtraBytes; i++)
			pbyTempCipherText[i] = pbyExtraCipherText[i];

		DWORD dwTempPlainText = (dwTempCipherText ^ m_Z[0]) & Helix_Decrypt_Masks[dwExtraBytes];
		locali = m_i8 & 0x7;
		Block(m_K[locali], dwTempPlainText, m_X1[locali] + m_i8);
		++m_i8;

		BYTE* pbyTempPlainText = (BYTE*) &dwTempPlainText;
		BYTE* pbyExtraPlainText = (BYTE*) &pdwPlainText[dwLoops];
		for (i=0; i<dwExtraBytes; i++)
			pbyExtraPlainText[i] = pbyTempPlainText[i];
	}


	//Compute the authentication tag
	CHelixMAC CalculatedMac;
	Finish( dwCipherTextSize % 4, CalculatedMac );

	//And check to see if it is the same as that passed to us
	return( CalculatedMac == *( ( CHelixMAC * )( pbyCipherText + dwCipherTextSize ) ) );
}
