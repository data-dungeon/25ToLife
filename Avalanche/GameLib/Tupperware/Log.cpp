////////////////////////////////////////////////////////////////////////////
//
// TupperwareLog
//
// A class for tupperware verbosity.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"
#include "Log.h"

// comment this line out to allow the log to happen
//#define USE_LOG


////////////////////////////////////////////////////////////////////////////

static TupperwareLog g_Log;

////////////////////////////////////////////////////////////////////////////

void TupperwareLog::Write( const char* format, ... ) {
#ifdef USE_LOG
#ifndef GCN
	va_list args;
	va_start( args, format );
	char* string = g_Log.GetFormattedString( format, args );
	va_end( args );

	if ( g_Log.m_WriteCB ) {
		g_Log.m_WriteCB( string );
	} else {
		FILE* f = fopen( TUPPERWARE_LOG_FILE_NAME, "a" );
		if ( f != NULL ) {
//			char time[ 9 ];
//yuch-- prevents linking with import export, temporarily commented out-- ned
//			_strtime( time );

//			fprintf( f, "[%s] %s", time, string );
			fprintf( f, "%s", string );
			fclose( f );
		}
	}
#endif
#endif
}


void TupperwareLog::OnWrite( TupperwareLogCB on_write ) {
#ifdef USE_LOG
#ifndef GCN
	g_Log.m_WriteCB = on_write;
#endif
#endif
}


TupperwareLog::TupperwareLog() {
#ifdef USE_LOG
#ifndef GCN
	m_BufferSize = 256;
	m_Buffer = new char[ m_BufferSize ];
	m_WriteCB = NULL;

	FILE* f = fopen( TUPPERWARE_LOG_FILE_NAME, "w" );
	if ( f != NULL ) {
//		char date[ 9 ];
//yuch-- prevents linking with import export, temporarily commented out-- ned
//		_strdate( date );
//		char time[ 9 ];
//yuch-- prevents linking with import export, temporarily commented out-- ned
//		_strtime( time );

		fprintf( f, "TUPPERWARE LOG\n" );
//		fprintf( f, "Created on %s at %s.\n\n", date, time );
		fclose( f );
	}
#endif
#endif
}


TupperwareLog::~TupperwareLog() {
#ifdef USE_LOG
#ifndef GCN
	delete [] m_Buffer;
#endif
#endif
}


char* TupperwareLog::GetFormattedString( const char* format, va_list args ) {
#ifdef USE_LOG
#ifndef GCN
	int num_chars;
	do {
		num_chars = vsprintf( m_Buffer, format, args );
		if ( num_chars == -1 ) {
			m_BufferSize *= 2;
			delete [] m_Buffer;
			m_Buffer = new char[ m_BufferSize ];
			assert( m_Buffer );
		}
	} while ( num_chars == -1 );
	
	return m_Buffer;
#else
	return( NULL);
#endif
#else
	return( NULL);
#endif
}

////////////////////////////////////////////////////////////////////////////
