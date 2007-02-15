//============================================================================
//=
//= Translator.cpp - Handle language localization (english, spanish, etc)
//=
//============================================================================

#include "GameHelper/GameHelperPCH.h"
#include "GameHelper/Translator.h"
#include "GameHelper/TranslatorDict.h"

//============================================================================

// Singleton
CTranslator g_translator;

//============================================================================

CTranslator::CTranslator()
{
	d_dict = NULL;
	d_language[0] = 0;
}

//============================================================================

CTranslator::~CTranslator()
{
	Reset();
}

//============================================================================

void CTranslator::Reset()
{
	if ( d_dict )
	{
		memFree( d_dict );
		d_dict = NULL;
	}
}

//============================================================================

bool CTranslator::LoadDictionary()
{
	// free old dictionary
	Reset();

	// load file
	char filename[ 64 ];
	sprintf( filename, "lang\\%s.bin", d_language );
	t_Error	error;
	s32 size;
	d_dict = (CTranslatorDict*) Media::Load( filename, &size, &error );
	if ( !d_dict )
	{
		ASSERTF( false, ( "missing language file '%s'", filename ) );
		return false;
	}

	if ( !d_dict->Init() )
	{
		ASSERTS( false, "Language translation file failed to load, bad header" );
		memFree( d_dict );
		d_dict = NULL;
		return false;
	}

	return true;
}

//============================================================================

const char* CTranslator::Translate( const char* text ) const
{
	if ( text && text[0] && d_dict )
	{
		char key[ 2048 ];
		int keylen = TransformKey( text, key );
		ASSERT( keylen < sizeof(key) );
		if ( keylen > 1 )
		{
			u32 hash = Hash::CalcChecksum( key, keylen, d_dict->d_seed );
			const char* translation = d_dict->Lookup( hash );
			if ( translation )
				return translation;
		}
	}
	return text;
}

//============================================================================

int CTranslator::TransformKey( const char* text, char* output )
{
	char* p = output;

	if ( text )
	{
		// use "unsigned char" so isalpha/isdigit/isspace work correctly.
		for ( unsigned char* s = (unsigned char*) text ; *s ; s++ )
		{
			if      ( isalpha( *s ) )              { *p++ = tolower( *s ); }
			else if ( isdigit( *s ) )              { *p++ = *s;  }
			else if ( isspace( *s ) || *s == '_' ) { *p++ = '_'; }

			// collapse multiple '_' into single.
			if ( p - 2 >= output && p[-1] == '_' && p[-2] == '_' ) { p--; }
		}
	}

	*p = 0;
	return p - output;
}

//============================================================================

void CTranslator::SetLanguage( const char* language )
{
	int i;
	for ( i = 0 ; i < sizeof(d_language) - 1 && language[i] && !isspace(language[i]) ; i++ )
		g_translator.d_language[i] = language[i];
	g_translator.d_language[i] = 0;
	// NOTE: I know this makes no sense, but when this function is called on the
	// g_translator object (e.g. "g_translator.SetLanguage()"), the "this" pointer
	// is bogus, and d_language appears to live at a bogus address.  Thus, the
	// function fails and writes to bad memory.  Scoping it with the global
	// object name directly serves as a temporary workaround.  Perhaps this
	// is a compiler problem, not sure.  -Squirrel
}

//============================================================================

char* CTranslator::GetLanguage()
{
	// hack, people care if we're english, but not uk_english...
	if ( stricmp( d_language, "uk_english" ) == 0 )
		return "english";
	else
		return d_language;
}

//============================================================================
