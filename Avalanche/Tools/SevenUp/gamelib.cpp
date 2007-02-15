#include "pch.h"
#include "direct3d.h"

#include <Layers/Assert/Assert.h>
#include <Display/Common/DisplayState.h>
#include "GameHelper/Translator.h"

CTranslator g_translator;

bool Assert::CallHandler(const char *expr, const char *file, int line, const char* string) { return false;}
const char* Assert::FormatString( const char* format, ... ) { return 0; }
void memSetOneShotNewName( char *pcName ) {}
ts_DisplayState DisplayState;
void LoadingScreen_Render( void ) {}

//=============================================================================
// Translator stubs...
//=============================================================================

CTranslator::CTranslator( void )
{
}

CTranslator::~CTranslator( void )
{
}

const char* CTranslator::Translate( const char* pWord ) const
{
	return pWord;
}