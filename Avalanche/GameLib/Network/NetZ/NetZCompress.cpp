// ----------------------------------------------------------------------------
// File			-> NetZCompress.cpp
// Created		-> 9/28/2004
// Author		-> Dwain Skinner
// Description	-> Net-Z to LZO compression interface
// ----------------------------------------------------------------------------

// Precompiled header
#include "Network/NetworkPCH.h"

// System includes
#if defined (WIN32) && !defined (_XBOX)
#include <windows.h>
#endif
#if defined (PS2)
#include <malloc.h>
#endif

// Engine includes
#include "Layers/Debug.h"
#include "EngineHelper/timer.h"

// Profiling
#include "Layers/Timing.h"

// my header file
#include "Network/NetZ/NetZCompress.h"
#include "plugins/core/buffer.h"

// --- lzo include file
#include "GameHelper/compression/LZO/minilzo.h"

// --- static vars
static u8 lzoInitialized = false;
static u8 wrkmem[ 16384 * 4 ];


// ----------------------------------------------------------------------------

CNetZCompress::CNetZCompress( )
{
	if( lzoInitialized ) return;
	if( lzo_init() != LZO_E_OK )
	{
		ASSERTS( false, "failed to initialize LZO compression engine" );
		return;
	}
	lzoInitialized = true;
	return;
}

// ----------------------------------------------------------------------------

CNetZCompress::~CNetZCompress( )
{
}

// ----------------------------------------------------------------------------

void CNetZCompress::CompressImpl(const Buffer &oInBuffer, Buffer *pOutBuffer)
{
	u32 outLen = 0;

#if OUTPUT_STATS
	u32 start = g_timer.GetOSTime();
#endif
	int r = lzo1x_1_compress( oInBuffer.GetContentPtr(), oInBuffer.GetContentSize(), pOutBuffer->GetContentPtr()+2, (lzo_uint * )&outLen, wrkmem );
#if OUTPUT_STATS
	u32 end = g_timer.GetOSTime();
#endif

	ASSERTS( outLen+2 < pOutBuffer->GetSize(), "NetZ compression buffer overflow" );
	if (r == LZO_E_OK)
	{
		if( outLen < oInBuffer.GetContentSize() )
		{
			pOutBuffer->SetContentSize( outLen + 2 );
#ifdef OUTPUT_STATS
			g_console.Echo("compress %d bytes down to %d bytes in %d MS\n", oInBuffer.GetContentSize(), outLen, (end-start) );
#endif
			*( ( unsigned short * )pOutBuffer->GetContentPtr() ) = ( unsigned short )oInBuffer.GetContentSize();
		}
		else
		{
			memcpy( pOutBuffer->GetContentPtr() + 2, oInBuffer.GetContentPtr(), oInBuffer.GetContentSize() );
			pOutBuffer->SetContentSize( oInBuffer.GetContentSize() + 2 );
			*( ( unsigned short * )pOutBuffer->GetContentPtr() ) = 0;
		}
	}
	else
	{
		ASSERTS( false, "internal LZO error" );
	}
}

// ----------------------------------------------------------------------------

void CNetZCompress::DecompressImpl(const Buffer &oInBuffer, Buffer *pOutBuffer)
{
	u32 uncompLen = *( ( unsigned short * ) oInBuffer.GetContentPtr() );

	if( uncompLen )
	{
		// --- resize buffer if our output size is larger
		if( uncompLen > pOutBuffer->GetSize() )
			pOutBuffer->Resize( uncompLen );

		int r = lzo1x_decompress( oInBuffer.GetContentPtr() + 2, oInBuffer.GetContentSize() - 2, pOutBuffer->GetContentPtr(), (lzo_uint * )&uncompLen, NULL );
		if( r == LZO_E_OK  )
		{
			pOutBuffer->SetContentSize( uncompLen );
		}
		else
		{
			ASSERTS( false, "internal LZO error" );
		}
	}
	else
	{
		memcpy( pOutBuffer->GetContentPtr(), oInBuffer.GetContentPtr() + 2, oInBuffer.GetContentSize() - 2 );
		pOutBuffer->SetContentSize( oInBuffer.GetContentSize() - 2 );
	}
}