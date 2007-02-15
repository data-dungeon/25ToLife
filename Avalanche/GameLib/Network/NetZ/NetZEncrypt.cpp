// ----------------------------------------------------------------------------
// File				-> NetZEncrypt.h
// Created			-> Oct 25, 2004
// Author			-> Dwain Skinner
// Description		-> Encryption/Decryption interface for NetZ
// ----------------------------------------------------------------------------
// Precompiled header
#include "Network/NetworkPCH.h"


// Engine includes
#include "Layers/Debug.h"
#include "EngineHelper/timer.h"

// Profiling
#include "Layers/Timing.h"

// my header file
#include "Network/NetZ/NetZEncrypt.h"
#include "plugins/core/buffer.h"
#include "plugins/encryption/interface/encryptionerror.h"

// --- helix encryption
#include "GameHelper/encryption/Helix/Helix.h"

CHelixNonce nonce;
CHelixMAC mac;

// ---------------------------------------------------------------------------

CNetZEncryption::CNetZEncryption( )
{
}

CNetZEncryption::~CNetZEncryption( )
{
}


qUnsignedInt32 CNetZEncryption::Encrypt( const Buffer &oInBuffer, Buffer *pOutBuffer )
{
	int headerSize = g_helix.GetEncryptionHeaderSize( );
	qUnsignedInt32 outSize = headerSize + oInBuffer.GetContentSize( );

	// --- resize buffer if our output size is larger
	if( outSize > pOutBuffer->GetSize() )
		pOutBuffer->Resize( outSize );

	if( g_helix.Encrypt( oInBuffer.GetContentPtr( ), oInBuffer.GetContentSize( ), pOutBuffer->GetContentPtr( ) ) )
	{
		pOutBuffer->SetContentSize( outSize );
	}
	else
	{
		ASSERTS( false, "Fatal Error - Failed to encrypt message" );
		pOutBuffer->SetContentSize( outSize );
		return( ENCRYPTION_ERROR );
	}
	return( ENCRYPTION_SUCCESS );
}

qUnsignedInt32 CNetZEncryption::Decrypt( const Buffer &oInBuffer, Buffer *pOutBuffer )
{
	int headerSize = g_helix.GetEncryptionHeaderSize( );
	int outSize = oInBuffer.GetContentSize( ) - headerSize;

	if( g_helix.Decrypt( oInBuffer.GetContentPtr( ), oInBuffer.GetContentSize( ), pOutBuffer->GetContentPtr( ) ) )
	{
		pOutBuffer->SetContentSize( outSize );
	}
	else
	{
		ASSERTS( false, "Fatal Error - Failed to decrypt message" );
		pOutBuffer->SetContentSize( outSize );
		return( ENCRYPTION_ERROR );
	}
	return( ENCRYPTION_SUCCESS );
}

