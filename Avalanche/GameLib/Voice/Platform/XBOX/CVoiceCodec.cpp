// ---------------------------------------------------------------------------
// File			-> CVoiceCodec.h
// Author		-> Dwain Skinner
// Created		-> December 15, 2003
// Description	-> Platform independent codec interface class implementation
//
//  Copyright (c) 2003, Avalanche Software Inc.
//  All Rights Reserved.
// ---------------------------------------------------------------------------

// Precompiled header
#include "Voice/VoicePCH.h"

// --- Local includes
#include "Voice/CodecLib/interface/liblgcodec.h"

// lgcodec globals
int          g_lgcodec = 0;
lgCodecDesc* g_lgcodec_desc = NULL;

// ---------------------------------------------------------------------------
CVoiceCodec::CVoiceCodec( )
{
}

// ---------------------------------------------------------------------------
CVoiceCodec::~CVoiceCodec( )
{
}

// ---------------------------------------------------------------------------
void CVoiceCodec::create( )
{
	lgCodecInit( );
	setCodecIndex( 0 );
}

// ---------------------------------------------------------------------------
int CVoiceCodec::encode( unsigned char *pSrc, unsigned char *pDst, int size )
{
	int encoded_bytes = 2048*8;

	// --- encode
	lgCodecEncode( g_lgcodec, ( short * )pSrc, size,( u_char * )pDst, &encoded_bytes );
	return( encoded_bytes );
}

// ---------------------------------------------------------------------------
int CVoiceCodec::decode( unsigned char *pDst, unsigned char *pSrc, int encodedSize, int size )
{
	int decoded_bytes = 2048*8;

	// --- decode
	lgCodecDecode( g_lgcodec, ( const u_char * )pSrc, encodedSize, ( short * )pDst, &decoded_bytes );
	return( decoded_bytes );
}

// ---------------------------------------------------------------------------
void CVoiceCodec::destroy( )
{
}

// ---------------------------------------------------------------------------
void CVoiceCodec::info( )
{
}

// ---------------------------------------------------------------------------
void CVoiceCodec::setCodecIndex( int newCodec )
{
	m_codecIndex = newCodec;
	openCodec( );
}

// ---------------------------------------------------------------------------
void CVoiceCodec::openCodec( )
{
	if ( g_lgcodec )
	{
		lgCodecClose( g_lgcodec );
		g_lgcodec = 0;
	}

	do
	{
		g_lgcodec_desc = lgCodecEnumerate( m_codecIndex );

		if ( g_lgcodec_desc == NULL )
			m_codecIndex = 0;
	}
	while ( !g_lgcodec_desc );


	lgCodecOpen( g_lgcodec_desc->id, &g_lgcodec );

	printf("\n\nOpening codec %s (%d bps)\n\n", g_lgcodec_desc->name, g_lgcodec_desc->bits_per_second);
}
