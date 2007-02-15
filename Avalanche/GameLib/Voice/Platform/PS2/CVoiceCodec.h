// ---------------------------------------------------------------------------
// File			-> CVoiceCodec.h
// Author		-> Dwain Skinner
// Created		-> December 15, 2003
// Description	-> Platform independent codec interface class
//
//  Copyright (c) 2003, Avalanche Software Inc.
//  All Rights Reserved.
// ---------------------------------------------------------------------------


class CVoiceCodec
{

public:
	CVoiceCodec													( );
	virtual ~CVoiceCodec										( );


	virtual void				create						( );
	virtual int					encode						( unsigned char *pSrc, unsigned char *pDst, int size );
	virtual int					decode						( unsigned char *pDst, unsigned char *pSrc, int encodedSize, int size );
	virtual void				destroy						( );
	virtual void				info							( );
	virtual void				setCodecIndex				( int i );

	const int					getCodecIndex				( )								{ return( m_codecIndex ); }
	const char					*getCodecName				( )								{ return( m_pCodecName ); }
	
// --- public enums

	// --- public defines
	#define MAX_CODEC_NAME		32

protected:
	void							setName						( char *pName );

private:
	void							openCodec( );

	int							m_codecIndex;
	char							m_pCodecName[ MAX_CODEC_NAME ];
	
};
