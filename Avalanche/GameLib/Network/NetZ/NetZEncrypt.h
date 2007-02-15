// ----------------------------------------------------------------------------
// File				-> NetZEncrypt.h
// Created			-> Oct 25, 2004
// Author			-> Dwain Skinner
// Description		-> Encryption/Decryption interface for NetZ
// ----------------------------------------------------------------------------


#ifndef _NETZENCRYPTION_H
#define _NETZENCRYPTION_H

#include <plugins/encryption/interface/encryptiondecl.h>
#include <plugins/encryption/interface/encryptionalgorithm.h>
#include <platform/core/criticalsection.h>
#include <platform/core/type.h>

USING_NAMESPACE_QUAZAL;


class CNetZEncryption: public EncryptionAlgorithm 
{
public:
	CNetZEncryption								( );
	virtual ~CNetZEncryption					( );

//	virtual void KeyHasChanged					( );


	virtual qUnsignedInt32 Encrypt			( const Buffer &oInBuffer, Buffer *pOutBuffer );
//	virtual qUnsignedInt32 Encrypt(Buffer *pInOutBuffer);

	virtual qUnsignedInt32 Decrypt			( const Buffer &oInBuffer, Buffer *pOutBuffer );
//	virtual qUnsignedInt32 Decrypt(Buffer *pInOutBuffer);

//	void Encrypt(qByte* pInData, qUnsignedInt32 uiInSize);
//	void Decrypt(qByte* pInData, qUnsignedInt32 uiInSize);

//	//{secret}
//	static Key GetDefaultKey();

private:
};

#endif

